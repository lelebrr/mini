"""Helper: lista quem (fora de src/legacy) faz include de header do legacy."""
import os
import re

ROOT = r"F:\mini"
LEGACY_NAMES = {
    "ble.h", "channel.h", "config.h", "deauth.h", "display.h",
    "frame.h", "minigotchi.h", "mood.h", "parasite.h",
    "pin_config_legacy.h", "platformio.h", "pwnagotchi.h",
    "user_setup.h", "webui.h",
}
PATTERN = re.compile(
    r'#\s*include\s*"(?P<name>' + "|".join(re.escape(n) for n in LEGACY_NAMES) + r')"'
)
hits = []
for base in (os.path.join(ROOT, "include"), os.path.join(ROOT, "src")):
    for root, _, files in os.walk(base):
        if "legacy" in root.split(os.sep):
            continue
        for fn in files:
            if not fn.endswith((".h", ".cpp")):
                continue
            p = os.path.join(root, fn)
            try:
                with open(p, encoding="utf-8", errors="ignore") as fh:
                    for i, line in enumerate(fh, 1):
                        m = PATTERN.search(line)
                        if m:
                            hits.append((p, i, line.rstrip(), m.group("name")))
            except OSError:
                pass

if not hits:
    print("NENHUM include de header do legacy fora de src/legacy/ -- SEGURO MOVER")
else:
    for p, i, line, name in hits:
        print(f"{p}:{i}: {line}   <- {name}")
