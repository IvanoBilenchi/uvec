/**
 * uVec - a type-safe, generic C vector.
 *
 * @see test.c for usage examples.
 * @author Ivano Bilenchi
 *
 * @copyright Copyright (c) 2018-2020 Ivano Bilenchi <https://ivanobilenchi.com>
 * @copyright SPDX-License-Identifier: MIT
 *
 * @file
 */
#ifndef UVEC_H
#define UVEC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// #########
// # Types #
// #########

/**
 * A type safe, generic vector.
 * @struct UVec
 */

/**
 * Unsigned integer type.
 *
 * @public @memberof UVec
 */
#if defined UVEC_TINY
    typedef uint16_t uvec_uint;
#elif defined UVEC_HUGE
    typedef uint64_t uvec_uint;
#else
    typedef uint32_t uvec_uint;
#endif

/**
 * Return codes.
 *
 * @public @memberof UVec
 */
typedef enum uvec_ret {

    /// The operation succeeded.
    UVEC_OK = 0,

    /// The operation could not be completed.
    UVEC_NO,

    /**
     * The operation failed due to an error.
     * As of right now, it can only happen if memory cannot be allocated.
     */
    UVEC_ERR,

} uvec_ret;

// #############
// # Constants #
// #############

/// Maximum value of a uvec_uint variable.
#if defined UVEC_TINY
    #define UVEC_UINT_MAX UINT16_MAX
#elif defined UVEC_HUGE
    #define UVEC_UINT_MAX UINT64_MAX
#else
    #define UVEC_UINT_MAX UINT32_MAX
#endif

/// Index returned by find-like functions when a matching element cannot be found.
#define UVEC_INDEX_NOT_FOUND UVEC_UINT_MAX

/// Cache line size (B).
#ifndef UVEC_CACHE_LINE_SIZE
    #define UVEC_CACHE_LINE_SIZE 64
#endif

/// Quicksort stack size.
#define P_UVEC_SORT_STACK_SIZE 64

// ###############
// # Private API #
// ###############

/// Concatenates the 'a' and 'b' tokens, allowing 'a' and 'b' to be macro-expanded.
#define P_UVEC_CONCAT(a, b) P_UVEC_CONCAT_INNER(a, b)
#define P_UVEC_CONCAT_INNER(a, b) a##b

/// Cross-platform 'inline' specifier.
#ifndef p_uvec_inline
    #ifdef _MSC_VER
        #define p_uvec_inline __inline
    #else
        #define p_uvec_inline inline
    #endif
#endif

/// Cross-platform 'unused' directive.
#ifndef p_uvec_unused
    #if (defined __clang__ && __clang_major__ >= 3) || (defined __GNUC__ && __GNUC__ >= 3)
        #define p_uvec_unused __attribute__ ((__unused__))
    #else
        #define p_uvec_unused
    #endif
#endif

/// Specifier for static inline definitions.
#define p_uvec_static_inline static p_uvec_inline p_uvec_unused

/// Give hints to the static analyzer.
#if (__clang_analyzer__)
    #define p_uvec_analyzer_assert(c) do { if (!(c)) exit(1); } while(0)
#else
    #define p_uvec_analyzer_assert(c)
#endif

/// malloc override.
#ifndef UVEC_MALLOC
    #define UVEC_MALLOC malloc
#endif

/// realloc override.
#ifndef UVEC_REALLOC
    #define UVEC_REALLOC realloc
#endif

/// free override.
#ifndef UVEC_FREE
    #define UVEC_FREE free
#endif

/**
 * Changes the specified unsigned integer into the next power of two.
 *
 * @param x [uvec_uint] Unsigned integer.
 */
#if defined UVEC_TINY
    #define p_uvec_uint_next_power_2(x) (                                                           \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u,                                     \
        ++(x)                                                                                       \
    )
#elif defined UVEC_HUGE
    #define p_uvec_uint_next_power_2(x) (                                                           \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u, (x)|=(x)>>16u, (x)|=(x)>>32u,       \
        ++(x)                                                                                       \
    )
#else
    #define p_uvec_uint_next_power_2(x) (                                                           \
        --(x),                                                                                      \
        (x)|=(x)>>1u, (x)|=(x)>>2u, (x)|=(x)>>4u, (x)|=(x)>>8u, (x)|=(x)>>16u,                      \
        ++(x)                                                                                       \
    )
#endif

/**
 * Identity macro.
 *
 * @param a LHS of the identity.
 * @param b RHS of the identity.
 * @return a == b
 */
#define p_uvec_identical(a, b) ((a) == (b))

/**
 * "Less than" comparison macro.
 *
 * @param a LHS of the comparison.
 * @param b RHS of the comparison.
 * @return a < b
 */
#define p_uvec_less_than(a, b) ((a) < (b))

/**
 * Defines a new vector struct.
 *
 * @param T [symbol] Vector type.
 */
#define P_UVEC_DEF_TYPE(T)                                                                          \
    typedef struct UVec_##T {                                                                       \
        /** @cond */                                                                                \
        uvec_uint allocated;                                                                        \
        uvec_uint count;                                                                            \
        T *storage;                                                                                 \
        /** @endcond */                                                                             \
    } UVec_##T;

