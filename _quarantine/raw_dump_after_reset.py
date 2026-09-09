"""Dump bruto da COM3 logo apos reset: mostra TUDO que chega (ROM incluida).

Diferente do reset_reopen_read (que so imprime linhas com \n), este mostra
bytes crus em hex+ascii dos primeiros segundos apos o reset — pega inclusive
fragmentos da ROM que chegam antes da re-enumeracao USB e mensagens sem
quebra de linha. Se aparecer qualquer byte, o chip esta vivo apos o reset.
Se ficar 100% zerado, o reset via RTS nao esta sequer reiniciando o chip
(ou o USB nao re-enumera).
"""
import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

with serial.Serial('COM3', 115200, timeout=0.3,
                   dsrdtr=False, rtscts=False, xonxoff=False) as p:
    try:
        p.setDTR(False)
    except Exception:
        pass
    try:
        p.setRTS(False)
    except Exception:
        pass
    time.sleep(0.3)
    try:
        p.reset_input_buffer()
    except Exception:
        pass
    print('=== RTS pulse ===', flush=True)
    p.setRTS(True)
    time.sleep(0.3)
    p.setRTS(False)
    p.close()

time.sleep(3.0)
print('=== reabrindo e capturando bytes crus por 20s ===', flush=True)
total = 0
try:
    p = serial.Serial('COM3', 115200, timeout=0.5,
                      dsrdtr=False, rtscts=False, xonxoff=False)
except Exception as e:
    print(f'[FALHA reopen: {e}]', flush=True)
    sys.exit(1)
end = time.time() + 20
try:
    while time.time() < end:
        try:
            b = p.read(256)
        except Exception as e:
            print(f'[ERRO: {e}]', flush=True)
            break
        if b:
            total += len(b)
            for i in range(0, len(b), 16):
                chunk = b[i:i + 16]
                hexs = ' '.join(f'{c:02x}' for c in chunk)
                asc = ''.join(chr(c) if 32 <= c < 127 else '.' for c in chunk)
                print(f'{total - len(b) + i:06d}  {hexs:<47}  {asc}', flush=True)
        else:
            time.sleep(0.05)
finally:
    p.close()
print(f'--- total bytes: {total} ---', flush=True)
