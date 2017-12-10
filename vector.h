//
//  Created by Ivano Bilenchi on 26/11/17.
//  Copyright © 2017 SisInf Lab. All rights reserved.
//

#ifndef vector_h
#define vector_h

#include <stdbool.h>
#include <stdlib.h>
#include "array.h"

#define VECTOR_INDEX_NOT_FOUND UINT32_MAX

#ifndef __vector_inline
    #ifdef _MSC_VER
        #define __vector_inline __inline
    #else
        #define __vector_inline inline
    #endif
#endif /* __vector_inline */

#ifndef __vector_unused
    #if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
        #define __vector_unused __attribute__ ((__unused__))
    #else
        #define __vector_unused
    #endif
#endif /* __vector_unused */

#define __vector_int32_next_power_2(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#define __vector_identical(a, b) ((a) == (b))

#define __VECTOR_IMPL_EQUATABLE(T, SCOPE, __equal_func)                                         \
    SCOPE uint32_t vector_index_of_##T(Vector_##T vector, T item) {                             \
        for (uint32_t i = 0, n = vector.n; i < n; ++i) {                                        \
            if (__equal_func(vector.a[i], item)) return i;                                      \
        }                                                                                       \
        return VECTOR_INDEX_NOT_FOUND;                                                          \
    }                                                                                           \
    SCOPE uint32_t vector_index_of_identical_##T(Vector_##T vector, T item) {                   \
        for (uint32_t i = 0, n = vector.n; i < n; ++i) {                                        \
            if (vector.a[i] == item) return i;                                                  \
        }                                                                                       \
        return VECTOR_INDEX_NOT_FOUND;                                                          \
    }                                                                                           \
    SCOPE bool vector_push_unique_##T(Vector_##T vector, T item) {                              \
        bool insert = vector_index_of_##T(vector, item) == VECTOR_INDEX_NOT_FOUND;              \
        if (insert) vector_push(T, vector, item);                                               \
        return insert;                                                                          \
    }                                                                                           \
    SCOPE bool vector_push_unique_identical_##T(Vector_##T vector, T item) {                    \
        bool insert = vector_index_of_identical_##T(vector, item) == VECTOR_INDEX_NOT_FOUND;    \
        if (insert) vector_push(T, vector, item);                                               \
        return insert;                                                                          \
    }                                                                                           \
    SCOPE bool vector_remove_##T(Vector_##T vector, T item) {                                   \
        uint32_t idx = vector_index_of_##T(vector, item);                                       \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                    \
            vector_remove_at(T, vector, idx);                                                   \
            return true;                                                                        \
        }                                                                                       \
        return false;                                                                           \
    }                                                                                           \
    SCOPE bool vector_remove_identical_##T(Vector_##T vector, T item) {                         \
        uint32_t idx = vector_index_of_identical_##T(vector, item);                             \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                    \
            vector_remove_at(T, vector, idx);                                                   \
            return true;                                                                        \
        }                                                                                       \
        return false;                                                                           \
    }

#define VECTOR_DECL(T) typedef struct Vector_##T { uint32_t n, m; T *a; } Vector_##T

#define VECTOR_DECL_EQUATABLE(T, __equal_func)  \
    VECTOR_DECL(T);                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __equal_func)

#define VECTOR_DECL_EQUATABLE_IDENTITY(T) VECTOR_DECL_EQUATABLE(T, __vector_identical)

#define Vector(T) Vector_##T

#define VECTOR_INIT(T) (Vector_##T){ .n = 0, .m = 0, .a = NULL }
#define vector_init(vec) ((vec).n = (vec).m = 0, (vec).a = NULL)
#define vector_deinit(vec) do {     \
    if ((vec).a) {                  \
        free((vec).a);              \
        (vec).a = NULL;             \
    }                               \
    (vec).n = (vec).m = 0;          \
} while(0)

