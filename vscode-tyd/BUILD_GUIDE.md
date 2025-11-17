# TYD VSCode Extension - Build & Usage Guide

## 🚀 Özellikler

### ✅ Syntax Highlighting
- TYD anahtar kelimeleri (YAZDIR, EĞER, DÖNGÜ, İŞLEÇ, vb.)
- String literals
- Comments (-- ile başlayan)
- Sayılar ve operatörler

### ✅ IntelliSense
- Otomatik tamamlama
- Hover tooltips
- Diagnostics (hata kontrolü)

### ✅ Build Commands
- **Ctrl+Shift+B** (Cmd+Shift+B Mac'te): Compile Current File
- **F5**: Compile and Run
- Command Palette: "TYD: Build All TYD Files"

## 📦 Kurulum

### 1. Extension'ı Derle

```bash
cd vscode-tyd
npm install
npm run compile
```

### 2. VSIX Paketi Oluştur

```bash
npm install -g vsce
vsce package
```

Bu `tyd-language-0.1.0.vsix` dosyası oluşturacak.

### 3. VSCode'a Kur

**Seçenek A: VSCode içinden**
1. VSCode'u aç
2. Extensions paneline git (Ctrl+Shift+X)
3. "..." menüsünden "Install from VSIX..." seç
4. `tyd-language-0.1.0.vsix` dosyasını seç

**Seçenek B: Komut satırından**
```bash
code --install-extension tyd-language-0.1.0.vsix
```

## 🎯 Kullanım

### Bir TYD Dosyası Aç

```tyd
-- Örnek TYD programı
YAZDIR "Merhaba VSCode!"

SAYISAL x = 42;
YAZDIR x

İŞLEÇ TOPLA(a, b)
    SAYISAL sonuc = a + b;
    DÖNÜŞ sonuc
SON

SAYISAL toplam = TOPLA(10, 20);
YAZDIR toplam
```

### Derleme

1. **Dosyayı Derle**: `Ctrl+Shift+B`
   - Assembly çıktısı: `dosya.asm`

2. **Derle ve Çalıştır**: `F5`
   - Assembly'i binary'ye çevir ve çalıştır (NASM gerekli)

3. **Tüm Dosyaları Derle**: Command Palette → "TYD: Build All"

### Syntax Highlighting Test

```tyd
-- Anahtar kelimeler
YAZDIR EĞER İSE DEĞİLSE DÖNGÜ DÖNGÜ_BITIR İŞLEÇ DÖNÜŞ SON

-- Tipler
SAYISAL METIN BOOL

-- String'ler
METIN mesaj = "Merhaba Dünya!";

-- Sayılar
SAYISAL sayi = 42;

-- Yorumlar
-- Bu bir yorum satırı

-- Operatörler
x = a + b - c * d / e
```

## 🔧 Geliştirme

### Watch Mode

```bash
npm run watch
```

### Debug

1. VSCode'da `vscode-tyd` klasörünü aç
2. F5'e bas (Debug modunda yeni VSCode penceresi açılır)
3. Yeni pencerede bir `.tyd` dosyası aç ve test et

## 📝 Komutlar

| Komut | Kısayol | Açıklama |
|-------|---------|----------|
| TYD: Compile Current File | Ctrl+Shift+B | Aktif TYD dosyasını derle |
| TYD: Compile and Run | F5 | Derle ve çalıştır |
| TYD: Build All TYD Files | - | Tüm TYD dosyalarını derle |

## 🐛 Sorun Giderme

### "compiler_test bulunamadı" hatası
```bash
cd c_compiler
gcc main.c c_lexer.c c_parser.c c_generator.c -o compiler_test -no-pie
gcc -c ../runtime/runtime.c -o ../runtime.o
```

### NASM yok
NASM kurulmamışsa, sadece assembly çıktısı oluşturulur:
```bash
sudo apt-get install nasm  # Linux
brew install nasm          # macOS
```

## 📚 Daha Fazla Bilgi

- [TYD Language Specification](../SPECS.md)
- [TYD Examples](../ornekler/)
- [Self-Hosting Compiler](../tyd_compiler/)
