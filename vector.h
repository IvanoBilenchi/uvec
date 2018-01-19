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

#define __VECTOR_IMPL(T, SCOPE)                                                                     \
    SCOPE Vector_##T* vector_alloc_##T(void) {                                                      \
        return calloc(1, sizeof(Vector_##T));                                                       \
    }                                                                                               \
    SCOPE void vector_free_##T(Vector_##T *vector) {                                                \
        if (!vector) return;                                                                        \
        if (vector->allocated) free(vector->storage);                                               \
        free(vector);                                                                               \
    }                                                                                               \
    SCOPE void vector_reserve_capacity_##T(Vector_##T *vector, uint32_t capacity) {                 \
        if (vector->allocated < capacity) {                                                         \
            __vector_int32_next_power_2(capacity);                                                  \
            vector->allocated = capacity;                                                           \
            vector->storage = realloc(vector->storage, sizeof(T) * capacity);                       \
        }                                                                                           \
    }                                                                                               \
    SCOPE void vector_append_array_##T(Vector_##T *vector, T const *array, uint32_t n) {            \
        if (!(n && array)) return;                                                                  \
        uint32_t old_count = vector->count;                                                         \
        uint32_t new_count = old_count + n;                                                         \
        vector->count = new_count;                                                                  \
        vector_reserve_capacity_##T(vector, new_count);                                             \
        memcpy(&(vector->storage[old_count]), array, n * sizeof(T));                                \
    }                                                                                               \
    SCOPE Vector_##T* vector_copy_##T(Vector_##T const *vector) {                                   \
        Vector_##T* copy = vector_alloc_##T();                                                      \
        vector_append_array_##T(copy, vector->storage, vector->count);                              \
        return copy;                                                                                \
    }                                                                                               \
    SCOPE void vector_shrink_##T(Vector_##T *vector) {                                              \
        uint32_t new_allocated = vector->count;                                                     \
        if (new_allocated) {                                                                        \
            __vector_int32_next_power_2(new_allocated);                                             \
            if (new_allocated < vector->allocated) {                                                \
                vector->allocated = new_allocated;                                                  \
                vector->storage = realloc(vector->storage, sizeof(T) * new_allocated);              \
            }                                                                                       \
        } else {                                                                                    \
            free(vector->storage);                                                                  \
            vector->allocated = 0;                                                                  \
        }                                                                                           \
    }                                                                                               \
    SCOPE void vector_push_##T(Vector_##T *vector, T item) {                                        \
        uint32_t allocated = vector->allocated;                                                     \
        uint32_t count = vector->count;                                                             \
        if (count == allocated) {                                                                   \
            allocated = allocated ? allocated<<1 : 2;                                               \
            vector->allocated = allocated;                                                          \
            vector->storage = realloc(vector->storage, sizeof(T) * allocated);                      \
        }                                                                                           \
        vector->storage[count] = item;                                                              \
        vector->count++;                                                                            \
    }                                                                                               \
    SCOPE T vector_pop_##T(Vector_##T *vector) {                                                    \
        return vector->storage[--vector->count];                                                    \
    }                                                                                               \
    SCOPE void vector_remove_at_##T(Vector_##T *vector, uint32_t idx) {                             \
        uint32_t count = vector->count;                                                             \
        if (idx < count - 1) {                                                                      \
            size_t block_size = (count - idx) * sizeof(T);                                          \
            memmove(&(vector->storage[idx]), &(vector->storage[idx + 1]), block_size);              \
        }                                                                                           \
        vector->count--;                                                                            \
    }                                                                                               \
    SCOPE void vector_remove_all_##T(Vector_##T *vector) {                                          \
        vector->count = 0;                                                                          \
    }

