/**
 * Tests for the Vector(T) library.
 *
 * @author Ivano Bilenchi
 *
 * @copyright Copyright (c) 2018-2019 Ivano Bilenchi <https://ivanobilenchi.com>
 * @copyright SPDX-License-Identifier: MIT
 *
 * @file
 */

#include <stdio.h>
#include "vector.h"

/// @name Utility macros

#define array_size(array) (sizeof(array) / sizeof(*array))

#define vector_assert(exp) do {                                                                     \
    if (!(exp)) {                                                                                   \
        printf("Test failed: %s, line %d (%s)\n", __func__, __LINE__, #exp);                        \
        return false;                                                                               \
    }                                                                                               \
} while(0)

#define vector_assert_elements(T, vec, ...) do {                                                    \
    T const result[] = { __VA_ARGS__ };                                                             \
    vector_assert((vec)->count == array_size(result));                                              \
    vector_assert(memcmp((vec)->storage, result, sizeof(result)) == 0);                             \
} while(0)

#define vector_assert_elements_array(T, vec, arr) do {                                              \
    vector_assert(memcmp((vec)->storage, arr, (vec)->count) == 0);                                  \
} while(0)

/// @name Type definitions

VECTOR_INIT_IDENTIFIABLE(int)

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
    Vector(int) *v = vector_alloc(int);
    vector_assert(vector_is_empty(v));

    vector_append_items(int, v, 3, 2, 4, 1);
    vector_assert(!vector_is_empty(v));
    vector_assert_elements(int, v, 3, 2, 4, 1);

    vector_assert(vector_get(v, 2) == 4);
    vector_assert(vector_first(v) == 3);
    vector_assert(vector_last(v) == 1);

    vector_set(v, 2, 5);
    vector_assert(vector_get(v, 2) == 5);

    vector_push(int, v, 4);
    vector_assert_elements(int, v, 3, 2, 5, 1, 4);

    vector_assert(vector_pop(int, v) == 4);
    vector_assert_elements(int, v, 3, 2, 5, 1);

    vector_insert_at(int, v, 2, 4);
    vector_assert_elements(int, v, 3, 2, 4, 5, 1);

    vector_remove_at(int, v, 1);
    vector_assert_elements(int, v, 3, 4, 5, 1);

    vector_remove_all(int, v);
    vector_assert(vector_is_empty(v));

    vector_free(int, v);
    return true;
}

static bool test_capacity(void) {
    Vector(int) *v = vector_alloc(int);
    vector_uint_t const capacity = 5;
    vector_uint_t const expand = 3;

    vector_reserve_capacity(int, v, capacity);
    vector_assert(v->allocated >= capacity);

    vector_expand(int, v, expand);
    vector_assert(v->allocated >= capacity + expand);

    vector_push(int, v, 2);
    vector_assert(v->allocated >= vector_count(v));

    vector_remove_all(int, v);
    vector_assert(vector_count(v) == 0);

    vector_shrink(int, v);
    vector_assert(v->allocated == 0);

    vector_free(int, v);
    return true;
}

static bool test_equality(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 4, 1);

    Vector(int) *v2 = vector_deep_copy(int, v1, int_increment);
    vector_assert_elements(int, v2, 4, 3, 5, 2);
    vector_free(int, v2);

    v2 = vector_copy(int, v1);
    vector_assert(vector_equals(int, v1, v2));

    int arr[vector_count(v1)];
    vector_copy_to_array(int, v1, arr);
    vector_assert_elements_array(int, v1, arr);

    vector_pop(int, v2);
    vector_assert(!vector_equals(int, v1, v2));

    vector_push(int, v2, 5);
    vector_assert(!vector_equals(int, v1, v2));

    vector_free(int, v1);
    vector_free(int, v2);
    return true;
}

