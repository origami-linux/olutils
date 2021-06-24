#ifndef OLUTILS_IO_H
#define OLUTILS_IO_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "olutils/bool.h"

char *read_file(char *path, long *len_ref, bool binary);
void write_file(char *path, char *buffer, long length);

#ifdef __cplusplus
}
#endif

#endif
