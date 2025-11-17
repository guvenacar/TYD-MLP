# 🐛 Self-Host Segfault Debug - ÖZET

**Tarih:** 17 Kasım 2024
**AI:** Claude Sonnet 4.5
**Süre:** 120 dakika
**Durum:** ⚠️ Sorun bulundu, fix devam ediyor

---

## 📋 Sorun

`self-host-v1.tyd` çalıştırıldığında segmentation fault (exit code 139).

---

## 🔍 Debug Süreci

### 1. Valgrind ile Analiz
```bash
==36857== Invalid read of size 4
==36857==    at fclose (iofclose.c:48)
==36857==    by dosya_kapat
==36857==  Address 0x17 is not stack'd, malloc'd or free'd
```

**Bulgu:** `dosya_kapat`'a geçersiz pointer (0x17) geliyor.

### 2. Runtime Debug Mesajları
```
DEBUG [DOSYA_AC]: Başarılı! FILE*=0x407710   ✅
DEBUG [DOSYA_OKU]: Çağrıldı - dosya_ptr=0x407710   ✅
DEBUG [DOSYA_OKU]: 23 byte okundu   ✅
DEBUG [DOSYA_KAPAT]: Çağrıldı - dosya_ptr=0x17 (23)   ❌
```

**Bulgu:** `dosya` değişkeni DOSYA_OKU sonrası bozuluyor!

---

## 🎯 Kök Neden

**Stack Corruption:**
- `dosya` değişkeni: FILE* (0x407710)
- DOSYA_OKU çağrısı sonrası: 0x17 (=23 = dosya boyutu)
- Dosya boyutu `dosya` değişkeninin üzerine yazılmış

**Neden:** Assembly generator'da stack offset veya register yönetimi hatası.

---

## 📝 Assembly Analizi

```assembly
; self-host-v1.asm

; SAYISAL dosya = DOSYA_AC(...)
call dosya_ac
mov [rbp-24], rax      ; dosya = FILE* (0x407710) ✅

; METIN kaynak = DOSYA_OKU(dosya)
mov rax, [rbp-24]      ; dosya yükle ✅
mov rdi, rax
call dosya_oku
mov [rbp-32], rax      ; kaynak = char* ✅

; SAYISAL kapat1 = DOSYA_KAPAT(dosya)
mov rax, [rbp-24]      ; dosya yükle... ama artık 0x17! ❌
mov rdi, rax
call dosya_kapat       ; SEGFAULT!
```

**Sorun:** `[rbp-24]` adresi DOSYA_OKU sonrası bozulmuş.

---

## 🔧 Denenen Çözümler

### 1. Path Resolution Fix ✅
Path resolution zaten düzeltilmişti, bu farklı bir sorun.

### 2. TYD Kodu Workaround ⚠️
```tyd
SAYISAL dosya_backup = dosya;
METIN kaynak = DOSYA_OKU(dosya_backup);
SAYISAL kapat1 = DOSYA_KAPAT(dosya);
```
**Sonuç:** Derlendi ama test edilemedi (NASM yok).

### 3. Runtime Debug ✅
Sorunun yerini tam olarak bulduk.

---

## 💡 Gerçek Fix (Gerekli)

**c_generator.c'de düzeltme gerekiyor:**
1. Fonksiyon çağrıları sırasında stack frame yönetimi
2. Caller-saved register'lar doğru kaydedilmeli
3. Stack offset hesaplaması düzeltilmeli

**Alternatif:** Fonksiyon çağrısı öncesi tüm değişkenleri stack'e push et, sonra geri yükle.

---

## 📊 Etkilenen Dosyalar

| Dosya | Değişiklik | Durum |
|-------|------------|-------|
| `runtime/runtime.c` | Debug mesajları eklendi | ✅ |
| `ornekler/self-host-v1.tyd` | Workaround denendi | ⚠️ |
| `c_compiler/c_generator.c` | FIX GEREKLİ | ❌ |

---

## 🔄 Sonraki Adımlar

### Yüksek Öncelik
1. [ ] NASM kurulumu → Workaround test edilebilsin
2. [ ] c_generator.c fonksiyon çağrı kodunu incele
3. [ ] Stack allocation büyütülmesi dene (64→128 byte)

### Orta Öncelik
4. [ ] Assembly generator unit test
5. [ ] Caller-saved register koruması ekle

---

## 🎓 Öğrenilenler

### Stack Corruption Debug
- **Valgrind:** Geçersiz pointer erişimlerini yakalar
- **Debug Printf:** Runtime'da değer akışını izle
- **Assembly Analiz:** Her değişkenin stack offset'ini takip et

### x86-64 ABI
- Caller-saved: rax, rdi, rsi, rdx, rcx, r8-r11
- Callee-saved: rbx, rbp, r12-r15
- Fonksiyon çağrısı register'ları bozabilir

### TYD Compiler Bug
- Fonksiyon çağrıları stack'i bozuyor
- Stack offset hesabı yanlış
- c_generator.c'de major fix gerekli

---

## 📌 Özet

**Sorun:** ✅ BULUNDU - Stack corruption
**Sebep:** Assembly generator bug'ı
**Etki:** self-host-v1.tyd çalışmıyor
**Fix:** ⏳ c_generator.c düzeltilmeli

**Geçici Çözüm:** Workaround kodu var ama test edilemedi (NASM gerekli).

---

**Rapor Yazan:** Claude Sonnet 4.5
**Satır Sayısı:** 178 (Max 200 ✅)
**Son Güncelleme:** 17 Kasım 2024 21:45
