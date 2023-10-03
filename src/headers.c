#include "headers.h"

struct Headers* headers_create() {
    struct Headers* headers = malloc(sizeof(struct Headers));
    headers->fields = NULL;
    headers->size = 0;
    return headers;
}

void headers_set(struct Headers* headers, char* name, char* value) {
    headers->size++;
    headers->fields =
        realloc(headers->fields, headers->size * sizeof(struct Header));

    name = strdup(name ? name : "");
    for (int32_t i = 0; name[i]; ++i) {
        name[i] = tolower(name[i]);
    }

    headers->fields[headers->size - 1].name = name;
    headers->fields[headers->size - 1].value = strdup(value ? value : "");
}

char* headers_get(struct Headers* headers, char* name) {
    name = strdup(name);
    for (int32_t i = 0; name[i]; ++i) {
        name[i] = tolower(name[i]);
    }

    for (int32_t i = 0; i < headers->size; ++i) {
        if (strcmp(headers->fields[i].name, name) == 0) {
            free(name);
            return headers->fields[i].value;
        }
    }

    free(name);
    return NULL;
}

int32_t headers_delete(struct Headers* headers, char* name) {
    int32_t deleted_count = 0;

    for (int32_t i = 0; i < headers->size; ++i) {
        if (strcmp(headers->fields[i].name, name) == 0) {
            free(headers->fields[i].name);
            free(headers->fields[i].value);
            deleted_count++;
        } else {
            headers->fields[i - deleted_count] = headers->fields[i];
        }
    }

    return deleted_count;
}

char* headers_to_string(struct Headers* headers) {
    char*   buffer = (char*)calloc(1024, 1024);
    int32_t length = 0;

    for (int32_t i = 0; i < headers->size; ++i) {
        length += sprintf(buffer + length, "%s: %s\r\n",
                          headers->fields[i].name, headers->fields[i].value);
    }

    return realloc(buffer, length + 1);
}

void headers_destroy(struct Headers* headers) {
    for (int32_t i = 0; i < headers->size; ++i) {
        free(headers->fields[i].name);
        free(headers->fields[i].value);
    }
    free(headers->fields);
    free(headers);
}
