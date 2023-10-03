#ifndef _GUARD_HEADER_SERVER
#define _GUARD_HEADER_SERVER

#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "middleware.h"

// The server configuration struct.
typedef struct ServerConfig {
    // The address to listen on.
    char* address;
    // The port to listen on.
    int32_t port;
    // The maximum number of clients.
    int32_t client_limit;
    // The size of the receive buffer.
    int32_t buffer_size;
    // The middlewares to use.
    struct ServerMiddlewares* middlewares;
    // Enable debug mode.
    bool debug;
} ServerConfig;

typedef struct Server {
    int32_t              sockfd;
    struct ServerConfig* config;
} Server;

struct ServerConfig* server_config_create();

struct Server* server_create(struct ServerConfig* config);
void           server_start(struct Server* server);
void           server_stop(struct Server* server);
void           server_destroy(struct Server* server);

#endif
