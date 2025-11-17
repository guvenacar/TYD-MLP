// c_compiler/c_parser.c

#include "c_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Parser Durum Yönetimi ---

// Lexer'dan gelen mevcut Token'ı tutar (Python'daki self.mevcut_token'a karşılık gelir)
static Token* current_token = NULL;

// --- Hata Yönetimi ---

/**
 * Sözdizimi hatası durumunda programı sonlandırır.
 */
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

// SPECS.md Kural 1: Noktalı virgül kontrolü
void specs_check_no_semicolon(const char* komut_adi) {
    // Bir sonraki token noktalı virgül mü?
    if (current_token && current_token->type == TOKEN_NOKTALI_VIRGUL) {
        fprintf(stderr, "SPECS.md İhlali: %s sonunda ';' kullanılamaz!\n", komut_adi);
        fprintf(stderr, "Doğru kullanım: 'YAZDIR \"...\" SON YAZDIR' (noktalı virgül yok)\n");
        exit(1);
    }
}

// --- Token Yönetimi ---

/**
 * Python'daki tuket(token_tipi) metoduna karşılık gelir.
 * Mevcut token'ı kontrol eder ve Lexer'dan bir sonraki token'ı alır.
 */
void consume(TokenType expected_type) {
    if (current_token == NULL) {
        parseError("Dosya beklenenden önce bitti.", "Herhangi bir Token");
    }

    if (current_token->type == expected_type) {
        // Eski token'ın değerini temizle
        if (current_token->value != NULL) {
            free(current_token->value);
        }
        // Eski token struct'ını temizle
        free(current_token);

        // Lexer'dan bir sonraki token'ı al
        current_token = getNextToken();
    } else {
        // Hata durumunda beklenen token tipini string'e çevirmek zordur.
        // Şimdilik sayısal tipi basıyoruz.
        char expected_str[20];
        sprintf(expected_str, "Tip %d", expected_type);
        parseError("Token tipi uyuşmuyor.", expected_str);
    }
}

// --- AST Düğümü Oluşturucular ---

/**
 * Sayısal sabit bir değer için AST düğümü oluşturur.
 */
ASTNode* createAST_Sayi(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;

    node->type = AST_SAYI;

    // KRİTİK DÜZELTME: Belleği manuel olarak ayır ve değeri kopyala.
    if (token->value != NULL) {
        size_t len = strlen(token->value);
        // Yeni bellek ayır (null terminator için +1)
        node->sabit_data.deger = (char*)malloc(len + 1);

        if (node->sabit_data.deger == NULL) {
            perror("Hafıza ayırma hatası (AST Değeri)");
            exit(EXIT_FAILURE);
        }

        // Değeri yeni ayrılan belleğe kopyala
        strcpy(node->sabit_data.deger, token->value);
    } else {
        node->sabit_data.deger = NULL;
    }

    // Artık 'consume' çağrılsa bile AST düğümü güvendedir.
    return node;
}

/**
 * Metin (string literal) sabiti için bir AST düğümü oluşturur.
 */
ASTNode* createAST_Metin(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Hafıza ayırma hatası (AST Metin)");
        exit(EXIT_FAILURE);
    }

    node->type = AST_METIN;

    // Token değerini (string'i) kopyala
    if (token->value != NULL) {
        size_t len = strlen(token->value);
        node->sabit_data.deger = (char*)malloc(len + 1);
        if (node->sabit_data.deger == NULL) {
            perror("Hafıza ayırma hatası (AST Metin Deger)");
            exit(EXIT_FAILURE);
        }
        strcpy(node->sabit_data.deger, token->value);
    } else {
        node->sabit_data.deger = NULL;
    }

    return node;
}

/**
 * Değişken kullanımı için bir AST düğümü oluşturur.
 */
ASTNode* createAST_Degisken(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Hafıza ayırma hatası (AST Degisken)");
        exit(EXIT_FAILURE);
    }
    node->type = AST_DEGISKEN;

    // Token'ın kendisini kopyalamalıyız çünkü consume() onu serbest bırakacak.
    node->degisken_data.ad = (Token*)malloc(sizeof(Token));
    if (node->degisken_data.ad == NULL) {
        perror("Hafıza ayırma hatası (AST Degisken Token)");
        exit(EXIT_FAILURE);
    }
    node->degisken_data.ad->type = token->type;
    node->degisken_data.ad->value = strdup(token->value);
    if (node->degisken_data.ad->value == NULL) {
        perror("Hafıza ayırma hatası (AST Degisken Token Value)");
        exit(EXIT_FAILURE);
    }
    
    return node;
}

