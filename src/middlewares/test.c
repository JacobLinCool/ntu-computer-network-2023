#include "test.h"

bool middleware_test_handler(Request* request, Response* response) {
    response->status = 200;
    headers_set(response->headers, "Content-Type", "text/plain");

    char* body = (char*)calloc(1024, 1024);
    char* headers = headers_to_string(request->headers);
    sprintf(body, "You requested %s %s\n---\nWith headers:\n%s\n---\nBody:\n%s",
            request->method, request->path, headers, request->body);
    free(headers);
    response->body = realloc(body, strlen(body) + 1);

    return true;
}