/**
 * Generates function declarations for the specified vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define P_UVEC_DECL(T, SCOPE)                                                                       \
    /** @cond */                                                                                    \
    SCOPE UVec_##T* uvec_alloc_##T(void);                                                           \
    SCOPE void uvec_free_##T(UVec_##T *vec);                                                        \
    SCOPE uvec_ret uvec_reserve_capacity_##T(UVec_##T *vec, uvec_uint capacity);                    \
    SCOPE uvec_ret uvec_append_array_##T(UVec_##T *vec, T const *array, uvec_uint n);               \
    SCOPE UVec_##T* uvec_copy_##T(UVec_##T const *vec);                                             \
    SCOPE UVec_##T* uvec_deep_copy_##T(UVec_##T const *vec, T (*copy_func)(T));                     \
    SCOPE void uvec_copy_to_array_##T(UVec_##T const *vec, T array[]);                              \
    SCOPE uvec_ret uvec_shrink_##T(UVec_##T *vec);                                                  \
    SCOPE uvec_ret uvec_push_##T(UVec_##T *vec, T item);                                            \
    SCOPE T uvec_pop_##T(UVec_##T *vec);                                                            \
    SCOPE T uvec_remove_at_##T(UVec_##T *vec, uvec_uint idx);                                       \
    SCOPE uvec_ret uvec_insert_at_##T(UVec_##T *vec, uvec_uint idx, T item);                        \
    SCOPE void uvec_remove_all_##T(UVec_##T *vec);                                                  \
    SCOPE void uvec_reverse_##T(UVec_##T *vec);                                                     \
    /** @endcond */

/**
 * Generates function declarations for the specified equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define P_UVEC_DECL_EQUATABLE(T, SCOPE)                                                             \
    /** @cond */                                                                                    \
    SCOPE uvec_uint uvec_index_of_##T(UVec_##T const *vec, T item);                                 \
    SCOPE uvec_uint uvec_index_of_reverse_##T(UVec_##T const *vec, T item);                         \
    SCOPE bool uvec_remove_##T(UVec_##T *vec, T item);                                              \
    SCOPE bool uvec_equals_##T(UVec_##T const *vec, UVec_##T const *other);                         \
    SCOPE bool uvec_contains_all_##T(UVec_##T const *vec, UVec_##T const *other);                   \
    SCOPE bool uvec_contains_any_##T(UVec_##T const *vec, UVec_##T const *other);                   \
    SCOPE uvec_ret uvec_push_unique_##T(UVec_##T *vec, T item);                                     \
    /** @endcond */

/**
 * Generates function declarations for the specified comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the declarations.
 */
#define P_UVEC_DECL_COMPARABLE(T, SCOPE)                                                            \
    /** @cond */                                                                                    \
    SCOPE uvec_uint uvec_index_of_min_##T(UVec_##T const *vec);                                     \
    SCOPE uvec_uint uvec_index_of_max_##T(UVec_##T const *vec);                                     \
    SCOPE void uvec_sort_range_##T(UVec_##T *vec, uvec_uint start, uvec_uint len);                  \
    SCOPE uvec_uint uvec_insertion_index_sorted_##T(UVec_##T const *vec, T item);                   \
    SCOPE uvec_uint uvec_index_of_sorted_##T(UVec_##T const *vec, T item);                          \
    SCOPE uvec_ret uvec_insert_sorted_##T(UVec_##T *vec, T item, uvec_uint *idx);                   \
    SCOPE uvec_ret uvec_insert_sorted_unique_##T(UVec_##T *vec, T item, uvec_uint *idx);            \
    /** @endcond */

/**
 * Generates function definitions for the specified vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the definitions.
 */
