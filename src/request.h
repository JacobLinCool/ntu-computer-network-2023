#ifndef _GUARD_HEADER_REQUEST
#define _GUARD_HEADER_REQUEST

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"

typedef struct Request {
    char*           method;
    char*           path;
    struct Headers* headers;
    char*           body;
} Request;

struct Request* request_parse(char* buffer);
void            request_destroy(struct Request* request);

#endif
