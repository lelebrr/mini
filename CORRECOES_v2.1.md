# Mini Lele — Correções, Integração e Otimizações (v2.1)

Este documento resume tudo que foi corrigido, integrado e otimizado nesta revisão.
O foco foi: **deixar o firmware realmente funcional**, **corrigir a pinagem**,
**adicionar carregamento/monitoramento de bateria** e **redesenhar toda a interface**
(WebUI + tela do dispositivo).

---

## 1. Bugs críticos corrigidos (impediam compilar/funcionar)

| # | Problema | Correção |
|---|----------|----------|
| 1 | **`main.cpp` não usava a arquitetura v2.0** — só rodava uma demo de hardware; pet, WiFi, WebUI e áudio existiam mas nunca eram inicializados. | `main.cpp` reescrito para **integrar todos os módulos** no `setup()`/`loop()`. |
| 2 | **ArduinoJson 7** instalado, mas o código usava a API v6 removida (`DynamicJsonDocument`, `createNestedObject`) → **não compilava**. | Migração completa para `JsonDocument` / `add<JsonObject>()` / `is<T>()` em `ConfigManager`, `PwnPet` e `WebHandler`. |
| 3 | **`SDMMC_D0 = 42` errado** → cartão SD nunca montava (config, saves, logs, portal, handshakes falhavam). | Corrigido para **GPIO3** (DO/MISO, conforme wiki oficial) + **EXIO7 (CS) em HIGH** no boot. |
| 4 | **`lv_font_montserrat_10` usado sem estar habilitado** no `lv_conf.h` → erro de link. | Habilitadas as fontes 10/12/14/16/20/28/40 no `lv_conf.h`. |
| 5 | **Membros estáticos definidos em headers** (`FaceHandler`, `EvilPortal`, `rtc_save`) → “multiple definition” ao integrar. | Tornados `inline` (C++17) ou movidos para `core_singletons.cpp`. |
| 6 | **`PwnCompress` quebrado** — tratava o retorno `size_t` como `int status`, com código morto e sem cabeçalho zlib. | Reescrito: usa corretamente `tdefl/tinfl`, escreve cabeçalho zlib e implementa `saveCompressedLog`. |
| 7 | **WebUI dependia de CDN (Bootstrap)** → quebrava **offline** no modo AP (uso normal!). | WebUI **reescrita 100% autocontida** (CSS/JS embutidos, sem CDN). |
| 8 | **Bibliotecas ausentes** (`XPowersLib`, `AsyncTCP`, `ESPAsyncWebServer`) com `lib_deps` vazio → não linkava. | Adicionadas ao `lib_deps` (versões fixadas). Touch passou a usar driver interno (removida a dependência de `TouchLib`, que **não suporta FT3168**). |
| 9 | **Amplificador no pino errado** — `AudioHandler` acionava o expansor (P6); o demo oficial usa **GPIO46**. | `setAmpPower()` corrigido para **GPIO46**. |
| 10 | **ES8311 nunca inicializado** → sem áudio. | Adicionado `AudioHandler::init()` com init do codec (I2C) — seguro: se falhar, o dispositivo continua funcionando sem áudio. |
| 11 | **Conflito de porta 80** — `EvilPortal` e `WebHandler` usavam a mesma porta. | Adicionado `WebHandler::stop()`; portal fica off por padrão e é mutuamente exclusivo com a WebUI. |
| 12 | **`CommandHandler` referenciava `extern power`/`gfx` inexistentes.** | Isolado do build (não é incluído; `PwnVoice` faz o mesmo papel). |
| 13 | **Senhas em texto puro** na API `/api/config`. | `getJSONRedacted()` mascara `sys_web_pass`/`sys_ap_pass`/`sys_sta_pass`; o placeholder `********` é ignorado ao salvar. |

---

## 2. Pinagem verificada (contra a wiki oficial Waveshare)

Conferido item a item; correções aplicadas em `include/pin_config.h` e na doc:

- **Display QSPI (SH8601):** CS 12, SCLK 11, SDIO0‑3 = 4/5/6/7 — ✅ ok
- **I²C (Touch/PMU/IMU/RTC/IO):** SDA 15, SCL 14 — ✅ ok
- **Touch FT3168:** INT 21, endereço 0x38 — ✅ ok
- **Áudio I²S (ES8311):** MCLK 16, BCLK 9, WS 45, DO 8, DI 10 — ✅ ok
- **Amplificador (PA):** **GPIO46** — ✅ corrigido no código
- **Cartão SD:** CLK 2, CMD 1, **D0 = 3 (corrigido, era 42)**, **CS = EXIO7** — ✅ corrigido

---

## 3. Bateria & carregamento (novo) — PL502030 250 mAh / 3,7 V

Carregamento por **USB‑C** via **AXP2101** (`include/core/PwnPower.h`):

