extern printf
section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
str_0: db "Ahmet", 0
str_1: db "Hoş geldin!", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    ; --- DegiskenTanimlama: isim ---
    mov rax, str_0
    mov [rbp-8], rax
    mov rax, [rbp-8]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: mesaj ---
    mov rax, str_1
    mov [rbp-16], rax
    mov rax, [rbp-16]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
