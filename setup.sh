#!/bin/bash

# TYD-MLP Kurulum Script'i
# Usage: ./setup.sh

set -e

echo "=================================="
echo "TYD-MLP Kurulum Başlıyor..."
echo "=================================="

# Renk kodları
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 1. Python kontrolü
echo -e "\n${YELLOW}[1/5]${NC} Python kontrolü..."
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}✗ Python 3 bulunamadı!${NC}"
    echo "  Lütfen Python 3.7+ kurun: sudo apt-get install python3"
    exit 1
fi
PYTHON_VERSION=$(python3 --version | cut -d' ' -f2)
echo -e "${GREEN}✓ Python $PYTHON_VERSION bulundu${NC}"

# 2. GCC kontrolü ve runtime derleme
echo -e "\n${YELLOW}[2/5]${NC} C Runtime derleniyor..."
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}✗ GCC bulunamadı!${NC}"
    echo "  Lütfen GCC kurun: sudo apt-get install gcc"
    exit 1
fi

cd runtime
if [ ! -f runtime.o ] || [ runtime.c -nt runtime.o ]; then
    gcc -c runtime.c -o runtime.o -std=c11 -Wall
    echo -e "${GREEN}✓ Runtime derlendi${NC}"
else
    echo -e "${GREEN}✓ Runtime zaten derlenmiş${NC}"
fi
cd ..

# 3. Build klasörü oluştur
echo -e "\n${YELLOW}[3/5]${NC} Build klasörü hazırlanıyor..."
mkdir -p build
echo -e "${GREEN}✓ build/ klasörü hazır${NC}"

# 4. Test programı derle
echo -e "\n${YELLOW}[4/5]${NC} Test programı derleniyor..."
cat > build/test.tyd << 'EOF'
SAYISAL işleç main()
    YAZDIR "TYD-MLP başarıyla kuruldu!";
    YAZDIR "Versiyon: 0.9.5 (Bootstrap)";
    DÖNÜŞ 0;
SON

main();
EOF

python3 -m src.compiler build/test.tyd > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Test programı derlendi${NC}"
else
    echo -e "${RED}✗ Derleme başarısız!${NC}"
    exit 1
fi

# 5. NASM kontrolü (opsiyonel)
echo -e "\n${YELLOW}[5/5]${NC} Build tools kontrolü..."
if command -v nasm &> /dev/null && command -v gcc &> /dev/null; then
    echo -e "${GREEN}✓ NASM ve GCC mevcut - Executable oluşturulabilir${NC}"

    # Test programını executable yap
    nasm -f elf64 build/output.asm -o build/output.o 2>/dev/null
    gcc build/output.o runtime/runtime.o -o build/tyd_test 2>/dev/null

    if [ -f build/tyd_test ]; then
        echo -e "\n${GREEN}Test programı çalıştırılıyor:${NC}"
        echo "---"
        ./build/tyd_test
        echo "---"
    fi
else
    echo -e "${YELLOW}⚠ NASM bulunamadı - Sadece Assembly üretilecek${NC}"
    echo "  Executable oluşturmak için: sudo apt-get install nasm"
fi

# Özet
echo -e "\n=================================="
echo -e "${GREEN}✓ Kurulum Tamamlandı!${NC}"
echo "=================================="
echo ""
echo "Hızlı Başlangıç:"
echo "  1. TYD dosyası oluştur:     touch program.tyd"
echo "  2. Derle (Assembly):        python3 -m src.compiler program.tyd"
echo "  3. Çalıştır (Executable):   nasm -f elf64 build/output.asm -o build/output.o"
echo "                              gcc build/output.o runtime/runtime.o -o program"
echo "                              ./program"
echo ""
echo "VSCode Extension:"
echo "  cd vscode-tyd && npm install && npm run compile"
echo "  VSCode'da F5'e basarak test edin"
echo ""
echo "Dokümantasyon:"
echo "  - README.md"
echo "  - BOOTSTRAP_STATUS.md"
echo "  - PHASE8_COMPLETION.md"
echo ""
echo "Örnek Programlar:"
echo "  - test_simple_arithmetic.tyd"
echo "  - test_oop_simple.tyd"
echo "  - test_file_io.tyd"
echo ""
echo -e "${YELLOW}Not: Şu an Python'a bağımlıdır (bootstrap aşamasında)${NC}"
echo ""
