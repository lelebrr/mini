#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <SD_MMC.h>
#include "FS.h"
#include <math.h>

// WAV header used for simple 16‑bit PCM files
struct WavHeader {
    char riff[4];             // "RIFF"
    uint32_t overall_size;    // file size - 8
    char wave[4];             // "WAVE"
    char fmt_chunk_marker[4]; // "fmt "
    uint32_t length_of_fmt;   // 16
    uint16_t format_type;     // 1 (PCM)
    uint16_t channels;        // 1 (mono) or 2 (stereo)
    uint32_t sample_rate;     // e.g. 16000
    uint32_t byterate;        // SampleRate * NumChannels * BitsPerSample/8
    uint16_t block_align;     // NumChannels * BitsPerSample/8
    uint16_t bits_per_sample; // 16
    char data_chunk_header[4];// "data"
    uint32_t data_size;       // Size of data section
};

class AudioHandler {
public:
    // Simple WAV playback from SD_MMC
    static void playWav(const char* filename) {
        File file = SD_MMC.open(filename);
        if (!file) {
            Serial.printf("[Audio] Arquivo %s nao encontrado\n", filename);
            return;
        }

        WavHeader header;
        if (file.read((uint8_t*)&header, sizeof(WavHeader)) != sizeof(WavHeader)) {
            Serial.println("[Audio] Erro ao ler cabecalho WAV");
            file.close();
            return;
        }

        if (memcmp(header.riff, "RIFF", 4) != 0 || memcmp(header.wave, "WAVE", 4) != 0) {
            Serial.println("[Audio] Cabecalho WAV invalido");
            file.close();
            return;
        }

        Serial.printf("[Audio] Tocando %s: %lu Hz, %u bits, %u canais\n",
                      filename,
                      (unsigned long)header.sample_rate,
                      header.bits_per_sample,
                      header.channels);

        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

        uint8_t buffer[1024];
        size_t bytes_read = 0;
        size_t bytes_written = 0;

        while (file.available()) {
            bytes_read = file.read(buffer, sizeof(buffer));
            if (bytes_read == 0) break;
            i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        }
        file.close();

        // Envia um pequeno buffer zerado para evitar "pop"
        memset(buffer, 0, sizeof(buffer));
        i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    }

    // Grava WAV mono 16 kHz com detecção simples de silêncio (VAD)
    static bool recordWav(const char* filename, int max_duration_sec, bool use_vad = true) {
        File file = SD_MMC.open(filename, FILE_WRITE);
        if (!file) {
            Serial.println("[Audio] Falha ao criar arquivo de gravacao");
            return false;
        }

        const int sample_rate = 16000;
        const int channels = 1;
        const int bits = 16;

        WavHeader header;
        memcpy(header.riff, "RIFF", 4);
        header.overall_size = 0;
        memcpy(header.wave, "WAVE", 4);
        memcpy(header.fmt_chunk_marker, "fmt ", 4);
        header.length_of_fmt = 16;
        header.format_type = 1;
        header.channels = channels;
        header.sample_rate = sample_rate;
        header.bits_per_sample = bits;
        header.byterate = sample_rate * channels * (bits / 8);
        header.block_align = channels * (bits / 8);
        memcpy(header.data_chunk_header, "data", 4);
        header.data_size = 0;

        file.write((uint8_t*)&header, sizeof(WavHeader));

        Serial.printf("[Audio] Gravando (Max %d s, VAD: %s)...\n",
                      max_duration_sec, use_vad ? "ON" : "OFF");

        i2s_set_sample_rates(I2S_NUM_0, sample_rate);

        uint8_t buffer[1024];
        size_t bytes_read = 0;
        uint32_t total_bytes = 0;
        const uint32_t target_bytes = sample_rate * (bits / 8) * channels * max_duration_sec;

        // VAD (detecção de silêncio)
        const int silence_threshold = 500;  // Ajuste conforme ruído do microfone
        int silence_duration_ms = 0;
        const int max_silence_ms = 1500;
        bool voice_detected = false;

        while (total_bytes < target_bytes) {
            esp_err_t res = i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);
            if (res != ESP_OK || bytes_read == 0) break;

            if (use_vad) {
                long sum_squares = 0;
                int16_t* samples = (int16_t*)buffer;
                int num_samples = bytes_read / 2;
                if (num_samples > 0) {
                    for (int i = 0; i < num_samples; i++) {
                        sum_squares += (long)samples[i] * (long)samples[i];
                    }
                    float rms = sqrt((double)sum_squares / (double)num_samples);

                    if (rms > silence_threshold) {
                        silence_duration_ms = 0;
                        voice_detected = true;
                    } else if (voice_detected) {
                        // ~32 ms por buffer @ 16kHz / 1024 bytes
                        silence_duration_ms += 32;
                    }

                    if (voice_detected && silence_duration_ms > max_silence_ms) {
                        Serial.println("[Audio] Silencio detectado, encerrando gravacao");
                        file.write(buffer, bytes_read);
                        total_bytes += bytes_read;
                        break;
                    }
                }
            }

            file.write(buffer, bytes_read);
            total_bytes += bytes_read;
        }

        header.overall_size = total_bytes + sizeof(WavHeader) - 8;
        header.data_size = total_bytes;
        file.seek(0);
        file.write((uint8_t*)&header, sizeof(WavHeader));
        file.close();

        Serial.printf("[Audio] Gravacao finalizada: %lu bytes\n", (unsigned long)total_bytes);
        return true;
    }
};

#endif
