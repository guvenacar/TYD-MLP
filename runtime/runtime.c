#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

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