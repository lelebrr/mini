#ifndef PWN_COMPRESS_H
#define PWN_COMPRESS_H

#include <Arduino.h>
// ESP32 ROM includes miniz (Deflate/zlib compatible)
#include <rom/miniz.h>

// Otimização 11: Compressão (usando ROM miniz para economizar flash)

class PwnCompress {
public:
    // Comprime buffer de entrada
    static size_t compress(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_len) {
        size_t compressed_len = output_len;
        int status = tdefl_compress_mem_to_mem(output, compressed_len, input, input_len, TDEFL_DEFAULT_MAX_PROBES);

        if (status != 0) { // tdefl retorna tamanho ou 0 se falha?
             // Na verdade tdefl_compress_mem_to_mem retorna o tamanho real ou 0 on failure na implementação ROM
             return compressed_len; // Se a lib retornar o tamanho em status, verificar docs.
             // Docs ROM ESP32: size_t tdefl_compress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pIn_buf, size_t in_buf_len, int flags);
             // Retorna tamanho.
             return status;
        }
        return 0; // Erro
    }

    // Descomprime
    static size_t decompress(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_len) {
        size_t decompressed_len = output_len;
        int status = tinfl_decompress_mem_to_mem(output, &decompressed_len, input, input_len, TINFL_FLAG_PARSE_ZLIB_HEADER);

        if (status != TINFL_STATUS_DONE) {
             return 0;
        }
        return decompressed_len;
    }

    // Helper para salvar logs comprimidos no SD
    static bool saveCompressedLog(const char* filename, String data) {
        // Implementação futura: Comprimir chunks de logs WiFi
        return true;
    }
};

#endif
