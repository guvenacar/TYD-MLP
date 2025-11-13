# MLP Modern Özellikler - Söz Dizimi Tasarımı

> Bu dokümanda MLP'nin modern özelliklerinin (OOP, Generics, Pattern Matching, vb.) söz dizimi tasarlanmıştır.

---

## 🎯 Tasarım Prensipleri

1. **Türkçe-friendly**: Native MLP syntax Türkçe keyword'ler kullanır
2. **Multi-syntax**: Python/Java/C style alternatifleri mevcut
3. **Açık ve net**: Ambiguity yok
4. **Modern**: Rust, Swift, Kotlin'den ilham
5. **Güvenli**: Type safety, memory safety

---

## 1️⃣ OOP (Object-Oriented Programming)

### Class Tanımlama

**Native MLP:**
```mlp
SINIF Kişi
    -- Fields (private by default)
    ÖZEL METIN isim;
    ÖZEL SAYISAL yaş;

    -- Constructor
    KURUCU(METIN isim, SAYISAL yaş)
        BU.isim = isim;
        BU.yaş = yaş;
    KURUCU SON

    -- Public method
    AÇIK IŞLEÇ selamla()
        YAZDIR "Merhaba, ben " + BU.isim;
    IŞLEÇ SON

    -- Getter (property)
    AÇIK ÖZELLİK isim_al() -> METIN
        DÖNÜŞ BU.isim;
    ÖZELLİK SON

    -- Setter
    AÇIK ÖZELLİK isim_ayarla(METIN yeni_isim)
        BU.isim = yeni_isim;
    ÖZELLİK SON
SINIF SON

-- Kullanım
Kişi ahmet = YENİ Kişi("Ahmet", 25);
ahmet.selamla();
```

**Python-style:**
```mlp
class Kişi:
    def __init__(self, isim: str, yaş: int):
        self._isim = isim
        self._yaş = yaş

    def selamla(self):
        print(f"Merhaba, ben {self._isim}")

    @property
    def isim(self) -> str:
        return self._isim

    @isim.setter
    def isim(self, yeni_isim: str):
        self._isim = yeni_isim

ahmet = Kişi("Ahmet", 25)
ahmet.selamla()
```

**Java-style:**
```mlp
class Kişi {
    private String isim;
    private int yaş;

    public Kişi(String isim, int yaş) {
        this.isim = isim;
        this.yaş = yaş;
    }

    public void selamla() {
        System.out.println("Merhaba, ben " + this.isim);
    }

    public String getIsim() {
        return this.isim;
    }

    public void setIsim(String yeniIsim) {
        this.isim = yeniIsim;
    }
}

Kişi ahmet = new Kişi("Ahmet", 25);
ahmet.selamla();
```

### Inheritance (Kalıtım)

**Native MLP:**
```mlp
-- Base class
SINIF Hayvan
    KORUNAN METIN isim;

    KURUCU(METIN isim)
        BU.isim = isim;
    KURUCU SON

    SANALı IŞLEÇ ses_çıkar()
        YAZDIR "Bir ses çıkarıyor...";
    IŞLEÇ SON
SINIF SON

-- Derived class
SINIF Köpek KALITIR Hayvan
    KURUCU(METIN isim)
        ÜST(isim);  -- Super constructor
    KURUCU SON

    ÜZERİNE_YAZ IŞLEÇ ses_çıkar()
        YAZDIR BU.isim + " havlıyor: Hav hav!";
    IŞLEÇ SON
SINIF SON

-- Kullanım
Köpek karabaş = YENİ Köpek("Karabaş");
karabaş.ses_çıkar();  -- "Karabaş havlıyor: Hav hav!"
```

**Python-style:**
```mlp
class Hayvan:
    def __init__(self, isim: str):
        self._isim = isim

    def ses_çıkar(self):
        print("Bir ses çıkarıyor...")

class Köpek(Hayvan):
    def __init__(self, isim: str):
        super().__init__(isim)

    def ses_çıkar(self):
        print(f"{self._isim} havlıyor: Hav hav!")

karabaş = Köpek("Karabaş")
karabaş.ses_çıkar()
```

### Abstract Classes & Interfaces

