#ifndef __DYNAMIC_ARRAY__
#define __DYNAMIC_ARRAY__

#include <stdint.h>
#include <stdlib.h>

#ifndef DEFAULT_ARRAY_CAPACITY
#define DEFAULT_ARRAY_CAPACITY 8*1024       // default capacity 8KiB
#endif // DEFAULT_CAPACITY

typedef struct {
    uint64_t capacity;      // capacity of data in bytes
    uint64_t count;         // count of used data in bytes
    char buffer[];          // dynamic array
} DynamicArray;

/* DynamicArray constructor */
DynamicArray *new_dynamic_array() {
    DynamicArray *array = malloc(sizeof(DynamicArray) + DEFAULT_ARRAY_CAPACITY);
    array->capacity = DEFAULT_ARRAY_CAPACITY;
    array->count = 0;
    return array
}

/* DynamicArrary destructor */
void del_dynamic_array(DynamicArray **array) {
    free(*array);
    *array = NULL;
}

void append_dynamic_array(DynamicArray **array, void *data, uint64_t size) {
    while ((*array)->count+size > (*array)->capacity) {
        DynamicArray *new_array = malloc(sizeof(DynamicArray) + (*array)->capacity*2);
        new_array->capcity = (*array)->capacity*2;
        new_array->count = (*array)->count;
        for (uint64_t i = 0; i < (*array)->count; i++) {
            new_array->buffer[i] = (*array)->buffer[i];
        }
        *array = new_array;
    }
    for (uint64_t i = 0; i < size; i++) {
        (*array)->buffer[(*array)->count] = ((char *)data)[i];
        (*array)->buffer++;
    }
}

#endif // __DYNAMIC_ARRAY__
