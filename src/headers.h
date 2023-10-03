#ifndef _GUARD_HEADER_HEADERS
#define _GUARD_HEADER_HEADERS

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Header {
    char* name;
    char* value;
} Header;

typedef struct Headers {
    struct Header* fields;
    int32_t        size;
} Headers;

struct Headers* headers_create();
void            headers_set(struct Headers* headers, char* name, char* value);
char*           headers_get(struct Headers* headers, char* name);
int32_t         headers_delete(struct Headers* headers, char* name);
char*           headers_to_string(struct Headers* headers);
void            headers_destroy(struct Headers* headers);

#endif
