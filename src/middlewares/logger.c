#include "logger.h"
#define UNUSED(x) (void)(x)

bool middleware_logger_handler(Request* request, Response* response) {
    UNUSED(response);

    time_t     rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    printf("[%04d %02d %02d %02d:%02d:%02d] %6s | %s\n",
           timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
           timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
           request->method, request->path);

    return true;
}
