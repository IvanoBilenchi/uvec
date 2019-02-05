/** @file
 * Vector(T) - a type-safe, generic C vector.
 *
 * @see test.c for usage examples.
 * @author Ivano Bilenchi
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma mark - Constants
/// @name Constants

/// Index returned by find-like functions when a matching element cannot be found.
#define VECTOR_INDEX_NOT_FOUND UINT32_MAX

/// Cache line size (B).
#define __VECTOR_CACHE_LINE_SIZE 64

/// Quicksort stack size.
#define __VECTOR_SORT_STACK_SIZE 64

#pragma mark - Private API and Implementation
/// @name Private

/// Concatenates the 'a' and 'b' tokens, allowing 'a' and 'b' to be macro-expanded.
#define __MACRO_CONCAT(a, b) __MACRO_CONCAT_INNER(a, b)
#define __MACRO_CONCAT_INNER(a, b) a##b

/// Cross-platform 'inline' specifier.
#ifndef __vector_inline
    #ifdef _MSC_VER
        #define __vector_inline __inline
    #else
        #define __vector_inline inline
    #endif
#endif /* __vector_inline */

/// Cross-platform 'unused' directive.
#ifndef __vector_unused
    #if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
        #define __vector_unused __attribute__ ((__unused__))
    #else
        #define __vector_unused
    #endif
#endif /* __vector_unused */

/// Specifier for static inline definitions.
#define __vector_static_inline static __vector_inline __vector_unused

/**
 * Rounds x to the next power of 2.
 *
 * @param x 32 bit integer to round.
 */
#define __vector_int32_next_power_2(x) \
    (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

/**
 * Expands the vector if the allocated slots have all been filled up.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 */
#define __vector_expand_if_required(T, vec) do {                                                    \
    if ((vec)->count == (vec)->allocated) {                                                         \
        (vec)->allocated = (vec)->allocated ? (vec)->allocated<<1 : 2;                              \
        (vec)->storage = realloc((vec)->storage, sizeof(T) * (vec)->allocated);                     \
    }                                                                                               \
} while(0)

/**
 * Identity macro.
 *
 * @param a LHS of the identity.
 * @param b RHS of the identity.
 * @return a == b
 */
#define __vector_identical(a, b) ((a) == (b))

/**
 * "Less than" comparison macro.
 *
 * @param a LHS of the comparison.
 * @param b RHS of the comparison.
 * @return a < b
 */
#define __vector_less_than(a, b) ((a) < (b))

/**
 * Defines a new vector struct.
 *
 * @param T Vector type.
 */
#define __VECTOR_DEF_TYPE(T) \
    typedef struct Vector_##T { uint32_t count, allocated; T *storage; } Vector_##T;

/**
 * Generates function declarations for the specified vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the declarations.
 */
#define __VECTOR_DECL(T, SCOPE)                                                                     \
    SCOPE Vector_##T* vector_alloc_##T(void);                                                       \
    SCOPE void vector_free_##T(Vector_##T *vector);                                                 \
    SCOPE void vector_reserve_capacity_##T(Vector_##T *vector, uint32_t capacity);                  \
    SCOPE void vector_append_array_##T(Vector_##T *vector, T const *array, uint32_t n);             \
    SCOPE Vector_##T* vector_copy_##T(Vector_##T const *vector);                                    \
    SCOPE void vector_shrink_##T(Vector_##T *vector);                                               \
    SCOPE void vector_push_##T(Vector_##T *vector, T item);                                         \
    SCOPE T vector_pop_##T(Vector_##T *vector);                                                     \
    SCOPE void vector_remove_at_##T(Vector_##T *vector, uint32_t idx);                              \
    SCOPE void vector_insert_at_##T(Vector_##T *vector, uint32_t idx, T item);                      \
    SCOPE void vector_remove_all_##T(Vector_##T *vector);                                           \
    SCOPE void vector_reverse_##T(Vector_##T *vector);

/**
 * Generates function declarations for the specified equatable vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the declarations.
 */
#define __VECTOR_DECL_EQUATABLE(T, SCOPE)                                                           \
    SCOPE uint32_t vector_index_of_##T(Vector_##T *vector, T item);                                 \
    SCOPE uint32_t vector_index_of_reverse_##T(Vector_##T *vector, T item);                         \
    SCOPE bool vector_push_unique_##T(Vector_##T *vector, T item);                                  \
    SCOPE bool vector_remove_##T(Vector_##T *vector, T item);                                       \
    SCOPE bool vector_equals_##T(Vector_##T *vector, Vector_##T *other);                            \
    SCOPE bool vector_contains_all_##T(Vector_##T *vector, Vector_##T *other);                      \
    SCOPE bool vector_contains_any_##T(Vector_##T *vector, Vector_##T *other);

/**
 * Generates function declarations for the specified comparable vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the declarations.
 */
#define __VECTOR_DECL_COMPARABLE(T, SCOPE)                                                          \
    SCOPE uint32_t vector_index_of_min_##T(Vector_##T const *vec);                                  \
    SCOPE uint32_t vector_index_of_max_##T(Vector_##T const *vec);                                  \
    SCOPE void vector_sort_##T(Vector_##T *vec);                                                    \
    SCOPE uint32_t vector_insertion_index_sorted_##T(Vector_##T const *vec, T item);                \
    SCOPE uint32_t vector_index_of_sorted_##T(Vector_##T const *vec, T item);                       \
    SCOPE uint32_t vector_insert_sorted_##T(Vector_##T *vec, T item);                               \
    SCOPE uint32_t vector_insertion_index_sorted_unique_##T(Vector_##T const *vec, T item);         \
    SCOPE uint32_t vector_insert_sorted_unique_##T(Vector_##T *vec, T item);


