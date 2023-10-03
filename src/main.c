#include <signal.h>
#include <stdlib.h>

#include "middlewares/home-page.h"
#include "middlewares/logger.h"
#include "middlewares/not-found.h"
#include "server.h"

#ifndef HANDLE_SIGNAL
#define HANDLE_SIGNAL 1
#endif

ServerConfig* config;
Server*       server;

static void cleanup() {
    if (server) {
        server_stop(server);
        server_destroy(server);
    }
    if (config) {
        server_middlewares_destroy(config->middlewares);
        free(config);
    }
    exit(0);
}

int main() {
#if HANDLE_SIGNAL
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);
#endif

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
