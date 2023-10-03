#include "request.h"

struct Request* request_parse(char* buffer) {
    struct Request* request = malloc(sizeof(struct Request));
    request->headers = headers_create();
    request->body = NULL;

    // Tokenize the HTTP request
    size_t offset = 0;
    char*  line = strtok(buffer, "\r\n");
    offset += strlen(line) + 2;
    char* method = strtok(line, " ");
    char* path = strtok(NULL, " ");
    strtok(NULL, " ");  // Skip HTTP version

    // Copy method and path
    request->method = strdup(method);
    request->path = strdup(path);

    // Parse headers
    while ((line = strtok(buffer + offset, "\r\n")) && strlen(line) > 0) {
        // printf("header: \"%s\"\n", line);
        offset += strlen(line) + 2;
        char* name = strtok(line, ": ");
        char* value = strtok(NULL, "");
        headers_set(request->headers, name,
                    value[0] == ' ' ? value + 1 : value);

        if (strncmp(buffer + offset, "\r\n", 2) == 0) {
            offset += 2;
            break;
        }
    }

    // Parse body if exists
    char* body = strtok(buffer + offset, "");
    request->body = strdup(body ? body : "");

    return request;
}

void request_destroy(struct Request* request) {
    free(request->method);
    free(request->path);
    headers_destroy(request->headers);
    if (request->body) {
        free(request->body);
    }
    free(request);
}
