# TYD VSCode Eklentisi - Kurulum Rehberi

Bu rehber, TYD dili için VSCode eklentisinin kurulumunu ve ilk kullanımını açıklar.

## Hızlı Kurulum

### Adım 1: Bağımlılıkları Yükleyin

```bash
cd /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd
npm install
```

### Adım 2: Eklentiyi Derleyin

```bash
npm run compile
```

### Adım 3: Eklentiyi Test Edin

1. VSCode'da `vscode-tyd` klasörünü açın
2. **F5** tuşuna basın (veya Run > Start Debugging)
3. Yeni bir "Extension Development Host" penceresi açılacak
4. Bu pencerede yeni bir dosya oluşturun: `test.tyd`
5. Aşağıdaki kodu yazın:

```tyd
-- Merhaba Dünya
YAZDIR "Merhaba TYD!";

SAYISAL x = 10;
SAYISAL y = 20;

EĞER x < y İSE
    YAZDIR "x küçük";
DEĞILSE
    YAZDIR "y küçük";
EĞER SON
```

## Özellikleri Test Etme

### 1. Syntax Highlighting
- Anahtar kelimeler (YAZDIR, EĞER, vb.) renklendirilmiş olmalı
- Stringler farklı renkte görünmeli
- Sayılar vurgulanmış olmalı
- Yorumlar soluk renkte olmalı

### 2. IntelliSense
- Yazmaya başlayın: `SAY`
- Otomatik olarak öneri kutusu açılacak
- `SAYISAL` önerisini görmelisiniz
- Enter ile kabul edin

### 3. Code Snippets
- `eger` yazıp **Tab** tuşuna basın
- Otomatik olarak EĞER bloğu oluşmalı:
```tyd
EĞER kosul İSE
    // kod
EĞER SON
```

### 4. Hover Tooltips
- Fare ile `EĞER` kelimesinin üzerine gelin
- Açıklama ve örnek kod içeren tooltip görünmeli

### 5. Diagnostics (Hata Kontrolü)
Aşağıdaki kodu yazın:
```tyd
EĞER x > 5 İSE
    YAZDIR "Test"
```
- Noktalı virgül eksik uyarısı görmelisiniz (sarı çizgi)
- "SON" eksik hatası görmelisiniz (kırmızı çizgi)

## VSCode'a Kalıcı Kurulum

Eklentiyi VSCode'a kalıcı olarak yüklemek için:

### Yöntem 1: VSIX Paketi Oluşturma

1. `vsce` aracını yükleyin:
```bash
npm install -g @vscode/vsce
```

2. VSIX paketi oluşturun:
```bash
cd /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd
vsce package
```

3. Oluşan `.vsix` dosyasını yükleyin:
```bash
code --install-extension tyd-language-0.1.0.vsix
```

### Yöntem 2: Geliştirme Klasöründen Yükleme

1. VSCode'un extension klasörüne sembolik link oluşturun:

**Linux/Mac:**
```bash
ln -s /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd ~/.vscode/extensions/tyd-language-0.1.0
```

**Windows:**
```cmd
mklink /D "%USERPROFILE%\.vscode\extensions\tyd-language-0.1.0" "C:\path\to\TYD-MLP\vscode-tyd"
```

2. VSCode'u yeniden başlatın

## Sorun Giderme

### Eklenti Görünmüyor
1. VSCode'u yeniden başlatın
2. "Developer: Reload Window" komutunu çalıştırın (Ctrl+Shift+P)
3. Extensions panelinde "TYD" arayın

### IntelliSense Çalışmıyor
1. `.tyd` dosyasının dil modunun "TYD" olduğundan emin olun (sağ alt köşe)
2. Dosyayı kapatıp tekrar açın
3. Developer Tools'u açın (Help > Toggle Developer Tools) ve hata mesajı var mı kontrol edin

### Syntax Highlighting Yanlış
1. VSCode'u yeniden başlatın
2. "Developer: Reload Window" yapın
3. Tema ayarlarınızı kontrol edin

### Derleme Hataları
```bash
# Node modules'ı temizle ve yeniden yükle
rm -rf node_modules package-lock.json
npm install

# TypeScript'i tekrar derle
npm run compile
```

## Geliştirme Modu

Eklenti üzerinde değişiklik yaparken:

1. Watch mode'u başlatın:
```bash
npm run watch
```

2. VSCode'da F5 ile Extension Development Host'u açın

3. Kod değişikliği yaptıktan sonra Extension Development Host'ta:
   - Ctrl+Shift+P > "Developer: Reload Window"

4. Değişiklikleri test edin

## Desteklenen Dosya Uzantıları

Eklenti şu dosya uzantılarını otomatik tanır:
- `.tyd`

## Önerilen VSCode Ayarları

`.vscode/settings.json` dosyanıza ekleyebilirsiniz:

```json
{
  "[tyd]": {
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.wordWrap": "on",
    "editor.formatOnSave": false,
    "files.encoding": "utf8"
  }
}
```

## İleri Özellikler

### Özel Tema Renkleri

TYD için özel renk teması ayarlamak isterseniz, `settings.json`'a:

```json
{
  "editor.tokenColorCustomizations": {
    "textMateRules": [
      {
        "scope": "keyword.control.tyd",
        "settings": {
          "foreground": "#C678DD",
          "fontStyle": "bold"
        }
      },
      {
        "scope": "storage.type.tyd",
        "settings": {
          "foreground": "#E5C07B"
        }
      }
    ]
  }
}
```

## Test Senaryoları

### Senaryo 1: Basit Program
```tyd
YAZDIR "Test";
SAYISAL x = 5;
YAZDIR x;
```

**Beklenen:**
- Syntax highlighting çalışıyor
- IntelliSense çalışıyor
- Hata yok

### Senaryo 2: Koşul Yapısı
```tyd
SAYISAL x = 10;

EĞER x > 5 İSE
    YAZDIR "Büyük";
DEĞILSE
    YAZDIR "Küçük";
EĞER SON
```

**Beklenen:**
- Blok indent'leri doğru
- Hover tooltips çalışıyor
- Hata yok

### Senaryo 3: Fonksiyon
```tyd
SAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)
    DÖNÜŞ a + b;
IŞLEÇ SON

SAYISAL sonuc = topla(5, 10);
YAZDIR sonuc;
```

**Beklenen:**
- Fonksiyon renklendirmesi doğru
- IntelliSense parametreleri gösteriyor
- Hata yok

## Yardım ve Destek

- **Dokümantasyon:** [README.md](README.md)
- **Geliştirici Rehberi:** [DEVELOPMENT.md](DEVELOPMENT.md)
- **GitHub:** https://github.com/guvenacar/TYD-MLP
- **Issues:** https://github.com/guvenacar/TYD-MLP/issues

## Sonraki Adımlar

1. [QUICKSTART.md](/QUICKSTART.md) - TYD dili hakkında bilgi edinin
2. [ornekler/](/ornekler/) - Örnek TYD programlarını inceleyin
3. TYD derleyicisini kurun ve programları çalıştırın

---

**Başarılı kurulumlar!** 🎉
