# Waveshare ESP32-S3-Touch-AMOLED-1.8 "Pwnagotchi Edition"

![Status](https://img.shields.io/badge/Status-100%25%20Complete-brightgreen)
![Language](https://img.shields.io/badge/Language-Portuguese%20BR-blue)
![Platform](https://img.shields.io/badge/Platform-ESP32--S3-orange)

Este repositório contém o firmware completo para transformar o **Waveshare ESP32-S3-Touch-AMOLED-1.8** em um assistente de segurança ofensiva (inspirado no Pwnagotchi), totalmente em **Português do Brasil**, com foco em operação Offline, Gamificação e Evil Portal.

## 🚀 Funcionalidades Principais

*   **Offline AI Voice:** Comandos de voz sem internet (reconhecimento de sílabas).
*   **Evil Portal:** Crie pontos de acesso falsos (Phishing) com 10 templates humorísticos.
*   **WiFi Sniffer:** Veja dispositivos próximos (Wall of Flippers Style) e salve logs.
*   **Gamificação:** Ganhe XP e suba de nível usando o dispositivo.
*   **Dashboard:** Monitoramento em tempo real de CPU, RAM, Bateria e Temperatura.
*   **Real Manual Upload:** Envie handshakes capturados diretamente para `wpa-sec.stanev.org`.
*   **Personalidade:** Rostos ASCII (Faces) que reagem às suas ações.

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
*   **Bateria:** LiPo 3.7V (Conector MX1.25).

## 📚 Documentação

Leia os guias abaixo para começar:

*   [📘 Guia de Instalação](INSTALACAO.md) - Como compilar e gravar.
*   [📖 Manual de Uso](MANUAL.md) - Como usar voz, portal, sniffer e upload.
*   [⚙️ Hardware e Pinos](HARDWARE.md) - Detalhes técnicos.

## ⚠️ Aviso Legal

Este projeto é para fins educacionais e de teste em redes próprias. O uso de "Evil Portal" ou captura de dados em redes de terceiros sem permissão é ilegal. Use com responsabilidade.
