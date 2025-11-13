# TYD-MLP Stage 2: Full Self-Hosting Compiler

## Overview

Stage 1 (completed) verified the bootstrap pipeline works (Python → Assembly).
Stage 2 will implement a full self-hosting MLP compiler that can compile itself.

---

## Current Status

### ✅ Completed (Stage 1)
- MLP compiler source code written in MLP syntax (~1,463 lines)
- Bootstrap scripts created
- Python → Assembly pipeline verified
- Minimal stub proves concept

### ⚠️ Limitations Discovered
- TYD runtime lacks string manipulation functions
- Cannot implement full lexer/parser without:
  - `alt_metin(str, start, len)` - substring
  - `UZUNLUK(str)` - string length
  - `karakter_al(str, index)` - char at index
  - String concatenation
  - Array/list support

---

## Stage 2 Goals

**Primary Goal:** Create a fully functional self-hosting MLP compiler

**Success Criteria:**
1. MLP compiler written in TYD syntax compiles itself
2. `mlpc` executable can compile its own source code
3. Stage 1 → Stage 2 bootstrap produces identical binaries
4. All compiler phases work: Lexer → Parser → CodeGen → Assembly

---

## Phase 1: Runtime Extension (Est. 4-6 hours)

### Task 1.1: String Functions in C Runtime
**File:** `runtime/runtime.c`

Add functions:
```c
// String length
int tyd_strlen(const char* str);

// Substring extraction
char* tyd_substring(const char* str, int start, int length);

// Character at index
char tyd_char_at(const char* str, int index);

// String concatenation
char* tyd_concat(const char* str1, const char* str2);

// String comparison
int tyd_strcmp(const char* str1, const char* str2);

// String search/contains
int tyd_contains(const char* haystack, const char* needle);
```

**Implementation Notes:**
- Use `malloc` for dynamic allocation
- Return NULL on errors
- Use standard C library internally (`strlen`, `strncpy`, `strcat`)

### Task 1.2: Array/List Support
**File:** `runtime/runtime.c`

Add simple dynamic array:
```c
typedef struct {
    void** data;
    int size;
    int capacity;
} TydArray;

TydArray* tyd_array_create();
void tyd_array_push(TydArray* arr, void* item);
void* tyd_array_get(TydArray* arr, int index);
int tyd_array_size(TydArray* arr);
void tyd_array_destroy(TydArray* arr);
```

### Task 1.3: Update Generator for New Functions
**File:** `src/generator.py` → `src/c_arayuzu.py`

Add extern declarations and call wrappers:
- Map `UZUNLUK()` → `tyd_strlen()`
- Map `alt_metin()` → `tyd_substring()`
- Map `karakter_al()` → `tyd_char_at()`
- Map string `+` operator → `tyd_concat()`

**Testing:**
Create simple test TYD programs for each function:
```tyd
METIN s = "hello";
SAYISAL len = UZUNLUK(s);
YAZDIR len;  -- Should print 5
```

---

## Phase 2: Lexer Implementation (Est. 2-3 hours)

### Task 2.1: Convert lexer.mlp → lexer.tyd
**File:** `src/compiler/lexer.tyd`

Rewrite using TYD syntax with new runtime functions:

**Key Features:**
- Token structure (use string or simple struct)
- Character-by-character scanning
- Keyword recognition (hard-coded list)
- Number parsing (integers and floats)
- String literal parsing
- Comment handling (-- and ---)
- Operator tokenization

**Simplifications for Bootstrap:**
- Hard-code keywords (no JSON)
- Return simple token array or delimited string
- Basic error handling only

**Example Structure:**
```tyd
METIN kaynak_kod = "";
SAYISAL konum = 0;

METIN işleç sonraki_token()
    -- Skip whitespace
    DÖNGÜ mevcut_karakter() EŞIT " " İSE
        ilerle();
    SON

    -- Check for numbers
    EĞER rakam_mi(mevcut_karakter()) İSE
        DÖNÜŞ sayi_oku();
    SON

    -- ... more token types
SON
```

### Task 2.2: Test Lexer
Create test file: `tests/test_lexer.tyd`
```tyd
METIN kod = "SAYISAL x = 42;";
METIN tokenler = tokenize(kod);
YAZDIR tokenler;
```

