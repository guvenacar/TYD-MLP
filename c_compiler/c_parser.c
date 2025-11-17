// c_compiler/c_parser.c
// (KILAVUZA UYUMLU SÜRÜM: Tanımlamalar hariç ; YOK)

#include "c_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Parser Durum Yönetimi ---
static Token* current_token = NULL;

// --- Hata Yönetimi ---
void parseError(const char* message, const char* expected) {
    fprintf(stderr, "HATA [Parser]: Sözdizimi Hatası!\n");
    fprintf(stderr, "Beklenen: %s\n", expected);
    if (current_token != NULL) {
        fprintf(stderr, "Bulunan: Tip %d (%s)\n", current_token->type, current_token->value ? current_token->value : "NULL");
    } else {
        fprintf(stderr, "Bulunan: Dosya Sonu veya NULL\n");
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
        char expected_str[20];
        sprintf(expected_str, "Tip %d", expected_type);
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
        } else {
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
    
    // 2. Değişken Tanımlama (Noktalı virgüllü)
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

    // 7. Atama veya İfade Komutu (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IDENTIFIER) {
        ASTNode* sol_node = ifade(); // 'birincil()' çağrılır

        // DURUM 7.1: ATAMA (örn: x = 5)
        // 'ifade()' bize bir AST_DEGISKEN (x) döndürdüyse
        if (sol_node->type == AST_DEGISKEN && current_token->type == TOKEN_ASSIGN) {
            consume(TOKEN_ASSIGN); // '=' tüket
            ASTNode* sag_ifade = ifade(); // Sağ tarafı (5) ayrıştır
            
            specs_check_no_semicolon("Atama komutu");

            // 'sol_node'u (AST_DEGISKEN) AST_ATAMA_KOMUTU'na dönüştür
            Token* ad_token = sol_node->degisken_data.ad; // Adı al
            
            ASTNode* atama_node = createAST_AtamaKomutu(ad_token, sag_ifade);
            
            // sol_node'un belleğini düzgün yönet
            free(ad_token->value);
            free(ad_token);
            free(sol_node);
            
            return atama_node;
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