#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>  // readlink için
#include <libgen.h>  // dirname için
#include <errno.h>   // errno için

// Forward declarations
char* runtime_dizin_al();

// Çalıştırılabilir dosyanın tam yolunu alır
char* get_executable_path() {
    char* path = (char*)malloc(1024);
    if (path == NULL) return NULL;

    ssize_t len = readlink("/proc/self/exe", path, 1023);
    if (len != -1) {
        path[len] = '\0';
        return path;
    }

    free(path);
    return NULL;
}

void yazdir_sayi(int64_t sayi) {
    printf("%ld\n", sayi);
}

void yazdir_metin(const char* metin) {
    printf("%s\n", metin);
}

typedef struct {
    char* anahtar;
    int64_t deger;
    int kullanilmis;
} SozlukGirdisi;

SozlukGirdisi* sozluk_tablosu = NULL;
int sozluk_boyutu = 0;

unsigned long hash_fonksiyonu(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void sozluk_yok_et(void) {
    if (sozluk_tablosu == NULL) return;
    for (int i = 0; i < sozluk_boyutu; i++) {
        if (sozluk_tablosu[i].kullanilmis && sozluk_tablosu[i].anahtar != NULL) {
            free(sozluk_tablosu[i].anahtar);
        }
    }
    free(sozluk_tablosu);
    sozluk_tablosu = NULL;
    sozluk_boyutu = 0;
}

void sozluk_olustur(int boyut) {
    if (sozluk_tablosu != NULL) {
        sozluk_yok_et();
    }
    sozluk_boyutu = boyut;
    sozluk_tablosu = (SozlukGirdisi*)calloc(sozluk_boyutu, sizeof(SozlukGirdisi));
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük için hafıza ayrılamadı.\n");
        exit(1);
    }
}

void sozluk_ekle(const char* anahtar, int64_t deger) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            sozluk_tablosu[indeks].deger = deger;
            return;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            fprintf(stderr, "Hata: Sözlük dolu!\n");
            return;
        }
    }
    sozluk_tablosu[indeks].anahtar = strdup(anahtar);
    sozluk_tablosu[indeks].deger = deger;
    sozluk_tablosu[indeks].kullanilmis = 1;
}

int64_t sozluk_getir(const char* anahtar) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return -1;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            return sozluk_tablosu[indeks].deger;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            break;
        }
    }
    fprintf(stderr, "Hata: Anahtar bulunamadı: %s\n", anahtar);
    return -1;
}

char* string_birlestir(const char* str1, const char* str2) {
    
    // ⚠️ KORUMA: Gelen string'lerden herhangi biri NULL ise
    // (Bellek hatası/Segfault riskini engellemek için)
    if (str1 == NULL || str2 == NULL) {
        // Hata ayıklama için terminale yazdıralım
        fprintf(stderr, "Hata: Birleştirme için NULL string argümanı alındı.\n");
        return NULL; // Bu, Assembly'ye de NULL dönecektir.
    }
    
    // 1. Yeni string'in toplam uzunluğunu hesapla (+1, null terminator için)
    size_t uzunluk = strlen(str1) + strlen(str2) + 1;

    // 2. Yeni hafıza bloğu ayır (Dinamik hafıza yönetimi)
    char* yeni_str = (char*)malloc(uzunluk);
    if (yeni_str == NULL) {
        fprintf(stderr, "Hata: Birleştirme için hafıza ayrılamadı.\n");
        return NULL; 
    }

    // 3. İlk string'i yeni bloğa kopyala
    strcpy(yeni_str, str1);

    // 4. İkinci string'i ilkinin sonuna ekle (birleştir)
    strcat(yeni_str, str2);

    // 5. Birleştirilmiş yeni string'in adresini döndür
    return yeni_str;
}

// NOT: Bu fonksiyonun döndürdüğü string'in hafızasını 
// SIL komutu veya SOZLUK_YOK_ET gibi fonksiyonlarla serbest bırakmayı unutmamalıyız.

int string_karsilastir(const char* str1, const char* str2) {
    // strcmp her iki string eşitse 0 döndürür.
    // Assembly'de bu sonucu doğrudan kullanacağız.
    return strcmp(str1, str2);
}

