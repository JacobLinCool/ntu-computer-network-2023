#include "http-server.h"

struct HandlerArg {
    struct ServerConfig* config;
    int32_t              client_sockfd;
};

struct ServerConfig* server_config_create() {
    struct ServerConfig* config = malloc(sizeof(struct ServerConfig));
    if (config == NULL) {
        fprintf(stderr, "Failed to allocate memory for server config\n");
        exit(EXIT_FAILURE);
    }

    config->address = "0.0.0.0";
    config->port = 8080;
    config->client_limit = 4096 - 1;
    config->worker_limit = 4;
    config->buffer_size = 1024 * 1024;
    config->middlewares = server_middlewares_create();
    config->debug = false;

    return config;
}

void* handle_client(void* arg) {
    struct HandlerArg*   thread_arg = (struct HandlerArg*)arg;
    struct ServerConfig* config = thread_arg->config;
    int32_t              client_sockfd = thread_arg->client_sockfd;
    free(thread_arg);

    if (config->debug) {
        fprintf(stderr, "Client connected\n");
    }

    char* buffer = (char*)calloc(config->buffer_size, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for request buffer\n");
        close(client_sockfd);
        return NULL;
    }

    // Read request from the client
    ssize_t bytes_read = read(client_sockfd, buffer, config->buffer_size - 1);
    if (bytes_read <= 0) {
        fprintf(stderr, "Failed to read from client or client disconnected\n");
        free(buffer);
        close(client_sockfd);
        return NULL;
    }

    if (config->debug) {
        fprintf(stderr, "Request:\n%s\n", buffer);
    }

    // Create Request and Response structs
    struct Request*  request = request_parse(buffer);
    struct Response* response = response_create();

    for (int32_t i = 0; i < config->middlewares->size; ++i) {
        struct ServerMiddleware middleware =
            config->middlewares->middlewares[i];
        if (config->debug) {
            fprintf(stderr, "Middleware: %s\n", middleware.name);
        }
        bool turn_over = middleware.handler(request, response);
        if (config->debug) {
            fprintf(stderr, "  -> %s (stream: %s)\n",
                    turn_over ? "next" : "stop",
                    response->body_stream ? "set" : "unset");
        }
        if (!turn_over || response->body_stream != NULL) {
            break;
        }
    }

    bool stream_mode = response->body_stream != NULL;
    if (stream_mode) {
        // Send HTTP response headers back to client
        char* headers_str = headers_to_string(response->headers);
        write(client_sockfd, headers_str, strlen(headers_str));
        free(headers_str);

        // Send HTTP response body back to client
        char* buffer = (char*)calloc(config->buffer_size, sizeof(char));
        if (buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory for response buffer\n");
            close(client_sockfd);
            return NULL;
        }

        while (true) {
            ssize_t bytes_read =
                fread(buffer, sizeof(char), config->buffer_size,
                      response->body_stream);
            if (bytes_read <= 0) {
                break;
            }
            write(client_sockfd, buffer, bytes_read);
        }

        free(buffer);
        fclose(response->body_stream);

    } else {
        char* response_str = response_to_string(response);
        write(client_sockfd, response_str, strlen(response_str));
        free(response_str);
    }

    // Clean up
    response_destroy(response);
    request_destroy(request);
    free(buffer);
    close(client_sockfd);

    return NULL;
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

    server->pool = NULL;

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

    server->pool = thread_pool_create(server->config->worker_limit,
                                      handle_client, server->config->debug);

    printf("Server started on port %" PRId32 "\n", server->config->port);

    struct sockaddr_in client_addr;
    socklen_t          client_len = sizeof(client_addr);

    // Accept incoming connections
    while (true) {
        int32_t client_sockfd =
            accept(server->sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            fprintf(stderr, "Failed to accept client\n");
            continue;
        }

        struct HandlerArg* arg =
            (struct HandlerArg*)malloc(sizeof(struct HandlerArg));
        arg->client_sockfd = client_sockfd;
        arg->config = server->config;

        thread_pool_push(server->pool, arg);

        if (server->config->debug) {
            fprintf(stderr, "Socket [%d] queued.\n", client_sockfd);
        }
    }
}

void server_stop(struct Server* server) {
    if (shutdown(server->sockfd, SHUT_RDWR) < 0) {
        fprintf(stderr, "Failed to shutdown server\n");
    }

    fprintf(stderr, "Waiting for workers to finish ...\n");

    thread_pool_destroy(server->pool);

    fprintf(stderr, "Workers finished.\n");

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
