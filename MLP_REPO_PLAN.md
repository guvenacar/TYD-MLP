# MLP - Multi-Language Programming

> **"Write Once, Any Syntax" - Kendi Kendini Derleyen Modern Programlama Dili**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Self-Hosting](https://img.shields.io/badge/self--hosting-yes-green.svg)]()
[![Status](https://img.shields.io/badge/status-Beta-blue)]()

---

## 🎯 MLP Nedir?

**MLP (Multi-Language Programming)**, kendi kendini derleyen (self-hosting), modern bir programlama dilidir.

### Temel Özellikler:

1. **Multi-Syntax**: Aynı kodu farklı syntax'larla yazın
   - Python-style, Java-style, C-style, VB.NET-style
   - Özel syntax'ınızı tanımlayın

2. **Self-Hosting**: MLP compiler'ı MLP dilinde yazılmış
   - Bootstrap başarılı! ✅
   - Python/C++/başka dile bağımlılık YOK

3. **Modern Özellikler**:
   - OOP (Class, Inheritance, Polymorphism, Traits)
   - Generics/Templates
   - Pattern Matching
   - Async/Await
   - Memory Safety
   - Type Inference

4. **Native Performance**:
   - Direct assembly/LLVM backend
   - C seviyesinde performans
   - Sıfır overhead

---

## 🚀 Hızlı Başlangıç

### Kurulum

```bash
# Binary indir
curl -sSL https://mlp-lang.org/install.sh | sh

# Veya source'tan derle
git clone https://github.com/guvenacar/MLP.git
cd MLP
./build.sh
```

### İlk Programınız

**Python-style syntax:**
```mlp
class Kişi:
    def __init__(self, isim: str, yaş: int):
        self.isim = isim
        self.yaş = yaş

    def selamla(self):
        print(f"Merhaba, ben {self.isim}, {self.yaş} yaşındayım")

ahmet = Kişi("Ahmet", 25)
ahmet.selamla()
```

**Java-style syntax (aynı kod!):**
```mlp
class Kişi {
    String isim;
    int yaş;

    Kişi(String isim, int yaş) {
        this.isim = isim;
        this.yaş = yaş;
    }

    void selamla() {
        System.out.println("Merhaba, ben " + this.isim + ", " + this.yaş + " yaşındayım");
    }
}

Kişi ahmet = new Kişi("Ahmet", 25);
ahmet.selamla();
```

**Native MLP syntax:**
```mlp
SINIF Kişi
    METIN isim;
    SAYISAL yaş;

    KURUCU(METIN isim, SAYISAL yaş)
        BU.isim = isim;
        BU.yaş = yaş;
    KURUCU SON

    IŞLEÇ selamla()
        YAZDIR "Merhaba, ben " + BU.isim + ", " + BU.yaş + " yaşındayım";
    IŞLEÇ SON
SINIF SON

Kişi ahmet = YENİ Kişi("Ahmet", 25);
ahmet.selamla();
```

### Derleme ve Çalıştırma

```bash
# Derle
mlp program.mlp -o program

# Çalıştır
./program
```

---

## 📚 Özellikler

### ✅ Tamamlanan

- [x] Self-hosting compiler (MLP ile yazılmış!)
- [x] Multi-syntax preprocessing
- [x] OOP (Class, Inheritance, Polymorphism)
- [x] Generics
- [x] Pattern Matching
- [x] Lexer/Parser/Code Generator
- [x] Assembly backend (x86_64)
- [x] C Runtime integration
- [x] Module system
- [x] Error handling (try/catch)
- [x] String/Array/Dict operations
- [x] VSCode extension

### 🚧 Geliştirme Aşamasında

- [ ] LLVM backend
- [ ] Type inference (Hindley-Milner)
- [ ] Async/await
- [ ] Memory safety (ownership/borrowing)
- [ ] JIT compilation
- [ ] WebAssembly target
- [ ] Package manager
- [ ] Standard library
- [ ] GUI framework (GTK+/Qt bindings)

### 🔮 Gelecek Planlar

- [ ] IDE (MLP ile yazılmış!)
- [ ] Debugger
- [ ] Profiler
- [ ] Language Server Protocol
- [ ] REPL
- [ ] Jupyter notebook integration

---

## 🏗️ Mimari

```
┌────────────────────────────────────────┐
│  User Code (Multi-Syntax)              │
│  Python/Java/C/VB.NET/Native MLP       │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  MLP Compiler (MLP ile yazılmış!)      │
│  --------------------------------       │
│  Lexer → Parser → AST → Optimizer      │
│  → Code Generator                      │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  Backend (Assembly/LLVM/WASM)          │
└────────────────────────────────────────┘
                ↓
┌────────────────────────────────────────┐
│  Native Binary / JIT / WASM            │
└────────────────────────────────────────┘
```

---

## 🎓 Öğrenme Kaynakları

- [MLP Dil Referansı](./docs/LANGUAGE_REFERENCE.md)
- [OOP Rehberi](./docs/OOP_GUIDE.md)
- [Pattern Matching](./docs/PATTERN_MATCHING.md)
- [Async/Await](./docs/ASYNC_AWAIT.md)
- [Compiler İçi Bakış](./docs/COMPILER_INTERNALS.md)
- [Multi-Syntax Nasıl Çalışır](./docs/MULTI_SYNTAX.md)

---

## 🔧 Geliştirme

### Proje Yapısı

```
MLP/
├── src/                    # MLP compiler kaynak kodu (MLP dilinde!)
│   ├── lexer/             # Lexer (MLP ile yazılmış)
│   ├── parser/            # Parser (MLP ile yazılmış)
│   ├── ast/               # AST nodes
│   ├── semantic/          # Semantic analyzer
│   ├── optimizer/         # Code optimizer
│   ├── codegen/           # Code generators
│   │   ├── asm/          # x86_64 assembly
│   │   ├── llvm/         # LLVM IR
│   │   └── wasm/         # WebAssembly
│   ├── runtime/           # Runtime library (C)
│   └── stdlib/            # Standard library (MLP)
│
├── stage0/                # Bootstrap seed compiler (basit MLP)
│   └── simple-compiler.mlp
│
├── tests/                 # Test suite
│   ├── unit/
│   ├── integration/
│   └── benchmark/
│
├── docs/                  # Dokümantasyon
├── examples/              # Örnek programlar
├── vscode-mlp/           # VSCode extension
├── build.sh              # Build script
└── bootstrap.sh          # Bootstrap script
```

### Build

```bash
# Self-compile
./mlp src/compiler/main.mlp -o mlp-new

# Test
./test.sh

# Benchmark
./benchmark.sh
```

---

## 🌟 Örnek Projeler

### 1. OOP Örneği - Şekiller

```mlp
-- Trait tanımla
TRAIT Çizilebilir
    IŞLEÇ çiz()
TRAIT SON

-- Base class
ABSTRACT SINIF Şekil IMPLEMENTS Çizilebilir
    METIN renk;

    KURUCU(METIN renk)
        BU.renk = renk;
    KURUCU SON

    ABSTRACT IŞLEÇ alan() -> SAYISAL
SINIF SON

-- Derived class: Daire
SINIF Daire EXTENDS Şekil
    SAYISAL yarıçap;

    KURUCU(METIN renk, SAYISAL yarıçap)
        SUPER(renk);
        BU.yarıçap = yarıçap;
    KURUCU SON

    OVERRIDE IŞLEÇ alan() -> SAYISAL
        DÖNÜŞ 3.14159 * BU.yarıçap * BU.yarıçap;
    IŞLEÇ SON

    OVERRIDE IŞLEÇ çiz()
        YAZDIR "Çiziliyor: " + BU.renk + " daire, alan: " + BU.alan();
    IŞLEÇ SON
SINIF SON

-- Kullanım
DİZİ<Şekil> şekiller = [
    YENİ Daire("kırmızı", 5),
    YENİ Dikdörtgen("mavi", 10, 20),
];

HER şekil İÇİNDE şekiller
    şekil.çiz();
HER SON
```

### 2. Generics Örneği - Stack

```mlp
GENERIC<T> SINIF Stack
    DİZİ<T> items;

    KURUCU()
        BU.items = [];
    KURUCU SON

    IŞLEÇ push(T item)
        DİZİ_EKLE BU.items item;
    IŞLEÇ SON

    IŞLEÇ pop() -> T?
        EĞER BOŞ_MU BU.items İSE
            DÖNÜŞ HİÇLİK;
        EĞER SON
        DÖNÜŞ DİZİ_SON_ÇIKAR BU.items;
    IŞLEÇ SON

    IŞLEÇ peek() -> T?
        EĞER BOŞ_MU BU.items İSE
            DÖNÜŞ HİÇLİK;
        EĞER SON
        DÖNÜŞ BU.items[UZUNLUK BU.items - 1];
    IŞLEÇ SON
SINIF SON

-- Kullanım
Stack<SAYISAL> sayılar = YENİ Stack<SAYISAL>();
sayılar.push(10);
sayılar.push(20);
sayılar.push(30);

YAZDIR sayılar.pop();  -- 30
YAZDIR sayılar.peek(); -- 20
```

### 3. Pattern Matching Örneği

```mlp
ENUM Sonuç<T, E>
    Başarılı(T),
    Hatalı(E)
ENUM SON

IŞLEÇ dosya_oku(METIN yol) -> Sonuç<METIN, METIN>
    DENE
        METIN içerik = DOSYA_OKU yol;
        DÖNÜŞ Sonuç.Başarılı(içerik);
    YAKALA hata
        DÖNÜŞ Sonuç.Hatalı(hata.mesaj);
    DENE SON
IŞLEÇ SON

-- Pattern matching kullan
Sonuç<METIN, METIN> sonuç = dosya_oku("data.txt");

EŞLEŞTIR sonuç
    DURUM Başarılı(içerik)
        YAZDIR "Dosya okundu: " + içerik;
    DURUM Hatalı(hata)
        YAZDIR "Hata: " + hata;
EŞLEŞTIR SON
```

### 4. Async/Await Örneği

```mlp
ASYNC IŞLEÇ veri_çek(METIN url) -> METIN
    HTTP.İstek istek = HTTP.get(url);
    HTTP.Yanıt yanıt = AWAIT istek.gönder();
    DÖNÜŞ yanıt.içerik;
IŞLEÇ SON

ASYNC IŞLEÇ main()
    METIN veri1 = AWAIT veri_çek("https://api.example.com/data1");
    METIN veri2 = AWAIT veri_çek("https://api.example.com/data2");

    YAZDIR veri1;
    YAZDIR veri2;
IŞLEÇ SON
```

---

## 🤝 Katkıda Bulunma

MLP açık kaynak bir projedir ve katkılara açıktır!

```bash
# Fork et
git clone https://github.com/yourusername/MLP.git

# Feature branch oluştur
git checkout -b feature/amazing-feature

# Commit et
git commit -m 'feat: Add amazing feature'

# Push et
git push origin feature/amazing-feature

# Pull Request aç
```

---

## 📜 Lisans

MIT License - Detaylar için [LICENSE](./LICENSE) dosyasına bakın.

---

## 🎉 Bootstrap Hikayesi

MLP, **TYD-MLP** (Python ile yazılmış seed compiler) kullanılarak bootstrap edilmiştir.

### Bootstrap Süreci:

1. **Stage 0**: TYD-MLP (Python) - Seed compiler
2. **Stage 1**: Basit MLP compiler (MLP dilinde yazıldı, TYD-MLP ile derlendi)
3. **Stage 2**: Tam özellikli MLP compiler (MLP dilinde, Stage 1 ile derlendi)
4. **Stage 3**: Self-compile! (MLP compiler kendini derledi)
5. **Verify**: Binary'ler özdeş! ✅

**Artık Python'a gerek yok! MLP tamamen kendi kendine yeterli!** 🚀

---

## 🌍 Topluluk

- **Discord**: [MLP Community](https://discord.gg/mlp-lang)
- **Forum**: [forum.mlp-lang.org](https://forum.mlp-lang.org)
- **Twitter**: [@MLP_Lang](https://twitter.com/MLP_Lang)

---

**⭐ Projeyi beğendiyseniz, GitHub'da yıldız vermeyi unutmayın!**

**MLP - Multi-Language Programming: Write Once, Any Syntax!** 🚀
