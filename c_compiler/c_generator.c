// c_compiler/c_generator.c

#include "c_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // bool tipi için
#include <ctype.h>   // tolower için

// Forward declaration from main.c
extern const char* get_base_dir();

// --- Generator Durum Yönetimi ---
static AsmCode data_section; // .data bölümü (örn: "Merhaba")
static AsmCode text_section; // .text bölümü (ana kod)

// YENİ: Kapsam Yönetimi (Python'daki Kapsam sınıfına karşılık gelir)

#define MAX_DEGISKENLER 100
#define MAX_SCOPE_DEPTH 20

typedef struct {
    char* ad;
    char* asm_adresi; // Yığındaki adresi (örn: "[rbp-8]") veya global için label
    char* tip;        // Değişkenin tipi ("SAYISAL" veya "METIN")
    int scope_level;  // Hangi scope seviyesinde tanımlandı (0=global)
    bool is_global;   // Global değişken mi?
} Degisken;

// Çok seviyeli Kapsam (Scope) - Stack bazlı
Degisken kapsam_haritasi[MAX_DEGISKENLER];
int kapsam_degisken_sayisi = 0;
int kapsam_yigin_ofseti = 0; // RBP'den ne kadar aşağı inildiği (örn: -8, -16)

// Scope seviyesi tracking
int current_scope_level = 0;
int scope_stack_offsets[MAX_SCOPE_DEPTH]; // Her scope seviyesinde stack offset

// YENİ: Etiket (Label) Sayacı
static int etiket_sayaci = 0;

// YENİ: String literal sayacı (benzersiz etiketler için)
static int metin_sayaci = 0;

// YENİ: Aktif Döngü Bitiş Etiketi
// (İç içe döngüler için bu bir yığın (stack) olmalıdır, şimdilik basit tutalım)
static int aktif_dongu_son_etiketi = -1; 

// Argüman register'ları (Linux x86-64 ABI)
const char* arg_registerleri[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// Kapsam Yönetim Fonksiyonları

// Kapsam haritasında strdup ile ayrılmış belleği temizler
void kapsam_temizle() {
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        free(kapsam_haritasi[i].ad);
        free(kapsam_haritasi[i].asm_adresi);
        free(kapsam_haritasi[i].tip);
    }
    kapsam_degisken_sayisi = 0;
    current_scope_level = 0;
}

// Yeni bir scope'a gir (EĞER, DÖNGÜ, İŞLEÇ blokları için)
void kapsam_gir() {
    if (current_scope_level >= MAX_SCOPE_DEPTH - 1) {
        fprintf(stderr, "HATA [Generator]: Maksimum scope derinliği aşıldı!\n");
        exit(1);
    }

    // Mevcut stack offset'i kaydet
    scope_stack_offsets[current_scope_level] = kapsam_yigin_ofseti;
    current_scope_level++;
}

// Scope'tan çık - bu seviyedeki değişkenleri temizle
void kapsam_cik() {
    if (current_scope_level <= 0) {
        return; // Zaten en üst seviyedeyiz
    }

    current_scope_level--;

    // Bu seviyedeki değişkenleri temizle
    int new_var_count = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level <= current_scope_level) {
            // Bu değişkeni tut
            if (i != new_var_count) {
                kapsam_haritasi[new_var_count] = kapsam_haritasi[i];
            }
            new_var_count++;
        } else {
            // Bu değişkeni temizle
            free(kapsam_haritasi[i].ad);
            free(kapsam_haritasi[i].asm_adresi);
            free(kapsam_haritasi[i].tip);
        }
    }

    kapsam_degisken_sayisi = new_var_count;

    // Stack offset'i geri yükle
    kapsam_yigin_ofseti = scope_stack_offsets[current_scope_level];
}

// Değişkeni kaydeder ve yığındaki adresini döndürür
char* kapsam_degisken_yer_ayir(const char* ad, const char* tip) {
    kapsam_yigin_ofseti += 8; // Yığında 8 byte (64-bit) yer aç
    char* adres = (char*)malloc(32);
    sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);

    // Değişkeni haritaya kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(ad);
    d->asm_adresi = strdup(adres);
    d->tip = strdup(tip);
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);

    return adres;
}

// Bir değişkenin yığındaki adresini bulur
char* kapsam_degisken_adresi_bul(const char* ad) {
    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return kapsam_haritasi[i].asm_adresi;
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s\n", ad);
    exit(1);
}

