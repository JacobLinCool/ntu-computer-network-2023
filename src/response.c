#include "response.h"

struct Response* response_create() {
    struct Response* response = malloc(sizeof(struct Response));
    response->status = 200;
    response->headers = headers_create();
    response->body = NULL;
    return response;
}

char* response_to_string(struct Response* response) {
    char*   buffer = malloc(1024 * 1024);
    char*   body = response->body ? response->body : "";
    char*   headers = headers_to_string(response->headers);
    int32_t length = sprintf(buffer,
                             "HTTP/1.1 %d OK\r\n"
                             "Content-Length: %ld\r\n"
                             "%s"
                             "\r\n"
                             "%s",
                             response->status, strlen(body), headers, body);
    free(headers);
    return realloc(buffer, length + 1);
}

void response_destroy(struct Response* response) {
    headers_destroy(response->headers);
    if (response->body) {
        free(response->body);
    }
    free(response);
}
