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
# Manual do Usuário - Mini Lele

## 1. O Pet (Lele)
=======
# Manual do Usuário - PwnTamagotchi BR

## 1. O Pet (PwnPet)
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
O seu Tamagotchi vive de dados. Ele precisa de **handshakes** WiFi para comer e evoluir.

### Alimentação
- **Handshakes WPA2:** Comida padrão.
- **Handshakes WPA3:** Iguaria gourmet (muito XP).
- **Probes:** Snacks rápidos.

### Evolução
- **Egg:** Nível 0-5
- **Baby:** Nível 5-10
- **Kid:** Nível 10-20
- **Teen:** Nível 20-30
- **Adult:** Nível 30-50
- **PwnLord:** Nível 50+
- **PwnGod:** (Secreto) 10.000 Handshakes capturados.

## 2. Ferramentas de Pentest
⚠️ **Aviso:** Use apenas em redes que você possui permissão!

### Sniffer
Coleta pacotes passivamente. Detecta clientes e APs próximos.
- Salva `.pcap` no SD Card automaticamente.

### Deauth (Ataque)
Desconecta clientes de um AP alvo para forçar reconexão e capturar o handshake.
- Ativado automaticamente se o Pet estiver com muita fome (Fome > 80%).

### Evil Portal
Cria um Ponto de Acesso falso para capturar credenciais.
- Templates disponíveis: Google, Facebook, Café Grátis.
- Logs salvos em `/sdcard/credenciais.txt`.

## 3. Web Interface
Acesse via navegador para controle total.

### Login Padrão
- **Usuário:** admin
- **Senha:** admin (Altere imediatamente na aba Sistema!)

### Abas
- **Dashboard:** Status ao vivo.
- **Configurações:** 100 opções editáveis.
- **Arquivos:** Gerencie o Cartão SD.
- **Logs:** Debug em tempo real.

## 4. Hardware e Bateria
- **Deep Sleep:** O dispositivo dorme automaticamente se inativo. Agite para acordar.
- **Siesta:** A cada hora cheia, o Pet tira uma soneca de 3 min.
- **Carregamento:** Use USB-C. O LED indica carga.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
# 📖 Manual de Uso

## 🗣️ Comandos de Voz (Offline)

Clique no botão **"Voz"** e fale próximo ao microfone. O sistema detecta o número de sílabas/palavras.

| Comando (Sons) | Ação | Exemplo |
| :--- | :--- | :--- |
| **1 Som** | Alternar Lanterna/Tela | "Luz", "Sim", "Pai" |
| **2 Sons** | Relatório de Status | "Status", "Ola", "Teste" |
| **3 Sons** | Nível de Bateria | "Bateria", "Energia" |

*Nota: Fale pausadamente. O sistema usa energia sonora, não reconhecimento de palavras complexas.*

## 😈 Evil Portal

1.  Clique no botão vermelho **"EVIL PORTAL"**.
2.  O dispositivo criará uma rede Wi-Fi aberta (Ex: "Cafe_Gratis_WiFi").
3.  Qualquer pessoa que conectar será redirecionada para a página falsa (Template).
4.  Se a vítima digitar a senha, ela será salva em `/arquivos_cartao_sd/credenciais_capturadas.txt`.
5.  O Pwnagotchi ficará feliz `(^_^)`.

## 🕵️ Sniffer WiFi (Wall of Flippers)

1. Clique no botão **"Sniffer"**.
2. O dispositivo entra em modo promíscuo.
3. A tela exibirá uma lista em tempo real de endereços MAC próximos (dispositivos buscando WiFi).
4. Os dados são salvos em `/arquivos_cartao_sd/macs_detectados.txt`.

## 📤 Manual Upload (WPA-SEC)

Para enviar handshakes capturados para quebra de senha online:

1.  Crie uma conta em [wpa-sec.stanev.org](https://wpa-sec.stanev.org).
2.  Copie sua **Key**.
3.  Edite o arquivo `/arquivos_cartao_sd/wifi_config.txt` no cartão SD:
    ```
    SSID=SeuWiFi
    PASS=SuaSenha
    KEY=ColeSuaKeyAqui
    ```
4.  No dispositivo, clique em **"Upload"**.
5.  O sistema conectará ao WiFi e enviará todos os arquivos `.pcap` da pasta `/capturas` e `/fila_envio`.

## 🎮 Gamificação

*   **XP:** Ganhe experiência usando voz e capturando redes.
*   **Nível:** Suba de nível para desbloquear... respeito do seu Pwnagotchi.
*   **Status:** Acompanhe seu progresso na tela principal.

## 🔋 Economia de Energia

*   **Screen Saver:** A tela desliga automaticamente após 30 segundos de inatividade.
*   **Acordar:** Toque na tela para ligar novamente.
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
