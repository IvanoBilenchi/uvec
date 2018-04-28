//
//  Created by Ivano Bilenchi on 26/11/17.
//  Copyright © 2017 SisInf Lab. All rights reserved.
//

#ifndef vector_h
#define vector_h

#include <stdbool.h>
#include <stdlib.h>

#define VECTOR_INDEX_NOT_FOUND UINT32_MAX

#define MACRO_CONCAT(a, b) __MACRO_CONCAT(a, b)
#define __MACRO_CONCAT(a, b) a##b

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
            size_t block_size = (count - idx - 1) * sizeof(T);                                      \
            memmove(&(vector->storage[idx]), &(vector->storage[idx + 1]), block_size);              \
        }                                                                                           \
        vector->count--;                                                                            \
    }                                                                                               \
    SCOPE void vector_remove_all_##T(Vector_##T *vector) {                                          \
        vector->count = 0;                                                                          \
    }

#define __VECTOR_IMPL_EQUATABLE(T, SCOPE, __equal_func, equal_func_is_identity)                     \
    SCOPE uint32_t vector_index_of_##T(Vector_##T *vector, T item) {                                \
        for (uint32_t i = 0; i < vector->count; ++i) {                                              \
            if (__equal_func(vector->storage[i], item)) return i;                                   \
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
        if (vector->count != other->count) return false;                                            \
        if (!vector->count) return true;                                                            \
        if (equal_func_is_identity)                                                                 \
            return memcmp(vector->storage, other->storage, vector->count * sizeof(T)) == 0;         \
        for (uint32_t i = 0; i < vector->count; ++i) {                                              \
            if (!__equal_func(vector->storage[i], other->storage[i]))                               \
                return false;                                                                       \
        }                                                                                           \
        return true;                                                                                \
    }                                                                                               \
    SCOPE bool vector_contains_all_##T(Vector_##T *vector, Vector_##T *other) {                     \
        if (vector == other) return true;                                                           \
        for (uint32_t i = 0; i < other->count; ++i) {                                               \
            if (vector_index_of_##T(vector, other->storage[i]) == VECTOR_INDEX_NOT_FOUND)           \
                return false;                                                                       \
        }                                                                                           \
        return true;                                                                                \
    }                                                                                               \
    SCOPE bool vector_contains_any_##T(Vector_##T *vector, Vector_##T *other) {                     \
        if (vector == other) return true;                                                           \
        for (uint32_t i = 0; i < other->count; ++i) {                                               \
            if (vector_index_of_##T(vector, other->storage[i]) != VECTOR_INDEX_NOT_FOUND)           \
                return true;                                                                        \
        }                                                                                           \
        return false;                                                                               \
    }

#define VECTOR_DECL(T)                                                                          \
    typedef struct Vector_##T { uint32_t count, allocated; T *storage; } Vector_##T;            \
    __VECTOR_IMPL(T, static __vector_inline __vector_unused)

#define VECTOR_DECL_EQUATABLE(T, __equal_func)                                                  \
    VECTOR_DECL(T);                                                                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __equal_func, 0)

#define VECTOR_DECL_IDENTIFIABLE(T)                                                             \
    VECTOR_DECL(T);                                                                             \
    __VECTOR_IMPL_EQUATABLE(T, static __vector_inline __vector_unused, __vector_identical, 1)

#define Vector(T) MACRO_CONCAT(Vector_, T)
#define vector_struct(T) struct MACRO_CONCAT(Vector_, T)

#define vector_alloc(T) MACRO_CONCAT(vector_alloc_, T)()
#define vector_ensure(T, vec) (vec ? vec : (vec = vector_alloc(T)))
#define vector_free(T, vec) MACRO_CONCAT(vector_free_, T)(vec)
#define vector_copy(T, vec) MACRO_CONCAT(vector_copy_, T)(vec)

