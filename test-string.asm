extern printf
section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
str_0: db "Merhaba Dünya!", 0
str_1: db "TYD Dili String Desteği", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    mov rax, str_0
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_1
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, 42
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
