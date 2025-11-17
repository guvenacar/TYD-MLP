#!/bin/bash

# Hata durumunda script'i durdur
set -e

TYD_KAYNAK_DOSYASI=$1

if [ -z "$TYD_KAYNAK_DOSYASI" ]; then
    echo "Kullanım: ./derle.sh <dosya_adi.tyd>"
    exit 1
fi

# Dosya adından uzantıyı kaldırarak temel adı al
TEMEL_AD=$(basename -s .tyd "$TYD_KAYNAK_DOSYASI")

ASM_DOSYASI="$TEMEL_AD.asm"
NESNE_DOSYASI="$TEMEL_AD.o"
CALISTIRILABILIR_DOSYA="$TEMEL_AD"

echo "--- TYD Derleyici Zinciri Başlatılıyor ---"

echo "1. TYD -> Assembly"
./compiler_test "$TYD_KAYNAK_DOSYASI" "$ASM_DOSYASI"

echo "2. Assembly -> Nesne Kodu (nasm)"
nasm -f elf64 "$ASM_DOSYASI" -o "$NESNE_DOSYASI"

echo "3. Nesne Kodu -> Çalıştırılabilir (gcc linker + runtime)"
gcc "$NESNE_DOSYASI" ../runtime.o -o "$CALISTIRILABILIR_DOSYA" -no-pie

echo "--- Derleme Tamamlandı: '$CALISTIRILABILIR_DOSYA' oluşturuldu. ---"