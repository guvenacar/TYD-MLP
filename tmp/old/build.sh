#!/bin/bash

# TYD-MLP Build Script
# Kullanım: ./build.sh ornekler/test.tyd

set -e  # Hata olursa dur

# Renkler
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}═══════════════════════════════════════${NC}"
echo -e "${GREEN}   TYD-MLP Compiler & Builder${NC}"
echo -e "${GREEN}═══════════════════════════════════════${NC}"

# Argüman kontrolü
if [ $# -eq 0 ]; then
    echo -e "${RED}Hata: Kaynak dosyası belirtilmedi${NC}"
    echo "Kullanım: $0 <kaynak.tyd>"
    echo "Örnek: $0 ornekler/test.tyd"
    exit 1
fi

SOURCE_FILE=$1

if [ ! -f "$SOURCE_FILE" ]; then
    echo -e "${RED}Hata: Dosya bulunamadı: $SOURCE_FILE${NC}"
    exit 1
fi

echo -e "${YELLOW}Kaynak dosya: ${SOURCE_FILE}${NC}"
echo ""

# Build klasörünü oluştur
mkdir -p build

# 1. TYD Compiler (Python)
echo -e "${YELLOW}[1/4] TYD Compiler çalıştırılıyor...${NC}"
python3 -m src.compiler "$SOURCE_FILE" kurallar/diller.json

if [ $? -ne 0 ]; then
    echo -e "${RED}Derleme hatası!${NC}"
    exit 1
fi

# 2. Assembly → Object
echo -e "${YELLOW}[2/4] Assembly → Object file...${NC}"
nasm -f elf64 build/output.asm -o build/output.o

if [ $? -ne 0 ]; then
    echo -e "${RED}NASM hatası!${NC}"
    exit 1
fi

# 3. Runtime C → Object
echo -e "${YELLOW}[3/4] Runtime derleniyor...${NC}"
gcc -c runtime/runtime.c -o build/runtime.o

if [ $? -ne 0 ]; then
    echo -e "${RED}GCC hatası!${NC}"
    exit 1
fi

# 4. Linking
echo -e "${YELLOW}[4/4] Linking...${NC}"
gcc build/output.o build/runtime.o -o build/program -no-pie

if [ $? -ne 0 ]; then
    echo -e "${RED}Linking hatası!${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Derleme başarılı!${NC}"
echo -e "${GREEN}Çalıştırılabilir: ./build/program${NC}"
echo ""

# Otomatik çalıştır mı?
read -p "Programı şimdi çalıştırmak ister misiniz? (e/h): " -n 1 -r
echo
if [[ $REPLY =~ ^[Ee]$ ]]; then
    echo -e "${YELLOW}Program çıktısı:${NC}"
    echo "─────────────────────────────────────"
    ./build/program
    echo "─────────────────────────────────────"
fi

echo -e "${GREEN}Bitti!${NC}"
