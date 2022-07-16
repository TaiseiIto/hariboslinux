#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

void alignment_check_exception_handler(void);
void bound_range_exceeded_exception_handler(void);
void breakpoint_exception_handler(void);
void coprocessor_segment_overrun_exception_handler(void);
void debug_exception_handler(void);
void device_not_available_exception_handler(void);
void devide_by_zero_exception_handler(void);
void double_fault_exception_handler(void);
void fpu_error_exception_handler(void);
void general_protection_fault_exception_handler(unsigned int error_code);
void invalid_opcode_exception_handler(void);
void invalid_TSS_exception_handler(void);
void machine_check_exception_handler(void);
void non_maskable_interrupt_handler(void);
void overflow_exception_handler(void);
void page_fault_exception_handler(void);
void reserved_exception_handler0x00(void);
void reserved_exception_handler0x01(void);
void reserved_exception_handler0x02(void);
void reserved_exception_handler0x03(void);
void reserved_exception_handler0x04(void);
void reserved_exception_handler0x05(void);
void reserved_exception_handler0x06(void);
void reserved_exception_handler0x07(void);
void reserved_exception_handler0x08(void);
void reserved_exception_handler0x09(void);
void reserved_exception_handler0x0a(void);
void security_exception_handler(void);
void segment_not_present_exception_handler(void);
void simd_floating_point_exception_handler(void);
void stack_segment_fault_exception_handler(void);
void virtualization_exception_handler(void);
void x87_floating_point_exception_handler(void);

#endif

