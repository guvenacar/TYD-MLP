# TYD-MLP: Türkçe Yazılım Dili - Multi-Language Platform

> **"Herkesin Kendi Dilinde Kod Yazabildiği Evrensel Programlama Dili"**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-blue)]()
[![Status](https://img.shields.io/badge/status-Alpha-orange)]()

---

## 🎯 Vizyon

TYD-MLP, programlama dillerindeki en büyük engeli kaldırmayı hedefler: **Syntax bağımlılığı**

### Sorun
- Python geliştiricisi Java syntax'ını öğrenmek zorunda
- VB.NET kullanıcısı C-style syntax'a alışamıyor
- Yeni başlayanlar "hangi syntax" kararında kayboluyorlar

### Çözüm
**TYD-MLP ile herkes kendi bildiği syntax'la kod yazabilir:**

```python
# Python geliştiricisi yazar:
def topla(a, b):
    return a + b
```

```java
// Java geliştiricisi yazar:
int topla(int a, int b) {
    return a + b;
}
```

```vbnet
' VB.NET geliştiricisi yazar:
Function topla(a As Integer, b As Integer) As Integer
    Return a + b
End Function
```

**Hepsi aynı programa derlenir!**

---

## 🏗️ Mimari

```
┌─────────────────────────────────────────┐
│  Kullanıcı Katmanı (Ring 3)             │
│  - Python/Java/VB/Custom Syntax         │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  Syntax Preprocessor                    │
│  - Pattern matching                     │
│  - Syntax normalization                 │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  TYD Core (Base Language)               │
│  - Lexer (Token oluşturma)              │
│  - Parser (AST oluşturma)               │
│  - Generator (Assembly üretme)          │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  C Runtime (Ring 0)                     │
│  - Memory management                    │
│  - String operations                    │
│  - Dictionary/Array operations          │
└─────────────────────────────────────────┘
                ↓
         [Native Binary]
```

---

## ⚡ Hızlı Başlangıç

### Gereksinimler
- Python 3.8+
- GCC (C derleyici)
- Linux/macOS/Windows (WSL)

### Kurulum

```bash
# Repo'yu klonla
git clone https://github.com/guvenacar/TYD-MLP.git
cd TYD-MLP

# Test programını derle
python -m src.compiler ornekler/merhaba.tyd

# Çalıştır
./build/program
```

### İlk Programınız

**1. Native TYD Syntax:**
```tyd
-- ornekler/merhaba.tyd
YAZDIR "Merhaba Dünya!";

SAYISAL a = 10;
SAYISAL b = 20;
YAZDIR a + b;
```

**2. Python-Style Syntax:**
```python
# ornekler/merhaba_python.tyd
print("Merhaba Dünya!")

a = 10
b = 20
print(a + b)
```

```bash
# Python syntax ile derle
python -m src.compiler ornekler/merhaba_python.tyd --syntax python-style
```

**3. Java-Style Syntax:**
```java
// ornekler/merhaba_java.tyd
print("Merhaba Dünya!");

int a = 10;
int b = 20;
print(a + b);
```

```bash
# Java syntax ile derle
python -m src.compiler ornekler/merhaba_java.tyd --syntax java-style
```

---

## 🎨 Desteklenen Syntax Stilleri

### 1. Native TYD (Varsayılan)
```tyd
SAYISAL x = 5;

EĞER x > 3 İSE
    YAZDIR "Büyük";
DEĞILSE
    YAZDIR "Küçük";
EĞER SON
```

### 2. Python-Style
```python
x = 5

if x > 3:
    print("Büyük")
else:
    print("Küçük")
```

### 3. Java/C-Style
```java
int x = 5;

if (x > 3) {
    print("Büyük");
} else {
    print("Küçük");
}
```

### 4. VB.NET-Style
```vbnet
Dim x As Integer = 5

If x > 3 Then
    Print("Büyük")
Else
    Print("Küçük")
End If
```

### 5. Custom (Kendi Syntax'ınızı Tanımlayın!)
`kurallar/syntax_styles.json` dosyasına kendi kurallarınızı ekleyin.

---

## 📚 Özellikler

### ✅ Tamamlanan Özellikler
- [x] Değişken tanımlama (sayısal, metin, zıtlık)
- [x] Temel operatörler (+, -, *, /, **)
- [x] Karşılaştırma operatörleri (==, >, <, >=, <=)
- [x] Koşul yapıları (EĞER/İSE/DEĞILSE)
- [x] Döngüler (DÖNGÜ, DÖNGÜ_BITIR)
- [x] İşleç (fonksiyon) tanımlama
- [x] Parametreli işleçler
- [x] Dönüş değerleri
- [x] Diziler (temel erişim)
- [x] Sözlük (dictionary) desteği
- [x] C Runtime entegrasyonu
- [x] Multi-syntax preprocessing

### 🚧 Geliştirme Aşamasında
- [ ] Foreach döngüsü (HER...İÇİNDE)
- [ ] Gelişmiş dizi operasyonları
- [ ] Nesne yönelimli programlama (NESNE)
- [ ] Hata yakalama (DENE/YAKALA)
- [ ] Modül sistemi (KULLAN/İÇE_AKTAR)
- [x] **VSCode Extension** - ✅ TAMAMLANDI! ([vscode-tyd/](./vscode-tyd/))
- [ ] Language Server Protocol (LSP)

### 🔮 Gelecek Planlar
- [ ] JIT compilation desteği
- [ ] LLVM backend
- [ ] WebAssembly hedefi
- [ ] Paket yöneticisi

---

## 📖 Dokümantasyon

- **[SPECS.md](./SPECS.md)** - Mimari spesifikasyonlar ve tasarım kararları
- **[PIPELINE.md](./PIPELINE.md)** - Adım adım implementasyon rehberi
- **[SOZ_DIZIMI_REFERANS.md](./SOZ_DIZIMI_REFERANS.md)** - TYD söz dizimi kılavuzu
- **[API.md](./docs/API.md)** - C Runtime API referansı
- **[VSCODE_EXTENSION.md](./VSCODE_EXTENSION.md)** - VSCode eklentisi rehberi

---

## 🎨 VSCode Eklentisi

TYD için tam özellikli VSCode eklentisi hazır! 🎉

**Özellikler:**
- ✨ Syntax Highlighting
- 💡 IntelliSense (Kod Tamamlama)
- 📚 Hover Tooltips
- 🔍 Diagnostics (Hata Kontrolü)
- ⚡ Code Snippets
- 🔧 Otomatik Girintileme

**Hızlı Kurulum:**
```bash
cd vscode-tyd
npm install
npm run compile

# F5 ile test et veya kalıcı kur:
ln -s $(pwd) ~/.vscode/extensions/tyd-language-0.1.0
```

Detaylar: [vscode-tyd/QUICK_START.md](./vscode-tyd/QUICK_START.md)

---

## 🛠️ Geliştirme

### Proje Yapısı

```
TYD-MLP/
├── src/                    # TYD derleyici kaynak kodları
│   ├── lexer.py           # Token oluşturucu
│   ├── parser.py          # AST oluşturucu
│   ├── generator.py       # Assembly üretici
│   ├── ast_nodes.py       # AST düğüm tanımları
│   ├── tokens.py          # Token tanımları
│   ├── c_arayuzu.py       # C FFI katmanı
│   ├── compiler.py        # Ana derleyici
│   └── syntax_preprocessor.py  # Syntax dönüştürücü (TODO)
│
├── runtime/               # C Runtime kütüphanesi
│   └── runtime.c         # Bellek, string, sözlük operasyonları
│
├── kurallar/             # Dil kuralları (JSON)
│   ├── diller.json       # Anahtar kelime tanımları
│   └── syntax_styles.json # Syntax stil kuralları (TODO)
│
├── vscode-tyd/           # ✨ VSCode Eklentisi
│   ├── src/              # TypeScript kaynak kodu
│   │   ├── extension.ts          # Ana eklenti
│   │   ├── completionProvider.ts # IntelliSense
│   │   ├── hoverProvider.ts      # Tooltips
│   │   └── diagnostics.ts        # Hata kontrolü
│   ├── syntaxes/         # Syntax highlighting
│   ├── snippets/         # Code snippets
│   └── package.json      # Eklenti manifest
│
├── ornekler/             # Örnek programlar
│   ├── merhaba.tyd
│   ├── test_python.tyd
│   └── test_java.tyd
│
├── build/                # Derleme çıktıları
│   ├── output.asm        # Üretilen Assembly
│   └── program           # Çalıştırılabilir binary
│
└── tests/                # Test dosyaları
    ├── test_lexer.py
    ├── test_parser.py
    └── test_integration.py
```

### Test Çalıştırma

```bash
# Tüm testleri çalıştır
python -m pytest tests/

# Belirli bir testi çalıştır
python -m pytest tests/test_lexer.py -v

# Kapsam raporu
python -m pytest --cov=src tests/
```

### Katkıda Bulunma

1. Fork edin
2. Feature branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Commit edin (`git commit -m 'feat: Add amazing feature'`)
4. Push edin (`git push origin feature/amazing-feature`)
5. Pull Request açın

**Katkı Kuralları:**
- PEP 8 Python style guide'ı takip edin
- Her yeni özellik için test yazın
- Commit mesajları için [Conventional Commits](https://www.conventionalcommits.org/) kullanın

---

## 🎓 Öğrenme Kaynakları

### Yeni Başlayanlar İçin
1. [TYD'ye Giriş](./docs/tutorials/01-giris.md)
2. [İlk Programınız](./docs/tutorials/02-ilk-program.md)
3. [Değişkenler ve Tipler](./docs/tutorials/03-degiskenler.md)
4. [Kontrol Yapıları](./docs/tutorials/04-kontrol-yapilari.md)

### İleri Seviye
1. [Compiler Mimarisi](./docs/advanced/compiler-architecture.md)
2. [Syntax Preprocessor Geliştirme](./docs/advanced/syntax-preprocessor.md)
3. [C Runtime Genişletme](./docs/advanced/c-runtime-extension.md)

---

## 🌟 Örnekler

### Fibonacci Dizisi

**Python-style:**
```python
def fib(n):
    if n <= 1:
        return n
    return fib(n-1) + fib(n-2)

for i in range(10):
    print(fib(i))
```

**Java-style:**
```java
int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n-1) + fib(n-2);
}

for (int i = 0; i < 10; i++) {
    print(fib(i));
}
```

**Native TYD:**
```tyd
SAYISAL IŞLEÇ fib(SAYISAL n)
    EĞER n <= 1 İSE
        DÖNÜŞ n
    EĞER SON
    DÖNÜŞ fib(n-1) + fib(n-2)
IŞLEÇ SON

DÖNGÜ i = 0, i < 10, i = i + 1
    YAZDIR fib(i)
DÖNGÜ SON
```

**Hepsi aynı programa derlenir!**

---

## 🤝 Topluluk

- **Discord:** [TYD-MLP Community](https://discord.gg/tyd-mlp) (Yakında)
- **Forum:** [forum.tyd-lang.org](https://forum.tyd-lang.org) (Yakında)
- **Twitter:** [@TYD_Lang](https://twitter.com/TYD_Lang) (Yakında)

### Sorular?
- 🐛 Bug bildirimi için [Issues](https://github.com/guvenacar/TYD-MLP/issues) açın
- 💡 Özellik önerisi için [Discussions](https://github.com/guvenacar/TYD-MLP/discussions) kullanın
- 📧 Email: [email](mailto:guven.acar@gmail.com)

---

## 📜 Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](./LICENSE) dosyasına bakın.

---

## 🙏 Teşekkürler

TYD-MLP, şu projelerin ilhamını ve teknik yaklaşımlarını kullanır:
- **Python** - Lexer/Parser tasarımı
- **LLVM** - IR generation konsepti
- **Babel** - Syntax transformation yaklaşımı
- **Roslyn** - Multi-syntax parser mimarisi

---

## 📊 Proje İstatistikleri

```
Toplam Satır Sayısı: ~15,000
Python Kodu: ~8,000 satır
C Kodu: ~2,000 satır
Test Kapsamı: %75
Desteklenen Syntax: 4 (Native, Python, Java, VB.NET)
```

---

## 🚀 Yol Haritası

### v0.1.0 (Mevcut - Alpha)
- [x] Temel lexer/parser
- [x] C runtime entegrasyonu
- [x] Temel syntax desteği
- [ ] Syntax preprocessor

### v0.2.0 (Q1 2025)
- [x] **VSCode extension** ✅ TAMAMLANDI
- [ ] 10+ syntax style desteği
- [ ] Gelişmiş hata mesajları
- [ ] Source mapping

### v0.3.0 (Q2 2025)
- [ ] Nesne yönelimli programlama
- [ ] Modül sistemi
- [ ] Paket yöneticisi

### v1.0.0 (Q4 2025)
- [ ] Production-ready
- [ ] Tam dokümantasyon
- [ ] Online playground
- [ ] Topluluk araçları

---

**⭐ Projeyi beğendiyseniz, GitHub'da yıldız vermeyi unutmayın!**