// Bir değişkenin tipini bulur
char* kapsam_degisken_tipi_bul(const char* ad) {
    if (ad == NULL) {
        fprintf(stderr, "HATA [Generator]: kapsam_degisken_tipi_bul'a NULL ad geldi\n");
        exit(1);
    }

    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (kapsam_haritasi[i].ad != NULL && strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return kapsam_haritasi[i].tip;
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s (Kapsam: %d değişken)\n", ad, kapsam_degisken_sayisi);
    exit(1);
}

// --- Yardımcı Fonksiyonlar ---

// TYD fonksiyon adlarını C runtime fonksiyon adlarına map eder
const char* convert_function_name(const char* tyd_adi) {
    // Dosya fonksiyonları
    if (strcmp(tyd_adi, "DOSYA_AC") == 0) return "tyd_dosya_ac";
    if (strcmp(tyd_adi, "DOSYA_OKU") == 0) return "tyd_dosya_oku";
    if (strcmp(tyd_adi, "DOSYA_YAZ") == 0) return "tyd_dosya_yaz";
    if (strcmp(tyd_adi, "DOSYA_KAPAT") == 0) return "tyd_dosya_kapat";
    
    // String fonksiyonları
    if (strcmp(tyd_adi, "STRLEN") == 0) return "strlen";
    if (strcmp(tyd_adi, "SUBSTR") == 0) return "tyd_substr";
    if (strcmp(tyd_adi, "STRCAT") == 0) return "tyd_strcat";
    if (strcmp(tyd_adi, "STRSTR") == 0) return "strstr";
    if (strcmp(tyd_adi, "STRCMP") == 0) return "strcmp";
    
    return tyd_adi;
}

// AsmCode yapısına güvenli bir şekilde kod ekler
void asm_append(AsmCode* section, const char* code) {
    if (code == NULL) {
        fprintf(stderr, "HATA: asm_append'e NULL kod geldi!\n");
        return;
    }

    size_t code_len = strlen(code);
    size_t new_line_len = 1; // for '\n'
    size_t needed = section->size + code_len + new_line_len + 1; // +1 for null

    // Gerekirse kapasiteyi artır (GCC-style: 4KB başlangıç, 2x büyütme)
    if (needed > section->capacity) {
        // Yeni kapasite: başlangıçta 4KB, sonra 2x veya tam ihtiyaç
        size_t new_capacity = (section->capacity == 0) ? 4096 : section->capacity * 2;

        // Eğer 2x bile yetmezse, tam ihtiyacı karşıla + %25 buffer
        if (new_capacity < needed) {
            new_capacity = needed + (needed / 4);
        }

        char* new_code = (char*)realloc(section->code, new_capacity);
        if (new_code == NULL) {
            fprintf(stderr, "HATA: Bellek ayırma başarısız (istenen: %zu bytes)\n", new_capacity);
            exit(EXIT_FAILURE);
        }

        section->code = new_code;
        section->capacity = new_capacity;

        // İlk ayırmada string'i sıfırla
        if (section->size == 0) {
            section->code[0] = '\0';
        }
    }

    // GÜVENLİ string ekleme (strcpy yerine memcpy + manuel null)
    memcpy(section->code + section->size, code, code_len);
    section->code[section->size + code_len] = '\n';
    section->code[section->size + code_len + 1] = '\0';
    section->size += (code_len + new_line_len);
}

// --- Ziyaretçi (Visitor) Fonksiyonları ---

void visit(ASTNode* node); // Ana ziyaretçi fonksiyonu (prototip)
void visit_IkiliIslem(ASTNode* node); // İleri bildirim
void visit_DegiskenTanimlama(ASTNode* node); // İleri bildirim
void visit_Degisken(ASTNode* node); // İleri bildirim
void visit_AtamaKomutu(ASTNode* node); // İleri bildirim
void visit_KosulKomutu(ASTNode* node); // İleri bildirim
void visit_DonguKomutu(ASTNode* node); // İleri bildirim
void visit_DonguBitirKomutu(ASTNode* node); // İleri bildirim
void visit_IslecTanimlama(ASTNode* node); // İleri bildirim
void visit_IslecCagirma(ASTNode* node); // İleri bildirim
void visit_DonusKomutu(ASTNode* node); // İleri bildirim
void visit_ArrayTanimlama(ASTNode* node); // İleri bildirim
void visit_ArrayErisim(ASTNode* node); // İleri bildirim

void visit_Blok(ASTNode* node) {
    // Blok içindeki her komutu ziyaret et
    for (int i = 0; i < node->blok_data.sayisi; i++) {
        visit(node->blok_data.komutlar[i]);
    }
}

void visit_Yazdir(ASTNode* node) {

    // 1. İfadeyi ziyaret et (sonuç RAX'e yüklenecek)
    visit(node->tek_ifade_data.ifade);

    // 2. C 'printf' fonksiyonunu çağır
    asm_append(&text_section, "    ; --- YAZDIR Başlangıç ---");

    // İfadenin tipine göre format string seç
    bool is_string = false;

    if (node->tek_ifade_data.ifade->type == AST_METIN) {
        is_string = true;
    } else if (node->tek_ifade_data.ifade->type == AST_DEGISKEN) {
        // Değişken ise, tipini kontrol et
        if (node->tek_ifade_data.ifade->degisken_data.ad != NULL &&
            node->tek_ifade_data.ifade->degisken_data.ad->value != NULL) {
            char* degisken_adi = node->tek_ifade_data.ifade->degisken_data.ad->value;
            char* degisken_tipi = kapsam_degisken_tipi_bul(degisken_adi);
            if (degisken_tipi != NULL && strcmp(degisken_tipi, "METIN") == 0) {
                is_string = true;
            }
        } else {
        }
    }


    if (is_string) {
        // String yazdır (RAX'te string adresi var)
        asm_append(&text_section, "    mov rdi, format_metin"); // format_metin: "%s\n"
        asm_append(&text_section, "    mov rsi, rax");      // String adresi
    } else {
        // Sayı yazdır (RAX'te sayı var)
        asm_append(&text_section, "    mov rdi, format_sayi"); // format_sayi: "%ld\n"
        asm_append(&text_section, "    mov rsi, rax");      // Yazdırılacak değer
    }

    asm_append(&text_section, "    mov rax, 0");      // SSE kuralı
    asm_append(&text_section, "    call printf");
}

void visit_Sayi(ASTNode* node) {
    // Bir sayıyı RAX register'ına yükle
    // Geçici düzeltme (sprintf kullanarak):
    char buffer[64];
    sprintf(buffer, "    mov rax, %s", node->sabit_data.deger);
    asm_append(&text_section, buffer);
}

void visit_Metin(ASTNode* node) {
    // String literal'i .data bölümüne ekle ve adresini RAX'e yükle
    char etiket[64];
    char buffer[512];

    // Benzersiz etiket oluştur
    sprintf(etiket, "str_%d", metin_sayaci++);

    // .data bölümüne string ekle (null-terminated)
    sprintf(buffer, "%s: db \"%s\", 0", etiket, node->sabit_data.deger);
    asm_append(&data_section, buffer);

    // String'in adresini RAX'e yükle
    sprintf(buffer, "    mov rax, %s", etiket);
    asm_append(&text_section, buffer);
}

void visit_DegiskenTanimlama(ASTNode* node) {
    char* degisken_adi = node->tanimlama_data.ad->value;
    char* degisken_tipi = node->tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- DegiskenTanimlama: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // Global değişken mi? (scope level 0)
    if (current_scope_level == 0) {
        // Global değişken: .data section'a ekle
        sprintf(buffer, "global_%s: dq 0", degisken_adi);
        asm_append(&data_section, buffer);

        // Değişken için yığında değil, .data'da yer ayır
        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "[global_%s]", degisken_adi);

        // Scope'a ekle
        Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
        d->ad = strdup(degisken_adi);
        d->asm_adresi = global_adres;
        d->tip = strdup(degisken_tipi);
        d->scope_level = 0;
        d->is_global = true;

        // İfadeyi hesapla (sonuç RAX'te)
        visit(node->tanimlama_data.ifade);

        // Global değişkene ata
        sprintf(buffer, "    mov %s, rax", global_adres);
        asm_append(&text_section, buffer);
    } else {
        // Local değişken: Stack'te
        // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
        visit(node->tanimlama_data.ifade);

        // 2. Değişken için yığında (stack) yer ayır
        char* adres = kapsam_degisken_yer_ayir(degisken_adi, degisken_tipi);

        // 3. İfadenin sonucunu (RAX) yığındaki yeni adrese taşı
        sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
        asm_append(&text_section, buffer);
        free(adres); // kapsam_degisken_yer_ayir'dan gelen belleği temizle
    }
}

