# src/generator.py
# "ALTIN KOPYA" - Dizi + Heap Destekli, Refactor EDİLMEMİŞ Kararlı Sürüm

from src import ast_nodes
from src.tokens import TOKEN_TIPLERI

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

    def _degisken_yer_ayir(self, degisken_adi, adres=None, boyut=1):
        """
        Değişken/Dizi için yığında yer ayırır ve
        bilgilerini {'adres': '...', 'boyut': X} olarak kaydeder.
        """
        mevcut_kapsam = self.degisken_haritalari[-1]
        
        # Bu, hatanın kaynağıydı. Bu kontrolü ekliyoruz.
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
            'boyut': boyut
        }
        return mevcut_kapsam[degisken_adi]

    # --- Ana Üretim Fonksiyonları ---

    def generate(self):
        """Tüm AST'yi gezer ve tam Assembly kodunu döndürür."""
        
        self.code.append("extern printf")
        self.code.append("extern malloc")
        self.code.append("extern free")
        
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
        self.visit(node.ifade) # Değeri hesapla (rax)
        adres_bilgisi = self._degisken_yer_ayir(degisken_adi, adres=None, boyut=1)
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
        self.visit(node.ifade) # Atanacak değeri hesapla (rax)
        self.emit(f"push rax") 
        
        if isinstance(node.hedef, ast_nodes.DiziErisim):
            self.emit(f"; --- Dizi Erişimi (Yazma) ---")
            dizi_adi = node.hedef.hedef.ad
            adres_bilgisi = self._degisken_adres_bul(dizi_adi)
            dizi_baslangic_adresi_str = adres_bilgisi['adres']
            self.visit(node.hedef.indeks)
            self.emit("mov rdi, rax")
            self.emit("shl rdi, 3")
            self.emit(f"lea rax, {dizi_baslangic_adresi_str}")
            self.emit("add rax, rdi")
            self.emit(f"mov rdi, rax")
            self.emit(f"pop rax")
            self.emit(f"mov [rdi], rax")
        elif isinstance(node.hedef, ast_nodes.Degisken):
            self.emit(f"pop rax")
            degisken_adi = node.hedef.ad
            adres_bilgisi = self._degisken_adres_bul(degisken_adi)
            adres = adres_bilgisi['adres']
            self.emit(f"mov {adres}, rax")
        else:
            self.emit(f"pop rax")
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
        if not self.dongu_cikis_etiketleri:
            raise Exception("[Generator] 'DÖNGÜ_BITIR' komutu bir 'DÖNGÜ' bloğu içinde olmalıdır.")
        self.emit(f"jmp {self.dongu_cikis_etiketleri[-1]}")

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
        for i, param in enumerate(node.parametreler):
            param_adi = param.ad.deger
            adres = f"[rbp+{16 + i * 8}]"
            self._degisken_yer_ayir(param_adi, adres=adres, boyut=1)
        eski_main_kodu = self.main_kodu
        self.main_kodu = islec_kodu
        self.visit(node.govde)
        self.main_kodu = eski_main_kodu
        islec_kodu.append(f"; {islec_adi} için örtük epilog")
        islec_kodu.append("    mov rsp, rbp")
        islec_kodu.append("    pop rbp")
        islec_kodu.append("    ret")
        self.code.extend(islec_kodu)
        self.mevcut_islec = None
        self._kapsam_cik()

    def visit_DonusKomutu(self, node):
        if self.mevcut_islec is None:
            raise Exception("'DÖNÜŞ' komutu sadece İŞLEÇ içinde kullanılabilir.")
        self.visit(node.ifade)
        self.emit("    mov rsp, rbp")
        self.emit("    pop rbp")
        self.emit("    ret")

    def visit_IfadeKomutu(self, node):
        self.visit(node.ifade)

    def visit_YazdirKomutu(self, node):
        self.emit("; --- YAZDIR Başlangıç ---")
        self.visit(node.ifade)
        self.emit("mov rsi, rax") 
        if isinstance(node.ifade, ast_nodes.Metin):
            self.emit("mov rdi, format_metin")
        else:
            self.emit("mov rdi, format_sayi")
        self.emit("mov rax, 0")
        self.emit("call printf")

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
        self.emit(f"; --- Dizi Erişimi (Okuma) ---")
        dizi_adi = node.hedef.ad
        adres_bilgisi = self._degisken_adres_bul(dizi_adi)
        dizi_baslangic_adresi_str = adres_bilgisi['adres']
        self.visit(node.indeks)
        self.emit("mov rdi, rax")
        self.emit("shl rdi, 3")
        self.emit(f"lea rax, {dizi_baslangic_adresi_str}")
        self.emit("add rax, rdi")
        self.emit("mov rax, [rax]")

    def visit_IslecCagirma(self, node):
        islec_adi = node.hedef_token.deger
        self.emit(f"; --- İŞLEÇ Çağırma: {islec_adi} ---")
        arg_sayisi = len(node.argumanlar)
        for arg in reversed(node.argumanlar):
            self.visit(arg)
            self.emit("push rax")
        if arg_sayisi > 0 and (islec_adi == "YENI" or islec_adi == "SIL"):
             self.emit("pop rdi")
        if islec_adi == "YENI":
            self.emit("mov rax, 0")
            self.emit("call malloc")
        elif islec_adi == "SIL":
            self.emit("mov rax, 0")
            self.emit("call free")
        elif islec_adi in self.islec_haritasi:
            asm_etiketi = self.islec_haritasi[islec_adi]
            self.emit(f"call {asm_etiketi}")
            if arg_sayisi > 0:
                self.emit(f"add rsp, {arg_sayisi * 8}")
        else:
            raise Exception(f"Tanımsız İŞLEÇ çağrıldı: {islec_adi}")

    def visit_IkiliIslem(self, node):
        self.visit(node.sag)
        self.emit("push rax")
        self.visit(node.sol)
        self.emit("pop rbx")
        op = node.operator.tip
        if op == TOKEN_TIPLERI['PLUS'] or op == TOKEN_TIPLERİ['OP_ARTI']:
            self.emit("add rax, rbx")
        elif op == TOKEN_TIPLERI['MINUS'] or op == TOKEN_TIPLERİ['OP_EKSI']:
            self.emit("sub rax, rbx")
        elif op == TOKEN_TIPLERI['MUL'] or op == TOKEN_TIPLERİ['OP_CARP']:
            self.emit("imul rax, rbx")
        elif op == TOKEN_TIPLERI['DIV'] or op == TOKEN_TIPLERİ['OP_BOL']:
            self.emit("xor rdx, rdx")
            self.emit("idiv rbx")
        elif op in [TOKEN_TIPLERI['OP_ESIT'], TOKEN_TIPLERI['OP_GT'], TOKEN_TIPLERI['OP_LT']]:
            self.emit("cmp rax, rbx")
            if op == TOKEN_TIPLERI['OP_ESIT']: self.emit("sete al")
            elif op == TOKEN_TIPLERI['OP_GT']: self.emit("setg al")
            elif op == TOKEN_TIPLERI['OP_LT']: self.emit("setl al")
            self.emit("movzx rax, al")
        else:
            self.emit(f"; TODO: Desteklenmeyen İkili İşlem: {op}")