#ifndef _GUARD_HEADER_RESPONSE
#define _GUARD_HEADER_RESPONSE

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "headers.h"

typedef struct Response {
    int32_t         status;
    struct Headers* headers;
    char*           body;
    FILE*           body_stream;
} Response;

struct Response* response_create();
char*            response_to_string(struct Response* response);
void             response_destroy(struct Response* response);

#endif
