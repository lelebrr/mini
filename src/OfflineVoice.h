#ifndef OFFLINE_VOICE_H
#define OFFLINE_VOICE_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"
#include "AudioHandler.h" // For WavHeader definition

class OfflineVoice {
public:
    // Simple Syllable/Peak Detector based on Energy Envelope
    // Returns number of detected "pulses" (syllables)
    static int analyzeCommand(const char* filename) {
        File file = SD_MMC.open(filename);
        if (!file) return 0;

        WavHeader header;
        file.read((uint8_t*)&header, sizeof(WavHeader));

        // Skip to data if needed, simplified here assuming standard header

        int16_t sample;
        long energy_sum = 0;
        int samples_in_window = 0;
        int window_size = 1600; // 100ms at 16kHz

        bool in_peak = false;
        int peak_count = 0;
        int silence_gap_counter = 0;

        // Thresholds (Need calibration, assuming reasonable normalization or mic gain)
        // 16-bit signed audio: max 32767.
        // Quiet room noise might be ~50-100 RMS.
        // Voice might be > 500-1000 RMS.
        long threshold_energy = 500;

        while (file.available() >= 2) {
            file.read((uint8_t*)&sample, 2);
            energy_sum += abs(sample); // Simple Rectification
            samples_in_window++;

            if (samples_in_window >= window_size) {
                long avg_energy = energy_sum / samples_in_window;

                if (avg_energy > threshold_energy) {
                    if (!in_peak) {
                        in_peak = true;
                        peak_count++;
                        silence_gap_counter = 0;
                        Serial.printf("OfflineAI: Pico Detectado (Energia: %ld)\n", avg_energy);
                    }
                } else {
                    if (in_peak) {
                        silence_gap_counter++;
                        // If silence persists for > 200ms (2 windows), consider peak ended
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

        Serial.printf("OfflineAI: Total Silabas/Picos Detectados: %d\n", peak_count);
        return peak_count;
    }
};

#endif