#define vector_copy(T, vec, other_vec) do {                             \
    uint32_t __new_n = (other_vec).n;                                   \
    if (!__new_n) break;                                                \
    if ((vec).m < __new_n) vector_reserve_capacity(T, (vec), __new_n);  \
    (vec).n = __new_n;                                                  \
    memcpy((vec).a, (other_vec).a, sizeof(T) * __new_n);                \
} while (0)

#define vector_to_array(vec) ((vec).a)
#define vector_count(vec) ((vec).n)
#define vector_max_size(vec) ((vec).m)

#define vector_reserve_capacity(T, vec, size) do {              \
    if ((vec).m < (size)) {                                     \
        (vec).m = (size);                                       \
        __vector_int32_next_power_2((vec).m);                   \
        (vec).a = realloc((vec).a, sizeof(T) * (vec).m);        \
    }                                                           \
} while (0)

#define vector_expand(T, vec, size) vector_reserve_capacity(T, vec, ((vec).n + size))

#define vector_shrink(T, vec) do {                          \
    (vec).m = (vec).n;                                      \
    __vector_int32_next_power_2((vec).m);                   \
    (vec).a = realloc((vec).a, sizeof(T) * (vec).m);        \
} while (0)

#define vector_get(vec, idx) ((vec).a[(idx)])
#define vector_set(vec, idx, item) ((vec).a[(idx)] = (item))

#define vector_push(T, vec, item) do {                          \
    if ((vec).n == (vec).m) {                                   \
        (vec).m = (vec).m ? (vec).m<<1 : 2;                     \
        (vec).a = realloc((vec).a, sizeof(T) * (vec).m);        \
    }                                                           \
    (vec).a[(vec).n] = (item);                                  \
    (vec).n++;                                                  \
} while (0)

#define vector_pop(vec) ((vec).a[--(vec).n])

#define vector_remove_at(T, vec, idx) do {                              \
    if ((idx) < (vec).n - 1) {                                          \
        size_t block_size = ((vec).n - (idx)) * sizeof(T);              \
        memmove(&((vec).a[(idx)]), &((vec).a[(idx) + 1]), block_size);  \
    }                                                                   \
    --(vec).n;                                                          \
} while (0)

#define vector_remove_all(vec) ((vec).n = 0)

#define vector_append(T, vec, vec_to_append)         \
    vector_foreach((vec_to_append), T __item, {      \
        vector_push(T, (vec), __item);               \
    })

#define vector_append_array(T, vec, array_to_append) \
    array_foreach((array_to_append), T __item, {     \
        vector_push(T, (vec), __item);               \
    })

#define vector_append_all(T, vec, ...) vector_append_array(T, (vec), ((T[]){ __VA_ARGS__ }))

#define vector_iterate(vec, item, idx_name, code)                                                   \
    for (unsigned long (idx_name) = 0, __n = vector_count(vec); (idx_name) < __n; ++(idx_name)) {   \
        item = vector_get((vec), (idx_name));                                                       \
        code;                                                                                       \
    }

#define vector_foreach(vec, item, code) vector_iterate(vec, item, __i, code)

#define vector_index_of(T, vec, item) vector_index_of_##T(vec, item)
#define vector_index_of_identical(T, vec, item) vector_index_of_identical_##T(vec, item)

#define vector_contains(T, vec, item) (vector_index_of_##T(vec, item) != VECTOR_INDEX_NOT_FOUND)
#define vector_contains_identical(T, vec, item) (vector_index_of_identical_##T(vec, item) != VECTOR_INDEX_NOT_FOUND)

#define vector_push_unique(T, vec, item) vector_push_unique_##T(vec, item)
#define vector_push_unique_identical(T, vec, item) vector_push_unique_identical_##T(vec, item)

#define vector_remove(T, vec, item) vector_remove_##T(vec, item)
#define vector_remove_identical(T, vec, item) vector_remove_identical_##T(vec, item)

#endif /* vector_h */
