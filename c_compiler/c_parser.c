// c_compiler/c_parser.c
// (KILAVUZA UYUMLU SÜRÜM: Tanımlamalar hariç ; YOK)

#include "c_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Parser Durum Yönetimi ---
static Token* current_token = NULL;

// Token tip ismini döndüren helper fonksiyon
static const char* getTokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_SAYI: return "SAYI";
        case TOKEN_METIN: return "METIN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_TANIMLA_SAYI: return "SAYISAL";
        case TOKEN_TANIMLA_METIN: return "METIN (tip)";
        case TOKEN_TANIMLA_BOOL: return "BOOL";
        case TOKEN_YAPI_YAZDIR: return "YAZDIR";
        case TOKEN_YAPI_KOSUL_EGER: return "EĞER";
        case TOKEN_YAPI_KOSUL_ISE: return "İSE";
        case TOKEN_YAPI_KOSUL_DEGILSE: return "DEĞİLSE";
        case TOKEN_YAPI_ISLEC: return "İŞLEÇ";
        case TOKEN_YAPI_DONUS: return "DÖNÜŞ";
        case TOKEN_YAPI_DONGU: return "DÖNGÜ";
        case TOKEN_YAPI_DONGU_BITIR: return "DÖNGÜ_BITIR";
        case TOKEN_YAPI_SON: return "SON";
        case TOKEN_LEFT_PAREN: return "(";
        case TOKEN_RIGHT_PAREN: return ")";
        case TOKEN_COMMA: return ",";
        case TOKEN_ASSIGN: return "=";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_MUL: return "*";
        case TOKEN_DIV: return "/";
        case TOKEN_GT: return ">";
        case TOKEN_LT: return "<";
        case TOKEN_OP_ESIT_KARSILASTIRMA: return "==";
        case TOKEN_NOT_ESIT: return "!=";
        case TOKEN_GTE: return ">=";
        case TOKEN_LTE: return "<=";
        default: return "UNKNOWN";
    }
}

// --- Hata Yönetimi ---
void parseError(const char* message, const char* expected) {
    fprintf(stderr, "\n╔════════════════════════════════════════════════════════════╗\n");
    fprintf(stderr, "║ HATA [Parser]: Sözdizimi Hatası!                          ║\n");
    fprintf(stderr, "╚════════════════════════════════════════════════════════════╝\n\n");

    if (current_token != NULL) {
        fprintf(stderr, "📍 Konum: Satır %d, Kolon %d\n\n", current_token->line, current_token->column);
        fprintf(stderr, "❌ Bulunan: ");
        if (current_token->value) {
            fprintf(stderr, "\"%s\" (%s)\n", current_token->value, getTokenTypeName(current_token->type));
        } else {
            fprintf(stderr, "%s\n", getTokenTypeName(current_token->type));
        }
    } else {
        fprintf(stderr, "📍 Konum: Dosya sonu\n\n");
        fprintf(stderr, "❌ Bulunan: Dosya Sonu veya NULL\n");
    }

    fprintf(stderr, "✓ Beklenen: %s\n\n", expected);

    if (message && strlen(message) > 0) {
        fprintf(stderr, "💡 Açıklama: %s\n\n", message);
    }

    exit(EXIT_FAILURE);
}

// Kural Kontrolü (Hata ayıklama için)
void specs_check_no_semicolon(const char* komut_adi) {
    if (current_token && current_token->type == TOKEN_SEMICOLON) {
        fprintf(stderr, "SPECS.md İhlali: %s sonunda ';' kullanılamaz!\n", komut_adi);
        exit(1);
    }
}

