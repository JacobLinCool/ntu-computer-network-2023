#include "middleware.h"

struct ServerMiddlewares* server_middlewares_create() {
    struct ServerMiddlewares* middlewares =
        malloc(sizeof(struct ServerMiddlewares));
    middlewares->middlewares = NULL;
    middlewares->size = 0;
    return middlewares;
}

void server_middlewares_add(struct ServerMiddlewares* middlewares, char* name,
                            bool (*handler)(struct Request*,
                                            struct Response*)) {
    middlewares->size++;
    middlewares->middlewares =
        realloc(middlewares->middlewares,
                middlewares->size * sizeof(struct ServerMiddleware));
    middlewares->middlewares[middlewares->size - 1].name =
        strdup(name ? name : "unnamed middleware");
    middlewares->middlewares[middlewares->size - 1].handler = handler;
}

void server_middlewares_destroy(struct ServerMiddlewares* middlewares) {
    for (int32_t i = 0; i < middlewares->size; ++i) {
        free(middlewares->middlewares[i].name);
    }
    free(middlewares->middlewares);
    free(middlewares);
}
