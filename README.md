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
