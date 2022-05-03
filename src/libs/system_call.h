#ifndef _SYSTEM_CALL_H_
#define _SYSTEM_CALL_H_

#include "stddef.h"

unsigned int system_call_exit(unsigned int return_value);
unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

#endif

