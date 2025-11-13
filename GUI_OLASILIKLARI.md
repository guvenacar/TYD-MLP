# TYD-MLP GUI Olasılıkları

TYD-MLP'nin Ring 3 (user) ve Ring 0 (C runtime) mimarisi sayesinde GUI uygulamaları geliştirilebilir.

## 1. GTK+ (Cross-Platform - ÖNERİLEN)

**Artıları:**
- ✅ Linux, Windows, macOS desteği
- ✅ Zengin widget kütüphanesi
- ✅ Modern, güzel arayüzler
- ✅ C ile native entegrasyon

**Örnek C Runtime Entegrasyonu:**

```c
// runtime/gui_gtk.c
#include <gtk/gtk.h>
#include "runtime.h"

// TYD'den çağrılabilir fonksiyonlar
void tyd_pencere_olustur(char* baslik, int genislik, int yukseklik) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), baslik);
    gtk_window_set_default_size(GTK_WINDOW(window), genislik, yukseklik);
    gtk_widget_show_all(window);
}

void tyd_buton_olustur(char* etiket, void (*callback)()) {
    GtkWidget *button = gtk_button_new_with_label(etiket);
    g_signal_connect(button, "clicked", G_CALLBACK(callback), NULL);
}

void tyd_etiket_olustur(char* metin) {
    GtkWidget *label = gtk_label_new(metin);
}
```

**TYD Kullanımı:**

```tyd
-- TYD-MLP GUI örneği
KULLAN GUI;  -- GUI modülünü yükle

PENCERE_OLUŞTUR "Merhaba Dünya" 400 300;
BUTON_OLUŞTUR "Tıkla" buton_tikla_callback;
ETİKET_OLUŞTUR "Hoş Geldiniz!";

IŞLEÇ buton_tikla_callback()
    YAZDIR "Butona tıklandı!";
IŞLEÇ SON

GUI_BAŞLAT;
```

---

## 2. Windows API (Windows Native)

**Artıları:**
- ✅ Windows'ta native performans
- ✅ Sistem entegrasyonu mükemmel
- ❌ Sadece Windows

**Örnek:**

```c
// runtime/gui_winapi.c
#include <windows.h>

HWND tyd_pencere_olustur(char* baslik, int x, int y, int w, int h) {
    HWND hwnd = CreateWindowEx(
        0, "BUTTON", baslik,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y, w, h,
        NULL, NULL, NULL, NULL
    );
    return hwnd;
}
```

---

## 3. Qt (C Bindings)

**Artıları:**
- ✅ Cross-platform
- ✅ Çok profesyonel görünüm
- ✅ Zengin widget seti
- ❌ Biraz karmaşık C entegrasyonu (C++ daha kolay)

---

## 4. SDL2 (Oyun/Grafik Uygulamaları)

**Artıları:**
- ✅ 2D/3D grafik desteği
- ✅ Cross-platform
- ✅ Oyun geliştirme için ideal
- ✅ Basit C API

**Örnek:**

```c
// runtime/gui_sdl.c
#include <SDL2/SDL.h>

SDL_Window* tyd_pencere_olustur(char* baslik, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    return SDL_CreateWindow(baslik,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, SDL_WINDOW_SHOWN);
}

void tyd_cizgi_ciz(int x1, int y1, int x2, int y2) {
    // SDL çizim fonksiyonları
}
```

---

## 5. ImGui (Immediate Mode GUI)

**Artıları:**
- ✅ Çok basit kullanım
- ✅ Oyun editörleri, tool'lar için ideal
- ✅ Hafif ve hızlı

---

## ÖNERİ: GTK+ ile Başlayın

### Neden GTK+?

1. **Cross-platform**: Tek kod, her yerde çalışır
2. **C-friendly**: TYD-MLP C runtime'ıyla perfect uyum
3. **Glade**: Visual editor ile arayüz tasarlayın
4. **Büyük topluluk**: Çok kaynak var

### Hızlı Başlangıç:

```bash
# Ubuntu/Debian
sudo apt-get install libgtk-3-dev

# Fedora
sudo dnf install gtk3-devel

# macOS
brew install gtk+3
```

### TYD-MLP Entegrasyonu:

```bash
# runtime/Makefile'a ekle:
CFLAGS += `pkg-config --cflags gtk+-3.0`
LDFLAGS += `pkg-config --libs gtk+-3.0`
```

---

## Örnek Basit GUI Uygulaması (TYD-MLP)

```tyd
-- basit_hesap_makinesi.tyd

KULLAN GUI;

SAYISAL sonuc = 0;

PENCERE pencere = PENCERE_OLUŞTUR "Hesap Makinesi" 300 200;

ETIKET sonuc_etiket = ETİKET_OLUŞTUR "Sonuç: 0";
GIRIS sayi1 = GIRIS_OLUŞTUR;
GIRIS sayi2 = GIRIS_OLUŞTUR;

BUTON topla_btn = BUTON_OLUŞTUR "Topla" topla;
BUTON cikar_btn = BUTON_OLUŞTUR "Çıkar" cikar;

IŞLEÇ topla()
    SAYISAL a = GIRIS_AL sayi1;
    SAYISAL b = GIRIS_AL sayi2;
    sonuc = a + b;
    ETİKET_GÜNCELLE sonuc_etiket "Sonuç: " + sonuc;
IŞLEÇ SON

IŞLEÇ cikar()
    SAYISAL a = GIRIS_AL sayi1;
    SAYISAL b = GIRIS_AL sayi2;
    sonuc = a - b;
    ETİKET_GÜNCELLE sonuc_etiket "Sonuç: " + sonuc;
IŞLEÇ SON

GUI_BAŞLAT pencere;
```

---

## Yol Haritası

### Faz 1: Temel GUI (v0.2.0)
- [ ] GTK+ entegrasyonu
- [ ] Temel widget'lar (Pencere, Buton, Etiket, Giriş)
- [ ] Event handling

### Faz 2: Gelişmiş Widget'lar (v0.3.0)
- [ ] Liste, Tablo, Menü
- [ ] Diyaloglar
- [ ] Layout yönetimi

### Faz 3: Modern GUI (v0.4.0)
- [ ] CSS styling
- [ ] Tema desteği
- [ ] Responsive design

---

## Sonuç

**✅ Ring 3 (TYD-MLP) + Ring 0 (C Runtime) mimarisi GUI için PERFECT!**

TYD-MLP kullanıcıları Türkçe komutlarla modern masaüstü uygulamaları yazabilir:
- `PENCERE_OLUŞTUR`
- `BUTON_EKLE`
- `MENÜ_OLUŞTUR`

Arka planda GTK+/Win32/Qt çalışır, kullanıcı sadece Türkçe kod yazar! 🚀