void visit_Degisken(ASTNode* node) {
    char* degisken_adi = node->degisken_data.ad->value;
    char* adres = kapsam_degisken_adresi_bul(degisken_adi);
    
    // Değerin yığındaki adresinden (örn: [rbp-8]) RAX'e yükle
    char buffer[128];
    sprintf(buffer, "    mov rax, %s", adres); // Örn: mov rax, [rbp-8]
    asm_append(&text_section, buffer);
}

void visit_AtamaKomutu(ASTNode* node) {
    char* degisken_adi = node->atama_data.ad->value;
    char buffer[128];
    sprintf(buffer, "    ; --- AtamaKomutu: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
    visit(node->atama_data.ifade);

    // 2. Değişkenin adresini bul
    char* adres = kapsam_degisken_adresi_bul(degisken_adi);
    
    // 3. İfadenin sonucunu (RAX) değişkenin adresine taşı
    sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
    asm_append(&text_section, buffer);
}

void visit_KosulKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_degilse = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. Koşul İfadesini (a == 10) hesapla
    // (visit_IkiliIslem sayesinde sonuç RAX'e 1 veya 0 olarak yüklenecek)
    asm_append(&text_section, "    ; --- Kosul Komutu (EĞER) ---");
    visit(node->kosul_data.kosul);
    
    // 2. Koşulu kontrol et ve atla
    asm_append(&text_section, "    cmp rax, 0"); // Sonuç 0 mı (false) ?
    
    // Eğer 'DEĞİLSE' bloğu varsa oraya, yoksa 'SON' etiketine atla
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, "    je .L%d", etiket_degilse); // False ise DEĞİLSE'ye atla
    } else {
        sprintf(buffer, "    je .L%d", etiket_son); // False ise SON'a atla
    }
    asm_append(&text_section, buffer);

    // 3. 'İSE' Bloğunu (Doğruysa) çalıştır
    visit(node->kosul_data.ise_blok);
    
    // 'İSE' bloğu bittikten sonra 'SON' etiketine atla (DEĞİLSE'yi atlamak için)
    sprintf(buffer, "    jmp .L%d", etiket_son);
    asm_append(&text_section, buffer);

    // 4. 'DEĞİLSE' Bloğu (Varsa)
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, ".L%d:", etiket_degilse); // DEĞİLSE etiketi
        asm_append(&text_section, buffer);
        visit(node->kosul_data.degilse_blok);
    }
    
    // 5. 'SON' Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // SON etiketi
    asm_append(&text_section, buffer);
}