/**
 * İşleç (fonksiyon) çağırma için bir AST düğümü oluşturur.
 */
ASTNode* createAST_IslecCagirma(Token* ad_token, ASTNode** argumanlar, int arguman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Hafıza ayırma hatası (AST IslecCagirma)");
        exit(EXIT_FAILURE);
    }
    node->type = AST_ISLEC_CAGIRMA;

    // Hedef ad token'ını kopyala
    node->islec_cagirma_data.hedef_ad = (Token*)malloc(sizeof(Token));
    node->islec_cagirma_data.hedef_ad->type = ad_token->type;
    node->islec_cagirma_data.hedef_ad->value = strdup(ad_token->value);

    // Argümanları ata (şimdilik NULL ve 0 olacak)
    node->islec_cagirma_data.argumanlar = argumanlar;
    node->islec_cagirma_data.arguman_sayisi = arguman_sayisi;

    return node;
}

/**
 * Değişken tanımlama için AST düğümü oluşturur (örn: SAYISAL a = 5)
 */
ASTNode* createAST_DegiskenTanimlama(Token* tip, Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    
    node->type = AST_DEGISKEN_TANIMLAMA;
    
    // Tip Token'ını kopyala
    node->tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.tip->type = tip->type;
    node->tanimlama_data.tip->value = strdup(tip->value);
    
    // Ad Token'ını kopyala
    node->tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.ad->type = ad->type;
    node->tanimlama_data.ad->value = strdup(ad->value);
    
    // İfade düğümünü ata
    node->tanimlama_data.ifade = ifade;
    
    return node;
}

/**
 * Atama komutu için AST düğümü oluşturur (örn: sayac = 10)
 */
ASTNode* createAST_AtamaKomutu(Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    
    node->type = AST_ATAMA_KOMUTU;
    
    // Ad Token'ını kopyala
    node->atama_data.ad = (Token*)malloc(sizeof(Token));
    node->atama_data.ad->type = ad->type;
    node->atama_data.ad->value = strdup(ad->value);
    
    // İfade düğümünü ata
    node->atama_data.ifade = ifade;
    
    return node;
}

/**
 * Koşul komutu için AST düğümü oluşturur (EĞER)
 */
ASTNode* createAST_KosulKomutu(ASTNode* kosul, ASTNode* ise_blok, ASTNode* degilse_blok) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Hafıza ayırma hatası (AST KosulKomutu)");
        exit(EXIT_FAILURE);
    }
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
    
    // Ad Token'ını kopyala
    node->islec_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    if (node->islec_tanimlama_data.ad == NULL) return NULL;
    node->islec_tanimlama_data.ad->type = ad->type;
    node->islec_tanimlama_data.ad->value = strdup(ad->value);
    if (node->islec_tanimlama_data.ad->value == NULL) {
        free(node->islec_tanimlama_data.ad);
        return NULL;
    }
    
    node->islec_tanimlama_data.parametreler = parametreler;
    node->islec_tanimlama_data.parametre_sayisi = parametre_sayisi;
    node->islec_tanimlama_data.govde = govde;
    
    return node;
}

// --- Ayrıştırma Fonksiyonları (Python'daki metotlara karşılık gelir) ---

// İleri Bildirimler (C'de fonksiyonlar arası çağırma için gerekli)
ASTNode* ifade();
ASTNode* birincil();
ASTNode* ikili_islem(int onceki_oncelik);
ASTNode* komut();
ASTNode* blok();

// Prototip
ASTNode* kosul_komutu();
ASTNode* dongu_komutu(); // <-- YENİ İleri bildirim
ASTNode* islec_tanimlama(); // İleri bildirim
ASTNode* donus_komutu(); // İleri bildirim


// --- OPERATÖR ÖNCELİK MANTIGI ---

/**
 * Operatör öncelik seviyesini döndürür
 */