**Native MLP:**
```mlp
-- Abstract class
SOYUT SINIF Şekil
    SOYUT IŞLEÇ alan() -> SAYISAL
    SOYUT IŞLEÇ çevre() -> SAYISAL

    -- Concrete method
    IŞLEÇ bilgi_göster()
        YAZDIR "Alan: " + BU.alan();
        YAZDIR "Çevre: " + BU.çevre();
    IŞLEÇ SON
SINIF SON

-- Interface (Trait)
ÖZELLİK Çizilebilir
    IŞLEÇ çiz()
    IŞLEÇ sil()
ÖZELLİK SON

-- Implementation
SINIF Daire KALITIR Şekil UYGULAR Çizilebilir
    ÖZEL SAYISAL yarıçap;

    KURUCU(SAYISAL yarıçap)
        BU.yarıçap = yarıçap;
    KURUCU SON

    ÜZERİNE_YAZ IŞLEÇ alan() -> SAYISAL
        DÖNÜŞ 3.14159 * BU.yarıçap * BU.yarıçap;
    IŞLEÇ SON

    ÜZERİNE_YAZ IŞLEÇ çevre() -> SAYISAL
        DÖNÜŞ 2 * 3.14159 * BU.yarıçap;
    IŞLEÇ SON

    UYGULA IŞLEÇ çiz()
        YAZDIR "Daire çiziliyor...";
    IŞLEÇ SON

    UYGULA IŞLEÇ sil()
        YAZDIR "Daire siliniyor...";
    IŞLEÇ SON
SINIF SON
```

---

## 2️⃣ Generics / Templates

### Generic Class

**Native MLP:**
```mlp
GENEL<T> SINIF Kutu
    ÖZEL T içerik;

    KURUCU(T içerik)
        BU.içerik = içerik;
    KURUCU SON

    IŞLEÇ al() -> T
        DÖNÜŞ BU.içerik;
    IŞLEÇ SON

    IŞLEÇ ayarla(T yeni_içerik)
        BU.içerik = yeni_içerik;
    IŞLEÇ SON
SINIF SON

-- Kullanım
Kutu<SAYISAL> sayı_kutusu = YENİ Kutu<SAYISAL>(42);
Kutu<METIN> metin_kutusu = YENİ Kutu<METIN>("Merhaba");

YAZDIR sayı_kutusu.al();   -- 42
YAZDIR metin_kutusu.al(); -- "Merhaba"
```

### Generic Function

**Native MLP:**
```mlp
GENEL<T> IŞLEÇ takas(REF T a, REF T b)
    T geçici = a;
    a = b;
    b = geçici;
IŞLEÇ SON

-- Kullanım
SAYISAL x = 10;
SAYISAL y = 20;
takas<SAYISAL>(x, y);
YAZDIR x;  -- 20
YAZDIR y;  -- 10
```

### Generic Constraints

**Native MLP:**
```mlp
-- T must implement Karşılaştırılabilir
GENEL<T> NEREDE T: Karşılaştırılabilir
IŞLEÇ maks(T a, T b) -> T
    EĞER a > b İSE
        DÖNÜŞ a;
    DEĞILSE
        DÖNÜŞ b;
    EĞER SON
IŞLEÇ SON

-- Multiple constraints
GENEL<T> NEREDE T: Çizilebilir, Klonlanabilir
SINIF Canvas
    DİZİ<T> nesneler;

    IŞLEÇ ekle(T nesne)
        DİZİ_EKLE BU.nesneler nesne.klon();
    IŞLEÇ SON

    IŞLEÇ hepsini_çiz()
        HER nesne İÇİNDE BU.nesneler
            nesne.çiz();
        HER SON
    IŞLEÇ SON
SINIF SON
```

---

## 3️⃣ Pattern Matching

### Basic Pattern Matching

**Native MLP:**
```mlp
SAYISAL sayı = 42;

EŞLEŞTIR sayı
    DURUM 0
        YAZDIR "Sıfır";
    DURUM 1
        YAZDIR "Bir";
    DURUM 42
        YAZDIR "Hayatın anlamı!";
    VARSAYILAN
        YAZDIR "Başka bir sayı";
EŞLEŞTIR SON
```

### Enum Matching

