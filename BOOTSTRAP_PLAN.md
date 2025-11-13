# TYD-Bootstrap: Self-Hosting Compiler Planı

## 🎯 Hedef

TYD-MLP compiler'ını **TYD dilinde yeniden yazarak** Python bağımlılığından kurtulmak.

**Sonuç:** TYD compiler kendini derleyebilecek (self-hosting)!

---

## 📁 Yeni Repo Yapısı: `TYD-Bootstrap`

```
TYD-Bootstrap/
├── README.md                   # Self-hosting açıklaması
├── BOOTSTRAP_GUIDE.md         # Adım adım bootstrap rehberi
│
├── stage0/                    # Faz 0: Python compiler (referans)
│   ├── lexer.py              # Mevcut Python lexer
│   ├── parser.py             # Mevcut Python parser
│   └── generator.py          # Mevcut Python generator
│
├── stage1/                    # Faz 1: TYD ile yazılmış compiler
│   ├── lexer.tyd             # TYD Lexer (TYD dilinde!)
│   ├── parser.tyd            # TYD Parser (TYD dilinde!)
│   ├── generator.tyd         # TYD Generator (TYD dilinde!)
│   ├── ast.tyd               # AST node tanımları
│   ├── tokens.tyd            # Token tanımları
│   └── main.tyd              # Ana compiler entry point
│
├── stage2/                    # Faz 2: Bootstrap binary'leri
│   └── tyd-compiler-v1       # Python ile derlenmiş TYD compiler
│
├── stage3/                    # Faz 3: Self-compiled binary
│   └── tyd-compiler-v2       # Kendini derlemiş TYD compiler
│
├── tests/                     # Test suite
│   ├── test-lexer.tyd
│   ├── test-parser.tyd
│   └── integration/
│
├── runtime/                   # C Runtime (değişmez)
│   └── runtime.c
│
├── bootstrap.sh              # Otomatik bootstrap script
└── verify.sh                 # İki binary'yi karşılaştır
```

---

## 🔧 Teknik Gereksinimler

### 1. TYD Dili Özellikleri (Compiler yazmak için)

Compiler yazmak için TYD'de şunlar lazım:

- [x] **Veri yapıları**: Diziler, sözlükler (AST için)
- [x] **String işlemleri**: Tokenization için
- [x] **Fonksiyonlar**: Modüler kod
- [ ] **Dosya I/O**: Kaynak kodu okuma
- [ ] **Pattern matching**: Token/AST matching (veya if-else)
- [ ] **Struct/Class**: AST node'ları için

### 2. Eksik Özellikler (Eklenmeli)

```tyd
-- DOSYA I/O (önemli!)
METIN dosya_oku(METIN yol)
    -- C runtime'da implement edilmeli
IŞLEÇ SON

-- STRING İŞLEMLERİ
METIN parcala(METIN str, METIN ayrac)
METIN birlestir(DİZİ parcalar, METIN ayrac)
SAYISAL bul(METIN str, METIN aranan)

-- HATA YÖNETIMI
DENE
    -- Riskli kod
YAKALA hata
    YAZDIR hata;
DENE SON
```

---

## 📋 Bootstrap Adımları

### Adım 1: Python Compiler'ı Tamamla (TYD-MLP repo)

Şu anki Python compiler tam çalışmalı:

```bash
cd TYD-MLP
python -m src.compiler ornekler/05-fonksiyonlar.tyd
./build/program
# ✅ Çalışıyor
```

### Adım 2: Yeni Repo Oluştur

```bash
# GitHub'da yeni repo: TYD-Bootstrap
git clone https://github.com/guvenacar/TYD-Bootstrap.git
cd TYD-Bootstrap

# stage0: Python compiler'ı kopyala
cp -r ../TYD-MLP/src stage0/
cp -r ../TYD-MLP/runtime .
```

### Adım 3: Lexer'ı TYD'de Yaz (stage1/lexer.tyd)

