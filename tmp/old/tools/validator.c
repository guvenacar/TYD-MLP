// TYD Syntax Validator - SPECS.md kurallarını zorunlu kılar

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_LINE 1024
#define MAX_VIOLATIONS 1000

typedef struct {
    char filename[256];
    int line_number;
    char message[512];
} Violation;

Violation violations[MAX_VIOLATIONS];
int violation_count = 0;

// Satırda yorum mu var kontrol et
int is_comment(const char* line) {
    const char* trimmed = line;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
    return (trimmed[0] == '-' && trimmed[1] == '-');
}

// Kural 1: Noktalı virgül kontrolü
void check_semicolon_usage(const char* filename, const char* line, int line_num) {
    // Yorum satırlarını atla
    if (is_comment(line)) return;
    
    // Satırda ; var mı?
    const char* semicolon = strchr(line, ';');
    if (!semicolon) return;
    
    // İzin verilen: SAYISAL/METIN/MANTIKSAL ... ;
    if (strstr(line, "SAYISAL") || strstr(line, "METIN") || strstr(line, "MANTIKSAL")) {
        return; // OK
    }
    
    // İhlal tespit edildi
    snprintf(violations[violation_count].filename, 256, "%s", filename);
    violations[violation_count].line_number = line_num;
    snprintf(violations[violation_count].message, 512,
             "';' sadece değişken tanımında kullanılabilir (SPECS.md Kural 1)");
    violation_count++;
}

// Kural 2: Blok sonlandırma kontrolü
void check_block_endings(const char* filename, const char* line, int line_num) {
    if (is_comment(line)) return;
    
    // SON; kullanımı yasak
    if (strstr(line, "SON") && strchr(line, ';')) {
        char* son_pos = strstr(line, "SON");
        char* semi_pos = strchr(son_pos, ';');
        
        // SON ve ; aynı satırda ve aralarında sadece boşluk/İŞLEÇ/EĞER/DÖNGÜ var mı?
        int only_whitespace = 1;
        for (char* p = son_pos + 3; p < semi_pos; p++) {
            if (*p != ' ' && *p != '\t' && *p != '\n') {
                // İŞLEÇ, EĞER, DÖNGÜ gibi kelimeler kontrol edilebilir
                // Şimdilik basit kontrol
                only_whitespace = 0;
                break;
            }
        }
        
        if (only_whitespace) {
            snprintf(violations[violation_count].filename, 256, "%s", filename);
            violations[violation_count].line_number = line_num;
            snprintf(violations[violation_count].message, 512,
                     "'SON;' kullanılamaz. Doğrusu: 'SON İŞLEÇ' (SPECS.md Kural 2)");
            violation_count++;
        }
    }
}

// Tek bir dosyayı kontrol et
void validate_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "UYARI: %s okunamadı\n", filepath);
        return;
    }
    
    char line[MAX_LINE];
    int line_num = 0;
    
    while (fgets(line, MAX_LINE, file)) {
        line_num++;
        check_semicolon_usage(filepath, line, line_num);
        check_block_endings(filepath, line, line_num);
    }
    
    fclose(file);
}

// Dizin içindeki .tyd dosyalarını bul (recursive)
void find_tyd_files(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Gizli dosyaları atla
        
        char filepath[512];
        snprintf(filepath, 512, "%s/%s", dir_path, entry->d_name);
        
        struct stat statbuf;
        if (stat(filepath, &statbuf) != 0) continue;
        
        if (S_ISDIR(statbuf.st_mode)) {
            // Dizinse recursive çağrı
            find_tyd_files(filepath);
        } else if (strstr(entry->d_name, ".tyd")) {
            // .tyd dosyasıysa kontrol et
            validate_file(filepath);
        }
    }
    
    closedir(dir);
}

int main(int argc, char* argv[]) {
    printf("🔍 TYD Syntax Validator (SPECS.md Compliance Checker)\n");
    printf("================================================\n\n");
    
    if (argc < 2) {
        fprintf(stderr, "Kullanım: %s <dosya.tyd veya dizin>\n", argv[0]);
        fprintf(stderr, "Örnek: %s ornekler/\n", argv[0]);
        fprintf(stderr, "Örnek: %s test.tyd\n", argv[0]);
        return 1;
    }
    
    // Argümanları işle
    for (int i = 1; i < argc; i++) {
        struct stat statbuf;
        if (stat(argv[i], &statbuf) != 0) {
            fprintf(stderr, "HATA: %s bulunamadı\n", argv[i]);
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) {
            printf("📁 Dizin taranıyor: %s\n", argv[i]);
            find_tyd_files(argv[i]);
        } else {
            printf("📄 Dosya kontrol ediliyor: %s\n", argv[i]);
            validate_file(argv[i]);
        }
    }
    
    printf("\n================================================\n");
    
    if (violation_count == 0) {
        printf("✅ Tüm dosyalar SPECS.md'ye uygun!\n");
        return 0;
    } else {
        printf("❌ %d SPECS.md ihlali tespit edildi:\n\n", violation_count);
        
        for (int i = 0; i < violation_count; i++) {
            printf("%s:%d\n", violations[i].filename, violations[i].line_number);
            printf("  └─ %s\n\n", violations[i].message);
        }
        
        printf("🚫 Lütfen SPECS.md'yi kontrol edin ve hataları düzeltin.\n");
        return 1;
    }
}