// --- Token Yönetimi ---
void consume(TokenType expected_type) {
    if (current_token == NULL) {
        parseError("Dosya beklenenden önce bitti.", "Herhangi bir Token");
    }
    if (current_token->type == expected_type) {
        if (current_token->value != NULL) {
            free(current_token->value);
        }
        free(current_token);
        current_token = getNextToken();
    } else {
        char expected_str[100];
        sprintf(expected_str, "%s (%s)", getTokenTypeName(expected_type),
                expected_type == TOKEN_SEMICOLON ? "SAYISAL, METIN, BOOL değişken tanımlarında gerekli" : "Token");
        parseError("Token tipi uyuşmuyor.", expected_str);
    }
}

// --- AST Düğümü Oluşturucular ---
// (Dosyanızda bulunan tüm createAST_... fonksiyonları buradadır)
// (createAST_Sayi, createAST_Metin, createAST_Degisken, createAST_IslecCagirma, 
//  createAST_DegiskenTanimlama, createAST_KosulKomutu, createAST_IslecTanimlama...)

ASTNode* createAST_Sayi(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_SAYI;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

ASTNode* createAST_Metin(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_METIN;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

ASTNode* createAST_Degisken(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_DEGISKEN;
    node->degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->degisken_data.ad->type = token->type;
    node->degisken_data.ad->value = strdup(token->value);
    return node;
}

ASTNode* createAST_IslecCagirma(Token* ad_token, ASTNode** argumanlar, int arguman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ISLEC_CAGIRMA;
    node->islec_cagirma_data.hedef_ad = (Token*)malloc(sizeof(Token));
    node->islec_cagirma_data.hedef_ad->type = ad_token->type;
    node->islec_cagirma_data.hedef_ad->value = strdup(ad_token->value);
    node->islec_cagirma_data.argumanlar = argumanlar;
    node->islec_cagirma_data.arguman_sayisi = arguman_sayisi;
    return node;
}

ASTNode* createAST_DegiskenTanimlama(Token* tip, Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_DEGISKEN_TANIMLAMA;
    node->tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.tip->type = tip->type;
    node->tanimlama_data.tip->value = strdup(tip->value);
    node->tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.ad->type = ad->type;
    node->tanimlama_data.ad->value = strdup(ad->value);
    node->tanimlama_data.ifade = ifade;
    return node;
}

ASTNode* createAST_AtamaKomutu(Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ATAMA_KOMUTU;
    node->atama_data.ad = (Token*)malloc(sizeof(Token));
    node->atama_data.ad->type = ad->type;
    node->atama_data.ad->value = strdup(ad->value);
    node->atama_data.ifade = ifade;
    return node;
}

ASTNode* createAST_ArrayTanimlama(Token* tip, Token* ad, ASTNode* boyut) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_TANIMLAMA;
    node->array_tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.tip->type = tip->type;
    node->array_tanimlama_data.tip->value = strdup(tip->value);
    node->array_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.ad->type = ad->type;
    node->array_tanimlama_data.ad->value = strdup(ad->value);
    node->array_tanimlama_data.boyut = boyut;
    return node;
}

ASTNode* createAST_ArrayErisim(Token* ad, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ERISIM;
    node->array_erisim_data.ad = (Token*)malloc(sizeof(Token));
    node->array_erisim_data.ad->type = ad->type;
    node->array_erisim_data.ad->value = strdup(ad->value);
    node->array_erisim_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ArrayAtama(Token* ad, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ATAMA;
    node->array_atama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_atama_data.ad->type = ad->type;
    node->array_atama_data.ad->value = strdup(ad->value);
    node->array_atama_data.indeks = indeks;
    node->array_atama_data.deger = deger;
    return node;
}

// Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
ASTNode* createAST_StructTanimlama(Token* ad, Token** field_tipleri, Token** field_adlari, int field_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_TANIMLAMA;
    node->struct_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_tanimlama_data.ad->type = ad->type;
    node->struct_tanimlama_data.ad->value = strdup(ad->value);
    node->struct_tanimlama_data.field_tipleri = field_tipleri;
    node->struct_tanimlama_data.field_adlari = field_adlari;
    node->struct_tanimlama_data.field_sayisi = field_sayisi;
    return node;
}

// Struct Field Access (p.x)
ASTNode* createAST_StructFieldAccess(Token* struct_ad, Token* field_ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ACCESS;
    node->struct_field_access_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.struct_ad->type = struct_ad->type;
    node->struct_field_access_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_access_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.field_ad->type = field_ad->type;
    node->struct_field_access_data.field_ad->value = strdup(field_ad->value);
    return node;
}

// Struct Field Atama (p.x = 10;)
ASTNode* createAST_StructFieldAtama(Token* struct_ad, Token* field_ad, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ATAMA;
    node->struct_field_atama_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.struct_ad->type = struct_ad->type;
    node->struct_field_atama_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_atama_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.field_ad->type = field_ad->type;
    node->struct_field_atama_data.field_ad->value = strdup(field_ad->value);
    node->struct_field_atama_data.deger = deger;
    return node;
}

// Struct Değişken (Nokta p;)
ASTNode* createAST_StructDegisken(Token* struct_tip, Token* ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_DEGISKEN;
    node->struct_degisken_data.struct_tip = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.struct_tip->type = struct_tip->type;
    node->struct_degisken_data.struct_tip->value = strdup(struct_tip->value);
    node->struct_degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.ad->type = ad->type;
    node->struct_degisken_data.ad->value = strdup(ad->value);
    return node;
}

ASTNode* createAST_KosulKomutu(ASTNode* kosul, ASTNode* ise_blok, ASTNode* degilse_blok) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_KOSUL_KOMUTU;
    node->kosul_data.kosul = kosul;
    node->kosul_data.ise_blok = ise_blok;
    node->kosul_data.degilse_blok = degilse_blok;
    return node;
}

ASTNode* createAST_IslecTanimlama(Token* ad, Token** parametreler, int parametre_sayisi, ASTNode* govde) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ISLEC_TANIMLAMA;
    node->islec_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->islec_tanimlama_data.ad->type = ad->type;
    node->islec_tanimlama_data.ad->value = strdup(ad->value);
    node->islec_tanimlama_data.parametreler = parametreler;
    node->islec_tanimlama_data.parametre_sayisi = parametre_sayisi;
    node->islec_tanimlama_data.govde = govde;
    return node;
}


