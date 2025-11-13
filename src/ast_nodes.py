# src/ast_nodes.py

class ASTNode:
    """Tüm AST düğümlerinin temel sınıfı."""
    pass

# --- İfadeler (Expressions) ---
# Bir değer üreten yapılar (5 + 3, a, "merhaba")

class Sayi(ASTNode):
    """Sayısal bir değeri temsil eder (10, 3.14)."""
    def __init__(self, token):
        self.token = token
        self.deger = token.deger

class Metin(ASTNode):
    """Metinsel bir değeri temsil eder ("merhaba")."""
    def __init__(self, token):
        self.token = token
        self.deger = token.deger

class Degisken(ASTNode):
    """Bir değişkenin kullanımını temsil eder (a, topla)."""
    def __init__(self, token):
        self.token = token
        self.ad = token.deger

class IkiliIslem(ASTNode):
    """İki terimli bir işlemi temsil eder (a + b, s > 10)."""
    def __init__(self, sol, operator_token, sag):
        self.sol = sol           # Sol taraf (ASTNode)
        self.operator = operator_token # İşlem (+, -, >, ESIT)
        self.sag = sag           # Sağ taraf (ASTNode)

# --- Komutlar (Statements) ---
# Bir eylem gerçekleştiren yapılar (SAYISAL a = 10;)

class DegiskenTanimlama(ASTNode):
    """Yeni bir değişken tanımlama komutu (SAYISAL s = 10;)."""
    def __init__(self, tip_token, degisken_adi_token, ifade_node):
        self.tip = tip_token           # (SAYISAL, METIN vb.)
        self.ad = degisken_adi_token.deger # Değişkenin adı ('s')
        self.ifade = ifade_node        # Atanan değerin AST'si (10)

class Blok(ASTNode):
    """Birden fazla komutu içeren bir blok ({ ... } veya EĞER...SON arası)."""
    def __init__(self):
        self.komutlar = [] # Blok içindeki komutların listesi

class EgerBlogu(ASTNode):
    """EĞER koşul İSE blok [DEĞİLSE blok] SON yapısını temsil eder."""
    def __init__(self, kosul_node, dogru_blok_node, yanlis_blok_node=None):
        self.kosul = kosul_node
        self.dogru_blok = dogru_blok_node
        self.yanlis_blok = yanlis_blok_node # YENİ: Başlangıçta None (boş)

class YazdirKomutu(ASTNode):
    """YAZDIR ifade; komutunu temsil eder."""
    def __init__(self, ifade_node):
        # YAZDIR komutunun neyi yazacağını tutar (örn: bir Sayi, bir Degisken veya bir IkiliIslem)
        self.ifade = ifade_node

class AtamaKomutu(ASTNode):
    """Varolan bir değişkene atama komutu (s = s + 1;)."""
    def __init__(self, hedef_node, ifade_node):
        # Nereye atanacağı (örn: 's' değişkeni)
        self.hedef = hedef_node   # Bu bir ast_nodes.Degisken olacak
        # Ne atanacağı (örn: s + 1 ifadesi)
        self.ifade = ifade_node

class DonguBlogu(ASTNode):
    """DÖNGÜ ... SON bloğunu temsil eder."""
    def __init__(self, blok_node):
        # Döngünün içinde çalışacak komutların bloğu
        self.blok = blok_node

class DonguBitirKomutu(ASTNode):
    """DÖNGÜ_BITIR; komutunu temsil eder (break)."""
    pass # Bu komutun ek bir veriye ihtiyacı yok

# src/ast_nodes.py

# ... (DonguBitirKomutu sınıfının altına) ...

class Parametre(ASTNode):
    """Bir işleç tanımındaki tek bir parametreyi temsil eder (SAYISAL a)."""
    def __init__(self, tip_token, ad_token):
        self.tip = tip_token
        self.ad = ad_token

class IslecTanimlama(ASTNode):
    """İŞLEÇ ... SON bloğunu temsil eder (Fonksiyon Tanımı)."""
    def __init__(self, ad_token, parametreler_list, govde_node, donus_tipi_token=None):
        self.ad = ad_token                 # İşlecin adı (örn: topla)
        self.parametreler = parametreler_list  # Parametre(ler) listesi
        self.govde = govde_node            # Komutların olduğu Blok
        self.donus_tipi = donus_tipi_token # Opsiyonel dönüş tipi (örn: SAYISAL)

class DonusKomutu(ASTNode):
    """DÖNÜŞ ifade; komutunu temsil eder."""
    def __init__(self, ifade_node):
        self.ifade = ifade_node # Döndürülecek değer (AST Düğümü)

class IslecCagirma(ASTNode):
    """Bir işlecin çağrılmasını temsil eder (örn: topla(5, 10))."""
    def __init__(self, hedef_token, argumanlar_listesi):
        self.hedef_token = hedef_token       # Çağrılan işlecin adı (Token)
        self.argumanlar = argumanlar_listesi # Gönderilen argümanlar (İfadelerin listesi)

class IfadeKomutu(ASTNode):
    """Sadece bir ifadeden oluşan komut (örn: selam_ver();)."""
    def __init__(self, ifade_node):
        self.ifade = ifade_node

class DiziTanimlama(ASTNode):
    """SAYISAL liste[10]; gibi bir dizi tanımını temsil eder."""
    def __init__(self, tip_token, ad_token, boyut_node):
        self.tip = tip_token
        self.ad = ad_token
        self.boyut = boyut_node # Boyut bir ifade olabilir, örn: [s + 5]

class DiziErisim(ASTNode):
    """liste[3] gibi bir dizi elemanına erişimi temsil eder."""
    def __init__(self, hedef_node, indeks_node):
        self.hedef = hedef_node   # Erişilen dizi (ast_nodes.Degisken)
        self.indeks = indeks_node # İndeks bir ifade olabilir, örn: [s + 1]

class SozlukErisim(ASTNode):
    """Sözlük elemanına erişimi temsil eder (örn: sozluk{"anahtar"})"""
    def __init__(self, hedef_node, anahtar_node):
        # DiziErisim ile tutarlı hale getirildi
        self.hedef = hedef_node      # Erişilen sözlük (ast_nodes.Degisken olmalı)
        self.anahtar = anahtar_node  # Anahtar bir ifade olabilir

    def __repr__(self):
        # Artık 'hedef' bir Degisken düğümü olduğu için .ad kullanıyoruz
        return f"SozlukErisim(hedef={self.hedef.ad}, anahtar={self.anahtar})"