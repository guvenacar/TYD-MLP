# TYD-MLP Mimari Spesifikasyonları

**Versiyon:** 0.1.0-alpha  
**Son Güncelleme:** 12 Kasım 2024  
**Durum:** Alpha - Aktif Geliştirme

---

## 📋 İçindekiler

1. [Vizyon ve Felsefe](#vizyon-ve-felsefe)
2. [Mimari Genel Bakış](#mimari-genel-bakış)
3. [Katman Detayları](#katman-detayları)
4. [Syntax Preprocessor](#syntax-preprocessor)
5. [Lexer (Jetonlayıcı)](#lexer-jetonlayıcı)
6. [Parser (Ayrıştırıcı)](#parser-ayrıştırıcı)
7. [AST (Abstract Syntax Tree)](#ast-abstract-syntax-tree)
8. [Generator (Kod Üretici)](#generator-kod-üretici)
9. [C Runtime](#c-runtime)
10. [Tasarım Kararları](#tasarım-kararları)
11. [Performans Hedefleri](#performans-hedefleri)
12. [Gelecek Planları](#gelecek-planları)

---

## 🎯 Vizyon ve Felsefe

### Temel İlkeler

#### 1. **%100 Özelleştirilebilirlik**
> "Hiçbir sözdizimsel sembol veya anahtar kelime, Lexer veya Parser içerisinde sabit kodlu (hard-coded) olmamalıdır."

- ✅ **Başarı:** Anahtar kelimeler `diller.json`'dan yükleniyor
- 🚧 **Devam Eden:** Semboller hala sabit kodlu (Lexer satır 182-272)
- 🎯 **Hedef:** Tüm syntax elemanları JSON-driven

#### 2. **Multi-Syntax Desteği**
> "Herkes kendi bildiği syntax'la kod yazabilmeli."

- Python geliştiricisi Python syntax'ı kullanabilmeli
- Java geliştiricisi Java syntax'ı kullanabilmeli
- Kullanıcı kendi özel syntax'ını tanımlayabilmeli

#### 3. **Platform Bağımsızlığı**
> "Ring 3'te TYD, Ring 0'da C. Sorumluluk ayrımı net."

- TYD: Yüksek seviye mantık, syntax processing
- C Runtime: Bellek yönetimi, low-level operasyonlar
- Avantaj: Herhangi bir C derleyicisi olan platformda çalışır

#### 4. **Statik Tip Sistemi**
> "TYD statik/strongly-typed bir dildir. Runtime surprise'ları yok."

- Compile-time type checking
- Explicit type declarations
- Type inference (gelecekte)

---

## 🏗️ Mimari Genel Bakış

### Katman Mimarisi

```
┌──────────────────────────────────────────────────────────┐
│  KULLANICI KATMANI (Multiple Syntaxes)                   │
│                                                            │
│  Python-style  │  Java-style  │  VB-style  │  Custom     │
└──────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────┐
│  SYNTAX PREPROCESSOR                                      │
│  - Pattern matching engine                                │
│  - Syntax normalization                                   │
│  - Source mapping (error reporting için)                  │
└──────────────────────────────────────────────────────────┘
                            ↓
                  [Normalized TYD Code]
                            ↓
┌──────────────────────────────────────────────────────────┐
│  LEXER (Tokenizer)                                        │
│  - Character stream → Token stream                        │
│  - Keyword recognition (JSON-driven)                      │
│  - Symbol recognition                                     │
└──────────────────────────────────────────────────────────┘
                            ↓
                     [Token Stream]
                            ↓
┌──────────────────────────────────────────────────────────┐
│  PARSER                                                   │
│  - Token stream → AST                                     │
│  - Syntax validation                                      │
│  - Semantic pre-checks                                    │
└──────────────────────────────────────────────────────────┘
                            ↓
                          [AST]
                            ↓
┌──────────────────────────────────────────────────────────┐
│  GENERATOR                                                │
│  - AST → Assembly                                         │
│  - Register allocation                                    │
│  - Stack management                                       │
│  - C FFI calls                                            │
└──────────────────────────────────────────────────────────┘
                            ↓
                      [Assembly Code]
                            ↓
┌──────────────────────────────────────────────────────────┐
│  C RUNTIME                                                │
│  - Memory management (malloc/free)                        │
│  - String operations                                      │
│  - Dictionary/Array operations                            │
│  - I/O operations                                         │
└──────────────────────────────────────────────────────────┘
                            ↓
                    [Native Binary]
```

### Veri Akışı

```
Source Code (*.tyd)
    ↓
[Syntax Preprocessor] → Normalized TYD
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Generator] → Assembly (*.asm)
    ↓
[GCC/NASM] → Object Files (*.o)
    ↓
[Linker] → Executable
```

---

## 🔄 Katman Detayları

### 1. Syntax Preprocessor

**Sorumluluklar:**
- Kullanıcının syntax'ını tanıma
- Pattern-based transformation
- Normalized TYD üretme
- Source mapping (orijinal satır numaraları)

**Girdi:**
```python
# Python-style
def topla(a, b):
    return a + b
```

**Çıktı:**
```tyd
İŞLEÇ topla(a, b)
    DÖNÜŞ a + b
İŞLEÇ SON
```

**Implementasyon:** `src/syntax_preprocessor.py` (TODO)

**Konfigürasyon:** `kurallar/syntax_styles.json`

```json
{
  "python-style": {
    "rules": [
      {
        "pattern": "def\\s+(\\w+)\\s*\\((.*)\\):",
        "replace": "İŞLEÇ \\1(\\2)",
        "context": "function_definition"
      }
    ]
  }
}
```

**Zorluklar:**
- Nested structures (iç içe bloklar)
- Ambiguous patterns (belirsiz kalıplar)
- Expression parsing
- Context-awareness

**Çözüm Yaklaşımı:**
1. **Fazlı (Phased) Transformation:** Önce bloklar, sonra ifadeler
2. **Stack-based Parsing:** Nested structures için
3. **Priority System:** Ambiguity resolution için
4. **Lookahead:** Context determination için

---

### 2. Lexer (Jetonlayıcı)

**Sorumluluklar:**
- Karakter akışını token'lara dönüştürme
- Anahtar kelime tanıma
- Literal değer parsing (sayılar, metinler)
- Yorum atlama

**Mevcut Durum:**

| Özellik | Durum | Notlar |
|---------|-------|--------|
| Anahtar kelimeler | ✅ JSON-driven | `diller.json` |
| Semboller | 🔴 Hard-coded | Satır 182-272 |
| Yorumlar | ✅ Çalışıyor | `--` ve `---` |
| Sayılar | ✅ Çalışıyor | Int & Float |
| Metinler | ✅ Çalışıyor | `"..."` |
| Normalization | ✅ Çalışıyor | Case-insensitive, İ/ı handling |

**Hedef:** Sembolleri de JSON-driven yapmak

```json
// kurallar/semboller.json (TODO)
{
  "symbols": {
    "ASSIGN": "=",
    "SEMICOLON": ";",
    "LEFT_PAREN": "(",
    "RIGHT_PAREN": ")",
    "PLUS": "+",
    "MINUS": "-"
  },
  "multi_char_symbols": {
    "POWER": "**",
    "EQ_COMPARE": "==",
    "GTE": ">=",
    "LTE": "<="
  },
  "comments": {
    "single_line": "--",
    "multi_line_start": "---",
    "multi_line_end": "---"
  }
}
```

**Token Türleri:**

```python
TOKEN_TIPLERI = {
    # Tip Tanımları
    'TANIMLA_SAYI': 'TANIMLA_SAYI',
    'TANIMLA_METIN': 'TANIMLA_METIN',
    'TANIMLA_BOOL': 'TANIMLA_BOOL',
    
    # Yapısal Kelimeler
    'YAPI_KOSUL_EGER': 'YAPI_KOSUL_EGER',
    'YAPI_KOSUL_ISE': 'YAPI_KOSUL_ISE',
    'YAPI_ISLEC': 'YAPI_ISLEC',
    
    # Değer Token'ları
    'IDENTIFIER': 'IDENTIFIER',
    'INTEGER': 'INTEGER',
    'FLOAT': 'FLOAT',
    'STRING': 'STRING',
    
    # Semboller
    'ASSIGN': 'ASSIGN',
    'SEMICOLON': 'SEMICOLON',
    # ...
}
```

---

### 3. Parser (Ayrıştırıcı)

**Sorumluluklar:**
- Token akışını AST'ye dönüştürme
- Syntax validation
- Semantic pre-checks (örn: değişken tekrar tanımı)

**Parsing Stratejisi:** Recursive Descent Parser

**Grammar (Basitleştirilmiş BNF):**

```bnf
program         ::= statement*

statement       ::= variable_decl
                  | function_decl
                  | if_statement
                  | loop_statement
                  | assignment
                  | expression_statement

variable_decl   ::= TYPE IDENTIFIER '=' expression ';'

function_decl   ::= [TYPE] 'IŞLEÇ' IDENTIFIER '(' params ')' 
                    statement* 
                    'IŞLEÇ' 'SON'

if_statement    ::= 'EĞER' expression 'İSE' 
                    statement* 
                    ['DEĞİLSE' statement*] 
                    'EĞER' 'SON'

loop_statement  ::= 'DÖNGÜ' statement* 'DÖNGÜ' 'SON'

assignment      ::= lvalue '=' expression ';'

expression      ::= term (('+' | '-') term)*

term            ::= factor (('*' | '/') factor)*

factor          ::= INTEGER | FLOAT | STRING | IDENTIFIER
                  | function_call | array_access | '(' expression ')'
```

**Operator Precedence:**

| Öncelik | Operator | Açıklama |
|---------|----------|----------|
| 1 (En yüksek) | `()`, `[]`, `{}` | Parantez, dizi/sözlük erişimi |
| 2 | `**` | Üs alma |
| 3 | `*`, `/` | Çarpma, bölme |
| 4 | `+`, `-` | Toplama, çıkarma |
| 5 | `>`, `<`, `>=`, `<=` | Karşılaştırma |
| 6 | `==`, `!=` | Eşitlik |
| 7 (En düşük) | `=` | Atama |

---

### 4. AST (Abstract Syntax Tree)

**AST Node Hiyerarşisi:**

```python
ASTNode (base)
├── Expression (değer üreten)
│   ├── Sayi
│   ├── Metin
│   ├── Degisken
│   ├── IkiliIslem
│   ├── IslecCagirma
│   ├── DiziErisim
│   └── SozlukErisim
│
└── Statement (eylem gerçekleştiren)
    ├── DegiskenTanimlama
    ├── DiziTanimlama
    ├── AtamaKomutu
    ├── EgerBlogu
    ├── DonguBlogu
    ├── DonguBitirKomutu
    ├── IslecTanimlama
    ├── DonusKomutu
    ├── YazdirKomutu
    ├── IfadeKomutu
    └── Blok
```

**AST Örneği:**

```tyd
SAYISAL x = 5 + 3;
EĞER x > 5 İSE
    YAZDIR x;
EĞER SON
```

```
Blok
├── DegiskenTanimlama
│   ├── tip: SAYISAL
│   ├── ad: "x"
│   └── ifade: IkiliIslem
│       ├── sol: Sayi(5)
│       ├── op: PLUS
│       └── sag: Sayi(3)
│
└── EgerBlogu
    ├── kosul: IkiliIslem
    │   ├── sol: Degisken("x")
    │   ├── op: GT
    │   └── sag: Sayi(5)
    │
    └── dogru_blok: Blok
        └── YazdirKomutu
            └── ifade: Degisken("x")
```

---

### 5. Generator (Kod Üretici)

**Sorumluluklar:**
- AST → Assembly dönüşümü
- Register allocation
- Stack management
- C Runtime FFI calls

**Hedef Mimari:** x86-64 (AMD64)

**Register Kullanımı:**

| Register | Kullanım |
|----------|----------|
| `rax` | Hesaplamalar, dönüş değeri |
| `rbx` | Geçici değerler |
| `rcx`, `rdx`, `r8`, `r9` | Geçici |
| `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` | C fonksiyon argümanları (System V ABI) |
| `rbp` | Stack frame pointer |
| `rsp` | Stack pointer |

**Stack Layout:**

```
Yüksek Adres
┌─────────────────┐
│  Arguments      │  [rbp+16], [rbp+24], ...
├─────────────────┤
│  Return Address │
├─────────────────┤
│  Old RBP        │  ← rbp
├─────────────────┤
│  Local Var 1    │  [rbp-8]
├─────────────────┤
│  Local Var 2    │  [rbp-16]
├─────────────────┤
│  Array/Temp     │  [rbp-24], ...
└─────────────────┘  ← rsp
Düşük Adres
```

**Code Generation Patterns:**

**1. Değişken Tanımlama:**
```tyd
SAYISAL x = 10;
```
```asm
; --- DegiskenTanimlama: x ---
mov rax, 10
sub rsp, 8
mov [rbp-8], rax
```

**2. İkili İşlem:**
```tyd
x = a + b;
```
```asm
mov rax, [rbp-16]  ; b
push rax
mov rax, [rbp-8]   ; a
pop rbx
add rax, rbx
mov [rbp-24], rax  ; x
```

**3. İşleç Çağrısı (TYD):**
```tyd
sonuc = topla(5, 10);
```
```asm
; TYD stack calling convention
mov rax, 10
push rax
mov rax, 5
push rax
call _islec_topla
add rsp, 16        ; Stack cleanup
mov [rbp-X], rax
```

**4. C Fonksiyon Çağrısı:**
```tyd
YAZDIR "Merhaba";
```
```asm
; System V ABI (Linux/macOS)
mov rax, str_0     ; "Merhaba"
mov rsi, rax       ; 2. arg (değer)
mov rdi, format_metin  ; 1. arg (format)
mov rax, 0         ; SSE register sayısı
call printf
```

---

### 6. C Runtime

**Sorumluluklar:**
- Memory management
- String operations
- Dictionary operations
- Array operations
- I/O operations

**API:**

```c
// runtime/runtime.c

// Yazdırma
void yazdir_sayi(int64_t sayi);
void yazdir_metin(const char* metin);

// Sözlük
void sozluk_olustur(int boyut);
void sozluk_ekle(const char* anahtar, int64_t deger);
int64_t sozluk_getir(const char* anahtar);
void sozluk_yok_et(void);

// Gelecek:
// void* dizi_olustur(int boyut);
// void dizi_ekle(void* dizi, void* eleman);
// void* dizi_al(void* dizi, int indeks);
```

**Implementasyon Detayları:**

**1. Sözlük (Hash Table):**
```c
typedef struct {
    char* anahtar;
    int64_t deger;
    int kullanilmis;
} SozlukGirdisi;

SozlukGirdisi* sozluk_tablosu;
int sozluk_boyutu;

// DJB2 hash function
unsigned long hash_fonksiyonu(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Linear probing collision resolution
```

---

## 🎨 Syntax Preprocessor

### Tasarım

**Pattern Tanımlama (JSON):**

```json
{
  "python-style": {
    "name": "Python-like Syntax",
    "priority": 1,
    "rules": [
      {
        "id": "python_if",
        "pattern": "if\\s+(.+?):\\s*$",
        "replace": "EĞER \\1 İSE",
        "context": "statement_start",
        "multiline": false
      },
      {
        "id": "python_function",
        "pattern": "def\\s+(\\w+)\\s*\\(([^)]*)\\):",
        "replace": "İŞLEÇ \\1(\\2)",
        "context": "statement_start",
        "multiline": false
      },
      {
        "id": "python_return",
        "pattern": "^\\s*return\\s+(.+)$",
        "replace": "DÖNÜŞ \\1",
        "context": "inside_function",
        "multiline": false
      }
    ],
    "block_handling": {
      "indent_based": true,
      "indent_size": 4
    }
  },
  
  "java-style": {
    "name": "Java/C-like Syntax",
    "priority": 2,
    "rules": [
      {
        "id": "java_if",
        "pattern": "if\\s*\\((.+?)\\)\\s*\\{",
        "replace": "EĞER \\1 İSE",
        "context": "statement_start",
        "multiline": false
      },
      {
        "id": "java_closing_brace",
        "pattern": "^\\s*\\}\\s*$",
        "replace": "SON",
        "context": "any",
        "multiline": false
      }
    ],
    "block_handling": {
      "brace_based": true
    }
  }
}
```

**Preprocessor Algoritması:**

```python
class SyntaxPreprocessor:
    def transform(self, source_code):
        # 1. Satırlara ayır
        lines = source_code.split('\n')
        
        # 2. Her satırı işle
        transformed = []
        context_stack = []  # Blok context tracking
        
        for line_num, line in enumerate(lines):
            # 3. Pattern matching
            matched = False
            for rule in self.rules:
                if self._matches_context(rule, context_stack):
                    match = re.match(rule['pattern'], line)
                    if match:
                        new_line = self._apply_rule(rule, match)
                        transformed.append(new_line)
                        
                        # 4. Context güncelle
                        self._update_context(rule, context_stack)
                        matched = True
                        break
            
            # 5. Match yoksa, olduğu gibi geç
            if not matched:
                transformed.append(line)
        
        # 6. Source mapping oluştur
        self.source_map = self._build_source_map(lines, transformed)
        
        return '\n'.join(transformed)
```

**Zorluklar ve Çözümler:**

| Zorluk | Çözüm |
|--------|-------|
| **Nested blocks** | Context stack + indent/brace tracking |
| **Ambiguous patterns** | Priority system + longest match first |
| **Multiline expressions** | Lookahead buffer + continuation detection |
| **Error reporting** | Source mapping: (original_line → transformed_line) |

---

## 🔧 Tasarım Kararları

### 1. Neden C Runtime?

**Alternatifler:**
- **Pure Assembly:** Çok zor, platform bağımlı
- **LLVM IR:** Ağır, öğrenme eğrisi dik
- **JVM Bytecode:** Java dependency
- **✅ C Runtime:** Platform bağımsız, mature, fast

**Avantajlar:**
- Her platformda C derleyicisi var
- Mature toolchain (GCC, Clang)
- Zero-cost abstraction
- Easy debugging

### 2. Neden Statik Tip Sistemi?

**Alternatifler:**
- **Duck Typing (Python):** Runtime surprise'lar
- **Gradual Typing (TypeScript):** Karmaşık
- **✅ Static:** Compile-time safety

**Avantajlar:**
- Compile-time error detection
- Better performance (no runtime type checks)
- IDE support (autocomplete, type hints)
- Documentation (types are docs)

### 3. Neden Syntax Preprocessor?

**Alternatifler:**
- **Multi-Parser:** Her syntax için ayrı parser → Çok fazla kod
- **Unified Grammar:** Tüm syntaxları tek parser → İmkansız (syntax conflict)
- **✅ Preprocessor:** Syntax'ı normalize et → Tek parser

**Avantajlar:**
- Tek parser maintain etmek kolay
- Yeni syntax eklemek kolay (JSON'a ekle)
- Semantic layer değişmiyor
- Error reporting ayarlanabilir

### 4. Stack vs Register Calling Convention?

**TYD Fonksiyonları:** Stack-based (basitlik için)
**C Fonksiyonları:** System V ABI (standard)

```
TYD Call:           C Call (System V):
┌────────────┐     ┌────────────┐
│ push arg2  │     │ mov rsi,.. │
│ push arg1  │     │ mov rdi,.. │
│ call func  │     │ call func  │
│ add rsp,16 │     │            │
└────────────┘     └────────────┘
```

---

## 📊 Performans Hedefleri

### Compile Time

| Metrik | Hedef | Mevcut | Durum |
|--------|-------|--------|-------|
| Lexer | < 1ms/1000 satır | ~0.5ms | ✅ |
| Parser | < 5ms/1000 satır | ~2ms | ✅ |
| Generator | < 10ms/1000 satır | ~8ms | ✅ |
| Total | < 20ms/1000 satır | ~15ms | ✅ |

### Runtime Performance

| Benchmark | TYD | C | Python | Hedef |
|-----------|-----|---|--------|-------|
| Fibonacci(30) | ? | 0.5ms | 800ms | < 2ms |
| Array sum(1M) | ? | 2ms | 50ms | < 10ms |
| Dict lookup(10K) | ? | 5ms | 15ms | < 15ms |

*Not: TYD benchmarkları henüz yapılmadı*

---

## 🚀 Gelecek Planları

### v0.2.0 (Syntax Preprocessor)
- [ ] `syntax_preprocessor.py` implementasyonu
- [ ] `syntax_styles.json` tanımları
- [ ] 5+ syntax style desteği
- [ ] Source mapping
- [ ] Error reporting iyileştirmesi

### v0.3.0 (Lexer JSON-Driven)
- [ ] `semboller.json` tanımı
- [ ] Lexer refactor (symbol loading)
- [ ] %100 JSON-driven Lexer
- [ ] Custom symbol desteği

### v0.4.0 (Advanced Features)
- [ ] Foreach döngüsü (HER...İÇİNDE)
- [ ] Type inference
- [ ] Array literals (`[1, 2, 3]`)
- [ ] String interpolation (`"Değer: ${x}"`)

### v0.5.0 (OOP)
- [ ] NESNE tanımlama
- [ ] Inheritance
- [ ] Method dispatch
- [ ] Constructor/Destructor

### v1.0.0 (Production)
- [ ] Modül sistemi
- [ ] Paket yöneticisi
- [ ] LSP implementation
- [ ] Debugger
- [ ] Profiler
- [ ] Full documentation

---

## 📚 Referanslar

### Mimari Kararlar İçin
- [The Dragon Book (Compilers: Principles, Techniques, and Tools)](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools)
- [Engineering a Compiler (Cooper & Torczon)](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0)

### Benzer Projekte
- **Babel:** JavaScript transformation
- **Roslyn:** .NET multi-syntax compiler
- **LLVM:** IR-based multi-language backend
- **GraalVM:** Polyglot VM

### Standards
- [System V ABI (x86-64)](https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf)
- [ELF Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

---

**Son Güncelleme:** 12 Kasım 2024  
**Katkıda Bulunanlar:** Güven Acar, Claude AI  
**Lisans:** MIT

