# Minigotchi ESP32 - Suporte PlatformIO

Este documento descreve como usar o PlatformIO para compilar e flashar o firmware do Minigotchi ESP32.

## Pré-requisitos

1. Instale o [Visual Studio Code](https://code.visualstudio.com/)
2. Instale a extensão [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
3. Clone ou baixe este projeto

## Configuração do Projeto

O projeto já está configurado para PlatformIO com os seguintes arquivos:

- `platformio.ini` - Configurações principais do build
- `include/platformio.h` - Definições específicas para PlatformIO
- `lib_deps.txt` - Lista de dependências
- `platformio_override.ini` - Arquivo para configurações locais

## Compilando e Flashando

### Via Interface Gráfica (VS Code)

1. Abra a pasta do projeto no VS Code
2. Aguarde o PlatformIO carregar (pode demorar na primeira vez)
3. Clique no ícone de PlatformIO na barra lateral esquerda
4. Vá para a aba "PIO Home"
5. Clique em "Open" ao lado do projeto

### Via Linha de Comando

```bash
# Instalar dependências
pio lib install

# Compilar o projeto
pio run

# Flashar o dispositivo
pio run --target upload

# Monitor serial
pio device monitor
```

## Configurações Personalizadas

Para personalizar as configurações:

1. Copie `platformio_override.ini` para `platformio.local.ini`
2. Modifique as configurações desejadas no arquivo `platformio.local.ini`

## Opções de Build Disponíveis

### Board Suportado
- `esp32-s3-devkitc-1` - ESP32-S3 DevKitC-1 (padrão)

### Variáveis de Build
- `BOARD` - Altera o board alvo
- `MONITOR_SPEED` - Velocidade do monitor serial
- `UPLOAD_SPEED` - Velocidade de upload
- `BUILD_FLAGS` - Flags adicionais de compilação

### Exemplos de Uso

```bash
# Usar um board diferente
pio run -e esp32doit-devkit-v1

# Mudar a velocidade do monitor
pio device monitor --baud 921600

# Compilar com flags personalizadas
pio run --build_flags "-DCUSTOM_DEBUG=1"
```

## Depuração

Para depurar o firmware:

1. Conecte o ESP32 ao seu computador
2. Pressione F5 no VS Code ou clique no ícone de depuração
3. Configure breakpoints no código
4. Inicie a depuração

## Problemas Comuns

### Include Path
Se encontrar erros de include, certifique-se de que o VS Code reconheceu o projeto PlatformIO. Reinicie o VS Code se necessário.

### Bibliotecas Faltando
Execute `pio lib install` para instalar todas as dependências.

### Porta Serial
Verifique qual porta serial está sendo usada pelo seu ESP32 e configure no PlatformIO.

## Estrutura de Arquivos

```
minigotchi-ESP32/
├── platformio.ini          # Configurações principais
├── platformio_override.ini # Override de configurações
├── lib_deps.txt           # Dependências
├── src/
│   └── sketch.ino         # Arquivo principal (estrutura padrão PlatformIO)
├── include/
│   └── platformio.h       # Definições PlatformIO
└── ... (outros arquivos do projeto)
```

## Contribuição

Para contribuir com melhorias no suporte PlatformIO:

1. Faça um fork do projeto
2. Crie uma branch para sua feature
3. Faça commit das suas mudanças
4. Abra um Pull Request

## Licença

Este projeto está sob a licença GNU General Public License v3.0.

## Correções e Notas

### Bibliotecas Corrigidas
- **XPowersLib @ ^1.2.0**: Substituiu `adafruit/Adafruit AXP2101 @ ^1.0.3` que não estava disponível para Windows AMD64
- Removida opção `build_dir` do `platformio.ini` que não é suportada pela versão atual do PlatformIO

### Configuração Atual
- Board: ESP32-S3 DevKitC-1
- Framework: Arduino
- Monitor serial: 115200 baud
- Upload speed: 921600

### Bibliotecas Removidas
- **Adafruit SH8601**: Removida por não estar disponível para Windows AMD64. O display será configurado diretamente no código usando as bibliotecas TFT_eSPI e XPowersLib.