// test_sozluk.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void sozluk_yok_et(void); // prototip
void sozluk_olustur(int boyut);

void sozluk_yok_et(void) {
    // dummy
}

void sozluk_olustur(int boyut) {
    if (1) {
        sozluk_yok_et(); // çağrı
    }
}

int main(void) {
    sozluk_olustur(10);
    return 0;
}