#define __VECTOR_IMPL_EQUATABLE(T, SCOPE, __equal_func, equal_func_is_identity)                     \
    SCOPE uint32_t vector_index_of_##T(Vector_##T *vector, T item) {                                \
        T *storage = vector->storage;                                                               \
        for (uint32_t i = 0, n = vector->count; i < n; ++i) {                                       \
            if (__equal_func(storage[i], item)) return i;                                           \
        }                                                                                           \
        return VECTOR_INDEX_NOT_FOUND;                                                              \
    }                                                                                               \
    SCOPE bool vector_push_unique_##T(Vector_##T *vector, T item) {                                 \
        bool insert = vector_index_of_##T(vector, item) == VECTOR_INDEX_NOT_FOUND;                  \
        if (insert) vector_push_##T(vector, item);                                                  \
        return insert;                                                                              \
    }                                                                                               \
    SCOPE bool vector_remove_##T(Vector_##T *vector, T item) {                                      \
        uint32_t idx = vector_index_of_##T(vector, item);                                           \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                        \
            vector_remove_at_##T(vector, idx);                                                      \
            return true;                                                                            \
        }                                                                                           \
        return false;                                                                               \
    }                                                                                               \
    SCOPE bool vector_equals_##T(Vector_##T *vector, Vector_##T *other) {                           \
        if (vector == other) return true;                                                           \
        if (!vector || !other || vector->count != other->count) return false;                       \
        if (vector->count == 0 && other->count == 0) return true;                                   \
        if (equal_func_is_identity)                                                                 \
            return memcmp(vector->storage, other->storage, vector->count * sizeof(T)) == 0;         \
        vector_iterate(T, vector, item, idx, {                                                      \
            if (!__equal_func(item, other->storage[idx])) return false;                             \
        });                                                                                         \
        return true;                                                                                \
    }

#define __VECTOR_IMPL_IDENTICAL(T, SCOPE)                                                           \
    SCOPE uint32_t vector_index_of_identical_##T(Vector_##T *vector, T item) {                      \
        T *storage = vector->storage;                                                               \
        for (uint32_t i = 0, n = vector->count; i < n; ++i) {                                       \
            if (storage[i] == item) return i;                                                       \
        }                                                                                           \
        return VECTOR_INDEX_NOT_FOUND;                                                              \
    }                                                                                               \
    SCOPE bool vector_push_unique_identical_##T(Vector_##T *vector, T item) {                       \
        bool insert = vector_index_of_identical_##T(vector, item) == VECTOR_INDEX_NOT_FOUND;        \
        if (insert) vector_push_##T(vector, item);                                                  \
        return insert;                                                                              \
    }                                                                                               \
    SCOPE bool vector_remove_identical_##T(Vector_##T *vector, T item) {                            \
        uint32_t idx = vector_index_of_identical_##T(vector, item);                                 \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                        \
            vector_remove_at_##T(vector, idx);                                                      \
            return true;                                                                            \
        }                                                                                           \
        return false;                                                                               \
    }                                                                                               \
    SCOPE bool vector_identical_##T(Vector_##T *vector, Vector_##T *other) {                        \
        if (vector == other) return true;                                                           \
        if (!vector || !other || vector->count != other->count) return false;                       \
        if (vector->count == 0 && other->count == 0) return true;                                   \
        return memcmp(vector->storage, other->storage, vector->count * sizeof(T)) == 0;             \
    }


#define VECTOR_DECL(T)                                                                          \
    typedef struct Vector_##T { uint32_t count, allocated; T *storage; } Vector_##T;            \
    __VECTOR_IMPL(T, static __vector_inline __vector_unused)

#define VECTOR_DECL_EQUATABLE(T, __equal_func)                                                  \
    VECTOR_DECL(T);                                                                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __equal_func, 0)

#define VECTOR_DECL_EQUATABLE_IDENTITY(T, __equal_func)                                         \
    VECTOR_DECL(T);                                                                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __equal_func, 0)         \
    __VECTOR_IMPL_IDENTICAL(T, static __vector_inline __vector_unused)

#define VECTOR_DECL_IDENTITY(T)                                                                 \
    VECTOR_DECL(T);                                                                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __vector_identical, 1)   \
    __VECTOR_IMPL_IDENTICAL(T, static __vector_inline __vector_unused)

