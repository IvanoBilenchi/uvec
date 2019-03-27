/// @author Ivano Bilenchi

#include <stdio.h>
#include <assert.h>

#include "vector.h"

#define array_size(array) (sizeof(array) / sizeof(*array))

#define assert_vector_elements(T, vec, ...) do {                                                    \
    T const result[] = { __VA_ARGS__ };                                                             \
    assert((vec)->count == array_size(result));                                                     \
    assert(memcmp((vec)->storage, result, sizeof(result)) == 0);                                    \
} while(0)

VECTOR_INIT_IDENTIFIABLE(int);

static int int_comparator(const void * a, const void * b) {
    int va = *(const int*)a;
    int vb = *(const int*)b;
    return (va > vb) - (va < vb);
}

static void test_base(void) {
    Vector(int) *v = vector_alloc(int);
    assert(vector_is_empty(v));

    vector_append_items(int, v, 3, 2, 4, 1);
    assert(!vector_is_empty(v));
    assert_vector_elements(int, v, 3, 2, 4, 1);

    assert(vector_get(v, 2) == 4);
    assert(vector_first(v) == 3);
    assert(vector_last(v) == 1);

    vector_set(v, 2, 5);
    assert(vector_get(v, 2) == 5);

    vector_push(int, v, 4);
    assert_vector_elements(int, v, 3, 2, 5, 1, 4);

    assert(vector_pop(int, v) == 4);
    assert_vector_elements(int, v, 3, 2, 5, 1);

    vector_insert_at(int, v, 2, 4);
    assert_vector_elements(int, v, 3, 2, 4, 5, 1);

    vector_remove_at(int, v, 1);
    assert_vector_elements(int, v, 3, 4, 5, 1);

    vector_remove_all(int, v);
    assert(vector_is_empty(v));

    vector_free(int, v);
}

static void test_capacity(void) {
    Vector(int) *v = vector_alloc(int);
    uint32_t const capacity = 5;
    uint32_t const expand = 3;

    vector_reserve_capacity(int, v, capacity);
    assert(v->allocated >= capacity);

    vector_expand(int, v, expand);
    assert(v->allocated >= capacity + expand);

    vector_push(int, v, 2);
    assert(v->allocated >= vector_count(v));

    vector_remove_all(int, v);
    assert(vector_count(v) == 0);

    vector_shrink(int, v);
    assert(v->allocated == 0);

    vector_free(int, v);
}

static void test_equality(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 4, 1);

    Vector(int) *v2 = vector_copy(int, v1);
    assert(vector_equals(int, v1, v2));

    vector_pop(int, v2);
    assert(!vector_equals(int, v1, v2));

    vector_push(int, v2, 5);
    assert(!vector_equals(int, v1, v2));

    vector_free(int, v1);
    vector_free(int, v2);
}

static void test_contains(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 5, 4, 5, 1);

    assert(vector_index_of(int, v1, 5) == 2);
    assert(vector_index_of_reverse(int, v1, 5) == 4);
    assert(vector_index_of(int, v1, 6) == VECTOR_INDEX_NOT_FOUND);

    assert(vector_contains(int, v1, 2));
    assert(!vector_contains(int, v1, 7));

    Vector(int) *v2 = vector_alloc(int);
    vector_append_items(int, v2, 1, 6, 4, 5);

    assert(!vector_contains_all(int, v1, v2));
    assert(vector_contains_any(int, v1, v2));

    vector_remove(int, v2, 6);
    assert(!vector_contains(int, v2, 6));
    assert(vector_contains_all(int, v1, v2));
    assert(vector_contains_any(int, v1, v2));

    vector_remove_all(int, v2);
    vector_append_items(int, v2, 6, 7, 8);
    assert(!vector_contains_any(int, v1, v2));

    vector_free(int, v1);
    vector_free(int, v2);
}

static void test_unique(void) {
    Vector(int) *v1 = vector_alloc(int);
    vector_append_items(int, v1, 3, 2, 4, 1);

    vector_push_unique(int, v1, 2);
    vector_push_unique(int, v1, 5);
    assert_vector_elements(int, v1, 3, 2, 4, 1, 5);

    Vector(int) *v2 = vector_alloc(int);
    vector_append_items(int, v2, 2, 5, 6, 7);
    vector_append_unique(int, v1, v2);
    assert_vector_elements(int, v1, 3, 2, 4, 1, 5, 6, 7);

    vector_remove_all_from(int, v1, v2);
    assert_vector_elements(int, v1, 3, 4, 1);

    vector_free(int, v1);
    vector_free(int, v2);
}

static void test_qsort_reverse(void) {
    Vector(int) *v = vector_alloc(int);
    vector_append_items(int, v, 3, 2, 4, 1);

    vector_qsort(int, v, int_comparator);
    assert_vector_elements(int, v, 1, 2, 3, 4);

    vector_reverse(int, v);
    assert_vector_elements(int, v, 4, 3, 2, 1);

    vector_free(int, v);
}

static void test_higher_order(void) {
    Vector(int) *v = vector_alloc(int);
    vector_append_items(int, v, 3, 2, 4, 1);

    uint32_t idx;
    vector_first_index_where(int, v, idx, _vec_item > 3);
    assert(idx == 2);

    vector_first_index_where(int, v, idx, _vec_item > 5);
    assert(idx == VECTOR_INDEX_NOT_FOUND);

    bool contains;
    vector_contains_where(int, v, contains, _vec_item < 2);
    assert(contains);

    vector_contains_where(int, v, contains, _vec_item < 0);
    assert(!contains);

    vector_remove_first_where(int, v, _vec_item > 3);
    assert_vector_elements(int, v, 3, 2, 1);

    vector_remove_where(int, v, _vec_item > 1);
    assert_vector_elements(int, v, 1);

    vector_free(int, v);
}

static void test_comparable(void) {
    Vector(int) *v = vector_alloc(int);

    vector_insert_sorted(int, v, 0);
    assert_vector_elements(int, v, 0);
    vector_remove_all(int, v);

    Vector(int) *values = vector_alloc(int);
    vector_append_items(int, values, 3, 2, 2, 2, 4, 1, 5, 6, 5);

    vector_append(int, v, values);
    assert(vector_index_of_min(int, v) == 5);
    assert(vector_index_of_max(int, v) == 7);

    vector_sort(int, v);
    assert_vector_elements(int, v, 1, 2, 2, 2, 3, 4, 5, 5, 6);

    vector_remove_all(int, v);
    vector_append(int, v, values);
    vector_sort_range(int, v, 3, 3);
    assert_vector_elements(int, v, 3, 2, 2, 1, 2, 4, 5, 6, 5);

    vector_remove_all(int, v);
    vector_insert_all_sorted(int, v, values);
    assert_vector_elements(int, v, 1, 2, 2, 2, 3, 4, 5, 5, 6);

    assert(vector_contains_sorted(int, v, 6));
    assert(!vector_contains_sorted(int, v, -1));
    assert(vector_index_of_sorted(int, v, 3) == 4);
    assert(vector_index_of_sorted(int, v, 7) == VECTOR_INDEX_NOT_FOUND);

    vector_remove_all(int, v);
    vector_insert_all_sorted_unique(int, v, values);
    assert_vector_elements(int, v, 1, 2, 3, 4, 5, 6);

    vector_free(int, v);
    vector_free(int, values);
}

int main(void) {
    test_base();
    test_capacity();
    test_equality();
    test_contains();
    test_unique();
    test_comparable();
    test_qsort_reverse();
    test_higher_order();
    printf("All tests passed.\n");
    return 0;
}
