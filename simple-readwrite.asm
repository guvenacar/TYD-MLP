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
str_0: db "ornekler/hello.tyd", 0
str_1: db "r", 0
str_2: db "build/output.txt", 0
str_3: db "w", 0
str_4: db "Tamamlandi", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    and rsp, -16  ; Stack alignment (16-byte)
    ; --- DegiskenTanimlama: dosya1 ---
    mov rax, str_0
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: mod_r ---
    mov rax, str_1
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: f1 ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: icerik ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_oku
    mov [rbp-32], rax
    ; --- DegiskenTanimlama: k1 ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-40], rax
    ; --- DegiskenTanimlama: dosya2 ---
    mov rax, str_2
    mov [rbp-48], rax
    ; --- DegiskenTanimlama: mod_w ---
    mov rax, str_3
    mov [rbp-56], rax
    ; --- DegiskenTanimlama: f2 ---
    mov rax, [rbp-48]
    push rax
    mov rax, [rbp-56]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-64], rax
    ; --- DegiskenTanimlama: n ---
    mov rax, [rbp-64]
    push rax
    mov rax, [rbp-32]
    push rax
    pop rsi
    pop rdi
    call dosya_yaz
    mov [rbp-72], rax
    ; --- DegiskenTanimlama: k2 ---
    mov rax, [rbp-64]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-80], rax
    mov rax, str_4
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
