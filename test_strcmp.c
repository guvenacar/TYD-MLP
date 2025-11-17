#include <stdio.h>
extern int string_karsilastir(const char* s1, const char* s2);

int main() {
    int r1 = string_karsilastir("Test", "Test");
    int r2 = string_karsilastir("Test", "Baska");
    printf("Esit: %d\n", r1);
    printf("Farkli: %d\n", r2);
    return 0;
}
