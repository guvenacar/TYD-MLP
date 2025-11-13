# TYD VSCode Eklentisi - Hızlı Başlangıç

## ✅ Kurulum Tamamlandı!

Eklenti başarıyla derlendi. Artık test edebilirsiniz!

## 🚀 Test Etme (Geliştirme Modu)

### Yöntem 1: VSCode'dan F5 ile Test

1. VSCode'da bu klasörü açın:
   ```bash
   code /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd
   ```

2. **F5** tuşuna basın (veya Run > Start Debugging)

3. Yeni bir "Extension Development Host" penceresi açılacak

4. Bu pencerede yeni bir dosya oluşturun: `test.tyd`

5. Aşağıdaki kodu yazın ve özellikleri test edin:

```tyd
-- Test dosyası
YAZDIR "Merhaba TYD!";

SAYISAL x = 10;
SAYISAL y = 20;

EĞER x < y İSE
    YAZDIR "x küçük";
DEĞILSE
    YAZDIR "y küçük";
EĞER SON
```

### Yöntem 2: Manuel Sembolik Link ile Kurulum

Node.js 20 gereksinimi olmadan kalıcı kurulum:

```bash
# Eklentiyi VSCode'un extensions dizinine linkle
ln -s /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd ~/.vscode/extensions/tyd-language-0.1.0

# VSCode'u yeniden başlat
code --reuse-window
```

Artık tüm VSCode pencerelerinde `.tyd` dosyaları için eklenti aktif olacak!

## 🎯 Özellikleri Test Etme

### 1. Syntax Highlighting ✨
Anahtar kelimeler otomatik renklendirilmeli:
- `EĞER`, `DÖNGÜ`, `IŞLEÇ` → Kontrol yapıları rengi
- `SAYISAL`, `METIN`, `ZITLIK` → Tip rengi
- `"Merhaba"` → String rengi
- `123`, `3.14` → Sayı rengi
- `-- yorum` → Yorum rengi

### 2. IntelliSense 💡

Yazmaya başlayın:
```
SAY[Ctrl+Space veya otomatik]
```

Öneri listesi açılmalı:
- SAYISAL
- Açıklama: "Sayısal tip (integer/float)"
- Örnek kod

### 3. Code Snippets ⚡

Şu kısayolları deneyin (yazıp Tab'a basın):

| Kısayol | Sonuç |
|---------|-------|
| `say` + Tab | `SAYISAL değişken = 0;` |
| `eger` + Tab | EĞER bloğu oluşturulur |
| `func` + Tab | Fonksiyon şablonu |
| `dongu` + Tab | DÖNGÜ şablonu |

### 4. Hover Tooltips 📚

Fareyi şu kelimelerin üzerine getirin:
- `EĞER` → Açıklama + örnek görünmeli
- `DÖNGÜ` → Dokümantasyon
- `IŞLEÇ` → Fonksiyon açıklaması

### 5. Diagnostics (Hata Kontrolü) 🔍

Şu hatalı kodu yazın:
```tyd
EĞER x > 5 İSE
    YAZDIR "test"
```

Görmeniz gerekenler:
- ⚠️ Sarı dalgalı çizgi: "Noktalı virgül eksik"
- 🔴 Kırmızı dalgalı çizgi: "EĞER SON eksik"

### 6. Otomatik Özellikler 🔧

- **Parantez eşleştirme:** `(` yazınca otomatik `)` eklenir
- **Girintileme:** EĞER yazınca alt satır otomatik girintilenir
- **Yorum toggling:** Ctrl+/ ile satırı yoruma çevirin
- **Kod katlama:** EĞER bloklarını katla/aç

## 📝 Demo Dosyası

Hazır test dosyası:
```bash
code test-examples/demo.tyd
```

Bu dosya tüm TYD özelliklerini gösterir.

## 🔧 Geliştirme Modu

Eklenti üzerinde değişiklik yapıyorsanız:

```bash
# Watch mode - otomatik derleme
npm run watch

# Başka bir terminalde VSCode'u başlat
code .

# F5 ile test et
# Kod değiştikçe Extension Development Host'ta
# Ctrl+R ile yeniden yükle
```

## 🐛 Sorun Giderme

### Eklenti Çalışmıyor
1. VSCode'u yeniden başlatın
2. Dosya uzantısının `.tyd` olduğundan emin olun
3. Sağ alt köşede "TYD" dil modunu görüyor musunuz?

### IntelliSense Çalışmıyor
1. Dosyayı kaydedin
2. Ctrl+Space'e basın
3. "Developer Tools" açın (Help > Toggle Developer Tools)
4. Console'da hata var mı?

### Syntax Highlighting Yok
1. Dil modunu kontrol edin (sağ alt köşe)
2. "Change Language Mode" → "TYD" seçin
3. VSCode'u yeniden yükleyin (Ctrl+Shift+P > "Reload Window")

### Derleme Hatası
```bash
# Temizle ve yeniden derle
rm -rf out node_modules
npm install
npm run compile
```

## 📊 Test Checklist

Tüm özellikleri test etmek için:

- [ ] Syntax highlighting çalışıyor
- [ ] IntelliSense önerileri geliyor
- [ ] Hover tooltips görünüyor
- [ ] Code snippets çalışıyor
- [ ] Diagnostics hataları gösteriyor
- [ ] Otomatik parantez eşleştirme çalışıyor
- [ ] Girintileme doğru
- [ ] Ctrl+/ yorum toggling çalışıyor
- [ ] Kod katlama çalışıyor

## 📚 Ek Kaynaklar

- **Kullanıcı Rehberi:** [README.md](README.md)
- **Kurulum Detayları:** [INSTALLATION.md](INSTALLATION.md)
- **Geliştirici Kılavuzu:** [DEVELOPMENT.md](DEVELOPMENT.md)
- **TYD Dili Rehberi:** [../QUICKSTART.md](../QUICKSTART.md)

## 🎉 Başarılı!

Eklenti hazır! Artık TYD dosyalarını profesyonel bir editör desteğiyle yazabilirsiniz.

**F5'e basın ve test edin!** 🚀

---

### Hızlı Komutlar

```bash
# Derleme
npm run compile

# Watch mode
npm run watch

# VSCode'da test
code . && # F5'e bas

# Kalıcı kurulum
ln -s $(pwd) ~/.vscode/extensions/tyd-language-0.1.0
```

Sorularınız için: guven.acar@gmail.com
