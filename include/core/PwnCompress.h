#ifndef PWNCOMPRESS_H
#define PWNCOMPRESS_H

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

class PwnCompress {
public:
    static uint8_t* compress(const uint8_t* input, size_t input_len, size_t* output_len);
    static uint8_t* decompress(const uint8_t* input, size_t input_len, size_t* output_len);
    static bool saveCompressedLog(const char* path, const uint8_t* data, size_t len);
    static uint8_t* loadCompressedLog(const char* path, size_t* out_len);
};

#endif
