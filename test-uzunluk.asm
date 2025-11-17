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
str_0: db "Merhaba Dunya TYD!", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    and rsp, -16  ; Stack alignment (16-byte)
    ; --- DegiskenTanimlama: mesaj ---
    mov rax, str_0
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: uzunluk ---
    mov rax, [rbp-8]
    push rax
    pop rdi
    call string_uzunluk
    mov [rbp-16], rax
    mov rax, [rbp-16]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
