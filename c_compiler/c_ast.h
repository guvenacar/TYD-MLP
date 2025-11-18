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
    AST_ARRAY_TANIMLAMA,    // Array bildirimi: SAYISAL arr[10];
    AST_ARRAY_ERISIM,       // Array erişim: arr[5]
    AST_ARRAY_ATAMA,        // Array atama: arr[5] = 10;
    AST_STRUCT_TANIMLAMA,   // Struct tanımlama: YAPI Nokta İSE ... SON
    AST_STRUCT_FIELD_ACCESS,// Struct field erişim: p.x
    AST_STRUCT_FIELD_ATAMA, // Struct field atama: p.x = 10;
    AST_STRUCT_DEGISKEN,    // Struct değişken: Nokta p;
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

        // Array Tanımlama (SAYISAL arr[10];)
        struct {
            Token* tip;        // SAYISAL, METIN vb.
            Token* ad;         // Array ismi
            ASTNode* boyut;    // Array boyutu (sabit ifade)
        } array_tanimlama_data;

        // Array Erişim (arr[5], arr[i+1])
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
        } array_erisim_data;

        // Array Atama (arr[5] = 10;)
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
            ASTNode* deger;    // Atanacak değer
        } array_atama_data;

        // Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
        struct {
            Token* ad;              // Struct ismi (Nokta)
            Token** field_tipleri;  // Field tipleri (SAYISAL, METIN, vb.)
            Token** field_adlari;   // Field isimleri (x, y, vb.)
            int field_sayisi;       // Field sayısı
        } struct_tanimlama_data;

        // Struct Field Access (p.x)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
        } struct_field_access_data;

        // Struct Field Atama (p.x = 10;)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
            ASTNode* deger;    // Atanacak değer
        } struct_field_atama_data;

        // Struct Değişken (Nokta p;)
        struct {
            Token* struct_tip; // Struct tipi (Nokta)
            Token* ad;         // Değişken ismi (p)
        } struct_degisken_data;
    };
};

// Fonksiyon Prototipleri
ASTNode* parse(const char* source_code); // Ana ayrıştırma fonksiyonu
void freeAST(ASTNode* node);             // AST temizleme (Bellek yönetimi)

#endif // C_AST_H