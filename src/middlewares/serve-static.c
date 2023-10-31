#include "serve-static.h"

bool middleware_static_handler(Request* request, Response* response) {
    char* path = request->path;
    if (strcmp(path, "/") == 0) {
        path = "/index.html";
    }

    char* full_path = (char*)calloc(1, strlen(path) + strlen("public") + 1);
    sprintf(full_path, "public%s", path);

    // prevent directory traversal
    if (strstr(full_path, "..") != NULL) {
        response->status = 403;
        response->body = "403 Forbidden";
        return true;
    }

    FILE* file = fopen(full_path, "r");
    if (file == NULL) {
        response->status = 404;
        response->body = "404 Not Found";
        return true;
    }

    response->body_stream = file;

    char* ext = strrchr(path, '.');
    if (strcmp(ext, ".html") == 0) {
        ext = "text/html";
    } else if (strcmp(ext, ".css") == 0) {
        ext = "text/css";
    } else if (strcmp(ext, ".js") == 0) {
        ext = "application/javascript";
    } else if (strcmp(ext, ".png") == 0) {
        ext = "image/png";
    } else if (strcmp(ext, ".jpg") == 0) {
        ext = "image/jpeg";
    } else if (strcmp(ext, ".ico") == 0) {
        ext = "image/x-icon";
    } else {
        ext = "";
    }

    headers_set(response->headers, "Content-Type", ext);

    return true;
}
