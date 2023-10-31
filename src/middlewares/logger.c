#include "logger.h"
#define UNUSED(x) (void)(x)

uint64_t _global_request_id = 0;

bool middleware_logger_handler(Request* request, Response* response) {
    UNUSED(response);

    time_t     rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    printf("%10ld [%04d %02d %02d %02d:%02d:%02d] %6s | %s\n",
           _global_request_id, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
           timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min,
           timeinfo->tm_sec, request->method, request->path);

    ++_global_request_id;
    return true;
}
