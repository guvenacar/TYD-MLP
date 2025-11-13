# TYD Language Support for VSCode

TYD (Türkçe Yazılım Dili) için kapsamlı VSCode eklentisi. Sözdizimi vurgulama, IntelliSense, hata kontrolü ve daha fazlası!

## Özellikler

### 🎨 Syntax Highlighting
- Anahtar kelimeler için renklendirme
- String ve sayı değerleri için özel vurgulama
- Yorum satırları için desteklenmiş görünüm
- Operatörlerin belirgin gösterimi

### 💡 IntelliSense
- Akıllı kod tamamlama
- Anahtar kelime önerileri
- Operatör ve sabit önerileri
- Kod snippet'leri ile hızlı yazım

### 📚 Hover Tooltips
- Anahtar kelimelerin üzerine gelince açıklama
- Kullanım örnekleri
- Operatör dokümantasyonu

### 🔍 Diagnostics & Linting
- Gerçek zamanlı sözdizimi kontrolü
- Blok yapısı doğrulama (EĞER-SON, DÖNGÜ-SON, vb.)
- Noktalı virgül eksikliği uyarısı
- String kapatma hatası tespiti
- Anahtar kelime kullanım kontrolü

### ✨ Code Snippets
- Hızlı değişken tanımlama
- Koşul yapıları şablonları
- Döngü şablonları
- Fonksiyon tanımlama şablonları

## Kurulum

### VSCode Marketplace'den
1. VSCode'u açın
2. Extensions paneline gidin (Ctrl+Shift+X)
3. "TYD Language" arayın
4. Install butonuna tıklayın

### Manuel Kurulum
1. Bu deposunu klonlayın veya indirin
2. Eklenti klasörüne gidin:
   ```bash
   cd vscode-tyd
   ```
3. Bağımlılıkları yükleyin:
   ```bash
   npm install
   ```
4. Eklentiyi derleyin:
   ```bash
   npm run compile
   ```
5. VSCode'da F5'e basarak test edin

## Kullanım

### Yeni TYD Dosyası Oluşturma
1. `.tyd` uzantılı bir dosya oluşturun
2. Otomatik olarak TYD dil desteği aktif olacaktır

### IntelliSense Kullanımı
Yazmaya başladığınızda otomatik olarak öneriler görünecektir:
- `CTRL+Space` ile manuel olarak IntelliSense'i tetikleyebilirsiniz
- Ok tuşları ile önerilerde gezinin
- Enter veya Tab ile kabul edin

### Code Snippets
Aşağıdaki kısayolları yazıp Tab'a basın:

| Kısayol | Açıklama |
|---------|----------|
| `say` | Sayısal değişken |
| `metin` | Metin değişkeni |
| `zit` | Boolean değişken |
| `yaz` | YAZDIR komutu |
| `eger` | EĞER bloğu |
| `egerdeg` | EĞER-DEĞILSE bloğu |
| `dongu` | DÖNGÜ bloğu |
| `func` | Fonksiyon tanımlama |
| `hello` | Merhaba Dünya |

### Örnek Kullanım

```tyd
-- Basit bir program
SAYISAL x = 10;
SAYISAL y = 20;

EĞER x > y İSE
    YAZDIR "x büyük";
DEĞILSE
    YAZDIR "y büyük";
EĞER SON

-- Fonksiyon tanımlama
SAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)
    DÖNÜŞ a + b;
IŞLEÇ SON

SAYISAL sonuc = topla(x, y);
YAZDIR sonuc;
```

## Desteklenen Özellikler

### Anahtar Kelimeler
- **Tipler:** SAYISAL, METIN, ZITLIK, NESNE
- **Kontrol:** EĞER, İSE, DEĞILSE, DÖNGÜ, DÖNGÜ_BITIR, SON
- **Fonksiyon:** IŞLEÇ, DÖNÜŞ
- **I/O:** YAZDIR
- **Sabitler:** DOĞRU, YANLIŞ, HİÇLİK

### Operatörler
- **Aritmetik:** +, -, *, /, ** (üs alma)
- **Karşılaştırma:** ==, >, <, >=, <=
- **Atama:** =

### Yorum Satırları
- Tek satır: `--`
- Çok satır: `--- ... ---`

## Katkıda Bulunma

Projeye katkıda bulunmak için:

1. Depoyu fork edin
2. Yeni bir branch oluşturun (`git checkout -b feature/yeni-ozellik`)
3. Değişikliklerinizi commit edin (`git commit -am 'Yeni özellik ekle'`)
4. Branch'inizi push edin (`git push origin feature/yeni-ozellik`)
5. Pull Request oluşturun

## Sorun Bildirimi

Bir hata veya sorun bulduysanız, lütfen [GitHub Issues](https://github.com/guvenacar/TYD-MLP/issues) üzerinden bildirin.

## Lisans

Bu proje MIT lisansı altında lisanslanmıştır.

## İletişim

- GitHub: [TYD-MLP](https://github.com/guvenacar/TYD-MLP)
- Email: guven.acar@gmail.com

## Değişiklik Günlüğü

### v0.1.0 (İlk Sürüm)
- ✅ Syntax highlighting desteği
- ✅ IntelliSense ve kod tamamlama
- ✅ Hover tooltips
- ✅ Diagnostics ve linting
- ✅ Code snippets
- ✅ Otomatik parantez eşleştirme
- ✅ Otomatik girintileme

---

**TYD-MLP ile mutlu kodlamalar!** 🚀
