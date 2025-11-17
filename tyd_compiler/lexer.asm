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
    sub rsp, 256
    call tyd_fix_cwd
    ; --- Ana Program Akışı ---
    ; --- Program Sonu ---
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

; === Fonksiyon Tanımları ===

section .note.GNU-stack noalloc noexec nowrite progbits

db 0
