"""Dump bruto HEX+ASCII: abre COM3 com dtr=True (como pio monitor faz) e
mostra TUDO que chegar, byte a byte, por 30s.

Se chegar qualquer byte (mesmo binario/garbage), o chip esta transmitindo.
Se zero bytes, o chip esta mudo no USB (download sem comando ou travado).
Inclui modem status (CTS/DSR/RI/CD) para ver o estado das linhas.
"""
import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

p = serial.Serial('COM3', 115200, timeout=1.0)
time.sleep(0.5)
try:
    print(f'CD={p.cd} CTS={p.cts} DSR={p.dsr} RI={p.ri}', flush=True)
except Exception as e:
    print(f'[status indisponivel: {e}]', flush=True)
try:
    waiting = p.in_waiting
    print(f'in_waiting inicial: {waiting}', flush=True)
except Exception as e:
    print(f'[in_waiting erro: {e}]', flush=True)

print('=== dump bruto 30s ===', flush=True)
end = time.time() + 30
total = 0
while time.time() < end:
    try:
        b = p.read(256)
    except Exception as e:
        print(f'[ERRO: {e}]', flush=True)
        break
    if b:
        total += len(b)
        print(f'[{time.strftime(\"%H:%M:%S\")}] +{len(b)}B total={total}:', flush=True)
        for i in range(0, len(b), 16):
            chunk = b[i:i + 16]
            hexs = ' '.join(f'{x:02x}' for x in chunk)
            asc = ''.join(chr(x) if 32 <= x < 127 else '.' for x in chunk)
            print(f'  {hexs:<48} {asc}', flush=True)
    else:
        print(f'[{time.strftime(\"%H:%M:%S\")}] (1s sem dados)', flush=True)
p.close()
print(f'--- total bytes: {total} ---', flush=True)
