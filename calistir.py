# calistir.py

import subprocess
import sys
import os

# --- Yapılandırma ---
C_RUNTIME = "runtime/runtime.c"
ASM_OUTPUT = "build/output.asm"
OUTPUT_O = "build/output.o"
RUNTIME_O = "build/runtime.o"
EXECUTABLE = "build/program"
# Kullanıcının varsayılan olarak derleyeceği dosya
DEFAULT_SOURCE = "ornekler/test-birlestirme.tyd" 

# --- Yardımcı Fonksiyon ---
def run_command(command, step_name):
    """Komutu çalıştırır ve hata kodu (exit code) sıfır değilse durur."""
    print(f"\n--- {step_name}: Çalıştırılıyor ---")
    
    # Komutun terminal çıktısını görebilmek için (stdout/stderr)
    stdout_pipe = subprocess.PIPE
    stderr_pipe = subprocess.PIPE
    
    # Programı Çalıştırma adımında çıktıyı yakalamak yerine direkt terminale bas
    if step_name == "Programı Çalıştırma":
        stdout_pipe = None
        stderr_pipe = None

    try:
        # check=True: Hata durumunda CalledProcessError fırlatır
        result = subprocess.run(
            command, 
            check=True,  
            text=True,
            stdout=stdout_pipe,
            stderr=stderr_pipe
        )
        
        if result.stdout:
            print(result.stdout)
        
        print(f"✅ {step_name}: BAŞARILI")
        return True
    
    except subprocess.CalledProcessError as e:
        print(f"❌ HATA: {step_name} BAŞARISIZ OLDU.")
        print(f"Komut: {' '.join(command)}")
        print(f"Hata Çıktısı (stderr):\n{e.stderr}")
        sys.exit(1) # Programı hata koduyla sonlandır
    
    except FileNotFoundError:
        print(f"❌ HATA: '{command[0]}' komutu bulunamadı (Örn: gcc veya nasm)")
        sys.exit(1)


# --- Ana Fonksiyon ---
def build_and_run(source_file):
    
    print(f"--- TYD-MLP Derleyici Zinciri Başlatıldı (Kaynak: {source_file}) ---")
    
    # 1. build dizinini oluştur
    os.makedirs('build', exist_ok=True)
    
    # Komut Zinciri (Listeler halinde)
    commands = [
        # 1. C Runtime Derleme
        ["gcc", "-c", "-fPIC", "-no-pie", C_RUNTIME, "-o", RUNTIME_O],
        
        # 2. Python Derleyicisini Çalıştır (ASM üretir)
        [sys.executable, "-m", "src.compiler", source_file],
        
        # 3. Assembly Derleme (NASM)
        ["nasm", "-f", "elf64", ASM_OUTPUT, "-o", OUTPUT_O],
        
        # 4. Bağlama (Link)
        ["gcc", "-no-pie", OUTPUT_O, RUNTIME_O, "-o", EXECUTABLE],
        
        # 5. Programı Çalıştırma
        [f"./{EXECUTABLE}"]
    ]
    
    step_names = [
        "C Runtime Derleme", 
        "Python Derleyici Çalıştırma (ASM Üretimi)", 
        "Assembly Derleme (NASM)", 
        "Bağlama (GCC Link)", 
        "Program Çıktısı"
    ]
    
    for i, cmd in enumerate(commands):
        # Son adım için farklı subprocess.run çağrısı kullanıldı.
        run_command(cmd, step_names[i])

# --- Giriş Noktası ---
if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Komut satırı argümanı varsa onu kullan
        source_file = sys.argv[1]
    else:
        # Yoksa varsayılan test dosyasını kullan
        source_file = DEFAULT_SOURCE
        
    build_and_run(source_file)