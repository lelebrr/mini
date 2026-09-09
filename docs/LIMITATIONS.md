# Limitações — Mini Lele v2.1

## Rádio
- 2.4 GHz apenas (ESP32-S3 não tem 5 GHz)
- Antena SMD onboard (ganho ~2 dBi)
- TX RAW 802.11 pode não estar disponível (depende do driver) → deauth pode falhar
- APs com PMF (802.11w) required ignoram deauth
- Sem Bluetooth Classic (apenas BLE)

## Firmware (build Arduino)
- Arquitetura cooperativa (`loop()`), não multitarefa por subsistema
- WPA2/WPA3 não crackeável no chip (offloading impossível); OnlineCrack só
  **envia** o handshake para um serviço externo
- IDS avançado (deauth/beacon/evil-twin/karma/probe-leak/AirTag/HID) **não**
  implementado (é meta da reescrita IDF)
- Recursos ofensivos, exceto WPS RED, **não** passam por ARM/PIN/allowlist
- `WebHandler::sendWebSocket` é stub (broadcast WS não implementado)

## WPS
- `esp_wps` (enrollee) **não** aceita BSSID alvo → alvo rígido por BSSID é
  UNSUPPORTED; usamos *soft-target* (verificação pós-associação + abort/audit)
- Sem Pixie Dust (driver não expõe M1-M3)
- "PIN enrollee" apresenta o device-PIN do próprio ESP; **não** itera/quebra o
  PIN do registrador do AP (isso seria Reaver, proibido). Limite rígido de 5 PINs
- ARM do WPS usa GPIO17 por padrão (GPIO9 é I2S_BCLK neste build)

## Térmica
- Aquecimento em uso contínuo (scan + LVGL); WPS RED para a >80 °C (configurável)

## Hardware
- Placa tem 2 revisões: touch **FT3168 @0x38 (V1)** ou **CST820 @0x15 (V2)**.
  Este firmware traz driver **FT3168**; unidades V2 precisam de driver CST820.
