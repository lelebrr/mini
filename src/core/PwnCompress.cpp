#include "core/PwnCompress.h"
#include <SD_MMC.h>

// Simple RLE compression
uint8_t* PwnCompress::compress(const uint8_t* input, size_t input_len, size_t* output_len) {
    if (!input || input_len == 0) return nullptr;
    size_t max_out = input_len * 2 + 2;
    uint8_t* out = (uint8_t*)malloc(max_out);
    if (!out) return nullptr;

    size_t j = 0;
    out[j++] = (uint8_t)(input_len >> 8);
    out[j++] = (uint8_t)(input_len & 0xFF);

    for (size_t i = 0; i < input_len && j < max_out - 2;) {
        uint8_t val = input[i];
        uint8_t count = 1;
        while (i + count < input_len && input[i + count] == val && count < 255) count++;
        out[j++] = count;
        out[j++] = val;
        i += count;
    }
    *output_len = j;
    return out;
}

uint8_t* PwnCompress::decompress(const uint8_t* input, size_t input_len, size_t* output_len) {
    if (!input || input_len < 2) return nullptr;
    size_t orig = ((size_t)input[0] << 8) | input[1];
    uint8_t* out = (uint8_t*)malloc(orig + 1);
    if (!out) return nullptr;

    size_t j = 0;
    for (size_t i = 2; i + 1 < input_len && j < orig; i += 2) {
        uint8_t count = input[i];
        uint8_t val = input[i + 1];
        for (uint8_t k = 0; k < count && j < orig; k++) out[j++] = val;
    }
    *output_len = j;
    return out;
}

bool PwnCompress::saveCompressedLog(const char* path, const uint8_t* data, size_t len) {
    if (!SD_MMC.begin("/sd", true)) return false;
    size_t comp_len;
    uint8_t* comp = compress(data, len, &comp_len);
    if (!comp) return false;
    File f = SD_MMC.open(path, FILE_WRITE);
    if (!f) { free(comp); return false; }
    f.write(comp, comp_len);
    f.close();
    free(comp);
    return true;
}

uint8_t* PwnCompress::loadCompressedLog(const char* path, size_t* out_len) {
    if (!SD_MMC.begin("/sd", true)) return nullptr;
    File f = SD_MMC.open(path);
    if (!f) return nullptr;
    size_t len = f.size();
    uint8_t* data = (uint8_t*)malloc(len);
    if (!data) { f.close(); return nullptr; }
    f.read(data, len);
    f.close();
    uint8_t* decomp = decompress(data, len, out_len);
    free(data);
    return decomp;
}
