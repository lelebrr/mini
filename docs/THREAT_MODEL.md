# Modelo de Ameaças — Mini Lele v2.1

Descreve o que o firmware **atual (Arduino)** realmente detecta e faz. A visão
mais trancada (todo RED sob ARM+PIN+allowlist) é a meta da reescrita IDF.

## BLUE observa/detecta
- Scan passivo: APs e dispositivos BLE (GAP)
- Sniffer promíscuo + captura PCAP
- EAPOL **observado** (handshake capturado, **não** crackeado no chip)
- **WPS**: presença/higiene do IE WPS (WPS-ON/WPS-LOCKED); alerta quando um AP
  **conhecido** (baseline) passa a expor WPS

> IDS avançado (deauth flood, beacon flood, evil-twin, karma, probe-leak,
> AirTag/Tile, HID-over-GATT) é **planejado na visão SENTINEL-S3** e **não** está
> implementado no build Arduino atual.

## Ofensivo (o que o firmware pode fazer)
- Evil Portal (SoftAP + captura de credenciais, HTML do cartão)
- Deauth (se o driver permitir TX 802.11)
- OnlineCrack (envio de handshakes a serviço externo)
- **WPS RED de laboratório**: enrollee PBC/PIN, allowlist + ARM + auditoria,
  *soft-target*, até 5 PINs e para

## Contrato de segurança — estado real
- **WPS RED**: trancado (ARM físico GPIO17 + allowlist de BSSID + auditoria;
  alvo coringa/allowlist vazia = RECUSA).
- **Evil Portal / deauth / OnlineCrack**: **não** trancados por ARM/PIN/allowlist
  neste firmware — diferença conhecida vs. a visão SENTINEL-S3. Cabe ao operador
  usar apenas em ambiente próprio/autorizado.

## O WPS RED NÃO faz
- Brute-force online de PIN / Reaver / registrar-brute
- Pixie Dust (sem extração de campos M1-M3 neste driver)
- DoS/lockout de WPS como recurso
- WPS por NFC/USB, ou alvo fora da allowlist (coringa)

## Fora de escopo do hardware
- Crackear WPA2/WPA3 no chip; 5 GHz; jammer; IMSI catcher; Bluetooth Classic;
  portais bancários/Apple/Google; stealer de cookies/Chrome
