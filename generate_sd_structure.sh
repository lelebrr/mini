#!/bin/bash
# Script para gerar estrutura de arquivos no SD

# Diretórios base
mkdir -p sd_out/arquivos_cartao_sd/evil_portal
mkdir -p sd_out/arquivos_cartao_sd/voice
mkdir -p sd_out/arquivos_cartao_sd/tts
mkdir -p sd_out/capturas
mkdir -p sd_out/fila_envio
mkdir -p sd_out/custom-faces

# Configuração de Wi-Fi de exemplo (ajuste para a sua rede)
echo "SSID=SuaRede\nPASS=SuaSenha\nKEY=WpaSecKey" > sd_out/arquivos_cartao_sd/wifi_config.txt

# Copia templates de Evil Portal que já vêm no repositório
if [ -d "arquivos_cartao_sd/evil_portal" ]; then
    cp -n arquivos_cartao_sd/evil_portal/*.html sd_out/arquivos_cartao_sd/evil_portal/ 2>/dev/null || true
fi

echo "Gerando arquivos de áudio básicos (placeholders)..."

# Gera WAVs simples (bips) se python3 estiver disponível
if command -v python3 >/dev/null 2>&1; then
python3 - << 'EOF'
import os
import wave
import struct
import math

SAMPLE_RATE = 16000
DURATION = 0.6
VOLUME = 0.3

def make_beep(path, freq):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    n_samples = int(DURATION * SAMPLE_RATE)
    with wave.open(path, "w") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)  # 16-bit
        wf.setframerate(SAMPLE_RATE)
        for i in range(n_samples):
            value = int(VOLUME * 32767 * math.sin(2 * math.pi * freq * i / SAMPLE_RATE))
            wf.writeframes(struct.pack("<h", value))

files = {
    # Feedback de gravação / escuta
    "sd_out/arquivos_cartao_sd/voice/listening.wav": 1200,
    "sd_out/listening_pt.wav": 1200,
    "sd_out/arquivos_cartao_sd/listening_pt.wav": 1200,

    # TTS do Pet
    "sd_out/arquivos_cartao_sd/tts/hello.wav": 880,
    "sd_out/arquivos_cartao_sd/tts/hungry.wav": 660,
    "sd_out/arquivos_cartao_sd/tts/attack.wav": 440,
    "sd_out/arquivos_cartao_sd/tts/wakeup.wav": 990,
    "sd_out/arquivos_cartao_sd/tts/unknown.wav": 330,

    # Áudios de sistema (raiz e dentro de /arquivos_cartao_sd para compatibilidade)
    "sd_out/boot_pt.wav": 880,
    "sd_out/success_pt.wav": 1200,
    "sd_out/error_pt.wav": 220,
    "sd_out/arquivos_cartao_sd/boot_pt.wav": 880,
    "sd_out/arquivos_cartao_sd/success_pt.wav": 1200,
    "sd_out/arquivos_cartao_sd/error_pt.wav": 220,
}

for path, freq in files.items():
    make_beep(path, freq)
EOF
else
    echo "python3 não encontrado; criando arquivos WAV vazios (placeholders)."
    touch sd_out/arquivos_cartao_sd/voice/listening.wav
    touch sd_out/listening_pt.wav
    touch sd_out/arquivos_cartao_sd/listening_pt.wav

    touch sd_out/arquivos_cartao_sd/tts/hello.wav
    touch sd_out/arquivos_cartao_sd/tts/hungry.wav
    touch sd_out/arquivos_cartao_sd/tts/attack.wav
    touch sd_out/arquivos_cartao_sd/tts/wakeup.wav
    touch sd_out/arquivos_cartao_sd/tts/unknown.wav

    touch sd_out/boot_pt.wav
    touch sd_out/success_pt.wav
    touch sd_out/error_pt.wav
    touch sd_out/arquivos_cartao_sd/boot_pt.wav
    touch sd_out/arquivos_cartao_sd/success_pt.wav
    touch sd_out/arquivos_cartao_sd/error_pt.wav
fi

echo "Estrutura criada em sd_out/. Copie para o cartao SD."
