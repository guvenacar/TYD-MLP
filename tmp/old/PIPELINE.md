# TYD-MLP Implementation Pipeline

**Amaç:** Syntax Preprocessor'ı adım adım implement etmek için rehber  
**Hedef:** Multi-syntax desteği eklemek  
**Tahmini Süre:** 2-3 hafta

---

## 📋 İçindekiler

1. [Genel Bakış](#genel-bakış)
2. [Aşama 0: Hazırlık](#aşama-0-hazırlık)
3. [Aşama 1: Syntax Rules JSON](#aşama-1-syntax-rules-json)
4. [Aşama 2: Basit Preprocessor](#aşama-2-basit-preprocessor)
5. [Aşama 3: Context-Aware Parser](#aşama-3-context-aware-parser)
6. [Aşama 4: Source Mapping](#aşama-4-source-mapping)
7. [Aşama 5: Error Reporting](#aşama-5-error-reporting)
8. [Aşama 6: Test & Documentation](#aşama-6-test--documentation)
9. [Bonus: VSCode Extension](#bonus-vscode-extension)

---

## 🎯 Genel Bakış

### Mevcut Durum
```
Source Code (*.tyd)
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Generator] → Assembly
```

### Hedef Durum
```
Source Code (any syntax)
    ↓
[Syntax Preprocessor] → Normalized TYD    ← YENİ!
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Generator] → Assembly
```

### Başarı Kriterleri

✅ **Minimum Viable Product (MVP):**
- [ ] 3 syntax style desteği (Python, Java, Native TYD)
- [ ] Temel yapılar (if, function, loop)
- [ ] Basit transformasyon (regex-based)
- [ ] CLI flag: `--syntax python-style`

✅ **Production Ready:**
- [ ] 5+ syntax style
- [ ] Nested blocks desteği
- [ ] Source mapping (hata satır numaraları)
- [ ] Context-aware parsing
- [ ] Comprehensive tests

---

## 🚀 Aşama 0: Hazırlık

### Süre: 1-2 gün

### Görevler

#### 1. Test Dosyaları Hazırlama

**ornekler/test_native.tyd** (Baseline)
```tyd
-- Native TYD syntax
SAYISAL IŞLEÇ topla(SAYISAL a, SAYISAL b)
    DÖNÜŞ a + b
IŞLEÇ SON

SAYISAL x = 5;
SAYISAL y = 10;

EĞER x > 3 İSE
    YAZDIR "x büyük";
DEĞILSE
    YAZDIR "x küçük";
EĞER SON

SAYISAL sonuc = topla(x, y);
YAZDIR sonuc;
```

**ornekler/test_python.tyd** (Python-style)
```python
# Python-style syntax
def topla(a, b):
    return a + b

x = 5
y = 10

if x > 3:
    print("x büyük")
else:
    print("x küçük")

sonuc = topla(x, y)
print(sonuc)
```

**ornekler/test_java.tyd** (Java-style)
```java
// Java-style syntax
int topla(int a, int b) {
    return a + b;
}

int x = 5;
int y = 10;

if (x > 3) {
    print("x büyük");
} else {
    print("x küçük");
}

int sonuc = topla(x, y);
print(sonuc);
```

#### 2. Beklenen Çıktıyı Doğrulama

Tüm 3 dosya **aynı Assembly'i** üretmeli:

```bash
# Native TYD
python -m src.compiler ornekler/test_native.tyd
mv build/output.asm build/native.asm

# Python-style (henüz çalışmaz)
# python -m src.compiler ornekler/test_python.tyd --syntax python-style

# Java-style (henüz çalışmaz)
# python -m src.compiler ornekler/test_java.tyd --syntax java-style
```

**Karşılaştırma:**
```bash
diff build/native.asm build/python.asm  # Aynı olmalı
diff build/native.asm build/java.asm    # Aynı olmalı
```

#### 3. Proje Yapısını Güncelleyin

```bash
mkdir -p kurallar/syntax
mkdir -p tests/preprocessor
mkdir -p docs/tutorials
```

**Dosyalar:**
```
TYD-MLP/
├── src/
│   └── syntax_preprocessor.py    ← YENİ
├── kurallar/
│   └── syntax_styles.json        ← YENİ
├── tests/
│   ├── test_preprocessor.py      ← YENİ
│   └── fixtures/
│       ├── python_samples/       ← YENİ
│       ├── java_samples/         ← YENİ
│       └── expected_output/      ← YENİ
└── ornekler/
    ├── test_python.tyd           ← YENİ
    └── test_java.tyd             ← YENİ
```

---

## 📝 Aşama 1: Syntax Rules JSON

### Süre: 1 gün

### Görev: `kurallar/syntax_styles.json` Oluşturmak

#### Dosya Yapısı

```json
{
  "native": {
    "name": "Native TYD Syntax",
    "description": "TYD'nin varsayılan söz dizimi",
    "enabled": true,
    "rules": []
  },
  
  "python-style": {
    "name": "Python-like Syntax",
    "description": "Python syntax'ına benzer yapı",
    "enabled": true,
    "priority": 1,
    "indentation": {
      "based": true,
      "size": 4
    },
    "rules": [
      {
        "id": "py_function_def",
        "name": "Function Definition",
        "pattern": "^def\\s+(\\w+)\\s*\\(([^)]*)\\):\\s*$",
        "replace": "İŞLEÇ \\1(\\2)",
        "flags": ["MULTILINE"],
        "examples": [
          {
            "input": "def topla(a, b):",
            "output": "İŞLEÇ topla(a, b)"
          }
        ]
      },
      {
        "id": "py_return",
        "name": "Return Statement",
        "pattern": "^\\s*return\\s+(.+)$",
        "replace": "DÖNÜŞ \\1",
        "flags": ["MULTILINE"]
      },
      {
        "id": "py_if",
        "name": "If Statement",
        "pattern": "^if\\s+(.+?):\\s*$",
        "replace": "EĞER \\1 İSE",
        "flags": ["MULTILINE"]
      },
      {
        "id": "py_else",
        "name": "Else Statement",
        "pattern": "^else:\\s*$",
        "replace": "DEĞİLSE",
        "flags": ["MULTILINE"]
      },
      {
        "id": "py_print",
        "name": "Print Function",
        "pattern": "print\\((.+?)\\)",
        "replace": "YAZDIR \\1",
        "flags": []
      }
    ]
  },
  
  "java-style": {
    "name": "Java/C-like Syntax",
    "description": "Java/C/C++ benzeri syntax",
    "enabled": true,
    "priority": 2,
    "braces": {
      "based": true
    },
    "rules": [
      {
        "id": "java_function_def",
        "name": "Function Definition",
        "pattern": "(\\w+)\\s+(\\w+)\\s*\\(([^)]*)\\)\\s*\\{",
        "replace": "İŞLEÇ \\2(\\3)",
        "notes": "Return type (\\1) atılıyor - TYD type inference kullanacak"
      },
      {
        "id": "java_return",
        "name": "Return Statement",
        "pattern": "return\\s+(.+?);",
        "replace": "DÖNÜŞ \\1;"
      },
      {
        "id": "java_if",
        "name": "If Statement",
        "pattern": "if\\s*\\((.+?)\\)\\s*\\{",
        "replace": "EĞER \\1 İSE"
      },
      {
        "id": "java_else",
        "name": "Else Statement",
        "pattern": "\\}\\s*else\\s*\\{",
        "replace": "DEĞİLSE"
      },
      {
        "id": "java_closing_brace",
        "name": "Closing Brace",
        "pattern": "^\\s*\\}\\s*$",
        "replace": "SON",
        "flags": ["MULTILINE"]
      },
      {
        "id": "java_print",
        "name": "Print Function",
        "pattern": "print\\((.+?)\\);",
        "replace": "YAZDIR \\1;"
      }
    ]
  }
}
```

#### Test

```bash
# JSON syntax kontrolü
python -m json.tool kurallar/syntax_styles.json
```

---

## 🔧 Aşama 2: Basit Preprocessor

### Süre: 2-3 gün

### Görev: `src/syntax_preprocessor.py` v1.0 (Regex-based)

#### Implementasyon

```python
# src/syntax_preprocessor.py

import re
import json
from pathlib import Path
from typing import Dict, List, Optional

class SyntaxPreprocessor:
    """
    Kullanıcının syntax'ını TYD base syntax'ına dönüştürür.
    
    v1.0: Basit regex-based transformation
    """
    
    def __init__(self, syntax_style: str = 'native'):
        """
        Args:
            syntax_style: 'native', 'python-style', 'java-style', vb.
        """
        self.syntax_style = syntax_style
        self.rules = []
        self.config = {}
        
        if syntax_style != 'native':
            self._load_rules()
    
    def _load_rules(self):
        """syntax_styles.json'dan kuralları yükler"""
        config_path = Path(__file__).parent.parent / 'kurallar' / 'syntax_styles.json'
        
        with open(config_path, 'r', encoding='utf-8') as f:
            all_styles = json.load(f)
        
        if self.syntax_style not in all_styles:
            raise ValueError(f"Syntax style '{self.syntax_style}' bulunamadı.")
        
        self.config = all_styles[self.syntax_style]
        
        if not self.config.get('enabled', True):
            raise ValueError(f"Syntax style '{self.syntax_style}' devre dışı.")
        
        self.rules = self.config.get('rules', [])
        
        print(f"[Preprocessor] Syntax style yüklendi: {self.config['name']}")
        print(f"[Preprocessor] {len(self.rules)} kural tanımlı.")
    
    def transform(self, source_code: str) -> str:
        """
        Kaynak kodu normalize eder.
        
        Args:
            source_code: Kullanıcının yazdığı kod
            
        Returns:
            Normalized TYD kodu
        """
        if self.syntax_style == 'native':
            return source_code  # No transformation
        
        transformed = source_code
        
        # Her kuralı sırayla uygula
        for rule in self.rules:
            pattern = rule['pattern']
            replacement = rule['replace']
            flags = self._parse_flags(rule.get('flags', []))
            
            try:
                transformed = re.sub(pattern, replacement, transformed, flags=flags)
            except re.error as e:
                print(f"[Preprocessor] UYARI: Kural '{rule['id']}' uygulanamadı: {e}")
        
        # Python-style için: Block sonu işaretlerini ekle
        if self.config.get('indentation', {}).get('based', False):
            transformed = self._add_block_endings(transformed)
        
        return transformed
    
    def _parse_flags(self, flag_list: List[str]) -> int:
        """Regex flag'lerini parse eder"""
        flags = 0
        for flag_name in flag_list:
            if hasattr(re, flag_name):
                flags |= getattr(re, flag_name)
        return flags
    
    def _add_block_endings(self, code: str) -> str:
        """
        Python-style indent-based bloklar için 'SON' ekler.
        
        Basit algoritma:
        - Indent artarsa: blok başlıyor
        - Indent azalırsa: 'SON' ekle
        """
        lines = code.split('\n')
        result = []
        indent_stack = [0]  # Başlangıç indent seviyesi
        
        for line in lines:
            stripped = line.lstrip()
            if not stripped or stripped.startswith('#'):
                result.append(line)
                continue
            
            current_indent = len(line) - len(stripped)
            
            # Indent azaldı mı?
            while indent_stack and current_indent < indent_stack[-1]:
                indent_stack.pop()
                # Blok kapandı, 'SON' ekle
                result.append(' ' * indent_stack[-1] + 'SON')
            
            # Normal satırı ekle
            result.append(line)
            
            # Blok başlatıcı mı? (İSE, DEĞİLSE ile bitiyorsa)
            if stripped.endswith(' İSE') or stripped == 'DEĞİLSE':
                indent_stack.append(current_indent + 4)  # 4 space indent
        
        # Dosya sonu: Tüm açık blokları kapat
        while len(indent_stack) > 1:
            indent_stack.pop()
            result.append(' ' * indent_stack[-1] + 'SON')
        
        return '\n'.join(result)
```

#### Test

```python
# tests/test_preprocessor.py

import pytest
from src.syntax_preprocessor import SyntaxPreprocessor

def test_native_passthrough():
    """Native syntax değişmemeli"""
    preprocessor = SyntaxPreprocessor('native')
    code = "SAYISAL x = 5;"
    assert preprocessor.transform(code) == code

def test_python_function():
    """Python function → TYD IŞLEÇ"""
    preprocessor = SyntaxPreprocessor('python-style')
    
    input_code = "def topla(a, b):\n    return a + b"
    expected = "İŞLEÇ topla(a, b)\n    DÖNÜŞ a + b\nSON"
    
    result = preprocessor.transform(input_code)
    assert result.strip() == expected.strip()

def test_java_if():
    """Java if → TYD EĞER"""
    preprocessor = SyntaxPreprocessor('java-style')
    
    input_code = "if (x > 5) {\n    print(\"büyük\");\n}"
    expected = "EĞER x > 5 İSE\n    YAZDIR \"büyük\";\nSON"
    
    result = preprocessor.transform(input_code)
    assert result.strip() == expected.strip()
```

```bash
# Testleri çalıştır
pytest tests/test_preprocessor.py -v
```

---

## 🔗 Aşama 3: Compiler Entegrasyonu

### Süre: 1 gün

### Görev: `compiler.py`'yi Güncellemek

#### Değişiklikler

```python
# src/compiler.py

import argparse
from src.syntax_preprocessor import SyntaxPreprocessor

def calistir(dosya_adi, dil_json_yolu, dil_kodu='tr-TR', syntax_style='native'):
    
    # Dosya Okuma
    try:
        with open(dosya_adi, 'r', encoding='utf-8') as f:
            kaynak_kodu = f.read()
    except Exception as e:
        print(f"HATA: Dosya okunamadı: {e}")
        sys.exit(1)

    print(f"--- KAYNAK KOD ({dosya_adi}) ---")
    print(kaynak_kodu)
    print("-" * 40)

    # 🆕 SYNTAX PREPROCESSING
    preprocessor = SyntaxPreprocessor(syntax_style)
    normalized_kod = preprocessor.transform(kaynak_kodu)
    
    if syntax_style != 'native':
        print(f"\n--- NORMALIZED TYD (from {syntax_style}) ---")
        print(normalized_kod)
        print("-" * 40)

    # 1. Lexer (normalized kod ile)
    try:
        lexer = Lexer(normalized_kod, dil_json_yolu, dil_kodu)
    except Exception as e:
        print(f"HATA (Lexer): {e}")
        sys.exit(1)

    # ... geri kalan kod aynı ...

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='TYD-MLP Compiler')
    parser.add_argument('input', help='TYD kaynak dosyası')
    parser.add_argument('--dil', default='kurallar/diller.json', help='Dil JSON dosyası')
    parser.add_argument('--syntax', default='native', 
                        choices=['native', 'python-style', 'java-style'],
                        help='Syntax style')
    
    args = parser.parse_args()
    
    calistir(args.input, args.dil, dil_kodu='tr-TR', syntax_style=args.syntax)
```

#### Test

```bash
# Python-style test
python -m src.compiler ornekler/test_python.tyd --syntax python-style
./build/program

# Java-style test
python -m src.compiler ornekler/test_java.tyd --syntax java-style
./build/program

# Native (baseline)
python -m src.compiler ornekler/test_native.tyd
./build/program
```

**Beklenen:** Hepsi aynı çıktıyı vermeli!

---

## 🗺️ Aşama 4: Source Mapping

### Süre: 2 gün

### Görev: Hata Mesajlarında Orijinal Satır Numaralarını Göstermek

#### Neden Gerekli?

```python
# Kullanıcının yazdığı (test_python.tyd:5)
if x > 3:
    print("büyük")
```

**Preprocessor sonrası:**
```tyd
EĞER x > 3 İSE
    YAZDIR "büyük"
SON
```

**Hata olursa:**
```
HATA (Parser): Satır 2: 'YAZDIR' bekleniyordu, 'YAAAAZDIR' bulundu.
```

**Kullanıcı şaşırır:** "Ben satır 2'ye hiç `YAZDIR` yazmadım ki!"

**Çözüm:** Source map ile orijinal satırı göster:
```
HATA (Parser): test_python.tyd:6: 'print' bekleniyordu, 'printt' bulundu.
```

#### Implementasyon

```python
# src/syntax_preprocessor.py

class SourceMap:
    """
    Transformed line → Original line mapping
    """
    def __init__(self):
        self.mappings = {}  # {transformed_line: (file, original_line)}
    
    def add(self, transformed_line: int, original_file: str, original_line: int):
        self.mappings[transformed_line] = (original_file, original_line)
    
    def get_original(self, transformed_line: int):
        return self.mappings.get(transformed_line)

class SyntaxPreprocessor:
    # ... (önceki kod) ...
    
    def transform(self, source_code: str, source_file: str = '<input>') -> tuple[str, SourceMap]:
        """
        Returns:
            (transformed_code, source_map)
        """
        if self.syntax_style == 'native':
            return source_code, SourceMap()
        
        lines = source_code.split('\n')
        transformed_lines = []
        source_map = SourceMap()
        
        transformed_line_num = 1
        
        for original_line_num, line in enumerate(lines, 1):
            # Transform
            new_lines = self._transform_line(line)
            
            for new_line in new_lines:
                transformed_lines.append(new_line)
                source_map.add(transformed_line_num, source_file, original_line_num)
                transformed_line_num += 1
        
        return '\n'.join(transformed_lines), source_map
```

#### Error Reporting

```python
# src/parser.py

class Parser:
    def __init__(self, lexer, source_map=None):
        self.lexer = lexer
        self.source_map = source_map
        # ...
    
    def hata(self, mesaj):
        error_line = self.mevcut_token.satir  # Token'dan satır numarası
        
        if self.source_map:
            original = self.source_map.get_original(error_line)
            if original:
                file, line = original
                raise Exception(f"[Sözdizimi Hatası] {file}:{line}: {mesaj}")
        
        raise Exception(f"[Sözdizimi Hatası] Satır {error_line}: {mesaj}")
```

---

## ✅ Aşama 5: Test & Documentation

### Süre: 2-3 gün

### Görevler

#### 1. Comprehensive Tests

```python
# tests/test_preprocessor_comprehensive.py

class TestPythonStyle:
    def test_simple_function(self):
        ...
    
    def test_nested_if(self):
        ...
    
    def test_for_loop(self):  # Gelecekte
        ...

class TestJavaStyle:
    def test_simple_function(self):
        ...
    
    def test_nested_blocks(self):
        ...
    
    def test_multiline_expression(self):
        ...
```

#### 2. Integration Tests

```bash
# tests/integration/test_end_to_end.sh

#!/bin/bash

# Tüm syntax style'ları test et
for style in native python-style java-style; do
    echo "Testing $style..."
    python -m src.compiler ornekler/test_$style.tyd --syntax $style
    ./build/program > output_$style.txt
done

# Çıktıları karşılaştır
diff output_native.txt output_python-style.txt
diff output_native.txt output_java-style.txt

echo "✅ All outputs match!"
```

#### 3. Documentation

**docs/syntax-styles.md:**
```markdown
# Syntax Styles Rehberi

## Python-Style

### Desteklenen Özellikler
- [x] Function definition (`def`)
- [x] If/else
- [ ] For loops (TODO)
- [ ] Classes (TODO)

### Örnek
...

## Kendi Style'ınızı Ekleyin
...
```

---

## 🎉 Başarı Kriterleri Kontrolü

### MVP Checklist

- [ ] `syntax_styles.json` oluşturuldu
- [ ] `syntax_preprocessor.py` çalışıyor
- [ ] 3 syntax style test edildi (native, python, java)
- [ ] `--syntax` CLI flag çalışıyor
- [ ] Temel yapılar dönüştürülüyor (if, function)
- [ ] Tests yazıldı ve geçiyor
- [ ] README güncellendi

### Ekstra Özellikler

- [ ] Source mapping
- [ ] Context-aware parsing
- [ ] 5+ syntax style
- [ ] Nested blocks
- [ ] VSCode extension

---

## 📊 Milestone Timeline

```
Week 1:
├─ Day 1-2: Hazırlık (Aşama 0, 1)
├─ Day 3-5: Basit preprocessor (Aşama 2)
└─ Day 6-7: Entegrasyon (Aşama 3)

Week 2:
├─ Day 8-9: Source mapping (Aşama 4)
├─ Day 10-12: Testing (Aşama 5)
└─ Day 13-14: Documentation

Week 3:
├─ Ekstra syntax styles
├─ Context-aware improvements
└─ Community feedback
```

---

## 🐛 Bilinen Sorunlar ve Çözümler

### Sorun 1: Nested Blocks

**Problem:**
```python
if x > 0:
    if y > 0:
        print("both positive")
```

**Çözüm:** Indent tracking stack

### Sorun 2: Multiline Expressions

**Problem:**
```python
result = (x + y +
          z + w)
```

**Çözüm:** Line continuation detection

### Sorun 3: Comments

**Problem:**
```python
def foo():  # This is a comment
    pass
```

**Çözüm:** Comment stripping pre-pass

---

## 🚀 Next Steps (Aşama 7+)

1. **More Syntax Styles:**
   - Go-style
   - Rust-style
   - Ruby-style
   - Pascal-style

2. **Advanced Features:**
   - Type annotations
   - Decorators
   - List comprehensions
   - Lambda functions

3. **Tooling:**
   - VSCode extension
   - Online playground
   - Syntax converter web tool

---

**Happy Coding! 🎯**

