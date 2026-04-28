#ifndef NITROBTX_TEX_H
#define NITROBTX_TEX_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "texture.h"

#define TEX0_MAGIC "TEX0"

struct TextureDictData {
    uint16_t textureOffset;
    uint16_t : 4;
    uint16_t sSize : 3;
    uint16_t tSize : 3;
    uint16_t format : 3;
    uint16_t transparent : 1;
    uint16_t : 2;
    uint32_t width : 11;
    uint32_t height : 11;
    uint32_t : 9;
    uint32_t fillsTexSize : 1;
};

struct PaletteDictData {
    uint16_t plttOffset : 12;
    uint16_t unused1_4 : 4;
    uint16_t unknown2;
};

struct NSChunkTex {
    struct TexturesVec textures;
    struct PalettesVec palettes;
};

struct NSChunkTex *MakeTexChunk(void);
int GetTexelSizeForTexFmt(enum TextureFormat format);
int ReadTexChunk(FILE *file, struct NSChunkTex *chunk);
int WriteTexChunk(FILE *file, struct NSChunkTex *chunk);
int TexChunkToPNGSpritesheet(const struct NSChunkTex *chunk, const char *path);
int TexChunkToDirectory(const struct NSChunkTex *chunk, const char *directory);
void FreeTexChunk(struct NSChunkTex *chunk);
int LengthToTexSize(uint32_t x);
int TexChunkPalettesToDirectory(const struct NSChunkTex *chunk, const char *directory);

void SortTexturesLexico(struct NSChunkTex *chunk);
void SortPalettesLexico(struct NSChunkTex *chunk);
void SortTexturesSmart(struct NSChunkTex *chunk);
void SortPalettesSmart(struct NSChunkTex *chunk);

#endif // NITROBTX_TEX_H