- **`disableTSPinMeasure()`** — obrigatório para célula sem termistor.
- Corrente de carga **100 mA** (~0,4C), configurável (100/125/150/200 mA).
- Corte em **4,2 V**, término em **25 mA**, LED de carga automático.
- Proteções: aviso 10%, desligamento 5%, VSYS off 2,6 V.
- Monitoramento: `isCharging()`, `isVbusIn()`, `getChargeStatusStr()`,
  `getBatteryVoltage()`, `getEstimatedHours()`, `monitor()` (loga USB in/out).
- Estimativa de autonomia calibrada para 250 mAh.
- Novas chaves de config: `pwr_battery_capacity_mah`, `pwr_charge_current_ma`,
  `pwr_charge_target_mv`, `pwr_low_warn_pct`, `pwr_low_shutdown_pct`.

> **Recomendação:** mantenha a corrente de carga em **≤ 125 mA** para 250 mAh.

---

## 4. Design (o foco principal)

### WebUI (`include/web/WebAssets.h`)
- Reescrita do zero: tema escuro neon, **medidores circulares (SVG)**,
  cartão de **bateria/carga animado**, tiles de estatística, abas
  (Painel / Configurações / Arquivos / Sistema / Logs).
- Configurações agrupadas por categoria com rótulos amigáveis e toggles.
- Gerenciador de arquivos, **OTA** com barra de progresso, botão de reiniciar,
  **logs ao vivo** por WebSocket.
- 100% offline (sem CDN). Responsiva (desktop e celular).

### Tela do dispositivo (`include/core/PwnUI.h` + `FaceHandler.h`)
- Layout novo: barra superior (relógio + bateria/carga com ícone ⚡),
  cartão do rosto, nome/humor, medidores de fome/felicidade, chips de status.
- **Rostos corrigidos:** os kaomojis antigos usavam Unicode que a fonte
  Montserrat não tem (apareciam como quadradinhos). Agora são ASCII, grandes e
  coloridos por humor.

---

## 5. Como compilar

1. Abra a pasta no VS Code (PlatformIO).
2. Ambiente: `waveshare-esp32-s3-amoled`.
3. **Build** → **Upload** (a 1ª compilação baixa `XPowersLib`, `AsyncTCP`,
   `ESPAsyncWebServer`; depois fica em cache).
4. Cartão SD em FAT32 com a estrutura de `arquivos_cartao_sd/` (ou rode
   `generate_sd_structure.sh`).

---

## 5b. Novos recursos (v2.1)

Implementados por cima das correções, todos opcionais e com degradação segura:

| Recurso | Módulo | Destaque |
|---------|--------|----------|
| **Capacidade de bateria configurável** | PwnPower | `pwr_battery_capacity_mah` — troque a célula sem recompilar; cálculo de autonomia e limite de carga (0,5C) se ajustam sozinhos |
| **Relógio real + NTP** | drivers/PwnRTC | PCF85063 na tela/logs; sincroniza por NTP no modo STA |
| **Economia de energia** | core/PwnSleep | timeout de tela, auto‑dim, Eco, deep sleep acordando por toque (ext0) |
| **Interação por movimento** | main + PwnPet::onShake | chacoalhar alimenta o pet (acelerômetro) |
| **Persistência em RAM RTC** | PwnPet | stats sobrevivem ao deep sleep sem depender do SD |
| **Captura `.pcap` real + EAPOL** | WiFiTools | pcap linktype 105, detecção de handshake, buffer no callback e gravação no loop (seguro) |
| **Scanner BLE** | core/PwnBLE | varredura sob demanda; dispositivos viram "comida" |
| **UI por gestos** | core/PwnUI | `tileview` com 3 telas (Pet/Wi‑Fi/Energia) + anti‑burn‑in |
| **Botão físico** | main | clique = próxima tela · longo = deep sleep |
| **Portal cativo na WebUI** | WebHandler | DNS redireciona qualquer URL para a config |
| **Watchdog + buffer maior** | main | reinício automático se travar; LVGL 1/4 da tela em PSRAM |
| **Rotação de logs** | WiFiTools | evita crescimento infinito no SD |

Novas chaves de config: `pwr_battery_capacity_mah`, `pwr_charge_current_ma`,
`pwr_deep_sleep_after_sec`, `atk_ble_scan`, `atk_ble_interval_sec`, `sys_watchdog`.

## 6. Observação honesta sobre testes

As correções foram feitas com revisão estática rigorosa (pinagem conferida na
wiki oficial, API do ArduinoJson 7 e da XPowersLib validadas nos headers/exemplos,
JS da WebUI validado). **Não foi possível compilar/gravar em hardware neste
ambiente.** Faça um **Build** de verificação e, no primeiro boot, acompanhe o
log serial (USB‑CDC, 115200) — cada módulo imprime seu status (`[Power]`,
`[SD]`, `[Audio]`, `[Web]`, etc.), o que facilita identificar qualquer ajuste
fino específico da sua placa.
