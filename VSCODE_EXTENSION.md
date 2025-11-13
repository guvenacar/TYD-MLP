# TYD VSCode Eklentisi

TYD-MLP için tam özellikli VSCode eklentisi geliştirildi! 🎉

## Özellikler

### ✅ Tamamlanan Özellikler

1. **🎨 Syntax Highlighting**
   - Tüm TYD anahtar kelimeleri için renklendirme
   - String, sayı ve yorum vurgulama
   - Türkçe karakter desteği

2. **💡 IntelliSense (Kod Tamamlama)**
   - Akıllı anahtar kelime önerileri
   - Detaylı açıklamalar
   - Kullanım örnekleri

3. **📚 Hover Tooltips**
   - Anahtar kelime dokümantasyonu
   - Örnek kod gösterimi
   - Markdown formatında açıklamalar

4. **🔍 Diagnostics (Hata Kontrolü)**
   - Gerçek zamanlı sözdizimi kontrolü
   - Blok yapısı doğrulama
   - Noktalı virgül kontrolü
   - String kapatma kontrolü

5. **✨ Code Snippets**
   - Hızlı değişken tanımlama
   - Koşul ve döngü şablonları
   - Fonksiyon şablonları

6. **🔧 Dil Yapılandırması**
   - Otomatik parantez eşleştirme
   - Akıllı girintileme
   - Kod katlama (folding)
   - Yorum satırı toggling

## Kurulum

### Hızlı Başlangıç

```bash
cd vscode-tyd
npm install
npm run compile
```

### Test Etme

1. VSCode'da `vscode-tyd` klasörünü açın
2. **F5** tuşuna basın
3. Extension Development Host'ta `.tyd` dosyası açın
4. Özellikleri test edin

Detaylı kurulum için: [vscode-tyd/INSTALLATION.md](vscode-tyd/INSTALLATION.md)

## Dosya Yapısı

```
vscode-tyd/
├── src/
│   ├── extension.ts           # Ana eklenti
│   ├── completionProvider.ts  # IntelliSense
│   ├── hoverProvider.ts       # Tooltips
│   └── diagnostics.ts         # Hata kontrolü
├── syntaxes/
│   └── tyd.tmLanguage.json   # Syntax highlighting
├── snippets/
│   └── tyd.json              # Code snippets
├── language-configuration.json
├── package.json
└── README.md
```

## Kullanım Örnekleri

### IntelliSense Kullanımı

Yazmaya başlayın ve otomatik öneriler görün:

```tyd
SAY[Ctrl+Space]  →  SAYISAL
```

### Snippet Kullanımı

Kısayol yazıp Tab'a basın:

```
eger[Tab]  →  EĞER bloğu oluşturulur
func[Tab]  →  Fonksiyon şablonu oluşturulur
```

### Hover Dokümantasyon

Fareyi anahtar kelimenin üzerine getirin:

```tyd
EĞER  ←  (Fareyi buraya getirin: açıklama ve örnek görün)
```

### Hata Kontrolü

Gerçek zamanlı hata kontrolü:

```tyd
EĞER x > 5 İSE
    YAZDIR "test"  ← Noktalı virgül eksik uyarısı
← "EĞER SON" eksik hatası
```

## Desteklenen Özellikler

### Anahtar Kelimeler
- **Tipler:** SAYISAL, METIN, ZITLIK, NESNE
- **Kontrol:** EĞER, İSE, DEĞILSE, DÖNGÜ, DÖNGÜ_BITIR, SON
- **Fonksiyon:** IŞLEÇ, DÖNÜŞ
- **I/O:** YAZDIR
- **Sabitler:** DOĞRU, YANLIŞ, HİÇLİK

### Operatörler
- **Aritmetik:** +, -, *, /, **
- **Karşılaştırma:** ==, >, <, >=, <=
- **Atama:** =

### Yorum Satırları
- Tek satır: `--`
- Çok satır: `--- ... ---`

## Snippet Listesi

| Kısayol | Açıklama |
|---------|----------|
| `say` | SAYISAL değişken |
| `metin` | METIN değişken |
| `zit` | ZITLIK değişken |
| `yaz` | YAZDIR komutu |
| `eger` | EĞER bloğu |
| `egerdeg` | EĞER-DEĞILSE bloğu |
| `dongu` | DÖNGÜ bloğu |
| `func` | Fonksiyon tanımlama |
| `hello` | Merhaba Dünya |
| `com` | Tek satır yorum |
| `comblock` | Çok satırlı yorum |

## Test Dosyası

Demo dosyası: [vscode-tyd/test-examples/demo.tyd](vscode-tyd/test-examples/demo.tyd)

Bu dosya tüm eklenti özelliklerini gösterir.

## Dokümantasyon

- **README:** [vscode-tyd/README.md](vscode-tyd/README.md)
- **Kurulum:** [vscode-tyd/INSTALLATION.md](vscode-tyd/INSTALLATION.md)
- **Geliştirme:** [vscode-tyd/DEVELOPMENT.md](vscode-tyd/DEVELOPMENT.md)
- **Değişiklikler:** [vscode-tyd/CHANGELOG.md](vscode-tyd/CHANGELOG.md)

## Gelecek Özellikler

### Planlanan (v0.2.0)
- [ ] Definition provider (tanıma git - F12)
- [ ] Reference provider (kullanımları bul)
- [ ] Rename provider (yeniden adlandır - F2)
- [ ] Document symbols (outline view)
- [ ] Code formatter

### İleri Özellikler (v0.3.0+)
- [ ] Debugger integration
- [ ] REPL integration
- [ ] Build task integration
- [ ] Full LSP support

## Katkıda Bulunma

Katkılarınızı bekliyoruz!

1. Fork edin
2. Feature branch oluşturun
3. Değişikliklerinizi commit edin
4. Pull request gönderin

## Lisans

MIT License

## İletişim

- GitHub: https://github.com/guvenacar/TYD-MLP
- Email: guven.acar@gmail.com

---

**TYD ile mutlu kodlamalar!** 🚀
