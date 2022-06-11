#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "stddef.h"

#define _countof(array) (sizeof(array) / sizeof(array[0]))

int atoi(char const *digits);
void exit(int status);
void free(void *address);
void *malloc(size_t size);
void srand(unsigned int seed);

#endif

