#include "vector.h"
#include <stdlib.h>
#include <string.h>

#define VECTOR_INIT_CAPACITY 10

void vector_init(struct vector* v)
{
    memset(v, 0, sizeof(struct vector));
    v->capacity = VECTOR_INIT_CAPACITY;
    v->size = 0;
    v->data = malloc(v->capacity * sizeof(v->data));
}

void vector_destroy(struct vector* v)
{
    free(v->data);
    v->data = 0;
    v->capacity = 0;
    v->size = 0;
}

static void vector_grow(struct vector* v)
{
    v->capacity = (v->capacity * 2 + sizeof(v->data));
    v->data = realloc(v->data, v->capacity * sizeof(v->data));
    memset(v->data + v->size, 0, (v->capacity - v->size) * sizeof(v->data));
}

void vector_append(struct vector* v, long thing)
{
    if (v->capacity - v->size < 1) {
        vector_grow(v);
    }
    v->size++;
    v->data[v->size - 1] = thing;
}
