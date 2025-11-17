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
str_0: db "=== Dosya I/O Testi ===", 0
str_1: db "test_cikti.txt", 0
str_2: db "w", 0
str_3: db "Dosya açılıyor (yazma modu)...", 0
str_4: db "Merhaba TYD Dünyası!", 0
str_5: db "Dosyaya yazılıyor...", 0
str_6: db "Dosya kapatılıyor...", 0
str_7: db "Dosya açılıyor (okuma modu)...", 0
str_8: db "r", 0
str_9: db "Dosyadan okunuyor...", 0
str_10: db "Okunan içerik:", 0
str_11: db "Test tamamlandı!", 0

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
    ; --- DegiskenTanimlama: dosya_yolu ---
    mov rax, str_1
    mov [rbp-8], rax
    ; --- DegiskenTanimlama: mod_yaz ---
    mov rax, str_2
    mov [rbp-16], rax
    mov rax, str_3
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: dosya ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-24], rax
    ; --- DegiskenTanimlama: mesaj ---
    mov rax, str_4
    mov [rbp-32], rax
    mov rax, str_5
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: yazilan ---
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-32]
    push rax
    pop rsi
    pop rdi
    call dosya_yaz
    mov [rbp-40], rax
    mov rax, str_6
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: sonuc1 ---
    mov rax, [rbp-24]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-48], rax
    mov rax, str_7
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: mod_oku ---
    mov rax, str_8
    mov [rbp-56], rax
    ; --- DegiskenTanimlama: dosya2 ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-56]
    push rax
    pop rsi
    pop rdi
    call dosya_ac
    mov [rbp-64], rax
    mov rax, str_9
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: icerik ---
    mov rax, [rbp-64]
    push rax
    pop rdi
    call dosya_oku
    mov [rbp-72], rax
    mov rax, str_10
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-72]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: sonuc2 ---
    mov rax, [rbp-64]
    push rax
    pop rdi
    call dosya_kapat
    mov [rbp-80], rax
    mov rax, str_11
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret
