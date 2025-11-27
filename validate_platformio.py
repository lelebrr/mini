#!/usr/bin/env python3
"""
Script de validação da configuração PlatformIO para Minigotchi ESP32
Este script verifica se todos os arquivos necessários estão presentes e corretamente configurados.
"""

import os
import sys
from pathlib import Path

def check_file_exists(filepath, description):
    """Verifica se um arquivo existe."""
    if os.path.exists(filepath):
        print(f"✅ {description}: {filepath}")
        return True
    else:
        print(f"❌ {description}: {filepath} (NÃO ENCONTRADO)")
        return False

def check_platformio_ini():
    """Verifica a configuração do platformio.ini."""
    print("\n🔍 Verificando configuração do PlatformIO...")
    
    if not os.path.exists("platformio.ini"):
        print("❌ Arquivo platformio.ini não encontrado")
        return False
    
    # Lê o arquivo e verifica seções importantes
    with open("platformio.ini", "r") as f:
        content = f.read()
        
    required_sections = ["[env:esp32-s3-devkitc-1]", "platform =", "board =", "framework ="]
    missing_sections = []
    
    for section in required_sections:
        if section not in content:
            missing_sections.append(section)
    
    if missing_sections:
        print(f"❌ Seções obrigatórias faltando no platformio.ini: {missing_sections}")
        return False
    
    print("✅ Configuração do platformio.ini está correta")
    return True

def check_dependencies():
    """Verifica as dependências do projeto."""
    print("\n🔍 Verificando dependências...")
    
    if not os.path.exists("lib_deps.txt"):
        print("❌ Arquivo lib_deps.txt não encontrado")
        return False
    
    with open("lib_deps.txt", "r") as f:
        content = f.read()
    
    required_libs = ["TFT_eSPI", "Adafruit GFX Library", "Adafruit BusIO", "Adafruit SH8601", "Adafruit AXP2101"]
    missing_libs = []
    
    for lib in required_libs:
        if lib not in content:
            missing_libs.append(lib)
    
    if missing_libs:
        print(f"❌ Bibliotecas obrigatórias faltando em lib_deps.txt: {missing_libs}")
        return False
    
    print("✅ Dependências estão corretas")
    return True

def check_source_files():
    """Verifica os arquivos fonte principais."""
    print("\n🔍 Verificando arquivos fonte...")
    
    required_files = [
        "minigotchi-ESP32.ino",
        "include/platformio.h",
        "config.h",
        "minigotchi.h",
        "pin_config.h"
    ]
    
    all_exist = True
    for file in required_files:
        if not check_file_exists(file, f"Arquivo fonte"):
            all_exist = False
    
    return all_exist

def check_documentation():
    """Verifica a documentação."""
    print("\n🔍 Verificando documentação...")
    
    doc_files = ["README_PlatformIO.md"]
    all_exist = True
    
    for file in doc_files:
        if not check_file_exists(file, "Documentação"):
            all_exist = False
    
    return all_exist

def main():
    print("🚀 Validando configuração PlatformIO para Minigotchi ESP32")
    print("=" * 60)
    
    checks = [
        check_platformio_ini(),
        check_dependencies(),
        check_source_files(),
        check_documentation()
    ]
    
    if all(checks):
        print("\n🎉 Tudo está correto! A configuração PlatformIO está pronta para uso.")
        print("\nPróximos passos:")
        print("1. Instale o PlatformIO: https://platformio.org/install")
        print("2. Instale as dependências: pio lib install")
        print("3. Compile o projeto: pio run")
        print("4. Faça o upload: pio run --target upload")
        print("5. Inicie o monitor: pio device monitor")
        return 0
    else:
        print("\n❌ Alguns problemas foram encontrados. Verifique os itens acima.")
        return 1

if __name__ == "__main__":
    sys.exit(main())