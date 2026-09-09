"""Captura o boot SEM fechar a porta (mesmo handle).

O reopen de 3s perdia a mensagem da ROM (impressa <1s apos o reset).
Aqui: abre, pulsa RTS, e LE no mesmo handle por 20s.
Primeiro sem reset (estado atual), depois com RTS pulse se pedido.
Uso: python read_same_handle.py [--reset]
"""
import sys
import time
sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
import serial

DO_RESET = '--reset' in sys.argv
p = serial.Serial('COM3', 115200, timeout=0.3,
                  dsrdtr=False, rtscts=False, xonxoff=False)
print(f'=== porta aberta (reset={DO_RESET}), lendo 20s no mesmo handle ===', flush=True)
if DO_RESET:
    try:
        p.setDTR(False)
    except Exception:
        pass
    try:
        p.setRTS(False)
    except Exception:
        pass
    time.sleep(0.5)
    try:
        p.reset_input_buffer()
    except Exception:
        pass
    print('--- RTS pulse agora ---', flush=True)
    try:
        p.setRTS(True)
    except Exception as e:
        print(f'[ERRO RTS on: {e}]', flush=True)
    time.sleep(0.3)
    try:
        p.setRTS(False)
    except Exception as e:
        print(f'[ERRO RTS off: {e}]', flush=True)
    print('--- reset solto, lendo ---', flush=True)

end = time.time() + 20
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
    print('[RESTO] ' + buf.decode('utf-8', 'replace'), flush=True)
p.close()
print(f'--- linhas: {n} ---', flush=True)
