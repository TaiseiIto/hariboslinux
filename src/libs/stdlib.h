#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "stdbool.h"
#include "stddef.h"

#define _countof(array) (sizeof(array) / sizeof(array[0]))

int atoi(char const *digits);
void exit(int status);
void free(void *address);
// It's equal to init_by_array in http://math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/mt19937ar.c
void init_mersenne_twister(unsigned int *key, unsigned int length);
void *malloc(size_t size);
int rand(void);
void srand(unsigned int seed);

#endif

