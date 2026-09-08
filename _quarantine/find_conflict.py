import os
root = r"F:\PIO\packages\framework-arduinoespressif32"
hits = []
for base, _, files in os.walk(root):
    for fn in files:
        if fn.endswith((".c", ".cpp", ".h", ".hpp")):
            p = os.path.join(base, fn)
            try:
                with open(p, encoding="utf-8", errors="ignore") as fh:
                    for i, line in enumerate(fh, 1):
                        if "CONFLICT" in line:
                            hits.append((p, i, line.rstrip()))
            except OSError:
                pass
print(f"Total hits: {len(hits)}")
for p, i, line in hits[:15]:
    print(f"{p}:{i}: {line}")

