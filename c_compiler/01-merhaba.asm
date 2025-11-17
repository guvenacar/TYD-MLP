extern printf
extern string_birlestir
extern string_karsilastir
extern string_uzunluk
extern dosya_ac
extern dosya_oku
extern dosya_yaz
extern dosya_kapat
section .data
    format_sayi db "%ld", 10, 0
    format_metin db "%s", 10, 0
str_0: db "Merhaba Dünya!", 0
str_1: db "TYD-MLP'ye hoş geldiniz!", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    ; --- Ana Program Akışı ---
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
    ; --- Program Sonu ---
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

; === Fonksiyon Tanımları ===
