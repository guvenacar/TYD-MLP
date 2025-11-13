python3 -m src.compiler && \
nasm -f elf64 build/output.asm -o build/output.o && \
gcc -no-pie build/output.o runtime/runtime.c -o build/program && \
./build/program