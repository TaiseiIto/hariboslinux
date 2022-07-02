#ifndef _FPU_H_
#define _FPU_H_

#include "task.h"

Task const *get_fpu_user_task(void);
void init_fpu(void);
void release_fpu(void);
void take_fpu(void);

#endif

