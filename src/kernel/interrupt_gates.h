#ifndef _INTERRUPT_GATES_H_
#define _INTERRUPT_GATES_H_

// devide by 0 exception gate
void interrupt_gate0x00(void);

// debug exception gate
void interrupt_gate0x01(void);

// non maskable interrupt gate
void interrupt_gate0x02(void);

// breakpoint exception gate
void interrupt_gate0x03(void);

// overflow exception gate
void interrupt_gate0x04(void);

// bound range exceeded exception gate
void interrupt_gate0x05(void);

// invalid opcode exception gate
void interrupt_gate0x06(void);

// device not available exception gate
void interrupt_gate0x07(void);

// double fault exception gate
void interrupt_gate0x08(void);

// coprocessor segment overrun exception gate
void interrupt_gate0x09(void);

// invalid TSS exception gate
void interrupt_gate0x0a(void);

// segment not present exception gate
void interrupt_gate0x0b(void);

// stack segment fault exception gate
void interrupt_gate0x0c(void);

// general protection fault exception gate
void interrupt_gate0x0d(void);

// page fault exception gate
void interrupt_gate0x0e(void);

// reserved exception gate
void interrupt_gate0x0f(void);

// x87 floating point exception gate
void interrupt_gate0x10(void);

// alignment check exception gate
void interrupt_gate0x11(void);

// machine check exception gate
void interrupt_gate0x12(void);

// SIMD floating point exception gate
void interrupt_gate0x13(void);

// virtualization exception gate
void interrupt_gate0x14(void);

// reserved exception gate
void interrupt_gate0x15(void);

// reserved exception gate
void interrupt_gate0x16(void);

// reserved exception gate
void interrupt_gate0x17(void);

// reserved exception gate
void interrupt_gate0x18(void);

// reserved exception gate
void interrupt_gate0x19(void);

// reserved exception gate
void interrupt_gate0x1a(void);

// reserved exception gate
void interrupt_gate0x1b(void);

// reserved exception gate
void interrupt_gate0x1c(void);

// reserved exception gate
void interrupt_gate0x1d(void);

// security exception gate
void interrupt_gate0x1e(void);

// security exception gate
void interrupt_gate0x1f(void);

// keyboard interrupt gate
void interrupt_gate0x21(void);

// mouse interrupt gate
void interrupt_gate0x2c(void);

// FPU error exception gate
void interrupt_gate0x2d(void);

#endif

