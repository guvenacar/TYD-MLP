// c_compiler/c_lexer.c

#include "c_lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h> // iswalpha, iswalnum için
#include <ctype.h>  // isspace için

// --- Lexer Durum Yönetimi ---

// Kaynak kodu (Source Code) dize olarak tutulur.
static const char* source_code = NULL;

// Okuma pozisyonu (Lexer'ın kaynak kodda nerede olduğunu takip eder).
static int current_position = 0;
static int eof_reached = 0; // <-- YENİ: EOF bayrağı

// --- Bellek Yönetimi ve Yardımcı Fonksiyonlar ---

/**
 * Yeni bir Token nesnesi oluşturur ve bellek ayırır.
 * NOT: Bu fonksiyon, dinamik bellek (malloc) kullandığı için, 
 * oluşturulan Token'ların program sonunda serbest bırakılması (free) gerekir.
 */
Token* createToken(TokenType type, const char* value) {
    // Token struct'ı için bellek ayır
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        perror("Hafıza ayırma hatası (Token)");
        exit(EXIT_FAILURE);
    }
    
    token->type = type;
    
    // Değer (value) string'ini dinamik olarak kopyala
    if (value != NULL) {
        token->value = strdup(value); 
        if (token->value == NULL) {
            perror("Hafıza ayırma hatası (Token Value)");
            exit(EXIT_FAILURE);
        }
    } else {
        token->value = NULL;
    }
    return token;
}

typedef struct {
    const char* keyword;
    TokenType type;
} KeywordMap;

// Python'daki ANAHTAR_KELIMELER'e karşılık gelen C dizisi
// NOT: Tüm Türkçe karakterleri (İ, Ç, Ğ, Ö, Ş, Ü, ı) küçük/büyük harf uyumu ile yazmalısınız.
// Şimdilik sadece temel yapılar eklenmiştir.
KeywordMap keywords[] = {
    // Tip Tanımlamaları
    {"SAYISAL", TOKEN_TANIMLA_SAYI},
    {"METIN",   TOKEN_TANIMLA_METIN},

    // Yapısal Anahtar Kelimeler
    {"YAZDIR",  TOKEN_YAPI_YAZDIR},
    {"EĞER",    TOKEN_YAPI_KOSUL_EGER},
    {"İSE",     TOKEN_YAPI_KOSUL_ISE},
    {"DEĞİLSE", TOKEN_YAPI_KOSUL_DEGILSE},
    {"İŞLEÇ",   TOKEN_YAPI_ISLEC}, // Zaten ekli
    {"DÖNÜŞ",   TOKEN_YAPI_DONUS}, // <-- YENİ
    {"DÖNGÜ",   TOKEN_YAPI_DONGU},
    {"DÖNGÜ_BITIR", TOKEN_YAPI_DONGU_BITIR},
    {"SON",     TOKEN_YAPI_SON},

    // NOT: DOSYA_AC, DOSYA_OKU, DOSYA_YAZ, DOSYA_KAPAT built-in fonksiyonlar olarak
    // IDENTIFIER token'ı olarak kalacak (anahtar kelime değil)

    // NOT: Dizinin sonunu işaret etmek için tipik C null sonlandırma tekniği kullanılır
    {NULL, TOKEN_EOF} 
};

/**
 * Lexer'ı başlatır ve kaynak kodu hazırlar.
 */
void initLexer(const char* source) {
    source_code = source;
    current_position = 0;
    eof_reached = 0; // <-- RESET
}


// --- Ana Lexer Mantığı ---

/**
 * Python'daki sonraki_tokeni_al() metoduna karşılık gelir.
 * Kaynak koddan bir sonraki tokeni okur ve döndürür.
 */
