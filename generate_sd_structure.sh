#!/bin/bash
# Script para gerar estrutura de arquivos no SD
mkdir -p sd_out/arquivos_cartao_sd/evil_portal
mkdir -p sd_out/arquivos_cartao_sd/voice
mkdir -p sd_out/arquivos_cartao_sd/tts
mkdir -p sd_out/capturas
mkdir -p sd_out/fila_envio

echo "SSID=SuaRede\nPASS=SuaSenha\nKEY=WpaSecKey" > sd_out/arquivos_cartao_sd/wifi_config.txt

# Voice Assets (Placeholders - User should generate real TTS)
touch sd_out/arquivos_cartao_sd/voice/listening.wav
touch sd_out/arquivos_cartao_sd/tts/hello.wav
touch sd_out/arquivos_cartao_sd/tts/hungry.wav
touch sd_out/arquivos_cartao_sd/tts/attack.wav

echo "Estrutura criada em sd_out/. Copie para o cartao SD."
