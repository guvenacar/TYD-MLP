#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "runtime.h"

void yazdir_sayi(int64_t sayi) {
    printf("%ld\n", sayi);
}

void yazdir_metin(const char* metin) {
    printf("%s\n", metin);
}

typedef struct {
    char* anahtar;
    int64_t deger;
    int kullanilmis;
} SozlukGirdisi;

SozlukGirdisi* sozluk_tablosu = NULL;
int sozluk_boyutu = 0;

unsigned long hash_fonksiyonu(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void sozluk_yok_et(void) {
    if (sozluk_tablosu == NULL) return;
    for (int i = 0; i < sozluk_boyutu; i++) {
        if (sozluk_tablosu[i].kullanilmis && sozluk_tablosu[i].anahtar != NULL) {
            free(sozluk_tablosu[i].anahtar);
        }
    }
    free(sozluk_tablosu);
    sozluk_tablosu = NULL;
    sozluk_boyutu = 0;
}

void sozluk_olustur(int boyut) {
    if (sozluk_tablosu != NULL) {
        sozluk_yok_et();
    }
    sozluk_boyutu = boyut;
    sozluk_tablosu = (SozlukGirdisi*)calloc(sozluk_boyutu, sizeof(SozlukGirdisi));
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük için hafıza ayrılamadı.\n");
        exit(1);
    }
}

void sozluk_ekle(const char* anahtar, int64_t deger) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            sozluk_tablosu[indeks].deger = deger;
            return;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            fprintf(stderr, "Hata: Sözlük dolu!\n");
            return;
        }
    }
    sozluk_tablosu[indeks].anahtar = strdup(anahtar);
    sozluk_tablosu[indeks].deger = deger;
    sozluk_tablosu[indeks].kullanilmis = 1;
}

int64_t sozluk_getir(const char* anahtar) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return -1;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            return sozluk_tablosu[indeks].deger;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            break;
        }
    }
    fprintf(stderr, "Hata: Anahtar bulunamadı: %s\n", anahtar);
    return -1;
}

// ============================================
// STAGE 2 PHASE 1: STRING FUNCTIONS
// ============================================

/**
 * mlp_strlen - Get string length
 */
int64_t mlp_strlen(const char* str) {
    if (str == NULL) {
        return 0;
    }
    return (int64_t)strlen(str);
}

/**
 * mlp_substring - Extract substring
 * Returns a new string (caller must free)
 */
char* mlp_substring(const char* str, int64_t start, int64_t length) {
    if (str == NULL) {
        return NULL;
    }

    int64_t str_len = strlen(str);

    // Bounds checking
    if (start < 0 || start >= str_len) {
        return strdup("");  // Return empty string
    }

    // Adjust length if it goes past end of string
    if (start + length > str_len) {
        length = str_len - start;
    }

    if (length <= 0) {
        return strdup("");
    }

    // Allocate new string
    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        fprintf(stderr, "mlp_substring: Memory allocation failed\n");
        exit(1);
    }

    // Copy substring
    strncpy(result, str + start, length);
    result[length] = '\0';

    return result;
}

/**
 * mlp_char_at - Get character at index
 * Returns character as int64_t, or -1 if out of bounds
 */
int64_t mlp_char_at(const char* str, int64_t index) {
    if (str == NULL) {
        return -1;
    }

    int64_t str_len = strlen(str);

    if (index < 0 || index >= str_len) {
        return -1;  // Out of bounds
    }

    return (int64_t)(unsigned char)str[index];
}

/**
 * mlp_string_concat - Concatenate two strings
 * Returns a new string (caller must free)
 */
char* mlp_string_concat(const char* str1, const char* str2) {
    if (str1 == NULL && str2 == NULL) {
        return strdup("");
    }
    if (str1 == NULL) {
        return strdup(str2);
    }
    if (str2 == NULL) {
        return strdup(str1);
    }

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    char* result = (char*)malloc(len1 + len2 + 1);
    if (result == NULL) {
        fprintf(stderr, "mlp_string_concat: Memory allocation failed\n");
        exit(1);
    }

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}

/**
 * mlp_string_compare - Compare two strings
 * Returns: 0 if equal, <0 if str1 < str2, >0 if str1 > str2
 */
