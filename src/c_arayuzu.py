# src/c_arayuzu.py (Sizin 'generator_c' fikriniz)

from src import ast_nodes

class CArayuzu:
    """
    Generator (Kod Üretici) için bir yardımcı sınıftır.
    TYD-MLP ve C dünyası arasındaki köprüyü (FFI) yönetir.
    C Çağrı Protokolü (ABI), 'extern' bildirimleri ve
    'sihirli' fonksiyonların (printf, malloc, free) yönetiminden sorumludur.
    """
    def __init__(self, generator):
        self.generator = generator
        # Bilinen (sihirli) C fonksiyonları ve TYD-MLP'deki adları
        self.c_fonksiyonlari = {
            # Eskiler:
            "YENI": "malloc",
            "SIL": "free",
            "YAZDIR": "printf",

            # Sözlük:
            "SOZLUK_OLUSTUR": "sozluk_olustur",
            "SOZLUK_EKLE": "sozluk_ekle",
            "SOZLUK_GETIR": "sozluk_getir",
            "SOZLUK_YOK_ET": "sozluk_yok_et",

            # STAGE 2 PHASE 1: Runtime Extension
            # String Functions:
            "UZUNLUK": "mlp_strlen",
            "ALT_METIN": "mlp_substring",
            "KARAKTER_AL": "mlp_char_at",
            "METIN_BIRLESTIR": "mlp_string_concat",
            "METIN_KARSILASTIR": "mlp_string_compare",

            # Array Functions:
            "DIZI_OLUSTUR": "mlp_array_new",
            "DIZI_EKLE": "mlp_array_push",
            "DIZI_AL": "mlp_array_get",
            "DIZI_UZUNLUK": "mlp_array_length",
            "DIZI_CIKAR": "mlp_array_pop",
            "DIZI_YOK_ET": "mlp_array_destroy",

            # Memory Management:
            "BELLEK_AYIR": "mlp_alloc",
            "BELLEK_SERBEST": "mlp_free",
            "BELLEK_BOYUTLANDIR": "mlp_realloc"
        }

    def emit(self, instruction):
        """Generator'un emit fonksiyonuna kısayol."""
        self.generator.emit(instruction)

    def visit(self, node):
        """Generator'un visit fonksiyonuna kısayol."""
        self.generator.visit(node)

    def is_c_function(self, islec_adi):
        """Bu, bizim tarafımızdan yönetilen bir C fonksiyonu mu? (örn: YENI)"""
        return islec_adi in self.c_fonksiyonlari

    def generate_extern_bildirimleri(self):
        """Gerekli tüm 'extern' bildirimlerini üretir."""
        benzersiz_c_isimleri = set(self.c_fonksiyonlari.values())
        bildirimler = []
        for c_adi in benzersiz_c_isimleri:
            bildirimler.append(f"extern {c_adi}")
        return bildirimler

    def generate_c_call(self, islec_adi, node):
        """
        Bir 'sihirli' C fonksiyonu çağrısı için Assembly üretir.
        (node bir IslecCagirma veya YazdirKomutu düğümüdür)
        """
        
        # --- 1. Argümanları Hazırla (C Protokolü: RDI, RSI...) ---
        
        arg_node_listesi = []
        if islec_adi == "YAZDIR":
            arg_node_listesi = [node.ifade] # YazdirKomutu'nun 1 argümanı var
        else:
            arg_node_listesi = node.argumanlar # IslecCagirma'nın argüman listesi

        yazmaclar = ["rdi", "rsi", "rdx", "rcx", "r8", "r9"]
        
        if len(arg_node_listesi) > len(yazmaclar):
            raise Exception(f"C çağrıları ('{islec_adi}') için 6'dan fazla argüman desteklenmiyor.")

        # Argümanları hesapla ve yazmaçlara taşı
        # (NOT: C protokolü (System V ABI) ilk argümanı RDI'a koyar.
        # Bu yüzden argümanları SIRA İLE (soldan sağa) hesaplamalıyız.)
        for i, arg_node in enumerate(arg_node_listesi):
            self.visit(arg_node) # Sonuç her zaman RAX'te
            hedef_yazmac = yazmaclar[i]
            self.emit(f"mov {hedef_yazmac}, rax  ; Argüman {i+1} -> {hedef_yazmac}")

        # --- 2. Özel İşleme (YAZDIR) ---
        if islec_adi == "YAZDIR":
            # YAZDIR için 2 argüman gerekir: (format, değer)
            # 1. argüman (değer) az önce RSI'a taşındı.
            # 2. argümanı (format) RDI'a elle ayarlamalıyız.
            
            # (Düzeltme: YAZDIR'ın tek argümanı var 'ifade'. Değeri RSI'da)
            self.emit(f"mov rsi, {yazmaclar[0]}  ; Değeri RDI'dan RSI'a (2. arg) taşı")
            
            if isinstance(node.ifade, ast_nodes.Metin):
                self.emit("mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı")
            else:
                self.emit("mov rdi, format_sayi    ; Formatı RDI'a (1. arg) taşı")

        # --- 3. C Fonksiyonunu Çağır ---
        c_fonksiyon_adi = self.c_fonksiyonlari[islec_adi]
        self.emit("mov rax, 0  ; (SSE kuralı)")
        self.emit(f"call {c_fonksiyon_adi}")
        # Sonuç (eğer varsa, 'malloc' gibi) zaten RAX'tedir.
        
    def generate_sozluk_getir(self, anahtar_node):
        """
        'call sozluk_getir' için C protokolünü üretir.
        Arg1 (RDI): Anahtar (string)
        Dönüş (RAX): Değer
        """
        self.emit(f"; --- C Arayüzü: sozluk_getir ---")
        
        # 1. Argümanı (Anahtar) RDI'a hazırla
        self.visit(anahtar_node) # Anahtar (Metin) AST düğümünü ziyaret et
        self.emit(f"mov rdi, rax  ; Argüman 1 (Anahtar) -> rdi")
        
        # 2. Çağrıyı yap
        self.emit("mov rax, 0")
        self.emit("call sozluk_getir")
        # Dönen değer (değer) zaten RAX'te

    def generate_sozluk_ekle(self, anahtar_node, deger_node):
        """
        'call sozluk_ekle' için C protokolünü üretir.
        Arg1 (RDI): Anahtar (string)
        Arg2 (RSI): Değer
        """
        self.emit(f"; --- C Arayüzü: sozluk_ekle ---")
        
        # 1. Argümanı (Anahtar) RDI'a hazırla
        self.visit(anahtar_node) # Anahtar (Metin) AST düğümünü ziyaret et
        self.emit(f"mov rdi, rax  ; Argüman 1 (Anahtar) -> rdi")

        # 2. Argümanı (Değer) RSI'a hazırla
        self.visit(deger_node) # Değer (İfade) AST düğümünü ziyaret et
        self.emit(f"mov rsi, rax  ; Argüman 2 (Değer) -> rsi")
        
        # 3. Çağrıyı yap
        self.emit("mov rax, 0")
        self.emit("call sozluk_ekle")