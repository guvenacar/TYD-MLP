#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "runtime.h"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

int tests_passed = 0;
int tests_failed = 0;

void test_mlp_file_write() {
    printf(YELLOW "\nTesting mlp_file_write...\n" RESET);

    // Test 1: Write simple text
    const char* filename = "/tmp/test_mlp_write.txt";
    const char* content = "Hello from TYD-MLP!";

    int64_t result = mlp_file_write(filename, content);
    assert(result == 0);
    printf(GREEN "  ✓ Write simple text: SUCCESS\n" RESET);
    tests_passed++;

    // Test 2: Write empty string
    result = mlp_file_write("/tmp/test_empty.txt", "");
    assert(result == 0);
    printf(GREEN "  ✓ Write empty string: SUCCESS\n" RESET);
    tests_passed++;

    // Test 3: Write NULL filename (should fail)
    result = mlp_file_write(NULL, "content");
    assert(result == -1);
    printf(GREEN "  ✓ Write NULL filename fails correctly\n" RESET);
    tests_passed++;

    // Test 4: Write NULL content (should fail)
    result = mlp_file_write(filename, NULL);
    assert(result == -1);
    printf(GREEN "  ✓ Write NULL content fails correctly\n" RESET);
    tests_passed++;
}

void test_mlp_file_read() {
    printf(YELLOW "\nTesting mlp_file_read...\n" RESET);

    // Test 1: Read existing file
    const char* filename = "/tmp/test_mlp_write.txt";
    char* content = mlp_file_read(filename);
    assert(content != NULL);
    assert(strcmp(content, "Hello from TYD-MLP!") == 0);
    printf(GREEN "  ✓ Read existing file: '%s'\n" RESET, content);
    free(content);
    tests_passed++;

    // Test 2: Read empty file
    content = mlp_file_read("/tmp/test_empty.txt");
    assert(content != NULL);
    assert(strlen(content) == 0);
    printf(GREEN "  ✓ Read empty file: SUCCESS\n" RESET);
    free(content);
    tests_passed++;

    // Test 3: Read non-existent file (should fail)
    content = mlp_file_read("/tmp/nonexistent_file_12345.txt");
    assert(content == NULL);
    printf(GREEN "  ✓ Read non-existent file returns NULL\n" RESET);
    tests_passed++;

    // Test 4: Read NULL filename (should fail)
    content = mlp_file_read(NULL);
    assert(content == NULL);
    printf(GREEN "  ✓ Read NULL filename returns NULL\n" RESET);
    tests_passed++;
}

void test_file_roundtrip() {
    printf(YELLOW "\nTesting file write-read roundtrip...\n" RESET);

    const char* filename = "/tmp/test_roundtrip.txt";
    const char* original = "TYD-MLP Bootstrap Compiler\nFile I/O Test\n42";

    // Write
    int64_t write_result = mlp_file_write(filename, original);
    assert(write_result == 0);

    // Read back
    char* read_content = mlp_file_read(filename);
    assert(read_content != NULL);
    assert(strcmp(read_content, original) == 0);

    printf(GREEN "  ✓ Write-read roundtrip successful\n" RESET);
    printf("    Original: '%s'\n", original);
    printf("    Read:     '%s'\n", read_content);

    free(read_content);
    tests_passed++;
}

void test_multiline_content() {
    printf(YELLOW "\nTesting multiline content...\n" RESET);

    const char* filename = "/tmp/test_multiline.txt";
    const char* multiline = "Line 1\nLine 2\nLine 3\n\nLine 5 (after empty line)";

    // Write
    int64_t result = mlp_file_write(filename, multiline);
    assert(result == 0);

    // Read back
    char* content = mlp_file_read(filename);
    assert(content != NULL);
    assert(strcmp(content, multiline) == 0);

    printf(GREEN "  ✓ Multiline content preserved correctly\n" RESET);
    free(content);
    tests_passed++;
}

void test_large_file() {
    printf(YELLOW "\nTesting large file (1KB)...\n" RESET);

    const char* filename = "/tmp/test_large.txt";

    // Create 1KB of content
    char* large_content = (char*)malloc(1025);
    for (int i = 0; i < 1024; i++) {
        large_content[i] = 'A' + (i % 26);
    }
    large_content[1024] = '\0';

    // Write
    int64_t result = mlp_file_write(filename, large_content);
    assert(result == 0);

    // Read back
    char* content = mlp_file_read(filename);
    assert(content != NULL);
    assert(strlen(content) == 1024);
    assert(strcmp(content, large_content) == 0);

    printf(GREEN "  ✓ Large file (1KB) handled correctly\n" RESET);

    free(large_content);
    free(content);
    tests_passed++;
}

int main() {
    printf("========================================\n");
    printf("MLP FILE I/O FUNCTION TESTS\n");
    printf("========================================\n");

    test_mlp_file_write();
    test_mlp_file_read();
    test_file_roundtrip();
    test_multiline_content();
    test_large_file();

    printf("\n========================================\n");
    printf("TEST RESULTS\n");
    printf("========================================\n");
    if (tests_failed == 0) {
        printf(GREEN "ALL %d TESTS PASSED! ✓\n" RESET, tests_passed);
    } else {
        printf(RED "%d tests failed, %d tests passed\n" RESET, tests_failed, tests_passed);
    }
    printf("========================================\n");

    return tests_failed == 0 ? 0 : 1;
}
