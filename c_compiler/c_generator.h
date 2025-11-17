// c_compiler/c_generator.h

#ifndef C_GENERATOR_H
#define C_GENERATOR_H

#include "c_ast.h" // AST düğüm yapılarını kullanmak için
#include <stddef.h> // size_t tipi için

// --- Generator Durum Yönetimi ---

// Üretilen Assembly kodunu dinamik olarak tutmak için basit bir yapı
// (İleride 'realloc' ile büyütülecek)
typedef struct {
    char* code;   // Dinamik olarak ayrılan kod metni
    size_t size;  // Mevcut boyut
    size_t capacity; // Ayrılmış kapasite
} AsmCode;

// --- Ana Fonksiyon Prototipi ---

/**
 * Ana AST düğümünü (root) alır ve tam Assembly kodunu döndürür.
 */
char* generate_asm(ASTNode* root);

#endif // C_GENERATOR_H