// --- Ayrıştırma Fonksiyonları (İleri Bildirimler) ---
ASTNode* ifade();
ASTNode* birincil();
ASTNode* ikili_islem(int onceki_oncelik);
ASTNode* komut();
ASTNode* blok();
ASTNode* kosul_komutu();
ASTNode* dongu_komutu();
ASTNode* islec_tanimlama();
ASTNode* donus_komutu();


int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 10;
        case TOKEN_MUL:
        case TOKEN_DIV:
            return 20;
        case TOKEN_OP_ESIT_KARSILASTIRMA:
        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_GTE:
        case TOKEN_LTE:
        case TOKEN_NOT_ESIT:
            return 5;
        default:
            return 0;
    }
}

ASTNode* ifade() {
    return ikili_islem(0);
}

ASTNode* birincil() {
    if (current_token == NULL) {
        parseError("Bir ifade bekleniyordu.", "Sayı, Metin veya Değişken");
    }

    if (current_token->type == TOKEN_SAYI) {
        ASTNode* node = createAST_Sayi(current_token);
        consume(TOKEN_SAYI);
        return node;
    }

    if (current_token->type == TOKEN_METIN) {
        ASTNode* node = createAST_Metin(current_token);
        consume(TOKEN_METIN);
        return node;
    }

    if (current_token->type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN);
        ASTNode* node = ifade();
        if (current_token->type != TOKEN_RIGHT_PAREN) {
            parseError("Kapanış Parantezi bekleniyordu.", ")");
        }
        consume(TOKEN_RIGHT_PAREN);
        return node;
    }

    if (current_token->type == TOKEN_IDENTIFIER) {
        Token ad_token_kopya;
        ad_token_kopya.type = current_token->type;
        ad_token_kopya.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Fonksiyon çağrısı mı? (func(args))
        if (current_token->type == TOKEN_LEFT_PAREN) {
            consume(TOKEN_LEFT_PAREN);
            ASTNode** arguman_listesi = NULL;
            int a_sayisi = 0;
            if (current_token->type != TOKEN_RIGHT_PAREN) {
                arguman_listesi = (ASTNode**)malloc(sizeof(ASTNode*) * 10);
                do {
                    arguman_listesi[a_sayisi] = ifade();
                    a_sayisi++;
                } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
            }
            consume(TOKEN_RIGHT_PAREN);
            ASTNode* call_node = createAST_IslecCagirma(&ad_token_kopya, arguman_listesi, a_sayisi);
            free(ad_token_kopya.value);
            return call_node;
        }
        // Array erişimi mi? (arr[index])
        else if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            ASTNode* indeks = ifade();
            consume(TOKEN_RIGHT_BRACKET);
            ASTNode* array_erisim = createAST_ArrayErisim(&ad_token_kopya, indeks);
            free(ad_token_kopya.value);
            return array_erisim;
        }
        // Struct field access mi? (p.x)
        else if (current_token->type == TOKEN_DOT) {
            consume(TOKEN_DOT);
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field adı", "IDENTIFIER");
            }
            Token field_ad;
            field_ad.type = current_token->type;
            field_ad.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);
            ASTNode* field_access = createAST_StructFieldAccess(&ad_token_kopya, &field_ad);
            free(ad_token_kopya.value);
            free(field_ad.value);
            return field_access;
        }
        // Normal değişken
        else {
            ASTNode* var_node = createAST_Degisken(&ad_token_kopya);
            free(ad_token_kopya.value);
            return var_node;
        }
    }

    parseError("Birincil ifade bekleniyordu.", "Sayı, Parantez veya Tanımlayıcı");
    return NULL;
}

