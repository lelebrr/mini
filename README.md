# Mini Lele

![Status](https://img.shields.io/badge/status-ativo-brightgreen)
![Versão](https://img.shields.io/badge/vers%C3%A3o-2.1.0-blue)
![Plataforma](https://img.shields.io/badge/plataforma-ESP32--S3-orange)
![Linguagem](https://img.shields.io/badge/linguagem-C%2B%2B%20%2F%20Arduino-informational)
![Licen%C3%A7a](https://img.shields.io/badge/licen%C3%A7a-GPLv3-important)

O **Mini Lele** é um Tamagotchi Hacker + plataforma de pentest Wi‑Fi totalmente em **Português BR**, pensado para o kit **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**.  
Ele combina:

- Pet virtual que evolui conforme você explora redes Wi‑Fi
- Ferramentas reais de análise e ataque (para uso legal e educacional)
- Interface Web completa (WebUI) com mais de 100 opções de configuração
- Comandos de voz offline em PT‑BR
- Design otimizado com LVGL e uso intensivo de PSRAM

---

## 🧭 Índice

- [Visão geral](#visão-geral)
- [Funcionalidades principais](#funcionalidades-principais)
- [Hardware suportado](#hardware-suportado)
- [Arquitetura do projeto](#arquitetura-do-projeto)
- [Instalação rápida](#instalação-rápida)
- [Modos de operação de rede](#modos-de-operação-de-rede)
- [Comandos de voz](#comandos-de-voz)
- [Estrutura do cartão SD](#estrutura-do-cartão-sd)
- [Documentação complementar](#documentação-complementar)
- [Contribuindo com o projeto](#contribuindo-com-o-projeto)
- [Aviso legal](#aviso-legal)

---

## Visão geral

- **Nome:** Mini Lele  
- **Versão:** 2.1 (God Tier — integrado, corrigido e otimizado)  
- **Autor original:** Jules (Agent) – adaptado e documentado para PT‑BR  
- **Placa alvo:** Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8  
- **Display:** 1.8" AMOLED, 368x448 (SH8601, interface QSPI)  
- **Foco:** Pentest Wi‑Fi, gamificação e aprendizado de segurança ofensiva

O Mini Lele foi pensado para quem quer:

- Estudar protocolos Wi‑Fi de forma divertida
- Ter um “companheiro hacker” de bolso
- Ter um laboratório portátil com WebUI, sniffer, portal cativo e upload de handshakes

---

## Funcionalidades principais

### 🐣 Tamagotchi Hacker

- Pet virtual que “se alimenta” de:
  - Handshakes WPA2/WPA3
  - Probes e atividade Wi‑Fi em geral
- Sistema de **XP e níveis**, com múltiplas evoluções (Egg → Baby → Kid → Teen → Adult → “Deus do Pwn”).
- Barras de:
  - Fome
  - Felicidade
  - Energia / Bateria
- Faces ASCII e/ou imagens customizadas que reagem ao seu estilo de uso.

### 🛰 Ferramentas de pentest Wi‑Fi

- Sniffer em modo promíscuo
  - Lista APs e clientes próximos
  - Gera arquivos `.pcap` no cartão SD para análise posterior
- Deauth / ataques ativos
  - Força reconexão de clientes para captura de handshakes
  - Pode ser disparado manualmente ou quando o Pet está “com muita fome”
- Evil Portal (Portal Cativo)
  - Pontos de acesso falsos com templates HTML personalizáveis
  - Registro de credenciais em arquivo de log no SD
- Captura e organização de handshakes
  - Diretório dedicado no SD
  - Integração com upload manual/automático para serviços de cracking (ex.: wpa-sec)

Sempre utilize apenas em redes **suas** ou com **autorização explícita**.

### 🌐 WebUI (Interface Web)

- Painel responsivo **100% autocontido** (sem depender de CDN): funciona
  totalmente **offline** no modo AP, com tema neon, medidores circulares,
  cartão de bateria/carga e animações.
- Mais de **100 configurações** em tempo real, agrupadas por categoria (Pet,
  Display, Energia/Bateria, Ataques/WiFi, Sistema, WebUI):
  - Nome do Pet, tema, brilho, economia de energia
  - Parâmetros de scan, filtros, potência de TX (quando suportado)
  - Controle do Evil Portal, listas de permissão/bloqueio (whitelist/blacklist)
- Gerenciador de arquivos do cartão SD:
  - Upload/Download de `.pcap`, logs, templates HTML e arquivos de configuração
- Logs ao vivo via WebSocket
- Controles remotos básicos (iniciar/pausar scans, mudar modo, reiniciar, etc.)

### 🗣 Voz offline (Português BR)

- Detecção offline de padrões sonoros simples
- Comandos curtos, otimizados para funcionamento sem internet
- Feedback por áudio (WAV 16 kHz) e animações na tela

### 🔋 Bateria e carregamento (USB‑C)

- Gerenciado por hardware pelo **PMU AXP2101** — basta conectar o cabo **USB‑C**
  para carregar a bateria LiPo.
- Otimizado para a célula **PL502030 250 mAh / 3,7 V**:
  - Corrente de carga: **100 mA** (~0,4C, seguro para 250 mAh; ajustável em 100/125/150/200 mA)
  - Tensão de corte: **4,2 V** (padrão LiPo)
  - Corrente de término: **25 mA** (~0,1C)
  - Detecção do pino **TS desligada** (a célula não tem termistor — obrigatório
    para o carregamento funcionar corretamente)
  - Desligamento de proteção em ~5% e aviso em ~10%
- Monitoramento em tempo real (na tela e na WebUI): porcentagem, tensão, estado
  de carga (CC/CV/completa), presença de USB‑C e autonomia estimada.
- Modo crítico automático (reduz clock e desliga o Wi‑Fi em bateria baixa), com
  saída automática do modo crítico quando o USB‑C é conectado.

> ⚠️ A corrente de carga acima de 125 mA (0,5C) não é recomendada para uma célula
> de 250 mAh. O padrão de 100 mA é o mais seguro.

### ⚙️ Performance e tecnologia

- Framework: **Arduino** para ESP32‑S3
- UI: **LVGL 9.x** (otimizada para PSRAM e double buffering)
- Otimizações de compilador (`-O3`) e uso cuidadoso de PSRAM
- Drivers específicos para:
  - Display SH8601 (QSPI)
  - Touch (FT3168 / GT1151)
  - Codec de áudio ES8311
  - PMU AXP2101
  - IMU QMI8658
  - Expansor I²C TCA9554

---

## Hardware suportado

Projeto desenhado para a placa:

- **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**
  - ESP32‑S3R8 (Dual‑Core, 240 MHz, 16 MB Flash, 8 MB PSRAM)
  - Tela AMOLED 1.8" 368x448 (SH8601, QSPI)
  - Touch capacitivo (FT3168 ou GT1151, I²C)
  - Codec de áudio ES8311 + microfone + amplificador
  - PMU AXP2101 com suporte a bateria LiPo 3.7 V
  - IMU QMI8658 (acel. + giroscópio)
  - Slot microSD (FAT32)

Detalhes completos de pinagem e chips: consulte `FULL_HARDWARE.md` e `HARDWARE.md`.

---

## Arquitetura do projeto

Principais pastas do repositório:

- `src/`
  - `main.cpp`: **integra todos os módulos** — inicializa hardware (expansor,
    PMU/carga, display, touch FT3168, IMU, SD, LVGL) e sobe a camada de
    aplicação (config, pet, gamificação, UI, áudio, WebUI) no `setup()`/`loop()`.
  - `core_singletons.cpp`: definições dos membros estáticos dos módulos.
  - `legacy/`: firmware antigo (não compilado — excluído via `build_src_filter`).
- `include/`
  - Cabeçalhos e lógica de alto nível
  - `pin_config.h`: mapeamento completo de pinos da placa
  - `core/`: lógicas de jogo, gamificação, configuração, etc.
  - `drivers/`: drivers de display, touch, PMU, áudio, IMU, SD, etc.
  - `web/`: handlers da WebUI, APIs HTTP/WebSocket e templates embutidos
- `lib/`
  - Bibliotecas externas ou adaptadas (ES8311, expansor de IO, etc.)
- `arquivos_cartao_sd/`
  - Estrutura base de arquivos para o cartão SD (evil_portal, tts, voice, etc.)
- `sd_card_files/`
  - Versões/estruturas antigas – mantenha o foco em `arquivos_cartao_sd/` na versão atual
- `generate_sd_structure.sh`
  - Script para gerar automaticamente a estrutura mínima do SD em `sd_out/`
- `platformio.ini`
  - Configuração completa do ambiente `env:waveshare-esp32-s3-amoled`

---

## Instalação rápida

Para detalhes completos, leia `INSTALACAO.md`.  
Abaixo, um resumo:

1. Pré‑requisitos
   - VS Code + extensão **PlatformIO IDE**
   - Python 3.x instalado
   - Cabo USB‑C de boa qualidade
   - **Internet na primeira compilação:** a maioria das bibliotecas é
     vendorizada em `lib/`, mas três dependências são baixadas automaticamente
     pelo PlatformIO no primeiro build (ficam em cache depois):
     `XPowersLib` (PMU/bateria), `AsyncTCP` e `ESPAsyncWebServer` (WebUI/Portal).
     O touch FT3168 usa um driver próprio interno (sem TouchLib).
2. Clonar o repositório
   ```bash
   git clone https://github.com/seu-usuario/mini-lele.git
   cd mini-lele
   ```
3. Preparar o cartão SD
   - Formate em **FAT32**
   - Rode:
     ```bash
     sh generate_sd_structure.sh
     ```
   - Copie o conteúdo de `sd_out/` para a raiz do cartão microSD
4. Compilar e gravar o firmware
   - Abra o projeto no VS Code
   - Selecione o ambiente `waveshare-esp32-s3-amoled`
   - Clique em:
     - `Build` (compilar)
     - `Upload` (gravar o firmware)
5. Primeiro boot
   - Insira o cartão SD na placa
   - Conecte a bateria ou USB‑C
   - Aguarde o boot do Mini Lele até aparecer a tela inicial do Pet

Problemas comuns e soluções detalhadas: veja `INSTALACAO.md`.

---

## Modos de operação de rede

O Mini Lele pode operar em dois modos principais:

### 1. Modo AP (padrão)

- Cria um ponto de acesso próprio:
  - **SSID:** `Mini-Lele`
  - **Senha:** `minilele`
- IP padrão: `192.168.4.1`
- Acesse no navegador:
  - `http://192.168.4.1`
  - ou (quando suportado) `http://minilele.local`

Este modo é ideal para configuração inicial e uso totalmente offline.

### 2. Modo STA (cliente Wi‑Fi)

- Configurável pela WebUI:
  - Informe SSID, senha e, opcionalmente, configurações avançadas
- O endereço IP obtido via DHCP é exibido na tela principal do Pet ou na aba de **Status** da WebUI
- Permite:
  - Upload automático/manual de handshakes
  - Atualizações e integrações que dependam de rede local

---

## Comandos de voz

Os comandos de voz são baseados em padrões de som simples (número de sílabas/pulsos), otimizados para funcionar offline.

Exemplos (podem variar conforme configuração):

- “**Ei Lele**” – acorda o sistema de escuta / ativa interação
- “**Status**” – o Mini Lele fala (e mostra) o estado atual (XP, bateria, etc.)
- “**Ataque**” – inicia um ciclo de varredura/ataque mais agressivo

Detalhes, ajustes finos e exemplos de arquivos de áudio estão descritos em `MANUAL.md` e `MISSING_ASSETS.md`.

---

## Estrutura do cartão SD

Resumo (detalhado em `MISSING_ASSETS.md`):

```text
/
├── arquivos_cartao_sd/
│   ├── evil_portal/              # Templates HTML do portal cativo
│   ├── tts/                      # Áudios TTS (WAV 16 kHz, 16-bit mono)
│   ├── voice/                    # Gravações de entrada / processamento
│   ├── wifi_config.txt           # Credenciais para upload (WPA-SEC etc.)
│   ├── macs_detectados.txt       # Log do sniffer Wi‑Fi
│   └── credenciais_capturadas.txt# Log de senhas do Evil Portal
├── capturas/                     # Handshakes e .pcap salvos
└── fila_envio/                   # Arquivos na fila de upload
```

Use `generate_sd_structure.sh` para criar essa estrutura automaticamente com placeholders.

---

## Documentação complementar

Toda a documentação foi organizada em PT‑BR:

- `INSTALACAO.md` – Guia passo a passo de instalação e preparação do ambiente
- `MANUAL.md` – Manual de uso do Mini Lele (voz, WebUI, sniffer, Evil Portal, upload, gamificação)
- `HARDWARE.md` – Resumo de hardware, pinagem principal e visão rápida
- `FULL_HARDWARE.md` – Documentação completa de hardware (pinos, chips, tensões, observações)
- `DISPLAY_GUIDE.md` – Guia detalhado de display AMOLED e touchscreen (drivers, sequências, debug)
- `MISSING_ASSETS.md` – Lista de assets que precisam estar no SD (áudio, imagens, templates)
- `README_PlatformIO.md` – Detalhes sobre o uso do PlatformIO com o Mini Lele
- `TUTORIAL.md` – “Primeiros passos” com o Mini Lele (para iniciantes)
- `CONTRIBUTING.md` – Como contribuir com o projeto

---

## Contribuindo com o projeto

O Mini Lele é um projeto comunitário.  
Você pode ajudar:

- Abrindo issues com bugs, ideias ou melhorias
- Enviando Pull Requests:
  - Novos templates de Evil Portal
  - Melhorias na WebUI
  - Novas animações/faces do Pet
  - Otimizações de performance ou estabilidade
- Melhorando a documentação e exemplos

Leia `CONTRIBUTING.md` para conhecer o fluxo sugerido de contribuição (branchs, estilo de código, testes, etc).

---

## ⚠️ Aviso legal

Este firmware é fornecido **exclusivamente** para:

- Estudos de segurança em redes **próprias**
- Ambientes de laboratório controlados
- Pesquisas e demonstrações educacionais

O uso de:

- Ataques de deautenticação,
- Captura de handshakes,
- Portais cativos para coleta de credenciais,

em redes ou dispositivos de terceiros **sem autorização explícita** é **ilegal** em diversos países.

Ao utilizar o Mini Lele, você concorda em:

- Respeitar a legislação local
- Não responsabilizar os autores, mantenedores ou contribuidores por qualquer uso indevido

Use com responsabilidade. Aprenda, ensine, compartilhe conhecimento – sem prejudicar outras pessoas.
