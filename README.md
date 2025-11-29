<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
# Mini Lele v2.0

## Visão Geral
O **Mini Lele** é o dispositivo definitivo de pentest + tamagotchi, projetado exclusivamente para o hardware Waveshare ESP32-S3-Touch-AMOLED-1.8.
=======
# PwnTamagotchi BR v2.0

## Visão Geral
O **PwnTamagotchi BR** é o dispositivo definitivo de pentest + tamagotchi, projetado exclusivamente para o hardware Waveshare ESP32-S3-Touch-AMOLED-1.8.
>>>>>>> origin/pwntamagotchi-br-v2-webui-final

**Versão:** 2.0 (God Tier)
**Hardware:** Waveshare ESP32-S3 AMOLED 1.8" (SH8601/ST7701S + GT1151)
**Autor:** Jules (Agent)

## Funcionalidades (120+100)
- **Gameplay:** 6 Evoluções, Missões Diárias, Troféus, Voz Offline.
- **Pentest:** Sniffer, Deauth Injection, Evil Portal, Handshake Capture, Beacon Spam.
- **WebUI:** Interface completa (100 configs) responsiva, logs ao vivo, gerenciador de arquivos.
- **Performance:** LVGL 9.2, -O3, DMA2D Simulado, PSRAM Double Buffer.
- **Design:** Tema Cyber-Favela, Matrix Rain, Animações Fluídas.

## Instalação

### Requisitos
- VS Code + PlatformIO
- Python 3.x
- ESP-IDF v5.x (Gerenciado pelo PIO)

### Passos
1. Clone este repositório.
2. Abra no VS Code.
3. Conecte o dispositivo via USB-C.
4. Execute o Upload: `PlatformIO: Upload`
5. Execute o Upload do Filesystem (opcional, assets gerados em código): `PlatformIO: Upload Filesystem Image`

## Uso Básico

### Modos de Operação
1. **Modo AP (Padrão):**
<<<<<<< HEAD
   - SSID: `Mini-Lele`
   - Senha: `minilele`
=======
   - SSID: `PwnTamagotchi-BR`
   - Senha: `pwnme123`
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
   - Acesse: `http://192.168.4.1`

2. **Modo STA (Cliente):**
   - Configure via WebUI.
   - O IP aparecerá na tela do Pet.

### Comandos de Voz
<<<<<<< HEAD
- "Ei Lele" (2 sílabas fortes): Acorda/Ouve
=======
- "Ei Pwn" (2 sílabas fortes): Acorda/Ouve
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
- "Status" (2 sílabas): Fala status
- "Ataque" (3 sílabas): Inicia scan agressivo

## Estrutura de Pastas
- `src/`: Código fonte principal (`main.cpp`, handlers).
- `include/core/`: Lógica de negócio (Pet, Attack, Power, Config).
- `include/web/`: Servidor Web e Assets.
- `include/drivers/`: Drivers de hardware customizados.
- `lib/`: Drivers de terceiros/complexos (ES8311).
- `data/`: Arquivos para upload no SPIFFS/LittleFS (se usado).

## WebUI
A interface web permite controlar **100 configurações** em tempo real, incluindo:
- Upload/Download de arquivos do SD.
- Streaming de logs ao vivo via WebSocket.
- Controle remoto do Pet.
- Configuração de ataques e whitelist/blacklist.

Divirta-se hackeando o planeta (com responsabilidade)!
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
# PwnTamagotchi BR (ESP32-S3 AMOLED)

Projeto completo de Pentest Gamificado para Waveshare ESP32-S3 AMOLED.

## 🌟 Funcionalidades
1. **Tamagotchi Hacker:** Evolua de Ovo para PwnGod capturando handshakes.
2. **Pentest:** Sniffer, Deauth (simulado/frame), Evil Portal.
3. **Voz Offline:** Comandos em PT-BR ("Ataca", "Status").
4. **Hardware:** Suporte total a Tela AMOLED, Touch, SD, Power.

## 🛠 Instalação
1. Formate SD em FAT32.
2. Rode `sh generate_sd_structure.sh` e copie `sd_out/*` para o SD.
3. Gere arquivos WAV 16kHz para a pasta `tts/`.
4. Compile e Grave via PlatformIO.

## 🎮 Como Jogar
- **Fome:** Cai com o tempo. Alimente capturando redes WiFi.
- **Felicidade:** Aumenta interagindo (Voz/Touch).
- **Evolução:** Ganhe XP. Lvl 50 = PwnLord.

Divirta-se!
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
# Waveshare ESP32-S3-Touch-AMOLED-1.8 "Pwnagotchi Edition"

