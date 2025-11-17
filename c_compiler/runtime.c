// TYD Runtime Library - String Functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

// ✅ Çalışma dizinini binary'nin konumuna çevir (self-host path fix)
void tyd_fix_cwd(void) {
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len < 0 || len >= (ssize_t)sizeof(exe_path)) return;
    exe_path[len] = '\0';
    char* last_slash = strrchr(exe_path, '/');
    if (!last_slash) return;
    *last_slash = '\0';
    chdir(exe_path);
}
