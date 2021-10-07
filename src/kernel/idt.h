#ifndef _IDT_H_
#define _IDT_H_

// Interrupt Descriptor Table

typedef struct
{
	unsigned short offset_low;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	#define INTERRUPT_DESCRIPTOR_PRESENT 0x80
		// Set to 0 for unused interrupts
	#define INTERRUPT_DESCRIPTOR_PRIVILEGE 0x60
		// Gate call protection.
		// Specifies which privilege level the calling descriptor minimum should have.
		// So hardware and CPU interrupts can be protected from being called out of userspace.
	#define INTERRUPT_DESCRIPTOR_TASK_GATE 0x05
	#define INTERRUPT_DESCRIPTOR_INTERRUPT_GATE 0x0e
	#define INTERRUPT_DESCRIPTOR_TRAP_GATE 0x0f
	unsigned short offset_high;
} InterruptDescriptor;

void init_idt(void);

void set_gate(InterruptDescriptor *interrupt_descriptor, void (*handler)(void), unsigned char flags);

#endif

