#!/usr/bin/env python3
"""Gera compile_commands.json e auto-cura .vscode/c_cpp_properties.json.

O PlatformIO nao expoe compile_commands.json nativamente para ESP32-Arduino.
Este script tenta `pio run -t compiledb` primeiro e, na falta, monta a partir
do .pio/build/<env>/src/*.includes. Tambem reescreve o c_cpp_properties.json
descobrindo dinamicamente todos os paths existentes em lib/ e libdeps/,
descartando paths stale.
"""
import json
import os
import re
import shutil
import subprocess
import sys

ROOT = r"F:\mini"
ENV  = "waveshare-esp32-s3-amoled"
OUT  = os.path.join(ROOT, "compile_commands.json")
CPP_PROPS = os.path.join(ROOT, ".vscode", "c_cpp_properties.json")

STATIC_PATHS = [
    "${workspaceFolder}/include",
    "${workspaceFolder}/src",
    "F:/PIO/packages/framework-arduinoespressif32/cores/esp32",
    "F:/PIO/packages/framework-arduinoespressif32/libraries",
    "F:/PIO/packages/framework-arduinoespressif32/variants/esp32s3",
    "F:/PIO/packages/framework-arduinoespressif32-libs/esp32s3/include",
    "F:/PIO/packages/toolchain-xtensa-esp-elf/xtensa-esp-elf/include",
    "F:/PIO/packages/toolchain-xtensa-esp-elf/xtensa-esp-elf/include/c++/14.2.0",
    "F:/PIO/packages/toolchain-xtensa-esp-elf/xtensa-esp-elf/include/c++/14.2.0/xtensa-esp-elf",
]

def try_pio_compiledb():
    try:
        r = subprocess.run(
            ["python", "-m", "platformio", "run", "-e", ENV, "-t", "compiledb"],
            cwd=ROOT, capture_output=True, text=True, timeout=600,
        )
        for line in (r.stdout + "\n" + r.stderr).splitlines():
            # Saidas observadas: "Building compilation database compile_commands.json"
            # ou: "compilation database written to /path/compile_commands.json"
            m = re.search(r"compilation database (?:\S+\s+)?(compile_commands\.json|\S+\.json|\S+\\compile_commands\.json)", line)
            if m:
                p = m.group(1).strip().strip('"')
                if not os.path.isabs(p):
                    # Geralmente fica em .pio/build/<env>/ ou na raiz do projeto
                    for cand in (os.path.join(ROOT, p), os.path.join(ROOT, ".pio", "build", ENV, p)):
                        if os.path.isfile(cand):
                            p = cand
                            break
                if os.path.isfile(p):
                    return p
    except (subprocess.TimeoutExpired, FileNotFoundError):
        pass
    return None


def discover_paths():
    """Auto-descobre paths existentes em lib/ e .pio/libdeps/<env>/."""
    found = []
    lib_dir = os.path.join(ROOT, "lib")
    if os.path.isdir(lib_dir):
        for entry in os.listdir(lib_dir):
            p = os.path.join(lib_dir, entry)
            if not os.path.isdir(p):
                continue
            for cand in (os.path.join(p, "src"), p):
                if os.path.isdir(cand):
                    tail = "/src" if cand.endswith("src") else ""
                    found.append("F:/mini/lib/" + entry + tail)
                    break
    libdeps = os.path.join(ROOT, ".pio", "libdeps", ENV)
    if os.path.isdir(libdeps):
        for entry in os.listdir(libdeps):
            for cand in (os.path.join(libdeps, entry, "src"), os.path.join(libdeps, entry)):
                if os.path.isdir(cand):
                    rel = os.path.relpath(cand, ROOT).replace(os.sep, "/")
                    found.append("F:/mini/" + rel)
                    break
    return found


def collect_compile_includes(entries):
    """Extrai todos os -I do compile_commands.json (so os paths que existem)."""
    incs = set()
    for e in entries:
        cmd = e.get("command", "") or " ".join(e.get("arguments", []))
        for m in re.finditer(r"-I\s+(\S+)", cmd):
            raw = m.group(1)
            if raw.startswith(ROOT):
                rel = os.path.relpath(raw, ROOT).replace(os.sep, "/")
                if not rel.startswith(".."):
                    cand = "${workspaceFolder}/" + rel
                    if os.path.isdir(rel):
                        incs.add(cand)
            elif os.path.isabs(raw) and os.path.isdir(raw):
                incs.add(raw.replace(os.sep, "/"))
    return incs


