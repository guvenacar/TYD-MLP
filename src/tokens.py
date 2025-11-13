# src/tokens.py
# NOT: Bu dosyanın UTF-8 olarak kaydedildiğinden emin olun!

class Token:
    """Dilimizdeki tek bir kelimeyi veya sembolü temsil eder."""
    def __init__(self, tip, deger=None):
        self.tip = tip
        self.deger = deger
    
    def __repr__(self):
        """Hata ayıklama için token'ın güzel gösterimi."""
        if self.deger is not None:
            return f"<{self.tip}: {self.deger}>"
        return f"<{self.tip}>"

# Dilimizin tanıyacağı TÜM SOYUT (ABSTRACT) token tipleri
TOKEN_TIPLERI = {
    # Tip Tanımları (Kelimeler - JSON'dan)
    'TANIMLA_DEGISKEN': 'TANIMLA_DEGISKEN',  # MLP Syntax: DEĞIŞKEN
    'TANIMLA_SAYI': 'TANIMLA_SAYI',
    'TANIMLA_METIN': 'TANIMLA_METIN',
    'TANIMLA_BOOL': 'TANIMLA_BOOL',
    'TANIMLA_NESNE': 'TANIMLA_NESNE',

    # Değer Sabitleri (Kelimeler - JSON'dan)
    'DEGER_BOOL_DOGRU': 'DEGER_BOOL_DOGRU',
    'DEGER_BOOL_YANLIS': 'DEGER_BOOL_YANLIS',
    'DEGER_HICLIK': 'DEGER_HICLIK',
    
    # Yapısal Anahtar Kelimeler (Kelimeler - JSON'dan)
    'YAPI_KOSUL_EGER': 'YAPI_KOSUL_EGER',
    'YAPI_KOSUL_ISE': 'YAPI_KOSUL_ISE',
    'YAPI_KOSUL_DEGILSE': 'YAPI_KOSUL_DEGILSE',
    'YAPI_YAZDIR': 'YAPI_YAZDIR',
    'YAPI_DONUS': 'YAPI_DONUS',
    'YAPI_ISLEC': 'YAPI_ISLEC',
    'YAPI_DONGU': 'YAPI_DONGU',
    'YAPI_DONGU_BITIR': 'YAPI_DONGU_BITIR',
    'YAPI_SON': 'YAPI_SON',

    # Operatör Anahtar Kelimeleri (Kelimeler - JSON'dan)
    'OP_ESIT': 'OP_ESIT',
    'OP_ARTI': 'OP_ARTI',
    'OP_EKSI': 'OP_EKSI',
    'OP_CARP': 'OP_CARP',
    'OP_BOL': 'OP_BOL', 
    'OP_GT': 'OP_GT',
    'OP_LT': 'OP_LT',

    # Standart Değer Tokenları (Sabit Kodlu - Lexer'da)
    'IDENTIFIER': 'IDENTIFIER',
    'INTEGER': 'INTEGER',
    'FLOAT': 'FLOAT',
    'STRING': 'STRING',
    
    # Rezerve Sembol Tokenları (Sabit Kodlu - Lexer'da)
    'ASSIGN': 'ASSIGN',         # =
    'SEMICOLON': 'SEMICOLON',   # ;
    'COLON': 'COLON',           # :
    'COMMA': 'COMMA',           # ,
    'PLUS': 'PLUS', 'MINUS': 'MINUS', 'MUL': 'MUL', 'DIV': 'DIV',
    'POWER': 'POWER',           # **
    
    'GT': 'GT', 'LT': 'LT',     # > <
    'GTE': 'GTE', 'LTE': 'LTE', # >= <=
    
    'OP_ESIT_KARSILASTIRMA': 'OP_ESIT_KARSILASTIRMA', # ==
    
    # Gramer Yapısı (Sabit Kodlu - Lexer'da)
    'LEFT_PAREN': 'LEFT_PAREN', 'RIGHT_PAREN': 'RIGHT_PAREN', # ( )
    'LEFT_BRACKET': 'LEFT_BRACKET', 'RIGHT_BRACKET': 'RIGHT_BRACKET', # [ ]
    'LEFT_BRACE': 'LEFT_BRACE', 'RIGHT_BRACE': 'RIGHT_BRACE', # { }

    # --- HATA BURADAYDI ---
    # 'RIGHT_BRACE' satırının sonuna VİRGÜL (,) eklendi.
    
    'EOF': 'EOF', # Dosya Sonu 
}