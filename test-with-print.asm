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
str_0: db "Basliyor", 0
str_1: db "test.txt", 0
str_2: db "w", 0
str_3: db "Dosya aciliyor", 0
str_4: db "Kapat iliyor", 0
str_5: db "Bitti", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    and rsp, -16  ; Stack alignment (16-byte)
    mov rax, str_0
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: yol ---
    mov rax, str_1
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: mod ---
    mov rax, str_2
    mov [rbp-16], rax
    mov rax, str_3
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: f ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-24], rax
    mov rax, str_4
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: k ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-32], rax
    mov rax, str_5
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
