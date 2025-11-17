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
str_0: db "=== String İşlemleri Testi ===", 0
str_1: db "Ahmet", 0
str_2: db "Yilmaz", 0
str_3: db "String Birleştirme:", 0
str_4: db "String Uzunluk:", 0
str_5: db "String Karşılaştırma:", 0
str_6: db "Merhaba", 0
str_7: db "Merhaba", 0
str_8: db "Selam", 0
str_9: db "Test tamamlandı!", 0

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
    ; --- DegiskenTanimlama: isim ---
    mov rax, str_1
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: soyisim ---
    mov rax, str_2
    mov [rbp-16], rax
    mov rax, str_3
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: tam_isim ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call string_birlestir
    mov [rbp-24], rax
    mov rax, [rbp-24]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_4
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: uzunluk ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call string_uzunluk
    mov [rbp-32], rax
    mov rax, [rbp-32]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_5
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: test1 ---
    mov rax, str_6
    mov [rbp-40], rax
    ; --- DegiskenTanimlama: test2 ---
    mov rax, str_7
    mov [rbp-48], rax
    ; --- DegiskenTanimlama: test3 ---
    mov rax, str_8
    mov [rbp-56], rax
    ; --- DegiskenTanimlama: sonuc1 ---
    mov rax, [rbp-40]
    push rax
    mov rax, [rbp-48]
    push rax
    pop rsi
    pop rdi
    call string_karsilastir
    mov [rbp-64], rax
    ; --- DegiskenTanimlama: sonuc2 ---
    mov rax, [rbp-40]
    push rax
    mov rax, [rbp-56]
    push rax
    pop rsi
    pop rdi
    call string_karsilastir
    mov [rbp-72], rax
    mov rax, [rbp-64]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-72]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_sayi
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_9
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
