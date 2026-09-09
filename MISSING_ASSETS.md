# Assets SD — cartão do Mini Lele / SENTINEL-S3

Arquivos são abertos relativos à raiz do cartão (o SD_MMC prefixa o mountpoint).
Estrutura do cartão:

`
/sd/
  allowlist/ssid.txt      # SSIDs permitidos para RED
  allowlist/bssid.txt     # MACs permitidos para RED
  portals/lab_basic/index.html  # Captive portal
  scripts/hid/hello.txt   # Ducky script
  scripts/hid/lock-workstation.txt
  scripts/hid/open-notepad.txt
  badble/example.txt      # Script BLE
  public/README.txt        # Conteudo MSC
  reports/                 # Relatorios (wps_inventory.md)
  allowlist/wps_pins.txt   # PINs de laboratorio WPS (max 5)
  wps/baseline.jsonl       # baseline WPS por BSSID
  logs/wps_audit.jsonl     # auditoria RED WPS
`

Arquivos exemplo em [sd_skel/](sd_skel/).

## Formatos
- Scripts Ducky: .txt, suporta REM, DELAY, STRING, STRINGLN, ENTER, TAB, ESC, GUI, CTRL, ALT, SHIFT, LOCALE
- Locale: US ou ABNT2
- Scripts BLE: handle_hex + payload_hex
- Audio tamper: WAV 16kHz 16-bit mono
