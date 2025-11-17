extern dosya_yaz
extern malloc
extern sozluk_getir
extern printf
extern string_birlestir
extern sozluk_ekle
extern sozluk_yok_et
extern string_karsilastir
extern string_uzunluk
extern dosya_ac
extern free
extern sozluk_olustur
extern dosya_oku
extern dosya_kapat

section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
    str_2 db "=== TYD Self-Hosting Demo v1 ===", 0
    str_3 db "Kaynak kod okunuyor...", 0
    str_4 db "ornekler/hello.tyd", 0
    str_5 db "r", 0
    str_6 db "Okunan kaynak:", 0
    str_7 db "Assembly uretiliyor...", 0
    str_8 db "extern printf\n", 0
    str_9 db "section .data\n", 0
    str_10 db "section .text\n", 0
    str_11 db "global main\n", 0
    str_12 db "build/generated.asm", 0
    str_13 db "w", 0
    str_14 db "Olusturuldu!", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_2
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_3
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- DegiskenTanimlama: kaynak_dosya ---
    mov rax, str_4
    sub rsp, 8  ; 'kaynak_dosya' için yığında 8 byte yer aç
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: mod ---
    mov rax, str_5
    sub rsp, 8  ; 'mod' için yığında 8 byte yer aç
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: dosya ---
    ; --- İŞLEÇ Çağırma: DOSYA_AC ---
    mov rax, [rbp-8]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-16]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_ac
    sub rsp, 8  ; 'dosya' için yığında 8 byte yer aç
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: kaynak ---
    ; --- İŞLEÇ Çağırma: DOSYA_OKU ---
    mov rax, [rbp-24]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, 0  ; (SSE kuralı)
    call dosya_oku
    sub rsp, 8  ; 'kaynak' için yığında 8 byte yer aç
    mov [rbp-32], rax
    ; --- DegiskenTanimlama: kapat1 ---
    ; --- İŞLEÇ Çağırma: DOSYA_KAPAT ---
    mov rax, [rbp-24]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, 0  ; (SSE kuralı)
    call dosya_kapat
    sub rsp, 8  ; 'kapat1' için yığında 8 byte yer aç
    mov [rbp-40], rax
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_6
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, [rbp-32]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_7
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- DegiskenTanimlama: asm1 ---
    mov rax, str_8
    sub rsp, 8  ; 'asm1' için yığında 8 byte yer aç
    mov [rbp-48], rax
    ; --- DegiskenTanimlama: asm2 ---
    mov rax, str_9
    sub rsp, 8  ; 'asm2' için yığında 8 byte yer aç
    mov [rbp-56], rax
    ; --- DegiskenTanimlama: asm3 ---
    mov rax, str_10
    sub rsp, 8  ; 'asm3' için yığında 8 byte yer aç
    mov [rbp-64], rax
    ; --- DegiskenTanimlama: asm4 ---
    mov rax, str_11
    sub rsp, 8  ; 'asm4' için yığında 8 byte yer aç
    mov [rbp-72], rax
    ; --- DegiskenTanimlama: cikti_dosya ---
    mov rax, str_12
    sub rsp, 8  ; 'cikti_dosya' için yığında 8 byte yer aç
    mov [rbp-80], rax
    ; --- DegiskenTanimlama: mod_yaz ---
    mov rax, str_13
    sub rsp, 8  ; 'mod_yaz' için yığında 8 byte yer aç
    mov [rbp-88], rax
    ; --- DegiskenTanimlama: cikti ---
    ; --- İŞLEÇ Çağırma: DOSYA_AC ---
    mov rax, [rbp-80]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-88]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_ac
    sub rsp, 8  ; 'cikti' için yığında 8 byte yer aç
    mov [rbp-96], rax
    ; --- DegiskenTanimlama: n1 ---
    ; --- İŞLEÇ Çağırma: DOSYA_YAZ ---
    mov rax, [rbp-96]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-48]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_yaz
    sub rsp, 8  ; 'n1' için yığında 8 byte yer aç
    mov [rbp-104], rax
    ; --- DegiskenTanimlama: n2 ---
    ; --- İŞLEÇ Çağırma: DOSYA_YAZ ---
    mov rax, [rbp-96]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-56]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_yaz
    sub rsp, 8  ; 'n2' için yığında 8 byte yer aç
    mov [rbp-112], rax
    ; --- DegiskenTanimlama: n3 ---
    ; --- İŞLEÇ Çağırma: DOSYA_YAZ ---
    mov rax, [rbp-96]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-64]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_yaz
    sub rsp, 8  ; 'n3' için yığında 8 byte yer aç
    mov [rbp-120], rax
    ; --- DegiskenTanimlama: n4 ---
    ; --- İŞLEÇ Çağırma: DOSYA_YAZ ---
    mov rax, [rbp-96]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, [rbp-72]
    mov rsi, rax  ; Argüman 2 -> rsi
    mov rax, 0  ; (SSE kuralı)
    call dosya_yaz
    sub rsp, 8  ; 'n4' için yığında 8 byte yer aç
    mov [rbp-128], rax
    ; --- DegiskenTanimlama: kapat2 ---
    ; --- İŞLEÇ Çağırma: DOSYA_KAPAT ---
    mov rax, [rbp-96]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, 0  ; (SSE kuralı)
    call dosya_kapat
    sub rsp, 8  ; 'kapat2' için yığında 8 byte yer aç
    mov [rbp-136], rax
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_14
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, [rbp-80]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    xor rax, rax  ; Exit code 0 (başarılı)
    mov rsp, rbp
    pop rbp
    ret