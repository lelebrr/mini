# BLE — BLUE (Fases A, B e C implementadas)

Suporte BLE do Mini Lele. **BLUE-first**: scan → parser de AD → classificação de
vendor → inventário. **Não transmite, não conecta, não ataca.**

> Stack: o Mini Lele usa **Bluedroid** (`BLEDevice.h`; `CONFIG_BT_BLUEDROID_ENABLED`).
> A spec SENTINEL-S3 ([SENTINEL-S3-BLE-MASTER-PROMPT.md](../SENTINEL-S3-BLE-MASTER-PROMPT.md))
> pede **NimBLE** — é a meta da reescrita IDF. O parser abaixo opera sobre os
> bytes crus do advertising, então é independente de stack.

## Arquivos
- `include/BleCommon.h` — structs, tipos de AD, company IDs, service UUIDs,
  tabela Apple Continuity, classificação de endereço.
- `include/BleBlue.h` / `src/BleBlue.cpp` — parser, classificador, inventário, relatório.
- Hook em `src/core/PwnBLE.cpp` (callback de scan Bluedroid) → `BleBlue::onAdv()`.
- Console em `src/main.cpp` (`bleHandleCommand`).

## O que a Fase A faz (real, testado)
- **Parser de AD à prova de truncar**: percorre estruturas `[len][type][data]`;
  nunca lê além do `len` de cada campo nem do buffer.
- Extrai: nome, tx power, flags, service UUIDs 16-bit, service data 16-bit,
  presença de UUID 128-bit (incl. NUS), manufacturer (company id) e, para Apple,
  o Continuity type.
- **Classificação de vendor** (precedência: manufacturer → service → nome Flipper
  → NUS → HID):
  Apple Continuity / AirPods (0x07) / Find My (0x12) / iBeacon (0x02),
  Microsoft Swift Pair (0x0006), Samsung (0x0075), Espressif BluFi (0x02E5),
  Xiaomi (0xFE95/0x038F), Tile (0xFEED/0x0157), Google Fast Pair (0xFE2C),
  Eddystone (0xFEAA), Improv Wi-Fi (0x4677), Mesh (0x1827/0x1828),
  HID/HOGP (0x1812), Nordic UART (NUS 128-bit), Flipper Zero (prefixo de nome).
- **Classificação de endereço**: public / random-static / RPA / NRPA (pelos 2 bits
  altos do MSB, para endereços aleatórios).
- **Inventário** deduplicado por endereço (até `MAX_DEV` = 96): rssi, contagem,
  first/last seen; recicla o mais antigo quando cheio.
- **Relatório** `## BLE inventory` em `/reports/ble_inventory.md`
  (e `BleBlue::reportSection()`), com contagem de devices e de HID.

## Fase B — IDS (real, testado)
Módulo `include/BleIds.h` / `src/BleIds.cpp`, alimentado pelo `BleBlue`
(observer `setObserver`). Tudo receive-only.
- **Spam por tipo**: contadores com decaimento (Swift Pair, Fast Pair, Apple
  Nearby, Samsung, Continuity); flood cruza o limiar → alerta (com histerese).
- **HID inesperado**: 1º device com serviço 0x1812 → alerta (dedup por addr).
- **Flipper Zero**: assinatura por nome → alerta.
- **Stalker**: tag Find My/Tile ou endereço random-static, **não** no baseline,
  que persiste por N episódios de movimento (IMU) → alerta "te acompanha".
- **Baseline** de addrs conhecidos (`/allowlist/ble.txt`) exclui do stalker.
- **Foxhunt**: alvo por addr; beep com cadência proporcional ao RSSI
  (mais forte = mais rápido).
- Alertas em `/logs/ble_ids.jsonl`. Os callbacks de scan rodam na task do BT;
  o alerta é bufferizado e a UI atualiza no loop — LVGL nunca é tocado da task BT.

## Console serial
```
ble scan [s]   # roda um scan (default 4 s) e alimenta inventário + IDS
ble report     # imprime e grava /reports/ble_inventory.md
ble clear      # limpa o inventário
ble ids        # nº de alertas do IDS até agora
ble gatt MAC   # GATT audit de um alvo da allowlist (read-only)
ble fox MAC    # inicia foxhunt de um endereço (beep no loop)
ble foxstop    # encerra foxhunt
```
(`PwnBLE` faz init preguiçoso no primeiro `ble scan`.)

## Verificação
Parser/classificador **e o IDS** têm teste de host (ADs sintéticos: iBeacon,
AirPods, Find My, Swift Pair, Fast Pair, Eddystone, Tile, HID, Flipper, NUS;
truncação; flags/tx-power; endereço; dedup). IDS: alerta HID+dedup, Flipper, flood
Swift Pair, stalker por 3 episódios, exclusão por baseline, curva do foxhunt.
Todos os asserts passando. GATT: isOpenWrite/propsString, load+isAllowed da
allowlist, recusa fora dela, e formatReport (agrupamento por service, marcação de
write exposta). Todos passando.

## Fase C — GATT audit (só allowlist, real)
Módulo `include/BleGatt.h` / `src/BleGatt.cpp`. **Read-only, sem escrita, sem ataque.**
- Recusa alvo fora de `/allowlist/ble.txt` (auditado `REFUSED_NOT_ALLOWLISTED`).
- Conecta como central (Bluedroid), enumera services/characteristics e propriedades
  (R/W/Wnr/N/I), marca **WRITE exposta** (heurística: writable enumerável → revisar).
- Grava `gatt-<MAC>.md` em `/reports/` e auditoria em `/logs/ble_gatt.jsonl`;
  desconecta limpo ao fim.
- O corpo de conexão Bluedroid compila sob `#if defined(ARDUINO)` (real no firmware);
  a lógica pura (allowlist, props, relatório) é testada no host.
- Comando: `ble gatt AA:BB:CC:DD:EE:FF`.

## Próximas (lista verde, ainda não implementadas)
- **ESP-NOW inventory (BLUE)** — observação passiva de peers ESP-NOW.
- **RTC drop-box + dead-man IMU** — janela agendada e wipe de estado ao mover.
Plano completo em [SENTINEL-S3-BLE-FASES.txt](../SENTINEL-S3-BLE-FASES.txt)
e [SENTINEL-S3-EXTRAS-FASES.txt](../SENTINEL-S3-EXTRAS-FASES.txt).
