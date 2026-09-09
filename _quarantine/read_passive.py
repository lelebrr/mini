"""Le passivamente o serial, sem tocar em DTR/RTS e sem emitir reset."""
import serial, time

p = serial.Serial('COM3', 115200, timeout=0.3)
time.sleep(0.1)

# Nao mexe nas linhas de controle apos abrir
end = time.time() + 16
buf = b''
n = 0
print('=== leitura passiva ==', flush=True)
while time.time() < end:
    b = p.read(512)
    if b:
        buf += b
        while b'\n' in buf:
            line, _, buf = buf.partition(b'\n')
            line = line.strip(b'\r').strip()
            if line:
                n += 1
                print('[' + time.strftime('%H:%M:%S') + '] ' + line.decode('utf-8', 'replace'), flush=True)
    else:
        time.sleep(0.05)
if buf.strip():
    print(buf.decode('utf-8', 'replace'))
p.close()
print(f'--- linhas: {n} ---')