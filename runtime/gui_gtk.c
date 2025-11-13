/**
 * TYD-MLP GUI Runtime - GTK+ Backend
 *
 * Bu dosya TYD-MLP için GTK+ tabanlı GUI desteği sağlar.
 * Ring 0 (C Runtime) katmanında çalışır.
 */

#ifdef TYD_GUI_GTK

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GTK application global pointer
static GtkApplication *app = NULL;
static GtkWidget *main_window = NULL;
static GtkWidget *current_container = NULL;

/**
 * GTK uygulamasını başlat
 */
void tyd_gui_init(int argc, char **argv) {
    gtk_init(&argc, &argv);
}

/**
 * Pencere oluştur
 * @param baslik Pencere başlığı
 * @param genislik Pencere genişliği
 * @param yukseklik Pencere yüksekliği
 * @return Pencere işaretçisi
 */
void* tyd_pencere_olustur(const char* baslik, int genislik, int yukseklik) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), baslik);
    gtk_window_set_default_size(GTK_WINDOW(window), genislik, yukseklik);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // Kapatma sinyali
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Vertical box (container) oluştur
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    main_window = window;
    current_container = vbox;

    return window;
}

/**
 * Etiket (label) widget'ı oluştur
 * @param metin Etiket metni
 * @return Etiket işaretçisi
 */
void* tyd_etiket_olustur(const char* metin) {
    GtkWidget *label = gtk_label_new(metin);

    if (current_container) {
        gtk_box_pack_start(GTK_BOX(current_container), label, FALSE, FALSE, 5);
    }

    return label;
}

/**
 * Etiketi güncelle
 * @param etiket Etiket işaretçisi
 * @param yeni_metin Yeni metin
 */
void tyd_etiket_guncelle(void* etiket, const char* yeni_metin) {
    gtk_label_set_text(GTK_LABEL(etiket), yeni_metin);
}

/**
 * Buton callback wrapper
 */
typedef struct {
    void (*callback)(void);
} TydButtonCallback;

static void button_clicked_handler(GtkWidget *widget, gpointer data) {
    TydButtonCallback *cb = (TydButtonCallback*)data;
    if (cb && cb->callback) {
        cb->callback();
    }
}

/**
 * Buton widget'ı oluştur
 * @param etiket Buton etiketi
 * @param callback Tıklama callback fonksiyonu (TYD fonksiyonu olabilir)
 * @return Buton işaretçisi
 */
void* tyd_buton_olustur(const char* etiket, void (*callback)(void)) {
    GtkWidget *button = gtk_button_new_with_label(etiket);

    if (callback) {
        TydButtonCallback *cb = malloc(sizeof(TydButtonCallback));
        cb->callback = callback;
        g_signal_connect(button, "clicked", G_CALLBACK(button_clicked_handler), cb);
    }

    if (current_container) {
        gtk_box_pack_start(GTK_BOX(current_container), button, FALSE, FALSE, 5);
    }

    return button;
}

/**
 * Metin giriş widget'ı oluştur
 * @return Giriş işaretçisi
 */
void* tyd_giris_olustur(const char* placeholder) {
    GtkWidget *entry = gtk_entry_new();

    if (placeholder) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    }

    if (current_container) {
        gtk_box_pack_start(GTK_BOX(current_container), entry, FALSE, FALSE, 5);
    }

    return entry;
}

/**
 * Giriş alanından metin al
 * @param giris Giriş işaretçisi
 * @return Metin (caller'ın free etmesi gerekir)
 */
char* tyd_giris_al(void* giris) {
    const char* text = gtk_entry_get_text(GTK_ENTRY(giris));
    return strdup(text);
}

/**
 * Pencereyi göster
 * @param pencere Pencere işaretçisi
 */
void tyd_pencere_goster(void* pencere) {
    gtk_widget_show_all(GTK_WIDGET(pencere));
}

/**
 * GUI ana döngüsünü başlat
 * Bu fonksiyon bloklar, pencere kapanana kadar return etmez
 */
void tyd_gui_baslat(void) {
    if (main_window) {
        gtk_widget_show_all(main_window);
    }
    gtk_main();
}

/**
 * Mesaj kutusu göster
 * @param baslik Başlık
 * @param mesaj Mesaj
 * @param tip 0=bilgi, 1=uyarı, 2=hata
 */
void tyd_mesaj_kutusu(const char* baslik, const char* mesaj, int tip) {
    GtkMessageType gtk_tip;

    switch(tip) {
        case 1: gtk_tip = GTK_MESSAGE_WARNING; break;
        case 2: gtk_tip = GTK_MESSAGE_ERROR; break;
        default: gtk_tip = GTK_MESSAGE_INFO; break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        gtk_tip,
        GTK_BUTTONS_OK,
        "%s", mesaj
    );

    gtk_window_set_title(GTK_WINDOW(dialog), baslik);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

#endif // TYD_GUI_GTK
