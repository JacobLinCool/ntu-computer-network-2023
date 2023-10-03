#include "server.h"

struct ServerConfig* server_config_create() {
    struct ServerConfig* config = malloc(sizeof(struct ServerConfig));
    if (config == NULL) {
        fprintf(stderr, "Failed to allocate memory for server config\n");
        exit(EXIT_FAILURE);
    }

    config->address = "0.0.0.0";
    config->port = 8080;
    config->client_limit = 100;
    config->buffer_size = 1024 * 1024;
    config->middlewares = server_middlewares_create();
    config->debug = false;

    return config;
}

struct Server* server_create(struct ServerConfig* config) {
    struct Server* server = (struct Server*)malloc(sizeof(struct Server));
    if (server == NULL) {
        fprintf(stderr, "Failed to allocate memory for server\n");
        exit(EXIT_FAILURE);
    }

    server->config = config;

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(config->port);
    server_address.sin_addr.s_addr = inet_addr(config->address);

    if (bind(server->sockfd, (struct sockaddr*)&server_address,
             sizeof(server_address)) < 0) {
        fprintf(stderr, "Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }

    return server;
}

void server_start(struct Server* server) {
    // Listen for incoming connections
    if (listen(server->sockfd, server->config->client_limit) < 0) {
        fprintf(stderr, "Failed to listen\n");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %" PRId32 "\n", server->config->port);

    struct sockaddr_in client_addr;
    socklen_t          client_len = sizeof(client_addr);

    char buffer[server->config->buffer_size];

    // Accept incoming connections
    while (true) {
        // Block until a client connects
        int32_t client_sockfd =
            accept(server->sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            fprintf(stderr, "Failed to accept client\n");
            continue;
        }

        if (server->config->debug) {
            fprintf(stderr, "Client connected\n");
        }

        memset(buffer, 0, server->config->buffer_size);

        // Read request from the client
        ssize_t bytes_read =
            read(client_sockfd, buffer, server->config->buffer_size - 1);
        if (bytes_read <= 0) {
            fprintf(stderr,
                    "Failed to read from client or client disconnected\n");
            close(client_sockfd);
            continue;
        }

        if (server->config->debug) {
            fprintf(stderr, "Request:\n%s\n", buffer);
        }

        // Create Request and Response structs
        struct Request*  request = request_parse(buffer);
        struct Response* response = response_create();

        for (int32_t i = 0; i < server->config->middlewares->size; ++i) {
            struct ServerMiddleware middleware =
                server->config->middlewares->middlewares[i];
            if (server->config->debug) {
                fprintf(stderr, "Middleware: %s\n", middleware.name);
            }
            bool turn_over = middleware.handler(request, response);
            if (server->config->debug) {
                fprintf(stderr, "  -> %s\n", turn_over ? "next" : "stop");
            }
            if (!turn_over) {
                break;
            }
        }

        char* response_str = response_to_string(response);

        // Send HTTP response back to client
        write(client_sockfd, response_str, strlen(response_str));

        // Clean up
        free(response_str);
        response_destroy(response);
        request_destroy(request);
        close(client_sockfd);
    }
}

void server_stop(struct Server* server) {
    if (shutdown(server->sockfd, SHUT_RDWR) < 0) {
        fprintf(stderr, "Failed to shutdown server\n");
    }

    if (server->config->debug) {
        fprintf(stderr, "Server stopped.\n");
    }
}

void server_destroy(struct Server* server) {
    bool debug = server->config->debug;
    free(server);

    if (debug) {
        fprintf(stderr, "Server destroyed.\n");
    }
}
