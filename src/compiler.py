# src/compiler.py
import sys
import os # 'build' klasörünü oluşturmak için eklendi
from src.lexer import Lexer
from src.parser import Parser
from src.tokens import TOKEN_TIPLERI
import src.ast_nodes as nodes
from src.generator import Generator # Generator'ü içe aktar

# --- AST Yazdırma (Hata Ayıklama için) ---
# Bu fonksiyon, AST'nin yapısını görselleştirmek için kullanılır.
# 'calistir' fonksiyonunda şimdilik çağrılmıyor, ancak gelecekte
# hata ayıklama için (ast_yazdir(ast)) kullanılabilir.
def ast_yazdir(node, girinti=""):
    if isinstance(node, nodes.Blok):
        print(f"{girinti}Blok:")
        for komut in node.komutlar:
            ast_yazdir(komut, girinti + "  ")
    elif isinstance(node, nodes.DegiskenTanimlama):
        print(f"{girinti}DegiskenTanimlama: Tip={node.tip.deger}, Ad={node.ad.deger}")
        print(f"{girinti}  Atanan Deger:")
        ast_yazdir(node.ifade, girinti + "    ")
    elif isinstance(node, nodes.DiziTanimlama):
         print(f"{girinti}Dizi Tanımlama: {node.ad.deger} (Tip: {node.tip.deger})")
         print(f"{girinti}  Boyut:")
         ast_yazdir(node.boyut, girinti + "    ")
    elif isinstance(node, nodes.AtamaKomutu):
        print(f"{girinti}Atama Komutu:")
        print(f"{girinti}  Hedef (Değişken):")
        ast_yazdir(node.hedef, girinti + "    ")
        print(f"{girinti}  Atanan Deger (İfade):")
        ast_yazdir(node.ifade, girinti + "    ")
    elif isinstance(node, nodes.EgerBlogu):
        print(f"{girinti}EGER Blogu:")
        print(f"{girinti}  Kosul:")
        ast_yazdir(node.kosul, girinti + "    ")
        print(f"{girinti}  Dogruysa Yapilacaklar (IF):")
        ast_yazdir(node.dogru_blok, girinti + "    ")
        if node.yanlis_blok:
            print(f"{girinti}  Degilse Yapilacaklar (ELSE):")
            ast_yazdir(node.yanlis_blok, girinti + "    ")
    elif isinstance(node, nodes.DonguBlogu):
        print(f"{girinti}DÖNGÜ Blogu:")
        print(f"{girinti}  Döngü İçi Komutlar:")
        ast_yazdir(node.blok, girinti + "    ")
    elif isinstance(node, nodes.DonguBitirKomutu):
        print(f"{girinti}DÖNGÜ_BITIR Komutu (break)")
    elif isinstance(node, nodes.YazdirKomutu):
        print(f"{girinti}YAZDIR Komutu:")
        print(f"{girinti}  Yazdırılacak İfade:")
        ast_yazdir(node.ifade, girinti + "    ")
    elif isinstance(node, nodes.IfadeKomutu):
         print(f"{girinti}İfade Komutu (Procedure Call):")
         ast_yazdir(node.ifade, girinti + "    ")
    elif isinstance(node, nodes.IslecTanimlama):
        donus_tipi = node.donus_tipi.deger if node.donus_tipi else "HİÇLİK"
        print(f"{girinti}İŞLEÇ Tanımlama: {node.ad.deger} (Dönüş Tipi: {donus_tipi})")
        print(f"{girinti}  Parametreler:")
        if not node.parametreler:
            print(f"{girinti}    (Yok)")
        for param in node.parametreler:
            ast_yazdir(param, girinti + "    ")
        print(f"{girinti}  Gövde:")
        ast_yazdir(node.govde, girinti + "    ")
    elif isinstance(node, nodes.Parametre):
        print(f"{girinti}Parametre: {node.ad.deger} (Tip: {node.tip.deger})")
    elif isinstance(node, nodes.DonusKomutu):
        print(f"{girinti}DÖNÜŞ Komutu:")
        print(f"{girinti}  Döndürülecek İfade:")
        ast_yazdir(node.ifade, girinti + "    ")
    elif isinstance(node, nodes.IslecCagirma):
        print(f"{girinti}İŞLEÇ Çağırma: {node.hedef_token.deger}")
        print(f"{girinti}  Argümanlar:")
        if not node.argumanlar:
            print(f"{girinti}    (Yok)")
        for arg in node.argumanlar:
            ast_yazdir(arg, girinti + "    ")
    elif isinstance(node, nodes.DiziErisim):
        print(f"{girinti}Dizi Erişimi:")
        print(f"{girinti}  Hedef:")
        ast_yazdir(node.hedef, girinti + "    ")
        print(f"{girinti}  İndeks:")
        ast_yazdir(node.indeks, girinti + "    ")
    elif isinstance(node, nodes.Sayi):
        print(f"{girinti}Sayi: {node.deger}")
    elif isinstance(node, nodes.Metin):
        print(f"{girinti}Metin: {node.deger}")
    elif isinstance(node, nodes.Degisken):
        print(f"{girinti}DegiskenReferansi: {node.ad}")
    elif isinstance(node, nodes.IkiliIslem):
        print(f"{girinti}Islem: {node.operator.tip}")
        print(f"{girinti}  Sol:")
        ast_yazdir(node.sol, girinti + "    ")
        print(f"{girinti}  Sag:")
        ast_yazdir(node.sag, girinti + "    ")
    else:
        print(f"{girinti}Bilinmeyen Dugum: {type(node)}")


