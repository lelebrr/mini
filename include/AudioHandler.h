#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <SD_MMC.h>
#include "FS.h"
#include <math.h>

// Simple WAV header used for recording and playback
struct WavHeader {
    char riff[4];
    uint32_t overall_size;
    char wave[4];
    char fmt_chunk_marker[4];
    uint32_t length_of_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_chunk_header[4];
    uint32_t data_size;
};

class AudioHandler {
public:
    static void playWav(const char* filename) {
        File file = SD_MMC.open(filename);
        if (!file) {
            Serial.printf("[Audio] File %s not found\n", filename);
            return;
        }

        WavHeader header;
        file.read((uint8_t*)&header, sizeof(WavHeader));
        if (memcmp(header.riff, "RIFF", 4) != 0) {
            Serial.println("[Audio] Invalid WAV header");
            file.close();
            return;
        }

        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

        size_t bytes_read = 0;
        size_t bytes_written = 0;
        uint8_t buffer[1024];

        while (file.available()) {
            bytes_read = file.read(buffer, sizeof(buffer));
            i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        }
        file.close();

        memset(buffer, 0, sizeof(buffer));
        i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    }

    static bool recordWav(const char* filename, int max_duration_sec, bool use_vad = true) {
        File file = SD_MMC.open(filename, FILE_WRITE);
        if (!file) {
            Serial.println("[Audio] Failed to create recording file");
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

        Serial.printf("[Audio] Recording (max %d s, VAD: %s)\n",
                      max_duration_sec, use_vad ? "ON" : "OFF");

        size_t bytes_read = 0;
        uint8_t buffer[1024];
        uint32_t total_bytes = 0;
        uint32_t target_bytes = sample_rate * (bits / 8) * channels * max_duration_sec;

        i2s_set_sample_rates(I2S_NUM_0, sample_rate);

        int silence_threshold = 500;
        int silence_duration_ms = 0;
        const int max_silence_ms = 1500;
        bool voice_detected = false;

        while (total_bytes < target_bytes) {
            esp_err_t result = i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);
            if (result != ESP_OK) break;

            if (bytes_read > 0) {
                if (use_vad) {
                    long sum_squares = 0;
                    int16_t* samples = (int16_t*)buffer;
                    int num_samples = bytes_read / 2;
                    for (int i = 0; i < num_samples; i++) {
                        sum_squares += (samples[i] * samples[i]);
                    }
                    float rms = sqrt((float)sum_squares / (float)num_samples);

                    if (rms > silence_threshold) {
                        silence_duration_ms = 0;
                        voice_detected = true;
                    } else if (voice_detected) {
                        silence_duration_ms += 32;
                    }

                    if (voice_detected && silence_duration_ms > max_silence_ms) {
                        break;
                    }
                }

                file.write(buffer, bytes_read);
                total_bytes += bytes_read;
            }
        }

        header.overall_size = total_bytes + sizeof(WavHeader) - 8;
        header.data_size = total_bytes;
        file.seek(0);
        file.write((uint8_t*)&header, sizeof(WavHeader));
        file.close();

        Serial.printf("[Audio] Recording done: %u bytes\n", total_bytes);
        return true;
    }
};

#endif
