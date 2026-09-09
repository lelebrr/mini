"""Tenta executar o app da flash via stub (FLASH_END com reboot)."""
import sys, time
sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')

from esptool.cmds import detect_chip, run_stub

esp = detect_chip(port='COM3', baud=921600, connect_mode='default-reset',
                  trace_enabled=False, connect_attempts=7)
print('Conectado:', esp.CHIP_NAME, flush=True)

esp = run_stub(esp)
print('Stub carregado. Enviando FLASH_END reboot=True...', flush=True)

esp.flash_begin(0, 0)          # entra em flash mode (fake)
esp.flash_finish(reboot=True)  # reboot -> executa o app da flash
print('FLASH_END enviado. Aguardando app...', flush=True)

time.sleep(2)
try:
    esp._port.close()
except Exception:
    pass
print('done.', flush=True)