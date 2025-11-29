# Manual do Usuário – Mini Lele

Este manual explica como usar o **Mini Lele** no dia a dia:  
o Pet, a WebUI, as ferramentas de pentest e os recursos de voz e economia de energia.

---

## 1. Conhecendo o Pet (Lele)

O **Lele** é um Tamagotchi Hacker: ele “come” dados de redes Wi‑Fi.

### 1.1 Alimentação do Pet

O Lele ganha energia e experiência (XP) quando você:

- Captura **handshakes WPA2** – “comida padrão”
- Captura **handshakes WPA3** – iguaria rara, muito XP
- Observa **probes e tráfego Wi‑Fi** – snacks rápidos

Se você passar muito tempo sem gerar atividade, ele ficará:

- Com “fome” (barra vermelha)
- Menos feliz (barra de felicidade diminui)
- Potencialmente “mal‑humorado” (caras tristes na tela)

### 1.2 Evolução e níveis

Exemplo de progressão (pode variar conforme configuração):

- **Egg:** nível 0–5
- **Baby:** nível 5–10
- **Kid:** nível 10–20
- **Teen:** nível 20–30
- **Adult:** nível 30–50
- **Lendário:** nível 50+
- **Forma secreta:** após um grande número de handshakes (alvo: 10.000+)

Quanto mais você usa as ferramentas de pentest (de forma legal!), mais o Lele evolui.

---

## 2. WebUI (Interface Web)

A WebUI é o painel de controle completo do Mini Lele.

### 2.1 Acesso

1. Conecte‑se à rede Wi‑Fi criada pelo Mini Lele:
   - **SSID:** `Mini-Lele`
   - **Senha:** `minilele`
2. Abra o navegador e acesse:
   - `http://192.168.4.1`
   - ou `http://minilele.local` (quando mDNS estiver disponível)
3. Login padrão (se habilitado no firmware):
   - **Usuário:** `admin`
   - **Senha:** `admin`  
     (recomendado alterar na primeira utilização)

### 2.2 Abas principais

- **Dashboard**
  - Mostra status atual:
    - Nível, XP, fome, felicidade
    - Bateria, temperatura, uso de memória
    - Estado da rede (AP/STA), IP, etc.
- **Configurações**
  - Ajuste fino do comportamento:
    - Nome do Pet
    - Idioma (mantendo PT‑BR como base)
    - Modo de energia (Eco, Normal, Turbo)
    - Parâmetros de scan, filtros, potência (quando aplicável)
- **Ferramentas / Pentest**
  - Controles para:
    - Sniffer
    - Deauth / ataques ativos
    - Evil Portal
    - Upload de handshakes
- **Arquivos**
  - Gerenciador de arquivos do cartão SD:
    - Download de `.pcap`, logs e configs
    - Upload de templates de portal e outros assets
- **Logs**
  - Logs de sistema em tempo real (via WebSocket quando disponível)
  - Útil para debug de problemas

---

## 3. Ferramentas de Pentest

⚠️ **Atenção:** Sempre utilize APENAS em redes suas ou com autorização explícita.

### 3.1 Sniffer Wi‑Fi

O **Sniffer** permite ver dispositivos próximos e capturar tráfego relevante.

- Ativação:
  - Pelo botão correspondente na tela
  - Pela aba de Ferramentas na WebUI
- Quando ativo:
  - A tela exibe endereços MAC e informações básicas
  - O sistema salva capturas em arquivos `.pcap` na pasta:

```text
/capturas/
```

- Log simples de MACs observados:
  - `arquivos_cartao_sd/macs_detectados.txt`

### 3.2 Ataques de Deauth

O Lele pode, em modo laboratório, enviar quadros de deautenticação:

- Finalidade:
  - Forçar clientes a se reconectarem
  - Aumentar a chance de capturar o handshake
- Ativação:
  - Manual, via WebUI
  - Ou automaticamente, se o Pet estiver “com muita fome” (configurável)
- Recomendações:
  - Use em ambiente isolado
  - Nunca em redes de terceiros sem permissão

### 3.3 Evil Portal (Portal Cativo)

O **Evil Portal** simula uma rede Wi‑Fi aberta com páginas de login falsas (educacionais).

1. Ative o modo **Evil Portal** na WebUI ou pelo botão dedicado na interface do dispositivo.
2. O Mini Lele cria uma rede Wi‑Fi aberta (ex.: `Cafe_Gratis_WiFi`).
3. Ao conectar, o usuário é redirecionado para um template HTML da pasta:

```text
/arquivos_cartao_sd/evil_portal/
```

4. Se a vítima (em ambiente de teste controlado) digitar credenciais, elas são salvas em:

```text
/arquivos_cartao_sd/credenciais_capturadas.txt
```

> Esses templates são apenas para fins educacionais.  
> Não utilize para roubar contas de serviços reais ou de terceiros.

---

## 4. Upload de Handshakes (WPA‑SEC e afins)