```tyd
-- stage1/lexer.tyd
KULLAN RUNTIME;  -- C runtime fonksiyonları

-- Token tipi
NESNE Token
    METIN tip;
    METIN deger;
    SAYISAL satir;
NESNE SON

-- Tokenize fonksiyonu
DİZİ IŞLEÇ tokenize(METIN kaynak_kod)
    DİZİ tokens = [];
    SAYISAL pos = 0;
    SAYISAL satir = 1;

    DÖNGÜ pos < UZUNLUK kaynak_kod
        METIN karakter = kaynak_kod[pos];

        -- Boşlukları atla
        EĞER karakter == " " VEYA karakter == "\t" İSE
            pos = pos + 1;
            DÖNGÜ_DEVAM;
        EĞER SON

        -- Sayılar
        EĞER RAKAM_MI karakter İSE
            NESNE token = sayi_tokenize kaynak_kod pos;
            DİZİ_EKLE tokens token;
            pos = token.yeni_pos;
            DÖNGÜ_DEVAM;
        EĞER SON

        -- Anahtar kelimeler ve tanımlayıcılar
        EĞER HARF_MI karakter İSE
            NESNE token = kelime_tokenize kaynak_kod pos;
            DİZİ_EKLE tokens token;
            pos = token.yeni_pos;
            DÖNGÜ_DEVAM;
        EĞER SON

        -- Operatörler
        -- ... vs

        pos = pos + 1;
    DÖNGÜ SON

    DÖNÜŞ tokens;
IŞLEÇ SON

-- Yardımcı fonksiyonlar
NESNE IŞLEÇ sayi_tokenize(METIN kod, SAYISAL baslangic)
    METIN sayi = "";
    SAYISAL pos = baslangic;

    DÖNGÜ pos < UZUNLUK kod VE RAKAM_MI kod[pos]
        sayi = sayi + kod[pos];
        pos = pos + 1;
    DÖNGÜ SON

    NESNE token;
    token.tip = "SAYI";
    token.deger = sayi;
    token.yeni_pos = pos;

    DÖNÜŞ token;
IŞLEÇ SON
```

### Adım 4: Parser'ı TYD'de Yaz (stage1/parser.tyd)

```tyd
-- stage1/parser.tyd

NESNE ASTNode
    METIN tip;
    METIN deger;
    DİZİ cocuklar;
NESNE SON

NESNE IŞLEÇ parse(DİZİ tokens)
    SAYISAL current = 0;
    NESNE ast = program_parse tokens current;
    DÖNÜŞ ast;
IŞLEÇ SON

NESNE IŞLEÇ program_parse(DİZİ tokens, SAYISAL pos)
    NESNE program;
    program.tip = "PROGRAM";
    program.cocuklar = [];

    DÖNGÜ pos < UZUNLUK tokens
        NESNE ifade = ifade_parse tokens pos;
        DİZİ_EKLE program.cocuklar ifade;
        pos = ifade.yeni_pos;
    DÖNGÜ SON

    DÖNÜŞ program;
IŞLEÇ SON

-- Expression parsing
NESNE IŞLEÇ ifade_parse(DİZİ tokens, SAYISAL pos)
    NESNE token = tokens[pos];

    -- SAYISAL keyword
    EĞER token.tip == "ANAHTAR_KELIME" VE token.deger == "SAYISAL" İSE
        DÖNÜŞ degisken_tanim_parse tokens pos;
    EĞER SON

    -- YAZDIR keyword
    EĞER token.tip == "ANAHTAR_KELIME" VE token.deger == "YAZDIR" İSE
        DÖNÜŞ yazdir_parse tokens pos;
    EĞER SON

    -- ... vs
IŞLEÇ SON
```

### Adım 5: Generator'ı TYD'de Yaz (stage1/generator.tyd)

```tyd
-- stage1/generator.tyd

METIN IŞLEÇ generate(NESNE ast)
    METIN kod = "";

    -- Assembly başlangıç
    kod = kod + "section .data\n";
    kod = kod + generate_data ast;
    kod = kod + "\nsection .text\n";
    kod = kod + "global _start\n_start:\n";

    -- AST traverse
    DİZİ ifadeler = ast.cocuklar;
    HER ifade İÇİNDE ifadeler
        kod = kod + generate_ifade ifade;
    HER SON

    -- Exit syscall
    kod = kod + "    mov rax, 60\n";
    kod = kod + "    xor rdi, rdi\n";
    kod = kod + "    syscall\n";

    DÖNÜŞ kod;
IŞLEÇ SON

METIN IŞLEÇ generate_ifade(NESNE node)
    EĞER node.tip == "YAZDIR" İSE
        DÖNÜŞ generate_yazdir node;
    DEĞILSE EĞER node.tip == "DEGİSKEN_TANIM" İSE
        DÖNÜŞ generate_degisken node;
    EĞER SON

    DÖNÜŞ "";
IŞLEÇ SON
```