/**
 * STRING_ESIT_MI - İki string'in eşit olup olmadığını kontrol eder
 * @param str1: İlk string
 * @param str2: İkinci string
 * @return: Eşitse 1, değilse 0 (Boolean)
 */
int64_t string_esit_mi(const char* str1, const char* str2) {
    if (str1 == NULL || str2 == NULL) {
        return 0;  // NULL stringler eşit değildir
    }
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

/**
 * STRING_UZUNLUK - String'in uzunluğunu döndürür
 * @param str: String
 * @return: String uzunluğu (karakter sayısı)
 */
int64_t string_uzunluk(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_UZUNLUK]: NULL string\n");
        return 0;
    }
    return (int64_t)strlen(str);
}

// =============================================================================
// FILE I/O FONKSİYONLARI
// =============================================================================

/**
 * DOSYA_AC - Dosya açar ve dosya tanıtıcısını (FILE*) döndürür
 * @param yol: Dosya yolu (string) - relative veya absolute
 * @param mod: Açma modu ("r", "w", "a", vb.)
 * @return: FILE* pointer (int64_t olarak cast edilmiş)
 *
 * NOT: Eğer yol relative ise (/ ile başlamıyorsa), programın bulunduğu
 * dizin ile birleştirilerek absolute path oluşturulur. Bu sayede
 * program farklı dizinlerden çalıştırılsa bile dosyaları bulabilir.
 */
int64_t dosya_ac(const char* yol, const char* mod) {
    if (yol == NULL || mod == NULL) {
        fprintf(stderr, "HATA [DOSYA_AC]: NULL argüman alındı\n");
        return 0; // NULL pointer
    }

    char* kullanilacak_yol = NULL;
    int path_allocated = 0; // Flag to track if we allocated memory

    // Path absolute mi kontrol et (/ ile başlıyorsa absolute)
    if (yol[0] == '/') {
        // Absolute path - olduğu gibi kullan
        kullanilacak_yol = (char*)yol;
    } else {
        // Relative path - executable'ın dizini ile birleştir
        char* exe_dir = runtime_dizin_al();
        if (exe_dir == NULL) {
            fprintf(stderr, "HATA [DOSYA_AC]: Executable dizini alınamadı\n");
            return 0;
        }

        // Birleştir: exe_dir + "/" + yol
        size_t uzunluk = strlen(exe_dir) + 1 + strlen(yol) + 1;
        kullanilacak_yol = (char*)malloc(uzunluk);
        if (kullanilacak_yol == NULL) {
            free(exe_dir);
            fprintf(stderr, "HATA [DOSYA_AC]: Hafıza ayırma hatası\n");
            return 0;
        }

        snprintf(kullanilacak_yol, uzunluk, "%s/%s", exe_dir, yol);
        free(exe_dir);
        path_allocated = 1;
    }

    FILE* dosya = fopen(kullanilacak_yol, mod);

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_AC]: Dosya açılamadı: %s\n", kullanilacak_yol);
        if (path_allocated) {
            free(kullanilacak_yol);
        }
        return 0;
    }

    // Eğer yeni path oluşturduysak, onu serbest bırak
    if (path_allocated) {
        free(kullanilacak_yol);
    }

    // FILE* pointer'ı int64_t olarak döndür
    return (int64_t)dosya;
}

/**
 * DOSYA_OKU - Dosyadan tüm içeriği okur ve string olarak döndürür
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @return: Dosya içeriği (dynamically allocated string)
 */
char* dosya_oku(int64_t dosya_ptr) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Geçersiz dosya pointer\n");
        return NULL;
    }

    // Dosya boyutunu öğren
    fseek(dosya, 0, SEEK_END);
    long dosya_boyutu = ftell(dosya);
    fseek(dosya, 0, SEEK_SET);

    if (dosya_boyutu < 0) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Dosya boyutu okunamadı\n");
        return NULL;
    }

    // Bellek ayır
    char* icerik = (char*)malloc(dosya_boyutu + 1);
    if (icerik == NULL) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Hafıza ayırma hatası\n");
        return NULL;
    }

    // Dosyayı oku
    size_t okunan = fread(icerik, 1, dosya_boyutu, dosya);
    icerik[okunan] = '\0'; // Null terminator

    return icerik;
}

