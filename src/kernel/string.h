#ifndef _STRING_H_
#define _STRING_H_

#include "stddef.h"

void *memcpy(void *destination, void const *source, size_t size);
void *memset(void *buf, char ch, size_t size);
char const *strchr(char const *string, char character);
int strcmp(char const *string1, char const *string2);
int strncmp(char const *string1, char const *string2, size_t n);
char *strcpy(char *destination, char const *source);
unsigned int strlen(char const *string);

#endif