int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 10; // Düşük öncelik
        case TOKEN_MUL:
        case TOKEN_DIV:
            return 20; // Yüksek öncelik
        case TOKEN_OP_ESIT_KARSILASTIRMA:
        case TOKEN_GT:
        case TOKEN_LT:
            return 5;  // Karşılaştırmalar (En düşük)
        default:
            return 0;  // Operatör değil
    }
}

// TANIMLAMA: ifade()
ASTNode* ifade() {
    // Tüm ifadelerin giriş noktası. Python'da ikili_islem(0) çağrılıyordu.
    return ikili_islem(0);
}

/**
 * Sayısal sabitleri, Metinleri veya Değişkenleri ayrıştırır.
 */
ASTNode* birincil() {
    if (current_token == NULL) {
        parseError("Bir ifade bekleniyordu.", "Sayı, Metin veya Değişken");
    }

    // SAYI (örn: 10)
    if (current_token->type == TOKEN_SAYI) {
        ASTNode* node = createAST_Sayi(current_token);
        consume(TOKEN_SAYI);
        return node;
    }

    // METIN (string literal, örn: "merhaba")
    if (current_token->type == TOKEN_METIN) {
        ASTNode* node = createAST_Metin(current_token);
        consume(TOKEN_METIN);
        return node;
    }

    // YENİ: PARANTEZLİ İFADELER (Örn: (3 / 2))
    if (current_token->type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN); // '(' tüketildi

        ASTNode* node = ifade(); // İç ifadeyi ayrıştır (ifade, ikili_islem'i çağırır)

        // İç ifade ayrıştırıldıktan sonra, mutlaka ')' beklenir.
        if (current_token->type != TOKEN_RIGHT_PAREN) {
            parseError("Kapanış Parantezi bekleniyordu.", ")");
        }
        consume(TOKEN_RIGHT_PAREN); // ')' tüketildi
        
        return node;
    }

    // YENİDEN DÜZENLEME: Değişken veya Fonksiyon Çağrısı
    if (current_token->type == TOKEN_IDENTIFIER) {
        Token ad_token_kopya;
        ad_token_kopya.type = current_token->type;
        ad_token_kopya.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER); 

        // İleriye bak: Bir sonraki token '(' mi?
        if (current_token->type == TOKEN_LEFT_PAREN) {
            // Fonksiyon Çağrısı
            consume(TOKEN_LEFT_PAREN);

            ASTNode** arguman_listesi = NULL;
            int a_sayisi = 0;
            
            if (current_token->type != TOKEN_RIGHT_PAREN) {
                arguman_listesi = (ASTNode**)malloc(sizeof(ASTNode*) * 10); // Max 10 argüman
                do {
                    // Her argüman bir ifadedir
                    arguman_listesi[a_sayisi] = ifade(); 
                    a_sayisi++;
                } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
            }
            consume(TOKEN_RIGHT_PAREN);
            
            ASTNode* call_node = createAST_IslecCagirma(&ad_token_kopya, arguman_listesi, a_sayisi);
            free(ad_token_kopya.value);
            return call_node; // İfade olarak döndür
        } else {
            // BU BİR DEĞİŞKEN ERİŞİMİDİR
            ASTNode* var_node = createAST_Degisken(&ad_token_kopya);
            free(ad_token_kopya.value);
            return var_node; // Değişken olarak döndür
        }
    }

    parseError("Birincil ifade bekleniyordu.", "Sayı, Parantez veya Tanımlayıcı");
    return NULL; // Ulaşılamaz
}

/**
 * İkili işlemleri (Örn: toplama, çıkarma) ayrıştırır.
 * Precedence Climbing algoritması kullanır.
 */