O Mini Lele pode organizar e enviar handshakes para serviços de cracking online (ex.: `wpa-sec.stanev.org`).

### 4.1 Fluxo básico

1. Crie uma conta em  
   https://wpa-sec.stanev.org  
   e copie sua **KEY**.
2. Edite o arquivo `wifi_config.txt` no SD:

```text
/arquivos_cartao_sd/wifi_config.txt
```

Conteúdo de exemplo:

```text
SSID=RedeParaUpload
PASS=SenhaDaRede
KEY=SuaKeyDoWpaSec
```

- `SSID` e `PASS`: rede Wi‑Fi que o Mini Lele usará para enviar arquivos
- `KEY`: chave fornecida pelo wpa‑sec

3. No dispositivo, acione **Upload**:
   - Via WebUI (botão Upload)
   - Ou via atalho na interface principal (se implementado no firmware)
4. O Mini Lele:
   - Tenta conectar à rede definida em `wifi_config.txt`
   - Envia os arquivos `.pcap` contidos em:

```text
/capturas/
```

   - Pode mover arquivos enviados para:

```text
/fila_envio/
```

   (exatamente como o firmware estiver programado)

---

## 5. Comandos de voz (offline)

O Mini Lele possui reconhecimento simples de padrões de áudio, sem depender de internet.

### 5.1 Como usar

1. Pressione o botão de **Voz** na interface (ou ative via WebUI, se disponível).
2. Fale próximo ao microfone.
3. O sistema analisa a energia sonora / número de “pulsos” (sílabas simples), e:
   - Mapeia para comandos básicos (1 som, 2 sons, 3 sons, etc.)
   - Opcionalmente toca respostas gravadas em arquivos WAV do SD

### 5.2 Exemplos de mapeamento

| Padrão de som      | Ação típica                 | Exemplo de palavra  |
|--------------------|----------------------------|----------------------|
| 1 som              | Alternar tela/lanterna     | “Luz”, “Oi”         |
| 2 sons             | Relatório de status        | “Status”, “Le‑le”   |
| 3 sons             | Falar nível de bateria     | “Bateria”, “Energia”|

Além disso, comandos “fraseados” podem ser usados como gatilhos, por exemplo:

- “**Ei Lele**” – acordar atenção do Pet
- “**Status**” – forçar resumo falado do estado atual
- “**Ataque**” – iniciar um ciclo de scan/ataques (quando permitido na config)

> Os arquivos de áudio usados nas respostas ficam em `/arquivos_cartao_sd/tts/`.  
> Veja `MISSING_ASSETS.md` para a lista completa.

---

## 6. Gamificação

O Mini Lele registra e mostra o seu progresso de uso.

- **XP:** ganho ao:
  - Capturar handshakes
  - Usar comandos de voz
  - Manter o dispositivo ativo em modo de “exploração”
- **Nível:** indica o “quão hacker” seu Lele se tornou:
  - Níveis baixos: comportamento mais simples
  - Níveis altos: animações mais “confiante” e reações extras
- **Status:** exibido:
  - Na tela principal do dispositivo
  - Na aba de Dashboard da WebUI

---

## 7. Economia de energia e modos de sono

Para não acabar com a bateria rapidamente, o Mini Lele conta com mecanismos automáticos.

- **Screen Saver:**
  - Após alguns segundos de inatividade (ex.: 30s), a tela escurece ou desliga
  - Toque na tela ou use um botão para acordar
- **Sono leve / Deep Sleep:**
  - Em períodos longos sem atividade, o dispositivo pode entrar em modos de menor consumo
  - O acordar pode ocorrer por:
    - Toque na tela
    - Movimento detectado pela IMU
    - Botão físico

> A lógica exata pode variar conforme a versão do firmware.  
> Consulte `INSTALACAO.md` e `README.md` para detalhes mais técnicos sobre PMU e modos de energia.

---

## 8. Bateria e carregamento

- O AXP2101 gerencia:
  - Carregamento da bateria Li‑Po
  - Proteções básicas (sobrecarga, etc.)
- Recomendações:
  - Use baterias de qualidade conhecida
  - Não deixe o dispositivo em ambientes muito quentes
  - Se a bateria estiver muito baixa:
    - O Mini Lele pode reduzir brilho, desativar Wi‑Fi ou entrar em modo “zumbi” para se proteger

---

## 9. Boas práticas e ética

- Use o Mini Lele apenas em:
  - Redes suas
  - Ambientes de teste controlados
  - Laboratórios educacionais
- Nunca:
  - Ataque redes de terceiros sem permissão
  - Use o Evil Portal para roubar credenciais reais
- Se estiver ensinando outras pessoas:
  - Reforce sempre a importância da ética e da legislação local

---

Se algo não estiver claro, consulte também:

- `README.md` – visão geral do projeto
- `INSTALACAO.md` – detalhes de instalação e preparação do SD
- `MISSING_ASSETS.md` – lista de arquivos de áudio/imagens/templates
- `TUTORIAL.md` – primeiros passos práticos com o Mini Lele
