Elbette, işte TYD-MLP derleyicisinin tam iş akışı (pipeline) özeti:

### ⛓️ TYD-MLP İş Akışı (Pipeline)

```
[Kullanıcı Kodu (ornekler/test.tyd)]
           |
           v
[Python Motoru (src/compiler.py)]
           |
           +---------------------------------------------+
           |                                             |
           v                                             v
[1. LEXER (src/lexer.py)] <--- (Dil Profili: kurallar/diller.json)
           |
           | (Çıktı: Token Listesi)
           | örn: [TANIMLA_SAYI, s, =, 10, ;]
           v
[2. PARSER (src/parser.py)]
           |
           | (Çıktı: AST - Soyut Sözdizimi Ağacı)
           | örn: (Düğüm: DegiskenTanimlama, Ad: s, Deger: 10)
           v
[3. SEMANTİK ANALİZ (Gelecek Adım)]
           |
           | (Çıktı: Kontrol Edilmiş AST)
           | örn: "Tip uyuşmazlığı" kontrolü yapılır.
           v
[4. CODE GENERATOR (src/generator.py)] <--- (ASM Şablonları: kurallar/komutlar.json)
           |
           | (Çıktı: Assembly Kodu)
           | örn: "mov [s_adresi], 10"
           v
[Nihai Çıktı (build/output.asm)]
           |
           +---------------------------------------------+
           |                                             |
           v                                             v
[5a. ASSEMBLER (NASM)]                           [5b. C YARDIMCI KODU (runtime/runtime.c)]
           |                                             |
           v                                             v
[6. BAĞLAYICI (LINKER - GCC/LD)]
           |
           | (Tüm .o dosyalarını ve C kütüphanelerini birleştirir)
           v
[ÇALIŞTIRILABİLİR PROGRAM (build/tyd_program)]
```