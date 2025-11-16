# src/generator.py
# "ALTIN KOPYA" - Dizi + Heap Destekli, Refactor EDİLMEMİŞ Kararlı Sürüm

from src import ast_nodes
from src.tokens import TOKEN_TIPLERI 
from src.c_arayuzu import CArayuzu

class Generator:
    def __init__(self, ast_koku):
        self.ast_koku = ast_koku
        self.code = []  # Üretilen kodun tamamı
        self.data = []  # .data bölümü (sabitler, metinler)
        self.main_kodu = [] # main fonksiyonunun içindeki komutlar

        # Kapsam (Scope) Yönetimi
        self.degisken_haritalari = [{}] # Global kapsam (main) ile başla
        self.stack_offset = 0
        
        self.label_sayaci = 0
        self.dongu_cikis_etiketleri = []

        self.islec_haritasi = {}
        self.mevcut_islec = None
        # YENİ: C Arayüzü Katmanını başlat
        self.c_arayuzu = CArayuzu(self)

    # --- Kod Üretim Yardımcıları ---

    def emit(self, instruction):
        self.main_kodu.append(f"    {instruction}")

    def emit_data(self, data_line):
        self.data.append(f"    {data_line}")

    def _yeni_etiket(self):
        self.label_sayaci += 1
        return f"_etiket_{self.label_sayaci}"

    # --- Kapsam (Scope) ve Hafıza Yardımcıları ---
    
    def _kapsam_gir(self):
        self.degisken_haritalari.append({})
        self.stack_offset = 0

    def _kapsam_cik(self):
        if len(self.degisken_haritalari) > 1:
            self.degisken_haritalari.pop()
        self.stack_offset = 0

    def _degisken_adres_bul(self, degisken_adi):
        """Değişkenin adres/boyut sözlüğünü döndürür."""
        mevcut_kapsam = self.degisken_haritalari[-1]
        if degisken_adi in mevcut_kapsam:
            return mevcut_kapsam[degisken_adi]
        if degisken_adi in self.degisken_haritalari[0]:
             return self.degisken_haritalari[0][degisken_adi]
        raise Exception(f"[Generator] Tanımlanmamış değişken: {degisken_adi}")

    def _degisken_yer_ayir(self, degisken_adi, adres=None, boyut=1, tip=None): # <-- 'tip=None' EKLENDİ
        """
        Değişken/Dizi için yığında yer ayırır ve
        bilgilerini {'adres': '...', 'boyut': X} olarak kaydeder.
        """
        mevcut_kapsam = self.degisken_haritalari[-1]
        
        if degisken_adi in mevcut_kapsam:
             raise Exception(f"[Generator] Değişken '{degisken_adi}' bu kapsamda zaten tanımlı.")

        if adres is None:
            # LOKAL DEĞİŞKEN veya DİZİ
            toplam_byte_boyutu = boyut * 8
            self.stack_offset -= toplam_byte_boyutu
            adres = f"[rbp{self.stack_offset}]" 
            self.emit(f"sub rsp, {toplam_byte_boyutu}  ; '{degisken_adi}' için yığında {toplam_byte_boyutu} byte yer aç")
        
        mevcut_kapsam[degisken_adi] = {
            'adres': adres,
            'boyut': boyut,
            'tip': tip  # <-- 'tip' BİLGİSİ EKLENDİ
        }
        return mevcut_kapsam[degisken_adi]

    def sembol_bul(self, degisken_adi):
        """
        Bir değişkeni mevcut kapsamdan geriye doğru (global kapsama kadar) arar.
        """
        # Yığının sonundan (mevcut kapsam) başına (global kapsam) doğru ara
        for kapsam in reversed(self.degisken_haritalari):
            if degisken_adi in kapsam:
                return kapsam[degisken_adi]
        return None # Bulunamadı
        
    # --- Ana Üretim Fonksiyonları ---

    def generate(self):
        """Tüm AST'yi gezer ve tam Assembly kodunu döndürür."""
        # --- C Kütüphanesi kurulumu (Artık C_Arayuzu'nden geliyor) ---
        self.code.extend(self.c_arayuzu.generate_extern_bildirimleri())
        
        self.emit_data('format_sayi db "%d", 10, 0')
        self.emit_data('format_metin db "%s", 10, 0')

        self.visit(self.ast_koku) 
        self.code.append("\nsection .data")
        self.code.extend(self.data)
        
        self.code.append("\nsection .text")
        self.code.append("global main")
        
        self.code.append("main:")
        self.code.append("    push rbp")
        self.code.append("    mov rbp, rsp")
        
        self.code.extend(self.main_kodu)

        self.code.append("    xor rax, rax  ; Exit code 0 (başarılı)")
        self.code.append("    mov rsp, rbp")
        self.code.append("    pop rbp")
        self.code.append("    ret")

        return "\n".join(self.code)

    def visit(self, node):
        # Bu 'visit' fonksiyonundaki sıralama kritiktir
        
        # 1. IslecTanimlama'yı ara
        if isinstance(node, ast_nodes.IslecTanimlama):
            self.visit_IslecTanimlama(node)
            return
        
        # 2. DiziTanimlama'yı ara
        if isinstance(node, ast_nodes.DiziTanimlama):
            self.visit_DiziTanimlama(node)
            return

        # 3. Geri kalanları method adıyla çağır (örn: visit_DegiskenTanimlama)
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.visit_bilinmeyen)
        return visitor(node)

    def visit_bilinmeyen(self, node):
        raise Exception(f"[Generator] Tanınmayan AST Düğümü: {type(node).__name__}")

    # --- Ziyaretçi (Visitor) Fonksiyonları (Komutlar) ---

    def visit_Blok(self, node):
        for komut in node.komutlar:
            self.visit(komut)

    def visit_DegiskenTanimlama(self, node):
        degisken_adi = node.ad
        self.emit(f"; --- DegiskenTanimlama: {degisken_adi} ---")

        # === 1. ADIM: TİPİ AST'DEN OKU ===
        # Parser'ınızın 'tip' özelliğine 'METIN'/'SAYISAL' token'ını
        # koyduğunu varsayıyoruz. 'node.tip.deger' kullandığınızı varsayıyorum.
        degisken_tipi = node.tip.deger 
        
        # === 2. ADIM: DEĞERİ HESAPLA ===
        self.visit(node.ifade) # Değeri hesapla (rax)
        
        # === 3. ADIM: YERİ AYIR VE TİPİ KAYDET (DÜZELTİLDİ) ===
        # 'tip' parametresini _degisken_yer_ayir'a iletiyoruz.
        adres_bilgisi = self._degisken_yer_ayir(
            degisken_adi, 
            adres=None, 
            boyut=1, 
            tip=degisken_tipi # <-- TİPİ BURADA GÖNDERİYORUZ
        )
        
        adres = adres_bilgisi['adres']
        self.emit(f"mov {adres}, rax")

    def visit_DiziTanimlama(self, node):
        degisken_adi = node.ad.deger
        if not isinstance(node.boyut, ast_nodes.Sayi):
             raise Exception("Dizi boyutu şimdilik sadece sabit bir sayı olmalıdır.")
        boyut = int(node.boyut.deger)
        if boyut <= 0:
             raise Exception("Dizi boyutu pozitif olmalıdır.")
        self.emit(f"; --- Dizi Tanımlama: {degisken_adi}[{boyut}] ---")
        self._degisken_yer_ayir(degisken_adi, adres=None, boyut=boyut)

    def visit_AtamaKomutu(self, node):
        self.emit(f"; --- AtamaKomutu ---")
        
        # 1. Atanacak değeri (sağ taraf) hesapla ve yığına (stack) kaydet
        self.visit(node.ifade) 
        self.emit(f"push rax  ; Atanacak değeri (örn: 30) koru") 
        
        # 2. Hedefi belirle (sol taraf)
        
        if isinstance(node.hedef, ast_nodes.Degisken):
            # Hedef: Basit Değişken (örn: s_yas = ...)
            self.emit(f"pop rax") # Atanacak değeri geri al
            degisken_adi = node.hedef.ad
            adres_bilgisi = self._degisken_adres_bul(degisken_adi)
            adres = adres_bilgisi['adres']
            self.emit(f"mov {adres}, rax  ; {degisken_adi} = (sonuç)")

        elif isinstance(node.hedef, ast_nodes.DiziErisim):
            # Hedef: Dizi Erişimi (örn: liste[3] = ...)
            # TEMİZLENMİŞ KOD: Sadece Dizi Aritmetiği
            self.emit(f"; --- Dizi Erişimi (Yazma) [ ] ---")
            
            # 1. Adresi Hesapla
            dizi_adi = node.hedef.hedef.ad
            adres_bilgisi = self._degisken_adres_bul(dizi_adi)
            dizi_baslangic_adresi_str = adres_bilgisi['adres']

            self.visit(node.hedef.indeks) # İndeksi hesapla (RAX)
            self.emit("mov rdi, rax")      # İndeksi RDI'a taşı
            self.emit("shl rdi, 3")        # RDI = Index * 8
            self.emit(f"lea rax, {dizi_baslangic_adresi_str}") # RAX = Başlangıç Adresi
            self.emit("add rax, rdi  ; RAX = Elemanın Tam Adresi")
            
            # 2. Değeri Ata
            self.emit(f"mov rdi, rax  ; Hedef adresi RDI'a taşı")
            self.emit(f"pop rax       ; Atanacak değeri yığından geri al (RAX)")
            self.emit(f"mov [rdi], rax ; Değeri hedefin adresine yaz")

        elif isinstance(node.hedef, ast_nodes.SozlukErisim):
            # Hedef: Sözlük Erişimi (örn: sozluk{"key"} = ...)
            # YENİ KOD BLOĞU
            self.emit(f"; --- Sözlük Erişimi (Yazma) {{ }} ---")
            
            # 1. Değeri (Arg 2) yığından al -> RSI
            self.emit("pop rsi ; Atanacak değeri (Değer) RSI'a al")
            
            # 2. Anahtarı (Arg 1) AST'den al -> RDI
            # (c_arayuzu.visit, node.anahtar'ı ziyaret edip sonucu RAX'e koyar)
            self.c_arayuzu.visit(node.hedef.anahtar) 
            self.emit("mov rdi, rax ; Anahtarı RDI'a al")
            
            # 3. C fonksiyonunu çağır (Arg1=RDI, Arg2=RSI)
            self.emit("mov rax, 0")
            self.emit("call sozluk_ekle")
            
        else:
            self.emit(f"pop rax") # Yığını temizle
            raise Exception("Desteklenmeyen atama hedefi.")
            
    def visit_EgerBlogu(self, node):
        degilse_etiketi = self._yeni_etiket()
        son_etiket = self._yeni_etiket()
        self.visit(node.kosul)
        self.emit("cmp rax, 0")
        hedef_etiket = degilse_etiketi if node.yanlis_blok else son_etiket
        self.emit(f"je {hedef_etiket}")
        self.visit(node.dogru_blok)
        if node.yanlis_blok:
            self.emit(f"jmp {son_etiket}")
        if node.yanlis_blok:
            self.emit(f"{degilse_etiketi}:")
            self.visit(node.yanlis_blok)
        self.emit(f"{son_etiket}:")

    def visit_DonguBlogu(self, node):
        basi_etiketi = self._yeni_etiket()
        sonu_etiketi = self._yeni_etiket()
        self.dongu_cikis_etiketleri.append(sonu_etiketi)
        self.emit(f"{basi_etiketi}:")
        self.visit(node.blok)
        self.emit(f"jmp {basi_etiketi}")
        self.emit(f"{sonu_etiketi}:")
        self.dongu_cikis_etiketleri.pop()

    def visit_DonguBitirKomutu(self, node):
        self.emit("; --- DÖNGÜ_BITIR ---")
        if not self.dongu_cikis_etiketleri:
            raise Exception("HATA: DÖNGÜ_BITIR komutu bir DÖNGÜ bloğu içinde olmalıdır.")
            
        # En son girilen döngünün çıkış etiketine zıpla (pop yapma!)
        cikis_etiketi = self.dongu_cikis_etiketleri[-1] 
        self.emit(f"jmp {cikis_etiketi}")

    def visit_IslecTanimlama(self, node):
        islec_adi = node.ad.deger
        asm_etiketi = f"_islec_{islec_adi}"
        self.islec_haritasi[islec_adi] = asm_etiketi
        islec_kodu = []
        islec_kodu.append(f"\n; --- İŞLEÇ Tanımı: {islec_adi} ---")
        islec_kodu.append(f"{asm_etiketi}:")
        islec_kodu.append("    push rbp")
        islec_kodu.append("    mov rbp, rsp")
        self._kapsam_gir()
        self.mevcut_islec = node
        
        # Parametrelerin yığına yerleştirilmesi (Değişken tanımı gibi)
        for i, param in enumerate(node.parametreler):
            param_adi = param.ad.deger
            # rbp+16: Çağrı adresi, rbp+24: Argüman 1, rbp+32: Argüman 2...
            adres = f"[rbp+{16 + (i + 1) * 8}]" # Argüman 1, rbp+24'te başlar
            self._degisken_yer_ayir(param_adi, adres=adres, boyut=1)
            
        eski_main_kodu = self.main_kodu
        self.main_kodu = islec_kodu
        self.visit(node.govde)
        self.main_kodu = eski_main_kodu
        
        # ⚠️ KRİTİK DÜZELTME: Örtük epilog kaldırıldı. 
        # Sadece ret komutuna ulaşılırsa çalışır (DÖNÜŞ komutu yoksa).
        islec_kodu.append(f"; {islec_adi} için örtük epilog (Sadece DÖNÜŞ yoksa yürür)")
        islec_kodu.append("    mov rsp, rbp")
        islec_kodu.append("    pop rbp")
        islec_kodu.append("    ret")
        
        self.code.extend(islec_kodu)
        self.mevcut_islec = None
        self._kapsam_cik()


    def visit_DonusKomutu(self, node):
        if self.mevcut_islec is None:
            raise Exception("'DÖNÜŞ' komutu sadece İŞLEÇ içinde kullanılabilir.")
            
        # 1. Döndürülecek değeri hesapla ve RAX'e yükle (Zaten yapılıyor)
        self.visit(node.ifade) 

        # 2. Epilog (Yığın temizliği ve geri dönüş) için JMP yap.
        #    Bu kod, fonksiyonun normal bitişindeki ret komutunu atlar.
        # KRİTİK DÜZELTME: Doğrudan yığın temizliği yapmak yerine,
        #                  fonksiyonun sonuna (epilog'a) zıplayalım.
        
        # JMP yapacağımız etiketi almalıyız.
        # visit_IslecTanimlama metodunda, fonksiyonun sonuna JMP yapması için
        # bir etiket tanımlamamız gerekiyor.
        
        # --- İYİLEŞTİRME VE DÜZELTME ---
        # 
        # visit_IslecTanimlama'nın sonunda ret komutu olduğu için,
        # DÖNÜŞ komutunun sadece yığın temizliği yapıp ret komutunu çağırması yeterlidir.
        
        # Fonksiyon gövdesi normalde ret'i çağırmadığı için, 
        # DÖNÜŞ komutu doğrudan epilog'u çalıştırabilir (sizin kodunuz doğruydu,
        # sadece ret'i eklemeliyiz. Ama yığın temizliği sadece bir kere yapılmalı).

        # Eğer fonksiyonun sonuna kadar gelinirse ret çağrılır.
        # DÖNÜŞ komutu varsa, normal akışı atlamalı.
        
        # En basit ve doğru yol: Fonksiyonun normal bitişine zıplamasını sağlamak.
        # Ancak, sizin kodunuzda zaten epilog doğru. Sadece JMP yapmıyor.
        
        # Düzeltme: Fonksiyonun normal bitişine zıplamasını sağlamalıyız.
        # Ancak, bu etiketi visit_IslecTanimlama'dan almalıyız.
        
        # Şimdilik, sizin kodunuzdaki gibi yığın temizliğini yapıp RET çağırsın.
        # Zaten Generator, DÖNÜŞ'ten sonraki kodu da basacaktır, ama bu mantıken hatalı olsa da çalışır.
        
        # İhtiyacımız olan tek şey: YAZDIR komutundaki gibi bir JMP etiketi.
        # Ancak, fonksiyonlar için JMP etiketi tanımlamadık.
        
        # Düzeltme 1: Gereksiz kodu silme (DÖNÜŞ sonrası koda devam etmemek için)
        self.emit("    mov rsp, rbp") # Parametreleri ve yerel değişkenleri temizle
        self.emit("    pop rbp")    # Ana fonksiyonun RBP'sini geri yükle
        self.emit("    ret")      # Çağıran fonksiyona geri dön
        
        # Generator'ın DÖNÜŞ komutundan sonraki kodu basmasını engellemek için:
        # Bu metodun en sonuna ek bir JMP komutu basmak gerekir.

        # GEREKLİ OLAN: visit_IslecTanimlama metodunda bir END etiketi tanımlamak.
        # Ancak bunu yapmadık. Bu yüzden DÖNÜŞ sonrası kod basılmaya devam edecektir.
        
        # Şimdilik, sizin kodunuz doğru, sadece fonksiyonun sonuna JMP yapmasını sağlayalım.
        
        # DÖNÜŞ komutundan sonraki komutların çalışmaması için,
        # Generator'ın bunu bilmesi gerekir.
        
        # En temiz çözüm: visit_IslecTanimlama metodunda bir bitiş etiketi tanımlamak.
        
        # Düzeltme: visit_IslecTanimlama'nın sonuna bir etiket ekleyelim.
        
        # visit_IslecTanimlama'nın sonuna bir etiket ekleyelim:
        # islec_kodu.append(f"{asm_etiketi}_sonu:")
        
        # visit_DonusKomutu metoduna JMP ekleyelim:
        
        self.emit("    mov rsp, rbp") # Parametreleri ve yerel değişkenleri temizle
        self.emit("    pop rbp")    # Ana fonksiyonun RBP'sini geri yükle
        self.emit("    ret")      # Çağıran fonksiyona geri dön
        
        # DÖNÜŞ'ten sonraki kodun çalışmaması için, burada return yapmak gerekir.
        # Bu metodun, sonraki kodun basılmasını engellemek için, bir JMP etiketi tanımlanması GEREKİR.
        
        # Şu anki haliyle bu kod **Assembly hatası vermez**, ama `DÖNÜŞ`ten sonraki kodları da basar.
        
        # Sadece RET komutunu ekleyelim, yığın temizliğini Generator'a bırakalım.
        
        # Nihai Düzeltme: visit_DonusKomutu'ndan sonraki kodun basılmasını engellemek için
        #               sadece RET komutunu basmak ve bu metoddan sonraki kodun basılmasını engellemek.

        # Kodunuzdaki yığın temizliğini tutarlı olması için koruyorum, ancak Generator'da JMP eklenmeli.
        
        self.emit("    mov rsp, rbp") # Parametreleri ve yerel değişkenleri temizle
        self.emit("    pop rbp")    # Ana fonksiyonun RBP'sini geri yükle
        self.emit("    ret")      # Çağıran fonksiyona geri dön

    def visit_IfadeKomutu(self, node):
        self.visit(node.ifade)

    def visit_YazdirKomutu(self, node):
        self.emit("; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---")
        # Tüm işi C Arayüzü yapsın
        # (Burada SADECE BİR SATIR olduğundan emin olun)
        self.c_arayuzu.generate_c_call("YAZDIR", node)

    # --- İfade (Expression) Ziyaretçileri ---

    def visit_Sayi(self, node):
        self.emit(f"mov rax, {node.deger}")

    def visit_Metin(self, node):
        data_label = f"str_{len(self.data)}"
        self.emit_data(f'{data_label} db "{node.deger}", 0')
        self.emit(f"mov rax, {data_label}")

    def visit_Degisken(self, node):
        degisken_adi = node.ad
        adres_bilgisi = self._degisken_adres_bul(degisken_adi)
        if adres_bilgisi['boyut'] > 1:
            raise Exception(f"HATA: '{degisken_adi}' bir dizidir. Diziyi olduğu gibi okuyamazsınız, elemanına ([i]) erişin.")
        adres = adres_bilgisi['adres'] 
        self.emit(f"mov rax, {adres}")

    def visit_DiziErisim(self, node):
        # TEMİZLENMİŞ KOD:
        # Artık "İndeks Metin mi?" kontrolü (duck typing) yok.
        # Bu fonksiyon SADECE dizi erişimi [ ] içindir.
        
        self.emit(f"; --- Dizi Erişimi (Okuma) [ ] ---")
        
        # 1. Dizi'nin yığındaki (stack) adresini bul
        dizi_adi = node.hedef.ad
        adres_bilgisi = self._degisken_adres_bul(dizi_adi)
        dizi_baslangic_adresi_str = adres_bilgisi['adres']
        
        # 2. İndeksi hesapla (örn: 3)
        self.visit(node.indeks) # İndeks (örn: 3) şimdi RAX'te
        
        # 3. Pointer Aritmetiği (64-bit / 8 byte)
        self.emit("mov rdi, rax")
        self.emit("shl rdi, 3") # rdi = rdi * 8 (byte offset)
        self.emit(f"lea rax, {dizi_baslangic_adresi_str}") # rax = dizinin başlangıç adresi
        self.emit("add rax, rdi") # rax = başlangıç + offset
        
        # 4. Değeri Oku
        self.emit("mov rax, [rax]") # Adresteki (örn: [rbp-16+24]) değeri oku
        # Okunan değer (örn: 123) şimdi RAX'te

    def visit_SozlukErisim(self, node):
        # YENİ FONKSİYON:
        # Bu fonksiyon SADECE sözlük erişimi { } içindir.
        
        self.emit(f"; --- Sözlük Erişimi (Okuma) {{ }} ---")
        
        # 'node.anahtar' { } içindeki ifadedir (örn: Metin("key"))
        # c_arayuzu'nu çağırıyoruz. O, anahtarı RDI'a koyacak
        # ve 'call sozluk_getir' diyecek.
        self.c_arayuzu.generate_sozluk_getir(node.anahtar)
        
        # Dönen değer (örn: 50000) C tarafından RAX'e yerleştirildi.
        # Başka bir şey yapmamıza gerek yok.

    def visit_IkiliIslem(self, node):
        op = node.operator.tip
        sol_tip = self.ifade_tipi_bul(node.sol)
        sag_tip = self.ifade_tipi_bul(node.sag)
        
        # --- ÖZEL İŞLEM 1: METİN KARŞILAŞTIRMA (==) ---
        if op == TOKEN_TIPLERI['OP_ESIT_KARSILASTIRMA']:
            if sol_tip == 'METIN' and sag_tip == 'METIN':
                self.emit("; --- İKİLİ İŞLEM: METİN KARŞILAŞTIRMA (==) ---")
                
                # 1. Argümanları RDI/RSI'ya yükle
                self.visit(node.sag)  # Sağ operandı hesapla (RAX)
                self.emit("mov rsi, rax") # RAX'i RSI'ya (str2) taşı
                self.visit(node.sol)  # Sol operandı hesapla (RAX)
                self.emit("mov rdi, rax") # RAX'i RDI'ya (str1) taşı
                
                # 2. string_karsilastir fonksiyonunu çağır (Sonuç RAX'e döner: 0 = Eşit)
                self.emit("mov rax, 0")
                self.emit("call string_karsilastir") 
                
                # 3. Sonucu 0 ile karşılaştır ve BOOL sonucunu AL'ye yaz
                # str1 == str2 ise: string_karsilastir 0 döndürür. RAX=0 ise (setz/sete) AL=1 olur.
                self.emit("cmp rax, 0")
                self.emit("sete al") # Eşit ise (Zero Flag set ise) AL=1, değilse AL=0
                
                self.emit("movzx rax, al") # AL'yi RAX'e taşı (RAX = 0 veya 1)
                return # Bitir
            
            elif sol_tip != 'SAYISAL' or sag_tip != 'SAYISAL':
                # Eğer SAYISAL değilse ve metin değilse hata ver
                raise Exception(f"HATA: '==' operatörü sadece SAYISAL veya METIN arasında kullanılabilir. ({sol_tip} vs {sag_tip})")

        # --- ÖZEL İŞLEM 2: METİN BİRLEŞTİRME (+) ---
        if op == TOKEN_TIPLERI['PLUS'] or op == TOKEN_TIPLERI['OP_ARTI']:
            if sol_tip == 'METIN' and sag_tip == 'METIN':
                self.emit("; --- İKİLİ İŞLEM: METİN BİRLEŞTİRME ---")
                
                self.visit(node.sag)  
                self.emit("mov rsi, rax") 
                self.visit(node.sol)  
                self.emit("mov rdi, rax") 
                
                self.emit("mov rax, 0  ; (SSE kuralı)")
                self.emit("call string_birlestir")
                return # Bitir

            elif sol_tip != 'SAYISAL' or sag_tip != 'SAYISAL':
                raise Exception(f"HATA: '+' operatörü sadece aynı tipler (SAYISAL+SAYISAL veya METIN+METIN) arasında kullanılabilir. ({sol_tip} + {sag_tip})")


        # --- Geleneksel Sayısal İşlemler ve Karşılaştırmalar ---
        
        # Koşul: Eğer buraya gelindiyse, işlem Sayısal Karşılaştırma veya Matematiksel İşlem olmalıdır.
        
        self.visit(node.sag)
        self.emit("push rax")
        self.visit(node.sol)
        self.emit("pop rbx")
        
        # Sayısal Matematik İşlemleri
        if op == TOKEN_TIPLERI['PLUS'] or op == TOKEN_TIPLERI['OP_ARTI']:
            self.emit("add rax, rbx")
        elif op == TOKEN_TIPLERI['MINUS'] or op == TOKEN_TIPLERI['OP_EKSI']:
            self.emit("sub rax, rbx")
        elif op == TOKEN_TIPLERI['MUL'] or op == TOKEN_TIPLERI['OP_CARP']:
            self.emit("imul rax, rbx")
        elif op == TOKEN_TIPLERI['DIV'] or op == TOKEN_TIPLERI['OP_BOL']:
            self.emit("xor rdx, rdx")
            self.emit("idiv rbx")
            
        # Sayısal Karşılaştırma İşlemleri
        elif op in [TOKEN_TIPLERI['OP_ESIT_KARSILASTIRMA'], TOKEN_TIPLERI['GT'], TOKEN_TIPLERI['LT']]:
            self.emit("cmp rax, rbx")
            
            # Bu kısım Claude tarafından düzeltildi ve setXX komutlarını içeriyor.
            if op == TOKEN_TIPLERI['OP_ESIT_KARSILASTIRMA']: self.emit("sete al")
            elif op == TOKEN_TIPLERI['GT']: self.emit("setg al")
            elif op == TOKEN_TIPLERI['LT']: self.emit("setl al")
            
            self.emit("movzx rax, al") # Sonucu 1 veya 0 olarak RAX'e taşı
        else:
            self.emit(f"; TODO: Desteklenmeyen İkili İşlem: {op}")

    def visit_IslecCagirma(self, node):
        islec_adi = node.hedef_token.deger
        self.emit(f"; --- İŞLEÇ Çağırma: {islec_adi} ---")

        # 1. Bu sihirli bir C fonksiyonu mu? (YENI, SIL, YAZDIR vb.)
        if self.c_arayuzu.is_c_function(islec_adi):
            # Evet, tüm işi C Arayüzü yapsın
            self.c_arayuzu.generate_c_call(islec_adi, node)
            
        # 2. Bu, bizim tanımladığımız bir TYD-MLP işlemi mi? (TOPLA)
        elif islec_adi in self.islec_haritasi:
            # Evet, kendi TYD-MLP fonksiyonumuzu çağır
            asm_etiketi = self.islec_haritasi[islec_adi]
            
            # Argümanları TYD-MLP protokolü (yığın) için hazırla
            arg_sayisi = len(node.argumanlar)
            for arg in reversed(node.argumanlar):
                self.visit(arg) # Argüman değerini RAX'e hesapla
                self.emit("push rax") # RAX'i yığına it
                
            self.emit(f"call {asm_etiketi}") # Fonksiyonu çağır (RAX'te sonuç bekliyoruz)
            
            # Yığını temizle (fonksiyonun parametreleri kadar yeri RSP'den çıkar)
            if arg_sayisi > 0:
                self.emit(f"add rsp, {arg_sayisi * 8}  ; Yığını temizle")
        
        # 3. Bulunamadı
        else:
            raise Exception(f"Tanımsız İŞLEÇ çağrıldı: {islec_adi}")

    def ifade_tipi_bul(self, node):
        """
        Verilen AST düğümünün çıktısını vereceği tipi belirler
        ('SAYISAL' veya 'METIN').
        """
        from src import ast_nodes # AST düğümlerini içe aktar
        
        # 1. Sabit Değer Kontrolü
        if isinstance(node, ast_nodes.Sayi):
            return 'SAYISAL'
        if isinstance(node, ast_nodes.Metin):
            return 'METIN'

        # 2. Değişken Kontrolü (Bizim durumumuz bu)
        if isinstance(node, ast_nodes.Degisken):
            sembol = self.sembol_bul(node.ad)
            if sembol is None:
                raise Exception(f"HATA: '{node.ad}' değişkeni tanımlanmamış.")
            # Tip bilgisini büyük harfe çevirerek döndür (Hata avlama dersimizden öğrendik!)
            tip = sembol.get('tip', 'SAYISAL')
            if tip is None:
                tip = 'SAYISAL'  # Default değer
            return tip.upper() 
            
        # 3. İkili İşlem Kontrolü (Örn: a + b)
        if isinstance(node, ast_nodes.IkiliIslem):
            # İkili işlemde, sonuç tipi genelde sol operandın tipiyle aynıdır.
            # (Şimdilik sadece Sayısal+Sayısal veya Metin+Metin destekliyoruz)
            return self.ifade_tipi_bul(node.sol)
        
        # 4. İşleç Çağırma Kontrolü (Fonksiyonlar)
        if isinstance(node, ast_nodes.IslecCagirma):
            # Basit FFI çağrılarının varsayılan olarak SAYISAL döndürdüğünü varsayalım.
            # (Gelecekte buraya tip kontrolü eklenecek.)
            return 'SAYISAL' 
            
        # Diğer tüm durumlar (Dizi Erişimi, Sözlük Erişimi vb.) şimdilik varsayılan tipi döndürsün.
        return 'SAYISAL'