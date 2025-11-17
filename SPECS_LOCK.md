# 🔒 TYD Spesifikasyon Değişiklik Günlüğü

## ⚠️ SPECS.md'yi Değiştirmeden Önce OKUMANIZ GEREKENLER

1. **Değişiklik nedenini** bu dosyaya yaz
2. **Etkilenen dosyaları** listele
3. **Tüm testleri** çalıştır ve kaydet
4. **Git commit** yap
5. **İnceleme** bekle (takım projelerinde)

---

## 📋 Değişiklik Formatı

```markdown
### YYYY-MM-DD - vX.Y
**Değiştiren:** [Kişi Adı]
**Neden:** [Değişiklik sebebi]
**Etkilenen Dosyalar:**
- [ ] c_lexer.c
- [ ] c_parser.c
- [ ] c_generator.c
- [ ] ornekler/*.tyd
**Testler:** 
- [ ] test_01_merhaba.tyd - ✅ Geçti
- [ ] test_02_fonksiyon.tyd - ✅ Geçti
**Onay:** ✅ [Proje Sahibi]
**Commit:** [git hash]
```

---

## 📜 Değişiklik Geçmişi

### 2024-11-17 - v1.0 (İlk Sürüm)
**Değiştiren:** Güven Acar  
**Neden:** TYD dilinin ilk spesifikasyon kilidi oluşturuldu  
**Değişiklikler:**
- Noktalı virgül (`;`) sadece değişken tanımında kullanılacak
- Blok sonlandırma: `SON ANAHTAR` formatı zorunlu
- Yorum söz dizimi: `--` (tek satır), `---` (çok satır)
- Tüm kontrol yapıları noktalı virgül kullanmayacak

**Etkilenen Dosyalar:**
- [x] SPECS.md - İlk oluşturma
- [ ] c_parser.c - Henüz güncellenmedi
- [ ] ornekler/*.tyd - Henüz güncellenmedi

**Testler:** ⚠️ Henüz çalıştırılmadı

**Durum:** 🚧 Devam ediyor

---

### ÖRNEK - Gelecekteki Değişiklik

```markdown
### 2024-11-20 - v1.1
**Değiştiren:** AI Assistant
**Neden:** DİZİ veri tipi eklendi
**Değişiklikler:**
- Yeni anahtar kelime: DİZİ
- Erişim: dizi[index]
- Tanımlama: DİZİ sayilar = [1, 2, 3];

**Etkilenen Dosyalar:**
- [x] SPECS.md
- [x] c_lexer.c - TOKEN_DIZI eklendi
- [x] c_parser.c - parse_array() eklendi
- [x] c_generator.c - Dizi assembly desteği
- [x] ornekler/06-diziler.tyd - Yeni test

**Testler:**
- [x] test_06_diziler.tyd - ✅ Geçti
- [x] test_all.sh - ✅ Tüm testler geçti

**Onay:** ✅ Güven Acar
**Commit:** abc123def456
```

---

## 🛡️ Koruma Katmanları

1. **📖 SPECS.md** - Kaynak gerçek
2. **📝 Bu dosya (SPECS_LOCK.md)** - Değişiklik günlüğü
3. **🔍 validate_syntax.py** - Otomatik kontrol
4. **🪝 .git/hooks/pre-commit** - Commit öncesi engel
5. **🤖 .cursorrules** - AI kısıtlamaları
6. **☁️ GitHub Actions** - CI/CD doğrulama

---

## 📞 İletişim

Değişiklik önerileri için:
- Issue aç: `github.com/[proje]/issues`
- Pull Request: Önce bu dosyaya kayıt ekle
- E-posta: [email]

---

**Son Güncelleme:** 17 Kasım 2024

<!-- filepath: /home/pardus/projeler/tyd-lang/TYD-MLP/tools/validate_syntax.py -->
#!/usr/bin/env python3
"""
TYD Syntax Validator - SPECS.md kurallarını zorunlu kılar
Her commit öncesi çalışmalı (git pre-commit hook)
"""

import re
import sys
from pathlib import Path

SPECS_VIOLATIONS = []

def check_semicolon_usage(file_path, content):
    """Kural 1: ; sadece değişken tanımında"""
    lines = content.split('\n')
    
    for i, line in enumerate(lines, 1):
        line = line.strip()
        
        # Yorum satırlarını atla
        if line.startswith('--'):
            continue
            
        # ; içeren satırlar
        if ';' in line:
            # İzin verilen: SAYISAL/METIN ... ;
            if not re.match(r'^(SAYISAL|METIN|MANTIKSAL)\s+\w+', line):
                SPECS_VIOLATIONS.append(
                    f"{file_path}:{i} - HATA: ';' sadece değişken tanımında kullanılabilir\n"
                    f"  Satır: {line}"
                )

def check_block_endings(file_path, content):
    """Kural 2: Blok sonlandırma formatı"""
    lines = content.split('\n')
    
    for i, line in enumerate(lines, 1):
        line = line.strip()
        
        # SON; kullanımı yasak
        if re.match(r'^SON\s*;', line):
            SPECS_VIOLATIONS.append(
                f"{file_path}:{i} - HATA: 'SON;' kullanılamaz\n"
                f"  Doğrusu: 'SON İŞLEÇ' veya 'SON EĞER'\n"
                f"  Satır: {line}"
            )
        
        # SON İŞLEÇ; yasak
        if re.match(r'^SON\s+İŞLEÇ\s*;', line):
            SPECS_VIOLATIONS.append(
                f"{file_path}:{i} - HATA: 'SON İŞLEÇ;' yanlış\n"
                f"  Doğrusu: 'SON İŞLEÇ' (noktalı virgül yok)\n"
                f"  Satır: {line}"
            )

def validate_file(file_path):
    """Tek bir .tyd dosyasını kontrol et"""
    try:
        content = file_path.read_text(encoding='utf-8')
        check_semicolon_usage(file_path, content)
        check_block_endings(file_path, content)
    except Exception as e:
        print(f"UYARI: {file_path} okunamadı: {e}")

def main():
    # Tüm .tyd dosyalarını bul
    root = Path(__file__).parent.parent
    tyd_files = list(root.glob('**/*.tyd'))
    
    print(f"🔍 {len(tyd_files)} TYD dosyası kontrol ediliyor...")
    
    for tyd_file in tyd_files:
        validate_file(tyd_file)
    
    if SPECS_VIOLATIONS:
        print("\n❌ SPECS.md İHLALLERİ TESPİT EDİLDİ:\n")
        print('\n'.join(SPECS_VIOLATIONS))
        print(f"\n🚫 Toplam {len(SPECS_VIOLATIONS)} hata bulundu!")
        print("📖 SPECS.md dosyasını kontrol edin.")
        return 1
    else:
        print("✅ Tüm dosyalar SPECS.md'ye uygun!")
        return 0

if __name__ == '__main__':
    sys.exit(main())