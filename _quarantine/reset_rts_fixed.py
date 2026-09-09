"""Reset via RTS com workaround usbser.sys (igual ao esptool) + leitura.

Correcao em relacao ao reset_rtsonly.py: no Windows o driver usbser.sys
IGNORA mudanca de RTS sozinha num SET_CONTROL_LINE_STATE se o DTR nao
mudar junto. O esptool contorna isso em ResetStrategy._setRTS():
    port.setRTS(state); port.setDTR(port.dtr)
ou seja, re-afirma o DTR atual para forcar o pacote USB com o novo RTS.

Sem isso, nosso 'RTS-only' anterior provavelmente NUNCA resetou o chip
de verdade (0 linhas em vez do esperado SPI boot).

Sequencia aqui (IO0=HIGH o tempo todo, DTR=False):
    setRTS(True)  + setDTR(False)   -> EN LOW (chip em reset)
    sleep 0.3
    setRTS(False) + setDTR(False)   -> EN HIGH (solta reset, SPI boot)
Depois: fecha, espera re-enumerar, reabre com DTR=True e le por 25s.
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
time.sleep(0.2)
try:
    p.reset_input_buffer()
except Exception:
    pass


def set_rts(state):
    p.setRTS(state)
    p.setDTR(p.dtr)  # workaround usbser.sys (igual esptool._setRTS)


p.setDTR(False)
set_rts(False)
time.sleep(0.2)
try:
    p.reset_input_buffer()
except Exception:
    pass

print('=== RTS pulse (com workaround usbser) ===', flush=True)
set_rts(True)
time.sleep(0.3)
set_rts(False)
p.close()
print('=== porta fechada, aguardando re-enumeracao (4s) ===', flush=True)
time.sleep(4.0)

p = None
for i in range(15):
    try:
        p = serial.Serial(PORT, BAUD, timeout=0.5,
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
print('=== lendo por 25s ===', flush=True)
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
