#ifndef NITROBTX_NS_RESOURCE_TREE_H
#define NITROBTX_NS_RESOURCE_TREE_H

#include <stdint.h>
#include <stdlib.h>

#include "ns/resource_name.h"

struct NSDictTreeNode {
    uint8_t bitIndex;
    uint8_t leftChild;
    uint8_t rightChild;
    uint8_t entryIndex;
};

int MakeTreeFromResNames(const struct ResourceName entries[], uint8_t numEntries, struct NSDictTreeNode **out, size_t *outSize);

#endif // NITROBTX_NS_RESOURCE_TREE_H
