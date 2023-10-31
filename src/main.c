#include <signal.h>
#include <stdlib.h>

#include "chat-server.h"
#include "http-server.h"
#include "middlewares/home-page.h"
#include "middlewares/logger.h"
#include "middlewares/not-found.h"

#ifndef HANDLE_SIGNAL
#define HANDLE_SIGNAL 1
#endif

ServerConfig* config = NULL;
Server*       server = NULL;
ChatServer*   chat_server = NULL;

static void cleanup() {
    if (server) {
        server_stop(server);
        server_destroy(server);
    }
    if (config) {
        server_middlewares_destroy(config->middlewares);
        free(config);
    }
    if (chat_server) {
        destroy_chat_server(chat_server);
    }
    exit(0);
}

int main() {
    signal(SIGPIPE, SIG_IGN);

#if HANDLE_SIGNAL
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
#endif

    ChatServerConfig chat_config = {
        .max_name_length = 32,
        .max_clients = 256,
        .message_buffer_size = 1024,
        .server_port = 30001,
    };
    parse_chat_config_from_env(&chat_config);

    chat_server = malloc(sizeof(ChatServer));
    init_chat_server(chat_server, chat_config);

    pthread_t chat_server_thread;
    if (start_chat_server(chat_server, &chat_server_thread)) {
        fprintf(stderr, "Failed to start chat server.\n");
        exit(EXIT_FAILURE);
    }
    pthread_detach(chat_server_thread);
    printf("Chat server started on port %d\n", chat_server->config.server_port);

    config = server_config_create();
    server_middlewares_add(config->middlewares, "Logger",
                           middleware_logger_handler);
    server_middlewares_add(config->middlewares, "Home Page",
                           middleware_homepage_handler);
    server_middlewares_add(config->middlewares, "404", middleware_404_handler);
    config->debug = getenv("DEBUG") != NULL;

    server = server_create(config);
    server_start(server);

    // unreachable
    return EXIT_SUCCESS;
}
