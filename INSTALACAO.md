# Guia de Instalação – Mini Lele v2.0

Este guia explica, passo a passo, como preparar o ambiente, o cartão SD e a placa **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8** para rodar o **Mini Lele**.

---

## 1. Requisitos

### 1.1 Hardware

- Placa **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**
- Cartão **microSD** (mínimo 4 GB, **FAT32**)
- Cabo **USB‑C** de boa qualidade
- (Opcional) Bateria **Li‑Po 3,7 V** com conector MX1.25

### 1.2 Software no computador

- **Sistema operacional:** Windows, Linux ou macOS
- **Visual Studio Code:**  
  https://code.visualstudio.com/
- **Extensão PlatformIO IDE para VS Code:**  
  https://platformio.org/install/ide?install=vscode
- **Python 3.x** (recomendado, para scripts auxiliares)  
  https://www.python.org/

---

## 2. Clonando o repositório

Abra um terminal e execute:

```bash
git clone https://github.com/seu-usuario/mini-lele.git
cd mini-lele
```

> Ajuste a URL acima para o repositório que você estiver realmente usando (fork, mirror, etc).

---

## 3. Preparando o cartão SD

O Mini Lele depende do cartão SD para:

- Armazenar handshakes `.pcap`
- Guardar logs (sniffer, Evil Portal, etc.)
- Ler/armazenar configurações persistentes
- Reproduzir áudios TTS (voz do Pet)

### 3.1 Formatar o cartão

1. Insira o cartão no computador
2. Formate em **FAT32** (tabela de partição MBR)
3. Nome do volume pode ser qualquer um (ex.: `MINILELE`)

### 3.2 Gerar estrutura de pastas automaticamente (recomendado)

Dentro da pasta do projeto:

```bash
# Linux / macOS
sh generate_sd_structure.sh
```

Isso criará a estrutura mínima em `sd_out/`.  
Depois, basta copiar TUDO de `sd_out/` para a raiz do cartão microSD.

Resultado esperado no SD:

```text
/
├── arquivos_cartao_sd/
│   ├── evil_portal/
│   ├── voice/
│   ├── tts/
│   ├── wifi_config.txt
├── capturas/
└── fila_envio/
```

### 3.3 Estrutura manual (se não usar o script)

Se preferir criar manualmente, a raiz do SD deve ter:

```text
/
├── arquivos_cartao_sd/         # Assets do sistema
│   ├── evil_portal/            # Templates HTML do portal cativo
│   ├── tts/                    # Áudios TTS (WAV 16kHz 16-bit mono)
│   ├── voice/                  # Arquivos de entrada de voz (gerados em runtime)
│   ├── wifi_config.txt         # Configuração de Wi‑Fi para upload (opcional)
│   ├── macs_detectados.txt     # (gerado automaticamente pelo sniffer)
│   └── credenciais_capturadas.txt # (gerado automaticamente pelo Evil Portal)
├── capturas/                   # Handshakes .pcap
└── fila_envio/                 # Arquivos prontos para upload (fila)
```

Exemplo de `wifi_config.txt` (para usar upload automático/manual):

```text
SSID=MinhaRedeWiFi
PASS=SenhaDaMinhaRede
KEY=MinhaKeyDoWpaSec
```

> Detalhes completos sobre os arquivos esperados: ver `MISSING_ASSETS.md`.

---

## 4. Abrindo o projeto no VS Code / PlatformIO

1. Abra o **Visual Studio Code**
2. Vá em `File` → `Open Folder...` e selecione a pasta do projeto (`mini-lele`)
3. Aguarde o PlatformIO:
   - Ler o arquivo `platformio.ini`
   - Baixar todas as dependências listadas em `lib_deps` / `lib_deps.txt`  
     (pode demorar alguns minutos na primeira vez)

O ambiente principal definido em `platformio.ini` é:

```ini
[env:waveshare-esp32-s3-amoled]
```

É esse ambiente que você deve usar para build, upload e monitor.

---

## 5. Compilando o firmware

### 5.1 Pelo VS Code (interface gráfica)

1. No canto lateral esquerdo, clique no ícone da **cabeça de alien** (PlatformIO)
2. Em “Project Tasks”, localize o ambiente `env:waveshare-esp32-s3-amoled`
3. Clique em:
   - `General → Build` para compilar
4. Aguarde a compilação terminar sem erros

### 5.2 Pela linha de comando

Dentro da pasta do projeto:

```bash
pio run -e waveshare-esp32-s3-amoled
```

Se tudo estiver correto, o build termina com sucesso.