void visit_DonguKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_basla = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. DÖNGÜ_BITIR için bu döngünün bitiş etiketini kaydet
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    aktif_dongu_son_etiketi = etiket_son;

    // 2. Döngü Başlangıç Etiketi
    sprintf(buffer, ".L%d:", etiket_basla); // .L_BASLA
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- Dongu Komutu (DÖNGÜ) ---");

    // 3. Döngü Gövdesi
    visit(node->dongu_data.govde);
    
    // 4. Gövde bittikten sonra başa atla
    sprintf(buffer, "    jmp .L%d", etiket_basla); // jmp .L_BASLA
    asm_append(&text_section, buffer);

    // 5. Döngü Bitiş Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // .L_SON
    asm_append(&text_section, buffer);

    // 6. Döngü bitti, eski etiketi geri yükle (iç içe döngüler için)
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
}

void visit_DonguBitirKomutu(ASTNode* node) {
    if (aktif_dongu_son_etiketi == -1) {
        fprintf(stderr, "HATA [Generator]: 'DÖNGÜ_BITIR' bir döngü içinde değil.\n");
        exit(1);
    }
    
    char buffer[128];
    sprintf(buffer, "    jmp .L%d", aktif_dongu_son_etiketi); // jmp .L_SON
    asm_append(&text_section, "    ; --- DonguBitir Komutu ---");
    asm_append(&text_section, buffer);
}

