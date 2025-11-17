extern printf
section .data
    format_sayi db "%d", 10, 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    jmp _ISLEC_SON_topla
topla:
    ; --- Islec Tanimlama: topla ---
    push rbp
    mov rbp, rsp
    mov [rbp-8], rdi
    mov [rbp-16], rsi
    ; --- Donus Komutu ---
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    pop rbx
    add rax, rbx
    mov rsp, rbp
    pop rbp
    ret
_ISLEC_SON_topla:
    mov rax, 10
    push rax
    mov rax, 15
    push rax
    pop rsi
    pop rdi
    call topla
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
