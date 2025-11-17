extern printf
section .data
    format_sayi db "%d", 10, 0
    format_metin db "%s", 10, 0
str_0: db "=== TYD Derleyici Test Programı ===", 0
str_1: db "Ahmet", 0
str_2: db "Yılmaz", 0
str_3: db "Test tamamlandı!", 0

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
    ; --- DegiskenTanimlama: sayi1 ---
    mov rax, 10
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: sayi2 ---
    mov rax, 20
    mov [rbp-16], rax
    mov rax, [rbp-8]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-16]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: isim ---
    mov rax, str_1
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: soyisim ---
    mov rax, str_2
    mov [rbp-32], rax
    mov rax, [rbp-24]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-32]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: toplam ---
    mov rax, [rbp-16]
    push rax
    mov rax, [rbp-8]
    pop rbx
    add rax, rbx
    mov [rbp-40], rax
    mov rax, [rbp-40]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_3
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
