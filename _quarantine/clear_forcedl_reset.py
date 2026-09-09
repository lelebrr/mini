"""Limpa RTC_CNTL_OPTION1.FORCE_DOWNLOAD_BOOT e faz hard reset via RTS.

Contexto: existe um workaround conhecido (arduino-esp32 #6762) em que o
stub do esptool seta o bit RTC_CNTL_OPTION1.FORCE_DOWNLOAD_BOOT durante a
gravacao; o hard_reset() do target ESP32-S3 LIMPA esse bit via write_reg
ANTES de pulsar RTS. O upload do PlatformIO terminou com "Hard resetting
via RTS pin..." — precisamos confirmar qual caminho ele usou e, em caso
de duvida, repetir aqui: limpar o bit e pulsar RTS-only (uses_usb=False,
chip na COM3 = USB-Serial/JTAG mas sem USB-OTG => ESPLoader.hard_reset
puro, so RTS, sem sleep extra).

Depois do reset: fecha, espera re-enumerar, reabre e LE por 25s com
DTR=True (host presente p/ o HWCDC) sem tocar RTS.
"""
import sys
import time

sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
try:
    import serial
except ImportError:
    import serial  # noqa

from esptool.cmds import detect_chip, run_stub

PORT = 'COM3'
BAUD = 921600

print('=== conectando (default-reset) p/ limpar FORCE_DOWNLOAD_BOOT ===', flush=True)
esp = detect_chip(port=PORT, baud=BAUD, connect_mode='default-reset',
                  trace_enabled=False, connect_attempts=7)
print(f'Conectado: {esp.CHIP_NAME}', flush=True)
esp = run_stub(esp)
print('Stub rodando.', flush=True)

REG = esp.RTC_CNTL_OPTION1_REG
MASK = esp.RTC_CNTL_FORCE_DOWNLOAD_BOOT_MASK
try:
    before = esp.read_reg(REG)
    print(f'RTC_CNTL_OPTION1 antes = 0x{before:08x} (FORCE_DL bit={bool(before & MASK)})', flush=True)
except Exception as e:
    print(f'[aviso] read_reg falhou: {e}', flush=True)
try:
    esp.write_reg(REG, 0, MASK)
    after = esp.read_reg(REG)
    print(f'RTC_CNTL_OPTION1 depois = 0x{after:08x} (FORCE_DL bit={bool(after & MASK)})', flush=True)
except Exception as e:
    print(f'[aviso] write_reg falhou: {e}', flush=True)

print('=== hard_reset() do target (limpa bit + RTS) ===', flush=True)
try:
    esp.hard_reset()
except Exception as e:
    print(f'[aviso] hard_reset: {e}', flush=True)
try:
    esp._port.close()
except Exception:
    pass

print('=== aguardando re-enumeracao USB (4s) ===', flush=True)
time.sleep(4.0)

p = None
for i in range(15):
    try:
        p = serial.Serial(PORT, 115200, timeout=0.5,
                          dsrdtr=True, rtscts=False, xonxoff=False)
        break
    except Exception as e:
        print(f'[reopen {i + 1}/15: {e}]', flush=True)
        time.sleep(1.0)
if p is None:
    print('[FALHA] COM3 nao reabriu.', flush=True)
    sys.exit(1)
try:
    p.setDTR(True)
except Exception:
    pass
time.sleep(0.5)
try:
    p.reset_input_buffer()
except Exception:
    pass
print('=== lendo por 25s (DTR=True, RTS intocado) ===', flush=True)
end = time.time() + 25
buf = b''
n = 0
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
                print(f'[{time.strftime("%H:%M:%S")}] ' + line.decode('utf-8', 'replace'), flush=True)
    else:
        time.sleep(0.05)
if buf.strip():
    print(buf.decode('utf-8', 'replace'))
p.close()
print(f'--- linhas: {n} ---', flush=True)
