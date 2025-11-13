# TYD-MLP GUI Geliştirme Yol Haritası

## 🎯 Vizyon

TYD-MLP kullanıcıları **Türkçe komutlarla** modern masaüstü uygulamaları geliştirebilecek:

```tyd
PENCERE_OLUŞTUR "Uygulamam" 800 600;
BUTON_EKLE "Kaydet" kaydet_fonksiyonu;
MENÜ_OLUŞTUR ["Dosya", "Düzenle", "Yardım"];
```

## 🏗️ Mimari

```
┌─────────────────────────────────────────┐
│  Ring 3: TYD-MLP Kullanıcı Kodu         │
│  --------------------------------        │
│  PENCERE_OLUŞTUR "App" 800 600;         │
│  BUTON_EKLE "Tıkla" callback;           │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  TYD Compiler                           │
│  - Lexer/Parser/Generator               │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  Ring 0: C Runtime + GUI Bindings       │
│  --------------------------------        │
│  void tyd_pencere_olustur(...)          │
│  void tyd_buton_ekle(...)               │
│                                         │
│  Backend: GTK+/Win32/Qt                 │
└─────────────────────────────────────────┘
```

## 📅 Faz 1: GTK+ Temel Entegrasyon (v0.2.0)

**Hedef:** Basit pencere ve widget desteği

### Görevler:
- [x] C Runtime GUI modülü (`runtime/gui_gtk.c`)
- [ ] TYD Lexer: GUI keyword'leri ekle
  - `PENCERE_OLUŞTUR`, `BUTON_EKLE`, `ETİKET_OLUŞTUR`, vb.
- [ ] TYD Parser: GUI ifadeleri parse et
- [ ] Generator: C GUI fonksiyon çağrıları üret
- [ ] Temel widget'lar:
  - [ ] Pencere (Window)
  - [ ] Buton (Button)
  - [ ] Etiket (Label)
  - [ ] Giriş (Entry)
  - [ ] Metin Alanı (TextView)

### Örnek Kullanım:

```tyd
GUI_INIT;

NESNE pencere = PENCERE_OLUŞTUR "Merhaba" 400 300;
NESNE etiket = ETİKET_OLUŞTUR "Hoş geldiniz!";
NESNE buton = BUTON_OLUŞTUR "Tıkla" buton_callback;

IŞLEÇ buton_callback()
    YAZDIR "Butona tıklandı!";
    ETİKET_GÜNCELLE etiket "Tıkladınız!";
IŞLEÇ SON

GUI_BAŞLAT;
```

## 📅 Faz 2: Layout ve Container'lar (v0.2.5)

**Hedef:** Gelişmiş yerleşim yönetimi

### Görevler:
- [ ] Box Layout (Vertical/Horizontal)
- [ ] Grid Layout
- [ ] Stack/Notebook (Sekmeler)
- [ ] ScrolledWindow
- [ ] Frame/Grouping

### Örnek:

```tyd
NESNE pencere = PENCERE_OLUŞTUR "Layout Örneği" 600 400;

KUTU dikey = DİKEY_KUTU_OLUŞTUR 5;
KUTU_EKLE dikey ETİKET_OLUŞTUR "Başlık";

KUTU yatay = YATAY_KUTU_OLUŞTUR 10;
KUTU_EKLE yatay BUTON_OLUŞTUR "Kaydet" kaydet;
KUTU_EKLE yatay BUTON_OLUŞTUR "İptal" iptal;

KUTU_EKLE dikey yatay;
PENCERE_CONTAINER_AYARLA pencere dikey;
```

## 📅 Faz 3: İleri Seviye Widget'lar (v0.3.0)

**Hedef:** Zengin widget kütüphanesi

### Widget'lar:
- [ ] Liste (ListView/TreeView)
- [ ] Tablo (Grid)
- [ ] Menü (MenuBar, ContextMenu)
- [ ] Toolbar
- [ ] Dialog'lar (File picker, Color picker, etc.)
- [ ] ProgressBar
- [ ] Slider/SpinButton
- [ ] CheckBox/RadioButton
- [ ] ComboBox

### Örnek:

