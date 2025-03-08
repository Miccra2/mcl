#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__

#include <stddef.h>
#include <stdlib.h>

typedef struct {
    size_t capacity;
    size_t count;
    char *data;
} da_t;

#ifndef DYNAMIC_ARRAY_CAPACITY
#define DYNAMIC_ARRAY_CAPACITY 4096
#endif

#define new_da() \
    ((da_t){ \
        .capacity = DYNAMIC_ARRAY_CAPACITY, \
        .count = 0, \
        .data = malloc(DYNAMIC_ARRAY_CAPACITY), \
    })

#define del_da(da) ({ free(da); (da) = NULL; })

#define da_append(da, d) ({ \
    while ((da).capacity < (da).count + sizeof(d)) { \
        (da).capacity *= 2; \
        (da).data = realloc((da).data, (da).capacity); \
    } \
    (typeof(d) *)((da).data)[(da).count/sizeof(d)] = (d); \
    (da).count += sizeof(d); \
    })

#endif // __DYNAMIC_ARRAY_H__
