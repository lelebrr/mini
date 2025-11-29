# Arquivos e Assets Necessários (Cartão SD) – Mini Lele

Este documento lista todos os arquivos que o firmware **Mini Lele v2.0** espera encontrar no cartão **microSD** para funcionar completamente.  
O repositório contém o código‑fonte, mas a maior parte dos áudios, imagens e alguns arquivos de configuração devem ser gerados por você.

---

## 1. Estrutura de diretórios

Certifique‑se de que seu cartão SD (FAT32) tenha, na raiz, algo como:

```text
/
├── arquivos_cartao_sd/
│   ├── evil_portal/          # Templates HTML do portal cativo
│   ├── tts/                  # Áudios TTS (voz do Lele)
│   ├── voice/                # Buffer/gravações temporárias (criados em runtime)
│   ├── wifi_config.txt       # Configuração de rede p/ upload (opcional)
│   ├── macs_detectados.txt   # Log do sniffer (criado em runtime)
│   └── credenciais_capturadas.txt # Log do Evil Portal (criado em runtime)
├── capturas/                 # Handshakes e .pcap
├── fila_envio/               # Arquivos aguardando upload
└── custom-faces/             # (Opcional) Faces personalizadas do Pet
```

Você pode criar essa estrutura automaticamente rodando:

```bash
sh generate_sd_structure.sh
```

Isso criará um diretório `sd_out/` com a estrutura mínima a ser copiada para o SD.

---

## 2. Voz e áudio (TTS)

Estes arquivos são usados para feedback sonoro do Pet e para alguns eventos do sistema.  
Formato recomendado: **WAV 16 kHz, 16‑bit, Mono**.

| Caminho no SD                         | Descrição                  | Sugestão de texto para TTS            |
|--------------------------------------|----------------------------|---------------------------------------|
| `/arquivos_cartao_sd/tts/hello.wav`  | Saudação inicial           | “Olá, eu sou o Mini Lele!”           |
| `/arquivos_cartao_sd/tts/hungry.wav` | Fome                       | “Estou com muita fome.”              |
| `/arquivos_cartao_sd/tts/attack.wav` | Início de ataque           | “Iniciando ataque Wi‑Fi de teste.”   |
| `/arquivos_cartao_sd/tts/wakeup.wav` | Acordar                    | “Quem ousa me acordar?”              |
| `/arquivos_cartao_sd/tts/unknown.wav`| Comando não reconhecido    | “Não entendi o comando.”             |
| `/boot_pt.wav`                       | Áudio de boot do sistema   | “Sistema Mini Lele iniciado.”        |
| `/success_pt.wav`                    | Ação concluída com sucesso | “Comando executado com sucesso.”     |
| `/error_pt.wav`                      | Erro genérico              | “Ocorreu um erro. Verifique o sistema.” |

Notas importantes:

- Os arquivos na pasta `tts/` ficam dentro de `arquivos_cartao_sd/`.
- Arquivos como `/boot_pt.wav`, `/success_pt.wav` e `/error_pt.wav` ficam diretamente na **raiz** do cartão.
- O arquivo `/arquivos_cartao_sd/voice/input.wav` (ou similar) é normalmente criado e sobrescrito automaticamente pelo firmware – você não precisa criá‑lo.

---

## 3. Imagens e faces (opcional)

Por padrão, o Mini Lele pode usar **faces ASCII** (ex.: `(O_O)`, `(¬‿¬)`) diretamente na tela AMOLED.  
Se você quiser, também é possível usar imagens PNG, dependendo de como o firmware estiver configurado.

| Caminho no SD                | Resolução sugerida       | Descrição         |
|------------------------------|--------------------------|-------------------|
| `/custom-faces/HAPPY.png`    | 128x128 ou 240x240       | Face feliz        |
| `/custom-faces/SAD.png`      | 128x128 ou 240x240       | Face triste       |
| `/custom-faces/NEUTRAL.png`  | 128x128 ou 240x240       | Face neutra       |
| `/custom-faces/COOL.png`     | 128x128 ou 240x240       | Face “hackerman”  |

Recomendações:

- Use paleta que combine com o tema “cyberpunk”/“hacker” do Mini Lele.
- Mantenha o fundo preto para economizar energia no AMOLED.

---

## 4. Templates de Evil Portal (HTML)

O Evil Portal usa páginas HTML hospedadas no próprio Mini Lele.  
Esses arquivos ficam dentro de:

```text
/arquivos_cartao_sd/evil_portal/
```

Exemplos (podem variar conforme o repositório):

