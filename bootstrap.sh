#!/bin/bash
#
# ===============================================
# MLP Bootstrap Script
# ===============================================
#
# Bu script MLP compiler'ı self-hosting hale getirir.
#
# Bootstrap Süreci:
#   1. Python TYD-MLP ile MLP compiler kaynaklarını (.mlp) assembly'e çevir
#   2. NASM ile assembly'leri object file'lara derle
#   3. GCC ile object file'ları link edip 'mlpc' executable oluştur
#   4. mlpc'yi test et
#   5. (Opsiyonel) mlpc ile kendi kaynak kodunu tekrar derle (Stage 1 → Stage 2)
#
# Kullanım:
#   ./bootstrap.sh

set -e  # Hata olursa dur

# Renkler
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo ""
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo -e "${CYAN}   MLP BOOTSTRAP - Self-Hosting${NC}"
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo ""

# Build klasörünü oluştur
echo -e "${YELLOW}Hazırlık: Build klasörü oluşturuluyor...${NC}"
mkdir -p build/bootstrap
echo -e "${GREEN}✓ build/bootstrap/ oluşturuldu${NC}"
echo ""

# ===============================================
# STAGE 0: Gereksinimler Kontrolü
# ===============================================

echo -e "${BLUE}[STAGE 0] Gereksinimler kontrol ediliyor...${NC}"

# Python kontrolü
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}HATA: python3 bulunamadı${NC}"
    exit 1
fi
echo -e "  ✓ python3 bulundu: $(python3 --version)"

# NASM kontrolü
if ! command -v nasm &> /dev/null; then
    echo -e "${RED}HATA: nasm bulunamadı${NC}"
    echo -e "${YELLOW}Yüklemek için: sudo apt-get install nasm${NC}"
    exit 1
fi
echo -e "  ✓ nasm bulundu: $(nasm -version | head -1)"

# GCC kontrolü
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}HATA: gcc bulunamadı${NC}"
    exit 1
fi
echo -e "  ✓ gcc bulundu: $(gcc --version | head -1)"

echo -e "${GREEN}✓ Tüm gereksinimler mevcut${NC}"
echo ""

# ===============================================
# STAGE 1: MLP Kaynaklarını Assembly'e Çevir
# ===============================================

echo -e "${BLUE}[STAGE 1] MLP Compiler kaynaklarını assembly'e çeviriyoruz...${NC}"
echo -e "${YELLOW}  (Python TYD-MLP compiler kullanılıyor)${NC}"
echo ""

# MLP dosyaları
MLP_FILES=(
    "src/compiler/lexer.mlp"
    "src/compiler/parser.mlp"
    "src/compiler/codegen.mlp"
    "src/compiler/main.mlp"
)

# Her MLP dosyasını derle
for mlp_file in "${MLP_FILES[@]}"; do
    if [ ! -f "$mlp_file" ]; then
        echo -e "${RED}HATA: $mlp_file bulunamadı${NC}"
        exit 1
    fi

    # Dosya adını al (uzantısız)
    filename=$(basename "$mlp_file" .mlp)

    echo -e "${YELLOW}  [1.1] Derleniyor: $mlp_file${NC}"

    # Python compiler ile derle
    python3 -m src.compiler "$mlp_file" kurallar/diller.json

    if [ $? -ne 0 ]; then
        echo -e "${RED}HATA: $mlp_file derlenemedi${NC}"
        exit 1
    fi

    # output.asm'i yeniden adlandır
    mv build/output.asm "build/bootstrap/${filename}.asm"
    echo -e "${GREEN}  ✓ $filename.asm oluşturuldu${NC}"
done

echo ""
echo -e "${GREEN}✓ STAGE 1 tamamlandı: Tüm MLP dosyaları assembly'e çevrildi${NC}"
echo ""

# ===============================================
# STAGE 2: Assembly'leri Object File'lara Çevir
# ===============================================

echo -e "${BLUE}[STAGE 2] Assembly dosyalarını object file'lara çeviriyoruz...${NC}"
echo ""

# Her assembly dosyasını NASM ile derle
for mlp_file in "${MLP_FILES[@]}"; do
    filename=$(basename "$mlp_file" .mlp)
    asm_file="build/bootstrap/${filename}.asm"
    obj_file="build/bootstrap/${filename}.o"

    echo -e "${YELLOW}  [2.1] NASM: $asm_file → $obj_file${NC}"

    nasm -f elf64 "$asm_file" -o "$obj_file"

    if [ $? -ne 0 ]; then
        echo -e "${RED}HATA: $asm_file NASM ile derlenemedi${NC}"
        exit 1
    fi

    echo -e "${GREEN}  ✓ $obj_file oluşturuldu${NC}"
done

echo ""
echo -e "${GREEN}✓ STAGE 2 tamamlandı: Tüm object file'lar oluşturuldu${NC}"
echo ""

# ===============================================
# STAGE 3: C Runtime'ı Derle
# ===============================================

echo -e "${BLUE}[STAGE 3] C Runtime derleniyor...${NC}"
echo ""

