/*
 * Copyright (c) 2026 Gudf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "palette.h"

#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "common.h"
#include "ns/resource_name.h"

enum PalReadError {
    PAL_READ_SUCCESS,
    PAL_READ_ERROR_NULL_BYTE,
    PAL_READ_ERROR_LINE_TOO_LONG,
    PAL_READ_ERROR_INVALID_SIGNATURE,
    PAL_READ_ERROR_INVALID_VERSION,
    PAL_READ_ERROR_INVALID_COLOR_COUNT,
    PAL_READ_ERROR_INVALID_CHANNEL_VALUE,
};

static png_color NDSColorToPNG(struct NDSColor color);
static struct NDSColor PNGToNDSColor(png_color color);
static unsigned int NumDigits(unsigned int val);
static enum ErrorCode Palette_ReadPNG(const char *inputPath, struct Palette *out);
static enum ErrorCode Palette_ReadJASCPAL(const char *path, struct Palette *palette);

static bool ParseInteger(char *str, char **end, int base, int *dest, int min, int max)
{
    long val = strtol(str, end, base);

    if (*end == str) {
        return false;
    }

    if (val > max || val < min) {
        return false;
    }

    *dest = (int)val;
    return true;
}

void *Palette_WritePLTE(const struct Palette *palette, int maxSize, png_structp png_ptr, png_infop info_ptr)
{
    int numColors = maxSize < palette->numColors ? maxSize : palette->numColors;
    png_colorp pngPalette = calloc(numColors, sizeof(png_color));
    for (int i = 0; i < numColors; i++) {
        pngPalette[i] = NDSColorToPNG(palette->data[i]);
    }
    png_set_PLTE(png_ptr, info_ptr, pngPalette, numColors);
    return pngPalette;
}

enum ErrorCode Palette_WriteJASCPAL(const struct Palette *palette, const char *path)
{
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file %s for output!\n", path);
        return ERR_CODE_FAILED_OPEN_OUTPUT;
    }

    fputs("JASC-PAL\r\n", fp);
    fputs("0100\r\n", fp);
    fprintf(fp, "%d\r\n", palette->numColors);

    for (int i = 0; i < palette->numColors; i++) {
        struct RGBA8Color color = NDSColorToRGBA8(palette->data[i]);
        fprintf(fp, "%d %d %d\r\n", color.r, color.g, color.b);
    }

    fclose(fp);

    return ERR_CODE_OK;
}

static bool ParseJASCPALColor(char lineBuf[16], struct RGBA8Color *dest)
{
    char *c = lineBuf;
    char *end;
    int r, g, b;

    if (!ParseInteger(c, &end, 10, &r, 0, 255)) {
        return false;
    }

    c = end;

    while (*c == ' ') {
        c++;
    }

    if (!ParseInteger(c, &end, 10, &g, 0, 255)) {
        return false;
    }

    c = end;

    while (*c == ' ') {
        c++;
    }

    if (!ParseInteger(c, &end, 10, &b, 0, 255)) {
        return false;
    }

    dest->r = r;
    dest->g = g;
    dest->b = b;
    dest->a = 0;

    return true;
}

static int ReadLine(FILE *fp, char lineBuf[16])
{
    int length = 0;
    char c;

    while (true) {
        c = fgetc(fp);

        switch (c) {
        case '\n':
        case EOF:
            lineBuf[length] = 0;
            return PAL_READ_SUCCESS;
        case 0:
            return PAL_READ_ERROR_NULL_BYTE;
        case '\r':
            break;
        default:
            if (length == 15) {
                lineBuf[length] = 0;
                return PAL_READ_ERROR_LINE_TOO_LONG;
            }
            lineBuf[length++] = c;
            break;
        }
    }
}

enum ErrorCode PalettesVec_AppendFromJASCPAL(struct PalettesVec *vec, const char *path, const char *name, unsigned int copies)
{
    int nameLength = RES_NAME_LENGTH;

    if (copies > 1) {
        if (strlen(name) + 1 + NumDigits(copies) > RES_NAME_LENGTH) {
            nameLength = RES_NAME_LENGTH - 1 - NumDigits(copies);
            fprintf(stderr, "Warning: palette basename %s is too long to fit the palette number suffix! It will be truncated to %u characters: '%.*s'.\n", name, nameLength, nameLength, name);
        }
    } else {
        if (strlen(name) > RES_NAME_LENGTH) {
            fprintf(stderr, "Warning: palette basename %s is too long! It will be truncated to %u characters: '%.*s'.\n", name, nameLength, nameLength, name);
        }
    }

    VecAppend(*vec, (struct Palette) { 0 });

    struct Palette *pal = &VecLast(*vec);

    enum ErrorCode res;
    if ((res = Palette_ReadJASCPAL(path, pal)) != ERR_CODE_OK) {
        return res;
    }

    char nameBuf[RES_NAME_LENGTH + 1] = { 0 };

    if (copies > 1) {
        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s.%u", nameLength, name, 1);
    } else {
        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s", nameLength, name);
    }

    CopyToResName(&pal->name, nameBuf);

    for (int i = 1; i < copies; i++) {
        VecAppend(*vec, (struct Palette) { 0 });
        struct Palette *copy = &VecLast(*vec);

        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s.%u", nameLength, name, i + 1);
        CopyToResName(&copy->name, nameBuf);
        copy->numColors = pal->numColors;
        copy->unknown = pal->unknown;

        copy->data = calloc(copy->numColors, sizeof(*copy->data));
        memcpy(copy->data, pal->data, copy->numColors * sizeof(struct NDSColor));
    }

    return ERR_CODE_OK;
}

enum ErrorCode PalettesVec_AppendFromPNG(struct PalettesVec *vec, const char *path, const char *name, unsigned int copies)
{
    int nameLength = RES_NAME_LENGTH;

    if (copies > 1) {
        if (strlen(name) + 1 + NumDigits(copies) > RES_NAME_LENGTH) {
            nameLength = RES_NAME_LENGTH - 1 - NumDigits(copies);
            fprintf(stderr, "Warning: palette basename %s is too long to fit the palette number suffix! It will be truncated to %u characters: '%.*s'.\n", name, nameLength, nameLength, name);
        }
    } else {
        if (strlen(name) > RES_NAME_LENGTH) {
            fprintf(stderr, "Warning: palette basename %s is too long! It will be truncated to %u characters: '%.*s'.\n", name, nameLength, nameLength, name);
        }
    }

    VecAppend(*vec, (struct Palette) { 0 });

    struct Palette *pal = &VecLast(*vec);

    enum ErrorCode res;
    if ((res = Palette_ReadPNG(path, pal)) != ERR_CODE_OK) {
        return res;
    }

    char nameBuf[RES_NAME_LENGTH + 1] = { 0 };

    if (copies > 1) {
        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s.%u", nameLength, name, 1);
    } else {
        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s", nameLength, name);
    }

    CopyToResName(&pal->name, nameBuf);

    for (int i = 1; i < copies; i++) {
        VecAppend(*vec, (struct Palette) { 0 });
        struct Palette *copy = &VecLast(*vec);

        snprintf(nameBuf, RES_NAME_LENGTH + 1, "%.*s.%u", nameLength, name, i + 1);
        CopyToResName(&copy->name, nameBuf);
        copy->numColors = pal->numColors;
        copy->unknown = pal->unknown;

        copy->data = calloc(copy->numColors, sizeof(*copy->data));
        memcpy(copy->data, pal->data, copy->numColors * sizeof(struct NDSColor));
    }

    return ERR_CODE_OK;
}

enum ErrorCode Palette_ReadJASCPAL(const char *path, struct Palette *palette)
{
    enum ErrorCode err = ERR_CODE_OK;

    char lineBuf[16];
    FILE *fp = fopen(path, "rb");

    if (!fp) {
        fprintf(stderr, "Failed to open file %s for reading!\n", path);
        return ERR_CODE_FAILED_OPEN_INPUT;
    }

    if (ReadLine(fp, lineBuf) != PAL_READ_SUCCESS) {
        fprintf(stderr, "Error while reading %s!\n", path);
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    if (strcmp(lineBuf, "JASC-PAL") != 0) {
        fprintf(stderr, "Invalid JASC-PAL signature!\n");
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    if (ReadLine(fp, lineBuf) != PAL_READ_SUCCESS) {
        fprintf(stderr, "Error while reading %s!\n", path);
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    if (strcmp(lineBuf, "0100") != 0) {
        fprintf(stderr, "Invalid JASC-PAL version!\n");
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    if (ReadLine(fp, lineBuf) != PAL_READ_SUCCESS) {
        fprintf(stderr, "Error while reading %s!\n", path);
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    char *end;
    int numColors = strtol(lineBuf, &end, 10);
    printf("colors: %u\n", numColors);

    if (end == lineBuf || numColors < 1 || numColors > 256) {
        fprintf(stderr, "Invalid JASC-PAL color count!\n");
        err = ERR_CODE_INPUT_INVALID;
        goto cleanup;
    }

    palette->numColors = numColors;
    palette->data = calloc(numColors, sizeof(*palette->data));

    for (int i = 0; i < numColors; i++) {
        ReadLine(fp, lineBuf);
        struct RGBA8Color color;
        if (!ParseJASCPALColor(lineBuf, &color)) {
            fprintf(stderr, "Invalid JASC-PAL channel value!\n");
            err = ERR_CODE_INPUT_INVALID;
            goto cleanup_free_pal;
        }

        palette->data[i] = RGBA8ToNDSColor(color);
    }

    goto cleanup;

cleanup_free_pal:
    free(palette->data);
    palette->data = NULL;
    palette->numColors = 0;

cleanup:
    fclose(fp);
    return err;
}

enum ErrorCode Palette_ReadPNG(const char *inputPath, struct Palette *out)
{
    png_colorp palette;
    int num_palette;

    FILE *fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    enum ErrorCode res;
    if ((res = InitializeReadPNG(inputPath, &fp, &png_ptr, &info_ptr)) != ERR_CODE_OK) {
        return res;
    }

    png_read_info(png_ptr, info_ptr);

    uint32_t result = png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
    if (!result) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        fprintf(stderr, "Palette input PNG %s doesn't contain a pLTE chunk!\n", inputPath);
        return ERR_CODE_INPUT_INVALID;
    }

    out->numColors = num_palette;
    out->data = calloc(num_palette, sizeof(struct NDSColor));

    for (int i = 0; i < num_palette; i++) {
        out->data[i] = PNGToNDSColor(palette[i]);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    return ERR_CODE_OK;
}

static unsigned int NumDigits(unsigned int val)
{
    int n = 0;
    for (int i = 10, n = 1; i < INT_MAX / 10; i *= 10, n++) {
        if (val < i) {
            return n;
        }
    }
    return n;
}