void visit_IslecTanimlama(ASTNode* node) {
    char* islec_adi = node->islec_tanimlama_data.ad->value;
    char buffer[128];

    // Fonksiyon için yeni kapsam aç
    int onceki_degisken_sayisi = kapsam_degisken_sayisi;
    int onceki_yigin_ofseti = kapsam_yigin_ofseti;

    // Global değişkenleri koru, sadece stack offset'i sıfırla
    // Global değişkenleri say (scope_level == 0)
    int global_sayisi = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            global_sayisi++;
        }
    }

    // Global değişkenleri başa taşı ve sayıyı ayarla
    int new_idx = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            if (i != new_idx) {
                kapsam_haritasi[new_idx] = kapsam_haritasi[i];
            }
            new_idx++;
        }
    }
    kapsam_degisken_sayisi = global_sayisi;
    kapsam_yigin_ofseti = 0;  // Fonksiyon stack'i sıfırdan başlar

    // 1. Fonksiyon Etiketini Tanımla
    sprintf(buffer, "%s:", islec_adi);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    ; --- Islec Tanimlama: %s ---", islec_adi);
    asm_append(&text_section, buffer);

    // 2. Fonksiyon Girişi (Prolog)
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)

    // 3. Parametreleri kaydet
    int param_sayisi = node->islec_tanimlama_data.parametre_sayisi;
    for (int i = 0; i < param_sayisi; i++) {
        char* param_adi = node->islec_tanimlama_data.parametreler[i]->value;
        char* adres = kapsam_degisken_yer_ayir(param_adi, "SAYISAL");
        sprintf(buffer, "    mov %s, %s", adres, arg_registerleri[i]);
        asm_append(&text_section, buffer);
        free(adres);
    }

    // 4. Fonksiyon Gövdesi
    visit(node->islec_tanimlama_data.govde);

    // 5. Önceki kapsamı geri yükle
    kapsam_degisken_sayisi = onceki_degisken_sayisi;
    kapsam_yigin_ofseti = onceki_yigin_ofseti;
}

void visit_IslecCagirma(ASTNode* node) {
    char* islec_adi = node->islec_cagirma_data.hedef_ad->value;
    int arg_sayisi = node->islec_cagirma_data.arguman_sayisi;
    char buffer[128];

    // ===== STRING FONKSİYONLARI KONTROLÜ =====
    if (strcmp(islec_adi, "STRLEN") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]); // String argümanı RAX'e
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call strlen");
        return; // ✅ Sonuç zaten RAX'te
    }
    else if (strcmp(islec_adi, "STRCAT") == 0 && arg_sayisi == 2) {
        // Argüman 1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // Arg1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // Arg2 -> rsi
        asm_append(&text_section, "    call tyd_strcat");
        return; // ✅ Sonuç RAX'te
    }
    else if (strcmp(islec_adi, "SUBSTR") == 0 && arg_sayisi == 3) {
        // Argüman 1 (str)
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2 (pos)
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 3 (len)
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // pos
        asm_append(&text_section, "    pop rdi");  // str
        asm_append(&text_section, "    mov rdx, rax"); // len
        asm_append(&text_section, "    call tyd_substr");
        return;
    }
    else if (strcmp(islec_adi, "STRCMP") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strcmp");
        return;
    }
    else if (strcmp(islec_adi, "STRSTR") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strstr");
        return;
    }

    // ===== DOSYA FONKSİYONLARI =====
    else if (strcmp(islec_adi, "DOSYA_AC") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_ac");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_OKU") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_oku");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_YAZ") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_yaz");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_KAPAT") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_kapat");
        return;
    }
    // YENİ: DIZIN_AL fonksiyonu (self-hosting için)
    else if (strcmp(islec_adi, "DIZIN_AL") == 0 && arg_sayisi == 0) {
        asm_append(&text_section, "    ; --- DIZIN_AL cagirma ---");
        asm_append(&text_section, "    mov rax, 0"); // SSE kuralı
        asm_append(&text_section, "    call runtime_dizin_al");
        // Sonuç (dizin yolu char*) zaten RAX'te olacak
        return;
    }

    // ===== BOOTSTRAP STRING FONKSİYONLARI =====
    // (Bu fonksiyonlar sadece TYD compiler'ını TYD'de yazmak için gerekli)

    else if (strcmp(islec_adi, "STRING_KARAKTER_AL") == 0 && arg_sayisi == 2) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: indeks
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // indeks -> rsi
        asm_append(&text_section, "    call string_karakter_al");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_ALT") == 0 && arg_sayisi == 3) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: baslangic
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        // Argüman 3: uzunluk
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // baslangic -> rsi
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rdx, rax"); // uzunluk -> rdx
        asm_append(&text_section, "    call string_alt");
        return; // Sonuç RAX'te (substring)
    }

    else if (strcmp(islec_adi, "KARAKTER_KODU") == 0 && arg_sayisi == 1) {
        // Argüman: tek karakterlik string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call karakter_kodu");
        return; // Sonuç RAX'te (ASCII kodu)
    }

    else if (strcmp(islec_adi, "KODU_KARAKTERE") == 0 && arg_sayisi == 1) {
        // Argüman: ASCII kodu
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call kodu_karaktere");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_BIRLESTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_birlestir");
        return; // Sonuç RAX'te (birleştirilmiş string)
    }

    else if (strcmp(islec_adi, "STRING_KARSILASTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_karsilastir");
        return; // Sonuç RAX'te (0 = eşit, !=0 = farklı)
    }

    else if (strcmp(islec_adi, "STRING_UZUNLUK") == 0 && arg_sayisi == 1) {
        // Argüman: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call string_uzunluk");
        return; // Sonuç RAX'te (uzunluk)
    }

    else if (strcmp(islec_adi, "STRING_ESIT_MI") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_esit_mi");
        return; // Sonuç RAX'te (1 = eşit, 0 = farklı)
    }

    // ===== KULLANICI TANIMLI FONKSİYONLAR =====
    // 1. Argümanları hesapla ve yığına (stack) it
    for (int i = 0; i < arg_sayisi; i++) {
        visit(node->islec_cagirma_data.argumanlar[i]);
        asm_append(&text_section, "    push rax");
    }

    // 2. Argümanları yığından register'lara çek (ters sırada)
    for (int i = arg_sayisi - 1; i >= 0; i--) {
        sprintf(buffer, "    pop %s", arg_registerleri[i]);
        asm_append(&text_section, buffer);
    }

    // 3. Fonksiyonu çağır
    sprintf(buffer, "    call %s", islec_adi);
    asm_append(&text_section, buffer);
}