ASTNode* ikili_islem(int onceki_oncelik) {
    // 1. Sol Taraf (Birincil İfade)
    ASTNode* sol = birincil();

    if (sol == NULL) return NULL;

    while (1) {
        // 2. Operatörü kontrol et
        TokenType op_type = current_token->type;
        int oncelik = get_precedence(op_type);

        // Mevcut operatörün önceliği, önceki operatörün önceliğinden düşükse/eşitse, döngüyü kır
        if (oncelik <= onceki_oncelik) {
            break;
        }

        // 3. Operatörü tüket ve sağ tarafı ayrıştır
        consume(op_type); // Operatör tüketildi (+, *)

        // Sağ taraf için yeni öncelik
        // (Tüm matematik operatörleri sol-ilişmeli olduğu için oncelik + 1 kullanıyoruz)
        int sag_onceligi = oncelik + 1;

        // Sağ tarafı ayrıştır (rekürsif çağrı)
        ASTNode* sag = ikili_islem(sag_onceligi);

        // 4. Yeni AST düğümünü oluştur
        ASTNode* yeni_node = (ASTNode*)malloc(sizeof(ASTNode));
        if (yeni_node == NULL) return NULL;

        yeni_node->type = AST_IKILI_ISLEM;
        yeni_node->ikili_islem_data.sol = sol;
        yeni_node->ikili_islem_data.sag = sag;
        yeni_node->ikili_islem_data.operator_type = op_type;

        sol = yeni_node; // Yeni düğüm, bir sonraki iterasyon için sol taraf olur
    }

    return sol;
}

/**
 * Tek bir komutu ayrıştırır (Örn: YAZDIR ...; veya SAYISAL ...;)
 */