**Native MLP:**
```mlp
SABIT Renk
    Kırmızı,
    Yeşil,
    Mavi,
    RGB(SAYISAL, SAYISAL, SAYISAL)
SABIT SON

IŞLEÇ renk_açıkla(Renk renk)
    EŞLEŞTIR renk
        DURUM Renk.Kırmızı
            YAZDIR "Bu kırmızı";
        DURUM Renk.Yeşil
            YAZDIR "Bu yeşil";
        DURUM Renk.Mavi
            YAZDIR "Bu mavi";
        DURUM Renk.RGB(r, g, b)
            YAZDIR "RGB: " + r + ", " + g + ", " + b;
    EŞLEŞTIR SON
IŞLEÇ SON

-- Kullanım
renk_açıkla(Renk.Kırmızı);
renk_açıkla(Renk.RGB(255, 128, 0));
```

### Option/Result Pattern

**Native MLP:**
```mlp
SABIT Seçenek<T>
    Bazı(T),
    Hiç
SABIT SON

IŞLEÇ bul(DİZİ<METIN> liste, METIN aranan) -> Seçenek<SAYISAL>
    HER (index, item) İÇİNDE liste
        EĞER item == aranan İSE
            DÖNÜŞ Seçenek.Bazı(index);
        EĞER SON
    HER SON
    DÖNÜŞ Seçenek.Hiç;
IŞLEÇ SON

-- Kullanım
DİZİ<METIN> isimler = ["Ahmet", "Mehmet", "Ayşe"];
Seçenek<SAYISAL> sonuç = bul(isimler, "Mehmet");

EŞLEŞTIR sonuç
    DURUM Bazı(index)
        YAZDIR "Bulundu, index: " + index;
    DURUM Hiç
        YAZDIR "Bulunamadı";
EŞLEŞTIR SON
```

### Destructuring

**Native MLP:**
```mlp
-- Tuple destructuring
(SAYISAL x, SAYISAL y) = (10, 20);

-- Array destructuring
DİZİ<SAYISAL> sayılar = [1, 2, 3, 4, 5];
[ilk, ikinci, ...geri_kalanlar] = sayılar;

-- Struct destructuring
YAPI Nokta
    SAYISAL x;
    SAYISAL y;
YAPI SON

Nokta p = Nokta{x: 10, y: 20};
Nokta{x, y} = p;
YAZDIR x;  -- 10
YAZDIR y;  -- 20
```

---

## 4️⃣ Error Handling

### Try/Catch

**Native MLP:**
```mlp
DENE
    METIN içerik = DOSYA_OKU "data.txt";
    SAYISAL sayı = METINDEN_SAYIYA içerik;
    YAZDIR sayı;
YAKALA DosyaHatası hata
    YAZDIR "Dosya hatası: " + hata.mesaj;
YAKALA AyrıştırmaHatası hata
    YAZDIR "Sayı parse hatası: " + hata.mesaj;
YAKALA hata
    YAZDIR "Genel hata: " + hata.mesaj;
NİHAYET
    YAZDIR "Her durumda çalışır (finally)";
DENE SON
```

### Result Type

**Native MLP:**
```mlp
SABIT Sonuç<T, E>
    Başarılı(T),
    Hatalı(E)
SABIT SON

IŞLEÇ böl(SAYISAL a, SAYISAL b) -> Sonuç<SAYISAL, METIN>
    EĞER b == 0 İSE
        DÖNÜŞ Sonuç.Hatalı("Sıfıra bölme hatası");
    EĞER SON
    DÖNÜŞ Sonuç.Başarılı(a / b);
IŞLEÇ SON

-- Kullanım
Sonuç<SAYISAL, METIN> sonuç = böl(10, 2);

EŞLEŞTIR sonuç
    DURUM Başarılı(değer)
        YAZDIR "Sonuç: " + değer;
    DURUM Hatalı(hata)
        YAZDIR "Hata: " + hata;
EŞLEŞTIR SON

-- Veya unwrap (panic if error)
SAYISAL değer = böl(10, 2).bekle();  -- unwrap

-- Veya default value
SAYISAL değer = böl(10, 0).veya_varsayılan(0);
```

---

## 5️⃣ Async/Await

### Async Functions

