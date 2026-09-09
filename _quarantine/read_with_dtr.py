"""Leitura que FIXA DTR=True (obrigatorio p/ HWCDC transmitir) sem resetar.

Descoberta: no Arduino-ESP32 3.x, HWCDC::write() descarta TUDO quando o host
nao abriu a porta com DTR ativo (isCDC_Connected() pergunta ao driver se
DTR==1). Leituras 100% passivas (DTR=False) nunca recebem nada — nem ROM,
nem firmware. A ROM responde a DTR=False, mas o firmware precisa DTR=True.

IMPORTANTE para USB-Serial/JTAG: DTR controla IO0/boot e RTS controla EN.
Pulsar qualquer um pode gerar USB_UART_CHIP_RESET. Entao:
  1. abre a porta SEM tocar nas linhas,
  2. apos 2s lendo (pega ROM se o chip estiver em download),
  3. seta DTR=True de forma ATOMICA junto de RTS=False (IO0=HIGH + EN=HIGH,
     sem borda de reset — mesma ordem que o esptool usa no idle),
  4. le por mais 25s.

Se o firmware (probe) estiver rodando, os ticks aparecem apos o DTR=True.
Se aparecer 'waiting for download', o chip esta em download (strap BOOT).
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

p = serial.Serial(PORT, BAUD, timeout=0.5,
                  dsrdtr=False, rtscts=False, xonxoff=False)
try:
    p.setRTS(False)
except Exception:
    pass
print('=== fase 1: 3s com DTR=False (ROM fala, firmware cala) ===', flush=True)
end = time.time() + 3
buf = b''
n = 0


def drain(secs, tag):
    global buf, n
    end = time.time() + secs
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
                    print(f'[{time.strftime("%H:%M:%S")}][{tag}] '
                          + line.decode('utf-8', 'replace'), flush=True)
        else:
            time.sleep(0.05)


drain(3, 'noDTR')
print('=== fase 2: DTR=True (host abre CDC) + RTS=False, lendo 25s ===', flush=True)
try:
    p.setDTR(True)
except Exception as e:
    print(f'[ERRO DTR: {e}]', flush=True)
try:
    p.setRTS(False)
except Exception:
    pass
drain(25, 'DTR')
if buf.strip():
    print(buf.decode('utf-8', 'replace'))
p.close()
print(f'--- linhas capturadas: {n} ---', flush=True)
