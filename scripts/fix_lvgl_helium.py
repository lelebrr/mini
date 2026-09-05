"""
Fix de build para LVGL 9.x + PlatformIO em Xtensa (ESP32).

A LVGL >= 9.2 empacota assembly ARM (Helium/NEON), ex.:
  src/draw/sw/blend/helium/lv_blend_helium.S
  src/draw/sw/blend/neon/lv_blend_neon.S
O PlatformIO não exclui .S de libs (só .asm) e tenta montar esses arquivos
com o assembler Xtensa, que falha ("unknown opcode 'typedef'").

Este script remove TODO assembly (.S/.s) da cópia da LVGL instalada em
.pio/libdeps antes do build (a LVGL não tem assembly para Xtensa). Se a
lib for reinstalada, os arquivos voltam e são removidos novamente no
próximo build (auto-correção).
"""

import os

Import("env")

lvgl_src = os.path.join(
    env.subst("$PROJECT_LIBDEPS_DIR"),
    env.subst("$PIOENV"),
    "lvgl",
    "src",
)

if os.path.isdir(lvgl_src):
    removed = []
    for root, _dirs, files in os.walk(lvgl_src):
        for fname in files:
            if fname.lower().endswith(".s"):
                os.remove(os.path.join(root, fname))
                removed.append(os.path.relpath(os.path.join(root, fname), lvgl_src))
    if removed:
        print("fix_lvgl_helium: assembly ARM da LVGL removido (incompativel com Xtensa):")
        for rel in removed:
            print("  -", rel)