### Adım 6: Ana Compiler (stage1/main.tyd)

```tyd
-- stage1/main.tyd
KULLAN LEXER;
KULLAN PARSER;
KULLAN GENERATOR;
KULLAN DOSYA;

IŞLEÇ main(DİZİ arguman)
    -- Komut satırı argümanları
    EĞER UZUNLUK arguman < 2 İSE
        YAZDIR "Kullanım: tyd-compiler <dosya.tyd>";
        DÖNÜŞ 1;
    EĞER SON

    METIN dosya_yolu = arguman[1];

    -- Dosyayı oku
    METIN kaynak_kod = DOSYA_OKU dosya_yolu;

    -- Lex
    YAZDIR "Tokenizing...";
    DİZİ tokens = tokenize kaynak_kod;

    -- Parse
    YAZDIR "Parsing...";
    NESNE ast = parse tokens;

    -- Generate
    YAZDIR "Generating assembly...";
    METIN assembly = generate ast;

    -- Dosyaya yaz
    DOSYA_YAZ "output.asm" assembly;

    YAZDIR "Derleme başarılı! output.asm oluşturuldu.";
    DÖNÜŞ 0;
IŞLEÇ SON
```

### Adım 7: Bootstrap Script (bootstrap.sh)

```bash
#!/bin/bash
# bootstrap.sh - TYD compiler'ı bootstrap et

set -e

echo "=== TYD Bootstrap Süreci ==="

# Stage 0: Python compiler var mı kontrol et
echo "[Stage 0] Python compiler kontrol ediliyor..."
if [ ! -f "stage0/compiler.py" ]; then
    echo "HATA: stage0/compiler.py bulunamadı!"
    exit 1
fi

# Stage 1: TYD compiler kaynak kodu
echo "[Stage 1] TYD compiler kaynak kodu okunuyor..."
if [ ! -f "stage1/main.tyd" ]; then
    echo "HATA: stage1/main.tyd bulunamadı!"
    exit 1
fi

# Stage 2: Python ile TYD compiler'ı derle
echo "[Stage 2] TYD compiler Python ile derleniyor..."
python3 stage0/compiler.py stage1/main.tyd -o stage2/tyd-compiler-v1

echo "[Stage 2] v1 derlendi: stage2/tyd-compiler-v1"

# Test v1
echo "[Stage 2] v1 test ediliyor..."
./stage2/tyd-compiler-v1 tests/test-simple.tyd
echo "✅ v1 çalışıyor!"

# Stage 3: v1 ile kendini derle
echo "[Stage 3] TYD compiler kendini derliyor..."
./stage2/tyd-compiler-v1 stage1/main.tyd -o stage3/tyd-compiler-v2

echo "[Stage 3] v2 derlendi: stage3/tyd-compiler-v2"

# Verify: v1 ve v2 aynı mı?
echo "[Verify] v1 ve v2 karşılaştırılıyor..."
if cmp -s stage2/tyd-compiler-v1 stage3/tyd-compiler-v2; then
    echo "✅ BOOTSTRAP BAŞARILI!"
    echo "v1 ve v2 binary'leri özdeş."
    echo "Artık Python'a gerek yok!"
else
    echo "⚠️ UYARI: v1 ve v2 farklı!"
    echo "Compiler'da hata olabilir."
    diff <(xxd stage2/tyd-compiler-v1) <(xxd stage3/tyd-compiler-v2) | head -20
fi

# Stage 4 (opsiyonel): v2 ile v3 derle (double check)
echo "[Stage 4] Double check: v2 ile v3 derleniyor..."
./stage3/tyd-compiler-v2 stage1/main.tyd -o stage3/tyd-compiler-v3

if cmp -s stage3/tyd-compiler-v2 stage3/tyd-compiler-v3; then
    echo "✅ DOUBLE CHECK BAŞARILI!"
    echo "Compiler tamamen stable."
else
    echo "❌ Double check başarısız!"
    exit 1
fi

echo ""
echo "🎉 TYD COMPILER ARTIK SELF-HOSTING!"
echo "Final binary: stage3/tyd-compiler-v2"
echo ""
echo "Kullanım:"
echo "  ./stage3/tyd-compiler-v2 program.tyd"
```

---

## 🧪 Test Stratejisi

