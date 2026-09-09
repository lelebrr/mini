"""Reset limpo (RTS-only) + leitura CONSECUTIVA com DTR=True SEM fechar/reabrir.

Diferente do probe_read.py que fecha a porta e reabre, este script:
  1. abre a COM3,
  2. setDTR(True) + setRTS(False),
  3. le por ~4s SEM interromper (para capturar a assinatura do bootloader ROM
     'waiting for download' ou o primer do servo de comunicação),
  4. deixa o fluxo rodando e avisa se continua recebendo ou se trava (típico
     de DTR=True causar mudança de dispositivo no Windows).
Uso: python read_continuous.py [--reset]
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
DO_RESET = '--reset' in sys.argv


def open_connected():
    p = serial.Serial(PORT, BAUD, timeout=0.15,
                      dsrdtr=False, rtscts=False, xonxoff=False)
    try:
        p.setDTR(True)
    except Exception as e:
        print(f'[aviso DTR: {e}]', flush=True)
    try:
        p.setRTS(False)
    except Exception as e:
        print(f'[aviso RTS: {e}]', flush=True)
    time.sleep(0.2)
    try:
        p.reset_input_buffer()
    except Exception:
        pass
    return p


if DO_RESET:
    p = open_connected()
    try:
        p.setRTS(True)
    except Exception as e:
        print(f'[erro RTS on: {e}]', flush=True)
    time.sleep(0.3)
    try:
        p.setRTS(False)
    except Exception as e:
        print(f'[erro RTS off: {e}]', flush=True)
    try:
        p.close()
    except Exception:
        pass
    print('=== reset pulso RTS; aguardando 2s + reabrir ===', flush=True)
    time.sleep(2.0)

p = open_connected()
print('=== leitura consecutiva (DTR=True, sem fecha/reabre) por 12s ===', flush=True)
buf = bytearray()
end = time.time() + 12
n = 0
last = 0.0
while time.time() < end:
    try:
        b = p.read(256)
    except Exception as e:
        print(f'[erro leitura: {e}]', flush=True)
        break
    if b:
        buf.extend(b)
        tmp = b
        while b'\n' in tmp:
            line, _, tmp = tmp.partition(b'\n')
            line = line.strip(b'\r').strip()
            if line:
                n += 1
                last = time.time()
                print(f'[{time.strftime("%H:%M:%S")} t={int(time.time()-last+0.5)}s] '
                      + line.decode('utf-8', 'replace'), flush=True)
    else:
        time.sleep(0.05)

if buf.strip():
    print(f'\n=== resto bruto ({len(buf)} bytes) ===', flush=True)
    print(buf.decode('utf-8', 'replace'))
try:
    p.close()
except Exception:
    pass
print(f'--- linhas: {n}, ultima a {time.time()-last:.1f}s antes do fim ---', flush=True)
