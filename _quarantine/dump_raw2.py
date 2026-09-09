import sys
import time

try:
    import serial
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    import serial

p = serial.Serial('COM3', 115200, timeout=1.0)
time.sleep(0.5)
print('CD=' + str(p.cd) + ' CTS=' + str(p.cts) + ' DSR=' + str(p.dsr) + ' RI=' + str(p.ri), flush=True)
print('in_waiting inicial: ' + str(p.in_waiting), flush=True)

print('=== dump bruto 30s ===', flush=True)
end = time.time() + 30
total = 0
while time.time() < end:
    b = p.read(256)
    if b:
        total += len(b)
        stamp = time.strftime('%H:%M:%S')
        print('[' + stamp + '] +' + str(len(b)) + 'B total=' + str(total) + ':', flush=True)
        for i in range(0, len(b), 16):
            chunk = b[i:i + 16]
            hexs = ' '.join('%02x' % x for x in chunk)
            asc = ''.join(chr(x) if 32 <= x < 127 else '.' for x in chunk)
            print('  ' + hexs.ljust(48) + ' ' + asc, flush=True)
    else:
        stamp = time.strftime('%H:%M:%S')
        print('[' + stamp + '] (1s sem dados)', flush=True)
p.close()
print('--- total bytes: ' + str(total) + ' ---', flush=True)
