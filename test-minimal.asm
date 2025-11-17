extern printf
extern string_birlestir
extern string_karsilastir
extern dosya_ac
extern dosya_oku
extern dosya_yaz
extern dosya_kapat
section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
str_0: db "test.txt", 0
str_1: db "w", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    and rsp, -16  ; Stack alignment (16-byte)
    ; --- DegiskenTanimlama: yol ---
    mov rax, str_0
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: mod ---
    mov rax, str_1
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: f ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: k ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-32], rax
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