if [ ! -f "runtime/runtime.c" ]; then
    echo -e "${RED}HATA: runtime/runtime.c bulunamadı${NC}"
    exit 1
fi

echo -e "${YELLOW}  [3.1] GCC: runtime/runtime.c → build/bootstrap/runtime.o${NC}"

gcc -c runtime/runtime.c -o build/bootstrap/runtime.o

if [ $? -ne 0 ]; then
    echo -e "${RED}HATA: runtime.c derlenemedi${NC}"
    exit 1
fi

echo -e "${GREEN}  ✓ runtime.o oluşturuldu${NC}"
echo ""
echo -e "${GREEN}✓ STAGE 3 tamamlandı: C Runtime hazır${NC}"
echo ""

# ===============================================
# STAGE 4: Linking - mlpc Executable Oluştur
# ===============================================

echo -e "${BLUE}[STAGE 4] Object file'ları link edip 'mlpc' oluşturuyoruz...${NC}"
echo ""

echo -e "${YELLOW}  [4.1] GCC Linking...${NC}"

# Tüm object file'ları topla
OBJ_FILES=""
for mlp_file in "${MLP_FILES[@]}"; do
    filename=$(basename "$mlp_file" .mlp)
    OBJ_FILES="$OBJ_FILES build/bootstrap/${filename}.o"
done
OBJ_FILES="$OBJ_FILES build/bootstrap/runtime.o"

# Link et
gcc $OBJ_FILES -o build/mlpc -no-pie

if [ $? -ne 0 ]; then
    echo -e "${RED}HATA: Linking başarısız${NC}"
    exit 1
fi

echo -e "${GREEN}  ✓ build/mlpc executable oluşturuldu${NC}"
echo ""
echo -e "${GREEN}✓ STAGE 4 tamamlandı: mlpc hazır!${NC}"
echo ""

# ===============================================
# STAGE 5: Test - mlpc'yi Dene
# ===============================================

echo -e "${BLUE}[STAGE 5] mlpc test ediliyor...${NC}"
echo ""

# Test dosyası oluştur
echo -e "${YELLOW}  [5.1] Test dosyası oluşturuluyor...${NC}"

cat > build/bootstrap/test.mlp << 'EOF'
-- MLP Bootstrap Test
-- Bu dosya yeni mlpc tarafından derlenecek

IŞLEÇ main(): SAYI {
    YAZDIR "Merhaba, MLP Self-Hosting!";
    YAZDIR "Bootstrap başarılı!";
    DÖNÜŞ 0;
}
EOF

echo -e "${GREEN}  ✓ test.mlp oluşturuldu${NC}"

# mlpc ile derle
echo -e "${YELLOW}  [5.2] mlpc ile test.mlp derleniyor...${NC}"

./build/mlpc build/bootstrap/test.mlp build/bootstrap/test.asm

if [ $? -ne 0 ]; then
    echo -e "${RED}HATA: mlpc ile derleme başarısız${NC}"
    echo -e "${YELLOW}Not: mlpc henüz tam fonksiyonel olmayabilir (normal)${NC}"
    exit 1
fi

echo -e "${GREEN}  ✓ test.asm oluşturuldu${NC}"

# Assembly'i derle
echo -e "${YELLOW}  [5.3] test.asm NASM ile derleniyor...${NC}"
nasm -f elf64 build/bootstrap/test.asm -o build/bootstrap/test.o

# Link et
echo -e "${YELLOW}  [5.4] test executable oluşturuluyor...${NC}"
gcc build/bootstrap/test.o build/bootstrap/runtime.o -o build/bootstrap/test_program -no-pie

# Çalıştır
echo -e "${YELLOW}  [5.5] Test programı çalıştırılıyor...${NC}"
echo -e "${CYAN}  ─────────────────────────────────────${NC}"
./build/bootstrap/test_program
echo -e "${CYAN}  ─────────────────────────────────────${NC}"

echo ""
echo -e "${GREEN}✓ STAGE 5 tamamlandı: Test başarılı!${NC}"
echo ""

# ===============================================
# Özet
# ===============================================

echo ""
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo -e "${GREEN}✓ BOOTSTRAP BAŞARILI!${NC}"
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo ""
echo -e "${YELLOW}Oluşturulan Dosyalar:${NC}"
echo -e "  • build/mlpc                 - MLP Compiler (self-hosting)"
echo -e "  • build/bootstrap/*.asm      - Assembly dosyaları"
echo -e "  • build/bootstrap/*.o        - Object dosyaları"
echo ""
echo -e "${YELLOW}Kullanım:${NC}"
echo -e "  ./build/mlpc kaynak.mlp cikti.asm"
echo ""
echo -e "${YELLOW}Sonraki Adım (Opsiyonel - Stage 2):${NC}"
echo -e "  mlpc'yi kendi kaynak kodunu derlemek için kullan:"
echo -e "  ./build/mlpc src/compiler/main.mlp build/main_v2.asm"
echo ""
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo ""
