# Tutorial: Primeiros Passos com o Mini Lele

Este tutorial é um passo a passo rápido para você:

- Ligar o Mini Lele pela primeira vez
- Entender a tela inicial
- Acessar a WebUI
- Usar o modo de ataque (pentest) com segurança
- Aproveitar alguns truques úteis

Para detalhes mais completos, veja também `README.md`, `INSTALACAO.md` e `MANUAL.md`.

---

## 1. Primeiro contato com o Lele

Assim que você ligar a placa (com o cartão SD preparado e o firmware gravado), a tela deve mostrar:

- O rosto do **Lele**
- Barras de:
  - **Fome** (vermelha) – indica quanto ele precisa “comer” handshakes
  - **Felicidade** (verde) – sobe quando você interage com sucesso
- Informações de **nível** e **XP**, normalmente no rodapé ou canto da tela

Resumo visual:

- Pet com cara feliz → tudo bem
- Barras vazias → fome, tristeza ou pouca atividade
- Ícones de bateria → nível de carga atual

Se a tela ficar totalmente preta, veja a seção de solução de problemas em `INSTALACAO.md`.

---

## 2. Acessando a WebUI

A WebUI é o “painel de controle” do Mini Lele.

1. No seu celular ou computador, abra a lista de redes Wi‑Fi.
2. Conecte‑se à rede:

   - **SSID:** `Mini-Lele`
   - **Senha:** `minilele`

3. Abra o navegador (Chrome, Firefox, etc.).
4. Acesse:

   - `http://192.168.4.1`
   - ou `http://minilele.local` (quando mDNS estiver disponível)

5. Tela de login (padrão):

   - Usuário: `admin`
   - Senha: `admin` (recomenda‑se trocar na primeira vez)

Você deve ver a Dashboard com informações do Pet e da rede.

---

## 3. Configurando o Pet na WebUI

Na aba **Configurações** (ou similar, dependendo da versão da WebUI), você pode:

- Mudar o **nome** do Pet
- Ativar/desativar **voz** e sons
- Ajustar **modo de energia**:
  - **Eco:** economia máxima, menos scans / menor brilho
  - **Normal:** equilíbrio entre consumo e desempenho
  - **Turbo:** mais agressivo em scans, animações e efeitos

Outras opções comuns:

- Tema de cores (quando disponível)
- Intervalo e intensidade de scans Wi‑Fi
- Limites de atuação automática (por exemplo, quando usar Deauth)

---

## 4. Modo Ataque (Pentest)

O Mini Lele tem dois “estilos” de operação para fins de estudo:

### 4.1 Modo passivo

- O Lele fica “escutando” o ar:
  - Coleta beacons, probes e tráfego geral
  - Pode capturar handshakes que acontecem naturalmente
- Ideal para:
  - Observar redes ao redor
  - Ver como dispositivos se comportam ao procurar Wi‑Fi

### 4.2 Modo ativo

Neste modo, o Lele atua de forma mais “agressiva” (sempre em ambiente controlado):

- Pode enviar frames de **Deauth** para forçar reconexão de clientes
- Aumenta muito a chance de capturar handshakes

Para ativar:

- Pela tela, quando houver botão de “Ataque” ou similar; ou
- Pela WebUI, na aba de **Ferramentas / Pentest**

Também é comum poder acionar ataques de forma simplificada com comandos de voz, por exemplo:

- Falar “**Ataque**” perto do microfone (ver `MANUAL.md` para detalhes)

> Lembre‑se: use esse modo **apenas** em redes próprias ou ambientes de laboratório.

### 4.3 Evil Portal

O **Evil Portal** é o recurso em que o Mini Lele:

1. Cria uma rede Wi‑Fi aberta (ex.: `Cafe_Gratis_WiFi`)
2. Redireciona quem conecta para páginas HTML hospedadas no próprio dispositivo
3. Pode registrar credenciais em arquivos de log

Para ativar:

1. Vá até a aba de ferramentas na WebUI
2. Habilite o **Portal Cativo / Evil Portal**
3. Escolha um template da pasta:

   ```text
   /arquivos_cartao_sd/evil_portal/
   ```

4. Observe logs e arquivos em:

   ```text
   /arquivos_cartao_sd/credenciais_capturadas.txt
   ```

> Use sempre em cenários de demonstração/treino com pessoas cientes do que está acontecendo.

---

## 5. Interagindo por voz

Quando o módulo de voz está ativado e os áudios estão corretamente gravados no SD:

- Pressione o botão de **Voz** (se houver na UI) ou o atalho na WebUI
- Fale próximo ao microfone

Exemplos (dependem da configuração exata do firmware):

- “**Ei Lele**” → acorda o Pet / ativa modo de escuta
- “**Status**” → ele fala nível, fome, bateria, etc.
- “**Ataque**” → pode iniciar um ciclo de scan/ataques (se habilitado)

Se ele responder com algo como “Não entendi o comando”, verifique:

- Se os arquivos `.wav` estão no formato correto
- Se o microfone está funcionando
- Se o ambiente não está muito barulhento

---

## 6. Dicas úteis

- Se a **bateria** estiver muito baixa (ex.: abaixo de 10%):
  - O Lele pode entrar em modo de economia extrema (“modo zumbi”):
    - Tela simplificada
    - Pouco ou nenhum Wi‑Fi
- Se a **tela apagar** depois de um tempo:
  - É provavelmente o **screen saver** / economia de energia
  - Toque na tela ou use um botão para acender novamente
- Se algo parece não funcionar:
  - Confira `MISSING_ASSETS.md` para ver se não falta algum arquivo no SD
  - Veja `INSTALACAO.md` para revisar a preparação do ambiente

---

## 7. Próximos passos

Depois de brincar um pouco com o tutorial:

1. Leia o `MANUAL.md` para entender melhor todas as ferramentas
2. Abra `MISSING_ASSETS.md` para deixar o SD completamente equipado (áudios, templates, etc.)
3. Explore o código em `src/` e `include/` se quiser estudar como tudo foi implementado

E, se surgir alguma ideia de melhoria, você pode contribuir com o projeto – veja `CONTRIBUTING.md`.
