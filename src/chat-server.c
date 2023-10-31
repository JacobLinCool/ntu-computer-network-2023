#include "chat-server.h"

typedef struct ChatClientThreadArg {
    ChatClient *client;
    ChatServer *server;
} ChatClientThreadArg;

void parse_chat_config_from_env(ChatServerConfig *config) {
    char *max_clients = getenv("CHAT_SERVER_MAX_CLIENTS");
    if (max_clients != NULL) {
        config->max_clients = atol(max_clients);
    }

    char *message_buffer_size = getenv("CHAT_SERVER_MESSAGE_BUFFER_SIZE");
    if (message_buffer_size != NULL) {
        config->message_buffer_size = atol(message_buffer_size);
    }

    char *max_name_length = getenv("CHAT_SERVER_MAX_NAME_LENGTH");
    if (max_name_length != NULL) {
        config->max_name_length = atol(max_name_length);
    }

    char *server_port = getenv("CHAT_SERVER_PORT");
    if (server_port != NULL) {
        config->server_port = atoi(server_port);
    }
}

ChatClient *add_chat_client(ChatServer *server, int socket) {
    pthread_mutex_lock(&server->server_mutex);

    server->clients = realloc(
        server->clients, (server->client_count + 1) * sizeof(ChatClient *));
    server->clients[server->client_count] = malloc(sizeof(ChatClient));
    server->clients[server->client_count]->socket = socket;
    char *name = malloc(server->config.max_name_length);
    sprintf(name, "Client #%03zu", server->client_increment++);
    server->clients[server->client_count]->name = name;
    server->client_count++;

    pthread_mutex_unlock(&server->server_mutex);

    return server->clients[server->client_count - 1];
}

void remove_chat_client(ChatServer *server, ChatClient *client) {
    pthread_mutex_lock(&server->server_mutex);

    close(client->socket);
    for (size_t i = 0; i < server->client_count; i++) {
        if (server->clients[i]->socket == client->socket) {
            free(server->clients[i]->name);
            free(server->clients[i]);
            server->clients[i] = server->clients[server->client_count - 1];
            server->client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&server->server_mutex);
}

void init_chat_server(ChatServer *server, ChatServerConfig config) {
    server->clients = NULL;
    server->client_count = 0;
    server->client_increment = 0;
    pthread_mutex_init(&server->server_mutex, NULL);
    server->config = config;

    server->socket = socket(AF_INET, SOCK_STREAM, 0);
    server->address = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(config.server_port),
        .sin_addr.s_addr = INADDR_ANY,
    };
}

void *handle_chat_client(void *arg);

void *_start_chat_server(void *arg);

/**
 * Starts the chat server.
 *
 * @param server The server to start.
 * @param thread If `NULL`, the server will run on and block the current thread.
 *               Otherwise, the server will run on the provided thread.
 * @return 0 on success, non-zero on failure.
 */
int start_chat_server(ChatServer *server, pthread_t *thread) {
    bind(server->socket, (struct sockaddr *)&server->address,
         sizeof(server->address));
    int ret = listen(server->socket, 64);
    if (ret) {
        return ret;
    }

    if (thread != NULL) {
        pthread_create(thread, NULL, _start_chat_server, server);
    } else {
        _start_chat_server(server);
    }

    return 0;
}

void *_start_chat_server(void *arg) {
    ChatServer *server = (ChatServer *)arg;

    while (true) {
        struct sockaddr_in client_address;
        socklen_t          client_len = sizeof(client_address);
        int                client_socket = accept(
                           server->socket, (struct sockaddr *)&client_address, &client_len);

        ChatClient *client = add_chat_client(server, client_socket);

        pthread_t            thread;
        ChatClientThreadArg *arg = malloc(sizeof(ChatClientThreadArg));
        arg->client = client;
        arg->server = server;

        pthread_create(&thread, NULL, handle_chat_client, arg);
        pthread_detach(thread);
    }

    return NULL;
}

int stop_chat_server(ChatServer *server) {
    return shutdown(server->socket, SHUT_RDWR);
}

void destroy_chat_server(ChatServer *server) {
    for (size_t i = 0; i < server->client_count; i++) {
        free(server->clients[i]->name);
        free(server->clients[i]);
    }
    free(server->clients);
    pthread_mutex_destroy(&server->server_mutex);
    close(server->socket);
    free(server);
}

void *handle_chat_client(void *_arg) {
    ChatClientThreadArg *arg = (ChatClientThreadArg *)_arg;
    ChatServer          *server = arg->server;
    ChatClient          *client = arg->client;
    free(arg);

    char buffer[server->config.message_buffer_size + 1];
    memset(buffer, 0, server->config.message_buffer_size + 1);

    while (true) {
        ssize_t bytes_read =
            recv(client->socket, buffer, server->config.message_buffer_size, 0);
        if (bytes_read <= 0) {
            remove_chat_client(server, client);
            pthread_exit(NULL);
        }

        buffer[bytes_read] = '\0';

        // change name
        if (strncmp(buffer, "/rename ", 8) == 0) {
            char *new_name = buffer + 8;
            strtok(new_name, "\n");
            size_t new_name_length = strlen(new_name);

            if (new_name_length > server->config.max_name_length) {
                char *error_message = "Server: Name too long.";
                send(client->socket, error_message, strlen(error_message), 0);
                continue;
            }

            char *old_name = client->name;
            client->name = malloc(new_name_length + 1);
            strcpy(client->name, new_name);
            free(old_name);

            char *success_message = "Server: Name changed.";
            send(client->socket, success_message, strlen(success_message), 0);
        } else {
            char *broadcast = malloc(server->config.message_buffer_size +
                                     server->config.max_name_length + 3);
            sprintf(broadcast, "%s: %s", client->name, buffer);
            size_t broadcast_length = strlen(broadcast);

            fprintf(stderr, "Broadcasting: %s\n", broadcast);

            pthread_mutex_lock(&server->server_mutex);

            for (size_t i = 0; i < server->client_count; i++) {
                send(server->clients[i]->socket, broadcast, broadcast_length,
                     0);
            }

            pthread_mutex_unlock(&server->server_mutex);

            free(broadcast);
        }
    }

    return NULL;
}
