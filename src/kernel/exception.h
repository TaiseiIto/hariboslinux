#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

void alignment_check_exception_handler(void);
void bound_range_exceeded_exception_handler(void);
void breakpoint_exception_handler(void);
void control_protection_exception_handler(unsigned int error_code);
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
void reserved_exception_handler0x0f(void);
void reserved_exception_handler0x16(void);
void reserved_exception_handler0x17(void);
void reserved_exception_handler0x18(void);
void reserved_exception_handler0x19(void);
void reserved_exception_handler0x1a(void);
void reserved_exception_handler0x1b(void);
void reserved_exception_handler0x1f(void);
void security_exception_handler(void);
void segment_not_present_exception_handler(void);
void simd_floating_point_exception_handler(void);
void stack_segment_fault_exception_handler(void);
void virtualization_exception_handler(void);
void vmm_communication_exception_handler(unsigned int error_code);
void x87_floating_point_exception_handler(void);

#endif

