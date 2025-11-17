<!-- 🔒 UYARI: BU DOSYA TYD DİLİNİN TEK GERÇEĞİDİR (SINGLE SOURCE OF TRUTH) -->
<!-- 🚫 HİÇBİR AI/GELIŞTIRICI BU DOSYAYI SPECS_LOCK.md OLMADAN DEĞİŞTİREMEZ -->

# TYD Dil Spesifikasyonu v1.0

**Durum:** 🔒 **LOCKED** - Değişiklik için SPECS_LOCK.md'ye kayıt gerekli  
**Son Güncelleme:** 17 Kasım 2024  
**Yazar:** Güven Acar  

---

## 📋 İÇİNDEKİLER

1. [Değiştirilemez Kurallar](#değiştirilemez-kurallar)
2. [Söz Dizimi](#söz-dizimi)
3. [Anahtar Kelimeler](#anahtar-kelimeler)
4. [Operatörler](#operatörler)
5. [Veri Tipleri](#veri-tipleri)
6. [Kontrol Yapıları](#kontrol-yapıları)
7. [Fonksiyonlar](#fonksiyonlar)

---

## 🚫 DEĞİŞTİRİLEMEZ KURALLAR

### Kural 1: Noktalı Virgül (;) Kullanımı

**SADECE değişken tanımlamalarında kullanılır:**

✅ **İZİN VERİLEN:**
```tyd
SAYISAL x;
SAYISAL y = 10;
METIN isim = "Ahmet";
MANTIKSAL aktif = DOĞRU;
```

❌ **KESINLIKLE YASAK:**
```tyd
DÖNÜŞ x + y;        -- ❌ YANLIŞ
SON;                -- ❌ YANLIŞ  
SON İŞLEÇ;          -- ❌ YANLIŞ
İŞLEÇ SON;          -- ❌ YANLIŞ
YAZDIR x;           -- ❌ YANLIŞ (YAZDIR'de noktalı virgül YOK)
```

**NEDEN?** 
- Söz dizimi tutarlılığı
- Blok tabanlı yapılar nokta virgül gerektirmez
- Sadece tanımlamalar cümle benzeri yapılardır

---

### Kural 2: Blok Sonlandırma Formatı

**İki geçerli format:**

#### Format 1: `SON ANAHTAR`
```tyd
İŞLEÇ fonksiyon() İSE
    -- kod
SON İŞLEÇ

EĞER koşul İSE
    -- kod
SON EĞER

DÖNGÜ koşul İSE
    -- kod
SON DÖNGÜ
```

#### Format 2: `ANAHTAR SON` (Alternatif)
```tyd
İŞLEÇ fonksiyon() İSE
    -- kod
İŞLEÇ SON
```

❌ **ASLA:**
```tyd
SON;              -- ❌ Noktalı virgül yok
SON İŞLEÇ;        -- ❌ Noktalı virgül yok
```

---

### Kural 3: Yorumlar

**Tek satır:** `--`
```tyd
-- Bu bir yorum
SAYISAL x = 10  -- Satır sonu yorumu
```

**Çok satırlı:** `---`
```tyd
---
Bu bir
çok satırlı
yorum bloğu
---
```

---

## 🔤 SÖZ DİZİMİ

### Değişken Tanımlama

```tyd
SAYISAL x;                    -- Tanımlama (varsayılan = 0)
SAYISAL y = 10;               -- Başlangıç değeriyle
METIN isim = "Ahmet";         -- String tanımlama
MANTIKSAL aktif = DOĞRU;      -- Boolean tanımlama
```

**Önemli:** Noktalı virgül **SADECE** bu satırlarda!

---

### Atama

```tyd
x = 20                        -- ❌ Noktalı virgül YOK
isim = "Mehmet"               -- ❌ Noktalı virgül YOK
```

---

### Fonksiyon Tanımlama

```tyd
İŞLEÇ topla(a, b) İSE
    DÖNÜŞ a + b               -- ❌ Noktalı virgül YOK
SON İŞLEÇ
```

**Çağırma:**
```tyd
SAYISAL sonuc = topla(5, 3);  -- ✅ Tanımlama olduğu için noktalı virgül VAR
```

---

### Koşullu İfadeler

```tyd
EĞER x > 10 İSE
    YAZDIR "Büyük"            -- ❌ Noktalı virgül YOK
DEĞİLSE
    YAZDIR "Küçük"            -- ❌ Noktalı virgül YOK
SON EĞER
```

---

### Döngüler

```tyd
DÖNGÜ sayac < 10 İSE
    YAZDIR sayac              -- ❌ Noktalı virgül YOK
    sayac = sayac + 1         -- ❌ Noktalı virgül YOK
SON DÖNGÜ
```

---

### Yazdırma

```tyd
YAZDIR "Merhaba"              -- ❌ Noktalı virgül YOK
YAZDIR x + y                  -- ❌ Noktalı virgül YOK
```

---

## 🔑 ANAHTAR KELİMELER

| Türkçe | İngilizce Karşılık | Kullanım |
|--------|-------------------|----------|
| `İŞLEÇ` | function | Fonksiyon tanımlama |
| `DÖNÜŞ` | return | Değer döndürme |
| `EĞER` | if | Koşul başlangıcı |
| `DEĞİLSE` | else | Alternatif blok |
| `DÖNGÜ` | while | Döngü başlangıcı |
| `SON` | end | Blok sonlandırma |
| `SAYISAL` | int/number | Tam sayı tipi |
| `METIN` | string | Metin tipi |
| `MANTIKSAL` | boolean | Mantıksal tip |
| `DOĞRU` | true | Mantıksal doğru |
| `YANLIŞ` | false | Mantıksal yanlış |
| `YAZDIR` | print | Ekrana yazdır |
| `İSE` | then | Blok başlangıcı |

---

## ⚡ OPERATÖRLER

### Aritmetik
```tyd
x + y    -- Toplama
x - y    -- Çıkarma
x * y    -- Çarpma
x / y    -- Bölme
```

### Karşılaştırma
```tyd
x == y   -- Eşitlik
x != y   -- Eşitsizlik
x > y    -- Büyüktür
x < y    -- Küçüktür
x >= y   -- Büyük eşit
x <= y   -- Küçük eşit
```

### Mantıksal
```tyd
a VE b   -- Mantıksal VE (AND)
a VEYA b -- Mantıksal VEYA (OR)
DEĞİL a  -- Mantıksal DEĞİL (NOT)
```

---

## 📊 VERİ TİPLERİ

| Tip | Anahtar Kelime | Varsayılan | Örnek |
|-----|---------------|-----------|-------|
| Tam Sayı | `SAYISAL` | 0 | `SAYISAL x = 42;` |
| Metin | `METIN` | "" | `METIN ad = "Ali";` |
| Mantıksal | `MANTIKSAL` | YANLIŞ | `MANTIKSAL ok = DOĞRU;` |

---

## 🎯 TAM ÖRNEK PROGRAM

```tyd
-- Fibonacci hesaplayan program

İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON EĞER
    
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON İŞLEÇ

İŞLEÇ ana() İSE
    SAYISAL i = 0;
    
    DÖNGÜ i < 10 İSE
        YAZDIR fibonacci(i)
        i = i + 1
    SON DÖNGÜ
    
    DÖNÜŞ 0
SON İŞLEÇ

ana()
```

**DİKKAT EDİLECEK NOKTALAR:**
- ✅ `SAYISAL i = 0;` → Noktalı virgül var (tanımlama)
- ❌ `i = i + 1` → Noktalı virgül yok (atama)
- ❌ `DÖNÜŞ n` → Noktalı virgül yok
- ❌ `YAZDIR ...` → Noktalı virgül yok
- ❌ `SON İŞLEÇ` → Noktalı virgül yok

---

## 🚫 PROJE KURALLARI

### Kural 4: Python Yasağı - Sadece C!

**TYD-MLP projesi tamamen Python bağımlılığından kurtulmuştur.**

❌ **KESINLIKLE YASAK:**
```bash
# Python kullanımı
python validate.py
pip install ...
#!/usr/bin/env python3

# Python kütüphaneleri
import sys
import re
```

✅ **ZORUNLU:**
```bash
# Sadece C kullanımı
gcc validator.c -o validator
./validator

# C kütüphaneleri
#include <stdio.h>
#include <stdlib.h>
```

**NEDEN?**
1. **Self-Hosting Hedefi:** TYD kendini derleyecek, Python bağımlılığı olamaz
2. **Bağımsızlık:** Proje hiçbir harici dil/runtime'a bağlı olmamalı
3. **Performans:** C native performans sağlar
4. **Tutarlılık:** Compiler C ile, araçlar da C ile yazılmalı

**Özel Durumlar:**
- Build scriptleri: Bash kullanılabilir (derle.sh)
- Test scriptleri: Bash kullanılabilir (test_all.sh)
- Dokümantasyon: Markdown kullanılabilir
- Her şey: **C ve Bash dışında HİÇBİR ŞEY KULLANILAMAZ**

**İstisnasız Kural:**
> Eğer bir araç gerekiyorsa, o aracı C ile yazın!

**Örnek:**
```bash
# ❌ YANLIŞ
# Python ile syntax validator
python tools/validate_syntax.py

# ✅ DOĞRU  
# C ile syntax validator
gcc tools/validator.c -o tools/validator
./tools/validator ornekler/*.tyd
```

---

### Kural 5: Hedef - Self-Hosting

**TYD-MLP'nin nihai hedefi: TYD dilini C ile self-hosting yapmak.**

**Bu Ne Demek?**

```
Aşama 0: Python Prototipi (Tamamlandı, Kaldırıldı)
   └── Python ile ilk compiler yazıldı
   └── tmp/python/ klasörüne taşındı

Aşama 1: C Bootstrap Compiler (ŞU AN BURADAYIZ)
   └── C ile TYD compiler yazıldı
   └── TYD kodunu assembly'ye çeviriyor
   └── c_compiler/ klasöründe

Aşama 2: TYD ile TYD Compiler (HEDEF)
   └── Compiler'ın kendisi TYD dilinde yazılacak
   └── self-host-compiler.tyd dosyası
   └── C compiler bu dosyayı derleyecek

Aşama 3: Self-Hosting (NİHAİ HEDEF)
   └── TYD compiler kendini derleyecek
   └── Bootstrap döngüsü tamamlanacak
   └── C compiler artık gerekmeyecek
```

**Mevcut Durum (17 Kasım 2024):**
- ✅ Aşama 1 %90 tamamlandı
- ✅ Basit programlar çalışıyor
- 🚧 Karmaşık özellikler ekleniyor
- ⏳ Aşama 2 için hazırlık yapılıyor

**Sonraki Adımlar:**
1. Tüm dil özelliklerini tamamla (struct, array, vb.)
2. Compiler'ın kendisini TYD'de yaz
3. Bootstrap döngüsünü kur
4. Self-hosting testi yap

**Başarı Kriteri:**
```bash
# Bu komut çalıştığında başardık demektir:
./tyd-compiler self-host-compiler.tyd -o tyd-compiler-new
./tyd-compiler-new self-host-compiler.tyd -o tyd-compiler-stage2
diff tyd-compiler-new tyd-compiler-stage2
# Çıktı: Binary'ler aynı!
```

---

## 📖 KAYNAKLAR

- Proje Deposu: `/home/pardus/projeler/tyd-lang/TYD-MLP`
- Test Örnekleri: `ornekler/*.tyd`
- C Compiler: `c_compiler/`
- Validator: `tools/validate_syntax.py`

---

**© 2024 TYD-MLP Projesi - Tüm hakları saklıdır**