Token* getNextToken() {
    
    // 1. EOF zaten döndürüldüyse hemen EOF döndür.
    if (eof_reached) {
        return createToken(TOKEN_EOF, NULL);
    }
    
    // 2. Boşlukları ve Yeni Satırları Atla
    while (source_code[current_position] != '\0' && isspace(source_code[current_position])) {
        current_position++;
    }

    // YENİ: Yorum Satırlarını Atla
    // Bu döngü, birden fazla boş satır veya yorum satırı varsa hepsini atlar.
    while (source_code[current_position] == '-' && source_code[current_position + 1] == '-') {
        // Satır sonuna veya dosya sonuna kadar ilerle
        while (source_code[current_position] != '\n' && source_code[current_position] != '\0') {
            current_position++;
        }
        // Boşlukları tekrar atla (yorumdan sonraki boşluklar için)
        while (source_code[current_position] != '\0' && isspace(source_code[current_position])) {
            current_position++;
        }
    }


    // 3. Dosya Sonuna (EOF) ulaşıldı
    if (source_code[current_position] == '\0') {
        eof_reached = 1; // Bayrağı ayarla
        return createToken(TOKEN_EOF, NULL);
    }

    // --- YENİ: UTF-8 UYUMLU KARAKTER OKUMA ---
    wchar_t current_wc; // Geniş karakter (wide character)
    int char_len = mbtowc(&current_wc, &source_code[current_position], 4); // En fazla 4 byte oku

    if (char_len <= 0) { // Hata veya null karakter
        eof_reached = 1;
        return createToken(TOKEN_EOF, NULL);
    }
    // -----------------------------------------

    // YENİ: 4. Sayısal Sabit (INTEGER) Okuma
    if (iswdigit(current_wc)) {
        int start = current_position;
        // Tüm rakamları okumaya devam et
        while (source_code[current_position] != '\0') {
            wchar_t temp_wc;
            int temp_len = mbtowc(&temp_wc, &source_code[current_position], 4);
            if (temp_len > 0 && iswdigit(temp_wc)) {
                current_position += temp_len;
            } else {
                break;
            }
        }
        
        // Token değerini (string'i) yakala
        int len = current_position - start;
        char* value = (char*)malloc(len + 1);
        strncpy(value, source_code + start, len);
        value[len] = '\0';
        Token* token = createToken(TOKEN_SAYI, value);
        free(value); // createToken zaten kopyalıyor
        return token;
    }


    if (iswalpha(current_wc)) {
        int start = current_position;
        // İsimler harf, rakam veya alt çizgi içerebilir
        while (source_code[current_position] != '\0') {
            wchar_t temp_wc;
            int temp_len = mbtowc(&temp_wc, &source_code[current_position], 4);
            if (temp_len > 0 && (iswalnum(temp_wc) || temp_wc == L'_')) {
                current_position += temp_len;
            } else {
                break;
            }
        }

        // Token değerini (string'i) yakala
        int len = current_position - start;
        char* value = (char*)malloc(len + 1);
        strncpy(value, source_code + start, len);
        value[len] = '\0';

        // Anahtar Kelime Kontrolü
        TokenType type = TOKEN_IDENTIFIER;
        for (int i = 0; keywords[i].keyword != NULL; i++) {
            if (strcmp(keywords[i].keyword, value) == 0) {
                type = keywords[i].type;
                break;
            }
        }
        Token* token = createToken(type, value);
        free(value); // createToken zaten kopyalıyor
        return token;
    }

    char current_char = source_code[current_position]; // Tek baytlık operatörler için hala gerekli

    // YENİ: STRING LITERAL Okuma ("merhaba" veya 'merhaba')
    if (current_char == '"' || current_char == '\'') {
        char quote_char = current_char;
        current_position++; // Açılış tırnağını atla
        int start = current_position;

        // Kapanış tırnağına kadar oku
        while (source_code[current_position] != '\0' && source_code[current_position] != quote_char) {
            // Escape sequence desteği (\n, \t, vb.)
            if (source_code[current_position] == '\\' && source_code[current_position + 1] != '\0') {
                current_position += 2; // Escape karakter ve sonrakini atla
            } else {
                current_position++;
            }
        }

        if (source_code[current_position] == '\0') {
            fprintf(stderr, "HATA [Lexer]: Kapanmamış string literal\n");
            return createToken(TOKEN_EOF, NULL);
        }

        // String içeriğini al
        int len = current_position - start;
        char* value = (char*)malloc(len + 1);
        strncpy(value, source_code + start, len);
        value[len] = '\0';

        current_position++; // Kapanış tırnağını atla

        Token* token = createToken(TOKEN_METIN, value);
        free(value);
        return token;
    }

    // --- YENİ: İki Karakterli Operatörler (İleri Bakış ile) ---
    char next_char = source_code[current_position + 1];

    // == (Eşit Karşılaştırma)
    if (current_char == '=' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_OP_ESIT_KARSILASTIRMA, "==");
    }
    
    // != (Eşit Değil)
    if (current_char == '!' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_NOT_ESIT, "!=");
    }

    // >= (Büyük Eşit)
    if (current_char == '>' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_GTE, ">=");
    }
    
    // <= (Küçük Eşit)
    if (current_char == '<' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_LTE, "<=");
    }

    // --- YENİ: Tek Karakterli Operatörler ve Ayırıcılar ---
    if (current_char == '(') {
        current_position++;
        return createToken(TOKEN_LEFT_PAREN, "(");
    }
    if (current_char == ')') {
        current_position++;
        return createToken(TOKEN_RIGHT_PAREN, ")");
    }
    if (current_char == '+') {
        current_position++;
        return createToken(TOKEN_PLUS, "+");
    }
    if (current_char == '-') {
        // '-' karakteri geldiğinde:
        if (source_code[current_position + 1] == '-') {
            // Satır sonuna kadar yorum
            current_position += 2;
            while (source_code[current_position] != '\n' && source_code[current_position] != '\0') {
                current_position++;
            }
            if (source_code[current_position] == '\n') current_position++;
            // Yorum bitti, bir sonraki tokeni almak için başa dön
            return getNextToken(); // ✅ continue yerine recursive çağrı
        }
        // Tek '-' ise çıkarma operatörü
        current_position++;
        return createToken(TOKEN_MINUS, "-"); // ✅ TOKEN_MINUS kullan
    }
    if (current_char == '*') {
        current_position++;
        return createToken(TOKEN_MUL, "*");
    }
    if (current_char == '/') {
        current_position++;
        return createToken(TOKEN_DIV, "/");
    }
    if (current_char == '>') {
        current_position++;
        return createToken(TOKEN_GT, ">");
    }
    if (current_char == '<') {
        current_position++;
        return createToken(TOKEN_LT, "<");
    }
    if (current_char == '=') {
        current_position++;
        return createToken(TOKEN_ASSIGN, "=");
    }
    if (current_char == ';') {
        current_position++;
        return createToken(TOKEN_SEMICOLON, ";"); 
    }
    if (current_char == ',') {
        current_position++;
        return createToken(TOKEN_COMMA, ","); 
    }

    // 4. Tanınmayan Karakter (HATA YÖNETİMİ DÜZELTİLDİ)
    char error_val[5] = {0}; // UTF-8 karakteri en fazla 4 byte + null terminator
    strncpy(error_val, &source_code[current_position], char_len);

    current_position += char_len; 
    
    fprintf(stderr, "HATA [Lexer]: Tanınmayan karakter: %s\n", error_val);
    
    return createToken(TOKEN_EOF, NULL);
}

