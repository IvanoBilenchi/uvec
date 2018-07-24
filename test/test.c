/// @author Ivano Bilenchi

#include <stdio.h>
#include <assert.h>

#include "vector.h"

#pragma mark - Declarations

#define array_size(array) (sizeof(array) / sizeof(*array))

VECTOR_INIT_IDENTIFIABLE(int);

#pragma mark - Utilities

static int int_comparator(const void * a, const void * b) {
    return *(int*)a - *(int*)b;
}

#pragma mark - Tests

static void test_basic() {
    Vector(int) *v1 = vector_alloc(int);
    assert(vector_count(v1) == 0u);
    
    vector_push(int, v1, 1);
    assert(vector_pop(int, v1) == 1);
    
    int const array[] = { 3, 2, 4, 1 };
    vector_append_array(int, v1, array, array_size(array));
    vector_push_unique(int, v1, array[0]);
    vector_push_unique(int, v1, array[1]);
    
    assert(memcmp(v1->storage, array, sizeof(array)) == 0);
    assert(vector_contains(int, v1, array[2]));
    
    vector_remove(int, v1, array[2]);
    assert(!vector_contains(int, v1, array[2]));
    
    Vector(int) *v2 = vector_copy(int, v1);
    assert(vector_equals(int, v1, v2));
    
    vector_remove_all(int, v1);
    assert(v1->count == 0u);
    assert(!vector_equals(int, v1, v2));
    vector_remove_all(int, v2);
    
    vector_append_array(int, v1, array, array_size(array));
    vector_push(int, v1, 5);
    vector_append_array(int, v2, array, array_size(array));
    assert(vector_contains_all(int, v1, v2));
    
    vector_push(int, v2, 6);
    assert(!vector_contains_all(int, v1, v2));
    assert(vector_contains_any(int, v1, v2));
    
    vector_remove_all(int, v2);
    vector_push(int, v2, 6);
    assert(!vector_contains_any(int, v1, v2));
    
    vector_free(int, v1);
    vector_free(int, v2);
}

static void test_reverse() {
    int const array[] = { 1, 2, 3, 4 };
    int const result[] = { 4, 3, 2, 1 };
    
    Vector(int) *v = vector_alloc(int);
    vector_append_array(int, v, array, array_size(array));
    vector_reverse(int, v);
    
    assert(memcmp(v->storage, result, sizeof(result)) == 0);
    vector_free(int, v);
}

static void test_sort() {
    int const array[] = { 3, 2, 4, 1 };
    int const result[] = { 1, 2, 3, 4 };
    
    Vector(int) *v = vector_alloc(int);
    vector_append_array(int, v, array, array_size(array));
    vector_sort(int, v, int_comparator);
    
    assert(memcmp(v->storage, result, sizeof(result)) == 0);
    vector_free(int, v);
}

#pragma mark - Main

int main(void) {
    test_basic();
    test_reverse();
    test_sort();
    printf("Test passed.\n");
}
