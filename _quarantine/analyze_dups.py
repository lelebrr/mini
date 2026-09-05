import re
import sys


SIG = re.compile(
    r"^(?:static\s+)?(?:[A-Za-z_][A-Za-z0-9_]*[\s\*]+)+([A-Za-z_][A-Za-z0-9_]*)\s*\("
)


def read(p):
    with open(p, encoding="utf-8") as fh:
        return fh.readlines()


def def_lines(lines, name):
    """linhas (1-based) onde `name` é DEFINIDO (assinatura seguida de `{`)"""
    out = []
    for i, ln in enumerate(lines):
        m = SIG.match(ln.rstrip("\n"))
        if m and m.group(1) == name:
            # procurar `{` nas próximas 3 linhas (estilo Bosch)
            for j in range(i, min(i + 3, len(lines))):
                if lines[j].strip() == "{":
                    out.append(i + 1)
                    break
    return out


def block_end(lines, start):
    """linha (1-based) do `}` de fechamento do bloco iniciado em start (0-based idx)"""
    depth = 0
    for j in range(start - 1, len(lines)):
        depth += lines[j].count("{") - lines[j].count("}")
        if depth == 0 and j >= start - 1:
            return j + 1
    return len(lines)


def analyze(path, names):
    lines = read(path)
    print(f"== {path} ({len(lines)} linhas) ==")
    for name in names:
        d = def_lines(lines, name)
        ends = [block_end(lines, s) for s in d]
        print(f"  {name}: defs={d} ends={ends}")
    return lines


if __name__ == "__main__":
    analyze(sys.argv[1], sys.argv[2:])
