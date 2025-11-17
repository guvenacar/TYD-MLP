extern printf
extern string_birlestir
extern string_karsilastir
extern string_uzunluk
extern dosya_ac
extern dosya_oku
extern dosya_yaz
extern dosya_kapat
section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
str_0: db "Merhaba ", 0
str_1: db "Dunya", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    and rsp, -16  ; Stack alignment (16-byte)
    ; --- DegiskenTanimlama: a ---
    mov rax, str_0
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: b ---
    mov rax, str_1
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: c ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call string_birlestir
    mov [rbp-24], rax
    mov rax, [rbp-24]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
