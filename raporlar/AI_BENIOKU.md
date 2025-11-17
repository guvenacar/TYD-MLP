# 🤖 AI Asistanları İçin TYD-MLP Projesi Kılavuzu

**Son Güncelleme:** 17 Kasım 2024  
**Durum:** 🚀 Self-Hosting Aşamasında  
**Hedef:** TYD dilini C ile self-hosting  

---

## ⚠️ BAŞLAMADAN ÖNCE ZORUNLU OKUMANIZ GEREKENLER

### 1. 🔒 SPECS.md - TEK GERÇEK KAYNAK
```bash
/home/pardus/projeler/tyd-lang/TYD-MLP/SPECS.md
```

**BU DOSYA KİLİTLİDİR!** Hiçbir AI/geliştirici bu dosyayı `SPECS_LOCK.md` olmadan değiştiremez.

### 2. 📚 TÜM Raporları Okuyun (ZORUNLU!)

**Raporlar dizin yapısı:**
```
raporlar/
├── AI_BENIOKU.md          # ← Bu dosya (her AI önce bunu okur)
├── claude/                # Claude AI raporları
│   ├── 2024-11-17-*.md
│   └── ...
├── chatgpt/               # ChatGPT raporları (gelecekte)
│   └── ...
├── gemini/                # Gemini raporları (gelecekte)
│   └── ...
└── [diğer AI'lar]/
```

**OKUMA SIRASI:**

```bash
# 1. Bu dosyayı okuyun
cat raporlar/AI_BENIOKU.md

# 2. TÜM AI raporlarını tarih sırasına göre okuyun
find raporlar/ -name "*.md" -not -name "AI_BENIOKU.md" | sort

# 3. En son raporu bulun (en güncel durum)
ls -lt raporlar/*/*.md | head -1

# Örnek komut - Tüm raporları kronolojik okuma
for dir in raporlar/*/; do
    echo "=== $(basename $dir) raporları ==="
    ls -lt "$dir"*.md 2>/dev/null | head -5
done
```

**Neden tüm raporlar?**
- Farklı AI'lar farklı zamanlarda çalıştı
- Her AI'ın yaklaşımı ve bulduğu çözümler farklı
- Tekrar eden hataları önlemek için geçmişi bilmek zorundasınız
- "Son nerede kaldık?" sorusunun cevabı raporlarda

---

## 📝 OTURUM SONU RAPOR YAZMA (ZORUNLU!)

**Her AI oturumu sonunda rapor yazmalıdır!**

### Rapor Formatı

````markdown
<!-- filepath: raporlar/[ai-adı]/YYYY-MM-DD-konu.md -->
# [AI Adı] Çalışma Raporu - YYYY-MM-DD

**AI:** [Claude/ChatGPT/Gemini/vb.]  
**Tarih:** YYYY-MM-DD  
**Oturum Süresi:** XX dakika  
**Durum:** ✅ Başarılı / ⚠️ Kısmi / ❌ Başarısız  

---

## 📋 Oturum Özeti

Kısa özet (2-3 cümle):
- Ne yapıldı?
- Ana sonuç nedir?

---

## ✅ Tamamlanan Görevler

- [x] Görev 1 açıklaması
- [x] Görev 2 açıklaması
- [ ] Görev 3 (başlandı ama bitmedi)

