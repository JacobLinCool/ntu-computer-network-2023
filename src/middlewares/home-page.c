#include "home-page.h"

#define CSS "body { background-color: #f0f0f0; font-family: sans-serif; }"

#define HTML                                                                   \
    "<h1>NTNU 41047029S Jacob Lin</h1>"                                        \
    "<p>This is a simple web server written in C.</p>"                         \
    "<style>" CSS "</style>"

bool middleware_homepage_handler(Request* request, Response* response) {
    if (strcmp(request->path, "/") == 0) {
        response->status = 200;
        headers_set(response->headers, "Content-Type",
                    "text/html; charset=utf-8");

        time_t     rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char* time_str = asctime(timeinfo);

        response->body = strdup(HTML);
        response->body = realloc(response->body,
                                 strlen(response->body) + strlen(time_str) + 1);
        strcat(response->body, time_str);

        return false;
    }

    return true;
}
