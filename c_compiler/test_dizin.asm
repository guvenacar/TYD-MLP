extern printf
extern strlen
extern strcmp
extern strstr
extern tyd_substr
extern tyd_strcat
extern dosya_ac
extern dosya_oku
extern dosya_yaz
extern dosya_kapat
extern runtime_dizin_al
extern tyd_fix_cwd
section .data
    format_sayi db "%ld", 10, 0
    format_metin db "%s", 10, 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    call tyd_fix_cwd
    ; --- Ana Program Akışı ---
    ; --- DegiskenTanimlama: calisma_dizini ---
    ; --- DIZIN_AL cagirma ---
    mov rax, 0
    call runtime_dizin_al
    mov [rbp-8], rax
    mov rax, [rbp-8]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- Program Sonu ---
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

; === Fonksiyon Tanımları ===

section .note.GNU-stack noalloc noexec nowrite progbits

db 0