ASTNode* ikili_islem(int onceki_oncelik) {
    ASTNode* sol = birincil();
    if (sol == NULL) return NULL;

    while (1) {
        TokenType op_type = current_token->type;
        int oncelik = get_precedence(op_type);
        if (oncelik <= onceki_oncelik) {
            break;
        }
        consume(op_type);
        ASTNode* sag = ikili_islem(oncelik);
        ASTNode* yeni_node = (ASTNode*)malloc(sizeof(ASTNode));
        if (yeni_node == NULL) return NULL;
        yeni_node->type = AST_IKILI_ISLEM;
        yeni_node->ikili_islem_data.sol = sol;
        yeni_node->ikili_islem_data.sag = sag;
        yeni_node->ikili_islem_data.operator_type = op_type;
        sol = yeni_node;
    }
    return sol;
}

ASTNode* komut() {
    // 0. YAPI - Struct Tanımlama (YAPI Nokta İSE ... SON)
    if (current_token->type == TOKEN_YAPI_STRUCT) {
        consume(TOKEN_YAPI_STRUCT);

        // Struct adı
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Struct adı", "IDENTIFIER");
        }
        Token struct_ad;
        struct_ad.type = current_token->type;
        struct_ad.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // İSE
        consume(TOKEN_YAPI_KOSUL_ISE);

        // Field'ları parse et
        Token** field_tipleri = (Token**)malloc(sizeof(Token*) * 20);  // Max 20 field
        Token** field_adlari = (Token**)malloc(sizeof(Token*) * 20);
        int field_sayisi = 0;

        // SON'a kadar field'ları oku
        while (current_token->type != TOKEN_YAPI_SON) {
            // Field tipi (SAYISAL, METIN, vb.)
            if (current_token->type != TOKEN_TANIMLA_SAYI &&
                current_token->type != TOKEN_TANIMLA_METIN &&
                current_token->type != TOKEN_TANIMLA_BOOL) {
                parseError("Field tipi", "SAYISAL/METIN/BOOL");
            }

            Token* field_tip = (Token*)malloc(sizeof(Token));
            field_tip->type = current_token->type;
            field_tip->value = strdup(current_token->value);
            field_tipleri[field_sayisi] = field_tip;
            consume(current_token->type);

            // Field adı
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field adı", "IDENTIFIER");
            }
            Token* field_ad = (Token*)malloc(sizeof(Token));
            field_ad->type = current_token->type;
            field_ad->value = strdup(current_token->value);
            field_adlari[field_sayisi] = field_ad;
            consume(TOKEN_IDENTIFIER);

            // Noktalı virgül
            consume(TOKEN_SEMICOLON);

            field_sayisi++;
        }

        // SON
        consume(TOKEN_YAPI_SON);

        ASTNode* struct_node = createAST_StructTanimlama(&struct_ad, field_tipleri, field_adlari, field_sayisi);
        free(struct_ad.value);
        return struct_node;
    }

    // TODO: Struct Değişken Tanımlama (Nokta p;)
    // Şimdilik struct değişkenleri global olarak veya typedef ile tanımlanabilir
    // İleride peek_token() eklenip IDENTIFIER IDENTIFIER SEMICOLON pattern'i yakalanabilir

    // 1. YAZDIR (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_YAZDIR) {
        consume(TOKEN_YAPI_YAZDIR);
        ASTNode* ifade_dugumu = ifade();
        specs_check_no_semicolon("YAZDIR");
        ASTNode* yazdir_node = (ASTNode*)malloc(sizeof(ASTNode));
        yazdir_node->type = AST_YAZDIR_KOMUTU;
        yazdir_node->tek_ifade_data.ifade = ifade_dugumu;
        return yazdir_node;
    }

    // 3. Değişken Tanımlama (Noktalı virgüllü)
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL)
    {
        Token tip_token;
        tip_token.type = current_token->type;
        tip_token.value = strdup(current_token->value);
        consume(current_token->type);

        Token ad_token;
        if (current_token->type != TOKEN_IDENTIFIER) parseError("Değişken adı", "IDENTIFIER");
        ad_token.type = current_token->type;
        ad_token.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Array tanımlaması mı? (SAYISAL arr[10];)
        if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            ASTNode* boyut = ifade();  // Array boyutu
            consume(TOKEN_RIGHT_BRACKET);
            consume(TOKEN_SEMICOLON);

            ASTNode* array_node = createAST_ArrayTanimlama(&tip_token, &ad_token, boyut);
            free(tip_token.value);
            free(ad_token.value);
            return array_node;
        }

        // Normal değişken tanımlaması (SAYISAL x = 5;)
        consume(TOKEN_ASSIGN);
        ASTNode* ifade_dugumu = ifade();

        // ✅ KURAL: Tanımlamalar noktalı virgül ALIR
        consume(TOKEN_SEMICOLON);

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 3. EĞER (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_KOSUL_EGER) {
        return kosul_komutu(); // Kendi içinde 'SON'u yönetir
    }

    // 4. DÖNGÜ (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONGU) {
        return dongu_komutu(); // Kendi içinde 'SON'u yönetir
    }
    
    // 5. DÖNGÜ_BITIR (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONGU_BITIR) {
        consume(TOKEN_YAPI_DONGU_BITIR);
        specs_check_no_semicolon("DÖNGÜ_BITIR");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_DONGU_BITIR_KOMUTU;
        return node;
    }
    
    // 6. DÖNÜŞ (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONUS) {
        return donus_komutu(); // Kendi içinde yönetir
    }

    // 7. İŞLEÇ (Function Definition) (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_ISLEC) {
        return islec_tanimlama(); // Kendi içinde yönetir
    }

    // 8. Atama veya İfade Komutu (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IDENTIFIER) {
        ASTNode* sol_node = ifade(); // 'birincil()' çağrılır

        // DURUM 7.1: ATAMA (örn: x = 5, arr[i] = 5, p.x = 10)
        // 'ifade()' bize bir AST_DEGISKEN (x), AST_ARRAY_ERISIM veya AST_STRUCT_FIELD_ACCESS döndürdüyse
        if ((sol_node->type == AST_DEGISKEN || sol_node->type == AST_ARRAY_ERISIM || sol_node->type == AST_STRUCT_FIELD_ACCESS) &&
            current_token->type == TOKEN_ASSIGN) {
            consume(TOKEN_ASSIGN); // '=' tüket
            ASTNode* sag_ifade = ifade(); // Sağ tarafı (5) ayrıştır

            specs_check_no_semicolon("Atama komutu");

            // Normal değişken ataması
            if (sol_node->type == AST_DEGISKEN) {
                Token* ad_token = sol_node->degisken_data.ad; // Adı al
                ASTNode* atama_node = createAST_AtamaKomutu(ad_token, sag_ifade);

                // sol_node'un belleğini düzgün yönet
                free(ad_token->value);
                free(ad_token);
                free(sol_node);

                return atama_node;
            }
            // Array erişimi ataması (arr[i] = value)
            else if (sol_node->type == AST_ARRAY_ERISIM) {
                specs_check_no_semicolon("Array atama komutu");

                // sol_node AST_ARRAY_ERISIM, ondan ad ve indeks al
                Token* array_ad = sol_node->array_erisim_data.ad;
                ASTNode* array_indeks = sol_node->array_erisim_data.indeks;

                // Yeni AST_ARRAY_ATAMA node oluştur
                ASTNode* array_atama = createAST_ArrayAtama(array_ad, array_indeks, sag_ifade);

                // sol_node'u temizle (ama içindekiler array_atama'da kullanılıyor, dikkat!)
                free(sol_node); // Sadece wrapper'ı free et

                return array_atama;
            }
            // Struct field ataması (p.x = 10)
            else if (sol_node->type == AST_STRUCT_FIELD_ACCESS) {
                specs_check_no_semicolon("Struct field atama komutu");

                // sol_node AST_STRUCT_FIELD_ACCESS, ondan struct_ad ve field_ad al
                Token* struct_ad = sol_node->struct_field_access_data.struct_ad;
                Token* field_ad = sol_node->struct_field_access_data.field_ad;

                // Yeni AST_STRUCT_FIELD_ATAMA node oluştur
                ASTNode* field_atama = createAST_StructFieldAtama(struct_ad, field_ad, sag_ifade);

                // sol_node'u temizle
                free(sol_node);

                return field_atama;
            }
        }
        
        // DURUM 7.2: İFADE KOMUTU (örn: test())
        // 'ifade()' bize bir AST_ISLEC_CAGIRMA (test()) döndürdüyse
        if (sol_node->type == AST_ISLEC_CAGIRMA) {
            specs_check_no_semicolon("İfade komutu (fonksiyon çağrısı)");
            return sol_node; // 'test()' çağrısını komut olarak döndür
        }

        // Hata: 'x' (tek başına) veya 'x + 5' (ifade) bir komut değildir.
        parseError("Geçersiz komut. Atama (=) veya noktalı virgülsüz fonksiyon çağrısı bekleniyordu.", "Geçersiz Komut");
    }

    // Kopyala-yapıştır hatası (ve diğer hatalar) kaldırıldı.
    // Hiçbiri eşleşmezse bu bir hatadır.
    parseError("Komut bekleniyordu.", "YAZDIR, Tip, Tanımlayıcı, EĞER, DÖNGÜ veya İŞLEÇ");
    return NULL;
}

