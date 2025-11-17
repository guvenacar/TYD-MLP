// c_compiler/c_lexer.c

#include "c_lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h> // iswalpha, iswalnum için
#include <ctype.h>  // isspace için
#include <locale.h> // setlocale için

// --- Lexer Durum Yönetimi ---

// Kaynak kodu (Source Code) dize olarak tutulur.
static const char* source_code = NULL;

// Okuma pozisyonu (Lexer'ın kaynak kodda nerede olduğunu takip eder).
static int current_position = 0;
static int eof_reached = 0; // <-- YENİ: EOF bayrağı

// Satır ve kolon tracking (hata mesajları için)
static int current_line = 1;    // Satır numarası (1'den başlar)
static int current_column = 1;  // Kolon numarası (1'den başlar)

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

    // Pozisyon bilgisini ekle
    token->line = current_line;
    token->column = current_column;

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
    // UTF-8 desteği için locale ayarla
    setlocale(LC_ALL, "en_US.UTF-8");

    source_code = source;
    current_position = 0;
    eof_reached = 0; // <-- RESET

    // Pozisyon tracking'i reset et
    current_line = 1;
    current_column = 1;
}


// --- Ana Lexer Mantığı ---

/**
 * Pozisyon ilerletme helper fonksiyonu
 * current_position'ı 1 artırır ve line/column'u günceller
 */
static inline void advance_position() {
    if (source_code[current_position] == '\0') {
        return; // EOF'ta ilerleme yok
    }

    if (source_code[current_position] == '\n') {
        current_line++;
        current_column = 1;
    } else if (source_code[current_position] == '\t') {
        current_column += 4; // Tab = 4 space
    } else {
        current_column++;
    }

    current_position++;
}

/**
 * Tüm yorumları ve boşlukları atlayan helper fonksiyon
 * Hem tek satır (--) hem de çok satır ({- ... -}) yorumları destekler
 */
static void skip_whitespace_and_comments() {
    while (1) {
        // Boşlukları atla
        while (source_code[current_position] != '\0' && isspace(source_code[current_position])) {
            advance_position();
        }

        // Tek satır yorum: --
        if (source_code[current_position] == '-' && source_code[current_position + 1] == '-') {
            // Satır sonuna kadar atla
            while (source_code[current_position] != '\n' && source_code[current_position] != '\0') {
                advance_position();
            }
            continue; // Tekrar başa dön (boşluk ve yorum kontrolü için)
        }

        // Çok satır yorum: {- ... -}
        if (source_code[current_position] == '{' && source_code[current_position + 1] == '-') {
            // {- işaretini atla
            advance_position(); // {
            advance_position(); // -

            // -} bulana kadar devam et
            while (source_code[current_position] != '\0') {
                if (source_code[current_position] == '-' && source_code[current_position + 1] == '}') {
                    // -} işaretini atla
                    advance_position(); // -
                    advance_position(); // }
                    break;
                }
                advance_position();
            }
            continue; // Tekrar başa dön (boşluk ve yorum kontrolü için)
        }

        // Ne boşluk ne de yorum varsa, çık
        break;
    }
}

/**
 * Python'daki sonraki_tokeni_al() metoduna karşılık gelir.
 * Kaynak koddan bir sonraki tokeni okur ve döndürür.
 */
Token* getNextToken() {

    // 1. EOF zaten döndürüldüyse hemen EOF döndür.
    if (eof_reached) {
        return createToken(TOKEN_EOF, NULL);
    }

    // 2. Boşlukları ve Yorumları Atla
    skip_whitespace_and_comments();


    // 3. Dosya Sonuna (EOF) ulaşıldı
    if (source_code[current_position] == '\0') {
        eof_reached = 1; // Bayrağı ayarla
        return createToken(TOKEN_EOF, NULL);
    }

    // --- YENİ: UTF-8 UYUMLU KARAKTER OKUMA ---
    wchar_t current_wc = L'\0'; // Geniş karakter (wide character)
    int char_len = mbtowc(&current_wc, &source_code[current_position], 4); // En fazla 4 byte oku

    // mbtowc başarısız olsa bile UTF-8 multi-byte karakterler için devam et
    if (char_len <= 0) {
        // Eğer byte UTF-8 multi-byte başlangıcı ise (>= 0x80), devam et
        if ((unsigned char)source_code[current_position] >= 0x80) {
            current_wc = L' '; // Dummy value - identifier check'e geçsin
            char_len = 1;
        } else {
            // Gerçekten EOF
            eof_reached = 1;
            return createToken(TOKEN_EOF, NULL);
        }
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


    // UTF-8 harf kontrol (ASCII harf veya UTF-8 multi-byte: 0x80-0xFF)
    if (iswalpha(current_wc) || (unsigned char)source_code[current_position] >= 0x80) {
        int start = current_position;
        // İsimler harf, rakam, UTF-8 karakterler veya alt çizgi içerebilir
        while (source_code[current_position] != '\0') {
            unsigned char byte = (unsigned char)source_code[current_position];

            // ASCII harf/rakam/underscore
            if (isalnum(byte) || byte == '_') {
                current_position++;
                continue;
            }

            // UTF-8 multi-byte karakter başlangıcı (0xC0-0xFF)
            if (byte >= 0xC0) {
                // UTF-8 continuation bytes'ları da ekle
                current_position++;
                while (current_position < strlen(source_code)) {
                    unsigned char cont = (unsigned char)source_code[current_position];
                    if (cont >= 0x80 && cont < 0xC0) {  // Continuation byte (10xxxxxx)
                        current_position++;
                    } else {
                        break;
                    }
                }
                continue;
            }

            // Hiçbiri değilse dur
            break;
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

        // Escape sequence'leri işleyerek string'i oluştur
        char* value = (char*)malloc(1024); // Maksimum string uzunluğu
        int value_index = 0;

        // Kapanış tırnağına kadar oku ve escape sequence'leri dönüştür
        while (source_code[current_position] != '\0' && source_code[current_position] != quote_char) {
            // Escape sequence desteği (\n, \t, \", \\, vb.)
            if (source_code[current_position] == '\\' && source_code[current_position + 1] != '\0') {
                current_position++; // Backslash'i atla
                char escape_char = source_code[current_position];

                // Escape karakterini gerçek karaktere dönüştür
                switch (escape_char) {
                    case 'n':  value[value_index++] = '\n'; break;  // Newline
                    case 't':  value[value_index++] = '\t'; break;  // Tab
                    case 'r':  value[value_index++] = '\r'; break;  // Carriage return
                    case '\\': value[value_index++] = '\\'; break;  // Backslash
                    case '"':  value[value_index++] = '"';  break;  // Quote
                    case '\'': value[value_index++] = '\''; break;  // Single quote
                    case '0':  value[value_index++] = '\0'; break;  // Null character
                    default:
                        // Bilinmeyen escape - olduğu gibi ekle
                        value[value_index++] = '\\';
                        value[value_index++] = escape_char;
                        break;
                }
                current_position++;
            } else {
                value[value_index++] = source_code[current_position];
                current_position++;
            }
        }

        if (source_code[current_position] == '\0') {
            fprintf(stderr, "HATA [Lexer]: Kapanmamış string literal\n");
            free(value);
            return createToken(TOKEN_EOF, NULL);
        }

        value[value_index] = '\0'; // String'i sonlandır
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