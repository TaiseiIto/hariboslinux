#ifndef _IO_H_
#define _IO_H_

#include "gdt.h"
#include "idt.h"

// disable all interrupts
void cli(void);

// get eflags register
unsigned int get_eflags(void);
#define INTERRUPT_FLAG 0x00000020

// get nth arg in variadic arg function
// the first arg is 0th
unsigned int get_variadic_arg(unsigned int n);

// wait for next interrupt
void hlt(void);

// inb	$address,	%al
unsigned char inb(unsigned short address);

// inw	$address,	%ax
unsigned short inw(unsigned short address);

// inl	$address,	%eax
unsigned int inl(unsigned short address);

// devide by 0 exception handler
void interrupt_handler0x00(void);

// debug exception handler
void interrupt_handler0x01(void);

// non maskable interrupt handler
void interrupt_handler0x02(void);

// breakpoint exception handler
void interrupt_handler0x03(void);

// bound range exceeded exception handler
void interrupt_handler0x05(void);

// invalid opcode exception handler
void interrupt_handler0x06(void);

// device not available exception handler
void interrupt_handler0x07(void);

// double fault exception handler
void interrupt_handler0x08(void);

// coprocessor segment overrun exception handler
void interrupt_handler0x09(void);

// invalid TSS exception handler
void interrupt_handler0x0a(void);

// segment not present exception handler
void interrupt_handler0x0b(void);

// stack segment fault exception handler
void interrupt_handler0x0c(void);

// general protection fault exception handler
void interrupt_handler0x0d(void);

// page fault exception handler
void interrupt_handler0x0e(void);

// x87 floating point exception handler
void interrupt_handler0x10(void);

// alignment check exception handler
void interrupt_handler0x11(void);

// machine check exception handler
void interrupt_handler0x12(void);

// SIMD floating point exception handler
void interrupt_handler0x13(void);

// virtualization exception handler
void interrupt_handler0x14(void);

// security exception handler
void interrupt_handler0x1e(void);

// keyboard interrupt handler
void interrupt_handler0x21(void);

// FPU error exception handler
void interrupt_handler0x2d(void);

// lgdt
void lgdt(unsigned int limit, SegmentDescriptor *begin);

// lidt
void lidt(unsigned int limit, InterruptDescriptor *begin);

// outb	$value,	$address
void outb(unsigned short address, unsigned char value);

// outw	$value,	$address
void outw(unsigned short address, unsigned short value);

// outl	$value,	$address
void outl(unsigned short address, unsigned int value);

// movb	$segment:($address),%al
unsigned char readb(unsigned short segment, void *address);

// movw	$segment:($address),%ax
unsigned short readw(unsigned short segment, void *address);

// movl	$segment:($address),%eax
unsigned int readl(unsigned short segment, void *address);

// read size bytes from $source_segment:$source to $destination 
void reads(unsigned short source_segment, void const *source, void *destination, unsigned int size);

// enable all interrupts
void sti(void);

// movb	$value,$segment:($address)
void writeb(unsigned short segment, void *address, unsigned char value);

// movw	$value,$segment:($address)
void writew(unsigned short segment, void *address, unsigned char value);

// movl	$value,$segment:($address)
void writel(unsigned short segment, void *address, unsigned char value);

// write size bytes from $source to $destination_segment:$destination
void writes(void const *source, unsigned short destination_segment, void *destination, unsigned int size);

#endif