#define Vector(T) Vector_##T
#define vector_struct(T) struct Vector_##T

#define vector_alloc(T) vector_alloc_##T()
#define vector_free(T, vec) vector_free_##T(vec)
#define vector_copy(T, vec) vector_copy_##T(vec)

#define VECTOR_INIT(T) (Vector_##T){ .count = 0, .allocated = 0, .storage = NULL }
#define vector_deinit(vec) do {         \
    if ((vec).storage) {                \
        free((vec).storage);            \
        (vec).storage = NULL;           \
    }                                   \
    (vec).count = (vec).allocated = 0;  \
} while(0)

#define vector_reserve_capacity(T, vec, size) vector_reserve_capacity_##T(vec, size)
#define vector_expand(T, vec, size) vector_reserve_capacity_##T(vec, (vec->count + size))
#define vector_shrink(T, vec) vector_shrink_##T(vec)

#define vector_get(vec, idx) ((vec)->storage[(idx)])
#define vector_set(vec, idx, item) ((vec)->storage[(idx)] = (item))
#define vector_first(vec) ((vec)->storage[0])
#define vector_last(vec) ((vec)->storage[(vec)->count-1])
#define vector_is_empty(vec) (!((vec) && (vec)->count))
#define vector_count(vec) ((vec) ? (vec)->count : 0)

#define vector_push(T, vec, item) vector_push_##T(vec, item)
#define vector_pop(T, vec) vector_pop_##T(vec)

#define vector_remove_at(T, vec, idx) vector_remove_at_##T(vec, idx)
#define vector_remove_all(T, vec) vector_remove_all_##T(vec)

#define vector_append(T, vec, vec_to_append) vector_append_array_##T(vec, (vec_to_append)->storage, (vec_to_append)->count)
#define vector_append_array(T, vec, array, n) vector_append_array_##T(vec, array, n)

#define vector_iterate(T, vec, item_name, idx_name, code)                                       \
    if (vector_count(vec)) {                                                                    \
        for (uint32_t (idx_name) = 0, __n = (vec)->count; (idx_name) != __n; ++(idx_name)) {    \
            T item_name = vector_get((vec), (idx_name));                                        \
            code;                                                                               \
        }                                                                                       \
    }

#define vector_foreach(T, vec, item_name, code) vector_iterate(T, vec, item_name, __i, code)

#define vector_index_of(T, vec, item) vector_index_of_##T(vec, item)
#define vector_index_of_identical(T, vec, item) vector_index_of_identical_##T(vec, item)

#define vector_contains(T, vec, item) (vector_index_of_##T(vec, item) != VECTOR_INDEX_NOT_FOUND)
#define vector_contains_identical(T, vec, item) (vector_index_of_identical_##T(vec, item) != VECTOR_INDEX_NOT_FOUND)

#define vector_push_unique(T, vec, item) vector_push_unique_##T(vec, item)
#define vector_push_unique_identical(T, vec, item) vector_push_unique_identical_##T(vec, item)

#define vector_remove(T, vec, item) vector_remove_##T(vec, item)
#define vector_remove_identical(T, vec, item) vector_remove_identical_##T(vec, item)

#define vector_equals(T, vec_a, vec_b) vector_equals_##T(vec_a, vec_b)
#define vector_identical(T, vec_a, vec_b) vector_identical_##T(vec_a, vec_b)

#define vector_append_unique(T, vec, vec_to_append)     \
    vector_foreach(T, vec_to_append, __item, {          \
        vector_push_unique_##T(vec, __item);            \
    })
#define vector_append_unique_identical(T, vec, vec_to_append)   \
    vector_foreach(T, vec_to_append, __item, {                  \
        vector_push_unique_identical_##T(vec, __item);          \
    })

#define vector_remove_all_from(T, vec, vec_to_remove)   \
    vector_foreach(T, vec_to_remove, __item, {          \
        vector_remove_##T(vec, __item);                 \
    })

#endif /* vector_h */
