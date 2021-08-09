#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

void alignment_check_exception_handler(void);
void bound_range_exceeded_exception_handler(void);
void coprocessor_segment_overrun_exception_handler(void);
void device_not_available_exception_handler(void);
void devide_by_zero_exception_handler(void);
void double_fault_exception_handler(void);
void general_protection_fault_exception_handler(void);
void invalid_opcode_exception_handler(void);
void invalid_TSS_exception_handler(void);
void page_fault_exception_handler(void);
void segment_not_present_exception_handler(void);
void stack_segment_fault_exception_handler(void);
void x87_floating_point_exception_handler(void);

#endif

