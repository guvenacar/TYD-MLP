// c_compiler/c_lexer.h

#ifndef C_LEXER_H
#define C_LEXER_H

// Token Tipleri (Python'daki TOKEN_TIPLERI sözlüğüne karşılık gelir)
// Enum kullanmak, C'de daha temiz ve hızlıdır.
typedef enum {
    TOKEN_EOF, // 0

    // --- Değer Tipleri ---
    TOKEN_SAYI,
    TOKEN_METIN,
    TOKEN_IDENTIFIER,

    // --- Tip Tanımlamaları ---
    TOKEN_TANIMLA_SAYI,   // SAYISAL
    TOKEN_TANIMLA_METIN,  // METIN
    TOKEN_TANIMLA_BOOL,   // BOOL

    // --- Yapısal Anahtar Kelimeler ---
    TOKEN_YAPI_YAZDIR,    // YAZDIR
    TOKEN_YAPI_KOSUL_EGER, // EĞER
    TOKEN_YAPI_KOSUL_ISE,  // İSE
    TOKEN_YAPI_KOSUL_DEGILSE, // DEĞİLSE
    TOKEN_YAPI_ISLEC,     // İŞLEÇ
    TOKEN_YAPI_DONUS,     // DÖNÜŞ
    TOKEN_YAPI_DONGU,     // DÖNGÜ
    TOKEN_YAPI_DONGU_BITIR, // DÖNGÜ_BITIR
    TOKEN_YAPI_SON,       // SON
    TOKEN_YAPI_STRUCT,    // YAPI (Struct tanımlama)

    // --- Operatörler ve Ayırıcılar ---
    // --- Operatörler ve Ayırıcılar ---
    TOKEN_LEFT_PAREN,   // (
    TOKEN_RIGHT_PAREN,  // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_COMMA,        // ,
    TOKEN_ASSIGN,       // =
    TOKEN_SEMICOLON,    // ;  <-- YENİ
    
    // Tek Karakterli Operatörler
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MUL,          // *
    TOKEN_DIV,          // /
    TOKEN_GT,             // >
    TOKEN_LT,             // <
    
    // Çok Karakterli Operatörler
    TOKEN_OP_ESIT_KARSILASTIRMA, // ==
    TOKEN_NOT_ESIT,       // !=
    TOKEN_GTE,            // >=
    TOKEN_LTE,            // <=

} TokenType;

// Python'daki Token sınıfına karşılık gelen C struct'ı
typedef struct {
    TokenType type;
    char* value; // Token'ın string değeri (örn: "topla" veya "123")
    int line;    // Token'ın bulunduğu satır numarası (1'den başlar)
    int column;  // Token'ın bulunduğu kolon numarası (1'den başlar)
} Token;

// Lexer Fonksiyon Bildirimleri
void initLexer(const char* source); // <-- BU SATIRI EKLEYİN (Eksik Prototipti)
Token* getNextToken();

#endif // C_LEXER_H