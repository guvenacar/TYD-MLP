#!/bin/bash
# TYD programını derleyip çalıştırır (nasm gerektirir ama sadece assembly üretebilir)

set -e

TYD_DOSYA=$1

if [ -z "$TYD_DOSYA" ]; then
    echo "Kullanım: ./calistir.sh <dosya.tyd>"
    exit 1
fi

TEMEL_AD=$(basename -s .tyd "$TYD_DOSYA")
ASM_DOSYA="${TEMEL_AD}.asm"

echo "=== TYD -> Assembly ==="
./compiler_test "$TYD_DOSYA" "$ASM_DOSYA"

# Eğer nasm varsa, binary oluştur ve çalıştır
if command -v nasm &> /dev/null; then
    echo "=== Assembly -> Binary ==="
    OBJ_DOSYA="${TEMEL_AD}.o"
    BIN_DOSYA="${TEMEL_AD}"

    nasm -f elf64 "$ASM_DOSYA" -o "$OBJ_DOSYA"
    gcc "$OBJ_DOSYA" ../runtime.o -o "$BIN_DOSYA" -no-pie

    echo "=== Çalıştırılıyor: $BIN_DOSYA ==="
    ./"$BIN_DOSYA"
else
    echo "NOT: nasm bulunamadı, sadece assembly oluşturuldu: $ASM_DOSYA"
fi
