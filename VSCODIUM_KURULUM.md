# VSCodium için TYD Eklentisi Kurulum

## ✅ Kurulum Tamamlandı!

Eklenti VSCodium'a başarıyla kuruldu.

## 🔄 VSCodium'u Yeniden Başlatın

Eklentinin aktif olması için VSCodium'u kapatıp tekrar açın:

```bash
# VSCodium'u kapat ve yeniden başlat
pkill codium && codium
```

veya:

- VSCodium'da tüm pencereleri kapatın
- VSCodium'u tekrar başlatın

## ✨ Test Etme

### Adım 1: Test Dosyasını Açın

VSCodium'da şu dosyayı açın:
```bash
codium /home/pardus/projeler/tyd-lang/TYD-MLP/test.tyd
```

### Adım 2: Özellikleri Test Edin

#### 1. Syntax Highlighting (Renklendirme)
- `YAZDIR`, `EĞER`, `SAYISAL` gibi anahtar kelimeler renkli mi?
- Stringler (`"Merhaba"`) farklı renkte mi?
- Sayılar vurgulanmış mı?

#### 2. IntelliSense (Kod Tamamlama)
Yeni bir satırda yazın:
```
SAY
```
Otomatik öneri kutusu açılmalı ve `SAYISAL` önerisi görünmeli.

**Manuel tetikleme:**
- `Ctrl+Space` tuşlarına basın

#### 3. Code Snippets
Yeni satırda yazın ve Tab'a basın:
```
eger[Tab]
```
EĞER bloğu otomatik oluşmalı:
```tyd
EĞER kosul İSE
    // kod
EĞER SON
```

**Diğer snippet'ler:**
- `say` + Tab → SAYISAL değişken
- `func` + Tab → Fonksiyon şablonu
- `dongu` + Tab → DÖNGÜ şablonu
- `yaz` + Tab → YAZDIR komutu

#### 4. Hover Tooltips
Fareyi şu kelimelerin üzerine getirin:
- `EĞER` → Açıklama ve örnek görünmeli
- `DÖNGÜ` → Dokümantasyon gösterilmeli
- `IŞLEÇ` → Fonksiyon açıklaması

#### 5. Diagnostics (Hata Kontrolü)
Hatalı kod yazın:
```tyd
EĞER x > 5 İSE
    YAZDIR "test"
```

Görmeniz gerekenler:
- ⚠️ Sarı çizgi: "Noktalı virgül eksik"
- 🔴 Kırmızı çizgi: "EĞER SON eksik"

#### 6. Otomatik Özellikler
- **Parantez:** `(` yazınca otomatik `)` eklenmeli
- **Tırnak:** `"` yazınca otomatik kapanmalı
- **Girintileme:** EĞER yazınca alt satır girintilenmeli
- **Yorum:** Ctrl+/ ile satırı yoruma çevirin

## 🔍 Eklenti Kontrolleri

### Eklenti Yüklü mü?

1. VSCodium'da Extensions panelini açın (Ctrl+Shift+X)
2. "TYD" arayın
3. "TYD Language Support" görünmeli

### Dil Modu Doğru mu?

- Dosyayı açtığınızda sağ alt köşede "TYD" yazmalı
- Yazmıyorsa tıklayın ve "TYD" seçin

### Sorun Giderme

#### Eklenti Görünmüyor
```bash
# Sembolik linki kontrol et
ls -la ~/.vscode-oss/extensions/ | grep tyd

# Yoksa tekrar oluştur
ln -sf /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd ~/.vscode-oss/extensions/tyd-language-0.1.0

# VSCodium'u yeniden başlat
pkill codium && codium
```

#### IntelliSense Çalışmıyor
1. Dosyayı kaydedin (Ctrl+S)
2. "Reload Window" yapın:
   - Ctrl+Shift+P
   - "Developer: Reload Window" yazın
   - Enter

#### Syntax Highlighting Yok
1. Sağ alt köşede dil modunu "TYD" olarak seçin
2. VSCodium'u yeniden başlatın

#### Developer Tools
Hata mesajlarını görmek için:
- Help > Toggle Developer Tools
- Console sekmesini kontrol edin

## 📝 Demo Dosyaları

Daha kapsamlı test için:
```bash
codium /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd/test-examples/demo.tyd
```

veya örnek programlar:
```bash
codium /home/pardus/projeler/tyd-lang/TYD-MLP/ornekler/05-fonksiyonlar.tyd
```

## ✅ Test Checklist

Tüm özellikleri test edin:

- [ ] Syntax highlighting çalışıyor
- [ ] IntelliSense önerileri geliyor
- [ ] Code snippets çalışıyor (Tab completion)
- [ ] Hover tooltips görünüyor
- [ ] Diagnostics hataları gösteriyor
- [ ] Otomatik parantez eşleştirme
- [ ] Otomatik girintileme
- [ ] Ctrl+/ yorum toggling
- [ ] Kod katlama (folding)

## 🎯 Hızlı Komutlar

```bash
# Yeni test dosyası oluştur
touch test_yeni.tyd
codium test_yeni.tyd

# Eklentiyi kaldır (gerekirse)
rm ~/.vscode-oss/extensions/tyd-language-0.1.0

# Eklentiyi tekrar yükle
ln -sf /home/pardus/projeler/tyd-lang/TYD-MLP/vscode-tyd ~/.vscode-oss/extensions/tyd-language-0.1.0
```

## 📚 Ek Kaynaklar

- [README.md](vscode-tyd/README.md) - Genel bilgi
- [QUICK_START.md](vscode-tyd/QUICK_START.md) - Hızlı başlangıç
- [INSTALLATION.md](vscode-tyd/INSTALLATION.md) - Detaylı kurulum

## 🆘 Yardım

Sorun yaşıyorsanız:
- GitHub Issues: https://github.com/guvenacar/TYD-MLP/issues
- Email: guven.acar@gmail.com

---

**VSCodium'u yeniden başlatın ve test.tyd dosyasını açın!** 🚀
