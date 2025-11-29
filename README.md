<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
# Mini Lele v2.0

## Visão Geral
O **Mini Lele** é o dispositivo definitivo de pentest + tamagotchi, projetado exclusivamente para o hardware Waveshare ESP32-S3-Touch-AMOLED-1.8.

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
   - SSID: `Mini-Lele`
   - Senha: `minilele`
   - Acesse: `http://192.168.4.1`

2. **Modo STA (Cliente):**
   - Configure via WebUI.
   - O IP aparecerá na tela do Pet.

### Comandos de Voz
- "Ei Lele" (2 sílabas fortes): Acorda/Ouve
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
=======
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
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
