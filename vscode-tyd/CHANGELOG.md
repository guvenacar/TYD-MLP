# Değişiklik Günlüğü

TYD VSCode eklentisi için tüm önemli değişiklikler bu dosyada belgelenecektir.

## [0.1.0] - 2025-01-13

### Eklenen
- ✨ İlk sürüm yayınlandı
- 🎨 Syntax highlighting desteği
  - Anahtar kelimeler için renklendirme
  - String ve sayı vurgulama
  - Yorum satırları desteği
  - Operatör vurgulama

- 💡 IntelliSense özellikleri
  - Akıllı kod tamamlama
  - Anahtar kelime önerileri
  - Snippet destekli tamamlama
  - Detaylı dokümantasyon

- 📚 Hover Tooltips
  - Anahtar kelime açıklamaları
  - Kullanım örnekleri
  - Markdown formatında dokümantasyon

- 🔍 Diagnostics & Linting
  - Gerçek zamanlı sözdizimi kontrolü
  - Blok yapısı doğrulama (EĞER-SON, DÖNGÜ-SON, IŞLEÇ-SON)
  - Noktalı virgül eksikliği uyarısı
  - String kapatma hatası tespiti
  - DÖNÜŞ ifadesinin fonksiyon içinde olma kontrolü

- ✨ Code Snippets
  - Değişken tanımlama snippet'leri (say, metin, zit)
  - Kontrol yapısı snippet'leri (eger, egerdeg, dongu)
  - Fonksiyon tanımlama snippet'i (func)
  - Yorum snippet'leri (com, comblock)

- 🔧 Language Configuration
  - Otomatik parantez eşleştirme
  - Otomatik girintileme
  - Akıllı kod katlama (folding)
  - Yorum satırı toggling (Ctrl+/)

- 📖 Dokümantasyon
  - Kapsamlı README.md
  - Geliştirici kılavuzu (DEVELOPMENT.md)
  - Kurulum rehberi (INSTALLATION.md)

### Desteklenen Anahtar Kelimeler
- Tip tanımları: SAYISAL, METIN, ZITLIK, NESNE
- Kontrol yapıları: EĞER, İSE, DEĞILSE, DÖNGÜ, DÖNGÜ_BITIR, SON
- Fonksiyon: IŞLEÇ, DÖNÜŞ
- I/O: YAZDIR
- Sabitler: DOĞRU, YANLIŞ, HİÇLİK

### Desteklenen Operatörler
- Aritmetik: +, -, *, /, **
- Karşılaştırma: ==, >, <, >=, <=
- Atama: =

### Teknik Detaylar
- VSCode API 1.75+
- TypeScript 4.9+
- Node.js 18+
- UTF-8 encoding desteği
- Türkçe karakter desteği (ğ, ü, ş, ı, ö, ç)

---

## Gelecek Sürümler için Planlar

### [0.2.0] - Planlanan
- [ ] Definition provider (tanıma git)
- [ ] Reference provider (kullanımları bul)
- [ ] Rename provider (yeniden adlandır)
- [ ] Document symbol provider (outline view)
- [ ] Code formatter
- [ ] Bracket colorization
- [ ] Semantic highlighting

### [0.3.0] - Planlanan
- [ ] Debugger integration
- [ ] REPL integration
- [ ] Task runner integration
- [ ] Build system integration
- [ ] Test runner integration

### [1.0.0] - Planlanan
- [ ] Tam LSP (Language Server Protocol) desteği
- [ ] Workspace symbol provider
- [ ] Call hierarchy
- [ ] Type hierarchy
- [ ] Inlay hints
- [ ] Code actions (quick fixes)

---

## Katkıda Bulunanlar

- **Güven Acar** - İlk geliştirme ve tasarım

## Lisans

MIT License - Detaylar için LICENSE dosyasına bakın.
