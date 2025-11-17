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

section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    ; --- Ana Program Akışı ---
    call ana_hesaplama
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, 100
    push rax
    mov rax, 1
    push rax
    pop rsi
    pop rdi
    call topla
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- Program Sonu ---
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

; === Fonksiyon Tanımları ===
topla:
    ; --- Islec Tanimlama: topla ---
    push rbp
    mov rbp, rsp
    sub rsp, 128
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
ana_hesaplama:
    ; --- Islec Tanimlama: ana_hesaplama ---
    push rbp
    mov rbp, rsp
    sub rsp, 128
    ; --- DegiskenTanimlama: x ---
    mov rax, 10
    push rax
    mov rax, 5
    push rax
    pop rsi
    pop rdi
    call topla
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: y ---
    mov rax, [rbp-8]
    push rax
    mov rax, 2
    push rax
    pop rsi
    pop rdi
    call topla
    mov [rbp-16], rax
    ; --- Donus Komutu ---
    mov rax, [rbp-16]
    mov rsp, rbp
    pop rbp
    ret
