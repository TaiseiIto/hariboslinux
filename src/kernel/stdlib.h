#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "stdbool.h"
#include "stddef.h"

#define _countof(array) (sizeof(array) / sizeof(array[0]))
#define max(a, b) ((a) < (b) ? (b) : (a))
#define min(a, b) ((a) < (b) ? (a) : (b))

#endif

