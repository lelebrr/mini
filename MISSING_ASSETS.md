# Arquivos e Assets Faltantes (SD Card)

Este documento lista todos os arquivos que o firmware **Mini Lele v2.0** espera encontrar no cartão microSD para funcionar completamente. Como o repositório contém apenas código, você precisará gerar ou baixar esses assets.

## Estrutura de Diretórios
Certifique-se de que seu cartão SD tenha as seguintes pastas na raiz:

```
/
├── arquivos_cartao_sd/
│   ├── evil_portal/
│   ├── tts/
│   ├── voice/
│   ├── capturas/
│   └── fila_envio/
└── custom-faces/
```

## 1. Voz e Áudio (TTS)
Estes arquivos são usados para o feedback sonoro do Pet. Devem ser **WAV 16kHz 16-bit Mono**.

| Caminho no SD | Descrição | Sugestão de Conteúdo (Texto p/ IA) |
| :--- | :--- | :--- |
| `/arquivos_cartao_sd/tts/hello.wav` | Saudação | "Olá amigo!" |
| `/arquivos_cartao_sd/tts/hungry.wav` | Fome | "Estou com muita fome." |
| `/arquivos_cartao_sd/tts/attack.wav` | Ataque | "Iniciando ataque Wi-Fi." |
| `/arquivos_cartao_sd/tts/wakeup.wav` | Acordar | "Quem ousa me acordar?" |
| `/arquivos_cartao_sd/tts/unknown.wav` | Desconhecido | "Não entendi o comando." |
| `/boot_pt.wav` | Boot | "Sistema Mini Lele Iniciado." |
| `/success_pt.wav` | Sucesso | "Comando aceito." |
| `/error_pt.wav` | Erro | "Ocorreu um erro." |

> **Nota:** O arquivo `/voice/input.wav` é criado automaticamente pelo sistema durante a gravação. Não precisa criá-lo.

## 2. Imagens e Faces (Opcional)
O sistema usa faces ASCII por padrão (`(O_O)`), mas suporta PNGs se os arquivos existirem.

| Caminho no SD | Resolução | Descrição |
| :--- | :--- | :--- |
| `/custom-faces/HAPPY.png` | 128x128 ou 240x240 | Face Feliz |
| `/custom-faces/SAD.png` | 128x128 ou 240x240 | Face Triste |
| `/custom-faces/NEUTRAL.png` | 128x128 ou 240x240 | Face Neutra |
| `/custom-faces/COOL.png` | 128x128 ou 240x240 | Face PwnGod |

## 3. Evil Portal (Templates)
Páginas HTML para o ataque de Portal Cativo. O código já inclui alguns exemplos básicos na pasta `arquivos_cartao_sd` do repositório, mas você deve copiá-los para o SD.

| Caminho no SD | Descrição |
| :--- | :--- |
| `/arquivos_cartao_sd/evil_portal/01_wifi_update.html` | Fake Firmware Update |
| `/arquivos_cartao_sd/evil_portal/02_pwnagotchi_feed.html` | Alimentar Pet |
| ... outros HTMLs ... | Ver pasta do repo |

## 4. Configuração e Dados
Arquivos gerados ou lidos pelo sistema.

| Caminho no SD | Tipo | Descrição |
| :--- | :--- | :--- |
| `/config.json` | JSON | Configurações do sistema (Gerado auto no boot se não existir) |
| `/pwn_pet_save.json` | JSON | Save do Pet (XP, Nível) (Gerado auto) |
| `/game_stats.bin` | Binário | Estatísticas de Jogo (Gerado auto) |
| `/arquivos_cartao_sd/wifi_config.txt` | Texto | Credenciais para upload online (Crie manualmente: `SSID=... PASS=...`) |
| `/arquivos_cartao_sd/credenciais_capturadas.txt` | Texto | Log de senhas pegas no Evil Portal (Gerado auto) |
| `/arquivos_cartao_sd/macs_detectados.txt` | Texto | Log de MACs do Sniffer (Gerado auto) |

## Como Gerar os Arquivos
1.  **Áudio:** Use sites como ElevenLabs, OpenAI TTS ou ferramentas locais para gerar os WAVs.
2.  **Imagens:** Desenhe pixel art ou baixe assets de Pwnagotchi e converta para PNG.
3.  **Estrutura:** Rode o script `generate_sd_structure.sh` (Linux/Mac) para criar as pastas vazias e placeholders, depois substitua pelos arquivos reais.
