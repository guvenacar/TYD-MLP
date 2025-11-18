# tmp/old - Eski Dosyalar

Bu dizinde, TYD projesinin önceki aşamalarından kalan dosyalar bulunmaktadır.

## İçerik

### Eski Test Dosyaları
- `test-*` - Eski test executable'ları
- `test_*.asm`, `test_*.o` - Test assembly ve object files
- `test*.tyd` - Eski test TYD programları
- `self-host-v1*` - Eski self-hosting test dosyaları

### Build Artifacts
- `*.o` - Object files (c_lexer.o, c_parser.o, c_generator.o, main.o, runtime.o)
- `*.asm` - Generated assembly files (tyd_lexer.asm, tyd_parser.asm, etc.)
- `build/` - Eski build dizini

### Eski Dokümantasyon
- `PIPELINE.md` - Python compiler pipeline dokümantasyonu
- `QUICKSTART.md` - Eski quickstart rehberi
- `VSCODE_EXTENSION.md` - VSCode extension (eski)
- `VSCODIUM_KURULUM.md` - VSCodium kurulum
- `SPECS_LOCK.md` - Eski specs lock
- `calistirma_komutu.md` - Eski komutlar
- `donusum_tablosu.md` - Syntax dönüşüm tablosu
- `temel_komutlar.md` - Temel komutlar

### Eski Build System
- `build.sh` - Python compiler build script
- `test_bootstrap.sh` - Bootstrap test script
- `kurallar/` - Syntax preprocessor kuralları (JSON)

### Eski Dizinler
- `raporlar/` - Eski test raporları
- `tools/` - Yardımcı araçlar
- `.vscode/` - VSCode IDE ayarları
- `vscode-tyd/` - VSCode extension (eski)

### Diğer
- `output.txt`, `test.txt` - Test çıktı dosyaları
- `H�E�PH�E�P^_�B`, `H�E�PH�E�P^_�E` - Garbled files (silinebilir)

## Notlar

Bu dosyalar **artık aktif olarak kullanılmamaktadır**.

**Aktif Proje Yapısı:**
- `c_compiler/` - Bootstrap compiler (C)
- `tyd_compiler/` - Self-hosting compiler (TYD)
- `runtime/` - Runtime library
- `ornekler/` - Example programs
- `tydc.tyd` - Compiler driver (Stage 3)
- `test_self_hosting.tyd` - Self-hosting test
- `test_struct.tyd` - Struct test

**Silinebilir:** Bu dizindeki dosyalar güvenle silinebilir. Sadece tarihsel referans için saklanmaktadır.

---

**Tarih:** 2025-11-18
**Sebep:** Stage 3 tamamlandıktan sonra proje temizliği
