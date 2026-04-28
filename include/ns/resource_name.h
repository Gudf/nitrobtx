#ifndef NITROBTX_NS_RESOURCE_NAME_H
#define NITROBTX_NS_RESOURCE_NAME_H

#include <stdint.h>

#define RES_NAME_LENGTH 16

struct ResourceName {
    uint8_t asChars[RES_NAME_LENGTH];
};

void CopyToResName(struct ResourceName *name, const char str[RES_NAME_LENGTH]);
int CompareResNamesSmart(const struct ResourceName *first, const struct ResourceName *second);
int CompareResNamesLexico(const struct ResourceName *first, const struct ResourceName *second);
int LevenshteinDist(const struct ResourceName *first, const struct ResourceName *second);

#endif // NITROBTX_NS_RESOURCE_NAME_H
