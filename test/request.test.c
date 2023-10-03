#include "../src/request.h"

#include <assert.h>

void test_get();
void test_post();

int main() {
    test_get();
    test_post();

    return EXIT_SUCCESS;
}

void test_get() {
    printf("Running tests for request_parse with GET method\n");

    char* buffer = strdup(
        "GET /some/path HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: test/7.68.0\r\n"
        "Accept: */*\r\n"
        "\r\n");
    Request* request = request_parse(buffer);

    assert(strcmp(request->method, "GET") == 0);
    assert(strcmp(request->path, "/some/path") == 0);
    assert(strcmp(headers_get(request->headers, "Host"), "localhost:8080") ==
           0);
    assert(strcmp(headers_get(request->headers, "User-Agent"), "test/7.68.0") ==
           0);
    assert(strcmp(headers_get(request->headers, "Accept"), "*/*") == 0);
    assert(strcmp(request->body, "") == 0);

    request_destroy(request);
    free(buffer);
}

void test_post() {
    printf("Running tests for request_parse with POST method\n");

    char* buffer = strdup(
        "POST /some/path HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: test/7.68.0\r\n"
        "Accept: */*\r\n"
        "\r\n"
        "This is the body of the request");
    Request* request = request_parse(buffer);

    assert(strcmp(request->method, "POST") == 0);
    assert(strcmp(request->path, "/some/path") == 0);
    assert(strcmp(headers_get(request->headers, "Host"), "localhost:8080") ==
           0);
    assert(strcmp(headers_get(request->headers, "User-Agent"), "test/7.68.0") ==
           0);
    assert(strcmp(headers_get(request->headers, "Accept"), "*/*") == 0);
    assert(strcmp(request->body, "This is the body of the request") == 0);

    request_destroy(request);
    free(buffer);
}