static bool test_contains(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 5, 4, 5, 1);

    vector_assert(vector_index_of(int, v1, 5) == 2);
    vector_assert(vector_index_of_reverse(int, v1, 5) == 4);
    vector_assert(vector_index_of(int, v1, 6) == VECTOR_INDEX_NOT_FOUND);

    vector_assert(vector_contains(int, v1, 2));
    vector_assert(!vector_contains(int, v1, 7));

    Vector(int) *v2 = vector_alloc(int);
    vector_append_items(int, v2, 1, 6, 4, 5);

    vector_assert(!vector_contains_all(int, v1, v2));
    vector_assert(vector_contains_any(int, v1, v2));

    vector_remove(int, v2, 6);
    vector_assert(!vector_contains(int, v2, 6));
    vector_assert(vector_contains_all(int, v1, v2));
    vector_assert(vector_contains_any(int, v1, v2));

    vector_remove_all(int, v2);
    vector_append_items(int, v2, 6, 7, 8);
    vector_assert(!vector_contains_any(int, v1, v2));

    vector_free(int, v1);
    vector_free(int, v2);
    return true;
}

static bool test_unique(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 4, 1);

    vector_push_unique(int, v1, 2);
    vector_push_unique(int, v1, 5);
    vector_assert_elements(int, v1, 3, 2, 4, 1, 5);

    Vector(int) *v2 = vector_alloc(int);
    vector_append_items(int, v2, 2, 5, 6, 7);
    vector_append_unique(int, v1, v2);
    vector_assert_elements(int, v1, 3, 2, 4, 1, 5, 6, 7);

    vector_remove_all_from(int, v1, v2);
    vector_assert_elements(int, v1, 3, 4, 1);

    vector_free(int, v1);
    vector_free(int, v2);
    return true;
}

static bool test_qsort_reverse(void) {
    Vector(int) *v = vector_alloc(int);
    vector_append_items(int, v, 3, 2, 4, 1);

    vector_qsort(int, v, int_comparator);
    vector_assert_elements(int, v, 1, 2, 3, 4);

    vector_reverse(int, v);
    vector_assert_elements(int, v, 4, 3, 2, 1);

    vector_free(int, v);
    return true;
}

static bool test_higher_order(void) {
    Vector(int) *v = vector_alloc(int);
    vector_append_items(int, v, 3, 2, 4, 1);

    vector_uint_t idx;
    vector_first_index_where(int, v, idx, _vec_item > 3);
    vector_assert(idx == 2);

    vector_first_index_where(int, v, idx, _vec_item > 5);
    vector_assert(idx == VECTOR_INDEX_NOT_FOUND);

    vector_free(int, v);
    return true;
}

static bool test_comparable(void) {
    Vector(int) *v = vector_alloc(int);

    vector_insert_sorted(int, v, 0);
    vector_assert_elements(int, v, 0);
    vector_remove_all(int, v);

    Vector(int) *values = vector_alloc(int);
    vector_append_items(int, values, 3, 2, 2, 2, 4, 1, 5, 6, 5);

    vector_append(int, v, values);
    vector_assert(vector_index_of_min(int, v) == 5);
    vector_assert(vector_index_of_max(int, v) == 7);

    vector_sort(int, v);
    vector_assert_elements(int, v, 1, 2, 2, 2, 3, 4, 5, 5, 6);

    vector_remove_all(int, v);
    vector_append(int, v, values);
    vector_sort_range(int, v, 3, 3);
    vector_assert_elements(int, v, 3, 2, 2, 1, 2, 4, 5, 6, 5);

    vector_remove_all(int, v);
    vector_insert_all_sorted(int, v, values);
    vector_assert_elements(int, v, 1, 2, 2, 2, 3, 4, 5, 5, 6);

    vector_assert(vector_contains_sorted(int, v, 6));
    vector_assert(!vector_contains_sorted(int, v, -1));
    vector_assert(vector_index_of_sorted(int, v, 3) == 4);
    vector_assert(vector_index_of_sorted(int, v, 7) == VECTOR_INDEX_NOT_FOUND);

    vector_remove_all(int, v);
    vector_insert_all_sorted_unique(int, v, values);
    vector_assert_elements(int, v, 1, 2, 3, 4, 5, 6);

    vector_free(int, v);
    vector_free(int, values);
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
        test_unique,
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
