extern printf
extern sozluk_ekle
extern sozluk_olustur
extern free
extern string_birlestir
extern sozluk_getir
extern string_karsilastir
extern sozluk_yok_et
extern malloc

section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
    str_2 db "Sonuc: ", 0
    str_3 db "Toplam:", 0
    str_4 db "TAMAM.", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    ; --- DegiskenTanimlama: sayac ---
    mov rax, 1
    sub rsp, 8  ; 'sayac' için yığında 8 byte yer aç
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: sinir ---
    mov rax, 5
    sub rsp, 8  ; 'sinir' için yığında 8 byte yer aç
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: sonuc_metin ---
    mov rax, str_2
    sub rsp, 8  ; 'sonuc_metin' için yığında 8 byte yer aç
    mov [rbp-24], rax
    _etiket_1:
    ; --- AtamaKomutu ---
    mov rax, 1
    push rax
    mov rax, [rbp-8]
    pop rbx
    add rax, rbx
    push rax  ; Atanacak değeri (örn: 30) koru
    pop rax
    mov [rbp-8], rax  ; sayac = (sonuç)
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    pop rbx
    cmp rax, rbx
    sete al
    movzx rax, al
    cmp rax, 0
    je _etiket_4
    ; --- DÖNGÜ_BITIR ---
    jmp _etiket_2
    _etiket_4:
    jmp _etiket_1
    _etiket_2:
    ; --- DegiskenTanimlama: toplam ---
    mov rax, 10
    push rax
    mov rax, [rbp-8]
    pop rbx
    add rax, rbx
    sub rsp, 8  ; 'toplam' için yığında 8 byte yer aç
    mov [rbp-32], rax
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_3
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, [rbp-32]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_sayi   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- DegiskenTanimlama: tamam ---
    mov rax, str_4
    sub rsp, 8  ; 'tamam' için yığında 8 byte yer aç
    mov [rbp-40], rax
    ; --- DegiskenTanimlama: mesaj ---
    ; --- İKİLİ İŞLEM: METİN BİRLEŞTİRME ---
    mov rax, [rbp-40]
    mov rsi, rax
    mov rax, [rbp-24]
    mov rdi, rax
    mov rax, 0  ; (SSE kuralı)
    call string_birlestir
    sub rsp, 8  ; 'mesaj' için yığında 8 byte yer aç
    mov [rbp-48], rax
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, [rbp-48]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    ; --- İŞLEÇ Çağırma: SIL ---
    mov rax, [rbp-48]
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rax, 0  ; (SSE kuralı)
    call free
    xor rax, rax  ; Exit code 0 (başarılı)
    mov rsp, rbp
    pop rbp
    ret