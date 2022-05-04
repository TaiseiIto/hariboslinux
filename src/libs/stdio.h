#ifndef _STDIO_H_
#define _STDIO_H_

#include "stddef.h"

int fputs(char const *string, unsigned int file_descriptor);
int printf(const char *format, ...);
int puts(char const *string);

#endif

