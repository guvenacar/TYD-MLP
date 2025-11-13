#!/bin/bash
#
# ===============================================
# Minimal Bootstrap Test - Proof of Concept
# ===============================================
# Tests that the bootstrap pipeline works with a simple stub

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo ""
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo -e "${CYAN}   MINIMAL BOOTSTRAP TEST${NC}"
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo ""

# Create build directory
mkdir -p build

echo -e "${YELLOW}[1/4] Compiling bootstrap stub with Python TYD-MLP...${NC}"
python3 -m src.compiler src/compiler/bootstrap_stub.tyd kurallar/diller.json

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Python compilation failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Assembly generated: build/output.asm${NC}"
echo ""

echo -e "${YELLOW}[2/4] Checking for NASM assembler...${NC}"
if ! command -v nasm &> /dev/null; then
    echo -e "${RED}ERROR: NASM not found${NC}"
    echo -e "${YELLOW}Install with: sudo apt-get install nasm${NC}"
    echo ""
    echo -e "${CYAN}Pipeline verified up to assembly generation!${NC}"
    echo -e "${CYAN}To complete bootstrap, install NASM and run again.${NC}"
    exit 1
fi
echo -e "${GREEN}✓ NASM found: $(nasm -version | head -1)${NC}"
echo ""

echo -e "${YELLOW}[3/4] Assembling with NASM...${NC}"
nasm -f elf64 build/output.asm -o build/bootstrap_stub.o

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: NASM assembly failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Object file created: build/bootstrap_stub.o${NC}"
echo ""

echo -e "${YELLOW}[4/4] Linking with GCC...${NC}"
gcc build/bootstrap_stub.o -o build/mlpc_stub -no-pie

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Linking failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Executable created: build/mlpc_stub${NC}"
echo ""

echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo -e "${GREEN}✓ MINIMAL BOOTSTRAP SUCCESSFUL!${NC}"
echo -e "${CYAN}═══════════════════════════════════════${NC}"
echo ""
echo -e "${YELLOW}Running the stub:${NC}"
echo -e "${CYAN}─────────────────────────────────────${NC}"
./build/mlpc_stub
echo -e "${CYAN}─────────────────────────────────────${NC}"
echo ""
echo -e "${GREEN}Bootstrap pipeline verified:${NC}"
echo -e "  ✓ Python TYD-MLP → Assembly"
echo -e "  ✓ NASM → Object file"
echo -e "  ✓ GCC → Executable"
echo -e "  ✓ Program runs successfully"
echo ""
echo -e "${CYAN}Next Steps:${NC}"
echo -e "  • Extend runtime.c with string functions"
echo -e "  • Implement real lexer/parser/codegen in TYD"
echo -e "  • Full self-hosting compiler"
echo ""
