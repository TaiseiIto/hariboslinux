#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "serial.h"

#define IDT_ADDR ((InterruptDescriptor *)0x00007400)

void init_idt(void)
{
	InterruptDescriptor interrupt_descriptor;
	InterruptDescriptor *end_of_idt = (InterruptDescriptor *)0x00000800;
	interrupt_descriptor.offset_low = 0x0000;
	interrupt_descriptor.selector = 0x0000;
	interrupt_descriptor.zero = 0x00;
	interrupt_descriptor.flags = 0x00;
	interrupt_descriptor.offset_high = 0x0000;
	for(InterruptDescriptor *destination = 0x00000000; destination < end_of_idt; destination++)writes((void *)&interrupt_descriptor, idt_segment_selector, destination, sizeof(interrupt_descriptor));

	// devide by 0 exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000000, interrupt_handler0x00, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// bound range exceeded exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000005, interrupt_handler0x05, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// invalid opcode exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000006, interrupt_handler0x06, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// device not available exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000007, interrupt_handler0x07, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// double fault exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000008, interrupt_handler0x08, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// coprocessor segment overrun exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000009, interrupt_handler0x09, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// kerboard interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000021, interrupt_handler0x21, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);

	// load IDT
	lidt(0x07ff, IDT_ADDR);

	// check IDT
	for(InterruptDescriptor const *source = (InterruptDescriptor const *)0x00000000; source < (InterruptDescriptor const *)0x00000800; source++)
	{
		InterruptDescriptor interrupt_descriptor_checker;
		reads(idt_segment_selector, source, &interrupt_descriptor_checker, sizeof(interrupt_descriptor_checker));
		printf_serial_polling("Interrupt Descriptor %#010X\n", (unsigned int)source / sizeof(*source));
		printf_serial_polling("\t offset_low\t%#06X\n", interrupt_descriptor_checker.offset_low);
		printf_serial_polling("\t selector\t%#06X\n", interrupt_descriptor_checker.selector);
		printf_serial_polling("\t zero\t\t%#04X\n", interrupt_descriptor_checker.zero);
		printf_serial_polling("\t flags\t\t%#04X\n", interrupt_descriptor_checker.flags);
		printf_serial_polling("\t offset_high\t%#06X\n", interrupt_descriptor_checker.offset_high);
	}
	new_line_serial_polling();
}

void set_gate(InterruptDescriptor *interrupt_descriptor, void (*handler)(void), unsigned char flags)
{
	InterruptDescriptor temp;
	temp.offset_low = (unsigned short)((unsigned int)handler & 0x0000ffff);
	temp.selector = kernel_code_segment_selector;
	temp.zero = 0;
	temp.flags = flags | INTERRUPT_DESCRIPTOR_PRESENT;
	temp.offset_high = (unsigned short)((unsigned int)handler >> 16 & 0x0000ffff);
	writes((void *)&temp, idt_segment_selector, (void *)interrupt_descriptor, sizeof(temp));
}

