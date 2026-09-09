# Tentativa de upload do firmware da sonda local (302 KB, SPI boot) via esptool.
#
# IDEIA: o esptool aceita --before no-reset + manual reset do RTS no momento
# correto. Se o chip esta em DOWNLOAD (IO0=LOW) e o stubesptool na flash/OTA
# for aceito, ele carrega o stub e pode escrever a flash. MAS se IO0==HIGH,
# o chip nao entra no download mode e o esptool recebe 'waiting for download'
# e falha (timeout de handshake do stub).
#
# Em vez de reinventar o upload, usamos o esptool "write_flash" + "hard_reset"
# do proprio esptool na placa ESP32-S3 (COM3), apontando para o binario da sonda
# daqui (firmware_local.bin).
#
# O foco eh verificar se o esptool consegue CONECTAR (detectar chip) e se o
# chip entra em download antes do timeout do handshake do stub.
#
# Uso:
#   python upload_probe.py   (tenta conectar + flash + reset)

import sys
import time

# pylint: disable=import-error
try:
    from esptool import main as esptool_main
except ImportError:
    sys.path.insert(0, r'F:\PIO\penv\Lib\site-packages')
    from esptool import main as esptool_main

PORT = 'COM3'
CHIP = 'esp32s3'
BAUD = 921600
FLASH_ADDR = 0x10000
FIRMWARE = r'f:\mini\_quarantine\firmware_local.bin'

if __name__ == '__main__':
    args = [
        '--chip', CHIP,
        '--port', PORT,
        '--baud', str(BAUD),
        '--before', 'no-reset',
        '--after', 'hard_reset',
        'write_flash', '0x10000', FIRMWARE,
    ]
    try:
        rc = esptool_main(args)
        print(f'--- esptool rc={rc} ---', flush=True)
    except SystemExit as e:
        print(f'--- esptool SystemExit: {e} ---', flush=True)
    except Exception as e:
        import traceback
        traceback.print_exc()
