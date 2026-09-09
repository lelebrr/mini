# Manual — Mini Lele v2.1

Firmware Arduino para Waveshare ESP32-S3-Touch-AMOLED-1.8. UI LVGL em tiles;
console de comandos WPS via serial (115200).

## Interface (LVGL)

- Barra de status: modo (BLUE), bateria, Wi-Fi, BLE, SD, temperatura.
- Tiles: **HOME / WiFi / BLE / Ataques / Captura / Config**.
- **Botão BOOT (GPIO0)**: clique curto = próximo tile / acorda tela; segurar
  (>1,2 s) = deep sleep.
- **Toque**: navega e interage; qualquer toque acorda a tela.
- **Chacoalhar** (IMU): interage com o pet.

## Pet / gamification

Pet virtual com nível, XP, fome e humor; ganha XP com handshakes/BLE e evolui
de estágio. Estado salvo em `/pwn_pet_save.json` e `/game_stats.bin`.

## BLUE (defensivo, padrão)

### Wi-Fi
- Sniffer passivo (modo promíscuo) + captura **PCAP** (linktype 105) em `/capturas/`.
- Lista de dispositivos vistos; contagem de EAPOL **observada** (não crackeia).

### BLE
- Scan GAP (Bluedroid); lista addr / RSSI / nome.

### BLE inventory (novo)
- Parser de AD (à prova de truncar) + classificação de vendor (Apple Continuity/
  AirPods/Find My, Swift Pair, Fast Pair, Tile, Flipper, Eddystone/iBeacon, Xiaomi,
  Improv, BluFi, HID/HOGP, Mesh, Nordic UART) e tipo de endereço (public/random-
  static/RPA/NRPA).
- Inventário deduplicado; relatório `## BLE inventory` em `/reports/ble_inventory.md`.

### BLE IDS (novo)
- Spam por tipo (Swift Pair/Fast Pair/Apple Nearby/Samsung), alerta HID inesperado,
  stalker (tag Find My/Tile persistente durante movimento do IMU), Flipper, baseline
  (`/allowlist/ble.txt`) e foxhunt (beep proporcional ao RSSI). Alertas em
  `/logs/ble_ids.jsonl`.
- Console: `ble scan [s]` · `ble report` · `ble clear` · `ble ids` · `ble fox MAC` · `ble foxstop`.

### BLE GATT audit (novo, só allowlist)
- `ble gatt AA:BB:CC:DD:EE:FF`: conecta a um alvo **da allowlist** (`/allowlist/ble.txt`),
  enumera services/characteristics, marca WRITE exposta e grava `/reports/gatt-<MAC>.md`.
  Read-only, sem escrita; alvo fora da allowlist é recusado. Auditoria em `/logs/ble_gatt.jsonl`.
- Detalhe em [docs/BLE.md](docs/BLE.md).

### ESP-NOW inventory (novo, passivo)
- Observação passiva de peers ESP-NOW (fechaduras/relés/sensores caseiros em 2.4 GHz).
  `espnow scan [s]` · `espnow report` · `espnow clear`. Relatório em
  `/reports/espnow_inventory.md`. Só BLUE — sem TX/replay. Detalhe em [docs/ESPNOW.md](docs/ESPNOW.md).

### WPS (inventário)
- Parser do vendor IE WPS (00:50:F2:04) em beacons/probe-responses.
- Mostra: wps_version, config_methods (PIN/PBC/NFC/Keypad), ap_setup_locked,
  fabricante, modelo, device_name, uuid.
- Chip de higiene por AP: **WPS-ON** / **WPS-LOCKED** / **WPS-OFF**.
- Baseline: alerta quando um AP conhecido passa a expor WPS.
- Relatório `## WPS inventory` em `/reports/wps_inventory.md`.
- Console: `wps scan [s]` · `wps report` · `wps baseline`.
- Detalhe em [docs/WPS.md](docs/WPS.md).

## Ofensivo (tile "Ataques")

> **Atenção:** neste firmware, os recursos abaixo (exceto WPS RED) **não** são
> trancados por ARM/PIN/allowlist. Use só em redes próprias ou autorizadas.

- **Evil Portal**: SoftAP + captive portal (HTML de `/portals/`), captura de
  credenciais em `/portal_creds.json`.
- **Deauth**: toggle (depende de o driver permitir TX 802.11).
- **OnlineCrack**: envia handshakes capturados para serviço externo de quebra
  (requer Wi-Fi configurada).

### WPS RED (enrollee, laboratório — trancado)
Único caminho ofensivo com contrato completo:
- Gates fail-closed: **ARM físico** (GPIO padrão 17) + **allowlist** de BSSID
  (`/allowlist/bssid.txt`) + alvo não-coringa; senão **RECUSA**.
- **PBC** (120 s, contagem na UI) ou **PIN** (até 5 device-PINs de
  `/allowlist/wps_pins.txt`, depois PARA); vendor-PIN 1-shot opcional.
- *Soft-target*: `esp_wps` não aceita BSSID alvo; se o AP associado ≠ alvo,
  aborta e audita `VIOLATION`.
- Para na hora em: WPS lock, NACK flood ou térmico.
- Auditoria de toda tentativa em `/logs/wps_audit.jsonl`.
- **Não faz**: brute-force de PIN/Reaver, Pixie Dust, DoS/lockout, WPS coringa.
- Console: `wps arm` · `wps allow` · `wps pbc|pin|vpin MAC` · `wps status` · `wps stop`.

## Config / WebUI

- Config em `/config.json` (editável pela WebUI ou serial JSON).
- WebUI: servidor Async + DNS de portal cativo.
- Energia: dim/off por timeout, deep sleep opcional (AXP2101).

## Referências

[README.md](README.md) · [docs/WPS.md](docs/WPS.md) · [docs/THREAT_MODEL.md](docs/THREAT_MODEL.md) · [docs/DEVICE_TEST.md](docs/DEVICE_TEST.md)
