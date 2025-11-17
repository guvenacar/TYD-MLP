#!/bin/bash
set -euo pipefail

echo "🚀 TYD Self-Hosting Test Pipeline"
echo "===================================="
echo ""

# 1. Runtime library derle
echo "Step 1: Compiling runtime library..."
gcc -c runtime.c -o runtime.o
echo "✅ runtime.o created"
echo ""

# 2. C compiler'ı güncelle
echo "Step 2: Rebuilding C compiler..."
gcc -o compiler_test c_*.c main.c -lm
echo "✅ compiler_test created"
echo ""

# 3. self-host-full.tyd'yi derle
echo "Step 3: Compiling self-host-full.tyd..."
if [ ! -f self-host-full.tyd ]; then
    echo "❌ self-host-full.tyd bulunamadı!"
    exit 1
fi

./compiler_test self-host-full.tyd self-host-full.asm
nasm -f elf64 self-host-full.asm -o self-host-full.o
gcc -no-pie self-host-full.o runtime.o -o self-host-full
echo "✅ self-host-full binary created"
echo ""

# 4. Self-host compiler'ı çalıştır
echo "Step 4: Running self-host compiler..."
mkdir -p build
./self-host-full
echo ""

# 5. Üretilen assembly'yi derle
echo "Step 5: Compiling generated assembly..."
if [ -f build/self-host-output.asm ]; then
    nasm -f elf64 build/self-host-output.asm -o build/self-host-output.o
    gcc -no-pie build/self-host-output.o -o build/hello-from-self-host
    echo "✅ hello-from-self-host binary created"
    echo ""
    
    echo "Step 6: Running generated program..."
    ./build/hello-from-self-host
    echo ""
    
    echo "🎉🎉🎉 SELF-HOSTING SUCCESS! 🎉🎉🎉"
else
    echo "❌ Output assembly not found!"
    exit 1
fi
