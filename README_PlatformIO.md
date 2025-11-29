# Mini Lele – Guia de Uso com PlatformIO

Este documento descreve como usar o **PlatformIO** para compilar, gravar e depurar o firmware do **Mini Lele** na placa **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**.

---

## 1. Pré‑requisitos

1. Instale o [Visual Studio Code](https://code.visualstudio.com/)
2. Instale a extensão [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
3. Instale o **Git** (recomendado, para clonar o repositório)
4. Tenha o Python 3.x instalado (para scripts auxiliares, se necessário)

---

## 2. Arquivos de configuração importantes

O projeto já vem preparado para PlatformIO. Os arquivos principais são:

- `platformio.ini`
  - Define o ambiente:
    - `[env:waveshare-esp32-s3-amoled]`
  - Configura:
    - Placa (`esp32-s3-devkitc-1`)
    - Velocidade de monitor serial
    - Particionamento de 16 MB
    - Flags de compilação (otimizações, PSRAM, LVGL, USB CDC)
    - Bibliotecas necessárias (via `lib_deps`)
- `platformio_override.ini`
  - Arquivo opcional para ajustes locais (em muitos projetos não é versionado por padrão)
- `lib_deps.txt`
  - Lista de dependências de bibliotecas externas (usada em algumas versões de configuração)
- `sdkconfig.defaults`
  - Configurações base para a stack do ESP‑IDF quando usado junto do framework Arduino (em algumas builds)

> Não há `include/platformio.h` neste projeto – essa referência era de um template antigo e pode ser ignorada.

---

## 3. Abrindo o projeto no VS Code

1. Abra o **VS Code**
2. Vá em `File → Open Folder…` e selecione a pasta raiz do projeto (onde está o `platformio.ini`)
3. Aguarde o PlatformIO:
   - Ler o projeto
   - Baixar as bibliotecas definidas em `lib_deps`

No canto inferior da janela, você deve ver mensagens do PlatformIO indicando o carregamento do ambiente.

---

## 4. Compilando e gravando o firmware

### 4.1 Pela interface gráfica (VS Code)

1. No menu lateral, clique no ícone da **cabeça de alien** (PlatformIO)
2. Em **PROJECT TASKS**, localize o ambiente:

   ```ini
   env:waveshare-esp32-s3-amoled
   ```

3. Clique em:
   - `General → Build` para compilar
   - `General → Upload` para gravar o firmware na placa

Durante o upload:

- Certifique‑se de que a placa está conectada via **USB‑C**
- Se necessário, use a combinação:
  - Segurar **BOOT**
  - Tocar **RESET**
  - Soltar **BOOT** (modo de bootloader)

### 4.2 Pela linha de comando

Dentro da pasta do projeto:

```bash
# Compilar
pio run -e waveshare-esp32-s3-amoled

# Gravar firmware
pio run -e waveshare-esp32-s3-amoled -t upload

# Abrir monitor serial
pio device monitor -e waveshare-esp32-s3-amoled
```

---

## 5. Personalizando configurações (sem quebrar o repositório)

Para evitar conflitos com o `platformio.ini` original, utilize o arquivo de override:

1. Copie o arquivo de exemplo (se existir) ou crie um novo:

   ```bash
   cp platformio_override.ini platformio.local.ini  # ou crie do zero
   ```

2. Dentro de `platformio.local.ini`, você pode sobrescrever ajustes, por exemplo:

   ```ini
   [env:waveshare-esp32-s3-amoled]
   monitor_speed = 921600
   upload_speed = 460800
   ```

3. Para que o PlatformIO mescle as configurações de `platformio.ini` com esse arquivo extra (como `platformio.local.ini` ou `platformio_override.ini`), é necessário adicioná-lo na opção `extra_configs` dentro de uma seção `[platformio]` do `platformio.ini` (veja a documentação oficial).

> Mantenha `platformio.ini` o mais próximo possível do upstream; faça ajustes locais via override quando puder.

---

## 6. Opções e parâmetros de build

### 6.1 Ambiente principal

- **Ambiente:** `env:waveshare-esp32-s3-amoled`
- **Board:** `esp32-s3-devkitc-1` (equivalente ao módulo usado na placa Waveshare)
- **Framework:** `arduino`
- **Flash:** 16 MB (configurada em `board_upload.flash_size`)
- **PSRAM:** habilitada (`BOARD_HAS_PSRAM`)

### 6.2 Flags de compilação (resumo)

O `platformio.ini` configura, entre outras coisas:

- Otimização (`-O3`, `-funroll-loops`, etc.)
- Uso de PSRAM e LVGL:
  - `-DBOARD_HAS_PSRAM`
  - `-DLV_CONF_INCLUDE_SIMPLE`
  - `-DLV_CONF_PATH="include/lv_conf.h"`
- USB nativo:
  - `-DARDUINO_USB_CDC_ON_BOOT=1`
  - `-DARDUINO_USB_MODE=1`

Essas flags são importantes para:

- Desempenho (UI fluida com LVGL)
- Disponibilidade de memória para texturas e buffers
- Debug via USB‑C usando `Serial` (CDC), em vez de UART nos pinos 1 e 2.

---

## 7. Monitor serial e depuração básica

### 7.1 Monitor serial

- Velocidade padrão (verifique em `platformio.ini`): geralmente `115200` baud.
- No VS Code:
  - Clique em **Monitor** na barra de status do PlatformIO
- Na linha de comando:

  ```bash
  pio device monitor -e waveshare-esp32-s3-amoled
  ```

Isso permite observar:

- Logs de boot do ESP32‑S3
- Mensagens do PMU (AXP2101), display, touch, SD, etc.
- Informações de debug da WebUI e das ferramentas de pentest (se habilitadas no código)

### 7.2 Depuração (debug avançado)

O projeto não vem com uma configuração de **debug por JTAG** pré‑pronta, mas você pode:

- Usar o monitor serial e logs para a maioria dos problemas
- Integrar adaptadores JTAG suportados pelo PlatformIO, se desejar
- Configurar sessões de debug no VS Code (depende do hardware de debug disponível)

---

## 8. Estrutura do projeto (visão PlatformIO)

Para referência rápida, a estrutura do projeto dentro do contexto PlatformIO é aproximadamente:

```text
mini-lele/
├── platformio.ini            # Configuração principal de ambiente/build
├── platformio_override.ini   # (Opcional) ajustes locais
├── lib_deps.txt              # Lista auxiliar de dependências
├── src/
│   └── main.cpp              # Entrypoint principal do firmware
├── include/
│   ├── pin_config.h          # Mapeamento de pinos da placa
│   ├── core/                 # Lógica do Pet, gamificação, config, etc.
│   ├── drivers/              # Display, touch, PMU, IMU, SD, áudio, etc.
│   └── web/                  # WebUI, handlers HTTP/WS, etc.
├── lib/                      # Bibliotecas externas/adaptadas
├── arquivos_cartao_sd/       # Estrutura de exemplo do cartão SD
└── ...
```

---

## 9. Problemas comuns e soluções

### 9.1 Erros de include / bibliotecas faltando

- Rode:

  ```bash
  pio run -e waveshare-esp32-s3-amoled
  ```

  e deixe o PlatformIO baixar todas as libs listadas em `lib_deps`.
- Se o VS Code não “enxerga” corretamente os includes:
  - Feche e reabra o VS Code
  - Ou execute `Rebuild IntelliSense` (via Command Palette, `Ctrl+Shift+P`).

### 9.2 Erro ao detectar a porta serial

- Verifique qual porta o seu sistema atribuiu ao ESP32‑S3:
  - Windows: `COMx`
  - Linux: `/dev/ttyACM0`, `/dev/ttyUSB0`, etc.
  - macOS: `/dev/cu.usbmodem…` ou semelhante
- Configure a porta correta no PlatformIO, se necessário, ou deixe em auto.

### 9.3 Erros de flash / timeout

- Reduza a velocidade de upload em `platformio.ini`:
  - Ex.: `upload_speed = 460800` ou `115200`
- Use a sequência de bootloader:
  - Segure **BOOT**
  - Aperte **RESET**
  - Solte **BOOT** após alguns segundos

---

## 10. Notas sobre bibliotecas e correções

### 10.1 Bibliotecas importantes usadas pelo projeto

- **LVGL** (UI)
- **Arduino_GFX** (display SH8601)
- **XPowersLib** (PMU AXP2101)
- **ESPAsyncWebServer / AsyncTCP** (WebUI)
- **TouchLib** (controladores de touch FT3168/GT1151)
- **ArduinoJson** (configurações, WebUI)
- Bibliotecas auxiliares da Waveshare/lewisxhe/Maucke para drivers e sensores

A maior parte é obtida automaticamente via `lib_deps` no próprio `platformio.ini`.

### 10.2 Ajustes anteriores

Alguns ajustes já foram feitos (em versões anteriores do projeto) para melhorar a compatibilidade com o PlatformIO, por exemplo:

- Substituição de bibliotecas incompatíveis em certas plataformas
- Remoção de opções avançadas de build não suportadas em versões recentes do PlatformIO
- Ajustes do particionamento para **16 MB** de flash

Se você fizer mudanças significativas no `platformio.ini`, considere documentá‑las e, se fizer sentido, abrir um Pull Request.

---

## 11. Licença

O firmware do Mini Lele é distribuído sob a licença **GNU General Public License v3.0 (GPLv3)**.

Em resumo:

- Você pode usar, estudar, modificar e redistribuir
- Mas deve manter a mesma licença em trabalhos derivados
- Leia o texto completo da GPLv3 para detalhes legais

---

Se tiver sugestões específicas para melhorar a integração com o PlatformIO (novos ambientes, suporte a outros kits, presets de debug), fique à vontade para contribuir via Pull Request ou abrir uma issue descrevendo o caso de uso.