def write_compile_commands(entries, label):
    with open(OUT, "w", encoding="utf-8") as fh:
        json.dump(entries, fh, indent=2)
    size_mb = os.path.getsize(OUT) / (1024 * 1024)
    print(f"compile_commands.json: {len(entries)} TUs, {size_mb:.1f} MB [{label}]")
    return collect_compile_includes(entries)


def rebuild_cpp_properties(extra_includes):
    """Reescreve c_cpp_properties.json: STATIC + discovered + extras do PIO.
    Descarta silenciosamente qualquer path que nao existe no disco."""
    valid = []
    for p in STATIC_PATHS:
        real = p.replace("${workspaceFolder}", ROOT).replace("/", os.sep)
        if os.path.isdir(real):
            valid.append(p)
    for p in discover_paths():
        if os.path.isdir(p.replace("/", os.sep)):
            valid.append(p)
    for p in sorted(extra_includes):
        if p in valid:
            continue
        real = p.replace("${workspaceFolder}", ROOT).replace("/", os.sep)
        if os.path.isdir(real):
            valid.append(p)

    cfg = {
        "configurations": [
            {
                "name": "Mini Lele (ESP32-S3)",
                "compileCommands": ["${workspaceFolder}/compile_commands.json"],
                "compilerPath": "F:/PIO/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp32s3-elf-g++.exe",
                "cStandard": "c17",
                "cppStandard": "c++17",
                "intelliSenseMode": "gcc-x64",
                "configurationProvider": "ms-vscode.cpptools",
                "forcedInclude": [],
                "browse": {
                    "path": valid,
                    "limitSymbolsToIncludedHeaders": True,
                    "databaseFilename": "",
                },
            }
        ],
        "version": 4,
    }
    os.makedirs(os.path.dirname(CPP_PROPS), exist_ok=True)
    with open(CPP_PROPS, "w", encoding="utf-8") as fh:
        json.dump(cfg, fh, indent=2)
    print(f"c_cpp_properties.json: {len(valid)} paths validos "
          f"({len(STATIC_PATHS)} estaticos + {len(discover_paths())} descobertos + extras)")


def fallback_build_parse():
    """Monta compile_commands.json varrendo .pio/build/<env>/src/*.includes."""
    build_dir = os.path.join(ROOT, ".pio", "build", ENV)
    if not os.path.isdir(build_dir):
        print(f"ERRO: faca `pio run -e {ENV}` antes.", file=sys.stderr)
        sys.exit(1)

    entries = []
    for root, _, files in os.walk(os.path.join(build_dir, "src")):
        for fn in files:
            if not fn.endswith(".o"):
                continue
            base = fn[:-2]
            inc_file = os.path.join(root, base + ".includes")
            if not os.path.isfile(inc_file):
                continue
            with open(inc_file, encoding="utf-8", errors="ignore") as fh:
                includes = [l.split()[1] for l in fh if l.strip() and len(l.split()) >= 2]
            src = None
            for ext in (".cpp", ".c", ".cc", ".cxx"):
                cand = os.path.join(root, base + ext)
                if os.path.isfile(cand):
                    src = cand
                    break
            if not src:
                continue
            entries.append({
                "directory": build_dir,
                "file": src,
                "arguments": ["clang", "-c", "-std=c++17", "-DARDUINO=200"] + sum([["-I", p] for p in includes], []),
            })
    if not entries:
        print("Nenhuma TU encontrada; rode o build antes.", file=sys.stderr)
        sys.exit(1)
    return entries


def main():
    pio_db = try_pio_compiledb()
    if pio_db:
        shutil.copy(pio_db, OUT)
        with open(OUT, encoding="utf-8") as fh:
            entries = json.load(fh)
        extras = write_compile_commands(entries, "via compiledb")
    else:
        print("compiledb indisponivel, montando a partir do build existente...")
        entries = fallback_build_parse()
        extras = write_compile_commands(entries, "fallback includes.list")
    rebuild_cpp_properties(extras)
    return 0


if __name__ == "__main__":
    sys.exit(main())