/**
 * DOSYA_YAZ - Dosyaya string yazar
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @param veri: Yazılacak string
 * @return: Yazılan byte sayısı
 */
int64_t dosya_yaz(int64_t dosya_ptr, const char* veri) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_YAZ]: Geçersiz dosya pointer\n");
        return -1;
    }

    if (veri == NULL) {
        fprintf(stderr, "HATA [DOSYA_YAZ]: NULL veri\n");
        return -1;
    }

    size_t yazilan = fwrite(veri, 1, strlen(veri), dosya);
    return (int64_t)yazilan;
}

/**
 * DOSYA_KAPAT - Dosyayı kapatır
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @return: 0 başarılı, -1 hata
 */
int64_t dosya_kapat(int64_t dosya_ptr) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_KAPAT]: Geçersiz dosya pointer (NULL)\n");
        return -1;
    }

    int sonuc = fclose(dosya);
    if (sonuc != 0) {
        fprintf(stderr, "HATA [DOSYA_KAPAT]: Dosya kapatılamadı (errno=%d)\n", errno);
        return -1;
    }

    return 0;
}

// =============================================================================
// YOL (PATH) YÖNETİMİ
// =============================================================================

/**
 * DIZIN_AL - Çalışan programın bulunduğu dizini döndürür.
 * (Self-hosting için dosya yolu sorununu çözer)
 * * @return: Programın dizin yolu (char*). Bu hafıza 'free' edilmeli.
 */
char* tyd_fix_cwd() {
    // 1. Programın tam yolunu al (zaten var olan fonksiyon)
    char* exe_path = get_executable_path();
    if (exe_path == NULL) {
        fprintf(stderr, "HATA [DIZIN_AL]: Program yolu alınamadı.\n");
        return NULL;
    }

    // 2. 'dirname' girdisini değiştirebilir, bu yüzden bir kopya üzerinde çalış
    // (Aksi takdirde 'exe_path' belleği bozulabilir)
    char* path_copy = strdup(exe_path);
    if (path_copy == NULL) {
        free(exe_path);
        fprintf(stderr, "HATA [DIZIN_AL]: Hafıza kopyalama hatası.\n");
        return NULL;
    }

    // 3. Dizin adını al (örn: /home/pardus/proje/c_compiler)
    char* dir = dirname(path_copy);

    // 4. 'dir' şu anda path_copy'nin içini işaret ediyor.
    // Bellek sızıntısını önlemek için 'dir'in de bir kopyasını oluşturup 
    // onu döndürmeliyiz.
    char* result = strdup(dir);

    // 5. Ara bellekleri temizle
    free(exe_path);   // Orijinal path'i serbest bırak
    free(path_copy);  // Kopyayı serbest bırak

    // 6. Yeni, bağımsız dizin kopyasını döndür
    return result;
}

char* runtime_dizin_al() {
    // 1. Programın tam yolunu al (zaten var olan fonksiyon)
    char* exe_path = get_executable_path();
    if (exe_path == NULL) {
        fprintf(stderr, "HATA [DIZIN_AL]: Program yolu alınamadı.\n");
        return NULL;
    }

    // 2. 'dirname' girdisini değiştirebilir, bu yüzden bir kopya üzerinde çalış
    // (Aksi takdirde 'exe_path' belleği bozulabilir)
    char* path_copy = strdup(exe_path);
    if (path_copy == NULL) {
        free(exe_path);
        fprintf(stderr, "HATA [DIZIN_AL]: Hafıza kopyalama hatası.\n");
        return NULL;
    }

    // 3. Dizin adını al (örn: /home/pardus/proje/c_compiler)
    char* dir = dirname(path_copy);

    // 4. 'dir' şu anda path_copy'nin içini işaret ediyor.
    // Bellek sızıntısını önlemek için 'dir'in de bir kopyasını oluşturup
    // onu döndürmeliyiz.
    char* result = strdup(dir);

    // 5. Ara bellekleri temizle
    free(exe_path);   // Orijinal path'i serbest bırak
    free(path_copy);  // Kopyayı serbest bırak

    // 6. Yeni, bağımsız dizin kopyasını döndür
    return result;
}