int64_t mlp_string_compare(const char* str1, const char* str2) {
    if (str1 == NULL && str2 == NULL) {
        return 0;
    }
    if (str1 == NULL) {
        return -1;
    }
    if (str2 == NULL) {
        return 1;
    }

    return (int64_t)strcmp(str1, str2);
}

// ============================================
// STAGE 2 PHASE 1: ARRAY FUNCTIONS
// ============================================

#define INITIAL_ARRAY_CAPACITY 8

/**
 * mlp_array_new - Create new dynamic array
 */
MlpArray* mlp_array_new(void) {
    MlpArray* arr = (MlpArray*)malloc(sizeof(MlpArray));
    if (arr == NULL) {
        fprintf(stderr, "mlp_array_new: Failed to allocate array structure\n");
        exit(1);
    }

    arr->data = (int64_t*)malloc(INITIAL_ARRAY_CAPACITY * sizeof(int64_t));
    if (arr->data == NULL) {
        fprintf(stderr, "mlp_array_new: Failed to allocate array data\n");
        free(arr);
        exit(1);
    }

    arr->size = 0;
    arr->capacity = INITIAL_ARRAY_CAPACITY;

    return arr;
}

/**
 * mlp_array_push - Add element to end of array
 */
void mlp_array_push(MlpArray* arr, int64_t item) {
    if (arr == NULL) {
        fprintf(stderr, "mlp_array_push: Array is NULL\n");
        return;
    }

    // Check if we need to resize
    if (arr->size >= arr->capacity) {
        int64_t new_capacity = arr->capacity * 2;
        int64_t* new_data = (int64_t*)realloc(arr->data, new_capacity * sizeof(int64_t));
        if (new_data == NULL) {
            fprintf(stderr, "mlp_array_push: Failed to resize array\n");
            exit(1);
        }
        arr->data = new_data;
        arr->capacity = new_capacity;
    }

    arr->data[arr->size] = item;
    arr->size++;
}

/**
 * mlp_array_get - Get element at index
 */
int64_t mlp_array_get(MlpArray* arr, int64_t index) {
    if (arr == NULL) {
        fprintf(stderr, "mlp_array_get: Array is NULL\n");
        return -1;
    }

    if (index < 0 || index >= arr->size) {
        return -1;  // Out of bounds
    }

    return arr->data[index];
}

/**
 * mlp_array_length - Get array length
 */
int64_t mlp_array_length(MlpArray* arr) {
    if (arr == NULL) {
        return 0;
    }
    return arr->size;
}

/**
 * mlp_array_pop - Remove and return last element
 */
int64_t mlp_array_pop(MlpArray* arr) {
    if (arr == NULL) {
        fprintf(stderr, "mlp_array_pop: Array is NULL\n");
        return -1;
    }

    if (arr->size == 0) {
        return -1;  // Array is empty
    }

    arr->size--;
    return arr->data[arr->size];
}

/**
 * mlp_array_destroy - Destroy array and free memory
 */
void mlp_array_destroy(MlpArray* arr) {
    if (arr == NULL) {
        return;
    }

    if (arr->data != NULL) {
        free(arr->data);
    }

    free(arr);
}

// ============================================
// STAGE 2 PHASE 1: MEMORY MANAGEMENT FUNCTIONS
// ============================================

/**
 * mlp_alloc - Allocate memory
 */
void* mlp_alloc(int64_t size) {
    if (size <= 0) {
        fprintf(stderr, "mlp_alloc: Invalid size: %ld\n", size);
        return NULL;
    }

    void* ptr = malloc((size_t)size);
    if (ptr == NULL) {
        fprintf(stderr, "mlp_alloc: Failed to allocate %ld bytes\n", size);
    }

    return ptr;
}

/**
 * mlp_free - Free allocated memory
 */
void mlp_free(void* ptr) {
    if (ptr == NULL) {
        return;  // Freeing NULL is safe, just ignore
    }

    free(ptr);
}

/**
 * mlp_realloc - Reallocate memory to new size
 */
void* mlp_realloc(void* ptr, int64_t new_size) {
    if (new_size <= 0) {
        fprintf(stderr, "mlp_realloc: Invalid size: %ld\n", new_size);
        return NULL;
    }

    void* new_ptr = realloc(ptr, (size_t)new_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "mlp_realloc: Failed to reallocate to %ld bytes\n", new_size);
    }

    return new_ptr;
}
