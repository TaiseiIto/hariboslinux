#ifndef _STDIO_H_
#define _STDIO_H_

#include "stddef.h"

#define _STRING(x) #x
#define _STRING2(x) _STRING(x)
#define ERROR() puts("ERROR " __FILE__ " : " _STRING2(__LINE__) "\n")

int fclose(unsigned int file_descriptor);
unsigned int fopen(char const *file_name, char const *mode);
int fputs(char const *string, unsigned int file_descriptor);
int printf(const char *format, ...);
int puts(char const *string);

#endif

