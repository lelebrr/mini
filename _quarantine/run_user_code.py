"""Forca o ESP32-S3 a executar o firmware ja gravado na flash, sem depender
do strap de boot (GPIO0), usando o comando RUN_USER_CODE do stub flasher."""
import sys, time
sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')

import esptool
from esptool.cmds import detect_chip, run_stub
from esptool.loader import ESPLoader

PORT = 'COM3'
BAUD = 921600

esp = detect_chip(port=PORT, baud=BAUD, connect_mode='default-reset',
                  trace_enabled=False, connect_attempts=7)
print('Conectado:', esp.CHIP_NAME, flush=True)

esp = run_stub(esp)
print('Stub carregado, executando RUN_USER_CODE...', flush=True)

# Acesso ao comando RUN_USER_CODE (0xD3) do stub
esptool_obj = esp  # esp agora eh da classe *StubLoader
esptool_obj.command(esptool_obj.ESP_CMDS['RUN_USER_CODE'], wait_response=False)
print('RUN_USER_CODE enviado.', flush=True)

# Deixa a porta para que o firmware use o serial
time.sleep(1)
try:
    esptool_obj._port.close()
except Exception:
    pass
print('done.', flush=True)