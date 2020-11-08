/**
 * Tests for the uVec library.
 *
 * @author Ivano Bilenchi
 *
 * @copyright Copyright (c) 2018-2020 Ivano Bilenchi <https://ivanobilenchi.com>
 * @copyright SPDX-License-Identifier: MIT
 *
 * @file
 */

#include "uvec.h"
#include <stdio.h>

/// @name Utility macros

#define array_size(array) (sizeof(array) / sizeof(*(array)))

#define uvec_assert(exp) do {                                                                       \
    if (!(exp)) {                                                                                   \
        printf("Test failed: %s, line %d (%s)\n", __func__, __LINE__, #exp);                        \
        return false;                                                                               \
    }                                                                                               \
} while(0)

#define uvec_assert_elements(T, vec, ...) do {                                                      \
    T const result[] = { __VA_ARGS__ };                                                             \
    uvec_assert((vec)->count == array_size(result));                                                \
    uvec_assert(memcmp((vec)->storage, result, sizeof(result)) == 0);                               \
} while(0)

#define uvec_assert_elements_array(T, vec, arr) do {                                                \
    uvec_assert(memcmp((vec)->storage, arr, (vec)->count) == 0);                                    \
} while(0)

/// @name Type definitions

UVEC_INIT_IDENTIFIABLE(int)

static int int_comparator(const void * a, const void * b) {
    int va = *(const int*)a;
    int vb = *(const int*)b;
    return (va > vb) - (va < vb);
}

static int int_increment(int a) {
    return a + 1;
}

/// @name Tests

static bool test_base(void) {
    UVec(int) *v = uvec_alloc(int);
    uvec_assert(v);
    uvec_assert(uvec_is_empty(v));

    uvec_ret ret = uvec_append_items(int, v, 3, 2, 4, 1);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(!uvec_is_empty(v));
    uvec_assert_elements(int, v, 3, 2, 4, 1);

    uvec_assert(uvec_get(v, 2) == 4);
    uvec_assert(uvec_first(v) == 3);
    uvec_assert(uvec_last(v) == 1);

    uvec_set(v, 2, 5);
    uvec_assert(uvec_get(v, 2) == 5);

    ret = uvec_push(int, v, 4);
    uvec_assert(ret == UVEC_OK);
    uvec_assert_elements(int, v, 3, 2, 5, 1, 4);

    uvec_assert(uvec_pop(int, v) == 4);
    uvec_assert_elements(int, v, 3, 2, 5, 1);

    ret = uvec_insert_at(int, v, 2, 4);
    uvec_assert(ret == UVEC_OK);
    uvec_assert_elements(int, v, 3, 2, 4, 5, 1);

    uvec_remove_at(int, v, 1);
    uvec_assert_elements(int, v, 3, 4, 5, 1);

    uvec_remove_all(int, v);
    uvec_assert(uvec_is_empty(v));

    uvec_free(int, v);
    return true;
}

static bool test_capacity(void) {
    UVec(int) *v = uvec_alloc(int);
    uvec_uint const capacity = 5;
    uvec_uint const expand = 3;

    uvec_ret ret = uvec_reserve_capacity(int, v, capacity);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(v->allocated >= capacity);

    ret = uvec_expand(int, v, expand);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(v->allocated >= capacity + expand);

    ret = uvec_push(int, v, 2);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(v->allocated >= uvec_count(v));

    uvec_remove_all(int, v);
    uvec_assert(uvec_count(v) == 0);

    ret = uvec_shrink(int, v);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(v->allocated == 0);

    uvec_free(int, v);
    return true;
}

static bool test_equality(void) {
    UVec(int) *v1 = uvec_alloc(int);
    uvec_ret ret = uvec_append_items(int, v1, 3, 2, 4, 1);
    uvec_assert(ret == UVEC_OK);

    UVec(int) *v2 = uvec_deep_copy(int, v1, int_increment);
    uvec_assert(v2);
    uvec_assert_elements(int, v2, 4, 3, 5, 2);
    uvec_free(int, v2);

    v2 = uvec_copy(int, v1);
    uvec_assert(v2);
    uvec_assert(uvec_equals(int, v1, v2));

    int arr[uvec_count(v1)];
    uvec_copy_to_array(int, v1, arr);
    uvec_assert_elements_array(int, v1, arr);

    uvec_pop(int, v2);
    uvec_assert(!uvec_equals(int, v1, v2));

    ret = uvec_push(int, v2, 5);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(!uvec_equals(int, v1, v2));

    uvec_free(int, v1);
    uvec_free(int, v2);
    return true;
}