---

## 6. Gravando o Mini Lele na placa

### 6.1 Conexão física

1. Conecte a placa via **USB‑C** ao computador
2. (Se necessário) Pressione e segure o botão **BOOT**
3. Dê um toque rápido no botão **RESET**
4. Solte o botão BOOT após alguns segundos (modo de download, se requisitado)

### 6.2 Upload pelo VS Code

No PlatformIO:

- Em `Project Tasks → env:waveshare-esp32-s3-amoled → General`, clique em **Upload**

### 6.3 Upload pela linha de comando

```bash
pio run -e waveshare-esp32-s3-amoled -t upload
```

---

## 7. Primeiro boot do Mini Lele

1. Insira o cartão microSD (já com a estrutura correta) na placa
2. Conecte a bateria (se for usar) ou apenas o cabo USB‑C
3. A placa irá:
   - Inicializar o PMU AXP2101
   - Ligar o display e periféricos
   - Carregar configurações do SD (ou criar novos arquivos padrão)
4. Na tela deve aparecer a interface inicial com o Pet **Mini Lele**

Se a tela permanecer preta, veja a seção de **Solução de Problemas** mais abaixo.

---

## 8. Acessando a WebUI

Por padrão, o Mini Lele inicia em **modo AP**:

- **SSID:** `Mini-Lele`
- **Senha:** `minilele`

Passos:

1. No seu celular ou computador, conecte‑se à rede Wi‑Fi `Mini-Lele`
2. Abra o navegador e acesse:
   - `http://192.168.4.1`
   - ou, quando mDNS estiver disponível: `http://minilele.local`
3. Login inicial padrão (se configurado no firmware):
   - Usuário: `admin`
   - Senha: `admin`  
     (recomendado alterar na primeira utilização)

A partir da WebUI você pode:

- Configurar modo STA (conectar em sua própria rede Wi‑Fi)
- Ajustar parâmetros do Pet, do sniffer, do Evil Portal, etc.
- Acompanhar logs em tempo real
- Gerenciar arquivos do cartão SD

Detalhes de uso: consulte `MANUAL.md`.

---

## 9. Monitor serial (logs de debug)

Para acompanhar o boot e verificar mensagens de erro:

### Pelo VS Code

1. Na barra inferior do PlatformIO, clique em **Monitor**
2. A velocidade padrão (baud rate) costuma ser `115200` (verifique em `platformio.ini`)

### Pela linha de comando

```bash
pio device monitor -e waveshare-esp32-s3-amoled
```

---

## 10. Solução de problemas

Alguns problemas comuns e como investigar:

### 10.1 Tela preta

- Verifique:
  - Cartão SD está inserido corretamente
  - Cabo USB‑C está em boa condição
  - Bateria (se usada) está carregada
- Tente:
  - Segurar o botão **PWR** (se houver) por ~10s para forçar reset do PMU AXP2101
  - Observar logs no monitor serial (mensagens de erro de display/PMU)
- Revise:
  - Se você alterou `pin_config.h` ou `platformio.ini`, reverta temporariamente para a versão padrão

### 10.2 Sem áudio

- Confirme:
  - Arquivos `.wav` estão nas pastas corretas (`arquivos_cartao_sd/tts` e arquivos de boot na raiz conforme `MISSING_ASSETS.md`)
  - Os arquivos são **16 kHz, 16‑bit, mono**
- Verifique no log se o codec ES8311 foi inicializado sem erros

### 10.3 SD não monta

- Teste com outro cartão
- Refaça a formatação em **FAT32**
- Crie novamente a estrutura com `generate_sd_structure.sh`
- Verifique no monitor serial se há mensagens de erro relacionadas ao SDMMC

### 10.4 Falha no upload de firmware

- Entre manualmente em modo de bootloader:
  1. Pressione e segure **BOOT**
  2. Aperte e solte **RESET**
  3. Solte **BOOT** após alguns segundos
- Confirme a porta correta no PlatformIO
- Reduza a `upload_speed` em `platformio.ini` se necessário

---

## 11. Próximos passos

Após instalação bem‑sucedida, leia:

- `MANUAL.md` – para aprender os comandos de voz, usar o sniffer, Evil Portal e WebUI
- `TUTORIAL.md` – primeiros passos rápidos com o Mini Lele
- `MISSING_ASSETS.md` – tudo o que falta no SD para ter a experiência completa (áudios, imagens, etc.)

Se encontrar algo faltando ou confuso, considere abrir uma issue ou PR ajudando a melhorar este guia. 🙂
