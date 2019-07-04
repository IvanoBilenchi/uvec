/**
 * Vector(T) - a type-safe, generic C vector.
 *
 * @see test.c for usage examples.
 * @author Ivano Bilenchi
 *
 * @copyright Copyright (c) 2018-2019 Ivano Bilenchi <https://ivanobilenchi.com>
 * @copyright SPDX-License-Identifier: MIT
 *
 * @file
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// #########
// # Types #
// #########

/**
 * A type safe, generic vector.
 * @struct Vector
 */

/**
 * Unsigned integer type.
 *
 * @public @memberof Vector
 */
#if defined VECTOR_TINY
    typedef uint16_t vector_uint_t;
#elif defined VECTOR_HUGE
    typedef uint64_t vector_uint_t;
#else
    typedef uint32_t vector_uint_t;
#endif

// #############
// # Constants #
// #############

/// Maximum value of a vector_uint_t variable.
#if defined VECTOR_TINY
    #define VECTOR_UINT_MAX UINT16_MAX
#elif defined VECTOR_HUGE
    #define VECTOR_UINT_MAX UINT64_MAX
#else
    #define VECTOR_UINT_MAX UINT32_MAX
#endif

/// Index returned by find-like functions when a matching element cannot be found.
#define VECTOR_INDEX_NOT_FOUND VECTOR_UINT_MAX

/// Cache line size (B).
#ifndef VECTOR_CACHE_LINE_SIZE
    #define VECTOR_CACHE_LINE_SIZE 64
#endif

/// Quicksort stack size.
#define __VECTOR_SORT_STACK_SIZE 64

// ###############
// # Private API #
// ###############

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
#endif

/// Cross-platform 'unused' directive.
#ifndef __vector_unused
    #if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
        #define __vector_unused __attribute__ ((__unused__))
    #else
        #define __vector_unused
    #endif
#endif

/// Specifier for static inline definitions.
#define __vector_static_inline static __vector_inline __vector_unused

/// Give hints to the static analyzer.
#if (__clang_analyzer__)
    #define __vector_analyzer_assert(c) do { if (!(c)) exit(1); } while(0)
#else
    #define __vector_analyzer_assert(c)
#endif

/**
 * Changes the specified unsigned integer into the next power of two.
 *
 * @param x [vector_uint_t] Unsigned integer.
 */
#if defined VECTOR_TINY
    #define __vector_uint_next_power_2(x) (                                                         \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u,                                     \
        ++(x)                                                                                       \
    )
#elif defined VECTOR_HUGE
    #define __vector_uint_next_power_2(x) (                                                         \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u, (x)|=(x)>>16u, (x)|=(x)>>32u,       \
        ++(x)                                                                                       \
    )
#else
    #define __vector_uint_next_power_2(x) (                                                         \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u, (x)|=(x)>>16u,                      \
        ++(x)                                                                                       \
    )
#endif

/**
 * Expands the vector if the allocated slots have all been filled up.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 */
#define __vector_expand_if_required(T, vec) do {                                                    \
    if ((vec)->count == (vec)->allocated) {                                                         \
        (vec)->allocated = (vec)->allocated ? (vec)->allocated<<1u : 2;                             \
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
 * @param T [symbol] Vector type.
 */
#define __VECTOR_DEF_TYPE(T)                                                                        \
    typedef struct Vector_##T {                                                                     \
        /** @cond */                                                                                \
        vector_uint_t allocated;                                                                    \
        vector_uint_t count;                                                                        \
        T *storage;                                                                                 \
        /** @endcond */                                                                             \
    } Vector_##T;

/**
 * Generates function declarations for the specified vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define __VECTOR_DECL(T, SCOPE)                                                                     \
    /** @cond */                                                                                    \
    SCOPE Vector_##T* vector_alloc_##T(void);                                                       \
    SCOPE void vector_free_##T(Vector_##T *vector);                                                 \
    SCOPE void vector_reserve_capacity_##T(Vector_##T *vector, vector_uint_t capacity);             \
    SCOPE void vector_append_array_##T(Vector_##T *vector, T const *array, vector_uint_t n);        \
    SCOPE Vector_##T* vector_copy_##T(Vector_##T const *vector);                                    \
    SCOPE void vector_shrink_##T(Vector_##T *vector);                                               \
    SCOPE void vector_push_##T(Vector_##T *vector, T item);                                         \
    SCOPE T vector_pop_##T(Vector_##T *vector);                                                     \
    SCOPE void vector_remove_at_##T(Vector_##T *vector, vector_uint_t idx);                         \
    SCOPE void vector_insert_at_##T(Vector_##T *vector, vector_uint_t idx, T item);                 \
    SCOPE void vector_remove_all_##T(Vector_##T *vector);                                           \
    SCOPE void vector_reverse_##T(Vector_##T *vector);                                              \
    /** @endcond */

