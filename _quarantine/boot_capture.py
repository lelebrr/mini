"""Captura o boot com DTR/RTS corretos para USB-Serial/JTAG nativo.

Descoberta: abrir a porta com DTR=False faz o HWCDC (Serial do app)
considerar o CDC "desconectado" e DESCARTAR todos os Serial.print().
A ROM ignora DTR, mas imprime <1s apos o reset — reopen lento perde.

Sequencia:
  1. abre, DTR=False, RTS=False (idle limpo, IO0=HIGH)
  2. pulso RTS 300ms (reset limpo, sem tocar DTR => boot SPI)
  3. fecha, reabre em loop rapido (200ms) por ate 6s (re-enumeracao USB)
  4. ao reabrir: DTR=True (marca CDC conectado p/ o app) e LE por 25s
"""
import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

PORT = 'COM3'
BAUD = 115200

p = serial.Serial(PORT, BAUD, timeout=0.3,
                  dsrdtr=False, rtscts=False, xonxoff=False)
try:
    p.setDTR(False)
except Exception:
    pass
try:
    p.setRTS(False)
except Exception:
    pass
time.sleep(0.4)
try:
    p.reset_input_buffer()
except Exception:
    pass

print('=== RTS pulse (reset limpo, IO0=HIGH => SPI boot) ===', flush=True)
p.setRTS(True)
time.sleep(0.3)
p.setRTS(False)
try:
    p.close()
except Exception:
    pass
print('=== porta fechada, reabrindo rapido p/ pegar a ROM ===', flush=True)

p2 = None
t0 = time.time()
while time.time() - t0 < 8:
    try:
        p2 = serial.Serial(PORT, BAUD, timeout=0.4,
                           dsrdtr=False, rtscts=False, xonxoff=False)
        break
    except Exception:
        time.sleep(0.2)
if p2 is None:
    print('[FALHA] COM3 nao reabriu.', flush=True)
    sys.exit(1)

# Marca CDC como conectado para o firmware (Serial) nao descartar os logs.
# Feito APOS o boot, sem nenhum reset junto => nao afeta o strap.
try:
    p2.setDTR(True)
except Exception as e:
    print(f'[aviso DTR: {e}]', flush=True)
try:
    p2.setRTS(False)
except Exception:
    pass

print('=== lendo por 25s ===', flush=True)
end = time.time() + 25
buf = b''
n = 0
try:
    while time.time() < end:
        try:
            b = p2.read(512)
        except Exception as e:
            print(f'[ERRO leitura: {e}]', flush=True)
            break
        if b:
            buf += b
            while b'\n' in buf:
                line, _, buf = buf.partition(b'\n')
                line = line.strip(b'\r').strip()
                if line:
                    n += 1
                    print(f'[{time.strftime("%H:%M:%S")}] '
                          + line.decode('utf-8', 'replace'), flush=True)
        else:
            time.sleep(0.05)
finally:
    if buf.strip():
        print('[RESTO] ' + buf.decode('utf-8', 'replace'))
    try:
        p2.close()
    except Exception:
        pass
print(f'--- linhas capturadas: {n} ---', flush=True)
