#include "queue.h"
#include <string.h>

void queue_init(struct queue* q, size_t elem_sz)
{
    memset(q, 0, sizeof(struct queue));
    q->elem_sz = elem_sz;
}

/* Node layout:
 *   Data (elem_sz size)
 *   Next ptr (void* size)
 *   Prev ptr (void* size) */

void queue_push(struct queue* q, void* v)
{
    /* Create new element with elem_sz + prev + next ptr size */
    void* node = malloc(q->elem_sz + 2 * sizeof(void*));
    /* Copy pushed data */
    memcpy(node, v, q->elem_sz);
    /* Set next, prev pointers */
    void** next = node + q->elem_sz;
    *next = q->back;
    void** prev = node + q->elem_sz + sizeof(void*);
    *prev = 0;
    /* Set queue old back prev ptr */
    if (q->back)
        *(void**)(q->back + q->elem_sz + sizeof(void*)) = node;
    /* Set node as the new back */
    q->back = node;
    if (!q->front)
        q->front = node;
    q->size++;
}

void* queue_front(struct queue* q)
{
    return q->front;
}

void queue_pop(struct queue* q)
{
    /* New front */
    void* front_prev = *(void**)(q->front + q->elem_sz + sizeof(void*));
    /* Free old front and set new one */
    free(q->front);
    q->front = front_prev;
    /* Set new front's next to 0 */
    if (q->front)
        *(void**)(q->front + q->elem_sz) = 0;
    q->size--;
}

void queue_destroy(struct queue* q)
{
    while(q->size)
        queue_pop(q);
    memset(q, 0, sizeof(struct queue));
}
