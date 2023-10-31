#ifndef _GUARD_HEADER_POOL
#define _GUARD_HEADER_POOL

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "queue.h"

typedef struct ThreadPool {
    int32_t           size;
    pthread_t*        threads;
    struct SafeQueue* queue;
    bool              running;
    bool              debug;
    void* (*worker)(void*);
} ThreadPool;

struct ThreadPool* thread_pool_create(int32_t size, void* (*worker)(void*),
                                      bool    debug);
void               thread_pool_push(struct ThreadPool* pool, void* worker_arg);
void               thread_pool_destroy(struct ThreadPool* pool);

#endif
