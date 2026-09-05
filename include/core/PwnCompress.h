#ifndef PWN_COMPRESS_H
#define PWN_COMPRESS_H

#include <Arduino.h>
#include <SD_MMC.h>
#include "FS.h"
// A ROM do ESP32 inclui o miniz (Deflate/zlib compatível), economizando flash.
#include <rom/miniz.h>

/**
 * PwnCompress
 * -----------
 * Compressão Deflate/zlib usando o miniz embutido na ROM do ESP32.
 *
 * CORRIGIDO: a versão anterior tratava o retorno de tdefl_compress_mem_to_mem
 * como um "status" int e tinha código morto/contraditório. Na verdade a função
 * retorna um size_t com o tamanho comprimido (0 em caso de falha). Além disso,
 * o compressor precisa escrever o cabeçalho zlib (TDEFL_WRITE_ZLIB_HEADER) para
 * que o decompressor (que usa TINFL_FLAG_PARSE_ZLIB_HEADER) faça o round-trip.
 */
class PwnCompress {
public:
    // Comprime `input`. Retorna o número de bytes escritos em `output`, ou 0 em erro.
    static size_t compress(const uint8_t *input, size_t input_len,
                           uint8_t *output, size_t output_len) {
        if (!input || !output || input_len == 0 || output_len == 0) return 0;

        size_t compressed_len = tdefl_compress_mem_to_mem(
            output, output_len,
            input, input_len,
            TDEFL_WRITE_ZLIB_HEADER | TDEFL_DEFAULT_MAX_PROBES);

        return compressed_len; // 0 = falha (buffer pequeno ou erro)
    }

    // Descomprime `input`. Retorna o número de bytes escritos em `output`, ou 0 em erro.
    static size_t decompress(const uint8_t *input, size_t input_len,
                             uint8_t *output, size_t output_len) {
        if (!input || !output || input_len == 0 || output_len == 0) return 0;

        size_t decompressed_len = tinfl_decompress_mem_to_mem(
            output, output_len,
            input, input_len,
            TINFL_FLAG_PARSE_ZLIB_HEADER);

        // tinfl_decompress_mem_to_mem retorna (size_t)-1 em falha.
        if (decompressed_len == (size_t)-1) return 0;
        return decompressed_len;
    }

    // Comprime `data` e grava (append) em `filename` no cartão SD.
    static bool saveCompressedLog(const char *filename, const String &data) {
        size_t in_len = data.length();
        if (in_len == 0) return false;

        // Deflate raramente expande; +64 de folga para blocos incompressíveis.
        size_t cap = in_len + (in_len / 2) + 64;
        uint8_t *out = (uint8_t *)malloc(cap);
        if (!out) {
            Serial.println("[Compress] Sem memória para buffer.");
            return false;
        }

        size_t written = compress((const uint8_t *)data.c_str(), in_len, out, cap);
        if (written == 0) {
            free(out);
            Serial.println("[Compress] Falha ao comprimir.");
            return false;
        }

        File f = SD_MMC.open(filename, FILE_APPEND);
        if (!f) {
            free(out);
            Serial.printf("[Compress] Falha ao abrir %s\n", filename);
            return false;
        }
        f.write(out, written);
        f.close();
        free(out);
        return true;
    }
};

#endif
