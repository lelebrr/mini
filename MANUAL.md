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
4.  Se a vítima digitar a senha, ela será salva em `/captured_creds.txt` no SD.
5.  O Pwnagotchi ficará feliz `(^_^)`.

## 🕵️ Sniffer WiFi (Wall of Flippers)

1. Clique no botão **"Sniffer"**.
2. O dispositivo entra em modo promíscuo.
3. A tela exibirá uma lista em tempo real de endereços MAC próximos (dispositivos buscando WiFi).
4. Os dados são salvos em `/sniffed_macs.txt`.

## 📤 Manual Upload (WPA-SEC)

Para enviar handshakes capturados para quebra de senha online:

1.  Crie uma conta em [wpa-sec.stanev.org](https://wpa-sec.stanev.org).
2.  Copie sua **Key**.
3.  Edite o arquivo `wifi_config.txt` no cartão SD:
    ```
    SSID=SeuWiFi
    PASS=SuaSenha
    KEY=ColeSuaKeyAqui
    ```
4.  No dispositivo, clique em **"Upload"**.
5.  O sistema conectará ao WiFi e enviará todos os arquivos `.pcap` da pasta `/handshakes` e `/upload_queue`.

## 🎮 Gamificação

*   **XP:** Ganhe experiência usando voz e capturando redes.
*   **Nível:** Suba de nível para desbloquear... respeito do seu Pwnagotchi.
*   **Status:** Acompanhe seu progresso na tela principal.

## 🔋 Economia de Energia

*   **Screen Saver:** A tela desliga automaticamente após 30 segundos de inatividade.
*   **Acordar:** Toque na tela para ligar novamente.
