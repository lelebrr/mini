"""USBJTAGSerialReset canonico do esptool + reopen + leitura.

Descoberta: no esptool 5.x, PID 1001 (USB-Serial/JTAG) usa USBJTAGSerialReset
para ENTRAR no download:
    DTR=False,RTS=False / DTR=True / RTS=True(*) / DTR=False,RTS=True
    / DTR=False,RTS=False (sai do reset)
  (*) _setRTS no Windows tambem reenvia DTR (workaround usbser.sys).

O '--after hard-reset' do esptool usa HardReset (RTS-only). Mas o nosso
reset manual RTS-only nao gerou boot visivel. Este script replica
EXATAMENTE o USBJTAGSerialReset (incluindo o workaround Windows), depois
fecha, espera re-enumerar, reabre e le por 25s.

Na saida do reset ele deixa DTR=False/RTS=False (IO0 HIGH, fora de reset),
que e SPI boot. Se aparecer 'boot:0x2B ... SPI_FAST_FLASH_BOOT' + logs do
firmware, vencemos. Se aparecer 'boot:0x23 ... DOWNLOAD', a sequencia entrou
em download (esperado so DURANTE a sequencia).
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


def set_rts(p, state):
    # replica ResetStrategy._setRTS no Windows (com workaround usbser.sys)
    p.setRTS(state)
    try:
        p.setDTR(p.dtr)
    except Exception:
        pass


def open_port(tries=15):
    for i in range(tries):
        try:
            return serial.Serial(PORT, BAUD, timeout=0.5,
                                 dsrdtr=False, rtscts=False, xonxoff=False)
        except Exception as e:
            print(f'[abrir {i + 1}/{tries}: {e}]', flush=True)
            time.sleep(1.0)
    return None


p = open_port()
if p is None:
    print('[FALHA] COM3 nao abre.', flush=True)
    sys.exit(1)

try:
    p.reset_input_buffer()
except Exception:
    pass

print('=== USBJTAGSerialReset canonico ===', flush=True)
p.setDTR(False)
set_rts(p, False)   # Idle
time.sleep(0.1)
p.setDTR(True)      # Set IO0
set_rts(p, False)
time.sleep(0.1)
set_rts(p, True)    # Reset (via (1,1))
p.setDTR(False)
set_rts(p, True)    # RTS set (Windows propaga DTR)
time.sleep(0.1)
p.setDTR(False)
set_rts(p, False)   # Chip out of reset
print('=== fora de reset; fechando p/ re-enumerar ===', flush=True)
try:
    p.close()
except Exception:
    pass
time.sleep(4.0)

p = open_port()
if p is None:
    print('[FALHA] COM3 nao reabriu.', flush=True)
    sys.exit(1)
print('=== lendo por 25s ===', flush=True)
end = time.time() + 25
buf = b''
n = 0
try:
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
                    print(f'[{time.strftime("%H:%M:%S")}] '
                          + line.decode('utf-8', 'replace'), flush=True)
        else:
            time.sleep(0.05)
finally:
    if buf.strip():
        print('[RESTO] ' + buf.decode('utf-8', 'replace'))
    try:
        p.close()
    except Exception:
        pass
print(f'--- linhas capturadas: {n} ---', flush=True)