static bool test_contains(void) {
    UVec(int) *v1 = uvec_alloc(int);
    uvec_ret ret = uvec_append_items(int, v1, 3, 2, 5, 4, 5, 1);
    uvec_assert(ret == UVEC_OK);

    uvec_assert(uvec_index_of(int, v1, 5) == 2);
    uvec_assert(uvec_index_of_reverse(int, v1, 5) == 4);
    uvec_assert(uvec_index_of(int, v1, 6) == UVEC_INDEX_NOT_FOUND);

    uvec_assert(uvec_contains(int, v1, 2));
    uvec_assert(!uvec_contains(int, v1, 7));

    UVec(int) *v2 = uvec_alloc(int);
    ret = uvec_append_items(int, v2, 1, 6, 4, 5);
    uvec_assert(ret == UVEC_OK);

    uvec_assert(!uvec_contains_all(int, v1, v2));
    uvec_assert(uvec_contains_any(int, v1, v2));

    uvec_remove(int, v2, 6);
    uvec_assert(!uvec_contains(int, v2, 6));
    uvec_assert(uvec_contains_all(int, v1, v2));
    uvec_assert(uvec_contains_any(int, v1, v2));

    uvec_remove_all(int, v2);
    ret = uvec_append_items(int, v2, 6, 7, 8);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(!uvec_contains_any(int, v1, v2));

    uvec_free(int, v1);
    uvec_free(int, v2);
    return true;
}

static bool test_qsort_reverse(void) {
    UVec(int) *v = uvec_alloc(int);
    uvec_ret ret = uvec_append_items(int, v, 3, 2, 4, 1);
    uvec_assert(ret == UVEC_OK);

    uvec_qsort(int, v, int_comparator);
    uvec_assert_elements(int, v, 1, 2, 3, 4);

    uvec_reverse(int, v);
    uvec_assert_elements(int, v, 4, 3, 2, 1);

    uvec_free(int, v);
    return true;
}

static bool test_higher_order(void) {
    UVec(int) *v = uvec_alloc(int);
    uvec_ret ret = uvec_append_items(int, v, 3, 2, 4, 1);
    uvec_assert(ret == UVEC_OK);

    uvec_uint idx;
    uvec_first_index_where(int, v, idx, _vec_item > 3);
    uvec_assert(idx == 2);

    uvec_first_index_where(int, v, idx, _vec_item > 5);
    uvec_assert(idx == UVEC_INDEX_NOT_FOUND);

    uvec_free(int, v);
    return true;
}

static bool test_comparable(void) {
    UVec(int) *v = uvec_alloc(int);

    uvec_uint idx = uvec_insertion_index_sorted(int, v, 0);
    uvec_assert(idx == 0);

    UVec(int) *values = uvec_alloc(int);
    uvec_ret ret = uvec_append_items(int, values, 3, 2, 2, 2, 4, 1, 5, 6, 5);
    uvec_assert(ret == UVEC_OK);

    ret = uvec_append(int, v, values);
    uvec_assert(ret == UVEC_OK);
    uvec_assert(uvec_index_of_min(int, v) == 5);
    uvec_assert(uvec_index_of_max(int, v) == 7);

    uvec_sort_range(int, v, 3, 3);
    uvec_assert_elements(int, v, 3, 2, 2, 1, 2, 4, 5, 6, 5);

    uvec_sort(int, v);
    uvec_assert_elements(int, v, 1, 2, 2, 2, 3, 4, 5, 5, 6);
    uvec_assert(uvec_contains_sorted(int, v, 6));
    uvec_assert(!uvec_contains_sorted(int, v, -1));
    uvec_assert(uvec_index_of_sorted(int, v, 3) == 4);
    uvec_assert(uvec_index_of_sorted(int, v, 7) == UVEC_INDEX_NOT_FOUND);

    uvec_remove_all(int, v);

    uvec_foreach(int, values, value, {
        if (!uvec_contains(int, v, value)) {
            ret = uvec_push(int, v, value);
            uvec_assert(ret == UVEC_OK);
        }
    });

    uvec_sort(int, v);
    uvec_remove(int, v, 4);
    uvec_assert_elements(int, v, 1, 2, 3, 5, 6);

    idx = uvec_insertion_index_sorted(int, v, 2);
    uvec_assert(idx == 1);

    uvec_free(int, v);
    uvec_free(int, values);
    return true;
}

int main(void) {
    printf("Starting tests...\n");
    
    int exit_code = EXIT_SUCCESS;
    bool (*tests[])(void) = {
        test_base,
        test_capacity,
        test_equality,
        test_contains,
        test_comparable,
        test_qsort_reverse,
        test_higher_order
    };

    for (uint32_t i = 0; i < array_size(tests); ++i) {
        if (!tests[i]()) exit_code = EXIT_FAILURE;
    }

    if (exit_code == EXIT_SUCCESS) {
        printf("All tests passed.\n");
    } else {
        printf("Some tests failed.\n");
    }
    
    return exit_code;
}