/**
 * Generates function definitions for the specified vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the definitions.
 */
#define __VECTOR_IMPL(T, SCOPE)                                                                     \
                                                                                                    \
    SCOPE Vector_##T* vector_alloc_##T(void) {                                                      \
        return calloc(1, sizeof(Vector_##T));                                                       \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_free_##T(Vector_##T *vector) {                                                \
        if (!vector) return;                                                                        \
        if (vector->allocated) free(vector->storage);                                               \
        free(vector);                                                                               \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_reserve_capacity_##T(Vector_##T *vector, uint32_t capacity) {                 \
        if (vector->allocated < capacity) {                                                         \
            __vector_int32_next_power_2(capacity);                                                  \
            vector->allocated = capacity;                                                           \
            vector->storage = realloc(vector->storage, sizeof(T) * capacity);                       \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_append_array_##T(Vector_##T *vector, T const *array, uint32_t n) {            \
        if (!(n && array)) return;                                                                  \
                                                                                                    \
        uint32_t old_count = vector->count;                                                         \
        uint32_t new_count = old_count + n;                                                         \
                                                                                                    \
        vector->count = new_count;                                                                  \
        vector_reserve_capacity_##T(vector, new_count);                                             \
        memcpy(&(vector->storage[old_count]), array, n * sizeof(T));                                \
    }                                                                                               \
                                                                                                    \
    SCOPE Vector_##T* vector_copy_##T(Vector_##T const *vector) {                                   \
        Vector_##T* copy = vector_alloc_##T();                                                      \
        vector_append_array_##T(copy, vector->storage, vector->count);                              \
        return copy;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_shrink_##T(Vector_##T *vector) {                                              \
        uint32_t new_allocated = vector->count;                                                     \
                                                                                                    \
        if (new_allocated) {                                                                        \
            __vector_int32_next_power_2(new_allocated);                                             \
                                                                                                    \
            if (new_allocated < vector->allocated) {                                                \
                vector->allocated = new_allocated;                                                  \
                vector->storage = realloc(vector->storage, sizeof(T) * new_allocated);              \
            }                                                                                       \
        } else {                                                                                    \
            free(vector->storage);                                                                  \
            vector->allocated = 0;                                                                  \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_push_##T(Vector_##T *vector, T item) {                                        \
        __vector_expand_if_required(T, vector);                                                     \
        vector->storage[vector->count++] = item;                                                    \
    }                                                                                               \
                                                                                                    \
    SCOPE T vector_pop_##T(Vector_##T *vector) {                                                    \
        return vector->storage[--vector->count];                                                    \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_remove_at_##T(Vector_##T *vector, uint32_t idx) {                             \
        uint32_t count = vector->count;                                                             \
                                                                                                    \
        if (idx < count - 1) {                                                                      \
            size_t block_size = (count - idx - 1) * sizeof(T);                                      \
            memmove(&(vector->storage[idx]), &(vector->storage[idx + 1]), block_size);              \
        }                                                                                           \
                                                                                                    \
        vector->count--;                                                                            \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_insert_at_##T(Vector_##T *vector, uint32_t idx, T item) {                     \
        __vector_expand_if_required(T, vector);                                                     \
                                                                                                    \
        if (idx < vector->count) {                                                                  \
            size_t block_size = (vector->count - idx) * sizeof(T);                                  \
            memmove(&(vector->storage[idx + 1]), &(vector->storage[idx]), block_size);              \
        }                                                                                           \
                                                                                                    \
        vector->storage[idx] = item;                                                                \
        vector->count++;                                                                            \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_remove_all_##T(Vector_##T *vector) {                                          \
        vector->count = 0;                                                                          \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_reverse_##T(Vector_##T *vector) {                                             \
        uint32_t count = vector->count;                                                             \
                                                                                                    \
        for (uint32_t i = 0; i < count / 2; ++i) {                                                  \
            T temp = vector->storage[i];                                                            \
            uint32_t swap_idx = count - i - 1;                                                      \
                                                                                                    \
            vector->storage[i] = vector->storage[swap_idx];                                         \
            vector->storage[swap_idx] = temp;                                                       \
        }                                                                                           \
    }

/**
 * Generates function definitions for the specified equatable vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the definitions.
 * @param __equal_func Equality function: (T, T) -> bool
 * @param equal_func_is_identity If true, generated code is optimized assuming __equal_func is ==.
 */
#define __VECTOR_IMPL_EQUATABLE(T, SCOPE, __equal_func, equal_func_is_identity)                     \
                                                                                                    \
    SCOPE uint32_t vector_index_of_##T(Vector_##T *vector, T item) {                                \
        for (uint32_t i = 0; i < vector->count; ++i) {                                              \
            if (__equal_func(vector->storage[i], item)) return i;                                   \
        }                                                                                           \
        return VECTOR_INDEX_NOT_FOUND;                                                              \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_index_of_reverse_##T(Vector_##T *vector, T item) {                        \
        for (uint32_t i = vector->count; i-- != 0;) {                                               \
            if (__equal_func(vector->storage[i], item)) return i;                                   \
        }                                                                                           \
        return VECTOR_INDEX_NOT_FOUND;                                                              \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_push_unique_##T(Vector_##T *vector, T item) {                                 \
        bool insert = vector_index_of_##T(vector, item) == VECTOR_INDEX_NOT_FOUND;                  \
        if (insert) vector_push_##T(vector, item);                                                  \
        return insert;                                                                              \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_remove_##T(Vector_##T *vector, T item) {                                      \
        uint32_t idx = vector_index_of_##T(vector, item);                                           \
                                                                                                    \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                        \
            vector_remove_at_##T(vector, idx);                                                      \
            return true;                                                                            \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_equals_##T(Vector_##T *vector, Vector_##T *other) {                           \
        if (vector == other) return true;                                                           \
        if (vector->count != other->count) return false;                                            \
        if (!vector->count) return true;                                                            \
                                                                                                    \
        if (equal_func_is_identity)                                                                 \
            return memcmp(vector->storage, other->storage, vector->count * sizeof(T)) == 0;         \
                                                                                                    \
        for (uint32_t i = 0; i < vector->count; ++i) {                                              \
            if (!__equal_func(vector->storage[i], other->storage[i]))                               \
                return false;                                                                       \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_contains_all_##T(Vector_##T *vector, Vector_##T *other) {                     \
        if (vector == other) return true;                                                           \
                                                                                                    \
        for (uint32_t i = 0; i < other->count; ++i) {                                               \
            if (vector_index_of_##T(vector, other->storage[i]) == VECTOR_INDEX_NOT_FOUND)           \
                return false;                                                                       \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_contains_any_##T(Vector_##T *vector, Vector_##T *other) {                     \
        if (vector == other) return true;                                                           \
                                                                                                    \
        for (uint32_t i = 0; i < other->count; ++i) {                                               \
            if (vector_index_of_##T(vector, other->storage[i]) != VECTOR_INDEX_NOT_FOUND)           \
                return true;                                                                        \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }

/**
 * Generates function definitions for the specified comparable vector type.
 *
 * @param T Vector type.
 * @param SCOPE Scope of the definitions.
 * @param __equal_func Equality function: (T, T) -> bool
 * @param __compare_func Comparison function: (T, T) -> bool
 */
#define __VECTOR_IMPL_COMPARABLE(T, SCOPE, __equal_func, __compare_func)                            \
                                                                                                    \
    SCOPE uint32_t vector_index_of_min_##T(Vector_##T const *vec) {                                 \
        if (!vec->count) return VECTOR_INDEX_NOT_FOUND;                                             \
                                                                                                    \
        uint32_t min_idx = 0;                                                                       \
                                                                                                    \
        for (uint32_t i = 1; i < vec->count; ++i) {                                                 \
            if (__compare_func(vec->storage[i], vec->storage[min_idx])) {                           \
                min_idx = i;                                                                        \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        return min_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_index_of_max_##T(Vector_##T const *vec) {                                 \
        if (!vec->count) return VECTOR_INDEX_NOT_FOUND;                                             \
                                                                                                    \
        uint32_t max_idx = 0;                                                                       \
                                                                                                    \
        for (uint32_t i = 1; i < vec->count; ++i) {                                                 \
            if (__compare_func(vec->storage[max_idx], vec->storage[i])) {                           \
                max_idx = i;                                                                        \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        return max_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_sort_##T(Vector_##T *vec) {                                                   \
        T *array = vec->storage;                                                                    \
        uint32_t len = vec->count, left = 0, pos = 0, seed = rand();                                \
        uint32_t stack[__VECTOR_SORT_STACK_SIZE];                                                   \
                                                                                                    \
        while (true) {                                                                              \
            for (; left + 1 < len; ++len) {                                                         \
                if (pos == __VECTOR_SORT_STACK_SIZE) len = stack[pos = 0];                          \
                                                                                                    \
                T pivot = array[left + seed % (len - left)];                                        \
                seed = seed * 69069 + 1;                                                            \
                stack[pos++] = len;                                                                 \
                                                                                                    \
                for (uint32_t right = left - 1;;) {                                                 \
                    for (++right; __compare_func(array[right], pivot); ++right);                    \
                    for (--len; __compare_func(pivot, array[len]); --len);                          \
                    if (right >= len) break;                                                        \
                                                                                                    \
                    T temp = array[right];                                                          \
                    array[right] = array[len];                                                      \
                    array[len] = temp;                                                              \
                }                                                                                   \
            }                                                                                       \
                                                                                                    \
            if (pos == 0) break;                                                                    \
            left = len;                                                                             \
            len = stack[--pos];                                                                     \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_insertion_index_sorted_##T(Vector_##T const *vec, T item) {               \
        T const *array = vec->storage;                                                              \
        uint32_t const linear_search_thresh = __VECTOR_CACHE_LINE_SIZE / sizeof(T);                 \
        int64_t r = (int64_t)vec->count - 1;                                                        \
        uint32_t l = 0;                                                                             \
                                                                                                    \
        while (r - l > linear_search_thresh) {                                                      \
            int64_t m = l + (r - l) / 2;                                                            \
            T middle = array[m];                                                                    \
                                                                                                    \
            if (__equal_func(middle, item))                                                         \
                return m;                                                                           \
                                                                                                    \
            if (__compare_func(middle, item))                                                       \
                l = m + 1;                                                                          \
            else                                                                                    \
                r = m - 1;                                                                          \
        }                                                                                           \
                                                                                                    \
        for (; l <= r; ++l) {                                                                       \
            T current = array[l];                                                                   \
            if (!__compare_func(current, item)) return l;                                           \
        }                                                                                           \
                                                                                                    \
        return l;                                                                                   \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_index_of_sorted_##T(Vector_##T const *vec, T item) {                      \
        uint32_t idx = vector_insertion_index_sorted_##T(vec, item);                                \
        return __equal_func(vec->storage[idx], item) ? idx : VECTOR_INDEX_NOT_FOUND;                \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_insert_sorted_##T(Vector_##T *vec, T item) {                              \
        uint32_t idx = vector_insertion_index_sorted_##T(vec, item);                                \
        vector_insert_at_##T(vec, idx, item);                                                       \
        return idx;                                                                                 \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_insertion_index_sorted_unique_##T(Vector_##T const *vec, T item) {        \
        uint32_t idx = vector_insertion_index_sorted_##T(vec, item);                                \
        return __equal_func(vec->storage[idx], item) ? VECTOR_INDEX_NOT_FOUND : idx;                \
    }                                                                                               \
                                                                                                    \
    SCOPE uint32_t vector_insert_sorted_unique_##T(Vector_##T *vec, T item) {                       \
        uint32_t idx = vector_insertion_index_sorted_unique_##T(vec, item);                         \
        if (idx != VECTOR_INDEX_NOT_FOUND) vector_insert_at_##T(vec, idx, item);                    \
        return idx;                                                                                 \
    }

#pragma mark - Public API

/// @name Type definitions
#pragma mark - Type definitions

/**
 * Declares a new vector type.
 *
 * @param T Vector type.
 */
#define VECTOR_DECL(T)                                                                              \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)

/**
 * Declares a new equatable vector type.
 *
 * @param T Vector type.
 */
#define VECTOR_DECL_EQUATABLE(T)                                                                    \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)                                                               \
    __VECTOR_DECL_EQUATABLE(T, __vector_unused)

/**
 * Declares a new comparable vector type.
 *
 * @param T Vector type.
 */
#define VECTOR_DECL_COMPARABLE(T)                                                                   \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)                                                               \
    __VECTOR_DECL_EQUATABLE(T, __vector_unused)                                                     \
    __VECTOR_DECL_COMPARABLE(T, __vector_unused)

/**
 * Implements a previously declared vector type.
 *
 * @param T Vector type.
 */
#define VECTOR_IMPL(T) \
    __VECTOR_IMPL(T, __vector_unused)

/**
 * Implements a previously declared equatable vector type.
 * Elements of an equatable vector can be checked for equality via __equal_func.
 *
 * @param T Vector type.
 * @param __equal_func Equality function: (T, T) -> bool
 */
#define VECTOR_IMPL_EQUATABLE(T, __equal_func)                                                      \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __equal_func, 0)

/**
 * Implements a previously declared comparable vector type.
 * Elements of a comparable vector can be checked for equality via __equal_func
 * and ordered via __compare_func.
 *
 * @param T Vector type.
 * @param __equal_func Equality function: (T, T) -> bool
 * @param __compare_func Comparison function: (T, T) -> bool (True if LHS is smaller than RHS)
 */
#define VECTOR_IMPL_COMPARABLE(T, __equal_func, __compare_func)                                     \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __equal_func, 0)                                    \
    __VECTOR_IMPL_COMPARABLE(T, __vector_unused, __equal_func, __compare_func)

/**
 * Implements a previously declared comparable vector type
 * whose elements can be checked for equality via == and compared via <.
 *
 * @param T Vector type.
 */
#define VECTOR_IMPL_IDENTIFIABLE(T)                                                                 \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __vector_identical, 1)                              \
    __VECTOR_IMPL_COMPARABLE(T, __vector_unused, __vector_identical, __vector_less_than)

/**
 * Defines a new static vector type.
 *
 * @param T Vector type.
 */
#define VECTOR_INIT(T)                                                                              \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)

/**
 * Defines a new static equatable vector type.
 *
 * @param T Vector type.
 * @param __equal_func Equality function: (T, T) -> bool
 */
#define VECTOR_INIT_EQUATABLE(T, __equal_func)                                                      \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)                                                        \
    __VECTOR_IMPL_EQUATABLE(T, __vector_static_inline, __equal_func, 0)

/**
 * Defines a new static comparable vector type.
 *
 * @param T Vector type.
 * @param __equal_func Equality function: (T, T) -> bool
 * @param __compare_func Comparison function: (T, T) -> bool (True if LHS is smaller than RHS)
 */
#define VECTOR_INIT_COMPARABLE(T, __equal_func, __compare_func)                                     \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)                                                        \
    __VECTOR_IMPL_EQUATABLE(T, __vector_static_inline, __equal_func, 0)                             \
    __VECTOR_IMPL_COMPARABLE(T, __vector_static_inline, __equal_func, __compare_func)

/**
 * Defines a new static equatable vector type
 * whose elements can be checked for equality via == and compared via <.
 *
 * @param T Vector type.
 */
#define VECTOR_INIT_IDENTIFIABLE(T)                                                                 \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)                                                        \
    __VECTOR_IMPL_EQUATABLE(T, __vector_static_inline, __vector_identical, 1)                       \
    __VECTOR_IMPL_COMPARABLE(T, __vector_static_inline, __vector_identical, __vector_less_than)

/// @name Declaration
#pragma mark - Declaration

/**
 * Declares a new vector variable.
 *
 * @param T Vector type.
 */
#define Vector(T) __MACRO_CONCAT(Vector_, T)

/**
 * Expands to 'struct Vector_T', useful for forward-declarations.
 *
 * @param T Vector type.
 */
#define vector_struct(T) struct __MACRO_CONCAT(Vector_, T)

/// @name Memory management
#pragma mark - Memory management

/**
 * Allocates a new vector.
 *
 * @param T Vector type.
 * @return Vector instance.
 */
#define vector_alloc(T) __MACRO_CONCAT(vector_alloc_, T)()

/**
 * Ensures the variable 'vec' expands to is not NULL,
 * otherwise a new vector is allocated and assigned to 'vec'.
 *
 * @param T Vector type.
 * @param vec Vector variable.
 * @return Variable 'vec' expands to.
 */
#define vector_ensure(T, vec) (vec ? vec : (vec = vector_alloc(T)))

/**
 * Deallocates the specified vector.
 *
 * @param T Vector type.
 * @param vec Vector to free.
 */
#define vector_free(T, vec) __MACRO_CONCAT(vector_free_, T)(vec)

/**
 * Copies the specified vector.
 *
 * @param T Vector type.
 * @param vec Vector to copy.
 * @return Copied vector instance.
 */
#define vector_copy(T, vec) __MACRO_CONCAT(vector_copy_, T)(vec)

/**
 * Initializes a new vector on the stack.
 *
 * @param T Vector type.
 * @return Initialized vector instance.
 */
#define vector_init(T) \
    ((__MACRO_CONCAT(Vector_, T)){ .count = 0, .allocated = 0, .storage = NULL })

/**
 * De-initializes a vector previously initialized via VECTOR_INIT.
 *
 * @param vec Vector to de-initialize.
 */
#define vector_deinit(vec) do {                                                                     \
    if ((vec).storage) {                                                                            \
        free((vec).storage);                                                                        \
        (vec).storage = NULL;                                                                       \
    }                                                                                               \
    (vec).count = (vec).allocated = 0;                                                              \
} while(0)

/**
 * Ensures the specified vector can hold at least as many elements as 'size'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param size Number of elements the vector should be able to hold.
 */
#define vector_reserve_capacity(T, vec, size) \
    __MACRO_CONCAT(vector_reserve_capacity_, T)(vec, size)

/**
 * Expands the specified vector so that it can contain additional 'size' elements.
 *
 * @param T Vector type.
 * @param vec Vector to expand.
 * @param size Number of additional elements the vector should be able to hold.
 */
#define vector_expand(T, vec, size) \
    __MACRO_CONCAT(vector_reserve_capacity_, T)(vec, (vec->count + size))

/**
 * Shrinks the specified vector so that its allocated size
 * exactly matches the number of elements it contains.
 *
 * @param T Vector type.
 * @param vec Vector to shrink.
 */
#define vector_shrink(T, vec) __MACRO_CONCAT(vector_shrink_, T)(vec)

/// @name Primitives
#pragma mark - Primitives

/**
 * Retrieves the element at the specified index.
 *
 * @param vec Vector instance.
 * @param idx Index.
 * @return Element at the specified index.
 */
#define vector_get(vec, idx) ((vec)->storage[(idx)])

/**
 * Replaces the element at the specified index.
 *
 * @param vec Vector instance.
 * @param idx Index.
 * @param item Replacement element.
 */
#define vector_set(vec, idx, item) ((vec)->storage[(idx)] = (item))

/**
 * Returns the first element in the vector.
 *
 * @param vec Vector instance.
 * @return First element.
 */
#define vector_first(vec) ((vec)->storage[0])

/**
 * Returns the last element in the vector.
 *
 * @param vec Vector instance.
 * @return Last element.
 */
#define vector_last(vec) ((vec)->storage[(vec)->count-1])

/**
 * Checks whether the specified vector is empty.
 *
 * @param vec Vector instance.
 * @return True if the vector is empty, false otherwise.
 *
 * @note For convenience, this macro returns 'true' for NULL vectors.
 */
#define vector_is_empty(vec) (!((vec) && (vec)->count))

/**
 * Returns the number of elements in the vector.
 *
 * @param vec Vector instance.
 * @return Number of elements.
 *
 * @note For convenience, this macro returns '0' for NULL vectors.
 */
#define vector_count(vec) ((vec) ? (vec)->count : 0)

/**
 * Pushes the specified element to the top of the vector (last element).
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to push.
 */
#define vector_push(T, vec, item) __MACRO_CONCAT(vector_push_, T)(vec, item)

/**
 * Removes and returns the element at the top of the vector (last element).
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @return Last element.
 */
#define vector_pop(T, vec) __MACRO_CONCAT(vector_pop_, T)(vec)

/**
 * Removes the element at the specified index.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param idx Index of the element to remove.
 */
#define vector_remove_at(T, vec, idx) __MACRO_CONCAT(vector_remove_at_, T)(vec, idx)

/**
 * Inserts an element at the specified index.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param idx Index at which the element should be inserted.
 * @param item Element to insert.
 */
#define vector_insert_at(T, vec, idx, item) __MACRO_CONCAT(vector_insert_at_, T)(vec, idx, item)

/**
 * Removes all the elements in the vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 */
#define vector_remove_all(T, vec) __MACRO_CONCAT(vector_remove_all_, T)(vec)

/**
 * Appends a vector to another.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param vec_to_append Vector to append.
 */
#define vector_append(T, vec, vec_to_append) \
    __MACRO_CONCAT(vector_append_array_, T)(vec, (vec_to_append)->storage, (vec_to_append)->count)

/**
 * Appends an array to the specified vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param array Array to append.
 * @param n Number of elements to append.
 */
#define vector_append_array(T, vec, array, n) \
    __MACRO_CONCAT(vector_append_array_, T)(vec, array, n)

/**
 * Appends multiple items to the specified vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param ... Elements to append.
 */
#define vector_append_items(T, vec, ...) do {                                                       \
    T const __vec_##T##_init[] = { __VA_ARGS__ };                                                   \
    size_t __vec_##T##_init_size = sizeof(__vec_##T##_init) / sizeof(*__vec_##T##_init);            \
    vector_append_array(T, vec, __vec_##T##_init, __vec_##T##_init_size);                           \
} while(0)

/**
 * 'vector_ensure' followed by 'vector_push'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to push.
 */
#define vector_push_lazy(T, vec, item) \
    do { vector_ensure(T, vec); vector_push(T, vec, item); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param vec_to_append Vector to append.
 */
#define vector_append_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append(T, vec, vec_to_append); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append_array'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param array Array to append.
 * @param n Number of elements to append.
 */
#define vector_append_array_lazy(T, vec, array, n) \
    do { vector_ensure(T, vec); vector_append_array(T, vec, array, n); } while(0)

/**
 * Reverses the vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 */
#define vector_reverse(T, vec) __MACRO_CONCAT(vector_reverse_, T)(vec)

/// @name Iteration
#pragma mark - Iteration

/**
 * Iterates over the vector, executing the specified code block for each element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item_name Name of the element variable.
 * @param idx_name Name of the index variable.
 * @param code Code block to execute.
 */
#define vector_iterate(T, vec, item_name, idx_name, code)                                           \
    if (vec) {                                                                                      \
        uint32_t __n_##idx_name = (vec)->count;                                                     \
        for (uint32_t idx_name = 0; idx_name != __n_##idx_name; ++idx_name) {                       \
            T item_name = vector_get((vec), (idx_name));                                            \
            code;                                                                                   \
        }                                                                                           \
    }

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item_name Name of the element variable.
 * @param idx_name Name of the index variable.
 * @param code Code block to execute.
 */
#define vector_iterate_reverse(T, vec, item_name, idx_name, code)                                   \
    if (vec) {                                                                                      \
        for (uint32_t idx_name = (vec)->count; idx_name-- != 0;) {                                  \
            T item_name = vector_get((vec), (idx_name));                                            \
            code;                                                                                   \
        }                                                                                           \
    }

/**
 * Iterates over the vector,
 * executing the specified code block for each element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item_name Name of the element variable.
 * @param code Code block to execute.
 */
#define vector_foreach(T, vec, item_name, code) \
    vector_iterate(T, vec, item_name, __i_##item_name, code)

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item_name Name of the element variable.
 * @param code Code block to execute.
 */
#define vector_foreach_reverse(T, vec, item_name, code) \
    vector_iterate_reverse(T, vec, item_name, __i_##item_name, code)

/// @name Equatable
#pragma mark - Equatable

/**
 * Returns the index of the first occurrence of the specified element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to search.
 * @return Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 */
#define vector_index_of(T, vec, item) __MACRO_CONCAT(vector_index_of_, T)(vec, item)

/**
 * Returns the index of the last occurrence of the specified element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to search.
 * @return Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 */
#define vector_index_of_reverse(T, vec, item) \
    __MACRO_CONCAT(vector_index_of_reverse_, T)(vec, item)

/**
 * Checks whether the vector contains the specified element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to search.
 * @return True if the vector contains the specified element, false otherwise.
 */
#define vector_contains(T, vec, item) \
    (__MACRO_CONCAT(vector_index_of_, T)(vec, item) != VECTOR_INDEX_NOT_FOUND)

/**
 * Checks whether the vector contains all the elements present in another vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param other_vec Vector containing the elements to search.
 * @return True if the vector contains all the specified elements, false otherwise.
 */
#define vector_contains_all(T, vec, other_vec) \
    __MACRO_CONCAT(vector_contains_all_, T)(vec, other_vec)

/**
 * Checks whether the vector contains any of the elements contained in another vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param other_vec Vector containing the elements to search.
 * @return True if the vector contains any of the specified elements, false otherwise.
 */
#define vector_contains_any(T, vec, other_vec) \
    __MACRO_CONCAT(vector_contains_any_, T)(vec, other_vec)

/**
 * Removes the specified element.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to remove.
 * @return True if the element was found and removed, false otherwise.
 */
#define vector_remove(T, vec, item) __MACRO_CONCAT(vector_remove_, T)(vec, item)

/**
 * Checks whether the two vectors are equal.
 * Two vectors are considered equal if they contain the same elements in the same order.
 *
 * @param T Vector type.
 * @param vec_a First vector.
 * @param vec_b Second vector.
 * @return True if the vectors are equal, false otherwise.
 */
#define vector_equals(T, vec_a, vec_b) __MACRO_CONCAT(vector_equals_, T)(vec_a, vec_b)

/**
 * Pushes an element if it is not already in the vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to push.
 * @return True if the element was pushed, false otherwise.
 */
#define vector_push_unique(T, vec, item) __MACRO_CONCAT(vector_push_unique_, T)(vec, item)

/**
 * Pushes all the elements present in 'vec_to_append' that are not already present in 'vec'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param vec_to_append Vector containing the elements to append.
 */
#define vector_append_unique(T, vec, vec_to_append)                                                 \
    vector_foreach(T, vec_to_append, __item, {                                                      \
        __MACRO_CONCAT(vector_push_unique_, T)(vec, __item);                                        \
    })

/**
 * Removes all the elements present in 'vec_to_remove' from 'vec'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param vec_to_remove Vector containing the elements to remove.
 */
#define vector_remove_all_from(T, vec, vec_to_remove)                                               \
    vector_foreach(T, vec_to_remove, __item, {                                                      \
        __MACRO_CONCAT(vector_remove_, T)(vec, __item);                                             \
    })

/**
 * 'vector_ensure' followed by 'vector_push_unique'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to push.
 */
#define vector_push_unique_lazy(T, vec, item) \
    do { vector_ensure(T, vec); vector_push_unique(T, vec, item); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append_unique'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param vec_to_append Vector containing the elements to append.
 */
#define vector_append_unique_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append_unique(T, vec, vec_to_append); } while(0)

/// @name Comparable
#pragma mark - Comparable

/**
 * Returns the index of the minimum element in the vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @return Index of the minimum element.
 */
#define vector_index_of_min(T, vec) __MACRO_CONCAT(vector_index_of_min_, T)(vec)

/**
 * Returns the index of the maximum element in the vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @return Index of the maximum element.
 */
#define vector_index_of_max(T, vec) __MACRO_CONCAT(vector_index_of_max_, T)(vec)

/**
 * Sorts the vector.
 * Average performance: O(n log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 */
#define vector_sort(T, vec) __MACRO_CONCAT(vector_sort_, T)(vec)

/**
 * Finds the insertion index for the specified item in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element whose insertion index should be found.
 * @return Insertion index.
 */
#define vector_insertion_index_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_insertion_index_sorted_, T)(vec, item)

/**
 * Returns the index of the specified element in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to search.
 * @return Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 *
 * @note The returned index is not necessarily the first occurrence of the item.
 */
#define vector_index_of_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_index_of_sorted_, T)(vec, item)

/**
 * Checks whether a sorted vector contains the specified element.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to search.
 * @return True if the vector contains the specified element, false otherwise.
 */
#define vector_contains_sorted(T, vec, item) \
    (__MACRO_CONCAT(vector_index_of_sorted_, T)(vec, item) != VECTOR_INDEX_NOT_FOUND)

/**
 * Inserts an element in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to insert.
 * @return Index of the inserted element.
 */
#define vector_insert_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_insert_sorted_, T)(vec, item)

/**
 * Inserts all the elements present in a vector into another sorted vector.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param source Vector containing the elements to insert.
 */
#define vector_insert_all_sorted(T, vec, source)                                                    \
    vector_foreach(T, source, __item, {                                                             \
        __MACRO_CONCAT(vector_insert_sorted_, T)(vec, __item);                                      \
    })

/**
 * Finds the insertion index for the specified item in a sorted vector if it is not already present.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element whose insertion index should be found.
 * @return Insertion index, or VECTOR_INDEX_NOT_FOUND if the element is already present.
 */
#define vector_insertion_index_sorted_unique(T, vec, item) \
    __MACRO_CONCAT(vector_insertion_index_sorted_unique_, T)(vec, item)

/**
 * Inserts an element in a sorted vector if it is not already present.
 * Average performance: O(log n)
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param item Element to insert.
 * @return Index of the inserted element, or VECTOR_INDEX_NOT_FOUND if it is already present.
 */
#define vector_insert_sorted_unique(T, vec, item) \
    __MACRO_CONCAT(vector_insert_sorted_unique_, T)(vec, item)

/**
 * Inserts all the elements present in a vector into another sorted vector
 * if they are not already present.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param source Vector containing the elements to insert.
 */
#define vector_insert_all_sorted_unique(T, vec, source)                                             \
    vector_foreach(T, source, __item, {                                                             \
        __MACRO_CONCAT(vector_insert_sorted_unique_, T)(vec, __item);                               \
    })

/// @name Deep manipulation
#pragma mark - Deep manipulation

/**
 * Performs a "deep copy": a new vector is allocated and assigned to 'dest',
 * then all the elements in 'source' are copied via '__copy_func' and pushed to 'dest'.
 *
 * @param T Vector type.
 * @param dest Destination vector.
 * @param source Source vector.
 * @param __copy_func Copy function: (T) -> T
 */
#define vector_deep_copy(T, dest, source, __copy_func) do {                                         \
    if (source) {                                                                                   \
        dest = vector_alloc(T);                                                                     \
        vector_reserve_capacity(T, dest, source->count);                                            \
        vector_foreach(T, source, __##__copy_func##_item, {                                         \
            vector_push(T, dest, __copy_func(__##__copy_func##_item));                              \
        });                                                                                         \
    } else {                                                                                        \
        dest = NULL;                                                                                \
    }                                                                                               \
} while(0)

/**
 * Performs a "deep free": '__free_func' is called on every
 * vector element before the whole vector is deallocated.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param __free_func Free function: (T) -> void
 */
#define vector_deep_free(T, vec, __free_func) do {                                                  \
    vector_foreach(T, vec, __##__free_func##_item, __free_func(__##__free_func##_item));            \
    vector_free(T, vec);                                                                            \
} while(0)

/**
 * Performs a "deep append": all the elements in 'source' are copied
 * via '__copy_func' and pushed to 'dest'. If 'dest' is NULL, it is allocated beforehand.
 *
 * @param T Vector type.
 * @param dest Destination vector.
 * @param source Source vector.
 * @param __copy_func Copy function: (T) -> T
 */
#define vector_deep_append(T, dest, source, __copy_func) do {                                       \
    uint32_t __##__copy_func##_count = vector_count(source);                                        \
    if (__##__copy_func##_count) {                                                                  \
        vector_ensure(T, dest);                                                                     \
        vector_expand(T, dest, __##__copy_func##_count);                                            \
        vector_foreach(T, source, __##__copy_func##_item, {                                         \
            vector_push(T, dest, __copy_func(__##__copy_func##_item));                              \
        });                                                                                         \
    }                                                                                               \
} while(0)

/**
 * Performs a "deep remove all": all elements are removed and deallocated via '__free_func'.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param __free_func Free function: (T) -> void
 */
#define vector_deep_remove_all(T, vec, __free_func) do {                                            \
    vector_foreach(T, vec, __##__free_func##_item, __free_func(__##__free_func##_item));            \
    vector_remove_all(T, vec);                                                                      \
} while(0)

/// @name Higher order
#pragma mark - Higher order

/**
 * Returns the index of the first element that matches the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param idx_var uint32_t out variable (must be declared in outer scope).
 * @param bool_exp Boolean expression.
 */
#define vector_first_index_where(T, vec, idx_var, bool_exp) do {                                    \
    idx_var = VECTOR_INDEX_NOT_FOUND;                                                               \
    vector_iterate(T, vec, _vec_item, __i_##idx_var, {                                              \
        if ((bool_exp)) {                                                                           \
            idx_var = __i_##idx_var;                                                                \
            break;                                                                                  \
        }                                                                                           \
    });                                                                                             \
} while(0)

/**
 * Checks whether the vector contains an element that matches the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param out_var bool out variable (must be declared in outer scope).
 * @param bool_exp Boolean expression.
 */
#define vector_contains_where(T, vec, out_var, bool_exp) do {                                       \
    out_var = false;                                                                                \
    vector_foreach(T, vec, _vec_item, {                                                             \
        if ((bool_exp)) {                                                                           \
            out_var = true;                                                                         \
            break;                                                                                  \
        }                                                                                           \
    });                                                                                             \
} while(0)

/**
 * Removes the first element that matches the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param bool_exp Boolean expression.
 */
#define vector_remove_first_where(T, vec, bool_exp) \
    vector_remove_and_free_first_where(T, vec, bool_exp, (void))

/**
 * Removes and deallocates the first element that matches the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param bool_exp Boolean expression.
 * @param __free_func Free function: (T) -> void
 */
#define vector_remove_and_free_first_where(T, vec, bool_exp, __free_func)                           \
    vector_iterate(T, vec, _vec_item, __i_remove, {                                                 \
        if ((bool_exp)) {                                                                           \
            vector_remove_at(T, vec, __i_remove);                                                   \
            __free_func(_vec_item);                                                                 \
            break;                                                                                  \
        }                                                                                           \
    })

/**
 * Removes all the elements that match the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param bool_exp Boolean expression.
 */
#define vector_remove_where(T, vec, bool_exp) \
    vector_remove_and_free_where(T, vec, bool_exp, (void))

/**
 * Removes and deallocates all the elements that match the specified boolean expression.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param bool_exp Boolean expression.
 * @param __free_func Free function: (T) -> void
 */
#define vector_remove_and_free_where(T, vec, bool_exp, __free_func)                                 \
    vector_iterate_reverse(T, vec, _vec_item, __i_remove, {                                         \
        if ((bool_exp)) {                                                                           \
            vector_remove_at(T, vec, __i_remove);                                                   \
            __free_func(_vec_item);                                                                 \
        }                                                                                           \
    })

/**
 * Sorts the vector via qsort.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param __comp_func qsort-compatible sorting function.
 *
 * @see qsort
 */
#define vector_qsort(T, vec, __comp_func) \
    vector_qsort_range(T, vec, 0, (vec)->count, __comp_func)

/**
 * Sorts the elements in the specified range via qsort.
 *
 * @param T Vector type.
 * @param vec Vector instance.
 * @param start Range start index.
 * @param len Range length.
 * @param __comp_func qsort-compatible sorting function.
 *
 * @see qsort
 */
#define vector_qsort_range(T, vec, start, len, __comp_func) \
    if (vec) qsort((vec)->storage + start, len, sizeof(T), __comp_func)

#endif // VECTOR_H
