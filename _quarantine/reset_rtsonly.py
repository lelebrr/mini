"""Reset RTS-ONLY (sem tocar DTR em nenhum momento) + leitura do boot.

Descoberta: no USB-Serial/JTAG nativo do ESP32-S3, a sequencia classica
(DTR=False/RTS=True -> DTR=True/RTS=False) faz o chip acordar em
boot:0x23 DOWNLOAD. O motivo: o periferico USB interpreta DTR como IO0;
qualquer transicao de DTR na janela de amostragem do strap seleciona o
modo download.

O HardReset(uses_usb=True) do esptool faz exatamente isto: pulsa SOMENTE
RTS (EN LOW->HIGH), deixando IO0 (pull-up interno 45k) em HIGH, de modo
que o chip boota da flash (SPI_FAST_FLASH_BOOT).

IMPORTANTE: nao chamar setDTR() NUNCA neste script — nem no inicio nem
no fim. So RTS.
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

# dsrdtr=False: abrir a porta NAO afirma DTR
p = serial.Serial(PORT, BAUD, timeout=0.3, dsrdtr=False, rtscts=False, xonxoff=False)
time.sleep(0.2)
try:
    p.reset_input_buffer()
except Exception:
    pass

# Estado inicial SEM tocar DTR: apenas garante RTS=False (fora de reset).
# Nota: setRTS no Windows (usbser.sys) reenvia tambem o estado atual de DTR,
# por isso abrimos com dsrdtr=False para DTR ja nascer False (IO0=HIGH).
p.setRTS(False)
time.sleep(0.2)
try:
    p.reset_input_buffer()
except Exception:
    pass

# --- Hard reset RTS-only: EN LOW -> HIGH, DTR intocado ---
p.setRTS(True)
time.sleep(0.25)
p.setRTS(False)
time.sleep(0.5)
print('=== RESET RTS-only aplicado, lendo boot por 15s ===', flush=True)

end = time.time() + 15
buf = b''
n = 0
while time.time() < end:
    b = p.read(256)
    if b:
        buf += b
        while b'\n' in buf:
            line, _, buf = buf.partition(b'\n')
            if line.strip():
                n += 1
                print(line.decode('utf-8', 'replace'))
    else:
        time.sleep(0.05)
if buf.strip():
    print(buf.decode('utf-8', 'replace'))
p.close()
print(f'--- linhas capturadas: {n} ---')