/**
 * Generates function declarations for the specified equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define __VECTOR_DECL_EQUATABLE(T, SCOPE)                                                           \
    /** @cond */                                                                                    \
    SCOPE vector_uint_t vector_index_of_##T(Vector_##T const *vector, T item);                      \
    SCOPE vector_uint_t vector_index_of_reverse_##T(Vector_##T const *vector, T item);              \
    SCOPE bool vector_push_unique_##T(Vector_##T *vector, T item);                                  \
    SCOPE bool vector_remove_##T(Vector_##T *vector, T item);                                       \
    SCOPE bool vector_equals_##T(Vector_##T const *vector, Vector_##T const *other);                \
    SCOPE bool vector_contains_all_##T(Vector_##T const *vector, Vector_##T const *other);          \
    SCOPE bool vector_contains_any_##T(Vector_##T const *vector, Vector_##T const *other);          \
    /** @endcond */

/**
 * Generates function declarations for the specified comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define __VECTOR_DECL_COMPARABLE(T, SCOPE)                                                          \
    /** @cond */                                                                                    \
    SCOPE vector_uint_t vector_index_of_min_##T(Vector_##T const *vec);                             \
    SCOPE vector_uint_t vector_index_of_max_##T(Vector_##T const *vec);                             \
    SCOPE void vector_sort_range_##T(Vector_##T *vec, vector_uint_t start, vector_uint_t len);      \
    SCOPE vector_uint_t vector_insertion_index_sorted_##T(Vector_##T const *vec, T item);           \
    SCOPE vector_uint_t vector_index_of_sorted_##T(Vector_##T const *vec, T item);                  \
    SCOPE vector_uint_t vector_insert_sorted_##T(Vector_##T *vec, T item);                          \
    SCOPE vector_uint_t vector_insertion_index_sorted_unique_##T(Vector_##T const *vec, T item);    \
    SCOPE vector_uint_t vector_insert_sorted_unique_##T(Vector_##T *vec, T item);                   \
    /** @endcond */

/**
 * Generates function definitions for the specified vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the definitions.
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
    SCOPE void vector_reserve_capacity_##T(Vector_##T *vector, vector_uint_t capacity) {            \
        if (vector->allocated < capacity) {                                                         \
            __vector_uint_next_power_2(capacity);                                                   \
            vector->allocated = capacity;                                                           \
            vector->storage = realloc(vector->storage, sizeof(T) * capacity);                       \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_append_array_##T(Vector_##T *vector, T const *array, vector_uint_t n) {       \
        if (!(n && array)) return;                                                                  \
                                                                                                    \
        vector_uint_t old_count = vector->count;                                                    \
        vector_uint_t new_count = old_count + n;                                                    \
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
        vector_uint_t new_allocated = vector->count;                                                \
                                                                                                    \
        if (new_allocated) {                                                                        \
            __vector_uint_next_power_2(new_allocated);                                              \
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
    SCOPE void vector_remove_at_##T(Vector_##T *vector, vector_uint_t idx) {                        \
        vector_uint_t count = vector->count;                                                        \
                                                                                                    \
        if (idx < count - 1) {                                                                      \
            size_t block_size = (count - idx - 1) * sizeof(T);                                      \
            memmove(&(vector->storage[idx]), &(vector->storage[idx + 1]), block_size);              \
        }                                                                                           \
                                                                                                    \
        vector->count--;                                                                            \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_insert_at_##T(Vector_##T *vector, vector_uint_t idx, T item) {                \
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
        vector_uint_t count = vector->count;                                                        \
                                                                                                    \
        for (vector_uint_t i = 0; i < count / 2; ++i) {                                             \
            T temp = vector->storage[i];                                                            \
            vector_uint_t swap_idx = count - i - 1;                                                 \
            vector->storage[i] = vector->storage[swap_idx];                                         \
            vector->storage[swap_idx] = temp;                                                       \
        }                                                                                           \
    }

/**
 * Generates function definitions for the specified equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the definitions.
 * @param __equal_func [(T, T) -> bool] Equality function.
 * @param equal_func_is_identity [bool] If true, generated code assumes __equal_func is ==.
 */
