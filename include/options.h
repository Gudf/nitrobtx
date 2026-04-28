#ifndef NITROBTX_OPTIONS_H
#define NITROBTX_OPTIONS_H

#include "texture.h"
#include "vec.h"

enum Mode {
    MODE_DUMP,
    MODE_PACK,
};

enum PaletteInputType {
    INPUT_TYPE_PNG,
    INPUT_TYPE_JASC_PAL,
};

struct Settings {
    enum Mode mode;
    char *inputPath;
    char *outputPath;
};

struct PaletteInput {
    enum PaletteInputType inputType;
    bool repeat;
    int repeatCount;
    const char *name;
    const char *path;
};

struct NSBTXInput {
    bool spritesheet;
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

#endif // NITROBTX_OPTIONS_H