// Anahtar kelimeleri kontrol et
Token* check_keyword(const char* word) {
    // Tip tanımlamaları
    if (strcmp(word, "SAYISAL") == 0) return createToken(TOKEN_TANIMLA_SAYI, word);  // ✅ Düzeltildi
    if (strcmp(word, "METIN") == 0) return createToken(TOKEN_TANIMLA_METIN, word);   // ✅ Düzeltildi
    
    // Yapısal anahtar kelimeler
    if (strcmp(word, "YAZDIR") == 0) return createToken(TOKEN_YAPI_YAZDIR, word);    // ✅ Düzeltildi
    if (strcmp(word, "EĞER") == 0) return createToken(TOKEN_YAPI_KOSUL_EGER, word);  // ✅ Düzeltildi
    if (strcmp(word, "İSE") == 0) return createToken(TOKEN_YAPI_KOSUL_ISE, word);    // ✅ Düzeltildi
    if (strcmp(word, "DEĞİLSE") == 0) return createToken(TOKEN_YAPI_KOSUL_DEGILSE, word); // ✅ Düzeltildi
    if (strcmp(word, "İŞLEÇ") == 0) return createToken(TOKEN_YAPI_ISLEC, word);      // ✅ Düzeltildi
    if (strcmp(word, "DÖNÜŞ") == 0) return createToken(TOKEN_YAPI_DONUS, word);      // ✅ Düzeltildi
    if (strcmp(word, "DÖNGÜ") == 0) return createToken(TOKEN_YAPI_DONGU, word);      // ✅ Düzeltildi
    if (strcmp(word, "DÖNGÜ_BITIR") == 0) return createToken(TOKEN_YAPI_DONGU_BITIR, word); // ✅ Düzeltildi
    if (strcmp(word, "SON") == 0) return createToken(TOKEN_YAPI_SON, word);          // ✅ Düzeltildi
    
    // String fonksiyonları - SİL (aşağıdaki satırları)
    // ❌ BUNLARI SİL - String fonksiyonları TOKEN_YAPI_ISLEC_CAGIR değil, normal TOKEN_TANIMLAYICI!
    
    // Bulunamadı, sıradan tanımlayıcı
    return NULL;
}