#ifndef _SYSTEM_CALL_H_
#define _SYSTEM_CALL_H_

#include "stddef.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

int system_call_close(unsigned int file_descriptor);
int system_call_exit(int return_value);
unsigned int system_call_open(char const *file_name, unsigned int flags);
#define SYSTEM_CALL_OPEN_FLAG_READ_ONLY 0x01
int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

#endif

