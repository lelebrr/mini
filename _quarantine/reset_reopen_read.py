"""Reset limpo (RTS-only, estilo HardReset uses_usb) + LEITURA com reopen.

Por que o reopen: no USB-Serial/JTAG nativo, pulsar RTS reseta o chip via
USB_UART_CHIP_RESET e o dispositivo USB re-enumera — o handle serial antigo
fica obsoleto e a mensagem da ROM (impressa logo no boot) se perde.
Entao: pulsa RTS, FECHA a porta, espera re-enumerar, REABRE e le.

Uso:
  1) so ler o que ja esta rodando:  python reset_reopen_read.py --no-reset
  2) resetar e ler (padrao):         python reset_reopen_read.py
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
DO_RESET = '--no-reset' not in sys.argv


def open_port():
    return serial.Serial(PORT, BAUD, timeout=0.5,
                         dsrdtr=False, rtscts=False, xonxoff=False)


def wait_reopen(tries=15):
    for i in range(tries):
        try:
            p = open_port()
            return p
        except Exception as e:
            print(f'[reopen {i + 1}/{tries}: {e}]', flush=True)
            time.sleep(1.0)
    return None


if DO_RESET:
    try:
        p = open_port()
    except Exception as e:
        print(f'[ERRO abrir porta p/ reset: {e}]', flush=True)
        p = None
    if p is not None:
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
        print('=== RTS pulse (reset limpo, IO0=HIGH) ===', flush=True)
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
        print('=== porta fechada, aguardando re-enumeracao USB ===', flush=True)
        time.sleep(3.0)

p = wait_reopen()
if p is None:
    print('[FALHA] COM3 nao reabriu apos reset.', flush=True)
    sys.exit(1)

print('=== porta reaberta, lendo por 25s ===', flush=True)
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
