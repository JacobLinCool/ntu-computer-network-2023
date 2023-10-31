#include "queue.h"
#define INITIAL_CAPACITY 128

struct SafeQueue* safe_queue_create() {
    struct SafeQueue* queue = malloc(sizeof(struct SafeQueue));
    queue->data = malloc(sizeof(void*) * INITIAL_CAPACITY);
    queue->size = 0;
    queue->capacity = INITIAL_CAPACITY;
    queue->front = 0;
    queue->rear = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_var, NULL);
    return queue;
}

void safe_queue_resize(struct SafeQueue* queue, size_t new_capacity) {
    pthread_mutex_lock(&queue->mutex);

    void** new_data = malloc(sizeof(void*) * new_capacity);
    size_t j = 0;

    for (size_t i = queue->front; i != queue->rear;
         i = (i + 1) % queue->capacity) {
        new_data[j++] = queue->data[i];
    }

    free(queue->data);
    queue->data = new_data;
    queue->front = 0;
    queue->rear = queue->size;
    queue->capacity = new_capacity;

    pthread_mutex_unlock(&queue->mutex);
}

void safe_queue_enqueue(struct SafeQueue* queue, void* item) {
    if (queue->size == queue->capacity) {
        safe_queue_resize(queue, queue->capacity * 2);
    }

    pthread_mutex_lock(&queue->mutex);
    queue->data[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    pthread_cond_signal(&queue->cond_var);
    pthread_mutex_unlock(&queue->mutex);
}

void* safe_queue_dequeue(struct SafeQueue* queue) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond_var, &queue->mutex);
    }

    void* item = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    if (queue->size > 0 && queue->size == queue->capacity / 4) {
        safe_queue_resize(queue, queue->capacity / 2);
    }

    pthread_mutex_unlock(&queue->mutex);
    return item;
}

void safe_queue_destroy(struct SafeQueue* queue) {
    pthread_mutex_lock(&queue->mutex);
    free(queue->data);
    pthread_mutex_unlock(&queue->mutex);

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_var);
    free(queue);
}
