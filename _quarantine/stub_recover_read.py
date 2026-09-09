"""Recuperacao de stub travado + reset limpo + leitura.

Situacao: esptool terminou (ou morreu) com o stub flasher RODANDO no chip.
Com o stub ativo, a COM3 responde ao protocolo SLIP do esptool, mas NAO
imprime boot da ROM nem Serial do firmware — leitura passiva da 0 linhas.

Estrategia:
  1. abre a COM3 (reopen com retry, pois o USB re-enumera apos resets),
  2. envia ESP_SYNC (0x07 0x07 0x12 0x20 + 32x 0x55) algumas vezes — se o
     stub responder com 0x07 0x07 0x12 0x20..., confirma stub travado,
  3. pulsa RTS (reset limpo, IO0=HIGH), fecha, espera re-enumerar,
  4. reabre e le por 25s (boot ROM + firmware, se houver).
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

ESP_SYNC = bytes([0x07, 0x07, 0x12, 0x20] + [0x55] * 32)


def open_port(tries=15):
    for i in range(tries):
        try:
            p = serial.Serial(PORT, BAUD, timeout=0.5,
                              dsrdtr=False, rtscts=False, xonxoff=False)
            return p
        except Exception as e:
            print(f'[abrir {i + 1}/{tries}: {e}]', flush=True)
            time.sleep(1.0)
    return None


p = open_port()
if p is None:
    print('[FALHA] COM3 nao abre.', flush=True)
    sys.exit(1)

# 1) sondar stub: ESP_SYNC espera eco de sincronismo
try:
    p.reset_input_buffer()
except Exception:
    pass
print('=== sondando stub flasher (ESP_SYNC x3) ===', flush=True)
stub = False
for i in range(3):
    try:
        p.write(ESP_SYNC)
        p.flush()
    except Exception as e:
        print(f'[write {i}: {e}]', flush=True)
        break
    time.sleep(0.3)
    try:
        r = p.read(128)
    except Exception as e:
        print(f'[read {i}: {e}]', flush=True)
        break
    if r:
        print(f'[sync {i}] {len(r)}B: {r[:64].hex()}', flush=True)
        if b'\x07\x07\x12\x20' in r:
            stub = True
    else:
        print(f'[sync {i}] sem resposta', flush=True)
print(f'=== stub travado: {stub} ===', flush=True)

# 2) reset limpo via RTS (IO0 continua HIGH => SPI boot)
try:
    p.setDTR(False)
except Exception:
    pass
try:
    p.setRTS(False)
except Exception:
    pass
time.sleep(0.2)
print('=== RTS pulse ===', flush=True)
try:
    p.setRTS(True)
except Exception as e:
    print(f'[RTS on: {e}]', flush=True)
time.sleep(0.3)
try:
    p.setRTS(False)
except Exception as e:
    print(f'[RTS off: {e}]', flush=True)
try:
    p.close()
except Exception:
    pass
print('=== aguardando re-enumeracao (4s) ===', flush=True)
time.sleep(4.0)

# 3) reabrir e ler
p = open_port()
if p is None:
    print('[FALHA] COM3 nao reabriu apos reset.', flush=True)
    sys.exit(1)
print('=== lendo por 25s ===', flush=True)
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
        print('[RESTO] ' + buf.decode('utf-8', 'replace'))
    try:
        p.close()
    except Exception:
        pass
print(f'--- linhas capturadas: {n} ---', flush=True)
