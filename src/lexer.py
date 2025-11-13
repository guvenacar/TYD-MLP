# src/lexer.py
# NOT: Bu dosyanın UTF-8 olarak kaydedildiğinden emin olun!
# "Pragmatik Çekirdek" Felsefesine Uygun Sürüm
# (Kelimeler dinamik, Semboller sabit kodlu)

import json
from src.tokens import Token, TOKEN_TIPLERI

class Lexer:
    """
    TYD kaynak kodunu okur ve onu Token (Jeton) listesine dönüştürür.
    JSON'dan yüklenen dil profiline göre çalışır.
    """
    def __init__(self, kaynak_kodu, dil_json_yolu, dil_kodu='tr-TR'):
        self.kaynak_kodu = kaynak_kodu
        self.konum = 0
        self.mevcut_karakter = self.kaynak_kodu[self.konum] if len(self.kaynak_kodu) > 0 else None
        
        # Anahtar Kelimeleri JSON'dan Yükle
        self.anahtar_kelimeler = self.dil_yukle(dil_json_yolu, dil_kodu)

    def _normalize_key(self, text):
        """
        Bir kelimeyi (anahtar kelime veya koddan gelen) 
        karşılaştırma için standart (kanonik) formata dönüştürür.
        """
        normalized_text = text.casefold()
        normalized_text = normalized_text.replace('ı', 'i')
        normalized_text = normalized_text.replace('İ', 'i')
        normalized_text = normalized_text.replace('i̇', 'i')
        return normalized_text

    def dil_yukle(self, json_yolu, dil_kodu):
        """
        JSON'dan dil profilini yükler.
        SADECE anahtar kelimeleri haritaya yükler.
        """
        try:
            with open(json_yolu, 'r', encoding='utf-8') as f:
                diller = json.load(f)
        except Exception as e:
            raise Exception(f"Dil JSON dosyası okunamadı: {json_yolu}. Hata: {e}")

        if dil_kodu not in diller['dil_profili']:
            raise Exception(f"Dil profili '{dil_kodu}' JSON'da bulunamadı.")
            
        profil = diller['dil_profili'][dil_kodu]
        
        anahtar_kelime_haritasi = {}
        for soyut_tip, somut_kelime in profil.items():
            if soyut_tip in TOKEN_TIPLERI:
                anahtar = self._normalize_key(somut_kelime)
                anahtar_kelime_haritasi[anahtar] = soyut_tip
        
        print(f"[Lexer] Dil profili yüklendi: {dil_kodu}. {len(anahtar_kelime_haritasi)} anahtar kelime tanındı.")
        
        return anahtar_kelime_haritasi

    # --- İlerleme ve Yardımcı Fonksiyonlar ---

    def ilerle(self, miktar=1):
        """Metin içinde bir (veya 'miktar' kadar) karakter ileri gider."""
        self.konum += miktar
        if self.konum >= len(self.kaynak_kodu):
            self.mevcut_karakter = None # Dosya Sonu (EOF)
        else:
            self.mevcut_karakter = self.kaynak_kodu[self.konum]

    def peek(self, miktar=1):
        """Bir sonraki karaktere ilerlemeden 'göz atar'."""
        peek_konum = self.konum + miktar
        if peek_konum >= len(self.kaynak_kodu):
            return None
        return self.kaynak_kodu[peek_konum]
    
    def mevcut_dilim(self, uzunluk):
        """Mevcut konumdan itibaren 'uzunluk' kadar metin dilimi alır."""
        if self.konum + uzunluk > len(self.kaynak_kodu):
            return None
        return self.kaynak_kodu[self.konum : self.konum + uzunluk]

    # --- Yorum Atlama Fonksiyonları ---

    def bosluk_atla(self):
        while self.mevcut_karakter is not None and self.mevcut_karakter.isspace():
            self.ilerle()

    def tek_satirli_yorum_atla(self):
        while self.mevcut_karakter is not None and self.mevcut_karakter != '\n':
            self.ilerle()
        if self.mevcut_karakter == '\n':
            self.ilerle() # Yeni satırı da atla

    def cok_satirli_yorum_atla(self):
        """ '---' görene kadar atlar. """
        while self.mevcut_karakter is not None:
            if self.mevcut_dilim(3) == "---":
                self.ilerle(3) # Kapatan sembolü atla
                return # Yorumdan çıktık
            
            if self.mevcut_karakter is None:
                break
            self.ilerle()

        raise Exception("Dosya sonuna ulaşıldı, ancak '---' yorum bloğu kapatılmadı.")

    # --- Token Okuma Fonksiyonları ---

    def sayi_oku(self):
        sayi_str = ''
        nokta_kullanildi = False
        while self.mevcut_karakter is not None and \
              (self.mevcut_karakter.isdigit() or self.mevcut_karakter == '.'):
            
            if self.mevcut_karakter == '.':
                if nokta_kullanildi:
                    raise Exception(f"Sayı formatı hatalı: {sayi_str}.")
                nokta_kullanildi = True
            
            sayi_str += self.mevcut_karakter
            self.ilerle()

        if nokta_kullanildi:
            return Token(TOKEN_TIPLERI['FLOAT'], float(sayi_str))
        else:
            return Token(TOKEN_TIPLERI['INTEGER'], int(sayi_str))

    def metin_oku(self):
        metin = ''
        self.ilerle() # Açılış tırnağını atla
        while self.mevcut_karakter is not None and self.mevcut_karakter != '"':
            metin += self.mevcut_karakter
            self.ilerle()
        if self.mevcut_karakter is None:
            raise Exception("Metin (string) kapatılmadı.")
        self.ilerle() # Kapanış tırnağını atla
        return Token(TOKEN_TIPLERI['STRING'], metin)

    def kelime_oku(self):
        kelime = ''
        while self.mevcut_karakter is not None and \
              (self.mevcut_karakter.isalnum() or self.mevcut_karakter == '_'):
            
            kelime += self.mevcut_karakter
            self.ilerle()
        
        kelime_kontrol_formati = self._normalize_key(kelime)
        
        if kelime_kontrol_formati in self.anahtar_kelimeler:
            token_tipi = self.anahtar_kelimeler[kelime_kontrol_formati]
            return Token(token_tipi, kelime_kontrol_formati)
        else:
            return Token(TOKEN_TIPLERI['IDENTIFIER'], kelime)

    # --- ANA TOKENLEŞTİRME MOTORU (PRAGMATİK ÇEKİRDEK) ---

    def sonraki_tokeni_al(self):
        """
        Kaynaktaki bir sonraki Token'ı bulur ve döndürür.
        Semboller ('+', '{', '==', '--') SABİT KODLUDUR.
        """
        
        while self.mevcut_karakter is not None:
            
            # 1. Boşluklar
            if self.mevcut_karakter.isspace():
                self.bosluk_atla()
                continue

            # 2. Kelimeler (Dinamik Haritadan)
            if self.mevcut_karakter.isalpha() or self.mevcut_karakter == '_':
                return self.kelime_oku()

            # 3. Sayılar (Sabit Kural)
            if self.mevcut_karakter.isdigit():
                return self.sayi_oku()

            # 4. Metinler (String) (Sabit Kural)
            if self.mevcut_karakter == '"':
                return self.metin_oku()

            # 5. SEMBOLLER, OPERATÖRLER VE YORUMLAR (Sabit Kodlu Çekirdek)
            
            # Yorumlar (---, --, -)
            if self.mevcut_karakter == '-':
                if self.peek() == '-':
                    self.ilerle(); self.ilerle() # İki '-' geç
                    if self.mevcut_karakter == '-':
                        self.ilerle() # Üçüncü '-' geç
                        self.cok_satirli_yorum_atla()
                        continue 
                    else:
                        self.tek_satirli_yorum_atla()
                        continue
                else:
                    self.ilerle()
                    return Token(TOKEN_TIPLERI['MINUS'], '-')
            
            # Üs alma (**)
            if self.mevcut_karakter == '*':
                if self.peek() == '*':
                    self.ilerle(); self.ilerle()
                    return Token(TOKEN_TIPLERI['POWER'], '**')
                else:
                    self.ilerle()
                    return Token(TOKEN_TIPLERI['MUL'], '*')

            # Karşılaştırmalar (==, >=, <=)
            if self.mevcut_karakter == '=':
                if self.peek() == '=':
                    self.ilerle(); self.ilerle()
                    return Token(TOKEN_TIPLERI['OP_ESIT_KARSILASTIRMA'], '==')
                else:
                    self.ilerle()
                    return Token(TOKEN_TIPLERI['ASSIGN'], '=')

            if self.mevcut_karakter == '>':
                if self.peek() == '=':
                    self.ilerle(); self.ilerle()
                    return Token(TOKEN_TIPLERI['GTE'], '>=')
                else:
                    self.ilerle()
                    return Token(TOKEN_TIPLERI['GT'], '>')

            if self.mevcut_karakter == '<':
                if self.peek() == '=':
                    self.ilerle(); self.ilerle()
                    return Token(TOKEN_TIPLERI['LTE'], '<=')
                else:
                    self.ilerle()
                    return Token(TOKEN_TIPLERI['LT'], '<')

            # --- Tek Karakterli Semboller ---
            
            if self.mevcut_karakter == '+':
                self.ilerle()
                return Token(TOKEN_TIPLERI['PLUS'], '+')
                
            if self.mevcut_karakter == '/':
                self.ilerle()
                return Token(TOKEN_TIPLERİ['DIV'], '/')

            if self.mevcut_karakter == '(':
                self.ilerle()
                return Token(TOKEN_TIPLERI['LEFT_PAREN'], '(')
            if self.mevcut_karakter == ')':
                self.ilerle()
                return Token(TOKEN_TIPLERI['RIGHT_PAREN'], ')')

            if self.mevcut_karakter == '[':
                self.ilerle()
                return Token(TOKEN_TIPLERI['LEFT_BRACKET'], '[')
            if self.mevcut_karakter == ']':
                self.ilerle()
                return Token(TOKEN_TIPLERI['RIGHT_BRACKET'], ']')
                
            if self.mevcut_karakter == ';':
                self.ilerle()
                return Token(TOKEN_TIPLERI['SEMICOLON'], ';')

            if self.mevcut_karakter == ':':
                self.ilerle()
                return Token(TOKEN_TIPLERI['COLON'], ':')

            if self.mevcut_karakter == ',':
                self.ilerle()
                return Token(TOKEN_TIPLERI['COMMA'], ',')

            # --- YENİ HEDEFİMİZ: SÖZLÜK SÖZ DİZİMİ ---
            if self.mevcut_karakter == '{':
                self.ilerle()
                return Token(TOKEN_TIPLERI['LEFT_BRACE'], '{')
            if self.mevcut_karakter == '}':
                self.ilerle()
                return Token(TOKEN_TIPLERI['RIGHT_BRACE'], '}')
            
            # 6. Hata (Tanınmayan karakter)
            raise Exception(f"Tanınmayan karakter: '{self.mevcut_karakter}'")

        # Döngü bittiyse dosyanın sonudur
        return Token(TOKEN_TIPLERI['EOF'], None)