// =============================================================================
// STRING İŞLEMLERİ (BOOTSTRAP FONKSİYONLARI)
// =============================================================================
// NOT: Bu fonksiyonlar sadece TYD compiler'ını TYD'de yazmak için gerekli.
// Self-hosting tamamlandıktan sonra bu işlevler TYD'de yeniden yazılacak.

/**
 * STRING_KARAKTER_AL - String'in belirtilen indeksindeki karakteri döndürür
 * @param str: Kaynak string
 * @param indeks: Karakter indeksi (0-tabanlı)
 * @return: Tek karakterlik string (dynamically allocated)
 *
 * Örnek: STRING_KARAKTER_AL("Merhaba", 0) -> "M"
 */
char* string_karakter_al(const char* str, int64_t indeks) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: NULL string\n");
        return NULL;
    }

    int64_t uzunluk = strlen(str);
    if (indeks < 0 || indeks >= uzunluk) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: İndeks sınır dışı (indeks=%ld, uzunluk=%ld)\n",
                indeks, uzunluk);
        return NULL;
    }

    // Tek karakterlik string oluştur
    char* sonuc = (char*)malloc(2); // 1 karakter + null terminator
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: Hafıza ayırma hatası\n");
        return NULL;
    }

    sonuc[0] = str[indeks];
    sonuc[1] = '\0';

    return sonuc;
}

/**
 * STRING_ALT - String'in bir kısmını (substring) döndürür
 * @param str: Kaynak string
 * @param baslangic: Başlangıç indeksi (0-tabanlı, dahil)
 * @param uzunluk: Alınacak karakter sayısı
 * @return: Substring (dynamically allocated)
 *
 * Örnek: STRING_ALT("Merhaba", 3, 2) -> "ha"
 */
char* string_alt(const char* str, int64_t baslangic, int64_t uzunluk) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_ALT]: NULL string\n");
        return NULL;
    }

    int64_t str_uzunluk = strlen(str);

    if (baslangic < 0 || baslangic >= str_uzunluk) {
        fprintf(stderr, "HATA [STRING_ALT]: Başlangıç indeksi sınır dışı\n");
        return NULL;
    }

    if (uzunluk < 0) {
        fprintf(stderr, "HATA [STRING_ALT]: Negatif uzunluk\n");
        return NULL;
    }

    // Gerçek alınabilecek uzunluğu hesapla (string sonunu aşmamalı)
    int64_t gercek_uzunluk = uzunluk;
    if (baslangic + uzunluk > str_uzunluk) {
        gercek_uzunluk = str_uzunluk - baslangic;
    }

    // Yeni string için bellek ayır
    char* sonuc = (char*)malloc(gercek_uzunluk + 1);
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [STRING_ALT]: Hafıza ayırma hatası\n");
        return NULL;
    }

    // Karakterleri kopyala
    strncpy(sonuc, str + baslangic, gercek_uzunluk);
    sonuc[gercek_uzunluk] = '\0';

    return sonuc;
}

/**
 * KARAKTER_KODU - Bir karakterin ASCII kodunu döndürür
 * @param karakter_str: Tek karakterlik string
 * @return: ASCII kodu (int64_t)
 *
 * Örnek: KARAKTER_KODU("A") -> 65
 */
int64_t karakter_kodu(const char* karakter_str) {
    if (karakter_str == NULL || karakter_str[0] == '\0') {
        fprintf(stderr, "HATA [KARAKTER_KODU]: NULL veya boş string\n");
        return -1;
    }

    return (int64_t)karakter_str[0];
}

/**
 * KODU_KARAKTERE - ASCII kodunu karaktere dönüştürür
 * @param kod: ASCII kodu
 * @return: Tek karakterlik string (dynamically allocated)
 *
 * Örnek: KODU_KARAKTERE(65) -> "A"
 */
char* kodu_karaktere(int64_t kod) {
    if (kod < 0 || kod > 127) {
        fprintf(stderr, "HATA [KODU_KARAKTERE]: Geçersiz ASCII kodu: %ld\n", kod);
        return NULL;
    }

    char* sonuc = (char*)malloc(2);
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [KODU_KARAKTERE]: Hafıza ayırma hatası\n");
        return NULL;
    }

    sonuc[0] = (char)kod;
    sonuc[1] = '\0';

    return sonuc;
}

