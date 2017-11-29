//
//  Created by Ivano Bilenchi on 26/11/17.
//  Copyright © 2017 SisInf Lab. All rights reserved.
//

#ifndef vector_h
#define vector_h

#include <stdbool.h>
#include <stdlib.h>
#include "array.h"

#define int32_next_power_2(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define Vector(type) struct { uint32_t n, m; type *a; }

#define VECTOR_INIT { .n = 0, .m = 0, .a = NULL }
#define vector_init(vec) ((vec).n = (vec).m = 0, (vec).a = NULL)
#define vector_deinit(vec) do {     \
    if ((vec).a) {                  \
        free((vec).a);              \
        (vec).a = NULL;             \
    }                               \
    (vec).n = (vec).m = 0;          \
} while(0)

#define vector_copy(type, vec, other_vec) do {                  \
    uint32_t __new_n = (other_vec).n;                           \
    if ((vec).m < __new_n)                                      \
        vector_reserve_capacity(type, (vec), __new_n);          \
    (vec).n = __new_n;                                          \
    memcpy((vec).a, (other_vec).a, sizeof(type) * __new_n);     \
} while (0)

#define vector_to_array(vec) ((vec).a)
#define vector_count(vec) ((vec).n)
#define vector_max_size(vec) ((vec).m)

#define vector_reserve_capacity(type, vec, size) do {           \
    if ((vec).m < (size)) {                                     \
        (vec).m = (size);                                       \
        int32_next_power_2((vec).m);                            \
        (vec).a = realloc((vec).a, sizeof(type) * (vec).m);     \
    }                                                           \
} while (0)

#define vector_shrink(type, vec) do {                       \
    (vec).m = (vec).n;                                      \
    int32_next_power_2((vec).m);                            \
    (vec).a = realloc((vec).a, sizeof(type) * (vec).m);     \
} while (0)

#define vector_get(vec, idx) ((vec).a[(idx)])
#define vector_set(vec, idx, item) ((vec).a[(idx)] = (item))

#define vector_push(type, vec, item) do {                       \
    if ((vec).n == (vec).m) {                                   \
        (vec).m = (vec).m ? (vec).m<<1 : 2;                     \
        (vec).a = realloc((vec).a, sizeof(type) * (vec).m);     \
    }                                                           \
    (vec).a[(vec).n] = (item);                                  \
    (vec).n++;                                                  \
} while (0)

#define vector_pop(vec) ((vec).a[--(vec).n])

#define vector_push_unique(type, vec, item, equal_func) do {    \
    bool __unique = true;                                       \
    vector_foreach(vec, type __item, {                          \
        if (equal_func(__item, (item))) {                       \
            __unique = false;                                   \
            break;                                              \
        }                                                       \
        if (__unique)                                           \
            vector_push(type, (vec), (item));                   \
    })                                                          \
} while (0)

#define __value_equals(a, b) ((a) == (b))
#define vector_push_unique_value(type, vec, item) vector_push_unique(type, vec, item, __value_equals)

#define vector_remove(type, vec, idx) do {                              \
    if ((idx) < (vec).n - 1) {                                          \
        size_t block_size = ((vec).n - (idx)) * sizeof(type);           \
        memmove(&((vec).a[(idx)]), &((vec).a[(idx) + 1]), block_size);  \
    }                                                                   \
    --(vec).n;                                                          \
} while (0)

#define vector_remove_all(vec) ((vec).n = 0)

#define vector_append(type, vec, vec_to_append)         \
    vector_foreach((vec_to_append), type __item, {      \
        vector_push(type, (vec), __item);               \
    })

#define vector_append_array(type, vec, array_to_append) \
    array_foreach((array_to_append), type __item, {     \
        vector_push(type, (vec), __item);               \
    })

#define vector_append_all(type, vec, ...) vector_append_array(type, (vec), ((type[]){ __VA_ARGS__ }))

#define vector_iterate(vec, item, idx_name, code)                                                   \
    for (unsigned long (idx_name) = 0, __n = vector_count(vec); (idx_name) < __n; ++(idx_name)) {   \
        item = vector_get((vec), (idx_name));                                                       \
        code;                                                                                       \
    }

#define vector_foreach(vec, item, code) vector_iterate(vec, item, __i, code)

#endif /* vector_h */
