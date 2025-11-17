#!/bin/bash

# Hata durumunda çık
set -e

# Proje dizini (betiğin çalıştığı yer)
PROJE_DIZINI=$(pwd)
C_DERLEYICI_DIZINI="$PROJE_DIZINI/c_compiler"
RUNTIME_DIZINI="$PROJE_DIZINI/runtime"
BUILD_DIZINI="$PROJE_DIZINI/build"

echo "=== TYD-MLP Stage 1 Bootstrap Testi ==="
echo "Proje Dizini: $PROJE_DIZINI"
echo "Build Dizini: $BUILD_DIZINI"

# 1. Gerekli dizinleri oluştur
mkdir -p $BUILD_DIZINI
mkdir -p $C_DERLEYICI_DIZINI/build # TYD derleyicisinin yazacağı yer

# 2. Stage 0: C Derleyicisini (compiler_test) ve Runtime'ı Derle
echo ""
echo "--- Stage 0: C Derleyicisi (compiler_test) ve Runtime (runtime.o) derleniyor..."
gcc -c -o $BUILD_DIZINI/runtime.o $RUNTIME_DIZINI/runtime.c
echo "runtime.o olusturuldu."

gcc $C_DERLEYICI_DIZINI/main.c \
    $C_DERLEYICI_DIZINI/c_lexer.c \
    $C_DERLEYICI_DIZINI/c_parser.c \
    $C_DERLEYICI_DIZINI/c_generator.c \
    -o $C_DERLEYICI_DIZINI/compiler_test
echo "compiler_test (Stage 0) olusturuldu."

# 3. Stage 1: C Derleyicisini kullanarak TYD Derleyicisini (self-host-v1.tyd) derle
echo ""
echo "--- Stage 1: TYD Derleyicisi (self-host-v1.tyd) derleniyor..."

# Kaynak TYD kodu (self-host derleyicisi)
KAYNAK_TYD="$C_DERLEYICI_DIZINI/self-host-v1.tyd"

# Çıktı dosyaları (self-host derleyicisinin binary'si)
CIKTI_ASM="$BUILD_DIZINI/self_host_v1.asm"
CIKTI_O="$BUILD_DIZINI/self_host_v1.o"
STAGE1_BINARY="$BUILD_DIZINI/self_host_v1" # Bu bizim Stage 1 derleyicimiz

# Stage 0 derleyicisini çalıştır
$C_DERLEYICI_DIZINI/compiler_test $KAYNAK_TYD $CIKTI_ASM
echo "Stage 0 -> $CIKTI_ASM (Assembly olustu)"

# NASM
nasm -f elf64 -o $CIKTI_O $CIKTI_ASM
echo "NASM -> $CIKTI_O (Nesne dosyasi olustu)"

# GCC (Linkleme)
gcc -no-pie $CIKTI_O $BUILD_DIZINI/runtime.o -o $STAGE1_BINARY
echo "GCC -> $STAGE1_BINARY (Stage 1 Derleyici olustu!)"

# 4. Test: Stage 1 Derleyicisini Çalıştır
echo ""
echo "--- TEST: Stage 1 Derleyici ($STAGE1_BINARY) calistiriliyor..."
echo "Stage 1 Derleyici Çıktısı:"
echo "-----------------------------------"
$STAGE1_BINARY
echo "-----------------------------------"
echo ""

# 5. Doğrulama
if [ -f "$C_DERLEYICI_DIZINI/build/generated.asm" ]; then
    echo "✅ BAŞARILI! Stage 1 Derleyici ($STAGE1_BINARY) çalıştı ve '$C_DERLEYICI_DIZINI/build/generated.asm' dosyasını oluşturdu."
    echo "Self-Hosting (Stage 1) tamamlandı!"
else
    echo "❌ BAŞARISIZ! Stage 1 Derleyici çalıştı ancak beklenen '$C_DERLEYICI_DIZINI/build/generated.asm' dosyası bulunamadı."
fi

echo "=== Bootstrap Testi Tamamlandı ==="