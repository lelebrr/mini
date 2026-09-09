# WPS — módulo BLUE-first, RED-limitado

O suporte a WPS segue o contrato SENTINEL: **BLUE por padrão** (observação
passiva) e **RED estritamente de laboratório** (ARM físico + allowlist +
auditoria). Nada aqui implementa Reaver, Pixie Dust, brute-force online de PIN
ou DoS/lockout de WPS.

> Caminhos no SD: o firmware abre arquivos **relativos à raiz do cartão**
> (o `SD_MMC` prefixa o mountpoint). Onde este doc escreve `/sd/allowlist/...`,
> o caminho no código é `/allowlist/...` (mesmo local físico no cartão).

Arquivos:
- `include/WpsCommon.h` — modelo de dados, IDs de atributos WSC, bits de config.
- `include/WpsBlue.h` / `src/WpsBlue.cpp` — parser do IE, inventário, higiene,
  baseline, relatório.
- `include/WpsRed.h` / `src/WpsRed.cpp` — enrollee de laboratório (PBC/PIN).
- Hook em `src/WiFiTools.cpp` (callback promíscuo) → `WpsBlue::onFrame()`.
- Console/loop em `src/main.cpp`.

---

## BLUE (sempre ativo)

Faz o parse do vendor IE do Wi-Fi Simple Config em beacons/probe-responses:
`Element ID 221 (0xDD), OUI 00:50:F2, OUI type 0x04`.

Campos extraídos por AP:
- `wps_version` (atributo Version base 0x104A; marca WSC 2.0 pelo subelemento
  Version2 na Vendor Extension WFA 00:37:2A).
- `config_methods` (0x1008) → categorias **PIN / PBC / NFC / Keypad**.
- `ap_setup_locked` (0x1057).
- `manufacturer` (0x1021), `model_name` (0x1023), `model_number` (0x1024),
  `device_name` (0x1011), `uuid` (UUID-E 0x1047).

**Chip de higiene** por AP:
- `WPS-ON` — WPS presente e **não** travado (âmbar; exposto).
- `WPS-LOCKED` — WPS presente porém `ap_setup_locked` (azul; menos exposto).
- `WPS-OFF` — sem IE WPS (verde).

**Baseline / alerta**: `/sd/wps/baseline.jsonl` mapeia `BSSID → wps_on`. Quando
um AP **conhecido** (presente no baseline) com `wps_on:false` passa a expor WPS,
o firmware emite um alerta BLUE (via `PwnUI::showAlert`). APs desconhecidos não
geram alerta. Atualize o baseline com `wps baseline`.

**Relatório**: seção `## WPS inventory` escrita em `/sd/reports/wps_inventory.md`
(e disponível em `WpsBlue::reportSection()`), com contagem de WPS-ON/LOCKED e uma
entrada por AP.

**Nota INFO opcional (pixie)**: alguns OUIs/modelos legados recebem a marca
`INFO: legacy chipset, historically pixie-vulnerable`. É **apenas informativo** —
nenhuma extração M1-M3 e nenhum ataque são habilitados por isso.

### Console serial BLUE
```
wps scan [segundos]   # varre canais 1..13 coletando IEs WPS (default 5 s)
wps report            # imprime e grava a seção "WPS inventory"
wps baseline          # snapshot do inventário atual como baseline
```

---

## RED (laboratório — ARM + allowlist + auditoria, senão RECUSA)

### Gates (todos obrigatórios, fail-closed)
1. **ARM físico** — GPIO `WPS_ARM_GPIO` (default **17**, nível ALTO = armado).
   Sem ARM → **RECUSA**. Ajuste ao seu jumper via build_flag
   `-DWPS_ARM_GPIO=<pino>` (o default 17 evita conflito com os pinos já usados:
   GPIO9 é I2S_BCLK, GPIO0 é BOOT).
2. **Allowlist de BSSID** — `/sd/allowlist/bssid.txt` (formato
   `XX:XX:XX:XX:XX:XX # comentário`). Vazia → **RECUSA**. Alvo fora dela →
   **RECUSA**. Alvo coringa (não especificado) → **RECUSA**.