#define P_UVEC_IMPL(T, SCOPE)                                                                       \
                                                                                                    \
    static inline uvec_ret uvec_expand_if_required_##T(UVec_##T *vec) {                             \
        if (vec->count < vec->allocated) return UVEC_OK;                                            \
                                                                                                    \
        uvec_uint new_allocated = vec->allocated ? (vec->allocated * 2) : 2;                        \
                                                                                                    \
        T* new_storage = UVEC_REALLOC(vec->storage, sizeof(T) * new_allocated);                     \
        if (!new_storage) return UVEC_ERR;                                                          \
                                                                                                    \
        vec->allocated = new_allocated;                                                             \
        vec->storage = new_storage;                                                                 \
                                                                                                    \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE UVec_##T* uvec_alloc_##T(void) {                                                          \
        UVec_##T *vec = UVEC_MALLOC(sizeof(*vec));                                                  \
        if (vec) *vec = (UVec_##T) { .allocated = 0, .count = 0, .storage = NULL };                 \
        return vec;                                                                                 \
    }                                                                                               \
                                                                                                    \
    SCOPE void uvec_free_##T(UVec_##T *vec) {                                                       \
        if (!vec) return;                                                                           \
        if (vec->allocated) UVEC_FREE(vec->storage);                                                \
        UVEC_FREE(vec);                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_reserve_capacity_##T(UVec_##T *vec, uvec_uint capacity) {                   \
        if (vec->allocated < capacity) {                                                            \
            p_uvec_uint_next_power_2(capacity);                                                     \
            T* new_storage = UVEC_REALLOC(vec->storage, sizeof(T) * capacity);                      \
            if (!new_storage) return UVEC_ERR;                                                      \
            vec->allocated = capacity;                                                              \
            vec->storage = new_storage;                                                             \
        }                                                                                           \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_append_array_##T(UVec_##T *vec, T const *array, uvec_uint n) {              \
        if (!(n && array)) return UVEC_OK;                                                          \
                                                                                                    \
        uvec_uint old_count = vec->count;                                                           \
        uvec_uint new_count = old_count + n;                                                        \
                                                                                                    \
        if (uvec_reserve_capacity_##T(vec, new_count)) return UVEC_ERR;                             \
                                                                                                    \
        vec->count = new_count;                                                                     \
        memcpy(&(vec->storage[old_count]), array, n * sizeof(T));                                   \
                                                                                                    \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE UVec_##T* uvec_copy_##T(UVec_##T const *vec) {                                            \
        UVec_##T* copy = uvec_alloc_##T();                                                          \
                                                                                                    \
        if (copy && uvec_append_array_##T(copy, vec->storage, vec->count)) {                        \
            uvec_free_##T(copy);                                                                    \
            copy = NULL;                                                                            \
        }                                                                                           \
                                                                                                    \
        return copy;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE void uvec_copy_to_array_##T(UVec_##T const *vec, T array[]) {                             \
        if (vec->count) memcpy(array, vec->storage, vec->count * sizeof(T));                        \
    }                                                                                               \
                                                                                                    \
    SCOPE UVec_##T* uvec_deep_copy_##T(UVec_##T const *vec, T (*copy_func)(T)) {                    \
        UVec_##T *copy = uvec_alloc_##T();                                                          \
                                                                                                    \
        if (copy) {                                                                                 \
            if (uvec_reserve_capacity_##T(copy, vec->count)) {                                      \
                uvec_free_##T(copy);                                                                \
                copy = NULL;                                                                        \
            } else {                                                                                \
                for (uvec_uint i = 0; i < vec->count; ++i) {                                        \
                    copy->storage[i] = copy_func(vec->storage[i]);                                  \
                }                                                                                   \
                copy->count = vec->count;                                                           \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        return copy;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_shrink_##T(UVec_##T *vec) {                                                 \
        uvec_uint new_allocated = vec->count;                                                       \
                                                                                                    \
        if (new_allocated) {                                                                        \
            p_uvec_uint_next_power_2(new_allocated);                                                \
                                                                                                    \
            if (new_allocated < vec->allocated) {                                                   \
                T* new_storage = UVEC_REALLOC(vec->storage, sizeof(T) * new_allocated);             \
                if (!new_storage) return UVEC_ERR;                                                  \
                                                                                                    \
                vec->allocated = new_allocated;                                                     \
                vec->storage = new_storage;                                                         \
            }                                                                                       \
        } else {                                                                                    \
            UVEC_FREE(vec->storage);                                                                \
            vec->allocated = 0;                                                                     \
        }                                                                                           \
                                                                                                    \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_push_##T(UVec_##T *vec, T item) {                                           \
        if (uvec_expand_if_required_##T(vec)) return UVEC_ERR;                                      \
        vec->storage[vec->count++] = item;                                                          \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE T uvec_pop_##T(UVec_##T *vec) {                                                           \
        return vec->storage[--vec->count];                                                          \
    }                                                                                               \
                                                                                                    \
    SCOPE T uvec_remove_at_##T(UVec_##T *vec, uvec_uint idx) {                                      \
        uvec_uint count = vec->count;                                                               \
        T item = vec->storage[idx];                                                                 \
                                                                                                    \
        if (idx < count - 1) {                                                                      \
            size_t block_size = (count - idx - 1) * sizeof(T);                                      \
            memmove(&(vec->storage[idx]), &(vec->storage[idx + 1]), block_size);                    \
        }                                                                                           \
                                                                                                    \
        vec->count--;                                                                               \
        return item;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_insert_at_##T(UVec_##T *vec, uvec_uint idx, T item) {                       \
        if (uvec_expand_if_required_##T(vec)) return UVEC_ERR;                                      \
                                                                                                    \
        if (idx < vec->count) {                                                                     \
            size_t block_size = (vec->count - idx) * sizeof(T);                                     \
            memmove(&(vec->storage[idx + 1]), &(vec->storage[idx]), block_size);                    \
        }                                                                                           \
                                                                                                    \
        vec->storage[idx] = item;                                                                   \
        vec->count++;                                                                               \
                                                                                                    \
        return UVEC_OK;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE void uvec_remove_all_##T(UVec_##T *vec) {                                                 \
        vec->count = 0;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE void uvec_reverse_##T(UVec_##T *vec) {                                                    \
        uvec_uint count = vec->count;                                                               \
                                                                                                    \
        for (uvec_uint i = 0; i < count / 2; ++i) {                                                 \
            T temp = vec->storage[i];                                                               \
            uvec_uint swap_idx = count - i - 1;                                                     \
            vec->storage[i] = vec->storage[swap_idx];                                               \
            vec->storage[swap_idx] = temp;                                                          \
        }                                                                                           \
    }

/**
 * Generates function definitions for the specified equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE [scope] Scope of the definitions.
 * @param equal_func [(T, T) -> bool] Equality function.
 * @param equal_func_is_identity [bool] If true, generated code assumes equal_func is ==.
 */
#define P_UVEC_IMPL_EQUATABLE(T, SCOPE, equal_func, equal_func_is_identity)                         \
                                                                                                    \
    SCOPE uvec_uint uvec_index_of_##T(UVec_##T const *vec, T item) {                                \
        for (uvec_uint i = 0; i < vec->count; ++i) {                                                \
            if (equal_func(vec->storage[i], item)) return i;                                        \
        }                                                                                           \
        return UVEC_INDEX_NOT_FOUND;                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_uint uvec_index_of_reverse_##T(UVec_##T const *vec, T item) {                        \
        for (uvec_uint i = vec->count; i-- != 0;) {                                                 \
            if (equal_func(vec->storage[i], item)) return i;                                        \
        }                                                                                           \
        return UVEC_INDEX_NOT_FOUND;                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool uvec_remove_##T(UVec_##T *vec, T item) {                                             \
        uvec_uint idx = uvec_index_of_##T(vec, item);                                               \
                                                                                                    \
        if (idx != UVEC_INDEX_NOT_FOUND) {                                                          \
            uvec_remove_at_##T(vec, idx);                                                           \
            return true;                                                                            \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    SCOPE bool uvec_equals_##T(UVec_##T const *vec, UVec_##T const *other) {                        \
        if (vec == other) return true;                                                              \
        if (vec->count != other->count) return false;                                               \
        if (!vec->count) return true;                                                               \
                                                                                                    \
        if (equal_func_is_identity) {                                                               \
            return memcmp(vec->storage, other->storage, vec->count * sizeof(T)) == 0;               \
        }                                                                                           \
                                                                                                    \
        for (uvec_uint i = 0; i < vec->count; ++i) {                                                \
            if (!equal_func(vec->storage[i], other->storage[i])) return false;                      \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool uvec_contains_all_##T(UVec_##T const *vec, UVec_##T const *other) {                  \
        if (vec == other) return true;                                                              \
                                                                                                    \
        for (uvec_uint i = 0; i < other->count; ++i) {                                              \
            if (uvec_index_of_##T(vec, other->storage[i]) == UVEC_INDEX_NOT_FOUND) return false;    \
        }                                                                                           \
                                                                                                    \
        return true;                                                                                \
    }                                                                                               \
                                                                                                    \
    SCOPE bool uvec_contains_any_##T(UVec_##T const *vec, UVec_##T const *other) {                  \
        if (vec == other) return true;                                                              \
                                                                                                    \
        for (uvec_uint i = 0; i < other->count; ++i) {                                              \
            if (uvec_index_of_##T(vec, other->storage[i]) != UVEC_INDEX_NOT_FOUND) return true;     \
        }                                                                                           \
                                                                                                    \
        return false;                                                                               \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_push_unique_##T(UVec_##T *vec, T item) {                                    \
        if (uvec_index_of_##T(vec, item) != UVEC_INDEX_NOT_FOUND) return UVEC_NO;                   \
        return uvec_push_##T(vec, item);                                                            \
    }

/**
 * Generates function definitions for the specified comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param SCOPE Scope of the definitions.
 * @param equal_func Equality function: (T, T) -> bool
 * @param compare_func Comparison function: (T, T) -> bool
 */
#define P_UVEC_IMPL_COMPARABLE(T, SCOPE, equal_func, compare_func)                                  \
                                                                                                    \
    SCOPE uvec_uint uvec_index_of_min_##T(UVec_##T const *vec) {                                    \
        if (!vec->count) return UVEC_INDEX_NOT_FOUND;                                               \
                                                                                                    \
        uvec_uint min_idx = 0;                                                                      \
                                                                                                    \
        for (uvec_uint i = 1; i < vec->count; ++i) {                                                \
            if (compare_func(vec->storage[i], vec->storage[min_idx])) min_idx = i;                  \
        }                                                                                           \
                                                                                                    \
        return min_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_uint uvec_index_of_max_##T(UVec_##T const *vec) {                                    \
        if (!vec->count) return UVEC_INDEX_NOT_FOUND;                                               \
                                                                                                    \
        uvec_uint max_idx = 0;                                                                      \
                                                                                                    \
        for (uvec_uint i = 1; i < vec->count; ++i) {                                                \
            if (compare_func(vec->storage[max_idx], vec->storage[i])) max_idx = i;                  \
        }                                                                                           \
                                                                                                    \
        return max_idx;                                                                             \
    }                                                                                               \
                                                                                                    \
    SCOPE void uvec_sort_range_##T(UVec_##T *vec, uvec_uint start, uvec_uint len) {                 \
        T *array = vec->storage + start;                                                            \
        start = 0;                                                                                  \
        uvec_uint pos = 0, seed = 31, stack[P_UVEC_SORT_STACK_SIZE];                                \
                                                                                                    \
        while (true) {                                                                              \
            for (; start + 1 < len; ++len) {                                                        \
                if (pos == P_UVEC_SORT_STACK_SIZE) len = stack[pos = 0];                            \
                                                                                                    \
                T pivot = array[start + seed % (len - start)];                                      \
                seed = seed * 69069 + 1;                                                            \
                stack[pos++] = len;                                                                 \
                                                                                                    \
                for (uvec_uint right = start - 1;;) {                                               \
                    p_uvec_analyzer_assert(false);                                                  \
                    for (++right; compare_func(array[right], pivot); ++right);                      \
                    for (--len; compare_func(pivot, array[len]); --len);                            \
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
    SCOPE uvec_uint uvec_insertion_index_sorted_##T(UVec_##T const *vec, T item) {                  \
        T const *array = vec->storage;                                                              \
        uvec_uint const linear_search_thresh = UVEC_CACHE_LINE_SIZE / sizeof(T);                    \
        uvec_uint r = vec->count, l = 0;                                                            \
                                                                                                    \
        while (r - l > linear_search_thresh) {                                                      \
            uvec_uint m = l + (r - l) / 2;                                                          \
                                                                                                    \
            if (compare_func(array[m], item)) {                                                     \
                l = m + 1;                                                                          \
            } else {                                                                                \
                r = m;                                                                              \
            }                                                                                       \
        }                                                                                           \
                                                                                                    \
        for (; l < r && compare_func(array[l], item); ++l);                                         \
        return l;                                                                                   \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_uint uvec_index_of_sorted_##T(UVec_##T const *vec, T item) {                         \
        uvec_uint const i = uvec_insertion_index_sorted_##T(vec, item);                             \
        return vec->storage && equal_func(vec->storage[i], item) ? i : UVEC_INDEX_NOT_FOUND;        \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_insert_sorted_##T(UVec_##T *vec, T item, uvec_uint *idx) {                  \
        uvec_uint i = uvec_insertion_index_sorted_##T(vec, item);                                   \
        if (idx) *idx = i;                                                                          \
        return uvec_insert_at_##T(vec, i, item);                                                    \
    }                                                                                               \
                                                                                                    \
    SCOPE uvec_ret uvec_insert_sorted_unique_##T(UVec_##T *vec, T item, uvec_uint *idx) {           \
        uvec_uint i = uvec_insertion_index_sorted_##T(vec, item);                                   \
        if (idx) *idx = i;                                                                          \
        if (i == vec->count || !equal_func(vec->storage[i], item)) {                                \
            return uvec_insert_at_##T(vec, i, item);                                                \
        } else {                                                                                    \
            return UVEC_NO;                                                                         \
        }                                                                                           \
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
 * @related UVec
 */
#define UVEC_DECL(T)                                                                                \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, p_uvec_unused)

/**
 * Declares a new vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related UVec
 */
#define UVEC_DECL_SPEC(T, SPEC)                                                                     \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, SPEC p_uvec_unused)

/**
 * Declares a new equatable vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_DECL_EQUATABLE(T)                                                                      \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, p_uvec_unused)                                                                   \
    P_UVEC_DECL_EQUATABLE(T, p_uvec_unused)

/**
 * Declares a new equatable vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related UVec
 */
#define UVEC_DECL_EQUATABLE_SPEC(T, SPEC)                                                           \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, SPEC p_uvec_unused)                                                              \
    P_UVEC_DECL_EQUATABLE(T, SPEC p_uvec_unused)

/**
 * Declares a new comparable vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_DECL_COMPARABLE(T)                                                                     \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, p_uvec_unused)                                                                   \
    P_UVEC_DECL_EQUATABLE(T, p_uvec_unused)                                                         \
    P_UVEC_DECL_COMPARABLE(T, p_uvec_unused)

/**
 * Declares a new comparable vector type, prepending a specifier to the generated declarations.
 *
 * @param T [symbol] Vector type.
 * @param SPEC [specifier] Specifier.
 *
 * @public @related UVec
 */
#define UVEC_DECL_COMPARABLE_SPEC(T, SPEC)                                                          \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_DECL(T, SPEC p_uvec_unused)                                                              \
    P_UVEC_DECL_EQUATABLE(T, SPEC p_uvec_unused)                                                    \
    P_UVEC_DECL_COMPARABLE(T, SPEC p_uvec_unused)

/**
 * Implements a previously declared vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_IMPL(T) \
    P_UVEC_IMPL(T, p_uvec_unused)

/**
 * Implements a previously declared equatable vector type.
 * Elements of an equatable vector can be checked for equality via equal_func.
 *
 * @param T [symbol] Vector type.
 * @param equal_func [(T, T) -> bool] Equality function.
 *
 * @public @related UVec
 */
#define UVEC_IMPL_EQUATABLE(T, equal_func)                                                          \
    P_UVEC_IMPL(T, p_uvec_unused)                                                                   \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_unused, equal_func, 0)

/**
 * Implements a previously declared comparable vector type.
 * Elements of a comparable vector can be checked for equality via equal_func
 * and ordered via compare_func.
 *
 * @param T [symbol] Vector type.
 * @param equal_func [(T, T) -> bool] Equality function.
 * @param compare_func [(T, T) -> bool] Comparison function (True if LHS is smaller than RHS).
 *
 * @public @related UVec
 */
#define UVEC_IMPL_COMPARABLE(T, equal_func, compare_func)                                           \
    P_UVEC_IMPL(T, p_uvec_unused)                                                                   \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_unused, equal_func, 0)                                          \
    P_UVEC_IMPL_COMPARABLE(T, p_uvec_unused, equal_func, compare_func)

/**
 * Implements a previously declared comparable vector type
 * whose elements can be checked for equality via == and compared via <.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_IMPL_IDENTIFIABLE(T)                                                                   \
    P_UVEC_IMPL(T, p_uvec_unused)                                                                   \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_unused, p_uvec_identical, 1)                                    \
    P_UVEC_IMPL_COMPARABLE(T, p_uvec_unused, p_uvec_identical, p_uvec_less_than)

/**
 * Defines a new static vector type.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_INIT(T)                                                                                \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_IMPL(T, p_uvec_static_inline)

/**
 * Defines a new static equatable vector type.
 *
 * @param T [symbol] Vector type.
 * @param equal_func [(T, T) -> bool] Equality function.
 *
 * @public @related UVec
 */
#define UVEC_INIT_EQUATABLE(T, equal_func)                                                          \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_IMPL(T, p_uvec_static_inline)                                                            \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_static_inline, equal_func, 0)

/**
 * Defines a new static comparable vector type.
 *
 * @param T [symbol] Vector type.
 * @param equal_func [(T, T) -> bool] Equality function.
 * @param compare_func [(T, T) -> bool] Comparison function (True if LHS is smaller than RHS).
 *
 * @public @related UVec
 */
#define UVEC_INIT_COMPARABLE(T, equal_func, compare_func)                                           \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_IMPL(T, p_uvec_static_inline)                                                            \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_static_inline, equal_func, 0)                                   \
    P_UVEC_IMPL_COMPARABLE(T, p_uvec_static_inline, equal_func, compare_func)

/**
 * Defines a new static equatable vector type
 * whose elements can be checked for equality via == and compared via <.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVEC_INIT_IDENTIFIABLE(T)                                                                   \
    P_UVEC_DEF_TYPE(T)                                                                              \
    P_UVEC_IMPL(T, p_uvec_static_inline)                                                            \
    P_UVEC_IMPL_EQUATABLE(T, p_uvec_static_inline, p_uvec_identical, 1)                             \
    P_UVEC_IMPL_COMPARABLE(T, p_uvec_static_inline, p_uvec_identical, p_uvec_less_than)

/// @name Declaration

/**
 * Declares a new vector variable.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define UVec(T) P_UVEC_CONCAT(UVec_, T)

/**
 * Expands to 'struct UVec_T', useful for forward-declarations.
 *
 * @param T [symbol] Vector type.
 *
 * @public @related UVec
 */
#define uvec_struct(T) struct P_UVEC_CONCAT(UVec_, T)

/// @name Memory management

/**
 * Allocates a new vector.
 *
 * @param T [symbol] Vector type.
 * @return [UVec(T)*] Vector instance, or NULL on error.
 *
 * @public @related UVec
 */
#define uvec_alloc(T) P_UVEC_CONCAT(uvec_alloc_, T)()

/**
 * Deallocates the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to free.
 *
 * @public @related UVec
 */
#define uvec_free(T, vec) P_UVEC_CONCAT(uvec_free_, T)(vec)

/**
 * Copies the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to copy.
 * @return [UVec(T)*] Copied vector instance, or NULL on error.
 *
 * @public @related UVec
 */
#define uvec_copy(T, vec) P_UVEC_CONCAT(uvec_copy_, T)(vec)

/**
 * Performs a deep copy of the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to copy.
 * @param copy_func [(T) -> T] Copy function, invoked for each element.
 * @return [UVec(T)*] Copied vector instance, or NULL on error.
 *
 * @public @related UVec
 */
#define uvec_deep_copy(T, vec, copy_func) P_UVEC_CONCAT(uvec_deep_copy_, T)(vec, copy_func)

/**
 * Copies the elements of the specified vector into the given array.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to copy.
 * @param array [T*] Array to copy the elements into.
 *
 * @note The array must be sufficiently large to hold all the elements.
 *
 * @public @related UVec
 */
#define uvec_copy_to_array(T, vec, array) P_UVEC_CONCAT(uvec_copy_to_array_, T)(vec, array)

/**
 * Initializes a new vector on the stack.
 *
 * @param T [symbol] Vector type.
 * @return [UVec(T)] Initialized vector instance.
 *
 * @public @related UVec
 */
#define uvec_init(T) ((P_UVEC_CONCAT(UVec_, T)){ .count = 0, .allocated = 0, .storage = NULL })

/**
 * De-initializes a vector previously initialized via uvec_init.
 *
 * @param vec [UVec(T)] Vector to de-initialize.
 *
 * @public @related UVec
 */
#define uvec_deinit(vec) do {                                                                       \
    if ((vec).storage) {                                                                            \
        UVEC_FREE((vec).storage);                                                                   \
        (vec).storage = NULL;                                                                       \
    }                                                                                               \
    (vec).count = (vec).allocated = 0;                                                              \
} while(0)

/**
 * Ensures the specified vector can hold at least as many elements as 'size'.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param size [uvec_uint] Number of elements the vector should be able to hold.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_reserve_capacity(T, vec, size) P_UVEC_CONCAT(uvec_reserve_capacity_, T)(vec, size)

/**
 * Expands the specified vector so that it can contain additional 'size' elements.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to expand.
 * @param size [uvec_uint] Number of additional elements the vector should be able to hold.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_expand(T, vec, size) \
    P_UVEC_CONCAT(uvec_reserve_capacity_, T)(vec, (vec)->count + (size))

/**
 * Shrinks the specified vector so that its allocated size
 * exactly matches the number of elements it contains.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector to shrink.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_shrink(T, vec) P_UVEC_CONCAT(uvec_shrink_, T)(vec)

/// @name Primitives

/**
 * Retrieves the element at the specified index.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @param idx [uvec_uint] Index.
 * @return [T] Element at the specified index.
 *
 * @public @related UVec
 */
#define uvec_get(vec, idx) ((vec)->storage[(idx)])

/**
 * Replaces the element at the specified index.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @param idx [uvec_uint] Index.
 * @param item [T] Replacement element.
 *
 * @public @related UVec
 */
#define uvec_set(vec, idx, item) ((vec)->storage[(idx)] = (item))

/**
 * Returns the first element in the vector.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @return [T] First element.
 *
 * @public @related UVec
 */
#define uvec_first(vec) ((vec)->storage[0])

/**
 * Returns the last element in the vector.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @return [T] Last element.
 *
 * @public @related UVec
 */
#define uvec_last(vec) ((vec)->storage[(vec)->count-1])

/**
 * Checks whether the specified vector is empty.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @return [bool] True if the vector is empty, false otherwise.
 *
 * @note For convenience, this macro returns 'true' for NULL vectors.
 *
 * @public @related UVec
 */
#define uvec_is_empty(vec) (!((vec) && (vec)->count))

/**
 * Returns the number of elements in the vector.
 *
 * @param vec [UVec(T)*] Vector instance.
 * @return [uvec_uint] Number of elements.
 *
 * @note For convenience, this macro returns '0' for NULL vectors.
 *
 * @public @related UVec
 */
#define uvec_count(vec) ((vec) ? (vec)->count : 0)

/**
 * Pushes the specified element to the top of the vector (last element).
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to push.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_push(T, vec, item) P_UVEC_CONCAT(uvec_push_, T)(vec, item)

/**
 * Removes and returns the element at the top of the vector (last element).
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @return [T] Last element.
 *
 * @public @related UVec
 */
#define uvec_pop(T, vec) P_UVEC_CONCAT(uvec_pop_, T)(vec)

/**
 * Removes the element at the specified index.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param idx [uvec_uint] Index of the element to remove.
 * @return [T] Removed element.
 *
 * @public @related UVec
 */
#define uvec_remove_at(T, vec, idx) P_UVEC_CONCAT(uvec_remove_at_, T)(vec, idx)

/**
 * Inserts an element at the specified index.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param idx [uvec_uint] Index at which the element should be inserted.
 * @param item [T] Element to insert.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_insert_at(T, vec, idx, item) P_UVEC_CONCAT(uvec_insert_at_, T)(vec, idx, item)

/**
 * Removes all the elements in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 *
 * @public @related UVec
 */
#define uvec_remove_all(T, vec) P_UVEC_CONCAT(uvec_remove_all_, T)(vec)

/**
 * Appends a vector to another.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param vec_to_append [UVec(T)*] Vector to append.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_append(T, vec, vec_to_append) \
    P_UVEC_CONCAT(uvec_append_array_, T)(vec, (vec_to_append)->storage, (vec_to_append)->count)

/**
 * Appends an array to the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param array [T*] Array to append.
 * @param n [uvec_uint] Number of elements to append.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_append_array(T, vec, array, n) P_UVEC_CONCAT(uvec_append_array_, T)(vec, array, n)

/**
 * Appends multiple items to the specified vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param ... [T] Elements to append.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_append_items(T, vec, ...)                                                              \
    P_UVEC_CONCAT(uvec_append_array_, T)                                                            \
        (vec, (T[]){ __VA_ARGS__ }, (sizeof((T[]){ __VA_ARGS__ }) / sizeof(T)))

/**
 * Reverses the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 *
 * @public @related UVec
 */
#define uvec_reverse(T, vec) P_UVEC_CONCAT(uvec_reverse_, T)(vec)

/// @name Iteration

/**
 * Iterates over the vector, executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param idx_name [symbol] Name of the index variable.
 * @param code [code] Code block to execute.
 *
 * @public @related UVec
 */
#define uvec_iterate(T, vec, item_name, idx_name, code) do {                                        \
    UVec(T) const *p_v_##idx_name = (vec);                                                          \
    if (p_v_##idx_name) {                                                                           \
        uvec_uint p_n_##idx_name = (p_v_##idx_name)->count;                                         \
        for (uvec_uint idx_name = 0; idx_name != p_n_##idx_name; ++idx_name) {                      \
            T item_name = uvec_get(p_v_##idx_name, (idx_name));                                     \
            code;                                                                                   \
        }                                                                                           \
    }                                                                                               \
} while(0)

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param idx_name [symbol] Name of the index variable.
 * @param code [code] Code block to execute.
 *
 * @public @related UVec
 */
#define uvec_iterate_reverse(T, vec, item_name, idx_name, code) do {                                \
    UVec(T) const *p_v_##idx_name = (vec);                                                          \
    if (p_v_##idx_name) {                                                                           \
        for (uvec_uint idx_name = (p_v_##idx_name)->count; idx_name-- != 0;) {                      \
            T item_name = uvec_get(p_v_##idx_name, (idx_name));                                     \
            code;                                                                                   \
        }                                                                                           \
    }                                                                                               \
} while(0)

/**
 * Iterates over the vector,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param code [code] Code block to execute.
 *
 * @public @related UVec
 */
#define uvec_foreach(T, vec, item_name, code) \
    uvec_iterate(T, vec, item_name, p_i_##item_name, code)

/**
 * Iterates over the vector in reverse order,
 * executing the specified code block for each element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item_name [symbol] Name of the element variable.
 * @param code [code] Code block to execute.
 *
 * @public @related UVec
 */
#define uvec_foreach_reverse(T, vec, item_name, code) \
    uvec_iterate_reverse(T, vec, item_name, p_i_##item_name, code)

/// @name Equatable

/**
 * Returns the index of the first occurrence of the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [uvec_uint] Index of the found element, or UVEC_INDEX_NOT_FOUND.
 *
 * @public @related UVec
 */
#define uvec_index_of(T, vec, item) P_UVEC_CONCAT(uvec_index_of_, T)(vec, item)

/**
 * Returns the index of the last occurrence of the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [uvec_uint] Index of the found element, or UVEC_INDEX_NOT_FOUND.
 *
 * @public @related UVec
 */
#define uvec_index_of_reverse(T, vec, item) P_UVEC_CONCAT(uvec_index_of_reverse_, T)(vec, item)

/**
 * Checks whether the vector contains the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [bool] True if the vector contains the specified element, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_contains(T, vec, item) \
    (P_UVEC_CONCAT(uvec_index_of_, T)(vec, item) != UVEC_INDEX_NOT_FOUND)

/**
 * Checks whether the vector contains all the elements present in another vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param other_vec [UVec(T)*] Vector containing the elements to search.
 * @return [bool] True if the vector contains all the specified elements, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_contains_all(T, vec, other_vec) P_UVEC_CONCAT(uvec_contains_all_, T)(vec, other_vec)

/**
 * Checks whether the vector contains any of the elements contained in another vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param other_vec [UVec(T)*] Vector containing the elements to search.
 * @return [bool] True if the vector contains any of the specified elements, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_contains_any(T, vec, other_vec) P_UVEC_CONCAT(uvec_contains_any_, T)(vec, other_vec)

/**
 * Removes the specified element.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to remove.
 * @return [bool] True if the element was found and removed, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_remove(T, vec, item) P_UVEC_CONCAT(uvec_remove_, T)(vec, item)

/**
 * Checks whether the two vectors are equal.
 * Two vectors are considered equal if they contain the same elements in the same order.
 *
 * @param T [symbol] Vector type.
 * @param vec_a [UVec(T)*] First vector.
 * @param vec_b [UVec(T)*] Second vector.
 * @return [bool] True if the vectors are equal, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_equals(T, vec_a, vec_b) P_UVEC_CONCAT(uvec_equals_, T)(vec_a, vec_b)

/**
 * Removes all the elements present in 'vec_to_remove' from 'vec'.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param vec_to_remove [UVec(T)*] Vector containing the elements to remove.
 *
 * @public @related UVec
 */
#define uvec_remove_all_from(T, vec, vec_to_remove)                                                 \
    uvec_foreach(T, vec_to_remove, p_item, {                                                        \
        P_UVEC_CONCAT(uvec_remove_, T)(vec, p_item);                                                \
    })

/**
 * Pushes the specified element to the top of the vector if it does not already contain it.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to push.
 * @return [uvec_ret] UVEC_OK if the element was pushed,
 *                    UVEC_NO if the element was already present, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_push_unique(T, vec, item) P_UVEC_CONCAT(uvec_push_unique_, T)(vec, item)

/// @name Comparable

/**
 * Returns the index of the minimum element in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @return [uvec_uint] Index of the minimum element.
 *
 * @public @related UVec
 */
#define uvec_index_of_min(T, vec) P_UVEC_CONCAT(uvec_index_of_min_, T)(vec)

/**
 * Returns the index of the maximum element in the vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @return [uvec_uint] Index of the maximum element.
 *
 * @public @related UVec
 */
#define uvec_index_of_max(T, vec) P_UVEC_CONCAT(uvec_index_of_max_, T)(vec)

/**
 * Sorts the vector.
 * Average performance: O(n log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 *
 * @public @related UVec
 */
#define uvec_sort(T, vec) P_UVEC_CONCAT(uvec_sort_range_, T)(vec, 0, (vec)->count)

/**
 * Sorts the elements in the specified range.
 * Average performance: O(n log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param start [uvec_uint] Range start index.
 * @param len [uvec_uint] Range length.
 *
 * @public @related UVec
 */
#define uvec_sort_range(T, vec, start, len) P_UVEC_CONCAT(uvec_sort_range_, T)(vec, start, len)

/**
 * Finds the insertion index for the specified item in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element whose insertion index should be found.
 * @return [uvec_uint] Insertion index.
 *
 * @public @related UVec
 */
#define uvec_insertion_index_sorted(T, vec, item) \
    P_UVEC_CONCAT(uvec_insertion_index_sorted_, T)(vec, item)

/**
 * Returns the index of the specified element in a sorted vector.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [uvec_uint] Index of the found element, or UVEC_INDEX_NOT_FOUND.
 *
 * @note The returned index is not necessarily the first occurrence of the item.
 *
 * @public @related UVec
 */
#define uvec_index_of_sorted(T, vec, item) P_UVEC_CONCAT(uvec_index_of_sorted_, T)(vec, item)

/**
 * Checks whether a sorted vector contains the specified element.
 * Average performance: O(log n)
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to search.
 * @return [bool] True if the vector contains the specified element, false otherwise.
 *
 * @public @related UVec
 */
#define uvec_contains_sorted(T, vec, item) \
    (P_UVEC_CONCAT(uvec_index_of_sorted_, T)(vec, item) != UVEC_INDEX_NOT_FOUND)

/**
 * Inserts the specified element in a sorted vector.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to insert.
 * @param[out] idx [uvec_uint] Index of the inserted element.
 * @return [uvec_ret] UVEC_OK on success, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_insert_sorted(T, vec, item, idx) P_UVEC_CONCAT(uvec_insert_sorted_, T)(vec, item, idx)

/**
 * Inserts the specified element in a sorted vector only if it does not already contain it.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param item [T] Element to insert.
 * @param[out] idx [uvec_uint] Index of the inserted (or that of the already present) element.
 * @return [uvec_ret] UVEC_OK if the element was inserted,
 *                    UVEC_NO if the element was already present, otherwise UVEC_ERR.
 *
 * @public @related UVec
 */
#define uvec_insert_sorted_unique(T, vec, item, idx) \
    P_UVEC_CONCAT(uvec_insert_sorted_unique_, T)(vec, item, idx)

/// @name Higher order

/**
 * Returns the index of the first element that matches the specified boolean expression.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param[out] idx_var [uvec_uint] Out variable (must be declared in outer scope).
 * @param bool_exp [expression] Boolean expression.
 *
 * @public @related UVec
 */
#define uvec_first_index_where(T, vec, idx_var, bool_exp) do {                                      \
    idx_var = UVEC_INDEX_NOT_FOUND;                                                                 \
    uvec_iterate(T, vec, _vec_item, p_i_##idx_var, {                                                \
        if ((bool_exp)) {                                                                           \
            idx_var = p_i_##idx_var;                                                                \
            break;                                                                                  \
        }                                                                                           \
    });                                                                                             \
} while(0)

/**
 * Sorts the vector via qsort.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param comp_func [(const void *, const void *) -> int] qsort-compatible sorting function.
 *
 * @see qsort
 *
 * @public @related UVec
 */
#define uvec_qsort(T, vec, comp_func) do {                                                          \
    UVec(T) *p_v_##comp_func = (vec);                                                               \
    if (p_v_##comp_func)                                                                            \
        qsort((p_v_##comp_func)->storage, (p_v_##comp_func)->count, sizeof(T), comp_func);          \
} while(0)

/**
 * Sorts the elements in the specified range via qsort.
 *
 * @param T [symbol] Vector type.
 * @param vec [UVec(T)*] Vector instance.
 * @param start [uvec_uint] Range start index.
 * @param len [uvec_uint] Range length.
 * @param comp_func [(const void *, const void *) -> int] qsort-compatible sorting function.
 *
 * @see qsort
 *
 * @public @related UVec
 */
#define uvec_qsort_range(T, vec, start, len, comp_func) do {                                        \
    UVec(T) *p_v_##comp_func = (vec);                                                               \
    if (p_v_##comp_func)                                                                            \
        qsort((p_v_##comp_func)->storage + (start), len, sizeof(T), comp_func);                     \
} while(0)


#endif // UVEC_H
