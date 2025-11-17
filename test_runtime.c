#include <stdio.h>
#include <stdint.h>

extern int64_t dosya_ac(const char* yol, const char* mod);
extern int64_t dosya_yaz(int64_t dosya_ptr, const char* veri);
extern int64_t dosya_kapat(int64_t dosya_ptr);

int main() {
    printf("Testing dosya_ac...\n");
    int64_t f = dosya_ac("test.txt", "w");
    printf("File pointer: %ld\n", f);

    printf("Testing dosya_yaz...\n");
    int64_t n = dosya_yaz(f, "Hello from C!\n");
    printf("Bytes written: %ld\n", n);

    printf("Testing dosya_kapat...\n");
    int64_t r = dosya_kapat(f);
    printf("Close result: %ld\n", r);

    return 0;
}
