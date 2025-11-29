# Correções e Ajustes – Mini Lele em ESP32‑S3 Touch AMOLED 1.8

Este arquivo resume problemas identificados durante a porta do projeto para o kit **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8** e o estado atual das correções no firmware do **Mini Lele**.

Ele serve como um “log técnico” para quem for continuar ajustes de baixo nível (display, pinos, build, etc.).

---

## 1. Problemas originalmente identificados

### 1.1 Display / controlador incorreto

- Uso incorreto do driver **ST7789** em vez do controlador correto **SH8601** (AMOLED).
- Configuração de resolução divergente:
  - Firmware pensava em 320x240 ou 320x480
  - Hardware real é 368x448 (AMOLED 1.8" da Waveshare)
- Mapeamento de pinos SPI/QSPI incompatível com:
  - Layout original da placa Waveshare
  - Arquivo `pin_config.h` alvo do Mini Lele

### 1.2 Mapeamento de pinos (QSPI / I²C / Touch)

- Pinos QSPI (display) divergentes entre:
  - Código legado (outros projetos / placas customizadas)
  - Esquemático da Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8
- Pinos I²C incorretos ou pouco claros:
  - Confusão entre SDA/SCL em 15/14 vs 38/39
  - Mistura de configurações de placas diferentes
- Configuração de “Touch CS” em drivers herdados de displays SPI, quando o touch real usa apenas I²C (sem CS dedicado).

### 1.3 Configurações de build

- Particionamento insuficiente para:
  - Firmware grande (LVGL + WebUI + drivers complexos)
  - Espaço para filesystem/SD
- Flags de build:
  - Otimizações não alinhadas com uso intenso de PSRAM
  - Falta de defines específicos para:
    - LVGL
    - SH8601 via `Arduino_GFX`
    - USB CDC on boot
- Bibliotecas:
  - Dependências para display e PMU não estavam padronizadas para uso com PlatformIO (Windows/Linux/macOS)

---

## 2. Correções aplicadas (estado do repositório atual)

### 2.1 `platformio.ini`

- Ambiente consolidado:

  ```ini
  [env:waveshare-esp32-s3-amoled]
  ```

- Configurações principais:
  - `platform = espressif32` (ou versão fixa do repositório oficial do PlatformIO para ESP32)
  - `board = esp32-s3-devkitc-1`
  - `framework = arduino`
  - Flash size: **16 MB**
  - PSRAM habilitada (`BOARD_HAS_PSRAM`)
  - Particionamento customizado via `partitions.csv`/`default_16MB.csv` (dependendo da revisão mantida)

- Flags ajustadas para:
  - Desempenho (`-O3`, unroll loops, etc.)
  - LVGL 8.x/9.x com `LV_CONF_PATH` apontando para `include/lv_conf.h`
  - USB CDC:

    ```ini
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1
    ```

- Dependências de bibliotecas centralizadas em `lib_deps`, englobando:
  - `Arduino_GFX` (display SH8601)
  - `XPowersLib` (AXP2101)
  - `TouchLib`
  - `LVGL`
  - `ESPAsyncWebServer`, `AsyncTCP`
  - `ArduinoJson`
  - Bibliotecas auxiliares da Waveshare/lewisxhe/Maucke

### 2.2 Driver de display

- Troca lógica de foco:
  - De **ST7789** (TFT comum) para **SH8601** (AMOLED Waveshare)
- Resolução correta adotada no código:
  - `368x448`
- Integração via **Arduino_GFX**:
  - Uso de `Arduino_ESP32QSPI` como barramento
  - Driver `Arduino_SH8601` com parâmetros corretos (resolução, rotação, etc.)
- Reset do display:
  - Agora documentado e planejado para ser feito via TCA9554 (pino 0 do expansor I²C)
  - Vide `DISPLAY_GUIDE.md` e `FULL_HARDWARE.md`

### 2.3 Mapeamento de pinos

- `FULL_HARDWARE.md`, `HARDWARE.md` e `pin_config.h` documentam (ainda com algumas sobreposições históricas) que:
  - O hardware alvo principal é a **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**
  - O display usa QSPI com pinos típicos (CS/SCK/D0..D3) alinhados com o esquemático da placa
  - O touch usa I²C (sem CS) com FT3168/GT1151
  - O barramento I²C integra PMU, IMU, codec, RTC, expansor, etc.

> Ainda existem blocos duplicados em `pin_config.h`, vindos de diferentes branches (“legendary edition”, placas customizadas).  
> Porém, os documentos de hardware foram unificados para o cenário Mini Lele + Waveshare.

---

## 3. Pendências e pontos de atenção

Mesmo após as correções descritas acima, ainda há pontos a revisar/limpar:

### 3.1 `pin_config.h`

- Situação atual:
  - Arquivo contém múltiplos blocos de pinos para:
    - Placa Waveshare
    - Placas customizadas (“Edição Lendária” com ST7701S, RGB paralelo, etc.)
  - Presença de marcadores de merge antigos (`<<<<<<< HEAD`, `>>>>>>> branch`), em versões anteriores.
- Tarefas recomendadas:
  - Escolher **uma** configuração canônica para o Mini Lele + Waveshare
  - Extrair variantes de hardware para arquivos separados (ex.: `pin_config_legendary.h`)
  - Remover duplicações e blocos inconsistentes
  - Garantir que toda a documentação (`HARDWARE.md`, `FULL_HARDWARE.md`, `DISPLAY_GUIDE.md`) esteja alinhada ao mesmo pinout final

### 3.2 Inicialização completa do display SH8601

- Verificar se:
  - A sequência de reset via TCA9554 está implementada em um ponto único e reutilizável
  - Todos os modos de rotação usados pela UI foram testados
  - Qualquer ajuste de gama/brilho oferecido pelo SH8601 está devidamente configurado
- Opcional:
  - Expor controles de brilho/tema na WebUI, se ainda não estiverem disponíveis

### 3.3 Limpeza de configurações legadas

- Remover do `platformio.ini`:
  - Blocos comentados ou duplicados de ambientes antigos
  - Configurações de partição que não são mais usadas
- Garantir que:
  - A estrutura de `lib_deps` esteja consistente
  - Não existam referências a bibliotecas que já não são necessárias

---

## 4. Próximos passos sugeridos

1. **Unificar `pin_config.h`**
   - Manter apenas o mapeamento alvo do Mini Lele + Waveshare
   - Documentar claramente cada pino em comentários
2. **Testar em hardware real**
   - Verificar:
     - Inicialização estável do display
     - Touch funcional (FT3168/GT1151)
     - Áudio (ES8311 + I²S)
     - SDMMC com cartão FAT32
3. **Conferir build e memória**
   - Checar uso de PSRAM e heap
   - Ajustar flags se detectar instabilidade com LVGL e WebUI
4. **Atualizar documentação técnica**
   - Caso algo seja alterado em pinos ou drivers:
     - Atualizar `HARDWARE.md`, `FULL_HARDWARE.md`, `DISPLAY_GUIDE.md` e este `corrections_summary.md`
5. **Automatizar validação**
   - Se possível, criar scripts ou tarefas PlatformIO para:
     - Verificar consistência básica de `pin_config.h`
     - Rodar testes mínimos de inicialização (ex.: displays de teste, pings na WebUI)

---

Este arquivo deve ser mantido sempre que ajustes grandes forem feitos em:

- `platformio.ini`
- `pin_config.h`
- Drivers de display/touch/PMU

Assim, qualquer pessoa que pegar o projeto no futuro consegue entender rapidamente o histórico de problemas e as soluções adotadas.