ASTNode* kosul_komutu() {
    consume(TOKEN_YAPI_KOSUL_EGER);
    ASTNode* kosul_ifadesi = ifade(); 
    consume(TOKEN_YAPI_KOSUL_ISE);
    ASTNode* ise_blogu = blok();
    ASTNode* degilse_blogu = NULL;
    if (current_token->type == TOKEN_YAPI_KOSUL_DEGILSE) {
        consume(TOKEN_YAPI_KOSUL_DEGILSE);
        degilse_blogu = blok();
    }
    consume(TOKEN_YAPI_SON);
    specs_check_no_semicolon("EĞER SON");
    return createAST_KosulKomutu(kosul_ifadesi, ise_blogu, degilse_blogu);
}

ASTNode* dongu_komutu() {
    consume(TOKEN_YAPI_DONGU);
    ASTNode* govde_blogu = blok(); 
    consume(TOKEN_YAPI_SON);
    specs_check_no_semicolon("DÖNGÜ SON");
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_DONGU_KOMUTU;
    node->dongu_data.govde = govde_blogu;
    return node;
}

ASTNode* islec_tanimlama() {
    consume(TOKEN_YAPI_ISLEC); 
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("İşleç adı bekleniyordu.", "IDENTIFIER");
    }
    Token ad_token;
    ad_token.type = current_token->type;
    ad_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER); 

    consume(TOKEN_LEFT_PAREN);
    Token** parametre_listesi = NULL;
    int p_sayisi = 0;
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        parametre_listesi = (Token**)malloc(sizeof(Token*) * 10);
        do {
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Parametre adı bekleniyordu.", "IDENTIFIER");
            }
            Token* param_token = (Token*)malloc(sizeof(Token));
            param_token->type = current_token->type;
            param_token->value = strdup(current_token->value);
            parametre_listesi[p_sayisi] = param_token;
            p_sayisi++;
            consume(TOKEN_IDENTIFIER);
        } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
    }
    consume(TOKEN_RIGHT_PAREN);

    consume(TOKEN_YAPI_KOSUL_ISE);
    ASTNode* govde_blogu = blok(); 
    consume(TOKEN_YAPI_SON);
    
    // Kılavuza göre SON İŞLEÇ değil, sadece SON
    specs_check_no_semicolon("İŞLEÇ SON");
    
    ASTNode* node = createAST_IslecTanimlama(&ad_token, parametre_listesi, p_sayisi, govde_blogu);
    free(ad_token.value);
    return node;
}

