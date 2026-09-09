"""Leitor de console p/ USB-Serial/JTAG (COM3) com DTR ASSERTADO.

Descoberta do diagnostico: os scripts anteriores abriam a porta com
dsrdtr=False e NUNCA assertavam DTR. O HWCDC (Serial) do Arduino considera
o host "desconectado" sem DTR e DESCARTA os bytes de TX -> leitura sempre
zerada mesmo com o firmware rodando. Este script:

  1. abre a COM3 a 115200,
  2. setDTR(True)  -> diz ao chip que o terminal esta conectado,
  3. setRTS(False) -> fora de reset,
  4. apenas LE por 30s (modo --no-reset), ou
  5. com --reset: pulso RTS-only (reset limpo, IO0 continua HIGH => SPI boot),
     fecha, espera re-enumerar, reabre com DTR=True e le.

Uso:
  python probe_read.py --no-reset   (so escuta o que ja roda)
  python probe_read.py --reset      (reseta e escuta)
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
    p = serial.Serial(PORT, BAUD, timeout=0.5,
                      dsrdtr=False, rtscts=False, xonxoff=False)
    try:
        p.setDTR(True)    # terminal conectado -> HWCDC aceita TX
    except Exception as e:
        print(f'[aviso DTR: {e}]', flush=True)
    try:
        p.setRTS(False)   # fora de reset
    except Exception as e:
        print(f'[aviso RTS: {e}]', flush=True)
    time.sleep(0.3)
    try:
        p.reset_input_buffer()
    except Exception:
        pass
    return p


if DO_RESET:
    print('=== RTS pulse (reset limpo, IO0=HIGH => SPI boot) ===', flush=True)
    p = open_connected()
    try:
        p.setRTS(True)
    except Exception as e:
        print(f'[ERRO RTS on: {e}]', flush=True)
    time.sleep(0.3)
    try:
        p.setRTS(False)
    except Exception as e:
        print(f'[ERRO RTS off: {e}]', flush=True)
    try:
        p.close()
    except Exception:
        pass
    print('=== porta fechada, aguardando re-enumeracao USB (4s) ===', flush=True)
    time.sleep(4.0)

p = open_connected()
print('=== DTR=True, lendo console por 30s ===', flush=True)
end = time.time() + 30
buf = b''
n = 0
try:
    while time.time() < end:
        try:
            b = p.read(512)
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
        print(buf.decode('utf-8', 'replace'))
    try:
        p.close()
    except Exception:
        pass
print(f'--- linhas capturadas: {n} ---', flush=True)
