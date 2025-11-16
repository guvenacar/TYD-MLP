# src/c_arayuzu.py

from src import ast_nodes # AST düğümlerini içe aktarmak için (Degisken, Metin)

class CArayuzu:
    """
    Generator (Kod Üretici) için bir yardımcı sınıftır.
    TYD-MLP ve C dünyası arasındaki köprüyü (FFI) yönetir.
    """
    def __init__(self, generator):
        self.generator = generator
        # Bilinen (sihirli) C fonksiyonları ve TYD-MLP'deki adları
        self.c_fonksiyonlari = {
            # YENİ VE ESKİ TEMİZLİK FONKSİYONLARI (free/malloc)
            "YENI": "malloc",
            "SIL": "free",                 # <-- KRİTİK DÜZELTME: SIL komutu free'ye çevrilir
            "YAZDIR": "printf",
            
            # SÖZLÜK FONKSİYONLARI
            "SOZLUK_OLUSTUR": "sozluk_olustur",
            "SOZLUK_EKLE": "sozluk_ekle",
            "SOZLUK_GETIR": "sozluk_getir",
            "SOZLUK_YOK_ET": "sozluk_yok_et",
            
            # METİN BİRLEŞTİRME FONKSİYONU
            "STRING_BIRLESTIR": "string_birlestir",
            "STRING_KARSILASTIR": "string_karsilastir" # <-- BU SATIRI EKLEYİN 

        }

    def emit(self, instruction):
        """Generator'un emit fonksiyonuna kısayol."""
        self.generator.emit(instruction)

    def visit(self, node):
        """Generator'un visit fonksiyonuna kısayol."""
        self.generator.visit(node)

    def is_c_function(self, islec_adi):
        """Bu, bizim tarafımızdan yönetilen bir C fonksiyonu mu?"""
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
        """
        
        # --- 1. Argümanları Hazırla (Değişiklik yok) ---
        arg_node_listesi = []
        if islec_adi == "YAZDIR":
            arg_node_listesi = [node.ifade] # YazdirKomutu'nun 1 argümanı var
        else:
            arg_node_listesi = node.argumanlar # IslecCagirma'nın argüman listesi

        yazmaclar = ["rdi", "rsi", "rdx", "rcx", "r8", "r9"]
        
        if len(arg_node_listesi) > len(yazmaclar):
            raise Exception(f"C çağrıları ('{islec_adi}') için 6'dan fazla argüman desteklenmiyor.")

        for i, arg_node in enumerate(arg_node_listesi):
            self.visit(arg_node) # Argüman değeri (pointer veya sayı) RAX'te
            hedef_yazmac = yazmaclar[i]
            self.emit(f"mov {hedef_yazmac}, rax  ; Argüman {i+1} -> {hedef_yazmac}")

        # --- 2. Özel İşleme (YAZDIR) ---
        if islec_adi == "YAZDIR":
            # YAZDIR için: RDI=format, RSI=değer (Değer RDI'dan RSI'a taşınır)
            self.emit(f"mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı")
            
            # === NİHAİ ÇALIŞAN TİP KONTROLÜ ===
            kullanilacak_format = "format_sayi" # Varsayılan SAYI

            if isinstance(node.ifade, ast_nodes.Metin):
                kullanilacak_format = "format_metin"
            
            elif isinstance(node.ifade, ast_nodes.Degisken):
                # Bu bloğa girildiğinde, değişkenin tipi kontrol edilir
                degisken_adi = node.ifade.ad
                sembol = self.generator.sembol_bul(degisken_adi) 
                
                if sembol is None:
                    raise Exception(f"Yazdırılamadı: '{degisken_adi}' tanımlı değil.")
                    
                # Tip kontrolü ve harf büyüklüğü düzeltmesi
                if 'tip' in sembol and sembol['tip'].upper() == 'METIN':
                    kullanilacak_format = "format_metin"

            self.emit(f"mov rdi, {kullanilacak_format}   ; Formatı RDI'a (1. arg) taşı")
        
        # --- 3. C Fonksiyonunu Çağır ---
        c_fonksiyon_adi = self.c_fonksiyonlari[islec_adi]
        self.emit("mov rax, 0  ; (SSE kuralı)")
        self.emit(f"call {c_fonksiyon_adi}")
        
    def generate_sozluk_getir(self, anahtar_node):
        """
        'call sozluk_getir' için C protokolünü üretir.
        """
        # ... (Bu metodun kalan kısmı değişmeden kalır)
        self.emit(f"; --- C Arayüzü: sozluk_getir ---")
        
        self.visit(anahtar_node) 
        self.emit(f"mov rdi, rax  ; Argüman 1 (Anahtar) -> rdi")
        
        self.emit("mov rax, 0")
        self.emit("call sozluk_getir")

    def generate_sozluk_ekle(self, anahtar_node, deger_node):
        """
        'call sozluk_ekle' için C protokolünü üretir.
        """
        # ... (Bu metodun kalan kısmı değişmeden kalır)
        self.emit(f"; --- C Arayüzü: sozluk_ekle ---")
        
        self.visit(anahtar_node) 
        self.emit(f"mov rdi, rax  ; Argüman 1 (Anahtar) -> rdi")

        self.visit(deger_node) 
        self.emit(f"mov rsi, rax  ; Argüman 2 (Değer) -> rsi")
        
        self.emit("mov rax, 0")
        self.emit("call sozluk_ekle")

# ... (Kalan metodlar)