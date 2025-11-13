# src/parser.py

from src.tokens import TOKEN_TIPLERI
from src import ast_nodes

class Parser:
    """
    Lexer'dan gelen Token listesini alır ve AST (Soyut Sözdizimi Ağacı) oluşturur.
    """
    def __init__(self, lexer):
        self.lexer = lexer
        self.mevcut_token = self.lexer.sonraki_tokeni_al() # İlk token'ı alarak başla

    def hata(self, mesaj):
        """Parser hatası fırlatır."""
        raise Exception(f"[Sözdizimi Hatası] {mesaj}. Bulunan: {self.mevcut_token}")

    def tuket(self, token_tipi):
        """
        Mevcut token beklenen tipteyse bir sonrakine geçer.
        Değilse hata verir.
        """
        if self.mevcut_token.tip == token_tipi:
            self.mevcut_token = self.lexer.sonraki_tokeni_al()
        else:
            self.hata(f"Beklenen token: {token_tipi}")

    # --- Gramer Kuralları ---

    def faktor(self):
        """
        En yüksek öncelik: Sayılar, Metinler, Değişkenler, Parantezler, 
        İşleç Çağırma, Dizi Erişimi VE SÖZLÜK ERİŞİMİ.
        """
        token = self.mevcut_token
        
        if token.tip == TOKEN_TIPLERI['INTEGER'] or token.tip == TOKEN_TIPLERI['FLOAT']:
            self.tuket(token.tip)
            return ast_nodes.Sayi(token)
        elif token.tip == TOKEN_TIPLERI['STRING']:
            self.tuket(TOKEN_TIPLERI['STRING'])
            return ast_nodes.Metin(token)
            
        elif token.tip == TOKEN_TIPLERI['IDENTIFIER']:
            self.tuket(TOKEN_TIPLERI['IDENTIFIER']) # 'd', 'liste' veya 'topla'
            
            if self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_PAREN']:
                # Bu bir İşleç Çağırma (topla())
                return self.islec_cagirma(token)
            
            elif self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_BRACKET']:
                # Bu bir Dizi Erişimi (liste[])
                return self.dizi_erisimi(token)

            # --- YENİ EKLENEN BLOK ---
            elif self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_BRACE']:
                # Bu bir Sözlük Erişimi (d{})
                return self.sozluk_erisimi(token)
            # --- YENİ BLOK BİTİŞİ ---

            else:
                # Bu normal bir Değişken Referansı
                return ast_nodes.Degisken(token)
                
        elif token.tip == TOKEN_TIPLERI['LEFT_PAREN']:
            self.tuket(TOKEN_TIPLERI['LEFT_PAREN'])
            dugum = self.ifade()
            self.tuket(TOKEN_TIPLERI['RIGHT_PAREN'])
            return dugum
        
        self.hata(f"Beklenmedik ifade başlangıcı: {token.tip}")

    def islec_cagirma(self, hedef_token):
        """
        Bir işleç çağrısının argümanlarını ayrıştırır.
        Gramer: '(' [ifade (COMMA ifade)*]? ')'
        'hedef_token' (örn: 'topla') zaten 'faktor' tarafından tüketildi.
        """
        argumanlar = []
        self.tuket(TOKEN_TIPLERI['LEFT_PAREN']) # '('
        
        # Eğer ilk token ')' değilse, argümanlar var demektir
        if self.mevcut_token.tip != TOKEN_TIPLERI['RIGHT_PAREN']:
            # İlk argüman
            # Bir argüman, '5 + 2' gibi karmaşık bir ifade olabilir!
            argumanlar.append(self.ifade()) 
            
            # Kalan argümanlar (virgülle ayrılmış)
            while self.mevcut_token.tip == TOKEN_TIPLERI['COMMA']:
                self.tuket(TOKEN_TIPLERI['COMMA']) # ','
                argumanlar.append(self.ifade())
                
        self.tuket(TOKEN_TIPLERI['RIGHT_PAREN']) # ')'
        
        return ast_nodes.IslecCagirma(hedef_token, argumanlar)

    # src/parser.py

    def dizi_erisimi(self, hedef_token):
        """
        Gramer: '[' ifade ']'
        'hedef_token' (örn: 'liste') zaten 'faktor' tarafından tüketildi.
        """
        self.tuket(TOKEN_TIPLERI['LEFT_BRACKET']) # '['
        
        # İndeks, '5 + 2' gibi karmaşık bir ifade olabilir!
        indeks_ifadesi = self.ifade() 
        
        self.tuket(TOKEN_TIPLERI['RIGHT_BRACKET']) # ']'
        
        hedef_node = ast_nodes.Degisken(hedef_token)
        return ast_nodes.DiziErisim(hedef_node, indeks_ifadesi)

    def terim(self):
        """
        Orta öncelik: Çarpma ve Bölme.
        Gramer: faktor ((MUL | DIV) faktor)*
        (SADECE BİR TANE OLMALI)
        """
        sol_dugum = self.faktor() # İlk sayıyı al (örn: 5)

        # Ardından * veya / geldiği sürece devam et (örn: * 2 / 4 ...)
        while self.mevcut_token.tip in [TOKEN_TIPLERI['MUL'], TOKEN_TIPLERI['DIV'], TOKEN_TIPLERI['POWER']]:

            operator = self.mevcut_token
            self.tuket(operator.tip) # Operatörü tüket (* veya /)
            sag_dugum = self.faktor() # Operatörden sonraki sayıyı al (örn: 2)
            
            # Sol düğümü yeni bir işlem düğümüyle güncelle: (eski_sol * sag)
            sol_dugum = ast_nodes.IkiliIslem(sol_dugum, operator, sag_dugum)
            
        return sol_dugum

    def ifade(self):
        """
        En düşük öncelikli işlemler: Toplama, Çıkarma VE Karşılaştırma.
        Gramer: terim ((PLUS | MINUS | OP_ESIT | GT | LT ...) terim)*
        (SADECE BİR TANE OLMALI - BU EN GÜNCEL HALİ)
        """
        sol_dugum = self.terim() # İlk terimi al (örn: s) 

        # GÜNCELLENMİŞ 'while' KONTROLÜ
        while self.mevcut_token.tip in [
            TOKEN_TIPLERI['PLUS'], TOKEN_TIPLERI['MINUS'],
            TOKEN_TIPLERI['OP_ESIT_KARSILASTIRMA'],
            TOKEN_TIPLERI['GT'], TOKEN_TIPLERI['LT'],
            TOKEN_TIPLERI['GTE'], TOKEN_TIPLERI['LTE']
        ]:
            operator = self.mevcut_token
            self.tuket(operator.tip) 
            sag_dugum = self.terim() 
            sol_dugum = ast_nodes.IkiliIslem(sol_dugum, operator, sag_dugum)
            
        return sol_dugum

    # src/parser.py (içine)

    # def atom(self):
    #     """
    #     Bir ifadenin en küçük "atomik" parçasını ayrıştırır.
    #     (Sayı, Metin, Değişken, Dizi/Sözlük Erişimi, Fonksiyon Çağrısı, Parantezli İfade)
    #     """
    #     token = self.mevcut_token

    #     # 1. Sayılar (INTEGER, FLOAT)
    #     if token.tip == TOKEN_TIPLERI['INTEGER']:
    #         self.tuket(TOKEN_TIPLERI['INTEGER'])
    #         return ast_nodes.Sayi(token)
        
    #     if token.tip == TOKEN_TIPLERI['FLOAT']:
    #         self.tuket(TOKEN_TIPLERI['FLOAT'])
    #         return ast_nodes.Sayi(token) # Sayi düğümü her ikisini de işler

    #     # 2. Metin (STRING)
    #     if token.tip == TOKEN_TIPLERI['STRING']:
    #         self.tuket(TOKEN_TIPLERI['STRING'])
    #         return ast_nodes.Metin(token)

    #     # 3. Parantezli Grup ( (ifade) )
    #     if token.tip == TOKEN_TIPLERI['LEFT_PAREN']:
    #         self.tuket(TOKEN_TIPLERI['LEFT_PAREN'])
    #         node = self.ifade() # Parantez içindeki ifadeyi ayrıştır
    #         self.tuket(TOKEN_TIPLERI['RIGHT_PAREN'])
    #         return node

    #     # 4. Değişken, Dizi, Sözlük veya Fonksiyon Çağrısı
    #     if token.tip == TOKEN_TIPLERI['IDENTIFIER']:
    #         hedef_token = self.mevcut_token
    #         self.tuket(TOKEN_TIPLERI['IDENTIFIER'])

    #         # Dizi Erişimi mi? [ ]
    #         if self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_BRACKET']:
    #             self.tuket(TOKEN_TIPLERI['LEFT_BRACKET'])
    #             indeks = self.ifade()
    #             self.tuket(TOKEN_TIPLERI['RIGHT_BRACKET'])
    #             return ast_nodes.DiziErisim(hedef_node=ast_nodes.Degisken(hedef_token), indeks_node=indeks)

    #         # Sözlük Erişimi mi? { }
    #         elif self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_BRACE']:
    #             self.tuket(TOKEN_TIPLERI['LEFT_BRACE'])
    #             anahtar = self.ifade()
    #             self.tuket(TOKEN_TIPLERI['RIGHT_BRACE'])
    #             return ast_nodes.SozlukErisim(hedef_node=ast_nodes.Degisken(hedef_token), anahtar_node=anahtar)
            
    #         # Fonksiyon Çağrısı mı? ( )
    #         elif self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_PAREN']:
    #             self.tuket(TOKEN_TIPLERI['LEFT_PAREN'])
    #             argumanlar = []
    #             if self.mevcut_token.tip != TOKEN_TIPLERI['RIGHT_PAREN']:
    #                 while True:
    #                     argumanlar.append(self.ifade())
    #                     if self.mevcut_token.tip == TOKEN_TIPLERI['RIGHT_PAREN']:
    #                         break
    #                     self.tuket(TOKEN_TIPLERI['COMMA'])
                
    #             self.tuket(TOKEN_TIPLERI['RIGHT_PAREN'])
    #             return ast_nodes.IslecCagirma(hedef_token=hedef_token, argumanlar_listesi=argumanlar)
            
    #         # Düz değişken (hiçbiri değilse)
    #         return ast_nodes.Degisken(hedef_token)

    #     # 5. Hata
    #     raise Exception(f"Parser hatası: Beklenmeyen token {self.mevcut_token} (atom içinde)")

    def komut(self):
        """Tek bir komutu ayrıştırır."""
        token = self.mevcut_token

        # 0. Phase 8: SINIF tanımı mı?
        if token.tip == TOKEN_TIPLERI['OOP_SINIF']:
            return self.sinif_tanimlama()

        # 1. MLP SYNTAX: DEĞIŞKEN isim: TİP = değer;
        if token.tip == TOKEN_TIPLERI['TANIMLA_DEGISKEN']:
            return self.mlp_degisken_tanimlama()

        # 2. TİP ile mi başlıyor?
        if token.tip in [TOKEN_TIPLERI['TANIMLA_SAYI'], TOKEN_TIPLERI['TANIMLA_METIN'], TOKEN_TIPLERI['TANIMLA_BOOL']]:
            return self.tip_ile_baslayan_komut()

        # 3. İŞLEÇ ile mi başlıyor?
        if token.tip == TOKEN_TIPLERI['YAPI_ISLEC']:
            return self.islec_tanimlama(None)

        # 3. EĞER Bloğu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_KOSUL_EGER']:
            return self.eger_blogu()
            
        # 4. YAZDIR Komutu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_YAZDIR']:
            return self.yazdir_komutu()
            
        # 5. DÖNGÜ Bloğu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_DONGU']:
            return self.dongu_blogu()
            
        # 6. DÖNGÜ_BITIR Komutu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_DONGU_BITIR']:
            return self.dongu_bitir_komutu()
            
        # 7. DÖNÜŞ Komutu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_DONUS']:
            return self.donus_komutu()

        # 8. Atama VEYA İşleç Çağırma Komutu mu?
        if token.tip == TOKEN_TIPLERI['IDENTIFIER']:
            # Burası kritik: YENİ 'atom' fonksiyonu sayesinde 'ifade_dugumu'
            # artık 'd' değil, 'SozlukErisim(d, "yas")' olarak gelmeli.
            ifade_dugumu = self.ifade()
            
            # Durum 1: 'd{"yas"} = 30;' (Atama)
            if self.mevcut_token.tip == TOKEN_TIPLERI['ASSIGN']:
                
                # GÜNCELLENMİŞ KONTROL (SozlukErisim eklendi)
                if not (isinstance(ifade_dugumu, ast_nodes.Degisken) or \
                        isinstance(ifade_dugumu, ast_nodes.DiziErisim) or \
                        isinstance(ifade_dugumu, ast_nodes.SozlukErisim)):
                    self.hata(f"Atama hedefi geçersiz. Hedef bir değişken (s), dizi elemanı (liste[0]) veya sözlük elemanı (sozluk{{'key'}}) olmalıdır. Bulunan: {type(ifade_dugumu)}")
                
                self.tuket(TOKEN_TIPLERI['ASSIGN']) # '='
                
                atanan_deger_ifadesi = self.ifade() # '30'
                
                self.tuket(TOKEN_TIPLERI['SEMICOLON']) # ';'
                return ast_nodes.AtamaKomutu(hedef_node=ifade_dugumu, ifade_node=atanan_deger_ifadesi)

            # Durum 2: 'SOZLUK_OLUSTUR(10);' (İşleç Çağırma Komutu)
            elif self.mevcut_token.tip == TOKEN_TIPLERI['SEMICOLON']:
                self.tuket(TOKEN_TIPLERI['SEMICOLON']) # ';'
                if isinstance(ifade_dugumu, ast_nodes.IslecCagirma):
                    return ast_nodes.IfadeKomutu(ifade_dugumu)
                else:
                    self.hata(f"Bu ifade tek başına bir komut olamaz: {ifade_dugumu}")
            
            else:
                self.hata(f"Komutun sonunda ';' veya '=' bekleniyordu, {self.mevcut_token} bulundu.")
            
        # 9. Hiçbiri değilse hatadır.
        self.hata(f"Tanınmayan komut başlangıcı: {token}")


    def degisken_tanimlama(self, tip_token, ad_token):
        """
        Gramer: (tip_token ve ad_token zaten alındı) ASSIGN ifade SEMICOLON
        """
        self.tuket(TOKEN_TIPLERI['ASSIGN'])
        ifade_node = self.ifade()
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])
        
        return ast_nodes.DegiskenTanimlama(tip_token, ad_token, ifade_node)

    def eger_blogu(self):
        """
        EĞER koşul İSE komutlar [DEĞİLSE komutlar] SON
        """
        self.tuket(TOKEN_TIPLERI['YAPI_KOSUL_EGER']) # 'EĞER'
        kosul = self.ifade() # Koşulu al (s EŞİT 10)
        self.tuket(TOKEN_TIPLERI['YAPI_KOSUL_ISE']) # 'İSE'
        
        # Doğru Bloğu (IF bloğu)
        dogru_blok = ast_nodes.Blok()
        
        # Komutları oku. Ta ki 'SON' veya 'DEĞİLSE' görene kadar.
        while self.mevcut_token.tip not in [
            TOKEN_TIPLERI['YAPI_SON'], 
            TOKEN_TIPLERI['YAPI_KOSUL_DEGILSE'], 
            TOKEN_TIPLERI['EOF']
        ]:
            dogru_blok.komutlar.append(self.komut())
            
        yanlis_blok = None # Varsayılan olarak DEĞİLSE bloğu yok
        
        # YENİ: 'DEĞİLSE' var mı diye kontrol et
        if self.mevcut_token.tip == TOKEN_TIPLERI['YAPI_KOSUL_DEGILSE']:
            self.tuket(TOKEN_TIPLERI['YAPI_KOSUL_DEGILSE']) # 'DEĞİLSE' kelimesini tüket
            
            # Yanlış Bloğu (ELSE bloğu)
            yanlis_blok = ast_nodes.Blok()
            
            # 'SON' görene kadar komutları oku
            while self.mevcut_token.tip not in [TOKEN_TIPLERI['YAPI_SON'], TOKEN_TIPLERI['EOF']]:
                yanlis_blok.komutlar.append(self.komut())

        # 'EĞER' bloğu mutlaka 'SON' ile bitmeli
        self.tuket(TOKEN_TIPLERI['YAPI_SON']) # 'SON'
        
        return ast_nodes.EgerBlogu(kosul, dogru_blok, yanlis_blok)

    def yazdir_komutu(self):
            """
            YAZDIR ifade;
            """
            self.tuket(TOKEN_TIPLERI['YAPI_YAZDIR']) # 'YAZDIR'
            
            # YAZDIR komutundan sonra herhangi bir matematiksel/mantıksal ifade gelebilir
            ifade = self.ifade() 
            
            # Komutun ; ile bittiğinden emin ol
            self.tuket(TOKEN_TIPLERI['SEMICOLON'])
            
            return ast_nodes.YazdirKomutu(ifade)

    def dongu_blogu(self):
        """
        DÖNGÜ komutlar SON
        """
        self.tuket(TOKEN_TIPLERI['YAPI_DONGU']) # 'DÖNGÜ'
        
        # Döngü Bloğu: 'SON' görene kadar komutları oku
        blok = ast_nodes.Blok()
        while self.mevcut_token.tip not in [TOKEN_TIPLERI['YAPI_SON'], TOKEN_TIPLERI['EOF']]:
            blok.komutlar.append(self.komut())
            
        self.tuket(TOKEN_TIPLERI['YAPI_SON']) # 'SON'
        
        return ast_nodes.DonguBlogu(blok)

    def dongu_bitir_komutu(self):
        """
        DÖNGÜ_BITIR;
        """
        self.tuket(TOKEN_TIPLERI['YAPI_DONGU_BITIR']) # 'DÖNGÜ_BITIR'
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])     # ';'
        return ast_nodes.DonguBitirKomutu()


    def mlp_degisken_tanimlama(self):
        """
        MLP Syntax: DEĞIŞKEN isim: TİP = değer;
        Örnek: DEĞIŞKEN x: SAYI = 42;
               DEĞIŞKEN kaynak_kod: METIN;
        """
        self.tuket(TOKEN_TIPLERI['TANIMLA_DEGISKEN'])  # 'DEĞIŞKEN' atla

        # İsim al
        ad_token = self.mevcut_token
        self.tuket(TOKEN_TIPLERI['IDENTIFIER'])

        # ':' bekle
        self.tuket(TOKEN_TIPLERI['COLON'])

        # Tip al (identifier VEYA tip keyword olabilir: METIN, SAYI, SÖZLÜK, DİZİ, vs.)
        tip_token = self.mevcut_token
        if tip_token.tip in [TOKEN_TIPLERI['TANIMLA_SAYI'], TOKEN_TIPLERI['TANIMLA_METIN'],
                             TOKEN_TIPLERI['TANIMLA_BOOL'], TOKEN_TIPLERI['IDENTIFIER']]:
            self.tuket(tip_token.tip)
        else:
            self.hata(f"Tip anotasyonu bekleniyor, bulundu: {tip_token}")

        # Opsiyonel başlangıç değeri
        ifade_node = None
        if self.mevcut_token.tip == TOKEN_TIPLERI['ASSIGN']:
            self.tuket(TOKEN_TIPLERI['ASSIGN'])
            ifade_node = self.ifade()

        # ';' ile bitir
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])

        # DegiskenTanimlama node döndür
        return ast_nodes.DegiskenTanimlama(tip_token, ad_token, ifade_node)

    def tip_ile_baslayan_komut(self):
        """
        Bir tip ile başlayan (SAYISAL, METİN...) komutları ayırır.
        - SAYISAL İŞLEÇ topla...
        - SAYISAL liste[10];
        - SAYISAL s = 10;
        """
        tip_token = self.mevcut_token
        self.tuket(tip_token.tip)
        
        # Durum 1: SAYISAL İŞLEÇ ...
        if self.mevcut_token.tip == TOKEN_TIPLERI['YAPI_ISLEC']:
            return self.islec_tanimlama(tip_token)
            
        # Durum 2 & 3: SAYISAL s ... (Değişken veya Dizi)
        elif self.mevcut_token.tip == TOKEN_TIPLERI['IDENTIFIER']:
            ad_token = self.mevcut_token
            self.tuket(TOKEN_TIPLERI['IDENTIFIER'])
            
            # Şimdi ne geliyor?
            
            # Durum 2: SAYISAL liste[ ...
            if self.mevcut_token.tip == TOKEN_TIPLERI['LEFT_BRACKET']:
                return self.dizi_tanimlama(tip_token, ad_token)
            
            # Durum 3: SAYISAL s = ...
            elif self.mevcut_token.tip == TOKEN_TIPLERI['ASSIGN']:
                return self.degisken_tanimlama(tip_token, ad_token)
            
            else:
                self.hata(f"Değişken adından sonra '[' veya '=' bekleniyordu, {self.mevcut_token} bulundu.")
        else:
            self.hata(f"Tip tanımından sonra 'İŞLEÇ' veya değişken adı bekleniyordu, {self.mevcut_token} bulundu.")

    def parse(self):
        """
        Tüm programı ayrıştırır ve AST'nin kökünü (bir Blok) döndürür.
        """
        ana_blok = ast_nodes.Blok()
        
        # Dosya sonuna (EOF) kadar tüm komutları oku
        while self.mevcut_token.tip != TOKEN_TIPLERI['EOF']:
            ana_blok.komutlar.append(self.komut())
            
        return ana_blok

    def donus_komutu(self):
        """
        DÖNÜŞ ifade;
        """
        self.tuket(TOKEN_TIPLERI['YAPI_DONUS']) # 'DÖNÜŞ'
        ifade = self.ifade()
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])
        return ast_nodes.DonusKomutu(ifade)

    def parametre_listesi(self):
        """
        ( [TİP IDENTIFIER (COMMA TİP IDENTIFIER)*]? )
        """
        parametreler = []
        self.tuket(TOKEN_TIPLERI['LEFT_PAREN']) # '('
        
        # Eğer ilk token ')' değilse, parametreler var demektir
        if self.mevcut_token.tip != TOKEN_TIPLERI['RIGHT_PAREN']:
            # İlk parametre
            tip_token = self.mevcut_token
            if tip_token.tip not in [TOKEN_TIPLERI['TANIMLA_SAYI'], TOKEN_TIPLERI['TANIMLA_METIN'], TOKEN_TIPLERI['TANIMLA_BOOL']]:
                self.hata("Parametre tipi bekleniyordu (SAYISAL, METİN vb.)")
            self.tuket(tip_token.tip)
            
            ad_token = self.mevcut_token
            self.tuket(TOKEN_TIPLERI['IDENTIFIER'])
            parametreler.append(ast_nodes.Parametre(tip_token, ad_token))
            
            # Kalan parametreler (virgülle ayrılmış)
            while self.mevcut_token.tip == TOKEN_TIPLERI['COMMA']:
                self.tuket(TOKEN_TIPLERI['COMMA']) # ','
                
                tip_token = self.mevcut_token
                if tip_token.tip not in [TOKEN_TIPLERI['TANIMLA_SAYI'], TOKEN_TIPLERI['TANIMLA_METIN'], TOKEN_TIPLERI['TANIMLA_BOOL']]:
                    self.hata("Parametre tipi bekleniyordu (SAYISAL, METİN vb.)")
                self.tuket(tip_token.tip)
                
                ad_token = self.mevcut_token
                self.tuket(TOKEN_TIPLERI['IDENTIFIER'])
                parametreler.append(ast_nodes.Parametre(tip_token, ad_token))
                
        self.tuket(TOKEN_TIPLERI['RIGHT_PAREN']) # ')'
        return parametreler

    def islec_tanimlama(self, donus_tipi_token=None):
        """
        [TİP]? İŞLEÇ IDENTIFIER ( parametreler ) komutlar SON
        'donus_tipi_token' None ise, bu tipsiz bir işleçtir (HİÇLİK).
        """
        # (donus_tipi_token) zaten ana komut fonksiyonu tarafından tüketildi (eğer varsa)
        
        self.tuket(TOKEN_TIPLERI['YAPI_ISLEC']) # 'İŞLEÇ'
        
        ad_token = self.mevcut_token
        self.tuket(TOKEN_TIPLERI['IDENTIFIER']) # İşlecin adı (örn: topla)
        
        parametreler = self.parametre_listesi() # (...)
        
        # İşleç Gövdesi
        govde = ast_nodes.Blok()
        while self.mevcut_token.tip not in [TOKEN_TIPLERI['YAPI_SON'], TOKEN_TIPLERI['EOF']]:
            govde.komutlar.append(self.komut())
            
        self.tuket(TOKEN_TIPLERI['YAPI_SON']) # 'SON'
        
        return ast_nodes.IslecTanimlama(ad_token, parametreler, govde, donus_tipi_token)

