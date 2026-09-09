# ESP-NOW — inventário BLUE (passivo)

Observação passiva de tráfego **ESP-NOW** em 2.4 GHz. Muitos dispositivos IoT
caseiros (fechaduras, relés, sensores) usam ESP-NOW **sem criptografia**. Este
módulo **apenas observa** os peers no ar. **Não transmite, não faz replay** — o
replay é RED/gated e **não** está implementado neste firmware (fica na spec
[SENTINEL-S3-EXTRAS-FASES.txt](../SENTINEL-S3-EXTRAS-FASES.txt), EXTRA-PHASE 3).

## Arquivos
- `include/EspNowScan.h` / `src/EspNowScan.cpp` — detector, inventário, relatório.
- Hook no callback promíscuo (`src/WiFiTools.cpp`, branch `WIFI_PKT_MGMT`) → `EspNowScan::onFrame()`.
- `EspNowScan::poll()` no loop (e durante a varredura de canais).
- Console em `src/main.cpp` (`espnowHandleCommand`).

## Como detecta
ESP-NOW é um *vendor specific action frame* 802.11. O detector confere:
- FC = `0xD0` (action / management)
- categoria `0x7F` (vendor specific) no offset 24
- OUI Espressif `18:FE:34` (nos offsets 25 e 34)
- elemento vendor `0xDD` no offset 32, tipo `0x04`

Extrai o **src** (addr2), se o destino é **broadcast** (FF:FF:FF:FF:FF:FF) e o
tamanho do payload (`element_len − 5`). Inventário deduplicado por src (RSSI,
canal, último tamanho, broadcast/unicast, contagem, first/last seen).

O `onFrame` roda na task de Wi-Fi e só enfileira (barato); o parse/inventário
ocorre em `poll()` na task do loop.

## Console serial
```
espnow scan [s]   # varre canais 1..13 coletando frames ESP-NOW (default 6 s)
espnow report     # imprime e grava /reports/espnow_inventory.md
espnow clear      # limpa o inventário
```

## Verificação
Detector e inventário têm teste de host: frame ESP-NOW sintético (broadcast e
unicast), extração de src/paylen, negativos (beacon 0x80, OUI errado, truncado)
e dedup do inventário via `onFrame`+`poll`.

## Escopo
Só BLUE (observação). O **replay ESP-NOW** (reenviar frame a um peer da
allowlist) é RED e permanece **não implementado** por decisão de escopo.
