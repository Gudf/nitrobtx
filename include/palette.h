#ifndef NITROBTX_PALETTE_H
#define NITROBTX_PALETTE_H

#include <png.h>
#include <stdbool.h>

#include "color.h"
#include "ns/resource_name.h"
#include "vec.h"

struct Palette {
    struct ResourceName name;
    struct NDSColor *data;
    int numColors;
    bool unknown;
};

MakeVecType(PalettesVec, struct Palette);

void *Palette_WritePLTE(const struct Palette *palette, int maxSize, png_structp png_ptr, png_infop info_ptr);

enum ErrorCode Palette_WriteJASCPAL(const struct Palette *palette, const char *path);
enum ErrorCode PalettesVec_AppendFromJASCPAL(struct PalettesVec *vec, const char *path, const char *name, unsigned int copies, bool addSuffix);
enum ErrorCode PalettesVec_AppendFromPNG(struct PalettesVec *vec, const char *path, const char *name, unsigned int copies, bool addSuffix);

#endif // NITROBTX_PALETTE_H
