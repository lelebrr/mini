# Arquivos de Áudio para o SD Card

Este diretório descreve os arquivos de áudio que você deve colocar na raiz do seu cartão microSD para habilitar a interface de voz em Português (IA).

Recomendamos usar uma ferramenta de IA (como OpenAI TTS, ElevenLabs, ou Azure TTS) para gerar estes arquivos com uma voz natural em Português do Brasil.

## Formato Obrigatório
*   **Formato:** WAV (PCM)
*   **Taxa de Amostragem:** 16000 Hz (16kHz)
*   **Bits:** 16-bit
*   **Canais:** Mono ou Estéreo (Mono preferível para gravação, Estéreo OK para playback)

## Lista de Arquivos Necessários

| Nome do Arquivo | Conteúdo Sugerido (Texto para IA) | Descrição |
| :--- | :--- | :--- |
| `boot_pt.wav` | "Sistema Waveshare iniciado. Pronto para operar." | Tocado ao ligar. |
| `listening_pt.wav` | "Estou ouvindo, pode falar." | Tocado ao iniciar gravação. |
| `success_pt.wav` | "Comando recebido com sucesso." | Tocado após gravar. |
| `error_pt.wav` | "Desculpe, ocorreu um erro." | Tocado em caso de falha. |

## Instruções
1. Gere os áudios.
2. Converta-os para WAV 16kHz 16-bit (use Audacity ou `ffmpeg`).
3. Copie para a raiz do cartão SD.
