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
