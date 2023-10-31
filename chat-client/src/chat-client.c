#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_MESSAGE_SIZE 1024
#define MAX_NAME_LENGTH 50

void  rename_user(int socket_fd, char *username);
void *receive_message(void *arg);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ./client <server_ip> <server_port>\n");
        return EXIT_FAILURE;
    }

    int   server_port = atoi(argv[2]);
    char *server_ip = argv[1];

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Failed to create socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(server_port),
        .sin_addr.s_addr = inet_addr(server_ip)};

    if (connect(socket_fd, (struct sockaddr *)&server_address,
                sizeof(server_address)) == -1) {
        perror("Connection failed");
        return EXIT_FAILURE;
    }

    char username[MAX_NAME_LENGTH];
    printf("Enter your username: ");
    fgets(username, MAX_NAME_LENGTH, stdin);
    strtok(username, "\n");

    rename_user(socket_fd, username);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_message, &socket_fd);

    char message[MAX_MESSAGE_SIZE];
    while (true) {
        fgets(message, MAX_MESSAGE_SIZE, stdin);
        strtok(message, "\n");
        send(socket_fd, message, strlen(message), 0);
    }

    pthread_join(recv_thread, NULL);
    close(socket_fd);

    return EXIT_SUCCESS;
}

void rename_user(int socket_fd, char *username) {
    char to_send[MAX_MESSAGE_SIZE + MAX_NAME_LENGTH + 3];
    sprintf(to_send, "%s %s", "/rename", username);
    send(socket_fd, to_send, strlen(to_send), 0);
}

void *receive_message(void *arg) {
    int  socket_fd = *((int *)arg);
    char buffer[MAX_MESSAGE_SIZE];

    while (true) {
        ssize_t bytes_read = recv(socket_fd, buffer, MAX_MESSAGE_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Connection closed.\n");
            exit(EXIT_SUCCESS);
        }

        buffer[bytes_read] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}
