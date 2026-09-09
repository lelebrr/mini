"""Leitura passiva COM DTR=True (host 'conectado' p/ o HWCDC).

Contexto: no HWCDC do Arduino-ESP32, isCDC_Connected() exige
  usb_serial_jtag_is_connected() (VBUS+sessao USB ok) && dtr (linha DTR
afirmada pelo host).
Com DTR=False o firmware ATE considera o host ausente: o loop de escrita
usa write sem timeout (descarta) e flush() limpa o buffer TX em vez de
enviar. Ou seja: com a porta aberta em dsrdtr=False/DTR=False, o [PROBE]
executa mas NADA chega na COM3 — silencio total, mesmo com o chip
rodando normalmente.

Este script abre a COM3 com dsrdtr=True e AFIRMA DTR (sem tocar RTS,
portanto sem resetar o chip), depois apenas LE por 25s. Se o probe
estiver rodando, os ticks devem aparecer.
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
                  dsrdtr=True, rtscts=False, xonxoff=False)
try:
    p.setDTR(True)   # host presente; NAO mexe no RTS (sem reset)
except Exception as e:
    print(f'[aviso DTR: {e}]', flush=True)
time.sleep(0.5)
try:
    p.reset_input_buffer()
except Exception:
    pass
print('=== leitura DTR=True por 25s (RTS intocado, sem reset) ===', flush=True)

end = time.time() + 25
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