### 1. Unit Tests (Her modül için)

```tyd
-- tests/test-lexer.tyd
KULLAN LEXER;
KULLAN ASSERT;

IŞLEÇ test_sayi_tokenize()
    DİZİ tokens = tokenize "123";
    ASSERT_ESIT tokens[0].tip "SAYI";
    ASSERT_ESIT tokens[0].deger "123";
    YAZDIR "✅ test_sayi_tokenize";
IŞLEÇ SON

IŞLEÇ test_anahtar_kelime()
    DİZİ tokens = tokenize "SAYISAL";
    ASSERT_ESIT tokens[0].tip "ANAHTAR_KELIME";
    ASSERT_ESIT tokens[0].deger "SAYISAL";
    YAZDIR "✅ test_anahtar_kelime";
IŞLEÇ SON

-- Tüm testleri çalıştır
test_sayi_tokenize;
test_anahtar_kelime;
```

### 2. Integration Tests

```bash
# Test 1: Basit program
./tyd-compiler tests/hello.tyd
./build/program
# Beklenen: "Merhaba Dünya!"

# Test 2: Fonksiyonlar
./tyd-compiler tests/fibonacci.tyd
./build/program
# Beklenen: Fibonacci sayıları

# Test 3: Self-compile
./tyd-compiler stage1/main.tyd -o test-compiler
cmp tyd-compiler test-compiler
# Beklenen: İkisi özdeş
```

---

## 📊 Milestone Timeline

### Milestone 1: Hazırlık (1-2 hafta)
- [x] TYD-MLP compiler tamamlansın (Python)
- [ ] Eksik TYD özellikleri (dosya I/O, string ops)
- [ ] Yeni repo oluştur

### Milestone 2: Lexer (1 hafta)
- [ ] `stage1/lexer.tyd` yaz
- [ ] Python compiler ile derle ve test et
- [ ] Unit tests

### Milestone 3: Parser (2 hafta)
- [ ] `stage1/parser.tyd` yaz
- [ ] AST node tanımları
- [ ] Integration tests

### Milestone 4: Generator (1 hafta)
- [ ] `stage1/generator.tyd` yaz
- [ ] Assembly üretimi
- [ ] Test programları derle

### Milestone 5: Bootstrap (3 gün)
- [ ] `stage1/main.tyd` tamamla
- [ ] `bootstrap.sh` çalıştır
- [ ] v1 == v2 verify et

### Milestone 6: Stabilize (1 hafta)
- [ ] Bug fixes
- [ ] Performance tuning
- [ ] Comprehensive tests
- [ ] Documentation

---

## 🎓 Ünlü Self-Hosting Compiler'lar

### C Compiler (GCC)
- C ile yazılmış
- İlk C compiler assembly'deydi, sonra bootstrapped

### Rust Compiler (rustc)
- Rust ile yazılmış
- Başta OCaml'deydi, 2011'de Rust'a geçti

### Go Compiler
- Go ile yazılmış
- Başta C'deydi, Go 1.5'te bootstrapped

### PyPy (Python Interpreter)
- Python ile yazılmış Python!
- CPython'dan daha hızlı

---

## 🚀 Başarı Kriterleri

Bootstrap **başarılı** sayılır eğer:

1. ✅ `stage2/tyd-compiler-v1` (Python ile derlenmiş) çalışıyor
2. ✅ `stage3/tyd-compiler-v2` (v1 ile derlenmiş) çalışıyor
3. ✅ v1 ve v2 binary'leri **özdeş** (cmp -s başarılı)
4. ✅ v2 ile v3 derleniyor ve v2 == v3 (double check)
5. ✅ Tüm test suite'i geçiyor
6. ✅ Artık Python'a **hiç** ihtiyaç yok!

---

## 📚 Kaynaklar

- **Bootstrapping**: https://en.wikipedia.org/wiki/Bootstrapping_(compilers)
- **Writing a C Compiler**: https://norasandler.com/2017/11/29/Write-a-Compiler.html
- **Rust Bootstrap**: https://doc.rust-lang.org/nightly/rustc-dev-guide/building/bootstrapping.html
- **Self-Compiling Compilers**: https://cs.stackexchange.com/questions/1034/self-compiling-compilers

---

**Bu TYD-MLP için en büyük milestone olacak! 🎉**

**"TYD ile yazılmış TYD compiler" = Gerçek bir programlama dili!**
