#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"
#include <driver/i2s.h>
#include "esp_heap_caps.h"
#include "pin_config.h"
extern "C" {
#include "es8311.h"
}

// Cabeçalho WAV simples (PCM 16 bits, mono).
// packed garante sizeof == 44 independente do compilador/padding.
struct __attribute__((packed)) WavHeader {
    char     riff[4];
    uint32_t chunkSize;
    char     wave[4];
    char     fmt[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char     data[4];
    uint32_t dataSize;
};
static_assert(sizeof(WavHeader) == 44, "WavHeader deve ter exatamente 44 bytes");

class AudioHandler {
public:
    static bool audio_ok;   // definido em core_singletons.cpp

    // Inicialização completa: I2S + codec ES8311. Segura — se o codec falhar,
    // o dispositivo continua funcionando (áudio apenas desativado).
    static bool init() {
        pinMode(PA, OUTPUT);
        digitalWrite(PA, LOW);      // amplificador começa mudo

        initI2S();

        // Inicializa o codec ES8311 (I2C, mesmo barramento do Wire).
        es8311_handle_t es = es8311_create(0, ES8311_ADDR);
        if (!es) {
            Serial.println("[Audio] ES8311 não encontrado — áudio desativado.");
            audio_ok = false;
            return false;
        }

        const es8311_clock_config_t clk = {
            .mclk_inverted     = false,
            .sclk_inverted     = false,
            .mclk_from_mclk_pin= true,
            .mclk_frequency    = 16000 * 256,  // MCLK = 256 * fs
            .sample_frequency  = 16000
        };

        if (es8311_init(es, &clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16) != ESP_OK) {
            Serial.println("[Audio] Falha ao inicializar ES8311 — áudio desativado.");
            audio_ok = false;
            return false;
        }
        es8311_sample_frequency_config(es, 16000 * 256, 16000);
        es8311_voice_volume_set(es, 80, nullptr);
        es8311_microphone_config(es, false);        // mic analógico
        es8311_microphone_gain_set(es, (es8311_mic_gain_t)3);

        audio_ok = true;
        Serial.println("[Audio] ES8311 pronto.");
        return true;
    }

    static void initI2S() {
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
            .sample_rate = 16000,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,
            .dma_buf_len = 256,
            .use_apll = false,
            .tx_desc_auto_clear = true,
            .fixed_mclk = 0
        };

