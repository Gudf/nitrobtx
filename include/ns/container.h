#ifndef NITROBTX_NS_CONTAINER_H
#define NITROBTX_NS_CONTAINER_H
#include <stdint.h>
#include <stdio.h>

#include "common.h"
#include "ns/chunks/tex.h"

enum ContainerType {
    CONTAINER_BTX0 = 0,
    CONTAINER_BMD0,
    NUM_CONTAINER_TYPES,
    CONTAINER_TYPE_UNSUPPORTED,
};

enum ChunkType {
    CHUNK_TEX0 = 0,
    CHUNK_MDL0,
    NUM_CHUNK_TYPES,
    CHUNK_TYPE_UNSUPPORTED,
};

struct NSChunk {
    enum ChunkType type;
    union {
        void *chunkPtr;
        struct NSChunkTex *texChunk;
    };
};

struct NSContainerHeader {
    union Magic magic;
    uint16_t byteOrder;
    uint16_t version;
    uint32_t fileSize;
    uint16_t headerSize;
    uint16_t chunkCount;
};

struct NSContainer {
    union Magic type;
    int chunkCount;
    struct NSChunk *chunks;
};

extern const union Magic containerMagics[NUM_CONTAINER_TYPES];
extern const union Magic chunkMagics[NUM_CHUNK_TYPES];

struct NSContainer NSContainer_Init(enum ContainerType type);
enum ErrorCode NSContainer_ReadFromFile(struct NSContainer *container, FILE *file);
enum ErrorCode NSContainer_WriteToFile(struct NSContainer *container, FILE *file);
void NSContainer_Free(struct NSContainer *container);

#endif // NITROBTX_NS_CONTAINER_H
