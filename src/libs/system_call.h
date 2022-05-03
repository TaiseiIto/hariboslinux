#ifndef _SYSTEM_CALL_H_
#define _SYSTEM_CALL_H_

#include "stddef.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

unsigned int system_call_exit(unsigned int return_value);
unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

#endif

