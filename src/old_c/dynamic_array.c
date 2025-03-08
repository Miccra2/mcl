#include "dynamic_array.h"

da_t *new_da() {
    da_t *dynamic_array = malloc (sizeof (da_t));

    dynamic_array->capacity = DYNAMIC_ARRAY_DEFAULT_CAPACITY;
    dynamic_array->count = 0;
    dynamic_array->data = malloc (DYNAMIC_ARRAY_DEFAULT_CAPACITY);

    return dynamic_array;
}

void del_da(da_t **dynamic_array) {
    free((*dynamic_array)->data);
    free(*dynamic_array);
    *dynamic_array = NULL;
}