3. **Auditoria** — toda tentativa é gravada em `/sd/logs/wps_audit.jsonl`
   (start, attempt, event, stop, finish, gate/REFUSED, verify/VIOLATION).

### Operações (só enrollee via `esp_wps`)
- **PBC** por até **120 s**, com contagem regressiva na UI
  (`WpsRed::secondsRemaining()`).
- **PIN enrollee**: até **5** PINs (device-PIN) de `/sd/allowlist/wps_pins.txt`,
  **depois PARA**. Exemplos em `sd_skel` incluem `00000000` e `12345670`,
  rotulados `LAB`.
- **PIN vendor (opcional)**: calcula **1** candidato do BSSID (serial 24-bit +
  checksum WPS), **1 tiro**. Desativado por padrão
  (`-DWPS_ENABLE_VENDOR_PIN=1` para habilitar). **Não** há brute 11000 online.

### Parada imediata
Interrompe e audita ao detectar: **WPS lock** (beacon do alvo com
`ap_setup_locked`), **NACK flood** (≥3 falhas), ou **térmico**
(`temperatureRead() > WPS_THERMAL_MAX_C`, default 80 °C).

### Console serial RED
```
wps arm                       # mostra estado do ARM físico
wps allow                     # recarrega a allowlist e mostra a contagem
wps pbc  AA:BB:CC:DD:EE:FF     # PBC 120 s contra alvo allowlisted
wps pin  AA:BB:CC:DD:EE:FF     # enrollee PIN (máx 5) contra alvo allowlisted
wps vpin AA:BB:CC:DD:EE:FF     # vendor-PIN 1-shot (se habilitado)
wps status                    # estado/contagem regressiva
wps stop                      # aborta a operação em andamento
```

---

## Limite de hardware: alvo por BSSID é "soft"

O `esp_wps` (enrollee) **não aceita um BSSID alvo** — `esp_wps_config_t` não tem
campo de BSSID; o enrollee associa a qualquer AP que esteja em modo WPS ativo.
O contrato pede "PBC/PIN contra o BSSID allowlisted **apenas**", o que **não pode
ser garantido de forma rígida** neste driver.

Conforme a cláusula de fallback do pedido ("If esp_wps cannot target a specific
BSSID, document UNSUPPORTED and keep BLUE parser"), o alvo rígido por BSSID é
**UNSUPPORTED** no `esp_wps`.

Implementação adotada: **soft targeting**. Ao ter sucesso, o firmware lê o BSSID
associado (`esp_wifi_sta_get_ap_info`); se **não** for o alvo, ele **aborta,
desconecta e audita `VIOLATION_NOT_TARGET`**. É uma garantia pós-associação, não
um travamento de BSSID no rádio. O parser BLUE permanece completo e é o caminho
recomendado.

### Nota honesta sobre o "PIN enrollee"
O enrollee apresenta o **seu próprio** device-PIN ao AP; ele **não** itera o PIN
do registrador do AP (isso seria Reaver/registrar-brute, **proibido**). Portanto
o fluxo PIN **só** conclui se o AP-alvo foi configurado para aceitar aquele PIN.
Ele existe como fluxo de laboratório limitado e auditado — não "quebra" o PIN de
um AP.

---

## Proibições (não implementadas)
- Brute-force online de PIN / máquina de estados Reaver / registrar-brute.
- Pixie Dust (sem extração de campos M1-M3 neste driver).
- DoS/lockout de WPS como recurso.
- WPS por NFC/USB.
- WPS coringa (alvo fora da allowlist).

## Configuração (build_flags)
| Flag | Default | Descrição |
|------|---------|-----------|
| `WPS_ARM_GPIO` | 17 | GPIO do jumper de ARM (nível ALTO = armado). `<0` = sempre desarmado. |
| `WPS_ARM_ACTIVE_HIGH` | 1 | 1 = ativo em nível ALTO; 0 = ativo em nível BAIXO. |
| `WPS_THERMAL_MAX_C` | 80.0 | Limite térmico de parada (°C). |
| `WPS_ENABLE_VENDOR_PIN` | 0 | Habilita o gerador vendor-PIN 1-shot. |
