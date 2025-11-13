extern sozluk_olustur
extern malloc
extern printf
extern sozluk_yok_et
extern sozluk_getir
extern sozluk_ekle
extern free

section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
    str_2 db "Büyük", 0
    str_3 db "Bitti", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    ; --- DegiskenTanimlama: x ---
    mov rax, 15
    sub rsp, 8  ; 'x' için yığında 8 byte yer aç
    mov [rbp-8], rax
    mov rax, 10
    push rax
    mov rax, [rbp-8]
    pop rbx
    ; TODO: Desteklenmeyen İkili İşlem: GT
    cmp rax, 0
    je _etiket_2
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_2
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    _etiket_2:
    ; --- YAZDIR Başlangıç (C Arayüzüne delege edildi) ---
    mov rax, str_3
    mov rdi, rax  ; Argüman 1 -> rdi
    mov rsi, rdi  ; Değeri RDI'dan RSI'a (2. arg) taşı
    mov rdi, format_metin   ; Formatı RDI'a (1. arg) taşı
    mov rax, 0  ; (SSE kuralı)
    call printf
    mov rsp, rbp
    pop rbp
    ret