        i2s_pin_config_t pin_config = {
            .mck_io_num = I2S_MCK_IO,
            .bck_io_num = I2S_BCK_IO,
            .ws_io_num = I2S_WS_IO,
            .data_out_num = I2S_DO_IO,
            .data_in_num = I2S_DI_IO
        };

        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, nullptr);
        i2s_set_pin(I2S_NUM_0, &pin_config);
    }

    // Liga/desliga o amplificador de áudio (PA) — GPIO46 na Waveshare AMOLED 1.8
    // (o demo oficial 15_ES8311 usa pinMode(PA)/digitalWrite(PA), NÃO o expansor).
    static void setAmpPower(bool on) {
        digitalWrite(PA, on ? HIGH : LOW);
    }

    // Resolve caminho para compatibilidade:
    //  - se começar com "/arquivos_cartao_sd" usa direto
    //  - senão tenta primeiro o caminho original, depois prefixa "/arquivos_cartao_sd"
    static File openForReadWithCompat(const char *filename) {
        String path(filename);

        // Se já for absoluto para /arquivos_cartao_sd, usa direto
        if (path.startsWith("/arquivos_cartao_sd")) {
            return SD_MMC.open(path, FILE_READ);
        }

        // Tenta exatamente o caminho recebido
        File f = SD_MMC.open(path, FILE_READ);
        if (f) return f;

        // Tenta dentro de /arquivos_cartao_sd
        String fallback = String("/arquivos_cartao_sd") + path;
        return SD_MMC.open(fallback, FILE_READ);
    }

    static String buildWritePath(const char *filename) {
        String path(filename);
        if (path.startsWith("/arquivos_cartao_sd")) {
            return path;
        }
        // Gravações do firmware usam pastas lógicas como "/voice",
        // então prefixamos com /arquivos_cartao_sd para manter o layout novo.
        if (path.startsWith("/voice") || path.startsWith("/tts")) {
            return String("/arquivos_cartao_sd") + path;
        }
        // Caso genérico: grava exatamente onde foi pedido
        return path;
    }

    static void playWav(const char *filename) {
        if (!audio_ok) return;   // codec não inicializado
        File file = openForReadWithCompat(filename);
        if (!file) {
            Serial.printf("[Audio] Falha ao abrir '%s'\n", filename);
            return;
        }

        WavHeader header;
        if (file.read((uint8_t *)&header, sizeof(WavHeader)) != sizeof(WavHeader)) {
            Serial.println("[Audio] Cabeçalho WAV inválido (tamanho).");
            file.close();
            return;
        }

        if (strncmp(header.riff, "RIFF", 4) != 0 ||
            strncmp(header.wave, "WAVE", 4) != 0) {
            Serial.println("[Audio] Arquivo WAV inválido (RIFF/WAVE).");
            file.close();
            return;
        }

        i2s_set_sample_rates(I2S_NUM_0, header.sampleRate);
        setAmpPower(true);

        const size_t bufSize = 512;
        uint8_t *buffer = (uint8_t *)heap_caps_malloc(bufSize, MALLOC_CAP_DMA);
        if (!buffer) {
            Serial.println("[Audio] Falha ao alocar buffer.");
            file.close();
            setAmpPower(false);
            return;
        }

        size_t bytesRead;
        size_t bytesWritten;
        while ((bytesRead = file.read(buffer, bufSize)) > 0) {
            i2s_write(I2S_NUM_0, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
        }

        heap_caps_free(buffer);
        file.close();
        setAmpPower(false);
    }

    static bool recordWav(const char *filename, int max_seconds, bool use_vad = true) {
        if (!audio_ok) return false;   // codec não inicializado
        String path = buildWritePath(filename);

        // Garante que o diretório exista, se houver subpastas
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash > 0) {
            String dir = path.substring(0, lastSlash);
            if (!SD_MMC.exists(dir)) {
                SD_MMC.mkdir(dir);
            }
        }

        File file = SD_MMC.open(path, FILE_WRITE);
        if (!file) {
            Serial.printf("[Audio] Falha ao criar '%s'\n", path.c_str());
            return false;
        }

        WavHeader header;
        memcpy(header.riff, "RIFF", 4);
        memcpy(header.wave, "WAVE", 4);
        memcpy(header.fmt, "fmt ", 4);
        header.subchunk1Size = 16;
        header.audioFormat   = 1;
        header.numChannels   = 1;
        header.sampleRate    = 16000;
        header.bitsPerSample = 16;
        header.byteRate      = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
        header.blockAlign    = header.numChannels * header.bitsPerSample / 8;
        memcpy(header.data, "data", 4);
        header.dataSize  = 0;
        header.chunkSize = 36;

        file.write((uint8_t *)&header, sizeof(WavHeader));

        setAmpPower(true);

        const size_t bufSize = 512;
        int16_t *buffer = (int16_t *)heap_caps_malloc(bufSize * sizeof(int16_t), MALLOC_CAP_DMA);
        if (!buffer) {
            Serial.println("[Audio] Falha ao alocar buffer de gravação.");
            file.close();
            setAmpPower(false);
            return false;
        }

        size_t bytesRead;
        unsigned long start_ms        = millis();
        long          energy_sum      = 0;
        int           samples_window  = 0;
        bool          voice_detected  = false;
        const long    threshold_energy = 500;

        while (millis() - start_ms < (unsigned long)max_seconds * 1000) {
            i2s_read(I2S_NUM_0, buffer, bufSize * sizeof(int16_t), &bytesRead, portMAX_DELAY);
            int samples = bytesRead / sizeof(int16_t);

            for (int i = 0; i < samples; ++i) {
                int16_t sample = buffer[i];
                energy_sum += abs(sample);
                samples_window++;

                if (samples_window >= 1600) { // ~100 ms
                    long avg_energy = energy_sum / samples_window;
                    if (avg_energy > threshold_energy) {
                        voice_detected = true;
                    } else if (use_vad && voice_detected) {
                        goto done_recording;
                    }
                    energy_sum     = 0;
                    samples_window = 0;
                }
            }

            file.write((uint8_t *)buffer, bytesRead);
            header.dataSize += bytesRead;
        }

    done_recording:
        heap_caps_free(buffer);

        // Atualiza cabeçalho com tamanho final
        file.seek(0);
        header.chunkSize = 36 + header.dataSize;
        file.write((uint8_t *)&header, sizeof(WavHeader));
        file.close();

        setAmpPower(false);
        return voice_detected;
    }
};

#endif
