#include "m_queue.h"

#define sta_log printf

void q_init(queue_T* q)
{
    q->data = NULL;
    q->next = NULL;
}

void* q_put(queue_T* q,void *data)
{
    if(q == NULL) {
        sta_log("queue is NULL.");
        return NULL;
    }

    if(data == NULL) {
        sta_log("data is NULL.");
        return NULL;
    }

    queue_T* q_ptr = q;
    queue_T* new_q = (queue_T*)malloc(sizeof(queue_T));
    if(new_q == NULL) {
        sta_log("malloc queue_T fail.");
        return NULL;
    }
    new_q->next = NULL;
    new_q->data = data;
    while(q_ptr->next){
        q_ptr = q_ptr->next;
    }
    q_ptr->next = new_q;

    return data;
}

void* q_get(queue_T* q,void **data)
{
    if(q == NULL) {
        sta_log("queue is NULL.");
        return NULL;
    }

    queue_T* q_ptr = q->next; // The first item.
    if(q_ptr == NULL) {
        return NULL;
    }
    q->next = q->next->next;
    *data = q_ptr->data;
    free(q_ptr);
    q_ptr = NULL;
    return data;
}

void q_free(queue_T* q)
{

}