![Status](https://img.shields.io/badge/Status-100%25%20Complete-brightgreen)
=======
# Waveshare ESP32-S3-Touch-AMOLED-1.8 "Pwnagotchi Edition"

![Status](https://img.shields.io/badge/Status-Functional-brightgreen)
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
# Waveshare ESP32-S3-Touch-AMOLED-1.8 "Pwnagotchi Edition"

![Status](https://img.shields.io/badge/Status-100%25%20Complete-brightgreen)
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
# Waveshare ESP32-S3-Touch-AMOLED-1.8 "Pwnagotchi Edition"

![Status](https://img.shields.io/badge/Status-100%25%20Complete-brightgreen)
>>>>>>> origin/waveshare-s3-amoled-full-review-406
![Language](https://img.shields.io/badge/Language-Portuguese%20BR-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32--S3-orange)

Este repositório contém o firmware completo para transformar o **Waveshare ESP32-S3-Touch-AMOLED-1.8** em um assistente de segurança ofensiva (inspirado no Pwnagotchi), totalmente em **Português do Brasil**, com foco em operação Offline, Gamificação e Evil Portal.

## 🚀 Funcionalidades Principais

*   **Offline AI Voice:** Comandos de voz sem internet (reconhecimento de sílabas).
*   **Evil Portal:** Crie pontos de acesso falsos (Phishing) com 10 templates humorísticos.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
*   **WiFi Sniffer:** Veja dispositivos próximos (Wall of Flippers Style) e salve logs.
*   **Gamificação:** Ganhe XP e suba de nível usando o dispositivo.
*   **Dashboard:** Monitoramento em tempo real de CPU, RAM, Bateria e Temperatura.
*   **Real Manual Upload:** Envie handshakes capturados diretamente para `wpa-sec.stanev.org`.
*   **Personalidade:** Rostos ASCII (Faces) que reagem às suas ações.

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
## 📂 Estrutura de Arquivos (Cartão SD)

O cartão SD deve estar formatado em **FAT32** e conter a seguinte estrutura na raiz:

```
/
├── arquivos_cartao_sd/     (Assets do sistema)
│   ├── evil_portal/        (Templates HTML)
│   ├── boot_pt.wav         (Áudio de boot)
│   ├── listening_pt.wav    (Áudio de escuta)
│   ├── success_pt.wav      (Áudio de sucesso)
│   ├── error_pt.wav        (Áudio de erro)
│   ├── wifi_config.txt     (Credenciais para Upload)
│   ├── macs_detectados.txt (Log do Sniffer)
│   └── credenciais_capturadas.txt (Logs do Portal)
├── capturas/               (Handshakes .pcap salvos)
└── fila_envio/             (Arquivos prontos para upload)
```

## 🛠 Hardware Necessário

*   **Placa:** Waveshare ESP32-S3-Touch-AMOLED-1.8
*   **Cartão SD:** MicroSD (FAT32).
<<<<<<< HEAD
=======
*   **Gamificação:** Ganhe XP e suba de nível usando o dispositivo.
*   **Dashboard:** Monitoramento em tempo real de CPU, RAM, Bateria e Temperatura.
*   **Manual Upload:** Salve handshakes e envie manualmente quando tiver WiFi.
*   **Personalidade:** Rostos ASCII (Faces) que reagem às suas ações.

=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
## 🛠 Hardware Necessário

*   **Placa:** Waveshare ESP32-S3-Touch-AMOLED-1.8
*   **Cartão SD:** MicroSD (FAT32) para armazenar logs, áudios e templates.
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
*   **Bateria:** LiPo 3.7V (Conector MX1.25).

## 📚 Documentação

Leia os guias abaixo para começar:

*   [📘 Guia de Instalação](INSTALACAO.md) - Como compilar e gravar.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
*   [📖 Manual de Uso](MANUAL.md) - Como usar voz, portal, sniffer e upload.
=======
*   [📖 Manual de Uso](MANUAL.md) - Como usar voz, portal e ferramentas.
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
*   [📖 Manual de Uso](MANUAL.md) - Como usar voz, portal, sniffer e upload.
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
*   [📖 Manual de Uso](MANUAL.md) - Como usar voz, portal, sniffer e upload.
>>>>>>> origin/waveshare-s3-amoled-full-review-406
*   [⚙️ Hardware e Pinos](HARDWARE.md) - Detalhes técnicos.

## ⚠️ Aviso Legal

Este projeto é para fins educacionais e de teste em redes próprias. O uso de "Evil Portal" ou captura de dados em redes de terceiros sem permissão é ilegal. Use com responsabilidade.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
=======
>>>>>>> origin/waveshare-s3-amoled-final-polish
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
