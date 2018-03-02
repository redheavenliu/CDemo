#ifndef M_QUEUE_H
#define M_QUEUE_H
#include <stdio.h>
#include <stdbool.h>
#include<stdlib.h>

#include "../config"

typedef struct queue_T {
    struct queue_T *next;
    void *data;
} queue_T;

extern void q_init(queue_T* q);
extern void* q_put(queue_T* q,void *data);
extern void* q_get(queue_T* q,void **data);
extern void q_free(queue_T* q);


#endif /* M_QUEUE_H */
