#ifndef _IO_H_
#define _IO_H_

#include "gdt.h"
#include "idt.h"
#include "task.h"

void call_application
(
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	void *application_stack_floor;
	TaskStatusSegment *kernel_task_status_segment;
);

// disable all interrupts
void cli(void);

// get nth arg in caller variadic arg function
// the first arg is 0th
unsigned int get_caller_variadic_arg(unsigned int);

// get eflags register
unsigned int get_eflags(void);
#define EFLAGS_NOTHING		0x00000002
#define EFLAGS_INTERRUPT_FLAG	0x00000200

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

// lcall $segment, %address
void lcall(unsigned int address, unsigned short segment);

// lgdt
void lgdt(unsigned int limit, SegmentDescriptor *begin);

// lidt
void lidt(unsigned int limit, InterruptDescriptor *begin);

// ljmp $segment, %address
void ljmp(unsigned int address, unsigned short segment);

// ltr
void ltr(unsigned int tr);

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

