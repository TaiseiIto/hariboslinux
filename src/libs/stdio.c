#include "common.h"
#include "stdio.h"

size_t fwrite(void const *buffer, size_t size, size_t n, void *file_descriptor)
{
	UNUSED_ARGUMENT(buffer);
	UNUSED_ARGUMENT(file_descriptor);
	return size * n;
}

