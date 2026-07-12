#ifndef NITROBTX_OPTIONS_H
#define NITROBTX_OPTIONS_H

#include "palette.h"
#include "texture.h"
#include "vec.h"

enum Mode {
    MODE_DUMP,
    MODE_PACK,
};

struct NSBTXInput {
    bool spritesheet;
    bool combinedPalette;
    const char *path;
    const char *palettesPath;
};

struct Options {
    enum Mode mode;
    MakeVecType(TextureInputVec, struct TextureInput) textures;
    MakeVecType(PaletteInputVec, struct PaletteInput) palettes;
    MakeVecType(NSBTXInputVec, struct NSBTXInput) nsbtxs;
    char *outputPath;
};

void ParseOptions(int argc, char **argv, struct Options *options);
void FreeOptions(struct Options *options);

#endif // NITROBTX_OPTIONS_H
