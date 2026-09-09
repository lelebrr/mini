"""Leitura TOTALMENTE passiva: abre a COM3 sem tocar DTR/RTS em NENHUM momento.

Motivo: no USB-Serial/JTAG nativo do ESP32-S3, qualquer transicao de DTR/RTS
(mesmo a abertura da porta pelo driver Windows) pode gerar
USB_UART_CHIP_RESET e recolocar o chip em DOWNLOAD ou reseta-lo no meio do
boot do firmware. Este script:
  1. abre a porta com dsrdtr=False / rtscts=False (nao toca nas linhas),
  2. NUNCA chama setDTR/setRTS,
  3. apenas LE por 30s.

Se o firmware estiver rodando (loop com Serial.println ou LVGL), veremos
as linhas aqui. Se ficar mudo, o firmware nao esta emitindo no HWCDC
(ARDUINO_USB_MODE=1 => Serial vai para USB-Serial/JTAG, mesma COM3)
ou o chip segue preso em download.
"""
import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

p = serial.Serial('COM3', 115200, timeout=0.5,
                  dsrdtr=False, rtscts=False, xonxoff=False)
print('=== leitura 100% passiva por 30s (DTR/RTS intocados) ===', flush=True)
end = time.time() + 30
buf = b''
n = 0
while time.time() < end:
    try:
        b = p.read(512)
    except Exception as e:
        print(f'[ERRO: {e}]', flush=True)
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
