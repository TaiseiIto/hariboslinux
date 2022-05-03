#include "common.h"
#include "stdlib.h"

void free(void *address)
{
	UNUSED_ARGUMENT(address);
}

void *malloc(size_t size)
{
	UNUSED_ARGUMENT(size);
	return NULL;
}