**Dosya Değişiklikleri:**
- `c_parser.c` - `specs_check_no_semicolon()` eklendi
- [validator.c](http://_vscodecontentref_/0) - Yeni dosya oluşturuldu
- [SPECS.md](http://_vscodecontentref_/1) - Python yasağı eklendi

---

## ⚠️ Karşılaşılan Sorunlar

### Sorun 1: [Açıklama]
**Belirtiler:**
```
Hata mesajı veya davranış
```

**Denenen Çözümler:**
1. Çözüm denemesi 1 - Sonuç: ❌
2. Çözüm denemesi 2 - Sonuç: ✅

**Nihai Çözüm:**
```c
// Kod örneği
```

**Öğrenilen Ders:**
- Neden oldu?
- Nasıl önlenir?

---

## 🔄 Sonraki AI İçin Görevler

### Yüksek Öncelik (Hemen)
1. [ ] Görev açıklaması
2. [ ] Görev açıklaması

### Orta Öncelik (Bu Hafta)
3. [ ] Görev açıklaması

### Düşük Öncelik (Gelecek)
4. [ ] Görev açıklaması

---

## 📊 Proje Durumu

**Derleme Durumu:**
```bash
✅ c_compiler: Derleniyor
✅ validator: Derleniyor
⚠️ test suite: Eksik
```

**Test Sonuçları:**
```bash
# Son test çıktısı
$ ./derle.sh ../ornekler/01-merhaba.tyd
✅ Başarılı

$ ./validator ../ornekler/
❌ 3 hata bulundu
```

**Kalan Sorunlar:**
- [ ] Sorun 1 açıklaması
- [x] Sorun 2 çözüldü

---

## 💡 Notlar ve İpuçları

**Sonraki AI için tavsiyeler:**
- İpucu 1
- İpucu 2
- Dikkat edilmesi gereken nokta

**Referanslar:**
- [SPECS.md](http://_vscodecontentref_/2) - Kural 1, Kural 2
- Önceki rapor: `raporlar/claude/2024-11-16-*.md`

---

## 📎 Ekler

**Oluşturulan/Değiştirilen Dosyalar:**
- [x] [validator.c](http://_vscodecontentref_/3)
- [x] `tools/Makefile`
- [x] [AI_BENIOKU.md](http://_vscodecontentref_/4)

**Commit Hash:** `abc123def` (varsa)

---

**Rapor Yazan:** [AI Adı + Model]  
**Son Kontrol:** YYYY-MM-DD HH:MM
`````

---

## 🎯 PROJE HEDEFİ

### Ana Hedef
**TYD dilini C ile self-hosting yapmak**

Bu şu anlama gelir:
1. TYD compiler'ı C ile yazıldı ✅
2. TYD compiler'ı TYD dilinde yeniden yazılacak 🚧
3. TYD compiler kendini derleyecek (bootstrap) 🎯

### Mevcut Durum (17 Kasım 2024)

```
✅ C Compiler Hazır
  ├── c_lexer.c      - Lexical Analysis
  ├── c_parser.c     - Syntax Analysis  
  ├── c_generator.c  - Assembly Generation
  └── main.c         - Entry Point

✅ Temel Dil Özellikleri
  ├── Değişkenler (SAYISAL, METIN, MANTIKSAL)
  ├── Fonksiyonlar (İŞLEÇ)
  ├── Koşullar (EĞER/DEĞİLSE)
  ├── Döngüler (DÖNGÜ)
  └── Yazdırma (YAZDIR)

🚧 Self-Hosting Aşaması
  ├── Basit programlar çalışıyor (01-merhaba.tyd)
  ├── Fonksiyonlar çalışıyor (topla, çarp)
  └── Karmaşık özellikler test ediliyor

🎯 Hedef: Compiler'ın Kendini Derlemesi
```

---

## 🚫 YAPMAMANIZ GEREKENLER (KRİTİK!)

### 1. Python Kullanmayın!
```bash
❌ YANLIŞ: Python validator yazalım
❌ YANLIŞ: Python ile test edelim
❌ YANLIŞ: pip install ...

✅ DOĞRU: C ile validator yazalım
✅ DOĞRU: C ile test edelim
✅ DOĞRU: Sadece C derleyici kullanın
```

**NEDEN?** 
- Proje Python bağımlılığından kurtuldu (2024-11-17)
- Tüm araçlar C ile yazılmalı
- Self-hosting hedefi Python'sız tam bağımsızlık gerektirir

### 2. SPECS.md'yi İhlal Etmeyin!

**YASAKLAR:**

```tyd
❌ DÖNÜŞ x + y;        -- Noktalı virgül yasak
❌ SON;                -- Noktalı virgül yasak
❌ SON İŞLEÇ;          -- Noktalı virgül yasak
❌ YAZDIR x;           -- Noktalı virgül yasak
```

**İZİN VERİLEN:**

```tyd
✅ SAYISAL x;          -- Sadece burada noktalı virgül
✅ SAYISAL y = 10;     -- Sadece tanımlamada
✅ DÖNÜŞ x + y         -- Noktalı virgül yok
✅ SON İŞLEÇ           -- Noktalı virgül yok
```

### 3. Tasarım Kararlarını Değiştirmeyin!

**Bu konularda kullanıcıya sormadan değişiklik yapmayın:**
- Anahtar kelimeler (İŞLEÇ, DÖNÜŞ, EĞER, vb.)
- Blok sonlandırma formatı (SON İŞLEÇ vs SON;)
- Noktalı virgül kullanımı
- Yorum söz dizimi (-- ve ---)
- Veri tipleri (SAYISAL, METIN, MANTIKSAL)

---

## ✅ YAPMANIZ GEREKENLER

### Her Oturumda (ZORUNLU!)

```bash
# 1. SPECS.md'yi okuyun
cat /home/pardus/projeler/tyd-lang/TYD-MLP/SPECS.md

# 2. SPECS_LOCK.md'yi kontrol edin
cat /home/pardus/projeler/tyd-lang/TYD-MLP/SPECS_LOCK.md

# 3. Son raporları okuyun
ls -lt raporlar/claude/ | head -5

# 4. Mevcut durumu test edin
cd c_compiler
./derle.sh ../ornekler/01-merhaba.tyd
./01-merhaba
```

### Kod Yazmadan Önce

1. **Syntax Kontrolü**
   ```bash
   # TYD kodunu SPECS.md'ye göre kontrol et
   grep -n ";" ornekler/test.tyd  # ; olmamalı (sadece tanımlamada)
   ```

2. **Parser Kontrolü**
   ```c
   // Her zaman specs_check_no_semicolon() ekle
   specs_check_no_semicolon("DÖNÜŞ komutu");
   ```

3. **Test**
   ```bash
   cd c_compiler
   gcc -o compiler_test *.c -lm
   ./compiler_test test.tyd output.asm
   nasm -f elf64 output.asm -o output.o
   gcc -no-pie output.o -o output
   ./output
   ```

---

## 📖 DİL KURALLARI (ÖZET)

### Noktalı Virgül (;)
```tyd
SAYISAL x;              ✅ Tanımlama
SAYISAL y = 10;         ✅ Başlangıç değerli tanımlama
x = 20                  ✅ Atama (noktalı virgül YOK)
DÖNÜŞ x                 ✅ Return (noktalı virgül YOK)
YAZDIR x                ✅ Print (noktalı virgül YOK)
```

### Blok Yapıları
```tyd
İŞLEÇ fonksiyon(a, b) İSE
    DÖNÜŞ a + b
SON İŞLEÇ               ✅ Doğru format

EĞER x > 5 İSE
    YAZDIR x
SON EĞER                ✅ Doğru format

DÖNGÜ i < 10 İSE
    i = i + 1
SON DÖNGÜ               ✅ Doğru format
```

### Yorumlar
```tyd
-- Tek satır yorum

---
Çok satırlı
yorum bloğu
---
```

---

## 🔧 GELİŞTİRME SÜRECİ

### 1. Yeni Özellik Eklerken

```markdown
1. SPECS.md'de tanımlı mı kontrol et
2. SPECS_LOCK.md'ye kayıt ekle
3. c_lexer.c'ye token ekle (gerekirse)
4. c_parser.c'ye parsing ekle
5. c_generator.c'ye assembly generation ekle
6. ornekler/ klasörüne test dosyası ekle
7. Derle ve test et
8. Rapor yaz (raporlar/claude/YYYY-MM-DD-*.md)
```

### 2. Hata Düzeltirken

```markdown
1. Hatayı raporlara kaydet
2. SPECS.md'ye uygun mu kontrol et
3. Geçmiş raporlarda benzer sorun var mı ara
4. Düzelt ve test et
5. Çözümü raporla
```

### 3. Test Yazarken

```bash
# Test dosyası formatı
# ornekler/XX-aciklama.tyd

# Örnek:
cat > ornekler/08-karmasik-fonksiyon.tyd << 'EOF'
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON EĞER
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON İŞLEÇ

YAZDIR fibonacci(10)
EOF

# Derle ve test et
cd c_compiler
./derle.sh ../ornekler/08-karmasik-fonksiyon.tyd
./08-karmasik-fonksiyon
```

---

## 📊 PROJE YAPISI

```
TYD-MLP/
├── SPECS.md                    # 🔒 Dil spesifikasyonu (KİLİTLİ)
├── SPECS_LOCK.md               # Değişiklik günlüğü
├── raporlar/
│   ├── AI_BENIOKU.md          # ← Bu dosya
│   └── claude/
│       └── 2024-11-17-*.md    # Geçmiş raporlar
├── c_compiler/
│   ├── c_lexer.c              # Lexical analyzer
│   ├── c_parser.c             # Parser (syntax check)
│   ├── c_generator.c          # Assembly generator
│   ├── main.c                 # Entry point
│   ├── derle.sh               # Build script
│   └── runtime.c              # Runtime library
├── ornekler/
│   ├── 01-merhaba.tyd         # Hello world
│   ├── 04-donguler.tyd        # Loops
│   └── 05-fonksiyonlar.tyd    # Functions
└── tmp/
    └── python/                # 🗑️ Eski Python kodu (KULLANILMAZ)
```

---

## 🎯 ŞU ANKİ ÖNCELİKLER

### Kısa Vadeli (Bu Hafta)
1. ✅ SPECS.md kilitleme - TAMAMLANDI
2. ✅ Parser strict kontroller - TAMAMLANDI
3. 🚧 Validator aracı (C ile) - DEVAM EDİYOR
4. 🚧 Tüm .tyd dosyalarını SPECS'e uyarla
5. 🚧 Test suite (C ile)

### Orta Vadeli (Bu Ay)
6. ⏳ Karmaşık özellikler (struct, array)
7. ⏳ Compiler'ı TYD'de yaz (self-host-compiler.tyd)
8. ⏳ Bootstrap süreci

### Uzun Vadeli (Gelecek)
9. 📅 Optimizasyonlar
10. 📅 Hata mesajları iyileştirme
11. 📅 IDE desteği (syntax highlighting, vb.)

---

## 🔍 SIKÇA SORULAN SORULAR

### S: Python neden tamamen kaldırıldı?
**C:** Self-hosting hedefi için TYD'nin hiçbir dış bağımlılığı olmamalı. Python sadece prototip aşamasındaydı.

### S: Neden C? Rust veya Go kullanılabilir miydi?
**C:** Minimal bağımlılık, maksimum kontrol, assembly'ye yakınlık. TYD'nin felsefesiyle uyumlu.

### S: Noktalı virgül kuralı neden bu kadar katı?
**C:** Tutarlılık. Blok tabanlı yapılar noktalı virgül gerektirmez. Sadece tanımlamalar cümle benzeri.

### S: SPECS.md'yi değiştirmem gerekirse?
**C:** 
1. SPECS_LOCK.md'ye kayıt ekle
2. Kullanıcıdan onay al
3. Tüm testleri güncelle
4. Commit yap

### S: Eski Python kodu nerede?
**C:** `tmp/python/` klasöründe. Referans amaçlı saklanıyor ama KULLANILMAZ.

---

## 📞 DESTEK VE İLETİŞİM

### Rapor Yazma
```bash
# Her önemli değişiklik için rapor yaz
nano raporlar/claude/2024-11-XX-konu.md

# Format:
# - Tarih
# - Yapılan değişiklikler
# - Karşılaşılan sorunlar
# - Çözümler
# - Test sonuçları
```

### Commit Mesajları
```bash
# İyi örnekler:
git commit -m "c_parser: SPECS.md kontrolü eklendi"
git commit -m "ornekler: Tüm .tyd dosyaları SPECS'e uyarlandı"
git commit -m "SPECS.md: Blok sonlandırma kuralı netleştirildi"

# Kötü örnekler:
git commit -m "fix"
git commit -m "update"
git commit -m "çalışıyor artık"
```

---

## 🎓 ÖĞRENİLMİŞ DERSLER

### Geçmiş Hatalar (Tekrarlamayın!)

1. **Python Bağımlılığı Ekleme**
   - ❌ Hata: Validator için Python önerildi
   - ✅ Çözüm: Her şey C ile yapılmalı

2. **SPECS.md'yi Görmezden Gelme**
   - ❌ Hata: Noktalı virgül kuralı ihmal edildi
   - ✅ Çözüm: Her değişiklik önce SPECS.md kontrol

3. **Test Etmeden Commit**
   - ❌ Hata: Parser değişikliği tüm testleri bozdu
   - ✅ Çözüm: Her commit öncesi ./derle.sh test

4. **Raporlama Yapmama**
   - ❌ Hata: Değişiklikler kayboldu, sonraki AI kafası karıştı
   - ✅ Çözüm: Her oturum sonunda rapor yaz

---

## ✅ BAŞLAMADAN ÖNCE CHECKLİST

Her oturumda bu adımları izleyin:

```markdown
- [ ] SPECS.md okudum
- [ ] SPECS_LOCK.md kontrol ettim
- [ ] Son 3 raporu okudum
- [ ] Mevcut durumu test ettim (./derle.sh)
- [ ] Yapacağım değişiklik SPECS'e uygun
- [ ] Python kullanmayacağım
- [ ] Test dosyası hazırlayacağım
- [ ] Commit sonrası rapor yazacağım
```

---

## 🚀 HEMEN BAŞLAYIN

```bash
# Projeyi klonla/güncelle
cd /home/pardus/projeler/tyd-lang/TYD-MLP

# SPECS.md'yi oku
cat SPECS.md

# Son raporları oku
ls -lt raporlar/ | head -3

# Mevcut durumu test et
cd c_compiler
./derle.sh ../ornekler/01-merhaba.tyd
./01-merhaba

# Yeni özellik ekle veya hata düzelt
# ... (kodlama) ...

# Test et
./derle.sh ../ornekler/test.tyd

# Rapor yaz
nano raporlar/claude/2024-11-XX-aciklama.md

# Commit yap
git add .
git commit -m "aciklama"
```

---

**Son Güncelleme:** 17 Kasım 2024  
**Yazar:** Güven Acar + Claude AI  
**Durum:** 🔒 Aktif Kılavuz  

**Notlar:**
- Bu dosya AI'lar için bir kılavuzdur
- İnsan geliştiriciler için de faydalıdır
- Düzenli olarak güncellenmelidir
- SPECS.md ile çelişmemelidir

---

## ⚠️ HAIKU RAPOR KURALI (ZORUNLU!)

**Tüm AI'lar max 200 satır rapor yazmalı!**

### 📏 Boyut Limitleri

```
✅ İZİN VERİLEN:
- Normal rapor: Max 200 satır
- Okuma süresi: 3-5 dakika

❌ YASAK:
- 500+ satırlık raporlar
- Tüm kod kopyala-yapıştır
- Konuşma geçmişi dump
```

### 📝 Rapor Formatı

```markdown
# [AI] Çalışma Raporu - YYYY-MM-DD

**AI:** [İsim]  
**Tarih:** YYYY-MM-DD  
**Süre:** XX dakika  
**Durum:** ✅/⚠️/❌

## Özet (2-3 cümle)
## ✅ Yapılanlar (checklist)
## ⚠️ Sorunlar (tablo: Sorun | Çözüm)
## 🔄 Sonraki Adımlar (öncelikli)
## 📊 Test Sonuçları
## 💡 Notlar
## 📎 Dosyalar
```

### 📚 Özet Sistemi

**Uzun rapor varsa (200+ satır) → Özet yaz!**

```
raporlar/
├── claude/
│   ├── 2024-11-17-konu.md           # Orijinal (500 satır)
│   └── ozet/
│       └── 2024-11-17-konu-OZET.md  # Özet (50 satır)
```

**Özet formatı:**
```markdown
# 📝 [AI] Rapor Özeti - YYYY-MM-DD

**Orijinal:** raporlar/[ai]/YYYY-MM-DD-konu.md  
**Durum:** ✅/⚠️/❌

## 🎯 Ne Yapıldı? (3-5 madde)
## ⚠️ Sorunlar (tablo)
## 🔄 Sonraki Adım (1 cümle)
## 📁 Dosyalar (tablo)
## 💡 Not (1 cümle)
```

---

## 📖 RAPOR OKUMA STRATEJİSİ

### Hızlı Başlangıç (3 dakika)

```bash
# 1. AI_BENIOKU.md oku
cat raporlar/AI_BENIOKU.md

# 2. SPECS.md oku (zorunlu)
cat SPECS.md

# 3. SON ÖZETLERİ oku ✅ YENİ!
ls -t raporlar/*/ozet/*OZET.md | head -3 | xargs cat

# 4. Proje durumunu test et
cd c_compiler && ./derle.sh ../ornekler/01-merhaba.tyd
```

### Detaylı İnceleme (Gerekirse)

```bash
# Özette yeterli bilgi yoksa orijinal raporu oku
cat raporlar/claude/2024-11-17-konu.md
```

**Kural:** Önce özet oku → Gerekirse orijinale git

---

## 🎯 RAPOR YAZMA KURALLARI

### Yapılması Gerekenler ✅

1. **Kısa ve öz** - Max 200 satır
2. **Tablo kullan** - Sorun/Çözüm tabloları
3. **Checklist kullan** - `[ ]` format
4. **Test sonucu göster** - Terminal çıktısı
5. **Önceki raporlara link ver** - Referans

### Yapılmaması Gerekenler ❌

1. **500+ satır rapor** - Özet yaz
2. **Tüm kod değişiklikleri** - Sadece kritik kısımlar
3. **Terminal çıktısı dump** - İlk/son 10 satır yeterli
4. **Detaysız özet** - "Çalıştı" yetmez, nasıl çalıştı?

---

## 📊 ÖRNEK RAPOR (İYİ)

```markdown
# Claude Rapor - 2024-11-17

**AI:** Claude | **Süre:** 90 dk | **Durum:** ✅

## Özet
Self-hosting için runtime.c oluşturuldu. Test başarılı.

## ✅ Yapılanlar
- [x] runtime.c (+320 satır)
- [x] String fonksiyonları (STRLEN, STRCAT)
- [ ] Bootstrap test (path sorunu)

## ⚠️ Sorunlar
| Sorun | Çözüm |
|-------|-------|
| Segfault | strcat→memcpy |
| Link error | runtime.o eklendi |

## 🔄 Sonraki Adım
1. [ ] Path resolution düzelt
2. [ ] Bootstrap test

## 📊 Test
```bash
✅ 01-merhaba.tyd
✅ 05-fonksiyonlar.tyd
⚠️  self-host-v1.tyd
```

## 📁 Dosyalar
- runtime.c (yeni)
- c_generator.c (+50 satır)
```

---