ASTNode* donus_komutu() {
    consume(TOKEN_YAPI_DONUS);
    ASTNode* ifade_dugumu = ifade();
    specs_check_no_semicolon("DÖNÜŞ komutu");
    ASTNode* donus_node = (ASTNode*)malloc(sizeof(ASTNode));
    donus_node->type = AST_DONUS_KOMUTU;
    donus_node->tek_ifade_data.ifade = ifade_dugumu;
    return donus_node;
}

ASTNode* blok() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) exit(EXIT_FAILURE);
    node->type = AST_BLOK;
    node->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    node->blok_data.sayisi = 0;

    while (current_token != NULL && // NULL kontrolü eklendi
           current_token->type != TOKEN_EOF &&
           current_token->type != TOKEN_YAPI_SON &&
           current_token->type != TOKEN_YAPI_KOSUL_DEGILSE) 
    {
        // Boş noktalı virgülleri atla (Sadece tanımlamalar için)
        if (current_token->type == TOKEN_SEMICOLON) {
            consume(TOKEN_SEMICOLON);
            continue;
        }

        ASTNode* k = komut();
        if (k != NULL) {
            node->blok_data.komutlar[node->blok_data.sayisi++] = k;
        }
    }
    return node;
}

// --- Ana Ayrıştırma Fonksiyonu ---

ASTNode* parse(const char* source_code) {
    initLexer(source_code);
    current_token = getNextToken();

    ASTNode* root = (ASTNode*)malloc(sizeof(ASTNode));
    root->type = AST_BLOK;
    root->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    root->blok_data.sayisi = 0;

    while (current_token != NULL && current_token->type != TOKEN_EOF) {
        // 'blok' fonksiyonu EOF görene kadar ana kod akışını okur
        root->blok_data.komutlar[root->blok_data.sayisi++] = komut();
    }
    return root;
}