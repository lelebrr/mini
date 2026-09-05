#!/usr/bin/env python3
"""Gera o par WebAssets.h (header limpo) + WebAssets.cpp (HTML real) a
partir de webui_preview.html (a fonte canonica do HTML). Idempotente.
"""
import os

ROOT = r"F:\mini"
SRC_HTML = os.path.join(ROOT, "webui_preview.html")
HEADER   = os.path.join(ROOT, "include", "web", "WebAssets.h")
CPP      = os.path.join(ROOT, "src", "web_assets.cpp")

with open(SRC_HTML, encoding="utf-8") as fh:
    html = fh.read()
assert html.startswith("<!DOCTYPE html>"), "Fonte HTML corrompida"
assert html.rstrip().endswith("</html>"),   "Fonte HTML corrompida (sem </html>)"

NEW_HEADER = """\
#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// WebUI do Mini Lele - 100% autocontida (sem CDN externo), funciona offline
// no modo AP. O HTML fica em WebAssets.cpp (nao em header!) para nao inflar
// cada translation unit que incluir este header. Declaracao externa aqui.

extern const char index_html[];

#endif
"""
with open(HEADER, "w", encoding="utf-8", newline="") as fh:
    fh.write(NEW_HEADER)
print(f"Header: {HEADER} ({os.path.getsize(HEADER)} bytes)")

NEW_CPP = (
    "// Autogerado por scripts/fix_web_assets.py a partir de webui_preview.html.\n"
    "// NAO edite este arquivo: edite webui_preview.html e rode o script.\n"
    "\n"
    '#include "web/WebAssets.h"\n'
    "\n"
    "const char index_html[] = R\"rawliteral(\n"
    f"{html}"
    ")rawliteral\";\n"
)
with open(CPP, "w", encoding="utf-8", newline="") as fh:
    fh.write(NEW_CPP)
print(f"CPP:    {CPP} ({os.path.getsize(CPP)} bytes)")
print("OK")