#define VECTOR_INIT(T) (MACRO_CONCAT(Vector_, T)){ .count = 0, .allocated = 0, .storage = NULL }
#define vector_deinit(vec) do {         \
    if ((vec).storage) {                \
        free((vec).storage);            \
        (vec).storage = NULL;           \
    }                                   \
    (vec).count = (vec).allocated = 0;  \
} while(0)

#define vector_reserve_capacity(T, vec, size) MACRO_CONCAT(vector_reserve_capacity_, T)(vec, size)
#define vector_expand(T, vec, size) MACRO_CONCAT(vector_reserve_capacity_, T)(vec, (vec->count + size))
#define vector_shrink(T, vec) MACRO_CONCAT(vector_shrink_, T)(vec)

#define vector_get(vec, idx) ((vec)->storage[(idx)])
#define vector_set(vec, idx, item) ((vec)->storage[(idx)] = (item))
#define vector_first(vec) ((vec)->storage[0])
#define vector_last(vec) ((vec)->storage[(vec)->count-1])
#define vector_is_empty(vec) (!((vec) && (vec)->count))
#define vector_count(vec) ((vec) ? (vec)->count : 0)

#define vector_push(T, vec, item) MACRO_CONCAT(vector_push_, T)(vec, item)
#define vector_push_lazy(T, vec, item) do { vector_ensure(T, vec); vector_push(T, vec, item); } while(0)
#define vector_pop(T, vec) MACRO_CONCAT(vector_pop_, T)(vec)

#define vector_remove_at(T, vec, idx) MACRO_CONCAT(vector_remove_at_, T)(vec, idx)
#define vector_remove_all(T, vec) MACRO_CONCAT(vector_remove_all_, T)(vec)

#define vector_append(T, vec, vec_to_append) \
    MACRO_CONCAT(vector_append_array_, T)(vec, (vec_to_append)->storage, (vec_to_append)->count)

#define vector_append_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append(T, vec, vec_to_append); } while(0)

#define vector_append_array(T, vec, array, n) MACRO_CONCAT(vector_append_array_, T)(vec, array, n)

#define vector_append_array_lazy(T, vec, array, n) \
    do { vector_ensure(T, vec); vector_append_array(T, vec, array, n); } while(0)

#define vector_iterate(T, vec, item_name, idx_name, code)                               \
    if (vector_count(vec)) {                                                            \
        uint32_t __n_##idx_name = (vec)->count;                                         \
        for (uint32_t idx_name = 0; idx_name != __n_##idx_name; ++idx_name) {           \
            T item_name = vector_get((vec), (idx_name));                                \
            code;                                                                       \
        }                                                                               \
    }

#define vector_iterate_reverse(T, vec, item_name, idx_name, code)                       \
    if (vector_count(vec)) {                                                            \
        uint32_t __n_##idx_name = (vec)->count;                                         \
        for (uint32_t idx_name = __n_##idx_name; idx_name-- != 0;) {                    \
            T item_name = vector_get((vec), (idx_name));                                \
            code;                                                                       \
        }                                                                               \
    }

#define vector_foreach(T, vec, item_name, code) vector_iterate(T, vec, item_name, __i_##item_name, code)
#define vector_foreach_reverse(T, vec, item_name, code) vector_iterate_reverse(T, vec, item_name, __i_##item_name, code)

#define vector_index_of(T, vec, item) MACRO_CONCAT(vector_index_of_, T)(vec, item)
#define vector_contains(T, vec, item) (MACRO_CONCAT(vector_index_of_, T)(vec, item) != VECTOR_INDEX_NOT_FOUND)
#define vector_contains_all(T, vec, other_vec) MACRO_CONCAT(vector_contains_all_, T)(vec, other_vec)
#define vector_contains_any(T, vec, other_vec) MACRO_CONCAT(vector_contains_any_, T)(vec, other_vec)
#define vector_remove(T, vec, item) MACRO_CONCAT(vector_remove_, T)(vec, item)
#define vector_equals(T, vec_a, vec_b) MACRO_CONCAT(vector_equals_, T)(vec_a, vec_b)

