"""Reseta via RTS APENAS (sequencia HardReset uses_usb do esptool) e le o boot.

Para USB-Serial/JTAG (COM3, VID 303A PID 1001) NAO existe circuito externo
DTR/RTS->EN/IO0. O proprio periferico USB interpreta as linhas CDC:
  - mexer em DTR (=IO0) + RTS (=EN) como o ClassicReset faz => entra em DOWNLOAD
  - pulsar SOMENTE RTS (HardReset uses_usb=True) => reset limpo, boota pela
    strap real (GPIO0 HIGH => SPI_FAST_FLASH_BOOT, executa o firmware)

Este script replica exatamente HardReset(uses_usb=True) e depois so LE,
sem tocar nas linhas de novo. Se o strap fisico estiver OK (botao BOOT
solto), o firmware deve bootar e imprimir os logs na COM3.
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

p = serial.Serial(PORT, BAUD, timeout=0.3, dsrdtr=False, rtscts=False, xonxoff=False)
# Garante idle SEM pulsar download: DTR=False (IO0 HIGH), RTS=False (fora de reset)
try:
    p.setDTR(False)
except Exception:
    pass
try:
    p.setRTS(False)
except Exception:
    pass
time.sleep(0.3)
p.reset_input_buffer()

# --- HardReset uses_usb=True: so RTS ---
print('=== HardReset (RTS-only, uses_usb) ===', flush=True)
p.setRTS(True)    # EN -> LOW (chip em reset)
time.sleep(0.25)
p.setRTS(False)   # EN -> HIGH (solta reset, IO0 continua HIGH => SPI boot)
time.sleep(0.4)
print('=== reset solto, lendo boot por 25s (sem tocar DTR/RTS) ===', flush=True)

end = time.time() + 25
buf = b''
n = 0
while time.time() < end:
    try:
        b = p.read(512)
    except Exception as e:
        print(f'[ERRO LEITURA: {e}]', flush=True)
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
try:
    p.setDTR(False)
    p.setRTS(False)
except Exception:
    pass
p.close()
print(f'--- linhas capturadas: {n} ---', flush=True)
