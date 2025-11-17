extern printf
section .data
    format_sayi db "%d", 10, 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    mov rax, 25
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
