// c_compiler/main.c

#include "c_generator.h" // <-- YENİ EKLENDİ
#include "c_parser.h"
#include <stdio.h>
#include <locale.h> // <-- 1. YEREL AYAR İÇİN BAŞLIK EKLENDİ
#include <stdlib.h>
#include <limits.h> // For realpath
#include <string.h> // For strrchr
#include <libgen.h> // For dirname

// Global variable to store the absolute path of the input file's directory
char* base_dir = NULL;

void printAST_helper(ASTNode* node, int indent) {
    if (node == NULL) {
        printf("%*sNULL\n", indent, "");
        return;
    }

    if (node->type == AST_SAYI) {
        printf("%*sSAYI(%s)\n", indent, "", node->sabit_data.deger);
    } else if (node->type == AST_IKILI_ISLEM) {
        const char* op_str = "?";
        switch (node->ikili_islem_data.operator_type) {
            case TOKEN_PLUS: op_str = "+"; break;
            case TOKEN_MINUS: op_str = "-"; break;
            case TOKEN_MUL: op_str = "*"; break;
            case TOKEN_DIV: op_str = "/"; break;
            case TOKEN_GT: op_str = ">"; break;
            case TOKEN_LT: op_str = "<"; break;
            case TOKEN_OP_ESIT_KARSILASTIRMA: op_str = "=="; break;
            default: break;
        }
        printf("%*sIKILI_ISLEM(%s)\n", indent, "", op_str);
        printf("%*s  Sol:\n", indent, "");
        printAST_helper(node->ikili_islem_data.sol, indent + 4);
        printf("%*s  Sag:\n", indent, "");
        printAST_helper(node->ikili_islem_data.sag, indent + 4);
    } else if (node->type == AST_DEGISKEN) {
        printf("%*sDEGISKEN(%s)\n", indent, "", node->degisken_data.ad->value);
    } else if (node->type == AST_ISLEC_CAGIRMA) {
        printf("%*sISLEC_CAGIRMA(%s)\n", indent, "", node->islec_cagirma_data.hedef_ad->value);
        // TODO: Argümanları da bas
    } else if (node->type == AST_BLOK) {
        printf("%*sBLOK (Komut Sayısı: %d)\n", indent, "", node->blok_data.sayisi);
        for (int i = 0; i < node->blok_data.sayisi; i++) {
            printf("%*s  Komut[%d]:\n", indent, "", i);
            printAST_helper(node->blok_data.komutlar[i], indent + 4);
        }
    } else if (node->type == AST_YAZDIR_KOMUTU) {
        printf("%*sYAZDIR_KOMUTU\n", indent, "");
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->tek_ifade_data.ifade, indent + 4);
    } else if (node->type == AST_DEGISKEN_TANIMLAMA) {
        printf("%*sDEGISKEN_TANIMLAMA\n", indent, "");
        printf("%*s  Tip: %s\n", indent, "", node->tanimlama_data.tip->value);
        printf("%*s  Ad: %s\n", indent, "", node->tanimlama_data.ad->value);
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->tanimlama_data.ifade, indent + 4);
    } else if (node->type == AST_ATAMA_KOMUTU) {
        printf("%*sATAMA_KOMUTU\n", indent, "");
        printf("%*s  Hedef Ad: %s\n", indent, "", node->atama_data.ad->value);
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->atama_data.ifade, indent + 4);
    } else if (node->type == AST_KOSUL_KOMUTU) {
        printf("%*sKOSUL_KOMUTU (EĞER)\n", indent, "");
        printf("%*s  Koşul:\n", indent, "");
        printAST_helper(node->kosul_data.kosul, indent + 4);
        printf("%*s  İse Bloğu:\n", indent, "");
        printAST_helper(node->kosul_data.ise_blok, indent + 4);
        if (node->kosul_data.degilse_blok) {
            printf("%*s  Değilse Bloğu:\n", indent, "");
            printAST_helper(node->kosul_data.degilse_blok, indent + 4);
        }
    } else if (node->type == AST_DONGU_KOMUTU) {
        printf("%*sDONGU_KOMUTU\n", indent, "");
        printf("%*s  Gövde:\n", indent, "");
        printAST_helper(node->dongu_data.govde, indent + 4);
    }
    else if (node->type == AST_DONGU_BITIR_KOMUTU) {
        printf("%*sDONGU_BITIR_KOMUTU\n", indent, "");
    } else if (node->type == AST_ISLEC_TANIMLAMA) {
        printf("%*sISLEC_TANIMLAMA\n", indent, "");
        printf("%*s  Ad: %s\n", indent, "", node->islec_tanimlama_data.ad->value);
        printf("%*s  Gövde:\n", indent, "");
        printAST_helper(node->islec_tanimlama_data.govde, indent + 4);
    }else {
        printf("%*sAST { Tip: %d }\n", indent, "", node->type);
    }
}

