#ifndef _GUARD_HEADER_SAFEQUEUE
#define _GUARD_HEADER_SAFEQUEUE

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

// A thread-safe queue.
typedef struct SafeQueue {
    void**          data;
    size_t          size;
    size_t          capacity;
    size_t          front;
    size_t          rear;
    pthread_mutex_t mutex;
    pthread_cond_t  cond_var;
} SafeQueue;

// Create a new thread-safe queue.
struct SafeQueue* safe_queue_create();
void safe_queue_resize(struct SafeQueue* queue, size_t new_capacity);
// Enqueue an item into the queue.
void safe_queue_enqueue(struct SafeQueue* queue, void* item);
// Dequeue an item from the queue. If the queue is empty, this function will
// block until an item is available.
void* safe_queue_dequeue(struct SafeQueue* queue);
void  safe_queue_destroy(struct SafeQueue* queue);

#endif
