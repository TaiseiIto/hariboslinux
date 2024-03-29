#include "io.h"
#include "system_call.h"

#define SYSTEM_CALL_EXIT	0x00000001
#define SYSTEM_CALL_READ	0x00000003
#define SYSTEM_CALL_WRITE	0x00000004
#define SYSTEM_CALL_OPEN	0x00000005
#define SYSTEM_CALL_CLOSE	0x00000006

int system_call_close(unsigned int file_descriptor)
{
	return system_call(SYSTEM_CALL_CLOSE, file_descriptor, 0, 0, 0, 0, 0);
}

int system_call_exit(int return_value)
{
	return system_call(SYSTEM_CALL_EXIT, return_value, 0, 0, 0, 0, 0);
}

unsigned int system_call_open(char const *file_name, unsigned int flags)
{
	return (unsigned int)system_call(SYSTEM_CALL_OPEN, (int)file_name, (int)flags, 0, 0, 0, 0);
}

size_t system_call_read(unsigned int file_descriptor, void *buffer, size_t count)
{
	return (size_t)system_call(SYSTEM_CALL_READ, (int)file_descriptor, (int)buffer, (int)count, 0, 0, 0);
}

int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count)
{
	return system_call(SYSTEM_CALL_WRITE, (int)file_descriptor, (int)buffer, (int)count, 0, 0, 0);
}

