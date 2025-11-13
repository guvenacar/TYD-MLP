#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdint.h>
#include <stddef.h>

// ============================================
// MLP Runtime Library - Header File
// ============================================

// Existing functions
void yazdir_sayi(int64_t sayi);
void yazdir_metin(const char* metin);
void sozluk_olustur(int boyut);
void sozluk_ekle(const char* anahtar, int64_t deger);
int64_t sozluk_getir(const char* anahtar);
void sozluk_yok_et(void);

// ============================================
// STAGE 2 PHASE 1: NEW RUNTIME FUNCTIONS
// ============================================

// GROUP 1: String Functions
// -------------------------

/**
 * Get string length
 * @param str: Input string
 * @return: Length of string
 */
int64_t mlp_strlen(const char* str);

/**
 * Extract substring
 * @param str: Input string
 * @param start: Starting index (0-based)
 * @param length: Number of characters to extract
 * @return: New string (caller must free)
 */
char* mlp_substring(const char* str, int64_t start, int64_t length);

/**
 * Get character at index
 * @param str: Input string
 * @param index: Position (0-based)
 * @return: Character as integer (or -1 if out of bounds)
 */
int64_t mlp_char_at(const char* str, int64_t index);

/**
 * Concatenate two strings
 * @param str1: First string
 * @param str2: Second string
 * @return: New concatenated string (caller must free)
 */
char* mlp_string_concat(const char* str1, const char* str2);

/**
 * Compare two strings
 * @param str1: First string
 * @param str2: Second string
 * @return: 0 if equal, <0 if str1 < str2, >0 if str1 > str2
 */
int64_t mlp_string_compare(const char* str1, const char* str2);

// GROUP 2: Array/List Functions
// ------------------------------

/**
 * Dynamic array structure for MLP
 */
typedef struct {
    int64_t* data;      // Array data
    int64_t size;       // Current number of elements
    int64_t capacity;   // Allocated capacity
} MlpArray;

/**
 * Create new dynamic array
 * @return: New array (caller must destroy with mlp_array_destroy)
 */
MlpArray* mlp_array_new(void);

/**
 * Add element to end of array
 * @param arr: Array to modify
 * @param item: Value to add
 */
void mlp_array_push(MlpArray* arr, int64_t item);

/**
 * Get element at index
 * @param arr: Array to query
 * @param index: Position (0-based)
 * @return: Value at index (or -1 if out of bounds)
 */
int64_t mlp_array_get(MlpArray* arr, int64_t index);

/**
 * Get array length
 * @param arr: Array to query
 * @return: Number of elements
 */
int64_t mlp_array_length(MlpArray* arr);

/**
 * Remove and return last element
 * @param arr: Array to modify
 * @return: Last element (or -1 if empty)
 */
int64_t mlp_array_pop(MlpArray* arr);

/**
 * Destroy array and free memory
 * @param arr: Array to destroy
 */
void mlp_array_destroy(MlpArray* arr);

// GROUP 3: Memory Management Functions
// -------------------------------------

/**
 * Allocate memory
 * @param size: Number of bytes to allocate
 * @return: Pointer to allocated memory (or NULL on failure)
 */
void* mlp_alloc(int64_t size);

/**
 * Free allocated memory
 * @param ptr: Pointer to memory to free
 */
void mlp_free(void* ptr);

/**
 * Reallocate memory to new size
 * @param ptr: Pointer to existing allocation
 * @param new_size: New size in bytes
 * @return: Pointer to reallocated memory (or NULL on failure)
 */
void* mlp_realloc(void* ptr, int64_t new_size);

// GROUP 4: File I/O Functions
// ----------------------------

/**
 * Read entire file content
 * @param filename: Path to file
 * @return: File content as string (caller must free)
 */
char* mlp_file_read(const char* filename);

/**
 * Write content to file
 * @param filename: Path to file
 * @param content: Content to write
 * @return: 0 on success, -1 on error
 */
int64_t mlp_file_write(const char* filename, const char* content);

#endif // RUNTIME_H