---

## Phase 3: Parser Implementation (Est. 3-4 hours)

### Task 3.1: Convert parser.mlp → parser.tyd
**File:** `src/compiler/parser.tyd`

Implement recursive descent parser:

**Key Components:**
- Token stream management
- AST node creation (use strings or simple structs)
- Expression parsing with precedence
  - `faktor()` - primary expressions
  - `terim()` - multiplicative ops
  - `ifade()` - additive ops
- Statement parsing
  - Variable declarations
  - Assignments
  - If/else blocks
  - Loops
  - Function definitions
  - Return statements

**Simplifications:**
- AST as string representation (not complex objects)
- Basic error messages only
- Support minimal TYD syntax subset

### Task 3.2: Test Parser
Create test: `tests/test_parser.tyd`
```tyd
METIN tokens = "SAYISAL,x,=,42,;,EOF";
METIN ast = parse(tokens);
YAZDIR ast;
```

---

## Phase 4: Code Generator Implementation (Est. 4-5 hours)

### Task 4.1: Convert codegen.mlp → codegen.tyd
**File:** `src/compiler/codegen.tyd`

Implement x86_64 assembly generation:

**Key Components:**
- Assembly code accumulation (use string concat)
- `.data` section for string literals
- `.text` section for code
- Expression code generation
  - Arithmetic operators
  - Comparisons
  - Function calls
- Statement code generation
  - Variable storage (stack-based)
  - Control flow (jumps, labels)
  - Function definitions (prologue/epilogue)

**Simplifications:**
- Basic stack management only
- Limited register optimization
- Minimal optimization
- Support core features only

### Task 4.2: Test Code Generator
Create test: `tests/test_codegen.tyd`
```tyd
METIN ast = "DegiskenTanimlama(x,42)";
METIN asm = generate(ast);
YAZDIR asm;
```

---

## Phase 5: Main Compiler Integration (Est. 2-3 hours)

### Task 5.1: Convert main.mlp → main.tyd
**File:** `src/compiler/main.tyd`

Implement main compiler orchestration:

**Pipeline:**
```tyd
METIN işleç compile(METIN source_file, METIN output_file)
    -- 1. Read source
    METIN kod = DOSYA_OKU(source_file);

    -- 2. Lexer
    METIN tokenler = tokenize(kod);

    -- 3. Parser
    METIN ast = parse(tokenler);

    -- 4. Code Generator
    METIN asm = generate(ast);

    -- 5. Write output
    DOSYA_YAZ(output_file, asm);

    DÖNÜŞ 0;
SON
```

**Additions Needed:**
- File I/O functions in runtime:
  ```c
  char* tyd_read_file(const char* path);
  int tyd_write_file(const char* path, const char* content);
  ```
- Command-line argument parsing:
  ```c
  int tyd_argc();
  char* tyd_argv(int index);
  ```

### Task 5.2: Create Test Suite
**File:** `tests/run_tests.sh`

Test programs:
1. `hello.tyd` - Basic print
2. `variables.tyd` - Variable declarations
3. `arithmetic.tyd` - Math operations
4. `functions.tyd` - Function definitions
5. `control_flow.tyd` - If/else, loops

---

## Phase 6: Bootstrap Execution (Est. 1-2 hours)

### Task 6.1: Update Bootstrap Script
**File:** `bootstrap.sh`

Full 5-stage bootstrap:

```bash
# Stage 0: Compile MLP compiler with Python
python3 -m src.compiler src/compiler/lexer.tyd -o build/lexer.asm
python3 -m src.compiler src/compiler/parser.tyd -o build/parser.asm
python3 -m src.compiler src/compiler/codegen.tyd -o build/codegen.asm
python3 -m src.compiler src/compiler/main.tyd -o build/main.asm

# Stage 1: Assemble
nasm -f elf64 build/*.asm -o build/*.o

# Stage 2: Link
gcc build/*.o runtime/runtime.o -o build/mlpc -no-pie

# Stage 3: Test mlpc
./build/mlpc tests/hello.tyd -o build/hello.asm

# Stage 4: Self-compile (mlpc compiles itself)
./build/mlpc src/compiler/main.tyd -o build/main_v2.asm

# Stage 5: Verify binary equivalence
diff build/main.asm build/main_v2.asm
```