**Native MLP:**
```mlp
EŞZAMANLI_OLMAYAN IŞLEÇ veri_çek(METIN url) -> Sonuç<METIN, METIN>
    HTTP.İstek istek = HTTP.get(url);
    HTTP.Yanıt yanıt = BEKLE istek.gönder();

    EĞER yanıt.durum == 200 İSE
        DÖNÜŞ Sonuç.Başarılı(yanıt.içerik);
    DEĞILSE
        DÖNÜŞ Sonuç.Hatalı("HTTP hatası: " + yanıt.durum);
    EĞER SON
IŞLEÇ SON

-- Kullanım
EŞZAMANLI_OLMAYAN IŞLEÇ main()
    Sonuç<METIN, METIN> sonuç = BEKLE veri_çek("https://api.example.com");

    EŞLEŞTIR sonuç
        DURUM Başarılı(veri)
            YAZDIR veri;
        DURUM Hatalı(hata)
            YAZDIR "Hata: " + hata;
    EŞLEŞTIR SON
IŞLEÇ SON

main();
```

### Parallel Async

**Native MLP:**
```mlp
EŞZAMANLI_OLMAYAN IŞLEÇ hepsini_çek()
    -- Paralel olarak 3 istek yap
    Gelecek<METIN> görev1 = veri_çek("https://api.example.com/1");
    Gelecek<METIN> görev2 = veri_çek("https://api.example.com/2");
    Gelecek<METIN> görev3 = veri_çek("https://api.example.com/3");

    -- Hepsini bekle
    [veri1, veri2, veri3] = BEKLE TÜMÜ [görev1, görev2, görev3];

    YAZDIR veri1;
    YAZDIR veri2;
    YAZDIR veri3;
IŞLEÇ SON
```

---

## 6️⃣ Ownership & Memory Safety (Rust-inspired)

### Ownership

**Native MLP:**
```mlp
IŞLEÇ sahipliği_al(SAHİP METIN s)
    -- s'nin sahipliği buraya taşındı
    YAZDIR s;
    -- s burada yok ediliyor (drop)
IŞLEÇ SON

METIN metin = "Merhaba";
sahipliği_al(metin);
-- metin artık geçersiz! Compiler hatası verir
```

### Borrowing

**Native MLP:**
```mlp
-- Immutable borrow (referans)
IŞLEÇ uzunluk_hesapla(REF METIN s) -> SAYISAL
    DÖNÜŞ UZUNLUK s;
IŞLEÇ SON

-- Mutable borrow
IŞLEÇ büyüt(MUT_REF METIN s)
    s = BÜYÜK_HARFE s;
IŞLEÇ SON

METIN metin = "merhaba";
SAYISAL uzunluk = uzunluk_hesapla(metin);  -- Borrow
büyüt(metin);  -- Mutable borrow
YAZDIR metin;  -- "MERHABA"
```

### Lifetime Annotations

**Native MLP:**
```mlp
-- Lifetime 'a
IŞLEÇ<'a> en_uzun(REF<'a> METIN x, REF<'a> METIN y) -> REF<'a> METIN
    EĞER UZUNLUK x > UZUNLUK y İSE
        DÖNÜŞ x;
    DEĞILSE
        DÖNÜŞ y;
    EĞER SON
IŞLEÇ SON

METIN s1 = "kısa";
METIN s2 = "çok daha uzun";
REF METIN sonuç = en_uzun(s1, s2);
YAZDIR sonuç;  -- "çok daha uzun"
```

---

## 7️⃣ Modules & Imports

### Module Definition

**Native MLP:**
```mlp
-- dosya: matematik.mlp
MODÜL matematik

AÇIK SABİT PI = 3.14159;

AÇIK IŞLEÇ topla(SAYISAL a, SAYISAL b) -> SAYISAL
    DÖNÜŞ a + b;
IŞLEÇ SON

ÖZEL IŞLEÇ yardımcı_fonksiyon()
    -- Sadece bu modül içinde erişilebilir
IŞLEÇ SON

MODÜL SON
```

### Import

**Native MLP:**
```mlp
-- Tüm modülü import et
KULLAN matematik;
YAZDIR matematik.PI;
YAZDIR matematik.topla(1, 2);

-- Specific imports
KULLAN matematik.{PI, topla};
YAZDIR PI;
YAZDIR topla(1, 2);

-- Alias
KULLAN matematik AS mat;
YAZDIR mat.PI;

-- Wildcard (dikkatli kullan!)
KULLAN matematik.*;
YAZDIR PI;
YAZDIR topla(1, 2);
```

