#!/usr/bin/env python3
"""
Script de validação da configuração PlatformIO para Mini Lele (ESP32-S3)

Este script verifica se:
- O ambiente principal em platformio.ini está correto
- As dependências em lib_deps.txt estão alinhadas com o firmware atual
- Os arquivos fonte e cabeçalhos principais existem
- Não há marcadores de merge pendentes fora de src/legacy
"""

import os
import sys
from pathlib import Path


def check_file_exists(filepath: str, description: str) -> bool:
    """Verifica se um arquivo existe e imprime um resultado legível."""
    if os.path.exists(filepath):
        print(f"✅ {description}: {filepath}")
        return True
    else:
        print(f"❌ {description}: {filepath} (NÃO ENCONTRADO)")
        return False


def check_platformio_ini() -> bool:
    """Verifica a configuração básica do platformio.ini."""
    print("\n🔍 Verificando configuração do PlatformIO...")

    ini_path = Path("platformio.ini")
    if not ini_path.exists():
        print("❌ Arquivo platformio.ini não encontrado")
        return False

    content = ini_path.read_text(encoding="utf-8", errors="ignore")

    required_snippets = [
        "[env:waveshare-esp32-s3-amoled]",
        "platform =",
        "board =",
        "framework =",
    ]
    missing = [s for s in required_snippets if s not in content]

    if missing:
        print(
            "❌ Seções obrigatórias faltando ou divergentes em platformio.ini:"
        )
        for s in missing:
            print(f"   - {s}")
        return False

    print("✅ Configuração básica do platformio.ini está correta")
    return True


def check_dependencies() -> bool:
    """Verifica as dependências descritas em lib_deps.txt (documentação)."""
    print("\n🔍 Verificando dependências (lib_deps.txt)...")

    deps_path = Path("lib_deps.txt")
    if not deps_path.exists():
        print("❌ Arquivo lib_deps.txt não encontrado")
        return False

    content = deps_path.read_text(encoding="utf-8", errors="ignore")

    # Padrões mínimos esperados de acordo com a arquitetura atual
    required = {
        "GFX Library for Arduino": "Biblioteca gráfica Arduino_GFX",
        "lvgl": "LVGL (UI)",
        "XPowersLib": "XPowersLib (PMU AXP2101)",
        "SensorLib": "SensorLib (IMU QMI8658)",
        "ArduinoJson": "ArduinoJson (config / WebUI)",
        "ESPAsyncWebServer": "ESPAsyncWebServer (WebUI)",
        "AsyncTCP": "AsyncTCP (stack assíncrona)",
    }

    missing = []
    for key, desc in required.items():
        if key not in content:
            missing.append(desc)

    if missing:
        print("❌ Algumas bibliotecas importantes parecem ausentes em lib_deps.txt:")
        for m in missing:
            print(f"   - {m}")
        return False

    # Aviso leve para biblioteca de touch, que varia por fornecedor
    if "TouchLib" not in content and "FT3168" not in content and "CST816" not in content:
        print(
            "ℹ️  Aviso: biblioteca de touch (FT3168/GT1151) não encontrada em lib_deps.txt."
        )
        print("   Adicione a entrada correspondente à biblioteca usada pelo seu hardware.")

    print("✅ Dependências básicas descritas em lib_deps.txt")
    return True


def check_source_files() -> bool:
    """Verifica a presença dos arquivos-fonte e cabeçalhos principais."""
    print("\n🔍 Verificando arquivos fonte principais...")

    required_files = [
        "src/main.cpp",
        "src/core_singletons.cpp",
        "include/pin_config.h",
        "include/lv_conf.h",
        "include/core/ConfigManager.h",
    ]

    all_exist = True
    for file in required_files:
        if not check_file_exists(file, "Arquivo/cabeçalho"):
            all_exist = False

    return all_exist


def check_documentation() -> bool:
    """Verifica se a documentação principal existe."""
    print("\n🔍 Verificando documentação...")

    doc_files = [
        "README_PlatformIO.md",
        "README.md",
        "MANUAL.md",
        "INSTALACAO.md",
    ]
    all_exist = True

    for file in doc_files:
        if not check_file_exists(file, "Documentação"):
            all_exist = False

    return all_exist


def check_merge_markers() -> bool:
    """
    Garante que não existam marcadores de merge (<<<<<<< HEAD)
    fora de src/legacy, que é mantida apenas como referência histórica.
    """
    print("\n🔍 Verificando marcadores de merge (<<<<<<< HEAD)...")

    root = Path(".").resolve()
    bad_files = []

    skip_dirs = {
        ".git",
        ".pio",
        ".vscode",
        "__pycache__",
        "sd_out",
        "sd_card_files",
    }

    for path in root.rglob("*"):
        if path.is_dir():
            continue

        rel = path.relative_to(root)
        parts = rel.parts

        # Ignora diretórios auxiliares e código legado
        if len(parts) >= 2 and parts[0] == "src" and parts[1] == "legacy":
            continue
        if any(part in skip_dirs for part in parts):
            continue

        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue

        # Considera apenas marcadores reais de merge, no início da linha,
        # para não acusar falsos positivos em comentários ou strings.
        for line in text.splitlines():
            if line.lstrip().startswith("<<<<<<< "):
                bad_files.append(str(rel))
                break

    if bad_files:
        print("❌ Marcadores de merge encontrados fora de src/legacy:")
        for p in bad_files:
            print(f"   - {p}")
        return False

    print("✅ Nenhum marcador de merge fora de src/legacy")
    return True


def main() -> int:
    print("🚀 Validando configuração PlatformIO para Mini Lele (ESP32-S3)")
    print("=" * 60)

    checks = [
        check_platformio_ini(),
        check_dependencies(),
        check_source_files(),
        check_documentation(),
        check_merge_markers(),
    ]

    if all(checks):
        print("\n🎉 Tudo está correto! A configuração parece consistente.")
        print("\nPróximos passos sugeridos:")
        print("1. Instale o PlatformIO: https://platformio.org/install")
        print("2. Compile: pio run -e waveshare-esp32-s3-amoled")
        print("3. Upload: pio run -e waveshare-esp32-s3-amoled -t upload")
        print("4. Monitor: pio device monitor -e waveshare-esp32-s3-amoled")
        return 0

    print("\n❌ Alguns problemas foram encontrados. Verifique os itens acima.")
    return 1


if __name__ == "__main__":
    sys.exit(main())