### Task 6.2: Verify Self-Hosting
Run complete bootstrap cycle:
```bash
./bootstrap.sh
```

Expected output:
```
✓ Stage 0: Python compilation successful
✓ Stage 1: Assembly successful
✓ Stage 2: Linking successful
✓ Stage 3: mlpc runs successfully
✓ Stage 4: Self-compilation successful
✓ Stage 5: Binaries match - TRUE SELF-HOSTING!
```

---

## Phase 7: Documentation and Testing (Est. 2-3 hours)

### Task 7.1: Update Documentation
**Files to update:**
- `README.md` - Add Stage 2 status
- `docs/BOOTSTRAP.md` - Document full process
- `docs/ARCHITECTURE.md` - Describe compiler structure
- `docs/RUNTIME.md` - Document runtime functions

### Task 7.2: Create Test Suite
**File:** `tests/test_suite.tyd`

Comprehensive tests:
- All token types
- All expression types
- All statement types
- Edge cases
- Error handling

### Task 7.3: Benchmarking
Compare:
- Python TYD-MLP vs self-hosted mlpc
- Compilation speed
- Generated code quality
- Binary sizes

---

## Success Metrics

### Functional Requirements
- [ ] All runtime functions implemented and tested
- [ ] Lexer handles all TYD syntax
- [ ] Parser builds complete AST
- [ ] Code generator produces valid assembly
- [ ] Main compiler orchestrates pipeline
- [ ] Bootstrap script completes without errors
- [ ] Self-compilation produces identical binaries

### Performance Requirements
- [ ] Compile hello world in < 1 second
- [ ] Bootstrap completes in < 30 seconds
- [ ] Generated code is reasonable quality

### Quality Requirements
- [ ] All tests pass
- [ ] Code is documented
- [ ] Error messages are helpful
- [ ] Bootstrap process is reproducible

---

## Timeline Estimate

| Phase | Tasks | Estimated Time |
|-------|-------|----------------|
| Phase 1 | Runtime Extension | 4-6 hours |
| Phase 2 | Lexer Implementation | 2-3 hours |
| Phase 3 | Parser Implementation | 3-4 hours |
| Phase 4 | Code Generator | 4-5 hours |
| Phase 5 | Main Compiler | 2-3 hours |
| Phase 6 | Bootstrap Execution | 1-2 hours |
| Phase 7 | Documentation & Testing | 2-3 hours |
| **TOTAL** | | **18-26 hours** |

---

## Risk Mitigation

### Risk 1: Runtime Function Complexity
**Mitigation:** Start with minimal implementations, add features incrementally

### Risk 2: Parser Complexity
**Mitigation:** Support subset of TYD syntax initially, expand later

### Risk 3: Assembly Generation Bugs
**Mitigation:** Test each feature individually, use known-good examples

### Risk 4: Bootstrap Failures
**Mitigation:** Detailed logging at each stage, compare with Python version

---

## Dependencies

### External Dependencies
- NASM assembler (must be installed)
- GCC compiler
- Standard C library
- Linux x86_64 environment

### Internal Dependencies
- Python TYD-MLP (Stage 0 compiler)
- runtime.c with extended functions
- Test suite for validation

---

## Future Enhancements (Stage 3+)

After successful Stage 2 bootstrap:

1. **Optimization Pass**
   - Register allocation
   - Dead code elimination
   - Constant folding

2. **Enhanced Features**
   - Full MLP syntax (generics, etc.)
   - Better error messages
   - Source maps / debugging info

3. **Tooling**
   - Package manager
   - Build system
   - Documentation generator

4. **Standard Library**
   - Data structures
   - File I/O
   - Networking
   - GUI (GTK+ bindings)

5. **Multi-platform**
   - ARM64 support
   - Windows support
   - macOS support

---

## Conclusion

Stage 2 will transform the proof-of-concept bootstrap into a fully functional,
self-hosting compiler. This is a significant milestone in language development
and demonstrates true compiler maturity.

Estimated completion: 18-26 hours of focused development work.

**Let's build a real compiler!**