void visit_ArrayTanimlama(ASTNode* node) {
    char* array_adi = node->array_tanimlama_data.ad->value;
    char* array_tipi = node->array_tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayTanimlama: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // Boyut ifadesini hesapla (sonuç RAX'te olacak)
    visit(node->array_tanimlama_data.boyut);

    // Boyut sabit olmalı (şimdilik sadece sabit boyutlu arrayler)
    // RAX'te boyut var
    // Array için yığında yer ayır
    // Şimdilik basit: Her eleman 8 byte (SAYISAL)
    // Toplam boyut = eleman_sayisi * 8

    // Array bilgisini kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(array_adi);
    d->tip = (char*)malloc(64);
    sprintf(d->tip, "ARRAY_%s", array_tipi);  // Örn: "ARRAY_SAYISAL"
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);

    // Boyut bilgisini sakla (şimdilik compile-time sabit olmalı)
    // HACK: Boyut değerini AST'den almayı deneyelim
    int array_boyut = 0;
    if (node->array_tanimlama_data.boyut->type == AST_SAYI) {
        array_boyut = atoi(node->array_tanimlama_data.boyut->sabit_data.deger);
    } else {
        fprintf(stderr, "HATA [Generator]: Array boyutu compile-time sabit olmalı\n");
        exit(1);
    }

    int total_bytes = array_boyut * 8;  // Her eleman 8 byte

    if (current_scope_level == 0) {
        // Global array: .bss section'da
        sprintf(buffer, "global_array_%s: resq %d  ; %d elements", array_adi, array_boyut, array_boyut);
        asm_append(&data_section, buffer);

        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "global_array_%s", array_adi);
        d->asm_adresi = global_adres;
    } else {
        // Local array: Stack'te
        kapsam_yigin_ofseti += total_bytes;
        char* adres = (char*)malloc(32);
        sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);
        d->asm_adresi = strdup(adres);
        free(adres);

        sprintf(buffer, "    sub rsp, %d  ; Array allocation for %s[%d]",
                total_bytes, array_adi, array_boyut);
        asm_append(&text_section, buffer);
    }
}

void visit_ArrayErisim(ASTNode* node) {
    char* array_adi = node->array_erisim_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayErisim: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // İndeks ifadesini hesapla (sonuç RAX'te)
    visit(node->array_erisim_data.indeks);

    // İndeksi offset'e çevir (indeks * 8)
    asm_append(&text_section, "    imul rax, 8  ; indeks * 8 (element size)");
    asm_append(&text_section, "    push rax  ; offset'i sakla");

    // Array base adresini bul
    char* array_base = kapsam_degisken_adresi_bul(array_adi);

    // Base + offset hesapla
    sprintf(buffer, "    lea rbx, %s  ; Array base adresi", array_base);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    pop rax  ; offset'i geri al");
    asm_append(&text_section, "    add rbx, rax  ; base + offset");

    // Değeri oku
    asm_append(&text_section, "    mov rax, [rbx]  ; Array elemanını oku");
}

void visit_DonusKomutu(ASTNode* node) {
    asm_append(&text_section, "    ; --- Donus Komutu ---");

    // 1. Döndürülecek ifadeyi hesapla (Sonuç RAX'e yüklenir)
    visit(node->tek_ifade_data.ifade);

    // 2. Fonksiyon Çıkışını (Epilog) üret
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");
}

/**
 * İkili İşlem (a + b) düğümünü ziyaret eder.
 * Sonuç her zaman RAX register'ında kalır.
 */
