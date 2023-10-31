#include "pool.h"

struct WorkerArg {
    struct ThreadPool* pool;
    int32_t            index;
};

static void* thread_pool_worker(void* _arg) {
    struct WorkerArg* arg = (struct WorkerArg*)_arg;

    while (true) {
        void* item = safe_queue_dequeue(arg->pool->queue);

        if (item == NULL && !arg->pool->running) {
            break;
        }

        if (arg->pool->debug) {
            fprintf(stderr,
                    "Task assigned to Worker %d. %ld tasks still waiting.\n",
                    arg->index, arg->pool->queue->size);
        }
        arg->pool->worker(item);
    }

    if (arg->pool->debug) {
        fprintf(stderr, "Worker %d exiting\n", arg->index);
    }
    free(arg);
    return NULL;
}

struct ThreadPool* thread_pool_create(int32_t size, void* (*worker)(void*),
                                      bool    debug) {
    struct ThreadPool* pool = malloc(sizeof(struct ThreadPool));
    pool->size = size;
    pool->worker = worker;
    pool->debug = debug;
    pool->running = true;
    pool->threads = malloc(sizeof(pthread_t) * size);
    pool->queue = safe_queue_create();

    for (int32_t i = 0; i < size; i++) {
        struct WorkerArg* arg = malloc(sizeof(struct WorkerArg));
        arg->pool = pool;
        arg->index = i;

        pthread_create(&pool->threads[i], NULL, thread_pool_worker, arg);
    }

    return pool;
}

void thread_pool_push(struct ThreadPool* pool, void* worker_arg) {
    if (pool->running) {
        safe_queue_enqueue(pool->queue, worker_arg);
    }
}

void thread_pool_destroy(struct ThreadPool* pool) {
    pool->running = false;
    for (int32_t i = 0; i < pool->size; i++) {
        safe_queue_enqueue(pool->queue, NULL);
    }

    for (int32_t i = 0; i < pool->size; i++) {
        if (pool->debug) {
            fprintf(stderr, "Waiting for Worker %d to finish ...\n", i);
        }
        pthread_join(pool->threads[i], NULL);
        if (pool->debug) {
            fprintf(stderr, "Worker %d finished.\n", i);
        }
    }

    safe_queue_destroy(pool->queue);
    free(pool->threads);
    free(pool);
}
