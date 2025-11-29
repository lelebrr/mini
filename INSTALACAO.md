<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
# Guia de Instalação - Mini Lele v2.0

## Requisitos de Sistema
*   **Hardware:** Waveshare ESP32-S3-Touch-AMOLED-1.8
*   **Computador:** Windows, Linux ou macOS
*   **Software:** VS Code com extensão PlatformIO

## Passo 1: Preparação do Ambiente
1.  Instale o [VS Code](https://code.visualstudio.com/).
2.  Instale a extensão **PlatformIO IDE** no VS Code.
3.  Instale o [Python 3.x](https://www.python.org/).

## Passo 2: Clonando o Repositório
```bash
git clone https://github.com/seu-usuario/mini-lele.git
cd mini-lele
```

## Passo 3: Compilação e Upload
1.  Abra a pasta do projeto no VS Code.
2.  Aguarde o PlatformIO baixar as dependências (pode demorar alguns minutos).
3.  Conecte o dispositivo via cabo USB-C.
4.  Clique no ícone do PlatformIO (cabeça de alien) na barra lateral.
5.  Em **Project Tasks**, selecione `env:waveshare-esp32-s3-amoled` -> **General** -> **Upload**.

## Passo 4: Preparação do Cartão SD
O sistema requer um cartão microSD (FAT32) para salvar configurações, capturas e assets.

1.  Formate o cartão SD em FAT32.
2.  Copie todo o conteúdo da pasta `arquivos_cartao_sd/` para a raiz do cartão.
3.  Insira o cartão no dispositivo.

## Passo 5: Primeiro Boot
1.  Reinicie o dispositivo.
2.  A tela deve ligar com o logo "Mini Lele".
3.  Conecte-se ao WiFi `Mini-Lele` (Senha: `minilele`).
4.  Acesse `http://192.168.4.1` ou `http://minilele.local` para configurar.

## Solução de Problemas
*   **Tela preta:** Verifique se o cartão SD está inserido corretamente.
*   **Bootloop:** Verifique se a partição foi gravada corretamente (use `Upload Filesystem Image` se necessário, embora este projeto use WebAssets embutidos).
*   **Falha no Upload:** Segure o botão BOOT, aperte RESET, solte BOOT para entrar no modo download.
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
# 📘 Guia de Instalação

## 1. Preparação do Ambiente

Você precisará do **PlatformIO** (extensão para VSCode).

1.  Instale o [VSCode](https://code.visualstudio.com/).
2.  Instale a extensão "PlatformIO IDE".
3.  Clone este repositório.

## 2. Preparação do Cartão SD

O cartão SD é **obrigatório**. Formate-o em **FAT32**.

Crie a seguinte estrutura de pastas na raiz do SD:

```
/
<<<<<<< HEAD
├── arquivos_cartao_sd/     (Copie o conteúdo desta pasta do repo para a raiz)
=======
├── sd_card_files/          (Copie o conteúdo desta pasta do repo para a raiz)
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
│   ├── evil_portal/        (Templates HTML)
│   ├── boot_pt.wav         (Áudios)
│   ├── listening_pt.wav
│   ├── success_pt.wav
│   └── error_pt.wav
<<<<<<< HEAD
│   └── wifi_config.txt     (Edite com suas credenciais)
├── capturas/               (Será criada automaticamente)
└── fila_envio/             (Será criada automaticamente)
=======
├── handshakes/             (Será criada automaticamente)
├── upload_queue/           (Será criada automaticamente)
├── wifi_config.txt         (Crie este arquivo com suas credenciais WiFi para upload)
└── captured_creds.txt      (Onde as senhas do Evil Portal serão salvas)
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
```

**Exemplo de `wifi_config.txt`:**
```
SSID=MinhaCasa
PASS=MinhaSenha123
KEY=SuaChaveWpaSec
```

## 3. Compilação e Upload

1.  Conecte a placa via USB-C (segure o botão BOOT se necessário).
2.  No PlatformIO, clique em `Build` (ícone de check).
3.  Clique em `Upload` (ícone de seta).
4.  Abra o `Serial Monitor` (115200) para ver o log de boot.

## 4. Troubleshooting

<<<<<<< HEAD
*   **Tela preta?** Verifique bateria e cabo USB. O PMU (AXP2101) pode precisar de reset (segure PWR 10s).
*   **Sem áudio?** Verifique se os arquivos `.wav` estão na pasta correta (`arquivos_cartao_sd`).
*   **SD não monta?** Tente outro cartão ou formate novamente (FAT32).
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> origin/mini-lele-v2-rebrand
=======
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
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
*   **Tela preta?** Verifique se a bateria está conectada ou se o cabo USB fornece energia suficiente. O PMU (AXP2101) pode precisar de um ciclo de energia.
*   **Sem áudio?** Verifique se os arquivos `.wav` estão na raiz do SD e são 16kHz 16-bit Mono.
*   **SD não monta?** Tente outro cartão ou formate novamente (FAT32).
>>>>>>> origin/waveshare-s3-amoled-evil-portal-final
