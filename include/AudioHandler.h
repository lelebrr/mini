#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

/**
 * AudioHandler.h
 * Manipulação de Áudio para ESP32-S3 (Gravação e Reprodução)
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
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
 * Otimizações:
 * 5. Mute hardware quando não em uso.
 * 26. Desligar DSP Task (via controle de estado).
 * 10. Sample rate configurável.
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
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
>>>>>>> origin/waveshare-s3-amoled-full-review-406
 */

#include <Arduino.h>
#include <driver/i2s.h>
#include <SD_MMC.h>
#include "FS.h"
#include <math.h>

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
// Definição de pinos de controle de áudio (assumindo Expander)
extern ESP_IOExpander *expander; // Disponível no main.cpp

>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
struct WavHeader {
    char riff[4];             // "RIFF"
    uint32_t overall_size;    // Tamanho do arquivo - 8
    char wave[4];             // "WAVE"
    char fmt_chunk_marker[4]; // "fmt "
    uint32_t length_of_fmt;   // 16
    uint16_t format_type;     // 1 (PCM)
    uint16_t channels;        // 1 (Mono) ou 2 (Estéreo)
    uint32_t sample_rate;     // Ex: 16000
    uint32_t byterate;        // Taxa de bytes por segundo
    uint16_t block_align;     // Alinhamento de bloco
    uint16_t bits_per_sample; // 16 bits
    char data_chunk_header[4]; // "data"
    uint32_t data_size;       // Tamanho dos dados de áudio
};

class AudioHandler {
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
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
private:
    static void setAmpPower(bool on) {
        if (expander) {
            // Pino 6 assumido como controle do Amplificador/Audio Power
            expander->digitalWrite(6, on ? HIGH : LOW);
        }
    }

public:
    static void playWav(const char* filename) {
        // Otimização 17 & 5: Liga o AMP e unmute
        setAmpPower(true);
        // es8311_voice_mute(false); // Assumindo função driver disponível ou I2S handles

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
public:
    static void playWav(const char* filename) {
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
public:
    static void playWav(const char* filename) {
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
public:
    static void playWav(const char* filename) {
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        // Ajuste de caminho para nova estrutura em Português
        String path = String("/arquivos_cartao_sd") + filename;

        File file = SD_MMC.open(path);
        if (!file) {
            Serial.printf("[Audio] Erro: Arquivo %s nao encontrado\n", path.c_str());
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
            setAmpPower(false);
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
            setAmpPower(false);
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
            setAmpPower(false);
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
            setAmpPower(false);
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
            setAmpPower(false);
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
            setAmpPower(false);
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
            return;
        }

        WavHeader header;
        file.read((uint8_t*)&header, sizeof(WavHeader));

        if (memcmp(header.riff, "RIFF", 4) != 0) {
            Serial.println("[Audio] Erro: Cabeçalho WAV invalido");
            file.close();
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
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
            setAmpPower(false);
            return;
        }

        // Otimização 10: Seta sample rate do arquivo
        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
            return;
        }

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
            return;
        }

>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
            return;
        }

>>>>>>> origin/waveshare-s3-amoled-full-review-406
        size_t bytes_read = 0;
        size_t bytes_written = 0;
        uint8_t buffer[1024];

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

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
        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
        i2s_set_sample_rates(I2S_NUM_0, header.sample_rate);

>>>>>>> origin/waveshare-s3-amoled-full-review-406
        while (file.available()) {
            bytes_read = file.read(buffer, sizeof(buffer));
            i2s_write(I2S_NUM_0, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        }
        file.close();

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
=======
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
        // Zero buffer para evitar pop e flush
        memset(buffer, 0, sizeof(buffer));
        i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);

        // Otimização 5: Desliga AMP e I2S clock (via mute/idle)
        setAmpPower(false);
        // es8311_voice_mute(true);
    }

    static bool recordWav(const char* filename, int max_duration_sec, bool use_vad = true) {
        // Liga o Microfone (Normalmente AMP não afeta Mic, mas verifica hardware)
        // Se houver Bias de mic no expander, ligar aqui.

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        memset(buffer, 0, sizeof(buffer));
        i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    }

    static bool recordWav(const char* filename, int max_duration_sec, bool use_vad = true) {
<<<<<<< HEAD
<<<<<<< HEAD
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
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        String path = String("/arquivos_cartao_sd") + filename;

        File file = SD_MMC.open(path, FILE_WRITE);
        if (!file) {
            Serial.println("[Audio] Erro: Falha ao criar arquivo de gravacao");
            return false;
        }

        int sample_rate = 16000;
        int channels = 1;
        int bits = 16;

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

        Serial.printf("[Audio] Gravando (Max %d s, VAD: %s)...\n", max_duration_sec, use_vad ? "LIGADO" : "DESLIGADO");

        size_t bytes_read = 0;
        uint8_t buffer[1024];
        uint32_t total_bytes = 0;
        uint32_t target_bytes = sample_rate * (bits/8) * channels * max_duration_sec;

        i2s_set_sample_rates(I2S_NUM_0, sample_rate);

        int silence_threshold = 500;
        int silence_duration_ms = 0;
        int max_silence_ms = 1500;
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
                    float rms = sqrt(sum_squares / num_samples);

                    if (rms > silence_threshold) {
                        silence_duration_ms = 0;
                        voice_detected = true;
                    } else {
                        if (voice_detected) {
                            silence_duration_ms += 32;
                        }
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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        // Desligar ou colocar em idle se necessário
=======
>>>>>>> origin/pwn-tamagotchi-br-release
=======
        // Desligar ou colocar em idle se necessário
>>>>>>> origin/pwn-tamagotchi-legendary-edition-final
=======
        // Desligar ou colocar em idle se necessário
>>>>>>> origin/pwn-tamagotchi-legendary-qs-final
=======
        // Desligar ou colocar em idle se necessário
>>>>>>> origin/pwntamagotchi-br-final-90-features
=======
        // Desligar ou colocar em idle se necessário
>>>>>>> origin/pwntamagotchi-br-final-lvgl9-optimized
=======
        // Desligar ou colocar em idle se necessário
>>>>>>> origin/pwntamagotchi-br-v2-webui-final
=======
>>>>>>> origin/waveshare-s3-amoled-complete-ptbr
=======
>>>>>>> origin/waveshare-s3-amoled-full-review-406
        return true;
    }
};

#endif
