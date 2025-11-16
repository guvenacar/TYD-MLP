# 1. C'yi Derle (DOSYA ADI DÜZELTİLDİ)
gcc -c -fPIC -no-pie runtime/runtime.c -o build/runtime.o && \

# 2. Sembolleri Kontrol Et (Değişiklik yok)
nm build/runtime.o | grep sozluk_ && \

# 3. Python Derleyicisini Çalıştır (GÜNCELLENDİ)
python3 -m src.compiler ornekler/test.tyd && \

# 4. Assembly'i Derle (Değişiklik yok)
nasm -f elf64 build/output.asm -o build/output.o && \

# 5. Bağla (Link) ve Çalıştır (Değişiklik yok)
gcc -no-pie build/output.o build/runtime.o -o build/program && \
./build/program