void visit_IkiliIslem(ASTNode* node) {
    // 1. Sağ tarafı (ifade 2) hesapla ve yığına (stack) it
    visit(node->ikili_islem_data.sag);
    asm_append(&text_section, "    push rax"); // Sağ tarafın sonucunu yığına kaydet

    // 2. Sol tarafı (ifade 1) hesapla (sonuç RAX'te kalır)
    visit(node->ikili_islem_data.sol);

    // 3. Sağ tarafı yığından çek (pop) ve işlemi yap
    asm_append(&text_section, "    pop rbx"); // Sağ tarafın sonucunu RBX'e al

    // Operatör tipine göre Assembly komutunu seç
    switch (node->ikili_islem_data.operator_type) {
        case TOKEN_PLUS:
            asm_append(&text_section, "    add rax, rbx"); // (Sol) + (Sağ)
            break;
        case TOKEN_MINUS:
            asm_append(&text_section, "    sub rax, rbx"); // (Sol) - (Sağ)
            break;
        case TOKEN_MUL:
            asm_append(&text_section, "    imul rax, rbx"); // (Sol) * (Sağ)
            break;
        case TOKEN_DIV:
            asm_append(&text_section, "    cqo"); // RAX'i RDX:RAX'e genişlet
            asm_append(&text_section, "    idiv rbx"); // (Sol) / (Sağ) -> Sonuç RAX'te
            break;
        
        // YENİ: KARŞILAŞTIRMA OPERATÖRLERİ
        case TOKEN_OP_ESIT_KARSILASTIRMA: // ==
            asm_append(&text_section, "    cmp rax, rbx"); // RAX ve RBX'i karşılaştır
            asm_append(&text_section, "    sete al");      // Eğer eşitse (Equal) AL = 1 yap
            asm_append(&text_section, "    movzx rax, al"); // RAX'i 1 veya 0 yap
            break;
        case TOKEN_NOT_ESIT: // !=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setne al");     // Eğer eşit değilse (Not Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LT: // <
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setl al");      // Eğer küçükse (Less) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GT: // >
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setg al");      // Eğer büyükse (Greater) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GTE: // >=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setge al");     // Eğer büyük veya eşitse (Greater or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LTE: // <=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setle al");     // Eğer küçük veya eşitse (Less or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Desteklenmeyen ikili operatör tipi: %d\n",
                node->ikili_islem_data.operator_type);
            exit(1);
    }
}

// Ana ziyaretçi fonksiyonu (Tüm düğüm tiplerini yönlendirir)
void visit(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case AST_BLOK:
            visit_Blok(node);
            break;
        case AST_SAYI:
            visit_Sayi(node);
            break;
        case AST_METIN:
            visit_Metin(node);
            break;
        case AST_YAZDIR_KOMUTU:
            visit_Yazdir(node);
            break;
        
        // YENİ: İkili İşlem
        case AST_IKILI_ISLEM:
            visit_IkiliIslem(node);
            break;
        
        // YENİ: Değişken Tanımlama
        case AST_DEGISKEN_TANIMLAMA:
            visit_DegiskenTanimlama(node);
            break;
            
        // YENİ: Değişken Okuma
        case AST_DEGISKEN:
            visit_Degisken(node);
            break;
            
        // YENİ: Değişken Güncelleme
        case AST_ATAMA_KOMUTU:
            visit_AtamaKomutu(node);
            break;
            
        // YENİ: Koşul (EĞER) Komutu
        case AST_KOSUL_KOMUTU:
            visit_KosulKomutu(node);
            break;
            
        // YENİ: Döngü Komutu
        case AST_DONGU_KOMUTU:
            visit_DonguKomutu(node);
            break;
            
        // YENİ: Döngü Bitir Komutu
        case AST_DONGU_BITIR_KOMUTU:
            visit_DonguBitirKomutu(node);
            break;

        // YENİ: İşleç Tanımlama
        case AST_ISLEC_TANIMLAMA:
            visit_IslecTanimlama(node);
            break;

        // YENİ: İşleç Çağırma
        case AST_ISLEC_CAGIRMA:
            visit_IslecCagirma(node);
            break;
            
        // YENİ: Dönüş Komutu
        case AST_DONUS_KOMUTU:
            visit_DonusKomutu(node);
            break;

        // Array Tanımlama
        case AST_ARRAY_TANIMLAMA:
            visit_ArrayTanimlama(node);
            break;

        // Array Erişim
        case AST_ARRAY_ERISIM:
            visit_ArrayErisim(node);
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Tanınmayan AST düğüm tipi: %d\n", node->type);
            exit(1);
    }
}

