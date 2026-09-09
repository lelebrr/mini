"""Reseta a esp32-s3 via USB-Serial/JTAG (sequencia igual a do esptool) e le o boot."""
import serial, time, sys

PORT = 'COM3'
BAUD = 115200

p = serial.Serial(PORT, BAUD, timeout=0.3)
p.reset_input_buffer()
time.sleep(0.2)

# --- Garante estado idle: IO0=HIGH (DTR off), EN=HIGH (RTS off) ---
p.setDTR(False)
p.setRTS(False)
time.sleep(0.1)

# --- Hard reset: EN=LOW (RTS on) ... EN=HIGH (RTS off), com IO0 sempre HIGH ---
p.setRTS(True)
p.setDTR(False)   # forcaria propagacao no usbser (windows)
time.sleep(0.25)
p.setRTS(False)
p.setDTR(False)
time.sleep(0.5)
print('=== RESET aplicado, lendo boot por 15s ===', flush=True)

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
p.setDTR(False)
p.setRTS(False)
p.close()
print(f'--- linhas capturadas: {n} ---')