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
str_0: db "=== TYD Self-Hosting Demo v1 ===", 0
str_1: db "Calisma dizini aliniyor...", 0
str_2: db "Kaynak kod yolu olusturuluyor...", 0
str_3: db "/../ornekler/01-merhaba.tyd", 0
str_4: db "r", 0
str_5: db "Okunan kaynak:", 0
str_6: db "Assembly uretiliyor...", 0
str_7: db "extern printf\n", 0
str_8: db "section .data\n", 0
str_9: db "section .text\n", 0
str_10: db "global main\n", 0
str_11: db "/../build/generated.asm", 0
str_12: db "w", 0
str_13: db "Olusturuldu!", 0

section .text
global main
main:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    call tyd_fix_cwd
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
    ; --- DegiskenTanimlama: ana_dizin ---
    ; --- DIZIN_AL cagirma ---
    mov rax, 0
    call runtime_dizin_al
    mov [rbp-8], rax
    mov rax, [rbp-8]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_2
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: kaynak_yol_eki ---
    mov rax, str_3
    mov [rbp-16], rax
    ; --- DegiskenTanimlama: kaynak_dosya ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-16]
    push rax
    pop rsi
    pop rdi
    call STRING_BIRLESTIR
    mov [rbp-24], rax
    mov rax, [rbp-24]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: mod ---
    mov rax, str_4
    mov [rbp-32], rax
    ; --- DegiskenTanimlama: dosya ---
    mov rax, [rbp-24]
    push rax
    mov rax, [rbp-32]
    pop rdi
    mov rsi, rax
    call dosya_ac
    mov [rbp-40], rax
    ; --- DegiskenTanimlama: kaynak ---
    mov rax, [rbp-40]
    mov rdi, rax
    call dosya_oku
    mov [rbp-48], rax
    ; --- DegiskenTanimlama: kapat1 ---
    mov rax, [rbp-40]
    mov rdi, rax
    call dosya_kapat
    mov [rbp-56], rax
    mov rax, str_5
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-48]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, str_6
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: asm1 ---
    mov rax, str_7
    mov [rbp-64], rax
    ; --- DegiskenTanimlama: asm2 ---
    mov rax, str_8
    mov [rbp-72], rax
    ; --- DegiskenTanimlama: asm3 ---
    mov rax, str_9
    mov [rbp-80], rax
    ; --- DegiskenTanimlama: asm4 ---
    mov rax, str_10
    mov [rbp-88], rax
    ; --- DegiskenTanimlama: cikti_yol_eki ---
    mov rax, str_11
    mov [rbp-96], rax
    ; --- DegiskenTanimlama: cikti_dosya ---
    mov rax, [rbp-8]
    push rax
    mov rax, [rbp-96]
    push rax
    pop rsi
    pop rdi
    call STRING_BIRLESTIR
    mov [rbp-104], rax
    mov rax, [rbp-104]
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    ; --- DegiskenTanimlama: mod_yaz ---
    mov rax, str_12
    mov [rbp-112], rax
    ; --- DegiskenTanimlama: cikti ---
    mov rax, [rbp-104]
    push rax
    mov rax, [rbp-112]
    pop rdi
    mov rsi, rax
    call dosya_ac
    mov [rbp-120], rax
    ; --- DegiskenTanimlama: n1 ---
    mov rax, [rbp-120]
    push rax
    mov rax, [rbp-64]
    pop rdi
    mov rsi, rax
    call dosya_yaz
    mov [rbp-128], rax
    ; --- DegiskenTanimlama: n2 ---
    mov rax, [rbp-120]
    push rax
    mov rax, [rbp-72]
    pop rdi
    mov rsi, rax
    call dosya_yaz
    mov [rbp-136], rax
    ; --- DegiskenTanimlama: n3 ---
    mov rax, [rbp-120]
    push rax
    mov rax, [rbp-80]
    pop rdi
    mov rsi, rax
    call dosya_yaz
    mov [rbp-144], rax
    ; --- DegiskenTanimlama: n4 ---
    mov rax, [rbp-120]
    push rax
    mov rax, [rbp-88]
    pop rdi
    mov rsi, rax
    call dosya_yaz
    mov [rbp-152], rax
    ; --- DegiskenTanimlama: kapat2 ---
    mov rax, [rbp-120]
    mov rdi, rax
    call dosya_kapat
    mov [rbp-160], rax
    mov rax, str_13
    ; --- YAZDIR Başlangıç ---
    mov rdi, format_metin
    mov rsi, rax
    mov rax, 0
    call printf
    mov rax, [rbp-104]
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

section .note.GNU-stack noalloc noexec nowrite progbits

db 0
