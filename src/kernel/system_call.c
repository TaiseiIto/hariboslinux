#include "common.h"
#include "io.h"
#include "system_call.h"

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp)
{
	UNUSED_ARGUMENT(eax);
	UNUSED_ARGUMENT(ebx);
	UNUSED_ARGUMENT(ecx);
	UNUSED_ARGUMENT(edx);
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	cli();
	hlt();
	return 0;
}

