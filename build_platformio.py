#!/usr/bin/env python3
"""
Script de build para PlatformIO do Minigotchi ESP32
Este script automatiza o processo de build e upload do firmware.
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def run_command(cmd, description):
    """Executa um comando e exibe o resultado."""
    print(f"\n{'='*50}")
    print(f"🔄 {description}")
    print(f"{'='*50}")
    print(f"Comando: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print("✅ Sucesso!")
        if result.stdout:
            print("Saída:")
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Erro ao executar comando: {e}")
        if e.stderr:
            print("Erro:")
            print(e.stderr)
        return False

def install_dependencies():
    """Instala as dependências do projeto."""
    return run_command(
        ["pio", "lib", "install"],
        "Instalando dependências do PlatformIO"
    )

def build_project():
    """Compila o projeto."""
    return run_command(
        ["pio", "run"],
        "Compilando o projeto Minigotchi ESP32"
    )

def upload_firmware():
    """Faz o upload do firmware para o dispositivo."""
    return run_command(
        ["pio", "run", "--target", "upload"],
        "Fazendo upload do firmware para o ESP32"
    )

def monitor_serial():
    """Inicia o monitor serial."""
    print("\n🔍 Iniciando monitor serial...")
    print("Pressione Ctrl+C para sair")
    try:
        subprocess.run(["pio", "device", "monitor"], check=True)
    except KeyboardInterrupt:
        print("\n👋 Monitor serial encerrado")
    except subprocess.CalledProcessError as e:
        print(f"❌ Erro no monitor serial: {e}")

def clean_build():
    """Limpa os arquivos de build."""
    return run_command(
        ["pio", "run", "--target", "clean"],
        "Limpando arquivos de build"
    )

def main():
    parser = argparse.ArgumentParser(description="Script de build para PlatformIO do Minigotchi ESP32")
    parser.add_argument("--install", action="store_true", help="Instala dependências")
    parser.add_argument("--build", action="store_true", help="Compila o projeto")
    parser.add_argument("--upload", action="store_true", help="Faz upload do firmware")
    parser.add_argument("--monitor", action="store_true", help="Inicia monitor serial")
    parser.add_argument("--clean", action="store_true", help="Limpa arquivos de build")
    parser.add_argument("--all", action="store_true", help="Executa todos os passos: install, build, upload")
    
    args = parser.parse_args()
    
    # Verifica se o PlatformIO está instalado
    try:
        subprocess.run(["pio", "--version"], check=True, capture_output=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ PlatformIO não encontrado. Por favor, instale o PlatformIO primeiro.")
        print("Instalação: https://platformio.org/install")
        sys.exit(1)
    
    # Executa as ações solicitadas
    if args.all:
        success = (install_dependencies() and 
                  build_project() and 
                  upload_firmware())
    else:
        success = True
        if args.install:
            success = install_dependencies()
        if args.build:
            success = build_project()
        if args.upload:
            success = upload_firmware()
        if args.monitor:
            monitor_serial()
            return
        if args.clean:
            success = clean_build()
    
    if success:
        print("\n🎉 Operação concluída com sucesso!")
        if not args.monitor:
            print("Para iniciar o monitor serial, use: python build_platformio.py --monitor")
    else:
        print("\n❌ Operação falhou. Verifique os erros acima.")
        sys.exit(1)

if __name__ == "__main__":
    main()