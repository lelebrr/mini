#ifndef OFFLINE_VOICE_H
#define OFFLINE_VOICE_H

/**
 * OfflineVoice.h
 * Processamento de Sinal de Áudio para detecção de comandos simples (Offline).
 *
 * Método:
 * - Analisa o envelope de energia do arquivo WAV gravado.
 * - Conta "picos" de energia (sílabas/palavras) separados por silêncio.
 * - Retorna o número de sílabas para o CommandHandler.
 */

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"
#include "AudioHandler.h"

class OfflineVoice {
public:
    /**
     * Analisa o arquivo de áudio e conta pulsos de energia.
     * @param filename Caminho do arquivo WAV.
     * @return Número de pulsos (sílabas) detectados.
     */
    static int analyzeCommand(const char* filename) {
        File file = SD_MMC.open(filename);
        if (!file) return 0;

        WavHeader header;
        file.read((uint8_t*)&header, sizeof(WavHeader));

        int16_t sample;
        long energy_sum = 0;
        int samples_in_window = 0;
        int window_size = 1600; // Janela de 100ms (16kHz)

        bool in_peak = false;
        int peak_count = 0;
        int silence_gap_counter = 0;

        // Limiar de Energia (Ajustável)
        // Valores típicos: Ruído ~100, Voz > 500
        long threshold_energy = 500;

        while (file.available() >= 2) {
            file.read((uint8_t*)&sample, 2);
            energy_sum += abs(sample); // Retificação
            samples_in_window++;

            if (samples_in_window >= window_size) {
                long avg_energy = energy_sum / samples_in_window;

                if (avg_energy > threshold_energy) {
                    if (!in_peak) {
                        in_peak = true;
                        peak_count++;
                        silence_gap_counter = 0;
                        Serial.printf("[VoiceAI] Pico Detectado (Energia: %ld)\n", avg_energy);
                    }
                } else {
                    if (in_peak) {
                        silence_gap_counter++;
                        // Se o silêncio persistir por > 200ms, o pico acabou
                        if (silence_gap_counter > 2) {
                            in_peak = false;
                        }
                    }
                }

                energy_sum = 0;
                samples_in_window = 0;
            }
        }

        file.close();

        Serial.printf("[VoiceAI] Total Silabas Detectadas: %d\n", peak_count);
        return peak_count;
    }
};

#endif