# --- ANA ÇALIŞTIRICI ---
def calistir(dosya_adi, dil_json_yolu, dil_kodu='tr-TR'):
    
    # Dosya Okuma
    try:
        with open(dosya_adi, 'r', encoding='utf-8') as f:
            kaynak_kodu = f.read()
    except Exception as e:
        print(f"HATA: Dosya okunamadı: {e}"); sys.exit(1)

    print(f"--- KAYNAK KOD ({dosya_adi}) ---")
    print(kaynak_kodu)
    print("-" * 40)

    # 1. Lexer
    try:
        lexer = Lexer(kaynak_kodu, dil_json_yolu, dil_kodu)
    except Exception as e:
        print(f"HATA (Lexer): {e}"); sys.exit(1)

    # 2. Parser
    print("\n--- PARSER (AST) ---")
    try:
        parser = Parser(lexer)
        ast = parser.parse()
        print("✅ Ayrıştırma Başarılı!")
            
    except Exception as e:
        print(f"\n!!! HATA (Parser): {e}")
        sys.exit(1)
        
    # 3. Generator
    print("\n--- GENERATOR (ASM) ---")
    try:
        generator = Generator(ast)
        asm_kodu = generator.generate()
        
        print(asm_kodu) # Üretilen Assembly kodunu ekrana bas
        
        output_asm_file = "build/output.asm"
        # 'build' klasörünün var olduğundan emin ol
        os.makedirs("build", exist_ok=True) 
        
        with open(output_asm_file, "w", encoding='utf-8') as f:
            f.write(asm_kodu)
        print(f"\n✅ Assembly Kodu Başarıyla Üretildi -> {output_asm_file}")
            
    except Exception as e:
        print(f"\n!!! HATA (Generator): {e}")
        sys.exit(1)

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='TYD-MLP Compiler')
    parser.add_argument('kaynak', help='TYD kaynak dosyası (.tyd)')
    parser.add_argument('dil_json', nargs='?', default='kurallar/diller.json', 
                        help='Dil kuralları JSON dosyası (varsayılan: kurallar/diller.json)')
    parser.add_argument('--dil-kodu', default='tr-TR', 
                        help='Dil kodu (varsayılan: tr-TR)')
    
    args = parser.parse_args()
    
    calistir(args.kaynak, args.dil_json, dil_kodu=args.dil_kodu)