# src/parser.py

    def dizi_tanimlama(self, tip_token, ad_token):
        """
        Gramer: (tip_token ve ad_token zaten alındı) '[' ifade ']' SEMICOLON
        """
        self.tuket(TOKEN_TIPLERI['LEFT_BRACKET']) # '['
        boyut_ifadesi = self.ifade()
        self.tuket(TOKEN_TIPLERI['RIGHT_BRACKET']) # ']'
        
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])
        
        return ast_nodes.DiziTanimlama(tip_token, ad_token, boyut_ifadesi)
    
    def sozluk_erisimi(self, hedef_token):
        """
        Gramer: '{' ifade '}'
        'hedef_token' (örn: 'sozluk') zaten 'faktor' tarafından tüketildi.
        """
        self.tuket(TOKEN_TIPLERI['LEFT_BRACE']) # '{'
        
        # Anahtar, '5 + 2' gibi karmaşık bir ifade olabilir!
        anahtar_ifadesi = self.ifade() 
        
        self.tuket(TOKEN_TIPLERI['RIGHT_BRACE']) # '}'
        
        hedef_node = ast_nodes.Degisken(hedef_token)
        # ast_nodes.py'deki SozlukErisim tanımımızla eşleşmeli
        return ast_nodes.SozlukErisim(hedef_node=hedef_node, anahtar_node=anahtar_ifadesi)

    # --- Phase 8: OOP Support ---

    def sinif_tanimlama(self):
        """
        SINIF tanımını parse eder:

        SINIF Person
            ÖZELLIK METIN isim;
            ÖZELLIK SAYISAL yas;

            KURUCU(METIN i, SAYISAL y)
                isim = i;
                yas = y;
            SON

            IŞLEÇ selamla()
                YAZDIR isim;
            SON
        SON
        """
        self.tuket(TOKEN_TIPLERI['OOP_SINIF'])  # 'SINIF'

        # Sınıf adı
        sinif_adi = self.mevcut_token
        self.tuket(TOKEN_TIPLERI['IDENTIFIER'])

        # Özellikler, kurucu, metodlar
        ozellikler = []
        kurucu = None
        metodlar = []

        # SINIF gövdesini parse et (SON'a kadar)
        while self.mevcut_token.tip != TOKEN_TIPLERI['YAPI_SON'] and \
              self.mevcut_token.tip != TOKEN_TIPLERI['EOF']:

            # ÖZELLIK tanımı mı?
            if self.mevcut_token.tip == TOKEN_TIPLERI['OOP_OZELLIK']:
                ozellikler.append(self.ozellik_tanimlama())

            # KURUCU tanımı mı?
            elif self.mevcut_token.tip == TOKEN_TIPLERI['OOP_KURUCU']:
                kurucu = self.kurucu_tanimlama()

            # IŞLEÇ (method) tanımı mı?
            elif self.mevcut_token.tip in [TOKEN_TIPLERI['TANIMLA_SAYI'],
                                            TOKEN_TIPLERI['TANIMLA_METIN'],
                                            TOKEN_TIPLERI['TANIMLA_BOOL']]:
                # Dönüş tipi olan method
                tip_token = self.mevcut_token
                self.tuket(self.mevcut_token.tip)

                if self.mevcut_token.tip == TOKEN_TIPLERI['YAPI_ISLEC']:
                    metodlar.append(self.islec_tanimlama(tip_token))
                else:
                    self.hata("SINIF içinde tip token'dan sonra IŞLEÇ bekleniyor")

            else:
                self.hata(f"SINIF içinde beklenmeyen token: {self.mevcut_token}")

        self.tuket(TOKEN_TIPLERI['YAPI_SON'])  # 'SON'

        return ast_nodes.SinifTanimlama(sinif_adi, ozellikler, kurucu, metodlar)

    def ozellik_tanimlama(self):
        """ÖZELLIK TIP isim; formatını parse eder"""
        self.tuket(TOKEN_TIPLERI['OOP_OZELLIK'])  # 'ÖZELLIK'

        # Tip
        tip_token = self.mevcut_token
        if self.mevcut_token.tip not in [TOKEN_TIPLERI['TANIMLA_SAYI'],
                                          TOKEN_TIPLERI['TANIMLA_METIN'],
                                          TOKEN_TIPLERI['TANIMLA_BOOL']]:
            self.hata("ÖZELLIK'ten sonra tip bekleniyor (SAYISAL, METIN, vb.)")

        self.tuket(self.mevcut_token.tip)

        # İsim
        ad_token = self.mevcut_token
        self.tuket(TOKEN_TIPLERI['IDENTIFIER'])

        self.tuket(TOKEN_TIPLERI['SEMICOLON'])  # ';'

        return ast_nodes.OzellikTanimlama(tip_token, ad_token)

    def kurucu_tanimlama(self):
        """KURUCU(parametreler) ... SON formatını parse eder"""
        self.tuket(TOKEN_TIPLERI['OOP_KURUCU'])  # 'KURUCU'

        # Parametreler
        self.tuket(TOKEN_TIPLERI['LEFT_PAREN'])  # '('

        parametreler = []
        if self.mevcut_token.tip != TOKEN_TIPLERI['RIGHT_PAREN']:
            # İlk parametre
            parametreler.append(self.parametre())

            # Virgülle ayrılmış diğer parametreler
            while self.mevcut_token.tip == TOKEN_TIPLERI['COMMA']:
                self.tuket(TOKEN_TIPLERI['COMMA'])
                parametreler.append(self.parametre())

        self.tuket(TOKEN_TIPLERI['RIGHT_PAREN'])  # ')'

        # Kurucu gövdesi (komutlar SON'a kadar)
        govde = ast_nodes.Blok()
        while self.mevcut_token.tip != TOKEN_TIPLERI['YAPI_SON'] and \
              self.mevcut_token.tip != TOKEN_TIPLERI['EOF']:
            govde.komutlar.append(self.komut())

        self.tuket(TOKEN_TIPLERI['YAPI_SON'])  # 'SON'

        return ast_nodes.KurucuTanimlama(parametreler, govde)

    def parametre(self):
        """TIP isim formatını parse eder (fonksiyon parametresi)"""
        # Tip
        tip_token = self.mevcut_token
        if self.mevcut_token.tip not in [TOKEN_TIPLERI['TANIMLA_SAYI'],
                                          TOKEN_TIPLERI['TANIMLA_METIN'],
                                          TOKEN_TIPLERI['TANIMLA_BOOL']]:
            self.hata("Parametre tipinde SAYISAL, METIN veya ZITLIK bekleniyor")

        self.tuket(self.mevcut_token.tip)

        # İsim
        ad_token = self.mevcut_token
        self.tuket(TOKEN_TIPLERI['IDENTIFIER'])

        return ast_nodes.Parametre(tip_token, ad_token)