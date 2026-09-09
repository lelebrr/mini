"""Replica EXATA da sequencia USBJTAGSerialReset do esptool + leitura.

Sequencia (reset.py, linhas 151-163):
    RTS=False, DTR=False; sleep 0.1
    DTR=True; RTS=False; sleep 0.1
    RTS=True (via _setRTS, que tambem reenvia DTR atual=True); DTR=False;
    RTS=True; sleep 0.1
    DTR=False; RTS=False  (chip sai do reset -> DOWNLOAD)

Nota: _setRTS no esptool faz port.setRTS(state) + port.setDTR(port.dtr),
ou seja, cada _setRTS re-afirma o DTR atual (workaround usbser.sys).
Replicamos isso com dtr_state acompanhado manualmente.
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
p.reset_input_buffer()


def set_rts(state, dtr_state):
    p.setRTS(state)
    p.setDTR(dtr_state)  # workaround usbser.sys do esptool (_setRTS)


def set_dtr(state):
    p.setDTR(state)


# --- USBJTAGSerialReset.reset() exato ---
set_rts(False, False)
set_dtr(False)   # Idle
time.sleep(0.1)
set_dtr(True)    # Set IO0
set_rts(False, True)
time.sleep(0.1)
set_rts(True, True)   # Reset. Calls inverted to go through (1,1) instead of (0,0)
set_dtr(False)
set_rts(True, False)  # RTS set as Windows only propagates DTR on RTS setting
time.sleep(0.1)
set_dtr(False)
set_rts(False, False)  # Chip out of reset
print('=== USBJTAGSerialReset aplicado, lendo boot por 15s ===', flush=True)

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
