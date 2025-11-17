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
str_0: db "Dosya I/O Testi", 0
str_1: db "output.txt", 0
str_2: db "w", 0
str_3: db "Merhaba TYD!", 0
str_4: db "r", 0
str_5: db "Tamamlandi", 0

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
    ; --- DegiskenTanimlama: yazmod ---
    mov rax, str_2
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: f ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: mesaj ---
    mov rax, str_3
    mov [rbp-32], rax
    ; --- DegiskenTanimlama: yazilan ---
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-32]
    push rax
    pop rsi
    pop rdi
    call dosya_yaz
    mov [rbp-40], rax
    ; --- DegiskenTanimlama: kapat1 ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-48], rax
    ; --- DegiskenTanimlama: okumod ---
    mov rax, str_4
    mov [rbp-56], rax
    ; --- DegiskenTanimlama: f2 ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-56]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-64], rax
    ; --- DegiskenTanimlama: icerik ---
    mov rax, [rbp-64]
    push rax
    pop rdi
    call dosya_oku
    mov [rbp-72], rax
    mov rax, [rbp-72]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: kapat2 ---
    mov rax, [rbp-64]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-80], rax
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
