"""Leitura conectada: abre a COM3, declara DTR=True (host presente) e LE.

Contexto: o firmware (probe ou principal) usa Serial sobre USB-Serial/JTAG
(HWCDC, ARDUINO_USB_MODE=1). O HWCDC so entrega bytes ao host quando o host
abre a porta com DTR ativo; scripts que nunca ativam DTR veem ZERO linhas
mesmo com o firmware rodando. Setar DTR=True sozinho NAO reseta o chip
(reset exige pulso de RTS) — apenas sinaliza "host conectado".

Uso:
  python read_connected.py            -> so le (firmware ja rodando?)
  python read_connected.py --reset    -> pulsa so RTS (boot SPI), reabre,
                                         declara DTR e le
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


def open_port():
    return serial.Serial(PORT, BAUD, timeout=0.5,
                         dsrdtr=False, rtscts=False, xonxoff=False)


if DO_RESET:
    p = open_port()
    try:
        p.setDTR(False)   # IO0 = HIGH (boot SPI)
        p.setRTS(False)   # fora de reset
    except Exception:
        pass
    time.sleep(0.3)
    try:
        p.reset_input_buffer()
    except Exception:
        pass
    print('=== RTS pulse (reset limpo, IO0=HIGH) ===', flush=True)
    p.setRTS(True)
    time.sleep(0.3)
    p.setRTS(False)
    try:
        p.close()
    except Exception:
        pass
    print('=== aguardando re-enumeracao USB (4s) ===', flush=True)
    time.sleep(4.0)

p = None
for i in range(15):
    try:
        p = open_port()
        break
    except Exception as e:
        print(f'[reopen {i + 1}/15: {e}]', flush=True)
        time.sleep(1.0)
if p is None:
    print('[FALHA] COM3 nao abriu.', flush=True)
    sys.exit(1)

# Declara host presente. So DTR, sem RTS -> sem reset.
try:
    p.setDTR(True)
except Exception as e:
    print(f'[aviso DTR: {e}]', flush=True)
time.sleep(0.5)
try:
    p.reset_input_buffer()
except Exception:
    pass

print('=== lendo por 25s (DTR=True, sem tocar RTS) ===', flush=True)
end = time.time() + 25
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
