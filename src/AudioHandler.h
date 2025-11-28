#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <SD_MMC.h>
#include "FS.h"

// WAV Header Structure
struct WavHeader {
    char riff[4];           // "RIFF"
    uint32_t overall_size;  // file size - 8
    char wave[4];           // "WAVE"
    char fmt_chunk_marker[4]; // "fmt "
    uint32_t length_of_fmt; // 16
    uint16_t format_type;   // 1 (PCM)
    uint16_t channels;      // 1 or 2
    uint32_t sample_rate;   // 16000
    uint32_t byterate;      // SampleRate * NumChannels * BitsPerSample/8
    uint16_t block_align;   // NumChannels * BitsPerSample/8
    uint16_t bits_per_sample; // 16
    char data_chunk_header[4]; // "data"
    uint32_t data_size;     // Size of data section
};

// Play WAV from SD
void playWav(const char* filename) {
    File file = SD_MMC.open(filename);
    if (!file) {
        Serial.printf("Audio: Arquivo %s nao encontrado\n", filename);
        return;
    }

    // Read header
    WavHeader header;
    file.read((uint8_t*)&header, sizeof(WavHeader));

    Serial.printf("Tocando %s: %d Hz, %d bits, %d canais\n", filename, header.sample_rate, header.bits_per_sample, header.channels);

    // Buffer
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    uint8_t buffer[1024];

    // Setup I2S for Playback if needed (assuming main setup did it, but sample rate might change)
    i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

    while (file.available()) {
        bytes_read = file.read(buffer, sizeof(buffer));
        i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
    }
    file.close();

    // Silence at end
    memset(buffer, 0, sizeof(buffer));
    i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);

    Serial.println("Audio: Reproducao concluida");
}

// Record WAV to SD
void recordWav(const char* filename, int duration_sec) {
    File file = SD_MMC.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Audio: Falha ao criar arquivo de gravacao");
        return;
    }

    int sample_rate = 16000;
    int channels = 1; // Mic is usually Mono on I2S logic often (L/R same) or just use 2
    int bits = 16;

    // Write Dummy Header
    WavHeader header;
    memcpy(header.riff, "RIFF", 4);
    header.overall_size = 0; // Update later
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
    header.data_size = 0; // Update later

    file.write((uint8_t*)&header, sizeof(WavHeader));

    // Record
    Serial.printf("Gravando por %d segundos...\n", duration_sec);
    size_t bytes_read = 0;
    uint8_t buffer[1024];
    uint32_t total_bytes = 0;

    // Ensure correct sample rate
    i2s_set_sample_rates(I2S_NUM_0, sample_rate);

    // Calculate bytes needed
    uint32_t target_bytes = sample_rate * (bits/8) * channels * duration_sec;

    while (total_bytes < target_bytes) {
        // Read from I2S
        i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

        // Write to SD
        if (bytes_read > 0) {
            file.write(buffer, bytes_read);
            total_bytes += bytes_read;
        }
    }

    // Update Header
    header.overall_size = total_bytes + sizeof(WavHeader) - 8;
    header.data_size = total_bytes;
    file.seek(0);
    file.write((uint8_t*)&header, sizeof(WavHeader));
    file.close();

    Serial.printf("Gravacao concluida: %d bytes\n", total_bytes);
}

#endif
