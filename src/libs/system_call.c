#include "io.h"
#include "system_call.h"

#define SYSTEM_CALL_EXIT	0x00000001
#define SYSTEM_CALL_WRITE	0x00000004

unsigned int system_call_exit(unsigned int return_value)
{
	return system_call(SYSTEM_CALL_EXIT, return_value, 0, 0, 0, 0, 0);
}

unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count)
{
	return system_call(SYSTEM_CALL_WRITE, file_descriptor, (unsigned int)buffer, (unsigned int)count, 0, 0, 0);
}

