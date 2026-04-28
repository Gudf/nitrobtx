#ifndef NITROBTX_HELP_H
#define NITROBTX_HELP_H

#include <stdio.h>

void PrintVersion(FILE* out);
void PrintUsage(FILE* out, char* progPath);
void PrintOptionHelp(FILE* out, char option);

#endif // NITROBTX_HELP_H
