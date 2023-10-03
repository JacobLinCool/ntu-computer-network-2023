#ifndef _GUARD_HEADER_MIDDLEWARE
#define _GUARD_HEADER_MIDDLEWARE

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "request.h"
#include "response.h"

typedef struct ServerMiddleware {
    char* name;
    // The handler function which will be chained together. If the handler
    // returns false, the server will stop calling the next handler.
    //
    // Do NOT free the request nor the response in the handler, they
    // will be freed automatically.
    bool (*handler)(struct Request*, struct Response*);
} ServerMiddleware;

typedef struct ServerMiddlewares {
    struct ServerMiddleware* middlewares;
    int32_t                  size;
} ServerMiddlewares;

struct ServerMiddlewares* server_middlewares_create();
void server_middlewares_add(struct ServerMiddlewares* middlewares, char* name,
                            bool (*handler)(struct Request*, struct Response*));
void server_middlewares_destroy(struct ServerMiddlewares* middlewares);

#endif
