# Tutorial — Mini Lele v2.1

## Primeiros passos
1. Copie o esqueleto de `sd_skel/` para um cartão microSD FAT32 e insira na placa
2. Compile e grave: `pio run -e waveshare-esp32-s3-amoled -t upload`
3. Abra o monitor: `pio device monitor -b 115200`
4. Na tela: navegue pelos tiles (toque / botão BOOT); segurar BOOT = deep sleep

## BLUE (padrão)
1. Deixe o sniffer rodar (ou ative em Config `atk_auto_scan`)
2. No serial: `wps scan 8` para varrer WPS nos canais 1–13
3. `wps report` grava o inventário em `/reports/wps_inventory.md`
4. `wps baseline` fixa o baseline; o firmware alerta se um AP conhecido ligar WPS

## Ofensivo
- Evil Portal / deauth / OnlineCrack: pelo tile "Ataques" / WebUI
  (⚠ não trancados por ARM/allowlist neste build — use só em rede autorizada)

## WPS RED (laboratório)
1. Coloque o jumper de ARM em HIGH (pino padrão GPIO17)
2. Preencha `/allowlist/bssid.txt` com o alvo autorizado
3. `wps arm` (confere), depois `wps pbc AA:BB:CC:DD:EE:FF` ou `wps pin AA:BB:...`
4. Acompanhe `wps status`; tudo é auditado em `/logs/wps_audit.jsonl`

## Documentos
- [README.md](README.md) · [MANUAL.md](MANUAL.md) · [docs/WPS.md](docs/WPS.md) · [docs/DEVICE_TEST.md](docs/DEVICE_TEST.md)