// --- Ana Fonksiyon ---

// AsmCode yapısı için ayrılan belleği serbest bırakır
void free_asm_code(AsmCode* section) {
    free(section->code);
}

char* generate_asm(ASTNode* root) {
    // Kapsamı sıfırla
    kapsam_degisken_sayisi = 0;
    kapsam_yigin_ofseti = 0;
    etiket_sayaci = 0;
    metin_sayaci = 0;
    aktif_dongu_son_etiketi = -1;

    // Initialize sections
    data_section.code = NULL;
    data_section.size = 0;
    data_section.capacity = 0;

    text_section.code = NULL;
    text_section.size = 0;
    text_section.capacity = 0;

    // 1. .data bölümü
    asm_append(&data_section, "extern printf");
    asm_append(&data_section, "extern strlen");      // ✅ Ekle
    asm_append(&data_section, "extern strcmp");      // ✅ Ekle
    asm_append(&data_section, "extern strstr");      // ✅ Ekle
    asm_append(&data_section, "extern tyd_substr");  // ✅ Ekle
    asm_append(&data_section, "extern tyd_strcat");  // ✅ Ekle
    asm_append(&data_section, "extern dosya_ac");
    asm_append(&data_section, "extern dosya_oku");
    asm_append(&data_section, "extern dosya_yaz");
    asm_append(&data_section, "extern dosya_kapat");
    asm_append(&data_section, "extern runtime_dizin_al"); // Self-host için eklendi
    asm_append(&data_section, "extern tyd_fix_cwd"); // ✅ yeni

    // String fonksiyonları (runtime.c'deki wrapperlar)
    asm_append(&data_section, "extern string_birlestir");
    asm_append(&data_section, "extern string_karsilastir");
    asm_append(&data_section, "extern string_uzunluk");
    asm_append(&data_section, "extern string_esit_mi");
    asm_append(&data_section, "extern string_karakter_al");
    asm_append(&data_section, "extern string_alt");

    asm_append(&data_section, "section .data");
    asm_append(&data_section, "    format_sayi db \"%ld\", 10, 0"); // %d -> %ld
    asm_append(&data_section, "    format_metin db \"%s\", 10, 0");

    // 2. .text bölümü başlangıcı
    asm_append(&text_section, "section .text");
    asm_append(&text_section, "global main");

    // 3. Ana program girişi (Prolog)
    asm_append(&text_section, "main:");
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)
    asm_append(&text_section, "    call tyd_fix_cwd"); // ✅ çalışma dizinini düzelt

    // 4. İki geçişli ziyaret
    ASTNode* blok = root;
    if (blok->type == AST_BLOK) {
        // İlk geçiş: Ana program komutları (fonksiyon tanımları hariç)
        asm_append(&text_section, "    ; --- Ana Program Akışı ---");
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if (node->type != AST_ISLEC_TANIMLAMA) {
                visit(node);
            }
        }
    } else {
        visit(root);
    }

    // 5. Program Çıkışı (Epilog)
    asm_append(&text_section, "    ; --- Program Sonu ---");
    asm_append(&text_section, "    xor rax, rax");
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");

    // 6. İkinci geçiş: Fonksiyon tanımları
    if (blok->type == AST_BLOK) {
        asm_append(&text_section, "");
        asm_append(&text_section, "; === Fonksiyon Tanımları ===");
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if (node->type == AST_ISLEC_TANIMLAMA) {
                visit(node);
            }
        }
    }

    // 7. Tüm bölümleri birleştir
    size_t stack_section_size = strlen("\nsection .note.GNU-stack noalloc noexec nowrite progbits\n\ndb 0\n");
    size_t total_size = data_section.size + text_section.size + stack_section_size + 2;
    char* final_code = (char*)malloc(total_size);
    if (final_code == NULL) {
        perror("Hafıza ayırma hatası (final assembly)");
        exit(EXIT_FAILURE);
    }

    strcpy(final_code, data_section.code);
    strcat(final_code, "\n");
    strcat(final_code, text_section.code);

    // ✅ NASM uyumlu stack notu + 1 byte içerik ekle (boş bölüm strip olmasın)
    strcat(final_code, "\nsection .note.GNU-stack noalloc noexec nowrite progbits\n");
    strcat(final_code, "\ndb 0\n");

    free_asm_code(&data_section);
    free_asm_code(&text_section);
    kapsam_temizle();

    return final_code;
}

