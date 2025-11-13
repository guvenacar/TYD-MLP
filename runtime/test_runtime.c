/**
 * test_runtime.c - Runtime Function Tests
 * Tests for Stage 2 Phase 1 string functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "runtime.h"

// ANSI color codes
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

int tests_passed = 0;
int tests_failed = 0;

void test_mlp_strlen() {
    printf(YELLOW "Testing mlp_strlen...\n" RESET);

    // Test 1: Normal string
    assert(mlp_strlen("hello") == 5);
    printf(GREEN "  ✓ Normal string length\n" RESET);
    tests_passed++;

    // Test 2: Empty string
    assert(mlp_strlen("") == 0);
    printf(GREEN "  ✓ Empty string\n" RESET);
    tests_passed++;

    // Test 3: NULL string
    assert(mlp_strlen(NULL) == 0);
    printf(GREEN "  ✓ NULL string\n" RESET);
    tests_passed++;

    // Test 4: UTF-8/Turkish characters
    const char* turkish = "Merhaba";
    assert(mlp_strlen(turkish) == 7);
    printf(GREEN "  ✓ Turkish characters\n" RESET);
    tests_passed++;
}

void test_mlp_substring() {
    printf(YELLOW "\nTesting mlp_substring...\n" RESET);

    const char* test_str = "Hello World";

    // Test 1: Normal substring
    char* result1 = mlp_substring(test_str, 0, 5);
    assert(strcmp(result1, "Hello") == 0);
    printf(GREEN "  ✓ Normal substring (0, 5): '%s'\n" RESET, result1);
    free(result1);
    tests_passed++;

    // Test 2: Middle substring
    char* result2 = mlp_substring(test_str, 6, 5);
    assert(strcmp(result2, "World") == 0);
    printf(GREEN "  ✓ Middle substring (6, 5): '%s'\n" RESET, result2);
    free(result2);
    tests_passed++;

    // Test 3: Length exceeds string
    char* result3 = mlp_substring(test_str, 6, 100);
    assert(strcmp(result3, "World") == 0);
    printf(GREEN "  ✓ Length exceeds (6, 100): '%s'\n" RESET, result3);
    free(result3);
    tests_passed++;

    // Test 4: Out of bounds start
    char* result4 = mlp_substring(test_str, 100, 5);
    assert(strcmp(result4, "") == 0);
    printf(GREEN "  ✓ Out of bounds start: '%s'\n" RESET, result4);
    free(result4);
    tests_passed++;

    // Test 5: Negative start
    char* result5 = mlp_substring(test_str, -1, 5);
    assert(strcmp(result5, "") == 0);
    printf(GREEN "  ✓ Negative start: '%s'\n" RESET, result5);
    free(result5);
    tests_passed++;
}

void test_mlp_char_at() {
    printf(YELLOW "\nTesting mlp_char_at...\n" RESET);

    const char* test_str = "Hello";

    // Test 1: First character
    assert(mlp_char_at(test_str, 0) == 'H');
    printf(GREEN "  ✓ First char (index 0): '%c'\n" RESET, (char)mlp_char_at(test_str, 0));
    tests_passed++;

    // Test 2: Last character
    assert(mlp_char_at(test_str, 4) == 'o');
    printf(GREEN "  ✓ Last char (index 4): '%c'\n" RESET, (char)mlp_char_at(test_str, 4));
    tests_passed++;

    // Test 3: Out of bounds
    assert(mlp_char_at(test_str, 10) == -1);
    printf(GREEN "  ✓ Out of bounds: -1\n" RESET);
    tests_passed++;

    // Test 4: Negative index
    assert(mlp_char_at(test_str, -1) == -1);
    printf(GREEN "  ✓ Negative index: -1\n" RESET);
    tests_passed++;

    // Test 5: NULL string
    assert(mlp_char_at(NULL, 0) == -1);
    printf(GREEN "  ✓ NULL string: -1\n" RESET);
    tests_passed++;
}

void test_mlp_string_concat() {
    printf(YELLOW "\nTesting mlp_string_concat...\n" RESET);

    // Test 1: Normal concatenation
    char* result1 = mlp_string_concat("Hello", " World");
    assert(strcmp(result1, "Hello World") == 0);
    printf(GREEN "  ✓ Normal concat: '%s'\n" RESET, result1);
    free(result1);
    tests_passed++;

    // Test 2: Empty strings
    char* result2 = mlp_string_concat("", "Hello");
    assert(strcmp(result2, "Hello") == 0);
    printf(GREEN "  ✓ Empty first string: '%s'\n" RESET, result2);
    free(result2);
    tests_passed++;

    // Test 3: NULL first string
    char* result3 = mlp_string_concat(NULL, "World");
    assert(strcmp(result3, "World") == 0);
    printf(GREEN "  ✓ NULL first string: '%s'\n" RESET, result3);
    free(result3);
    tests_passed++;

    // Test 4: NULL second string
    char* result4 = mlp_string_concat("Hello", NULL);
    assert(strcmp(result4, "Hello") == 0);
    printf(GREEN "  ✓ NULL second string: '%s'\n" RESET, result4);
    free(result4);
    tests_passed++;

    // Test 5: Both NULL
    char* result5 = mlp_string_concat(NULL, NULL);
    assert(strcmp(result5, "") == 0);
    printf(GREEN "  ✓ Both NULL: '%s'\n" RESET, result5);
    free(result5);
    tests_passed++;
}

void test_mlp_string_compare() {
    printf(YELLOW "\nTesting mlp_string_compare...\n" RESET);

    // Test 1: Equal strings
    assert(mlp_string_compare("hello", "hello") == 0);
    printf(GREEN "  ✓ Equal strings: 0\n" RESET);
    tests_passed++;

    // Test 2: First string less
    assert(mlp_string_compare("apple", "banana") < 0);
    printf(GREEN "  ✓ First < second: < 0\n" RESET);
    tests_passed++;

    // Test 3: First string greater
    assert(mlp_string_compare("zebra", "apple") > 0);
    printf(GREEN "  ✓ First > second: > 0\n" RESET);
    tests_passed++;

    // Test 4: NULL strings
    assert(mlp_string_compare(NULL, NULL) == 0);
    printf(GREEN "  ✓ Both NULL: 0\n" RESET);
    tests_passed++;

    // Test 5: One NULL
    assert(mlp_string_compare(NULL, "hello") < 0);
    printf(GREEN "  ✓ NULL first: < 0\n" RESET);
    tests_passed++;

    assert(mlp_string_compare("hello", NULL) > 0);
    printf(GREEN "  ✓ NULL second: > 0\n" RESET);
    tests_passed++;
}

void test_mlp_array() {
    printf(YELLOW "\nTesting mlp_array functions...\n" RESET);

    // Test 1: Create new array
    MlpArray* arr = mlp_array_new();
    assert(arr != NULL);
    assert(mlp_array_length(arr) == 0);
    printf(GREEN "  ✓ Array creation: length = 0\n" RESET);
    tests_passed++;

    // Test 2: Push elements
    mlp_array_push(arr, 10);
    mlp_array_push(arr, 20);
    mlp_array_push(arr, 30);
    assert(mlp_array_length(arr) == 3);
    printf(GREEN "  ✓ Push 3 elements: length = 3\n" RESET);
    tests_passed++;

    // Test 3: Get elements
    assert(mlp_array_get(arr, 0) == 10);
    assert(mlp_array_get(arr, 1) == 20);
    assert(mlp_array_get(arr, 2) == 30);
    printf(GREEN "  ✓ Get elements: [10, 20, 30]\n" RESET);
    tests_passed++;

    // Test 4: Out of bounds get
    assert(mlp_array_get(arr, 10) == -1);
    assert(mlp_array_get(arr, -1) == -1);
    printf(GREEN "  ✓ Out of bounds get: -1\n" RESET);
    tests_passed++;

    // Test 5: Pop element
    int64_t popped = mlp_array_pop(arr);
    assert(popped == 30);
    assert(mlp_array_length(arr) == 2);
    printf(GREEN "  ✓ Pop element: value = 30, length = 2\n" RESET);
    tests_passed++;

    // Test 6: Pop again
    popped = mlp_array_pop(arr);
    assert(popped == 20);
    assert(mlp_array_length(arr) == 1);
    printf(GREEN "  ✓ Pop again: value = 20, length = 1\n" RESET);
    tests_passed++;

    // Test 7: Array resize (push many elements)
    for (int i = 0; i < 20; i++) {
        mlp_array_push(arr, i * 100);
    }
    assert(mlp_array_length(arr) == 21);  // 1 + 20
    assert(mlp_array_get(arr, 20) == 1900);
    printf(GREEN "  ✓ Array auto-resize: length = 21\n" RESET);
    tests_passed++;

    // Test 8: Pop from empty
    MlpArray* arr2 = mlp_array_new();
    assert(mlp_array_pop(arr2) == -1);
    printf(GREEN "  ✓ Pop from empty: -1\n" RESET);
    tests_passed++;

    // Test 9: NULL array handling
    assert(mlp_array_length(NULL) == 0);
    assert(mlp_array_get(NULL, 0) == -1);
    printf(GREEN "  ✓ NULL array handling\n" RESET);
    tests_passed++;

    // Cleanup
    mlp_array_destroy(arr);
    mlp_array_destroy(arr2);
    printf(GREEN "  ✓ Array cleanup successful\n" RESET);
    tests_passed++;
}

void test_mlp_memory() {
    printf(YELLOW "\nTesting mlp_memory functions...\n" RESET);

    // Test 1: Basic allocation
    void* ptr1 = mlp_alloc(100);
    assert(ptr1 != NULL);
    printf(GREEN "  ✓ Basic allocation: 100 bytes\n" RESET);
    tests_passed++;

    // Test 2: Write and read from allocated memory
    int64_t* int_ptr = (int64_t*)ptr1;
    *int_ptr = 42;
    assert(*int_ptr == 42);
    printf(GREEN "  ✓ Memory read/write works\n" RESET);
    tests_passed++;

    // Test 3: Free memory
    mlp_free(ptr1);
    printf(GREEN "  ✓ Memory freed\n" RESET);
    tests_passed++;

    // Test 4: Free NULL (should be safe)
    mlp_free(NULL);
    printf(GREEN "  ✓ Free NULL is safe\n" RESET);
    tests_passed++;

    // Test 5: Realloc - grow memory
    void* ptr2 = mlp_alloc(50);
    assert(ptr2 != NULL);
    int64_t* nums = (int64_t*)ptr2;
    nums[0] = 10;
    nums[1] = 20;

    void* ptr3 = mlp_realloc(ptr2, 200);
    assert(ptr3 != NULL);
    int64_t* new_nums = (int64_t*)ptr3;
    assert(new_nums[0] == 10);  // Data should be preserved
    assert(new_nums[1] == 20);
    printf(GREEN "  ✓ Realloc grow: 50 -> 200 bytes, data preserved\n" RESET);
    tests_passed++;

    // Test 6: Realloc - shrink memory
    void* ptr4 = mlp_realloc(ptr3, 100);
    assert(ptr4 != NULL);
    int64_t* shrunk = (int64_t*)ptr4;
    assert(shrunk[0] == 10);  // Data should still be there
    printf(GREEN "  ✓ Realloc shrink: 200 -> 100 bytes, data preserved\n" RESET);
    tests_passed++;

    mlp_free(ptr4);

    // Test 7: Alloc with 0 size (should return NULL)
    void* ptr5 = mlp_alloc(0);
    assert(ptr5 == NULL);
    printf(GREEN "  ✓ Alloc with size 0: returns NULL\n" RESET);
    tests_passed++;

    // Test 8: Alloc with negative size (should return NULL)
    void* ptr6 = mlp_alloc(-10);
    assert(ptr6 == NULL);
    printf(GREEN "  ✓ Alloc with negative size: returns NULL\n" RESET);
    tests_passed++;

    // Test 9: Large allocation
    void* ptr7 = mlp_alloc(1024 * 1024);  // 1 MB
    assert(ptr7 != NULL);
    mlp_free(ptr7);
    printf(GREEN "  ✓ Large allocation (1 MB): success\n" RESET);
    tests_passed++;

    // Test 10: Realloc from NULL (should work like malloc)
    void* ptr8 = mlp_realloc(NULL, 100);
    assert(ptr8 != NULL);
    mlp_free(ptr8);
    printf(GREEN "  ✓ Realloc from NULL: works like alloc\n" RESET);
    tests_passed++;
}

int main() {
    printf("\n");
    printf("=========================================\n");
    printf("  MLP Runtime Tests - Phase 1\n");
    printf("=========================================\n\n");

    // GROUP 1: String Functions
    test_mlp_strlen();
    test_mlp_substring();
    test_mlp_char_at();
    test_mlp_string_concat();
    test_mlp_string_compare();

    // GROUP 2: Array Functions
    test_mlp_array();

    // GROUP 3: Memory Management Functions
    test_mlp_memory();

    printf("\n");
    printf("=========================================\n");
    printf("  Test Results\n");
    printf("=========================================\n");
    printf(GREEN "  Passed: %d\n" RESET, tests_passed);
    printf(RED "  Failed: %d\n" RESET, tests_failed);
    printf("=========================================\n\n");

    if (tests_failed == 0) {
        printf(GREEN "✓ All tests passed!\n\n" RESET);
        return 0;
    } else {
        printf(RED "✗ Some tests failed!\n\n" RESET);
        return 1;
    }
}
