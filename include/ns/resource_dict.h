#ifndef NITROBTX_RESOURCE_DICT_H
#define NITROBTX_RESOURCE_DICT_H

#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "ns/resource_name.h"

struct NSResourceDict {
    uint16_t entryCount;
    uint16_t dataEntrySize;
    void *data;
    struct ResourceName *names;
};

int CalcNSResourceDictSize(struct NSResourceDict *dict);
enum ErrorCode ReadNSResourceDict(FILE *file, struct NSResourceDict *dict);
enum ErrorCode WriteNSResourceDict(FILE *file, struct NSResourceDict *dict);
struct ResourceName GetResourceNamesPrefix(struct NSResourceDict *dict);
void FreeResourceDict(struct NSResourceDict *dict);

#endif // NITROBTX_RESOURCE_DICT_H
