#ifndef NITROBTX_COMMON_STRUCTS_H
#define NITROBTX_COMMON_STRUCTS_H

#include <png.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "errors.h"

#define RES_NAME_LENGTH 16

union Magic {
    uint8_t asBytes[4];
    uint32_t raw;
};

int CompareInts(const void *first, const void *second);
uint64_t Fletcher64(uint8_t *_data, size_t len);
bool AreSame(void *first, void *second, size_t len);
enum ErrorCode InitializeReadPNG(const char *inputPath, FILE **fp, png_structp *png_ptr, png_infop *info_ptr);
enum ErrorCode InitializeWritePNG(const char *inputPath, FILE **fp, png_structp *png_ptr, png_infop *info_ptr);

#endif // NITROBTX_COMMON_STRUCTS_H