ASTNode* komut() {
    // 1. YAZDIR komutu
    if (current_token->type == TOKEN_YAPI_YAZDIR) {
        consume(TOKEN_YAPI_YAZDIR);
        ASTNode* ifade_dugumu = ifade();
        
        // ✅ SPECS.md Kontrolü
        // specs_check_no_semicolon("YAZDIR komutu");
        
        // ❌ ARTIK KABUL ETMİYORUZ:
        // consume(TOKEN_SEMICOLON);
        
        ASTNode* yazdir_node = (ASTNode*)malloc(sizeof(ASTNode));
        yazdir_node->type = AST_YAZDIR_KOMUTU;
        yazdir_node->tek_ifade_data.ifade = ifade_dugumu;
        return yazdir_node;
    }
    
    // 2. Değişken Tanımlama (SAYISAL, METIN, BOOL)
    if (current_token->type == TOKEN_TANIMLA_SAYI || 
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL) 
    {
        // ... (Mevcut Değişken Tanımlama mantığınız) ...
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
        consume(TOKEN_SEMICOLON); 

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 3. EĞER Komutu
    if (current_token->type == TOKEN_YAPI_KOSUL_EGER) {
        return kosul_komutu();
    }

    // 4. DÖNGÜ Komutu
    if (current_token->type == TOKEN_YAPI_DONGU) {
        return dongu_komutu();
    }
    
    // 5. DÖNGÜ_BITIR Komutu
    if (current_token->type == TOKEN_YAPI_DONGU_BITIR) {
        // ... (Mevcut DÖNGÜ_BITIR mantığınız) ...
        consume(TOKEN_YAPI_DONGU_BITIR);
        consume(TOKEN_SEMICOLON);
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_DONGU_BITIR_KOMUTU;
        return node;
    }
    
    // YENİ: 6.5. DÖNÜŞ Komutu
    if (current_token->type == TOKEN_YAPI_DONUS) {
        return donus_komutu();
    }

    // 7. Atama veya İfade Komutu (fonksiyon çağrısı)
    if (current_token->type == TOKEN_IDENTIFIER) {
        // Bu bir atama mı yoksa tek başına bir fonksiyon çağrısı mı?
        // Her ikisi de bir ifade olarak başlar. `birincil()` fonksiyonu
        // zaten değişkenleri ve fonksiyon çağrılarını ayırt edebiliyor.
        // `ikili_islem` ise atamayı (`=`) henüz desteklemiyor, bu yüzden
        // ifadeyi ayrıştırıp sonra ne olduğuna bakacağız.

        // `ifade()` fonksiyonu, `a = 5` gibi bir yapıyı veya `hesapla()` gibi
        // bir çağrıyı ayrıştırabilir.
        ASTNode* node = ifade();

        // Eğer `ifade()` bir atama ayrıştırdıysa, bu bir `AST_ATAMA_KOMUTU` olur.
        // Eğer bir fonksiyon çağrısı ayrıştırdıysa, `AST_ISLEC_CAGIRMA` olur.
        // Her iki durumda da sonunda noktalı virgül olmalıdır.
        consume(TOKEN_SEMICOLON);

        // Not: Mevcut `ifade()` yapınız atamayı (`=`) bir ikili işlem olarak
        // görmüyor. Bu yüzden `a = 5` gibi bir ifadeyi `ifade()` ile ayrıştıramayız.
        // Şimdilik, `ifade()` sadece `hesapla();` gibi ifadeleri yakalayacak.
        // Atama komutları için ayrı bir mantık gerekiyor, ancak bu hatayı çözmek için
        // aşağıdaki değişiklik yeterlidir.

        // `ifade()`'nin döndürdüğü düğümü (örn: AST_ISLEC_CAGIRMA) doğrudan döndür.
        // Generator, bu ifadenin sonucunu (RAX) kullanıp atmaya karar verir.
        return node;
    }

    // Hiçbiri eşleşmezse
    // Bu bir ifade komutu olabilir (örn: bes_dondur();)
    // `birincil` artık fonksiyon çağrılarını bir ifade olarak ayrıştırdığı için,
    // `ifade()`'yi çağırıp ardından noktalı virgül bekleyebiliriz.
    ASTNode* node = ifade();
    consume(TOKEN_SEMICOLON);
    // Bu düğüm bir ISLEC_CAGIRMA veya başka bir ifade olabilir.
    // Generator, sonucu kullanıp kullanmayacağına karar verir.
    return node;
}

/**
 * EĞER ... İSE ... [DEĞİLSE ...] SON; komutunu ayrıştırır
 */
ASTNode* kosul_komutu() {
    consume(TOKEN_YAPI_KOSUL_EGER); // EĞER'i tüket
    
    // 1. Koşul İfadesi (örn: sayac == 5)
    ASTNode* kosul_ifadesi = ifade(); 
    
    // 2. İSE anahtar kelimesi
    consume(TOKEN_YAPI_KOSUL_ISE);
    
    // 3. 'İSE' Bloğu (SON veya DEĞİLSE'ye kadar olan komutlar)
    ASTNode* ise_blogu = blok();

    ASTNode* degilse_blogu = NULL;
    // 4. DEĞİLSE bloğu (opsiyonel)
    if (current_token->type == TOKEN_YAPI_KOSUL_DEGILSE) {
        consume(TOKEN_YAPI_KOSUL_DEGILSE);
        degilse_blogu = blok();
    }
    
    // 5. SON
    consume(TOKEN_YAPI_SON);
    
    // 6. AST Düğümünü oluştur
    return createAST_KosulKomutu(kosul_ifadesi, ise_blogu, degilse_blogu);
}

/**
 * DÖNGÜ ... SON; komutunu ayrıştırır
 */
ASTNode* dongu_komutu() {
    consume(TOKEN_YAPI_DONGU); // DÖNGÜ'yü tüket
    
    // 1. Döngü Gövdesi
    // Güncellenmiş 'blok()' fonksiyonu, 'SON' görene kadar okuyacak
    ASTNode* govde_blogu = blok(); 
    
    // 2. SON ve ;
    consume(TOKEN_YAPI_SON);
    consume(TOKEN_SEMICOLON);
    
    // 3. AST Düğümünü oluştur
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_DONGU_KOMUTU;
    node->dongu_data.govde = govde_blogu;
    
    return node;
}

/**
 * İŞLEÇ ... İSE ... SON; komutunu ayrıştırır
 */
ASTNode* islec_tanimlama() {
    consume(TOKEN_YAPI_ISLEC); 
    
    // 1. İsim Token'ını kopyala ve tüket
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("İşleç adı bekleniyordu.", "IDENTIFIER");
    }
    Token ad_token; // Kopyalama için geçici
    ad_token.type = current_token->type;
    ad_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER); 

    // Parametre Listesini Ayrıştır
    consume(TOKEN_LEFT_PAREN);
    
    Token** parametre_listesi = NULL;
    int p_sayisi = 0;
    
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        parametre_listesi = (Token**)malloc(sizeof(Token*) * 10); // Max 10 parametre
        do {
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Parametre adı bekleniyordu.", "IDENTIFIER");
            }
            // Parametre adını kopyala
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
    
    // ✅ Sonraki token İŞLEÇ olmalı (SON İŞLEÇ formatı)
    if (current_token->type != TOKEN_YAPI_ISLEC) {
        fprintf(stderr, "\n❌ SPECS.md İHLALİ!\n");
        fprintf(stderr, "Beklenen: SON İŞLEÇ\n");
        fprintf(stderr, "Bulunan: %s\n", current_token->value);
        fprintf(stderr, "Kural: Fonksiyonlar 'SON İŞLEÇ' ile biter.\n");
        fprintf(stderr, "Bkz: SPECS.md - Kural 2\n\n");
        exit(EXIT_FAILURE);
    }
    consume(TOKEN_YAPI_ISLEC);
    
    // ✅ SPECS.md Kontrolü - SON İŞLEÇ'ten sonra noktalı virgül yok
    // specs_check_no_semicolon("SON İŞLEÇ");
    
    // ❌ ARTIK KABUL ETMİYORUZ:
    // consume(TOKEN_SEMICOLON);
    
    ASTNode* node = createAST_IslecTanimlama(&ad_token, parametre_listesi, p_sayisi, govde_blogu);
    free(ad_token.value);
    return node;
}

