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
├── sd_card_files/          (Copie o conteúdo desta pasta do repo para a raiz)
│   ├── evil_portal/        (Templates HTML)
│   ├── boot_pt.wav         (Áudios)
│   ├── listening_pt.wav
│   ├── success_pt.wav
│   └── error_pt.wav
├── handshakes/             (Será criada automaticamente)
├── upload_queue/           (Será criada automaticamente)
├── wifi_config.txt         (Crie este arquivo com suas credenciais WiFi para upload)
└── captured_creds.txt      (Onde as senhas do Evil Portal serão salvas)
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

*   **Tela preta?** Verifique se a bateria está conectada ou se o cabo USB fornece energia suficiente. O PMU (AXP2101) pode precisar de um ciclo de energia.
*   **Sem áudio?** Verifique se os arquivos `.wav` estão na raiz do SD e são 16kHz 16-bit Mono.
*   **SD não monta?** Tente outro cartão ou formate novamente (FAT32).