```tyd
-- Liste oluştur
NESNE liste = LİSTE_OLUŞTUR ["İsim", "Yaş", "Şehir"];
LİSTE_SATIR_EKLE liste ["Ahmet" 25 "İstanbul"];
LİSTE_SATIR_EKLE liste ["Ayşe" 30 "Ankara"];

-- Menü oluştur
NESNE menu_bar = MENÜ_ÇUBUĞU_OLUŞTUR;
NESNE dosya_menu = MENÜ_OLUŞTUR "Dosya";
MENÜ_ÖĞESI_EKLE dosya_menu "Yeni" yeni_callback;
MENÜ_ÖĞESI_EKLE dosya_menu "Aç" ac_callback;
MENÜ_AYIRICI_EKLE dosya_menu;
MENÜ_ÖĞESI_EKLE dosya_menu "Çıkış" cikis_callback;
```

## 📅 Faz 4: Stil ve Tema (v0.3.5)

**Hedef:** Modern, güzel arayüzler

### Özellikler:
- [ ] CSS Styling desteği
- [ ] Önceden tanımlı temalar (Light/Dark)
- [ ] Özel tema oluşturma
- [ ] Font/Renk yönetimi
- [ ] İkon desteği

### Örnek:

```tyd
-- Tema ayarla
TEMA_AYARLA "dark";

-- CSS uygula
CSS_UYGULA buton1 "
    background-color: #3498db;
    color: white;
    border-radius: 5px;
    padding: 10px;
";

-- İkon ekle
BUTON_İKON_AYARLA kaydet_btn "document-save";
```

## 📅 Faz 5: Event Handling ve Binding (v0.4.0)

**Hedef:** Gelişmiş event yönetimi

### Özellikler:
- [ ] Keyboard events
- [ ] Mouse events (hover, drag, etc.)
- [ ] Custom signals
- [ ] Data binding (Model-View)
- [ ] Validation

### Örnek:

```tyd
-- Keyboard event
OLAY_EKLE pencere "key-press" klavye_eventi;

IŞLEÇ klavye_eventi(METIN tus)
    EĞER tus == "Escape" İSE
        PENCERE_KAPAT pencere;
    EĞER SON
IŞLEÇ SON

-- Data binding
NESNE model = MODEL_OLUŞTUR;
MODEL_VERİ_EKLE model "isim" "Ahmet";
MODEL_VERİ_EKLE model "yas" 25;

GİRİŞ_BAĞLA isim_giris model "isim";
```

## 📅 Faz 6: Cross-Platform ve Alternatif Backend'ler (v0.5.0)

**Hedef:** Her platformda çalışan uygulamalar

### Backend'ler:
- [x] GTK+ (Linux/Windows/macOS)
- [ ] Windows API (Native Windows)
- [ ] Cocoa (Native macOS)
- [ ] Qt (Gelişmiş cross-platform)

### Derleme:

```bash
# GTK+ ile derle (varsayılan)
tyd-mlp compile app.tyd --gui gtk

# Windows API ile derle
tyd-mlp compile app.tyd --gui win32

# Qt ile derle
tyd-mlp compile app.tyd --gui qt
```

## 📅 Faz 7: Modern Özellikler (v0.6.0)

**Hedef:** Modern GUI framework seviyesi

### Özellikler:
- [ ] Animasyonlar
- [ ] Responsive design
- [ ] Multi-window desteği
- [ ] Sistem tray entegrasyonu
- [ ] Bildirimler (notifications)
- [ ] Drag & Drop
- [ ] Clipboard desteği
- [ ] Print desteği

---

## 🚀 Başlangıç için Gereksinimler

### Linux (Ubuntu/Debian):
```bash
sudo apt-get install libgtk-3-dev build-essential
```

### macOS:
```bash
brew install gtk+3 pkg-config
```

### Windows:
```bash
# MSYS2 kullanın
pacman -S mingw-w64-x86_64-gtk3
```

---

## 📚 Kaynaklar

- [GTK+ Documentation](https://docs.gtk.org/gtk3/)
- [GTK+ Tutorial](https://www.gtk.org/docs/getting-started/)
- [Glade UI Designer](https://glade.gnome.org/)

---

## 🎓 Örnek Projeler

1. **Hesap Makinesi** (`ornekler/gui-hesap-makinesi.tyd`)
   - Temel widget kullanımı
   - Event handling
   - Layout yönetimi

2. **Metin Editörü** (Planlanan)
   - TextView/ScrolledWindow
   - Menü ve Toolbar
   - Dosya işlemleri

3. **Todo List Uygulaması** (Planlanan)
   - ListView
   - CheckBox
   - Veri yönetimi

4. **Form Builder** (Planlanan)
   - Dynamic widget oluşturma
   - Validation
   - Database entegrasyonu

---

**✨ TYD-MLP ile Türkçe GUI programlama devri başlıyor!** 🚀
