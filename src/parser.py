# src/parser.py

from src.tokens import TOKEN_TIPLERI
#from src.tokens import Token # SIL komutu için gerekli olabilir
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
        # KRİTİK GÜVENLİK KONTROLÜ
        if not hasattr(self.mevcut_token, 'tip'):
            self.hata(f"Lexer/Token Hatası: Parser'a Token objesi yerine {type(self.mevcut_token)} ('{self.mevcut_token}') döndü.")


        if self.mevcut_token.tip == token_tipi:
            yeni_token = self.lexer.sonraki_tokeni_al()
            
            # Yeni tokenin bir string olup olmadığını kontrol et
            if isinstance(yeni_token, str):
                self.hata(f"Lexer Hatası: Lexer'dan Token objesi yerine metin ('{yeni_token}') döndü. Lexer'ın EOF/Hata işleme mantığını kontrol edin.")
            
            self.mevcut_token = yeni_token
        else:
            self.hata(f"Beklenen token: {token_tipi}. Bulunan: <{self.mevcut_token.tip}>")

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

    def komut(self):
        """Tek bir komutu ayrıştırır."""
        token = self.mevcut_token
        
        # 1. TİP ile mi başlıyor?
        if token.tip in [TOKEN_TIPLERI['TANIMLA_SAYI'], TOKEN_TIPLERI['TANIMLA_METIN'], TOKEN_TIPLERI['TANIMLA_BOOL']]:
            return self.tip_ile_baslayan_komut()
        
        # 2. İŞLEÇ ile mi başlıyor?
        if token.tip == TOKEN_TIPLERI['YAPI_ISLEC']:
            return self.islec_tanimlama(None)

        # 3. EĞER Bloğu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_KOSUL_EGER']:
            return self.eger_blogu()
            
        # 4. YAZDIR Komutu mu?
        if token.tip == TOKEN_TIPLERI['YAPI_YAZDIR']:
            return self.yazdir_komutu() 

        # 5. SIL Komutu mu?  <-- BU YENİ KONTROLÜ EKLEYİN
        if token.tip == TOKEN_TIPLERI['YAPI_SIL']:
            return self.sil_komutu()

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

        # 'EĞER' bloğu 'SON EĞER' ile bitmeli (IntelliSense desteği için)
        self.tuket(TOKEN_TIPLERI['YAPI_SON'])
        self.tuket(TOKEN_TIPLERI['YAPI_KOSUL_EGER'])  # 'EĞER' kelimesini bekle

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

        # DÖNGÜ bloğu 'SON DÖNGÜ' ile bitmeli (IntelliSense desteği için)
        self.tuket(TOKEN_TIPLERI['YAPI_SON'])
        self.tuket(TOKEN_TIPLERI['YAPI_DONGU'])  # 'DÖNGÜ' kelimesini bekle

        return ast_nodes.DonguBlogu(blok)

    def dongu_bitir_komutu(self):
        """
        DÖNGÜ_BITIR;
        """
        self.tuket(TOKEN_TIPLERI['YAPI_DONGU_BITIR']) # 'DÖNGÜ_BITIR'
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])     # ';'
        return ast_nodes.DonguBitirKomutu()


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
        
        # <<< KRİTİK DEBUG KODU BURAYA EKLENMELİ >>>
        print(f"[DEBUG] Fonksiyon gövdesi başlangıcı token tipi: {type(self.mevcut_token)}")
        # <<< KRİTİK DEBUG KODU BURAYA EKLENMELİ >>>

        # İşleç Gövdesi
        govde = ast_nodes.Blok()
        while self.mevcut_token.tip not in [TOKEN_TIPLERI['YAPI_SON'], TOKEN_TIPLERI['EOF']]:
            govde.komutlar.append(self.komut())

        # İŞLEÇ bloğu 'SON İŞLEÇ' ile bitmeli (IntelliSense desteği için)
        self.tuket(TOKEN_TIPLERI['YAPI_SON'])
        self.tuket(TOKEN_TIPLERI['YAPI_ISLEC'])  # 'İŞLEÇ' kelimesini bekle

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
    
    def sil_komutu(self):
        """
        SIL ifade; komutunu temsil eder ve onu bir IslecCagirma olarak gizler.
        """
        # Lexer'daki Token sınıfını içe aktarmamız gerek.
        from src.tokens import Token 
        
        # 1. 'SIL' anahtar kelimesini tüket
        self.tuket(TOKEN_TIPLERI['YAPI_SIL']) 
        
        # 2. İfadeyi al (s3). Bu s3'ü tüketir.
        ifade = self.ifade() 
        
        # 3. Komutun ; ile bittiğinden emin ol
        self.tuket(TOKEN_TIPLERI['SEMICOLON'])
        
        # KRİTİK DÜZELTME: Token'ı sadece 2 argümanla oluştur
        sil_identifier_token = Token(
            'IDENTIFIER',
            'SIL' # Komutun adı
        )
        
        # HİLE: SIL'i bir IslecCagirma olarak döndür.
        return ast_nodes.IslecCagirma(sil_identifier_token, [ifade])