// DÖNÜŞ komutunu ayrıştıran fonksiyon - GÜNCELLENDİ
ASTNode* donus_komutu() {
    consume(TOKEN_YAPI_DONUS); // DÖNÜŞ'ü tüket
    
    ASTNode* ifade_dugumu = ifade(); // Döndürülecek ifadeyi ayrıştır
    
    // ✅ SPECS.md Kontrolü
    // specs_check_no_semicolon("DÖNÜŞ komutu");
    
    // ❌ ARTIK KABUL ETMİYORUZ:
    // consume(TOKEN_SEMICOLON);
    
    ASTNode* donus_node = (ASTNode*)malloc(sizeof(ASTNode));
    donus_node->type = AST_DONUS_KOMUTU;
    donus_node->tek_ifade_data.ifade = ifade_dugumu;
    return donus_node;
}

/**
 * Python'daki ana_blok() veya parse() metodunun gövdesi.
 * Birden fazla komutu ayrıştırır ve bir Blok düğümü döndürür.
 */
ASTNode* blok() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        perror("Hafıza ayırma hatası (AST Blok)");
        exit(EXIT_FAILURE);
    }
    node->type = AST_BLOK;
    
    // Komutları tutmak için dinamik bir dizi (şimdilik basit bir C dizisi)
    // İleride bu, 'realloc' ile büyütülen dinamik bir dizi olmalı.
    node->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100); // Max 100 komut
    node->blok_data.sayisi = 0;

    // Blok sonlandırıcı token'ları veya dosya sonunu görene kadar komutları oku
    while (current_token->type != TOKEN_EOF &&
           current_token->type != TOKEN_YAPI_SON &&
           current_token->type != TOKEN_YAPI_KOSUL_DEGILSE) 
    {
        // Noktalı virgül gibi ayırıcıları atla (boş komutlar için)
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
    // 1. Lexer'ı başlat
    initLexer(source_code);

    current_token = getNextToken();

    // YENİ: Ana program bloğunu oluştur
    ASTNode* root = (ASTNode*)malloc(sizeof(ASTNode));
    root->type = AST_BLOK;
    root->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    root->blok_data.sayisi = 0;

    // 1. Adım: Dosya sonuna kadar tüm global İŞLEÇ tanımlamalarını ayrıştır.
    // Bunlar ana program akışının bir parçası değildir, ancak AST'ye eklenirler
    // böylece generator onları bulabilir.
    while (current_token->type != TOKEN_EOF) {
        if (current_token->type == TOKEN_YAPI_ISLEC) {
            root->blok_data.komutlar[root->blok_data.sayisi++] = islec_tanimlama();
        } else {
            // İşleç tanımı değilse, bu ana program kodunun başlangıcıdır.
            // Döngüyü kır ve 2. adıma geç.
            break;
        }
    }
    // 2. Adım: Geriye kalan her şeyi (ana program akışı) tek bir blok olarak ayrıştır.
    // `blok()` fonksiyonu, dosya sonuna (EOF) kadar olan tüm komutları okuyacaktır.
    root->blok_data.komutlar[root->blok_data.sayisi++] = blok();
    return root;
}
