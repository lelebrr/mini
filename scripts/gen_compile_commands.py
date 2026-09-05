#!/usr/bin/env python3
"""
Gera compile_commands.json para o projeto.

O PlatformIO nao expoe compile_commands.json nativamente para ESP32-Arduino
(o plugin 'eclipse CDT' sobe .cproject, nao o formato que o VS Code espera).
Este script tenta `pio run -t compiledb` primeiro (plugin oficial), e cai
de volta para um parsing manual do .pio/build/.../includes.list.
"""
import json
import os
import re
import shutil
import subprocess
import sys

ROOT = r"F:\mini"
OUT  = os.path.join(ROOT, "compile_commands.json")


def try_pio_compiledb(env):
    try:
        r = subprocess.run(
            ["python", "-m", "platformio", "run", "-e", env, "-t", "compiledb"],
            cwd=ROOT, capture_output=True, text=True, timeout=600,
        )
        out = r.stdout + "\n" + r.stderr
        for line in out.splitlines():
            m = re.search(r"compilation database (?:written to|generated at)\s+(.+)", line)
            if m:
                p = m.group(1).strip().strip('"')
                if os.path.isfile(p):
                    return p
    except (subprocess.TimeoutExpired, FileNotFoundError):
        pass
    return None


def main():
    pio_db = try_pio_compiledb("waveshare-esp32-s3-amoled")
    if pio_db:
        shutil.copy(pio_db, OUT)
        print(f"OK via compiledb: {pio_db} -> {OUT}")
        return

    print("compiledb indisponivel, montando a partir do build existente...")
    build_dir = os.path.join(ROOT, ".pio", "build", "waveshare-esp32-s3-amoled")
    if not os.path.isdir(build_dir):
        print("ERRO: faca `pio run -e waveshare-esp32-s3-amoled` antes.", file=sys.stderr)
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

    with open(OUT, "w", encoding="utf-8") as fh:
        json.dump(entries, fh, indent=2)
    print(f"OK: {OUT} ({len(entries)} TUs)")


if __name__ == "__main__":
    main()

