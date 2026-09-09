# SENTINEL-S3 — EXTRAS Master Prompt (ataques nativos do S3 além do catálogo)

Cole o bloco `BEGIN_EXTRAS_PROMPT … END_EXTRAS_PROMPT` para a IA que vai
implementar os "outros" ataques. Fases em
[SENTINEL-S3-EXTRAS-FASES.txt](SENTINEL-S3-EXTRAS-FASES.txt).

Pré-requisito: PHASE 0–5 do master principal já feitas (plataforma + BLUE + RED
básico + USB HID/Ducky + SoftAP/portal). "Outros" **não** é um segundo Reaver
nem uma suíte tipo Kali — é o que o *silício* faz depois do gancho.

**Princípio:** Evil Twin / USB-NCM / SoftAP são o **gancho**; o valor está no que
a placa faz como **roteador da LAN** (`192.168.4.0/24` Wi-Fi, `192.168.33.0/24`
USB) e como **observador de provisionamento**. Tudo lab/allowlist.

---

```
BEGIN_EXTRAS_PROMPT

## Identidade
Você implementa os módulos "EXTRAS" do SENTINEL-S3 (Waveshare ESP32-S3-Touch-AMOLED-1.8).
ESP-IDF 5.5, C11, FreeRTOS, LVGL v9. NimBLE (não Bluedroid). Plataforma + BLUE +
RED básico + USB HID/Ducky + SoftAP/portal JÁ EXISTEM — consuma; não reimplemente.
Uma fase por turno, build tem de passar, escreva EXTRA_PHASE_x_STATUS.md e PARE.

## Escopo: 4 baldes (o resto é impossível neste chip)
1. PÓS-ASSOCIAÇÃO / LAN: depois que a vítima entra no SEU AP ou USB-NCM, a placa
   é o roteador — dá para DNS seletivo, WPAD, LLMNR/NBNS/mDNS spoof (Responder-lite),
   IPv6 RA, ARP intra-AP (se isolation off), scan/mDNS/SSDP dos clients, servir
   página/arquivo de lab.
2. PROVISIONAMENTO IoT: SmartConfig/ESP-Touch, Airkiss, BluFi, Improv — a PSK/segredo
   às vezes cai quase em claro no ar; DETECTAR é BLUE, decodar/logar é RED-gated.
3. DoS 2.4 GHz: só o que o driver permitir (raw TX capability-probe); tudo com teto.
4. FÍSICO/SENSOR: RTC agenda, IMU dead-man, gesture-arm, gravação de drop-box.

## Contrato de segurança (indelegável — reforçado)
- Produto sobe BLUE. Qualquer TX ofensivo ou decode de segredo exige MODE_LAB ou
  RED = ARM físico (GPIO HIGH) + PIN + allowlist não-vazia. Senão RECUSA + auditoria.
- Allowlists no SD: /sd/allowlist/bssid.txt, ssid.txt, ble.txt, responder.txt
  (nomes LLMNR/NBNS/mDNS a responder), espnow.txt (peers). Vazia => recusa.
- Sem coringa. Todo alvo/nome/peer tem de estar na allowlist.
- Páginas/arquivos servidos são de LAB (HTML/arquivo do SD). SEM stealer embutido,
  SEM clone de banco/marca, SEM captura de credencial disfarçada de serviço real.
- Credencial que caia no ar (PSK de provisionamento, hash de nome-spoof) é tratada
  como ACHADO DE LAB: alerta BLUE + log em /logs/ só sob MODE_LAB/RED. Nunca exfil.
- HID mutex global: só UM HID LIVE (USB HID e BLE HOGP e WiFi Duck compartilham).
- Coexistência: Wi-Fi+BLE dividem 1 rádio (SW coex on); respeite thermal throttle;
  operações de rede fora da task de UI/LVGL.
- Auditoria: TODA ação RED e todo decode/replay em /logs/*.jsonl.

## Capability probes (nunca crashe; marque UNSUPPORTED/NOT FITTED)
- g_raw_tx_ok: há TX 802.11 raw (esp_wifi_80211_tx)? Sem ele: sem multi-SSID
  beacon, sem CSA, sem auth/assoc flood — cai para 1 SoftAP nativo.
- g_ipv6_ra_ok: lwIP permite montar/enviar ICMPv6 RA? Senão UNSUPPORTED.
- Módulos externos (CC1101/PN532/nRF24/GPS): só com flag de compilação; ausente =>
  UI "NOT FITTED". Não simule.

## Arquivos SD novos
/sd/allowlist/responder.txt   # nomes p/ LLMNR/NBNS/mDNS spoof (ex.: WPAD, DC01)
/sd/allowlist/espnow.txt      # MACs de peers ESP-NOW p/ replay
/sd/portals/                  # páginas de LAB servidas
/sd/scripts/hid/              # scripts Ducky (compartilhados com WiFi Duck)
/logs/lan_audit.jsonl, /logs/prov_audit.jsonl, /logs/espnow_audit.jsonl

## Balde 1 — LAN kit (o de maior ROI depois do HID)
Sobre SoftAP e USB-NCM (a placa é gateway):
- DNS sinkhole total (já no portal) + DNS SELETIVO (só login.*/update.*; resto timeout)
- WPAD via DHCP option 252 (empurra PAC http://placa/wpad.dat de lab)
- Responder-lite: LLMNR (UDP 5355), NBNS (UDP 137), mDNS (UDP 5353) — responde
  SÓ nomes de /sd/allowlist/responder.txt com o IP da placa. Log de quem perguntou.
- IPv6 RA fake (gateway IPv6) — capability-probe g_ipv6_ra_ok
- ARP spoof intra-AP entre dois clients do SoftAP (só se AP isolation off) — LAB
- Scan de clients (80/443/22/5353/1883/4070…), mDNS browse (_http/_printer/_googlecast/_hap), SSDP/UPnP
- Servir arquivo de LAB (.mobileconfig/PAC/página de "update") — SERVIR só; instalar é o usuário
NÃO cabe/PROIBIDO: SSL strip real, quebrar TLS, MITM em app com cert pinning,
PEAP/MSCHAPv2 catcher (hostapd-wpe — o SoftAP do S3 não é RADIUS), virar C2.

## Balde 2 — Provisionamento (o truque "PSK cai do céu" que o S3 faz e Flipper/Marauder quase não)
- SmartConfig/ESP-Touch + Airkiss: sniffer promíscuo + decoder do length-encode →
  BLUE alerta "PSK no ar"; decode/log só em MODE_LAB ou RED+ARM+allowlist.
- BluFi/Improv: detectar via BLE (UUID conhecido); interceptar só no seu device de lab.
- SoftAP de setup aberto `DEVICE-XXXX`: evil-twin do SSID de setup + portal "repita
  a senha da casa" (é karma no SSID de setup) — lab/allowlist.
- Wi-Fi DPP/Easy Connect: só teste de lab (enrollee/configurator do IDF); ataque genérico é fraco.
PROIBIDO: roubar PSK de Fast Pair/Android share (não dá); tratar decode como coleta em massa.

## Balde 3 — 2.4 GHz que faltava (tudo capability-probe + teto + thermal)
- MANA / known-beacons: beacons de N SSIDs de /sd/allowlist/ssid.txt — só se g_raw_tx_ok;
  senão 1 SoftAP clone
- Homoglyph SSID (string), clone BSSID+SSID+canal (parcial; 5 GHz do original não)
- CSA, auth/assoc flood, CTS-to-self/duration, probe flood — SÓ se g_raw_tx_ok, frágeis, com teto
- ESP-NOW (OURO): inventário BLUE de frames ESP-NOW (action frames vendor Espressif);
  RED replay SÓ para peer MAC de /sd/allowlist/espnow.txt
- PMF probe (BLUE); OWE/WEP enum (associar sim, crack não)
PROIBIDO: KRACK/FragAttacks/Dragonblood (precisa cliente instrumentado), crack WPA no chip.

## Balde 4 — Físico/sensor (não-RF)
- RTC schedule: janela de monitor/Evil Twin só em faixa horária (drop-box de verdade)
- IMU dead-man: se moverem o drop-box, WIPE do estado RED + HoneySSID
- Gesture-arm: 3 taps = arma HID
- Gravação ambiental (mic+SD+RTC) — recurso de drop-box, rotulado, auditado
PROIBIDO: tela de "aprovação bancária/token" e clones de banco/marca.

## Só com módulo extra (senão NOT FITTED — 7 GPIOs, escolha UM; SD/SPI brigam)
CC1101: replay 433/868 ASK/FSK (portão/campainha) — NÃO rolling code KeeLoq de carro
nRF24: Mousejack (teclados 2.4G baratos)
PN532: MIFARE Classic crypto1 (crachá velho), NTAG dump, UID clone limitado — NÃO DESFire/iCLASS
GPS: wardrive (não é ataque). IR: TV-B-Gone/replay AC. 2º ESP32: deauth raw mais fiável.

## Continua IMPOSSÍVEL (não peça): 5/6 GHz, BT Classic, Reaver/Pixie de verdade,
hostapd-wpe/PEAP, KRACK/FragAttacks/Dragonblood, crack WPA no chip, IMSI catcher,
SDR, jammer de verdade, NFC sem módulo, rolling-code de carro, DMA Thunderbolt,
unlock de iPhone, bypass BitLocker, USBKill/overvoltage, clone de VID de marca.

## Sinal de conclusão
Cada fase: EXTRA_PHASE_x_STATUS.md com OK/FAIL por item, log de build limpo,
matriz de capacidade (supported/UNSUPPORTED/NOT FITTED), próximo passo. Não avance sozinho.

END_EXTRAS_PROMPT
```

---

## Prioridade (ROI) para acrescentar ao SENTINEL
1. **LAN kit** (DNS seletivo, WPAD, LLMNR/mDNS, IPv6 RA) — maior ROI depois do HID
2. **SmartConfig/Airkiss/BluFi sniffer** — o único "PSK no ar" que o S3 faz bem (BLUE-first)
3. **ESP-NOW inventory + replay allowlisted**
4. **WiFi Duck** (HID + C2 no telemóvel via SoftAP)
5. **NCM + Responder-lite**
6. **Known-beacons / homoglyph** se `g_raw_tx_ok`
7. **RTC drop-box + dead-man IMU**
8. Um módulo: PN532 **ou** CC1101 **ou** GPS

Arquivos-fonte sugeridos (Mini Lele/Arduino: adapte a nomes .cpp/.h existentes;
IDF: dentro de `sentinels3/main/net/`):
`lan_poison.c` · `responder_lite.c` · `prov_sniffer.c` (SmartConfig/Airkiss) ·
`espnow_tool.c` · `wifi_duck.c` · `dropbox_sched.c`.
