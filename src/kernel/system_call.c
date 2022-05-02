#include "common.h"
#include "io.h"
#include "shell.h"
#include "system_call.h"

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp)
{
	UNUSED_ARGUMENT(ebx);
	UNUSED_ARGUMENT(ecx);
	UNUSED_ARGUMENT(edx);
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	#define SYSTEM_CALL_EAX_EXIT 0x0001
	switch(eax)
	{
	case SYSTEM_CALL_EAX_EXIT:
		cli();
		hlt();
		break;
	default:
		break;
	}
	return 0;
}

