#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct ChatClient {
    char *name;
    int   socket;
} ChatClient;

typedef struct ChatServerConfig {
    size_t    max_name_length;
    size_t    max_clients;
    size_t    message_buffer_size;
    in_port_t server_port;
} ChatServerConfig;

typedef struct ChatServer {
    ChatClient       **clients;
    size_t             client_count;
    size_t             client_increment;
    pthread_mutex_t    server_mutex;
    ChatServerConfig   config;
    int                socket;
    struct sockaddr_in address;
} ChatServer;

void parse_chat_config_from_env(ChatServerConfig *config);

ChatClient *add_chat_client(ChatServer *server, int socket);
void        remove_chat_client(ChatServer *server, ChatClient *client);

void init_chat_server(ChatServer *server, ChatServerConfig config);
int  start_chat_server(ChatServer *server, pthread_t *thread);
int  stop_chat_server(ChatServer *server);
void destroy_chat_server(ChatServer *server);