| Caminho no SD                                              | Descrição                    |
|------------------------------------------------------------|-----------------------------|
| `/arquivos_cartao_sd/evil_portal/01_wifi_update.html`      | Página fake de “atualização de Wi‑Fi” |
| `/arquivos_cartao_sd/evil_portal/02_feed_mini_lele.html`   | Página temática para “alimentar o Pet” |
| `/arquivos_cartao_sd/evil_portal/03_cafe_gratis.html`      | “Wi‑Fi grátis” de cafeteria (para testes) |

Você pode:

- Abrir os exemplos que vêm no repositório (dentro de `arquivos_cartao_sd/evil_portal`)
- Copiá‑los para o SD
- Criar seus próprios templates (sempre com foco educacional e sem copiar marcas/identidades reais de terceiros)

---

## 5. Arquivos de configuração e dados

Alguns arquivos são lidos e/ou gerados automaticamente pelo Mini Lele:

| Caminho no SD                             | Tipo      | Descrição                                                                 |
|-------------------------------------------|-----------|---------------------------------------------------------------------------|
| `/config.json`                            | JSON      | Configurações gerais do sistema (criado/atualizado em runtime)           |
| `/pwn_pet_save.json`                      | JSON      | Dados do Pet (XP, nível, estados) (criado em runtime)                    |
| `/game_stats.bin`                         | Binário   | Estatísticas de jogo e histórico (criado em runtime)                     |
| `/arquivos_cartao_sd/wifi_config.txt`     | Texto     | Credenciais de Wi‑Fi e KEY para upload de handshakes (criar manualmente) |
| `/arquivos_cartao_sd/credenciais_capturadas.txt` | Texto | Log de credenciais capturadas pelo Evil Portal (criado em runtime)       |
| `/arquivos_cartao_sd/macs_detectados.txt` | Texto     | Log dos MACs vistos pelo sniffer (criado em runtime)                     |

### 5.1 Exemplo de `wifi_config.txt`

```text
SSID=MinhaRedeWiFi
PASS=SenhaDaMinhaRede
KEY=MinhaKeyDoWpaSec
```

Esse arquivo é usado quando você ativa upload de handshakes para serviços como o `wpa-sec`.

---

## 6. Como gerar os arquivos

### 6.1 Áudio (TTS)

Você pode usar:

- Serviços online de TTS (text‑to‑speech), como:
  - ElevenLabs, OpenAI TTS, Google TTS, etc.
- Ferramentas locais:
  - `espeak`, `pico2wave`, `tts` com Python, etc.
- Softwares de edição de áudio:
  - Audacity, Reaper, etc. (para ajustar sample rate, bit depth e canal)

Passos gerais:

1. Gere o áudio no serviço/ferramenta escolhida
2. Converta para:
   - 16 kHz
   - 16‑bit
   - Mono
3. Salve com o nome correto na pasta adequada do SD.

### 6.2 Imagens

- Crie suas próprias faces em editores de imagem (GIMP, Krita, Aseprite, etc.)
- Mantenha:
  - Fundo preto (ideal para AMOLED)
  - Resolução dentro dos limites sugeridos (128x128 ou 240x240)
- Exporte em formato **PNG** (sem compressão exagerada, para facilitar parsing)

### 6.3 Estrutura do SD

Para evitar erros de digitação em caminhos e nomes:

1. Rode `generate_sd_structure.sh` (Linux/macOS)
2. Edite/insira seus arquivos dentro de `sd_out/`
3. Copie tudo para a raiz do cartão SD

---

## 7. Checklist rápido

Antes de ligar o Mini Lele, confirme:

- [ ] Cartão SD formatado em **FAT32**
- [ ] Pastas `arquivos_cartao_sd/`, `capturas/` e `fila_envio/` existem
- [ ] `wifi_config.txt` criado se você pretende usar upload online
- [ ] Arquivos de áudio básicos (`boot_pt.wav`, `success_pt.wav`, `error_pt.wav`) presentes
- [ ] (Opcional) Faces personalizadas em `custom-faces/`
- [ ] Templates de Evil Portal copiados para `arquivos_cartao_sd/evil_portal/`

Se algo estiver faltando, o firmware pode:

- Exibir erros na tela
- Registrar mensagens de erro no monitor serial
- Deixar alguma funcionalidade parcialmente desativada (por exemplo, voz sem TTS)

---

Com o SD preparado corretamente, o Mini Lele oferece toda a experiência de voz, gamificação, logs e portal cativo descrita em `README.md`, `MANUAL.md` e `TUTORIAL.md`. Se surgir alguma dúvida, esses arquivos são um bom complemento para este guia. 🙂
