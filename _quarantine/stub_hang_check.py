"""Leitura do bootloader ROM ESP32-S3 usando o stub (como o esptool faz).

So funciona se o chip estiver em DOWNLOAD (IO0=LOW). Se estiver booteando
da flash (IO0=HIGH), o stub nao roda e vemos a assinatura do firmware normal
(contudo, o firmware precisa estar escutando na USB/JTAG).

Uso: python stub_hang_check.py [--reset]
  --reset : pulSa RTS limpo (IO0=HIGH) -> chip tenta bootar pela flash.
            Se o firmware nao escuta na COM3, so 'waiting for download' ---
            porem o stub nao pode ser deployado nele (ROM boot nao responde
            a upload de stub fora de download).
"""
import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

PORT = 'COM3'
BAUD = 921600
DO_RESET = '--reset' in sys.argv

p = serial.Serial(PORT, BAUD, timeout=0.4,
                  dsrdtr=False, rtscts=False, xonxoff=False)
try:
    p.setDTR(True)
except Exception as e:
    print(f'[aviso DTR: {e}]', flush=True)
try:
    p.setRTS(False)
except Exception as e:
    print(f'[aviso RTS: {e}]', flush=True)
time.sleep(0.3)
try:
    p.reset_input_buffer()
except Exception:
    pass

if DO_RESET:
    print('=== pulso RTS (reset limpo) ===', flush=True)
    try:
        p.setRTS(True)
    except Exception as e:
        print(f'[ERRO RTS on: {e}]', flush=True)
    time.sleep(0.3)
    try:
        p.setRTS(False)
    except Exception as e:
        print(f'[ERRO RTS off: {e}]', flush=True)
    time.sleep(2)

print('=== lendo por 8s buscando sinais do bootloader/firmware ===', flush=True)
end = time.time() + 8
buf = b''
n = 0
while time.time() < end:
    try:
        b = p.read(1024)
    except Exception as e:
        print(f'[erro leitura: {e}]', flush=True)
        break
    if b:
        buf += b
        tmp = b
        while b'\n' in tmp:
            line, _, tmp = tmp.partition(b'\n')
            line = line.strip(b'\r').strip()
            if line:
                n += 1
                print(f'[{time.strftime("%H:%M:%S")}] ' + line.decode('utf-8','replace'), flush=True)
    else:
        time.sleep(0.05)
if buf.strip():
    print(f'\n--- resto bruto ({len(buf)}B) ---', flush=True)
    print(buf.decode('utf-8', 'replace'))
try:
    p.close()
except Exception:
    pass
print(f'--- linhas: {n} ---', flush=True)
