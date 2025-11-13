# TYD-MLP Hızlı Başlangıç Rehberi

## 🚀 Kurulum

### Gereksinimler
- Python 3.8+
- GCC (C compiler)
- NASM (Netwide Assembler)

### Linux/Ubuntu/Debian:
```bash
sudo apt install python3 gcc nasm
```

### Pardus:
```bash
sudo apt install python3 gcc nasm
```

### macOS:
```bash
brew install python3 gcc nasm
```

## 📝 İlk Programınız

### 1. Basit bir TYD dosyası oluşturun:

**merhaba.tyd:**
```tyd
YAZDIR "Merhaba Dünya!";

SAYISAL x = 5;
SAYISAL y = 10;
SAYISAL toplam = x + y;

YAZDIR "Toplam:";
YAZDIR toplam;
```

### 2. Derleyin ve Çalıştırın:

**Manuel derleme:**
```bash
# 1. TYD → Assembly
python3 -m src.compiler merhaba.tyd

# 2. Assembly → Object
nasm -f elf64 build/output.asm -o build/output.o

# 3. C Runtime → Object
gcc -c runtime/runtime.c -o build/runtime.o

# 4. Link
gcc build/output.o build/runtime.o -o build/program -no-pie

# 5. Çalıştır
./build/program
```

**Otomatik build script ile:**
```bash
chmod +x build.sh
./build.sh merhaba.tyd
```

## 📚 Örnek Programlar

Proje `ornekler/` klasöründe öğretici programlar içerir:

- **01-merhaba.tyd** - Basit yazdırma
- **02-degiskenler.tyd** - Değişkenler ve tipler
- **03-kosullar.tyd** - IF/ELSE yapıları
- **04-donguler.tyd** - Döngüler
- **05-fonksiyonlar.tyd** - Fonksiyon tanımlama

Her birini derleyip çalıştırabilirsiniz:
```bash
./build.sh ornekler/01-merhaba.tyd
./build.sh ornekler/02-degiskenler.tyd
```

## 🎓 Temel Söz Dizimi

### Değişkenler
```tyd
SAYISAL x = 10;        -- Integer veya float
METIN ad = "Ali";      -- String
ZITLIK doğru = 1;      -- Boolean (1=true, 0=false)
```

### Operatörler
```tyd
x + y     -- Toplama
x - y     -- Çıkarma
x * y     -- Çarpma
x / y     -- Bölme
x ** y    -- Üs alma (x^y)
```

### Karşılaştırma
```tyd
x == y    -- Eşit mi?
x > y     -- Büyük mü?
x < y     -- Küçük mü?
x >= y    -- Büyük veya eşit mi?
x <= y    -- Küçük veya eşit mi?
```

### Koşullar
```tyd
EĞER x > 5 İSE
    YAZDIR "Büyük";
DEĞILSE
    YAZDIR "Küçük";
EĞER SON
```

### Döngüler
```tyd
SAYISAL i = 0;
DÖNGÜ
    YAZDIR i;
    i = i + 1;
    
    EĞER i > 10 İSE
        DÖNGÜ_BITIR;
    EĞER SON
DÖNGÜ SON
```

### Fonksiyonlar
```tyd
SAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)
    DÖNÜŞ a + b
IŞLEÇ SON

SAYISAL sonuc = topla(5, 10);
YAZDIR sonuc;
```

### Yorumlar
```tyd
-- Bu tek satırlık yorum

---
Bu çok satırlı
yorum bloğu
---
```

## 🐛 Hata Ayıklama

### Derleme hatası alıyorsanız:

1. **Python modül hatası:**
   ```bash
   # src/__init__.py var mı kontrol edin
   touch src/__init__.py
   ```

2. **NASM bulunamadı:**
   ```bash
   sudo apt install nasm
   ```

3. **GCC bulunamadı:**
   ```bash
   sudo apt install gcc
   ```

4. **Sözdizimi hatası:**
   - Noktalı virgül (;) unutmayın
   - Blokları SON ile kapatın
   - Tırnak işaretlerini doğru kullanın

## 📖 Daha Fazla Bilgi

- **README.md** - Proje hakkında genel bilgi
- **SPECS.md** - Mimari ve tasarım kararları
- **PIPELINE.md** - Geliştirme yol haritası

## 💡 İpuçları

1. **Her zaman basit başlayın:** Önce "Merhaba Dünya" çalıştırın
2. **Hataları okuyun:** Derleyici size neyin yanlış olduğunu söyler
3. **Örneklere bakın:** ornekler/ klasöründeki dosyalar iyi referanstır
4. **Adım adım test edin:** Büyük program yazmadan önce küçük parçaları test edin

## 🆘 Yardım

Sorun yaşıyorsanız:
- GitHub Issues: https://github.com/guvenacar/TYD-MLP/issues
- Email: guven.acar@gmail.com

**Kolay gelsin! 🚀**
