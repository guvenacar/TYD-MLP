// c_compiler/c_ast.h

#ifndef C_AST_H
#define C_AST_H

#include "c_lexer.h" // Token yapısını kullanmak için

// --- 1. Düğüm Tiplerini Tanımla (Enum) ---
typedef enum {
    AST_BLOK,
    AST_SAYI,
    AST_METIN,
    AST_DEGISKEN,
    AST_IKILI_ISLEM,
    AST_DEGISKEN_TANIMLAMA,
    AST_ATAMA_KOMUTU,
    AST_YAZDIR_KOMUTU,
    AST_KOSUL_KOMUTU, // <-- YENİ (EĞER)
    AST_DONGU_KOMUTU, // <-- YENİ (DÖNGÜ)
    AST_DONGU_BITIR_KOMUTU, // <-- YENİ (DÖNGÜ_BITIR)
    AST_ISLEC_TANIMLAMA,
    AST_DONUS_KOMUTU,
    AST_ISLEC_CAGIRMA,
    // ... (Diğer tüm AST düğüm tipleri buraya eklenecek)
} ASTNodeType;

// İleri Bildirimler (C'de iç içe struct'lar için gerekli)
typedef struct ASTNode ASTNode;
typedef struct Blok ASTBlok;

// --- 2. Temel AST Düğümü Yapısı ---
// Tüm düğümlerin ortak özelliklerini tutar
struct ASTNode {
    ASTNodeType type;
    
    // YENİ: Düğümlere Özgü Veri
    // Bu, C'deki "Union" yapısının kullanımını gerektirir (ileri aşamada)
    // Şimdilik sadece ana struct'ları tanımlayalım:
    union {
        // Blok yapısı (Blok, Döngü, İşleç Gövdesi)
        struct {
            ASTNode** komutlar; // Komut dizisi (Heap'ten ayrılacak)
            int sayisi;         // Komut sayısı
        } blok_data;
        
        // Sayi/Metin yapısı
        struct {
            char* deger; // Token'dan gelen değer
        } sabit_data;

        // İkili İşlem yapısı (a + b)
        struct {
            ASTNode* sol;
            ASTNode* sag;
            TokenType operator_type;
        } ikili_islem_data;
        
        // YENİ: Değişken/Tanımlayıcı
        struct {
            Token* ad;
        } degisken_data;

        // Değişken Tanımlama (SAYISAL a = 5;)
        struct {
            Token* tip;
            Token* ad;
            ASTNode* ifade; // Atanan değer
        } tanimlama_data;
        
        // YENİ: Atama Komutu (sayac = 10;)
        struct {
            Token* ad;
            ASTNode* ifade; // Atanan değer
        } atama_data;
        
        // YENİ: Koşul Komutu (EĞER ifade İSE blok [DEĞİLSE blok])
        struct {
            ASTNode* kosul;       // (sayac == 5)
            ASTNode* ise_blok;    // EĞER doğruysa çalışacak blok
            ASTNode* degilse_blok; // (Opsiyonel) EĞER yanlışsa çalışacak blok
        } kosul_data;
        
        // YENİ: Döngü Komutu (DÖNGÜ blok SON)
        struct {
            ASTNode* govde; // Döngü gövdesi
        } dongu_data;

        // İşleç Tanımlama (Fonksiyon)
        struct {
            Token* ad;
            Token** parametreler; // YENİ: Parametre adları (Token listesi)
            int parametre_sayisi; // YENİ
            ASTNode* govde;
        } islec_tanimlama_data;
        
        // İşleç Çağırma (topla(5, 7))
        struct {
            Token* hedef_ad;
            ASTNode** argumanlar;
            int arguman_sayisi;
        } islec_cagirma_data;
        
        // Diğer komutlar (YAZDIR, DÖNÜŞ, ATAMA)
        struct {
            ASTNode* ifade;
        } tek_ifade_data;
    };
};

// Fonksiyon Prototipleri
ASTNode* parse(const char* source_code); // Ana ayrıştırma fonksiyonu
void freeAST(ASTNode* node);             // AST temizleme (Bellek yönetimi)

#endif // C_AST_H