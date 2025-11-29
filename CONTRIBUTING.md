# Guia de Contribuição – Mini Lele

Obrigado por querer contribuir com o **Mini Lele**!  
Este projeto é comunitário e tem como objetivo ser uma plataforma acessível para estudo de segurança ofensiva em Wi‑Fi, sempre com responsabilidade.

---

## 🎯 Objetivos do projeto

- Firmware estável para o kit **Waveshare ESP32‑S3‑Touch‑AMOLED‑1.8**
- Experiência 100% em **Português BR**
- Ferramentas de pentest Wi‑Fi com foco educacional
- Documentação clara e completa para quem está começando

Contribuições são bem‑vindas em:

- Código (C++/Arduino, LVGL, WebUI)
- Ajustes de hardware/drivers
- Templates de Evil Portal
- Melhorias de documentação e tutoriais

---

## 🚀 Fluxo básico para contribuir

1. Faça um fork do repositório no GitHub
2. Clone o seu fork localmente:
   ```bash
   git clone https://github.com/seu-usuario/mini-lele.git
   cd mini-lele
   ```
3. Crie uma branch descritiva:
   ```bash
   git checkout -b feature/minha-melhoria
   ```
4. Faça suas alterações (código, docs, assets de exemplo)
5. Rode build e testes básicos (veja seção de testes)
6. Faça commits claros:
   ```bash
   git commit -m "feat: adiciona novo template de evil portal"
   ```
7. Envie a branch:
   ```bash
   git push origin feature/minha-melhoria
   ```
8. Abra um Pull Request explicando:
   - O que foi mudado
   - Como testar
   - Se impacta usuários existentes

---

## 🧱 Organização do código

Resumo das pastas mais importantes:

- `src/`
  - Código principal do firmware (loop, inicialização, integração de módulos)
- `include/`
  - Cabeçalhos e lógicas principais
  - `core/`: lógica do Pet, gamificação, configuração
  - `drivers/`: display, touch, PMU, IMU, SD, áudio, etc.
  - `web/`: WebUI, endpoints HTTP/WebSocket
- `lib/`
  - Bibliotecas externas ou adaptadas
- `arquivos_cartao_sd/`
  - Exemplo de estrutura do cartão SD (templates, TTS, etc.)
- `*.md`
  - Documentação em PT‑BR (favor manter organizada e atualizada)

Antes de criar algo novo, procure se já existe algo semelhante em `include/core` ou `include/drivers`.

---

## ✍️ Estilo de código

- Linguagem principal: **C++ (Arduino)**, com alguns scripts em Python/Bash.
- Convenções sugeridas:
  - Classes: `PascalCase` (ex.: `AudioHandler`)
  - Métodos: `camelCase` (ex.: `startSniffer()`)
  - Variáveis locais: `snake_case` (ex.: `wifi_ssid`)
  - Constantes/defines: `ALL_CAPS` (ex.: `MAX_CLIENTS`)
- Comentários:
  - Preferencialmente em **Português BR**
  - Objetivos e contextos, não óbvios (evitar “somar a + b”)
- Organização:
  - Evite arquivos gigantes; prefira separar em módulos:
    - Lógica de jogo
    - Drivers de hardware
    - WebUI
    - Utilitários

---

## 🌐 Texto visível ao usuário

Tudo o que aparece:

- Na tela (LVGL/Display)
- Na WebUI
- Em logs principais
- Em áudios TTS

deve estar em **Português BR** por padrão.

Se precisar manter termos técnicos em inglês (ex.: “handshake”, “sniffer”), tente:

- Explicar rapidamente no texto
- Ou usar um termo misto: “sniffer (monitor de pacotes)”

---

## ✅ Testes antes de enviar

Sempre que possível:

1. Compile o projeto via PlatformIO:
   ```bash
   pio run -e waveshare-esp32-s3-amoled
   ```
2. Grave em uma placa real:
   ```bash
   pio run -e waveshare-esp32-s3-amoled -t upload
   ```
3. Verifique:
   - Boot completo sem travar
   - Tela inicial renderizando corretamente
   - WebUI acessível em modo AP (`Mini-Lele` / `minilele`)
   - Funções que você alterou (voz, sniffer, portal, etc.)

Se não tiver o hardware em mãos:

- Deixe isso claro no Pull Request
- Tente isolar as mudanças em partes óbvias (ex.: correções de texto, ajustes de layout, refactors simples)

---

## 🧪 Mudanças em drivers e hardware

Se a contribuição envolver:

- `pin_config.h`
- Drivers de display/touch/PMU/IMU/SD/áudio
- Particionamento (`partitions.csv`)
- `platformio.ini`

por favor:

1. Descreva no PR qual placa/revisão você está usando
2. Informe se a alteração:
   - Substitui o comportamento atual
   - Ou adiciona suporte opcional (ex.: outro layout de pinos)
3. Atualize a documentação relevante:
   - `HARDWARE.md`
   - `FULL_HARDWARE.md`
   - `DISPLAY_GUIDE.md`

---

## 🌐 WebUI e templates de Evil Portal

Contribuições muito bem-vindas:

- Novos templates HTML em `arquivos_cartao_sd/evil_portal/`
- Melhorias de UX na WebUI
- Textos mais claros/engraçados (sempre éticos)

Regras básicas:

- Nada de conteúdo malicioso real:
  - Sem phishing real direcionado a serviços específicos com logos/marcas oficiais
  - Use temas genéricos e educativos
- Não envie dados sensíveis reais dentro do repositório
- Mantenha tudo em PT‑BR ou com tradução visível

---

## 🧾 Commits, Issues e Pull Requests

- Mensagens de commit:
  - Use prefixos como `feat:`, `fix:`, `docs:`, `refactor:`, `chore:`
  - Ex.: `fix: corrige path de arquivos do evil portal`
- Issues:
  - Descreva o problema, o passo a passo para reproduzir e, se possível, logs
  - Informe versão aproximada do código (tag/commit)
- Pull Requests:
  - Explique o “porquê” da mudança, não só o “o quê”
  - Liste impactos esperados e se há migração de configuração/SD necessária

---

## 🤝 Código de conduta resumido

- Respeito sempre
- Nada de ataques pessoais ou linguagem ofensiva
- Discussões técnicas são bem‑vindas, desde que respeitosas
- Lembre-se: o foco é **aprender e ensinar segurança**, não prejudicar outras pessoas

---

## 🔐 Ética e segurança

Reforçando:

- Não submeta payloads maliciosos prontos para uso criminoso
- Não incentive o uso indevido do Mini Lele
- Casos de uso devem ser voltados para:
  - Laboratórios pessoais
  - Pesquisas acadêmicas
  - Demonstrações controladas

Se tiver dúvidas se algo é apropriado ou não, abra uma issue para discutir antes.

---

Obrigado por ajudar a deixar o Mini Lele mais completo, divertido e útil para a comunidade brasileira de segurança! 💻🐣