#define vector_push_unique(T, vec, item) MACRO_CONCAT(vector_push_unique_, T)(vec, item)
#define vector_push_unique_lazy(T, vec, item) \
    do { vector_ensure(T, vec); vector_push_unique(T, vec, item); } while(0)

#define vector_append_unique(T, vec, vec_to_append)         \
    vector_foreach(T, vec_to_append, __item, {              \
        MACRO_CONCAT(vector_push_unique_, T)(vec, __item);  \
    })

#define vector_append_unique_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append_unique(T, vec, vec_to_append); } while(0)

#define vector_remove_all_from(T, vec, vec_to_remove)   \
    vector_foreach(T, vec_to_remove, __item, {          \
        MACRO_CONCAT(vector_remove_, T)(vec, __item);   \
    })

#define vector_deep_copy(T, dest, source, copy_func) do {           \
    if (source) {                                                   \
        dest = vector_alloc(T);                                     \
        vector_reserve_capacity(T, dest, source->count);            \
        vector_foreach(T, source, __##copy_func##_item, {           \
            vector_push(T, dest, copy_func(__##copy_func##_item));  \
        });                                                         \
    } else {                                                        \
        dest = NULL;                                                \
    }                                                               \
} while(0)

#define vector_deep_free(T, vec, free_func) do {                                    \
    vector_foreach(T, vec, __##free_func##_item, free_func(__##free_func##_item));  \
    vector_free(T, vec);                                                            \
} while(0)

#define vector_deep_append(T, dest, source, copy_func) do {         \
    uint32_t __##copy_func##_count = vector_count(source);          \
    if (__##copy_func##_count) {                                    \
        vector_ensure(T, dest);                                     \
        vector_expand(T, dest, __##copy_func##_count);              \
        vector_foreach(T, source, __##copy_func##_item, {           \
            vector_push(T, dest, copy_func(__##copy_func##_item));  \
        });                                                         \
    }                                                               \
} while(0)

#define vector_deep_remove_all(T, vec, free_func) do {                              \
    vector_foreach(T, vec, __##free_func##_item, free_func(__##free_func##_item));  \
    vector_remove_all(UniversalRole, vec);                                          \
} while(0)

#define vector_first_index_where(T, vec, idx_var, bool_exp) do {    \
    idx_var = VECTOR_INDEX_NOT_FOUND;                               \
    vector_iterate(T, vec, _vec_item, __i_##idx_var, {              \
        if ((bool_exp)) {                                           \
            idx_var = __i_##idx_var;                                \
            break;                                                  \
        }                                                           \
    });                                                             \
} while(0)

#define vector_contains_where(T, vec, out_var, bool_exp) do {   \
    out_var = false;                                            \
    vector_foreach(T, vec, _vec_item, {                         \
        if ((bool_exp)) {                                       \
            out_var = true;                                     \
            break;                                              \
        }                                                       \
    });                                                         \
} while(0)

#define vector_remove_first_where(T, vec, bool_exp) \
    vector_remove_and_free_first_where(T, vec, bool_exp, (void))

#define vector_remove_and_free_first_where(T, vec, bool_exp, free_func)     \
    vector_iterate(T, vec, _vec_item, __i_remove, {                         \
        if ((bool_exp)) {                                                   \
            vector_remove_at(T, vec, __i_remove);                           \
            free_func(_vec_item);                                           \
            break;                                                          \
        }                                                                   \
    })

#define vector_remove_where(T, vec, bool_exp) \
    vector_remove_and_free_where(T, vec, bool_exp, (void))

#define vector_remove_and_free_where(T, vec, bool_exp, free_func)   \
    vector_iterate_reverse(T, vec, _vec_item, __i_remove, {         \
        if ((bool_exp)) {                                           \
            vector_remove_at(T, vec, __i_remove);                   \
            free_func(item_name);                                   \
        }                                                           \
    })

#endif /* vector_h */
