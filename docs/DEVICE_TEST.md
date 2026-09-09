# Checklist de Teste — Mini Lele v2.1

Preparação:
- Cartão SD FAT32 com esqueleto de `sd_skel/`
- Cabo USB-C + `pio device monitor -b 115200`
- (RED WPS) jumper no pino de ARM (padrão GPIO17)

## 1. Boot
- [ ] Tela acende (AXP2101/BLDO1 ok)
- [ ] Logs `[IO]/[DISPLAY]/[SD]/[LVGL]` no serial
- [ ] Home mostra bateria, hora, contadores

## 2. Touch e UI
- [ ] Toque responde (fix do FT3168 aplicado)
- [ ] Botão BOOT troca de tile; segurar = deep sleep
- [ ] Chacoalhar interage com o pet

## 3. BLUE
- [ ] Sniffer + PCAP grava em `/capturas/`
- [ ] BLE scan lista dispositivos
- [ ] `wps scan` popula inventário; chip WPS-ON/WPS-LOCKED coerente
- [ ] `wps report` grava `/reports/wps_inventory.md`
- [ ] `wps baseline` e alerta ao religar WPS de um AP conhecido

## 4. Ofensivo
- [ ] Evil Portal serve HTML e captura em `/portal_creds.json`
- [ ] (se aplicável) OnlineCrack envia handshake

## 5. WPS RED (laboratório)
- [ ] `wps arm` reflete o jumper
- [ ] Sem ARM → RECUSA; allowlist vazia → RECUSA; alvo fora dela → RECUSA
- [ ] `wps pbc/pin MAC` audita em `/logs/wps_audit.jsonl`
- [ ] Para em WPS lock / NACK flood / térmico

## 6. Energia e estabilidade
- [ ] Bateria % coerente; dim/off por timeout; toque acorda
- [ ] 30 s de scan sem travar; SD monta e grava