// Function to be called from the generator
const char* get_base_dir() {
    return base_dir;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Kullanım: %s <girdi_dosyasi.tyd> <cikti_dosyasi.asm>\n", argv[0]);
        return 1;
    }

    const char* girdi_dosya_adi = argv[1];
    const char* cikti_dosya_adi = argv[2];

    // --- Resolve absolute path and set base_dir ---
    char* absolute_input_path = realpath(girdi_dosya_adi, NULL);
    if (absolute_input_path == NULL) {
        perror("Gerçek dosya yolu çözümlenemedi");
        return 1;
    }
    
    // dirname() can modify its argument, so we pass a copy.
    char* path_copy = strdup(absolute_input_path);
    base_dir = dirname(path_copy);
    // Note: base_dir now points to internal static storage of dirname 
    // or to path_copy. To be safe, let's copy it.
    base_dir = strdup(base_dir);
    // free(path_copy); // We can free the copy of the path
    // No, dirname may point inside path_copy, so we can't free it yet.
    // Let's leak it for now, or manage memory more carefully.
    // For this compiler, a small leak on startup is acceptable.

    // TÜRKÇE KARAKTER DESTEĞİNİ AKTİFLEŞTİR
    setlocale(LC_ALL, "");

    // --- Dosyadan Okuma ---
    FILE* girdi_dosyasi = fopen(absolute_input_path, "r");
    if (girdi_dosyasi == NULL) {
        perror("Girdi dosyası açılamadı");
        free(absolute_input_path);
        // free(base_dir) is tricky because of dirname behavior
        return 1;
    }
    free(absolute_input_path); // We have the content, no need for the path anymore

    fseek(girdi_dosyasi, 0, SEEK_END);
    long dosya_boyutu = ftell(girdi_dosyasi);
    fseek(girdi_dosyasi, 0, SEEK_SET);

    char* kaynak_kodu = (char*)malloc(dosya_boyutu + 1);
    fread(kaynak_kodu, 1, dosya_boyutu, girdi_dosyasi);
    kaynak_kodu[dosya_boyutu] = '\0';
    fclose(girdi_dosyasi);

    fprintf(stderr, "Derleniyor: %s\n", girdi_dosya_adi);

    // 1. Parser'ı çalıştır
    ASTNode* root = parse(kaynak_kodu);
    free(kaynak_kodu); // Kaynak kodunu serbest bırak

    fprintf(stderr, "Ayrıştırma Başarılı. Şimdi Assembly Üretiliyor...\n");

    // 2. Generator'ı Çalıştır
    char* asm_code = generate_asm(root);

    // --- Dosyaya Yazma ---
    FILE* cikti_dosyasi = fopen(cikti_dosya_adi, "w");
    if (cikti_dosyasi == NULL) {
        perror("Çıktı dosyası oluşturulamadı");
        free(asm_code);
        return 1;
    }
    fprintf(cikti_dosyasi, "%s", asm_code);
    fclose(cikti_dosyasi);

    free(asm_code);
    // free(base_dir); // Clean up the duplicated base directory path
    // free(path_copy);

    fprintf(stderr, "Başarıyla derlendi: %s -> %s\n", girdi_dosya_adi, cikti_dosya_adi);

    return 0;
}