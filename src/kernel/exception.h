#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

void bound_range_exceeded_exception_handler(void);
void coprocessor_segment_overrun_exception_handler(void);
void device_not_available_exception_handler(void);
void devide_by_zero_exception_handler(void);
void double_fault_exception_handler(void);
void invalid_opcode_exception_handler(void);

#endif