---

## 8️⃣ Type Inference

**Native MLP:**
```mlp
-- Explicit type
SAYISAL x = 10;

-- Type inference (var keyword)
VAR y = 20;  -- Compiler: y is SAYISAL

VAR isim = "Ahmet";  -- Compiler: isim is METIN

VAR liste = [1, 2, 3];  -- Compiler: liste is DİZİ<SAYISAL>

VAR dict = {"a": 1, "b": 2};  -- Compiler: dict is SÖZLÜK<METIN, SAYISAL>

-- Function return type inference
IŞLEÇ topla(SAYISAL a, SAYISAL b)
    DÖNÜŞ a + b;  -- Compiler infers return type: SAYISAL
IŞLEÇ SON
```

---

## 9️⃣ Attributes/Annotations

**Native MLP:**
```mlp
@test
IŞLEÇ test_topla()
    ASSERT topla(1, 2) == 3;
IŞLEÇ SON

@deprecated("Bunun yerine yeni_fonksiyon kullanın")
IŞLEÇ eski_fonksiyon()
    -- ...
IŞLEÇ SON

@inline
IŞLEÇ hızlı_fonksiyon()
    -- Compiler bunu inline edecek
IŞLEÇ SON

@derive(Eşitlenebilir, Klonlanabilir, Debug)
YAPI Nokta
    SAYISAL x;
    SAYISAL y;
YAPI SON
```

---

## 🔟 Operator Overloading

**Native MLP:**
```mlp
YAPI Vektör
    SAYISAL x;
    SAYISAL y;

    -- + operator overload
    IŞLEÇ_OPERATÖR + (Vektör diğer) -> Vektör
        DÖNÜŞ Vektör{
            x: BU.x + diğer.x,
            y: BU.y + diğer.y
        };
    IŞLEÇ SON

    -- == operator overload
    IŞLEÇ_OPERATÖR == (Vektör diğer) -> ZITLIK
        DÖNÜŞ BU.x == diğer.x VE BU.y == diğer.y;
    IŞLEÇ SON

    -- [] operator overload (indexing)
    IŞLEÇ_OPERATÖR [] (SAYISAL index) -> SAYISAL
        EŞLEŞTIR index
            DURUM 0
                DÖNÜŞ BU.x;
            DURUM 1
                DÖNÜŞ BU.y;
            VARSAYILAN
                HATA_FIRLATIR "Index out of bounds";
        EŞLEŞTIR SON
    IŞLEÇ SON
YAPI SON

-- Kullanım
Vektör v1 = Vektör{x: 1, y: 2};
Vektör v2 = Vektör{x: 3, y: 4};
Vektör toplam = v1 + v2;  -- {x: 4, y: 6}
YAZDIR v1 == v2;  -- YANLIŞ
YAZDIR v1[0];  -- 1
```

---

## 📊 Karşılaştırma Tablosu

| Özellik | TYD-MLP (Stage 0) | MLP (Modern) |
|---------|-------------------|--------------|
| **Teknoloji** | Python compiler | Self-hosting (MLP ile yazılmış) |
| **OOP** | ❌ Yok | ✅ Class, Inheritance, Traits |
| **Generics** | ❌ Yok | ✅ Full generics support |
| **Pattern Matching** | ❌ Yok | ✅ Enum, destructuring |
| **Error Handling** | ❌ Basic | ✅ Try/catch + Result type |
| **Async/Await** | ❌ Yok | ✅ Full async support |
| **Memory Safety** | ❌ Yok | ✅ Ownership/borrowing (Rust-like) |
| **Type Inference** | ❌ Yok | ✅ Hindley-Milner inference |
| **Operator Overload** | ❌ Yok | ✅ Full support |
| **Modules** | ❌ Basic | ✅ Modern module system |

---

## 🚀 Sonraki Adımlar

1. **MLP repo oluştur**
2. **OOP implementation** (Class, Inheritance, Traits)
3. **Generics implementation**
4. **Pattern matching**
5. **Error handling**
6. **Async/await runtime**
7. **Memory safety checker**
8. **Type inference engine**
9. **Standard library**
10. **Bootstrap!**

---

**TYD-MLP → Seed compiler (minimal)**
**MLP → Production-ready, modern language!** 🚀