#define __VECTOR_IMPL_EQUATABLE(T, SCOPE, __equal_func, equal_func_is_identity)                     \
                                                                                                    \
    SCOPE vector_uint_t vector_index_of_##T(Vector_##T const *vector, T item) {                     \
        for (vector_uint_t i = 0; i < vector->count; ++i) {                                         \
            if (__equal_func(vector->storage[i], item)) return i;                                   \
        }                                                                                           \
        return VECTOR_INDEX_NOT_FOUND;                                                              \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_index_of_reverse_##T(Vector_##T const *vector, T item) {             \
        for (vector_uint_t i = vector->count; i-- != 0;) {                                          \
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
        vector_uint_t idx = vector_index_of_##T(vector, item);                                      \
                                                                                                    \
        if (idx != VECTOR_INDEX_NOT_FOUND) {                                                        \
            vector_remove_at_##T(vector, idx);                                                      \
            return true;                                                                            \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_equals_##T(Vector_##T const *vector, Vector_##T const *other) {               \
        if (vector == other) return true;                                                           \
        if (vector->count != other->count) return false;                                            \
        if (!vector->count) return true;                                                            \
                                                                                                    \
        if (equal_func_is_identity)                                                                 \
            return memcmp(vector->storage, other->storage, vector->count * sizeof(T)) == 0;         \
                                                                                                    \
        for (vector_uint_t i = 0; i < vector->count; ++i) {                                         \
            if (!__equal_func(vector->storage[i], other->storage[i]))                               \
                return false;                                                                       \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_contains_all_##T(Vector_##T const *vector, Vector_##T const *other) {         \
        if (vector == other) return true;                                                           \
                                                                                                    \
        for (vector_uint_t i = 0; i < other->count; ++i) {                                          \
            if (vector_index_of_##T(vector, other->storage[i]) == VECTOR_INDEX_NOT_FOUND)           \
                return false;                                                                       \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool vector_contains_any_##T(Vector_##T const *vector, Vector_##T const *other) {         \
        if (vector == other) return true;                                                           \
                                                                                                    \
        for (vector_uint_t i = 0; i < other->count; ++i) {                                          \
            if (vector_index_of_##T(vector, other->storage[i]) != VECTOR_INDEX_NOT_FOUND)           \
                return true;                                                                        \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }

/**
 * Generates function definitions for the specified comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE Scope of the definitions.
 * @param __equal_func Equality function: (T, T) -> bool
 * @param __compare_func Comparison function: (T, T) -> bool
 */
#define __VECTOR_IMPL_COMPARABLE(T, SCOPE, __equal_func, __compare_func)                            \
                                                                                                    \
    SCOPE vector_uint_t vector_index_of_min_##T(Vector_##T const *vec) {                            \
        if (!vec->count) return VECTOR_INDEX_NOT_FOUND;                                             \
                                                                                                    \
        vector_uint_t min_idx = 0;                                                                  \
                                                                                                    \
        for (vector_uint_t i = 1; i < vec->count; ++i) {                                            \
            if (__compare_func(vec->storage[i], vec->storage[min_idx])) {                           \
                min_idx = i;                                                                        \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        return min_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_index_of_max_##T(Vector_##T const *vec) {                            \
        if (!vec->count) return VECTOR_INDEX_NOT_FOUND;                                             \
                                                                                                    \
        vector_uint_t max_idx = 0;                                                                  \
                                                                                                    \
        for (vector_uint_t i = 1; i < vec->count; ++i) {                                            \
            if (__compare_func(vec->storage[max_idx], vec->storage[i])) {                           \
                max_idx = i;                                                                        \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        return max_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE void vector_sort_range_##T(Vector_##T *vec, vector_uint_t start, vector_uint_t len) {     \
        T *array = vec->storage + start;                                                            \
        start = 0;                                                                                  \
        vector_uint_t pos = 0, seed = 31, stack[__VECTOR_SORT_STACK_SIZE];                          \
                                                                                                    \
        while (true) {                                                                              \
            for (; start + 1 < len; ++len) {                                                        \
                if (pos == __VECTOR_SORT_STACK_SIZE) len = stack[pos = 0];                          \
                                                                                                    \
                T pivot = array[start + seed % (len - start)];                                      \
                seed = seed * 69069 + 1;                                                            \
                stack[pos++] = len;                                                                 \
                                                                                                    \
                for (vector_uint_t right = start - 1;;) {                                           \
                    __vector_analyzer_assert(false);                                                \
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
            start = len;                                                                            \
            len = stack[--pos];                                                                     \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_insertion_index_sorted_##T(Vector_##T const *vec, T item) {          \
        T const *array = vec->storage;                                                              \
        vector_uint_t const linear_search_thresh = VECTOR_CACHE_LINE_SIZE / sizeof(T);              \
        vector_uint_t r = vec->count, l = 0;                                                        \
                                                                                                    \
        while (r - l > linear_search_thresh) {                                                      \
            vector_uint_t m = l + (r - l) / 2;                                                      \
                                                                                                    \
            if (__compare_func(array[m], item))                                                     \
                l = m + 1;                                                                          \
            else                                                                                    \
                r = m;                                                                              \
        }                                                                                           \
                                                                                                    \
        for (; l < r && __compare_func(array[l], item); ++l);                                       \
        return l;                                                                                   \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_index_of_sorted_##T(Vector_##T const *vec, T item) {                 \
        vector_uint_t const i = vector_insertion_index_sorted_##T(vec, item);                       \
        return vec->storage && __equal_func(vec->storage[i], item) ? i : VECTOR_INDEX_NOT_FOUND;    \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_insert_sorted_##T(Vector_##T *vec, T item) {                         \
        vector_uint_t const i = vector_insertion_index_sorted_##T(vec, item);                       \
        vector_insert_at_##T(vec, i, item);                                                         \
        return i;                                                                                   \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_insertion_index_sorted_unique_##T(Vector_##T const *vec, T item) {   \
        vector_uint_t const i = vector_insertion_index_sorted_##T(vec, item);                       \
        return vec->storage && __equal_func(vec->storage[i], item) ? VECTOR_INDEX_NOT_FOUND : i;    \
    }                                                                                               \
                                                                                                    \
    SCOPE vector_uint_t vector_insert_sorted_unique_##T(Vector_##T *vec, T item) {                  \
        vector_uint_t const i = vector_insertion_index_sorted_unique_##T(vec, item);                \
        if (i != VECTOR_INDEX_NOT_FOUND) vector_insert_at_##T(vec, i, item);                        \
        return i;                                                                                   \
    }

// ##############
// # Public API #
// ##############

/// @name Type definitions

/**
 * Declares a new vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @related Vector
 */
#define VECTOR_DECL(T)                                                                              \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)

/**
 * Declares a new vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related Vector
 */
#define VECTOR_DECL_SPEC(T, SPEC)                                                                   \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, SPEC __vector_unused)

/**
 * Declares a new equatable vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_DECL_EQUATABLE(T)                                                                    \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)                                                               \
    __VECTOR_DECL_EQUATABLE(T, __vector_unused)

/**
 * Declares a new equatable vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related Vector
 */
#define VECTOR_DECL_EQUATABLE_SPEC(T, SPEC)                                                         \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, SPEC __vector_unused)                                                          \
    __VECTOR_DECL_EQUATABLE(T, SPEC __vector_unused)

/**
 * Declares a new comparable vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_DECL_COMPARABLE(T)                                                                   \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, __vector_unused)                                                               \
    __VECTOR_DECL_EQUATABLE(T, __vector_unused)                                                     \
    __VECTOR_DECL_COMPARABLE(T, __vector_unused)

/**
 * Declares a new comparable vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related Vector
 */
#define VECTOR_DECL_COMPARABLE_SPEC(T, SPEC)                                                        \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_DECL(T, SPEC __vector_unused)                                                          \
    __VECTOR_DECL_EQUATABLE(T, SPEC __vector_unused)                                                \
    __VECTOR_DECL_COMPARABLE(T, SPEC __vector_unused)

/**
 * Implements a previously declared vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_IMPL(T) \
    __VECTOR_IMPL(T, __vector_unused)

/**
 * Implements a previously declared equatable vector type.
 * Elements of an equatable vector can be checked for equality via __equal_func.
 *
 * @param T [symbol] Vector type.
 * @param __equal_func [(T, T) -> bool] Equality function.
 *
 * @public @related Vector
 */
#define VECTOR_IMPL_EQUATABLE(T, __equal_func)                                                      \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __equal_func, 0)

/**
 * Implements a previously declared comparable vector type.
 * Elements of a comparable vector can be checked for equality via __equal_func
 * and ordered via __compare_func.
 *
 * @param T [symbol] Vector type.
 * @param __equal_func [(T, T) -> bool] Equality function.
 * @param __compare_func [(T, T) -> bool] Comparison function (True if LHS is smaller than RHS).
 *
 * @public @related Vector
 */
#define VECTOR_IMPL_COMPARABLE(T, __equal_func, __compare_func)                                     \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __equal_func, 0)                                    \
    __VECTOR_IMPL_COMPARABLE(T, __vector_unused, __equal_func, __compare_func)

/**
 * Implements a previously declared comparable vector type
 * whose elements can be checked for equality via == and compared via <.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_IMPL_IDENTIFIABLE(T)                                                                 \
    __VECTOR_IMPL(T, __vector_unused)                                                               \
    __VECTOR_IMPL_EQUATABLE(T, __vector_unused, __vector_identical, 1)                              \
    __VECTOR_IMPL_COMPARABLE(T, __vector_unused, __vector_identical, __vector_less_than)

/**
 * Defines a new static vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_INIT(T)                                                                              \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)

/**
 * Defines a new static equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param __equal_func [(T, T) -> bool] Equality function.
 *
 * @public @related Vector
 */
#define VECTOR_INIT_EQUATABLE(T, __equal_func)                                                      \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)                                                        \
    __VECTOR_IMPL_EQUATABLE(T, __vector_static_inline, __equal_func, 0)

/**
 * Defines a new static comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param __equal_func [(T, T) -> bool] Equality function.
 * @param __compare_func [(T, T) -> bool] Comparison function (True if LHS is smaller than RHS).
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define VECTOR_INIT_IDENTIFIABLE(T)                                                                 \
    __VECTOR_DEF_TYPE(T)                                                                            \
    __VECTOR_IMPL(T, __vector_static_inline)                                                        \
    __VECTOR_IMPL_EQUATABLE(T, __vector_static_inline, __vector_identical, 1)                       \
    __VECTOR_IMPL_COMPARABLE(T, __vector_static_inline, __vector_identical, __vector_less_than)

/// @name Declaration

/**
 * Declares a new vector variable.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define Vector(T) __MACRO_CONCAT(Vector_, T)

/**
 * Expands to 'struct Vector_T', useful for forward-declarations.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related Vector
 */
#define vector_struct(T) struct __MACRO_CONCAT(Vector_, T)

/// @name Memory management

/**
 * Allocates a new vector.
 *
 * @param T [symbol] Vector type.
 * @return [Vector(T)*] Vector instance.
 *
 * @public @related Vector
 */
#define vector_alloc(T) __MACRO_CONCAT(vector_alloc_, T)()

/**
 * Ensures the variable 'vec' expands to is not NULL,
 * otherwise a new vector is allocated and assigned to 'vec'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector variable.
 * @return [Vector(T)*] Variable 'vec' expands to.
 *
 * @public @related Vector
 */
#define vector_ensure(T, vec) (vec ? vec : (vec = vector_alloc(T)))

/**
 * Deallocates the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector to free.
 *
 * @public @related Vector
 */
#define vector_free(T, vec) __MACRO_CONCAT(vector_free_, T)(vec)

/**
 * Copies the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector to copy.
 * @return [Vector(T)*] Copied vector instance.
 *
 * @public @related Vector
 */
#define vector_copy(T, vec) __MACRO_CONCAT(vector_copy_, T)(vec)

/**
 * Initializes a new vector on the stack.
 *
 * @param T [symbol] Vector type.
 * @return [Vector(T)] Initialized vector instance.
 *
 * @public @related Vector
 */
#define vector_init(T) \
    ((__MACRO_CONCAT(Vector_, T)){ .count = 0, .allocated = 0, .storage = NULL })

/**
 * De-initializes a vector previously initialized via VECTOR_INIT.
 *
 * @param vec [Vector(T)] Vector to de-initialize.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param size [vector_uint_t] Number of elements the vector should be able to hold.
 *
 * @public @related Vector
 */
#define vector_reserve_capacity(T, vec, size) \
    __MACRO_CONCAT(vector_reserve_capacity_, T)(vec, size)

/**
 * Expands the specified vector so that it can contain additional 'size' elements.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector to expand.
 * @param size [vector_uint_t] Number of additional elements the vector should be able to hold.
 *
 * @public @related Vector
 */
#define vector_expand(T, vec, size) \
    __MACRO_CONCAT(vector_reserve_capacity_, T)(vec, (vec->count + size))

/**
 * Shrinks the specified vector so that its allocated size
 * exactly matches the number of elements it contains.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector to shrink.
 *
 * @public @related Vector
 */
#define vector_shrink(T, vec) __MACRO_CONCAT(vector_shrink_, T)(vec)

/// @name Primitives

/**
 * Retrieves the element at the specified index.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @param idx [vector_uint_t] Index.
 * @return [T] Element at the specified index.
 *
 * @public @related Vector
 */
#define vector_get(vec, idx) ((vec)->storage[(idx)])

/**
 * Replaces the element at the specified index.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @param idx [vector_uint_t] Index.
 * @param item [T] Replacement element.
 *
 * @public @related Vector
 */
#define vector_set(vec, idx, item) ((vec)->storage[(idx)] = (item))

/**
 * Returns the first element in the vector.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @return [T] First element.
 *
 * @public @related Vector
 */
#define vector_first(vec) ((vec)->storage[0])

/**
 * Returns the last element in the vector.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @return [T] Last element.
 *
 * @public @related Vector
 */
#define vector_last(vec) ((vec)->storage[(vec)->count-1])

/**
 * Checks whether the specified vector is empty.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @return [bool] True if the vector is empty, false otherwise.
 *
 * @note For convenience, this macro returns 'true' for NULL vectors.
 *
 * @public @related Vector
 */
#define vector_is_empty(vec) (!((vec) && (vec)->count))

/**
 * Returns the number of elements in the vector.
 *
 * @param vec [Vector(T)*] Vector instance.
 * @return [vector_uint_t] Number of elements.
 *
 * @note For convenience, this macro returns '0' for NULL vectors.
 *
 * @public @related Vector
 */
#define vector_count(vec) ((vec) ? (vec)->count : 0)

/**
 * Pushes the specified element to the top of the vector (last element).
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to push.
 *
 * @public @related Vector
 */
#define vector_push(T, vec, item) __MACRO_CONCAT(vector_push_, T)(vec, item)

/**
 * Removes and returns the element at the top of the vector (last element).
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @return [T] Last element.
 *
 * @public @related Vector
 */
#define vector_pop(T, vec) __MACRO_CONCAT(vector_pop_, T)(vec)

/**
 * Removes the element at the specified index.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param idx [vector_uint_t] Index of the element to remove.
 *
 * @public @related Vector
 */
#define vector_remove_at(T, vec, idx) __MACRO_CONCAT(vector_remove_at_, T)(vec, idx)

/**
 * Inserts an element at the specified index.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param idx [vector_uint_t] Index at which the element should be inserted.
 * @param item [T] Element to insert.
 *
 * @public @related Vector
 */
#define vector_insert_at(T, vec, idx, item) __MACRO_CONCAT(vector_insert_at_, T)(vec, idx, item)

/**
 * Removes all the elements in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 *
 * @public @related Vector
 */
#define vector_remove_all(T, vec) __MACRO_CONCAT(vector_remove_all_, T)(vec)

/**
 * Appends a vector to another.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param vec_to_append [Vector(T)*] Vector to append.
 *
 * @public @related Vector
 */
#define vector_append(T, vec, vec_to_append) \
    __MACRO_CONCAT(vector_append_array_, T)(vec, (vec_to_append)->storage, (vec_to_append)->count)

/**
 * Appends an array to the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param array [T*] Array to append.
 * @param n [vector_uint_t] Number of elements to append.
 *
 * @public @related Vector
 */
#define vector_append_array(T, vec, array, n) \
    __MACRO_CONCAT(vector_append_array_, T)(vec, array, n)

/**
 * Appends multiple items to the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param ... [T] Elements to append.
 *
 * @public @related Vector
 */
#define vector_append_items(T, vec, ...) do {                                                       \
    T const __vec_##T##_init[] = { __VA_ARGS__ };                                                   \
    size_t __vec_##T##_init_size = sizeof(__vec_##T##_init) / sizeof(*__vec_##T##_init);            \
    vector_append_array(T, vec, __vec_##T##_init, __vec_##T##_init_size);                           \
} while(0)

/**
 * 'vector_ensure' followed by 'vector_push'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to push.
 *
 * @public @related Vector
 */
#define vector_push_lazy(T, vec, item) \
    do { vector_ensure(T, vec); vector_push(T, vec, item); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param vec_to_append [Vector(T)*] Vector to append.
 *
 * @public @related Vector
 */
#define vector_append_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append(T, vec, vec_to_append); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append_array'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param array [T*] Array to append.
 * @param n [vector_uint_t] Number of elements to append.
 *
 * @public @related Vector
 */
#define vector_append_array_lazy(T, vec, array, n) \
    do { vector_ensure(T, vec); vector_append_array(T, vec, array, n); } while(0)

/**
 * Reverses the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 *
 * @public @related Vector
 */
#define vector_reverse(T, vec) __MACRO_CONCAT(vector_reverse_, T)(vec)

/// @name Iteration

/**
 * Iterates over the vector, executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param idx_name [symbol] Name of the index variable.
 * @param code [code] Code block to execute.
 *
 * @public @related Vector
 */
#define vector_iterate(T, vec, item_name, idx_name, code) do {                                      \
    if (vec) {                                                                                      \
        vector_uint_t __n_##idx_name = (vec)->count;                                                \
        for (vector_uint_t idx_name = 0; idx_name != __n_##idx_name; ++idx_name) {                  \
            T item_name = vector_get((vec), (idx_name));                                            \
            code;                                                                                   \
        }                                                                                           \
    }                                                                                               \
} while(0)

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param idx_name [symbol] Name of the index variable.
 * @param code [code] Code block to execute.
 *
 * @public @related Vector
 */
#define vector_iterate_reverse(T, vec, item_name, idx_name, code) do {                              \
    if (vec) {                                                                                      \
        for (vector_uint_t idx_name = (vec)->count; idx_name-- != 0;) {                             \
            T item_name = vector_get((vec), (idx_name));                                            \
            code;                                                                                   \
        }                                                                                           \
    }                                                                                               \
} while(0)

/**
 * Iterates over the vector,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param code [code] Code block to execute.
 *
 * @public @related Vector
 */
#define vector_foreach(T, vec, item_name, code) \
    vector_iterate(T, vec, item_name, __i_##item_name, code)

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param code [code] Code block to execute.
 *
 * @public @related Vector
 */
#define vector_foreach_reverse(T, vec, item_name, code) \
    vector_iterate_reverse(T, vec, item_name, __i_##item_name, code)

/// @name Equatable

/**
 * Returns the index of the first occurrence of the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [vector_uint_t] Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 *
 * @public @related Vector
 */
#define vector_index_of(T, vec, item) __MACRO_CONCAT(vector_index_of_, T)(vec, item)

/**
 * Returns the index of the last occurrence of the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [vector_uint_t] Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 *
 * @public @related Vector
 */
#define vector_index_of_reverse(T, vec, item) \
    __MACRO_CONCAT(vector_index_of_reverse_, T)(vec, item)

/**
 * Checks whether the vector contains the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [bool] True if the vector contains the specified element, false otherwise.
 *
 * @public @related Vector
 */
#define vector_contains(T, vec, item) \
    (__MACRO_CONCAT(vector_index_of_, T)(vec, item) != VECTOR_INDEX_NOT_FOUND)

/**
 * Checks whether the vector contains all the elements present in another vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param other_vec [Vector(T)*] Vector containing the elements to search.
 * @return [bool] True if the vector contains all the specified elements, false otherwise.
 *
 * @public @related Vector
 */
#define vector_contains_all(T, vec, other_vec) \
    __MACRO_CONCAT(vector_contains_all_, T)(vec, other_vec)

/**
 * Checks whether the vector contains any of the elements contained in another vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param other_vec [Vector(T)*] Vector containing the elements to search.
 * @return [bool] True if the vector contains any of the specified elements, false otherwise.
 *
 * @public @related Vector
 */
#define vector_contains_any(T, vec, other_vec) \
    __MACRO_CONCAT(vector_contains_any_, T)(vec, other_vec)

/**
 * Removes the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to remove.
 * @return [bool] True if the element was found and removed, false otherwise.
 *
 * @public @related Vector
 */
#define vector_remove(T, vec, item) __MACRO_CONCAT(vector_remove_, T)(vec, item)

/**
 * Checks whether the two vectors are equal.
 * Two vectors are considered equal if they contain the same elements in the same order.
 *
 * @param T [symbol] Vector type.
 * @param vec_a [Vector(T)*] First vector.
 * @param vec_b [Vector(T)*] Second vector.
 * @return [bool] True if the vectors are equal, false otherwise.
 *
 * @public @related Vector
 */
#define vector_equals(T, vec_a, vec_b) __MACRO_CONCAT(vector_equals_, T)(vec_a, vec_b)

/**
 * Pushes an element if it is not already in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to push.
 * @return [bool] True if the element was pushed, false otherwise.
 *
 * @public @related Vector
 */
#define vector_push_unique(T, vec, item) __MACRO_CONCAT(vector_push_unique_, T)(vec, item)

/**
 * Pushes all the elements present in 'vec_to_append' that are not already present in 'vec'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param vec_to_append [Vector(T)*] Vector containing the elements to append.
 *
 * @public @related Vector
 */
#define vector_append_unique(T, vec, vec_to_append)                                                 \
    vector_foreach(T, vec_to_append, __item, {                                                      \
        __MACRO_CONCAT(vector_push_unique_, T)(vec, __item);                                        \
    })

/**
 * Removes all the elements present in 'vec_to_remove' from 'vec'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param vec_to_remove [Vector(T)*] Vector containing the elements to remove.
 *
 * @public @related Vector
 */
#define vector_remove_all_from(T, vec, vec_to_remove)                                               \
    vector_foreach(T, vec_to_remove, __item, {                                                      \
        __MACRO_CONCAT(vector_remove_, T)(vec, __item);                                             \
    })

/**
 * 'vector_ensure' followed by 'vector_push_unique'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to push.
 *
 * @public @related Vector
 */
#define vector_push_unique_lazy(T, vec, item) \
    do { vector_ensure(T, vec); vector_push_unique(T, vec, item); } while(0)

/**
 * 'vector_ensure' followed by 'vector_append_unique'.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param vec_to_append [Vector(T)*] Vector containing the elements to append.
 *
 * @public @related Vector
 */
#define vector_append_unique_lazy(T, vec, vec_to_append) \
    do { vector_ensure(T, vec); vector_append_unique(T, vec, vec_to_append); } while(0)

/// @name Comparable

/**
 * Returns the index of the minimum element in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @return [vector_uint_t] Index of the minimum element.
 *
 * @public @related Vector
 */
#define vector_index_of_min(T, vec) __MACRO_CONCAT(vector_index_of_min_, T)(vec)

/**
 * Returns the index of the maximum element in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @return [vector_uint_t] Index of the maximum element.
 *
 * @public @related Vector
 */
#define vector_index_of_max(T, vec) __MACRO_CONCAT(vector_index_of_max_, T)(vec)

/**
 * Sorts the vector.
 * Average performance: O(n log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 *
 * @public @related Vector
 */
#define vector_sort(T, vec) __MACRO_CONCAT(vector_sort_range_, T)(vec, 0, (vec)->count)

/**
 * Sorts the elements in the specified range.
 * Average performance: O(n log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param start [vector_uint_t] Range start index.
 * @param len [vector_uint_t] Range length.
 *
 * @public @related Vector
 */
#define vector_sort_range(T, vec, start, len) \
    __MACRO_CONCAT(vector_sort_range_, T)(vec, start, len)

/**
 * Finds the insertion index for the specified item in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element whose insertion index should be found.
 * @return [vector_uint_t] Insertion index.
 *
 * @public @related Vector
 */
#define vector_insertion_index_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_insertion_index_sorted_, T)(vec, item)

/**
 * Returns the index of the specified element in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [vector_uint_t] Index of the found element, or VECTOR_INDEX_NOT_FOUND.
 *
 * @note The returned index is not necessarily the first occurrence of the item.
 *
 * @public @related Vector
 */
#define vector_index_of_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_index_of_sorted_, T)(vec, item)

/**
 * Checks whether a sorted vector contains the specified element.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [bool] True if the vector contains the specified element, false otherwise.
 *
 * @public @related Vector
 */
#define vector_contains_sorted(T, vec, item) \
    (__MACRO_CONCAT(vector_index_of_sorted_, T)(vec, item) != VECTOR_INDEX_NOT_FOUND)

/**
 * Inserts an element in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to insert.
 * @return [vector_uint_t] Index of the inserted element.
 *
 * @public @related Vector
 */
#define vector_insert_sorted(T, vec, item) \
    __MACRO_CONCAT(vector_insert_sorted_, T)(vec, item)

/**
 * Inserts all the elements present in a vector into another sorted vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param source [Vector(T)*] Vector containing the elements to insert.
 *
 * @public @related Vector
 */
#define vector_insert_all_sorted(T, vec, source)                                                    \
    vector_foreach(T, source, __item, {                                                             \
        __MACRO_CONCAT(vector_insert_sorted_, T)(vec, __item);                                      \
    })

/**
 * Finds the insertion index for the specified item in a sorted vector if it is not already present.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element whose insertion index should be found.
 * @return [vector_uint_t] Insertion index, or VECTOR_INDEX_NOT_FOUND if already present.
 *
 * @public @related Vector
 */
#define vector_insertion_index_sorted_unique(T, vec, item) \
    __MACRO_CONCAT(vector_insertion_index_sorted_unique_, T)(vec, item)

/**
 * Inserts an element in a sorted vector if it is not already present.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param item [T] Element to insert.
 * @return [vector_uint_t] Insertion index, or VECTOR_INDEX_NOT_FOUND if already present.
 *
 * @public @related Vector
 */
#define vector_insert_sorted_unique(T, vec, item) \
    __MACRO_CONCAT(vector_insert_sorted_unique_, T)(vec, item)

/**
 * Inserts all the elements present in a vector into another sorted vector
 * if they are not already present.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param source [Vector(T)*] Vector containing the elements to insert.
 *
 * @public @related Vector
 */
#define vector_insert_all_sorted_unique(T, vec, source)                                             \
    vector_foreach(T, source, __item, {                                                             \
        __MACRO_CONCAT(vector_insert_sorted_unique_, T)(vec, __item);                               \
    })

/// @name Deep manipulation

/**
 * Performs a "deep copy": a new vector is allocated and assigned to 'dest',
 * then all the elements in 'source' are copied via '__copy_func' and pushed to 'dest'.
 *
 * @param T [symbol] Vector type.
 * @param dest [Vector(T)*] Destination vector.
 * @param source [Vector(T)*] Source vector.
 * @param __copy_func [(T) -> T] Copy function.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param __free_func [(T) -> void] Free function.
 *
 * @public @related Vector
 */
#define vector_deep_free(T, vec, __free_func) do {                                                  \
    vector_foreach(T, vec, __##__free_func##_item, __free_func(__##__free_func##_item));            \
    vector_free(T, vec);                                                                            \
} while(0)

/**
 * Performs a "deep append": all the elements in 'source' are copied
 * via '__copy_func' and pushed to 'dest'. If 'dest' is NULL, it is allocated beforehand.
 *
 * @param T [symbol] Vector type.
 * @param dest [Vector(T)*] Destination vector.
 * @param source [Vector(T)*] Source vector.
 * @param __copy_func [(T) -> T] Copy function.
 *
 * @public @related Vector
 */
#define vector_deep_append(T, dest, source, __copy_func) do {                                       \
    vector_uint_t __##__copy_func##_count = vector_count(source);                                   \
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param __free_func [(T) -> void] Free function.
 *
 * @public @related Vector
 */
#define vector_deep_remove_all(T, vec, __free_func) do {                                            \
    vector_foreach(T, vec, __##__free_func##_item, __free_func(__##__free_func##_item));            \
    vector_remove_all(T, vec);                                                                      \
} while(0)

/// @name Higher order

/**
 * Returns the index of the first element that matches the specified boolean expression.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param idx_var [vector_uint_t] Out variable (must be declared in outer scope).
 * @param bool_exp [expression] Boolean expression.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param out_var [bool] Out variable (must be declared in outer scope).
 * @param bool_exp [expression] Boolean expression.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param bool_exp [expression] Boolean expression.
 *
 * @public @related Vector
 */
#define vector_remove_first_where(T, vec, bool_exp) \
    vector_remove_and_free_first_where(T, vec, bool_exp, (void))

/**
 * Removes and deallocates the first element that matches the specified boolean expression.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param bool_exp [expression] Boolean expression.
 * @param __free_func [(T) -> void] Free function.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param bool_exp [expression] Boolean expression.
 *
 * @public @related Vector
 */
#define vector_remove_where(T, vec, bool_exp) \
    vector_remove_and_free_where(T, vec, bool_exp, (void))

/**
 * Removes and deallocates all the elements that match the specified boolean expression.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param bool_exp [expression] Boolean expression.
 * @param __free_func [(T) -> void] Free function.
 *
 * @public @related Vector
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
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param __comp_func [(const void *, const void *) -> int] qsort-compatible sorting function.
 *
 * @see qsort
 *
 * @public @related Vector
 */
#define vector_qsort(T, vec, __comp_func) \
    vector_qsort_range(T, vec, 0, (vec)->count, __comp_func)

/**
 * Sorts the elements in the specified range via qsort.
 *
 * @param T [symbol] Vector type.
 * @param vec [Vector(T)*] Vector instance.
 * @param start [vector_uint_t] Range start index.
 * @param len [vector_uint_t] Range length.
 * @param __comp_func [(const void *, const void *) -> int] qsort-compatible sorting function.
 *
 * @see qsort
 *
 * @public @related Vector
 */
#define vector_qsort_range(T, vec, start, len, __comp_func) \
    if (vec) qsort((vec)->storage + start, len, sizeof(T), __comp_func)

#endif // VECTOR_H
