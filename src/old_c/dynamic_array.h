#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    size_t capacity;
    size_t count;
    void *data;
} da_t;

#ifndef DYNAMIC_ARRAY_DEFAULT_CAPACITY
#define DYNAMIC_ARRAY_DEFAULT_CAPACITY 4096
#endif // DYNAMIC_ARRAY_DEFAULT_CAPACITY

#define da_append(da, d) \
    if ((da)->count > (da)->capacity) { \
        (da)->capacity *= 2; \
        (da)->data = realloc ((da)->data, (da)->capacity*sizeof (d)); \
    } \
    ((typeof (d) *)(da)->data)[(da)->count] = (d); \
    (da)->count++;

da_t *new_da();
void del_da(da_t **dynamic_array);

#endif // __DYNAMIC_ARRAY_H__
