#include "not-found.h"
#define UNUSED(x) (void)(x)

bool middleware_404_handler(Request* request, Response* response) {
    UNUSED(request);

    response->status = 404;
    headers_set(response->headers, "Content-Type", "text/plain");
    response->body = strdup("404 Not Found!!\n");

    return false;
}
