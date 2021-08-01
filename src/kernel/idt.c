#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "serial.h"

#define IDT_ADDR ((void *)0x00007400)

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

	// load IDT
	lidt(0x07ff, (InterruptDescriptor *)IDT_ADDR);

	// check IDT
	for(InterruptDescriptor const *source = (InterruptDescriptor const *)0x00000000; source < (InterruptDescriptor const *)0x00000800; source++)
	{
		InterruptDescriptor interrupt_descriptor_checker;
		reads(idt_segment_selector, source, &interrupt_descriptor_checker, sizeof(interrupt_descriptor_checker));
		printf_serial_polling("Interrupt Descriptor %#06X\n", source);
		printf_serial_polling("\t offset_low\t%#06X\n", interrupt_descriptor_checker.offset_low);
		printf_serial_polling("\t selector\t%#06X\n", interrupt_descriptor_checker.selector);
		printf_serial_polling("\t zero\t\t%#04X\n", interrupt_descriptor_checker.zero);
		printf_serial_polling("\t flags\t\t%#04X\n", interrupt_descriptor_checker.flags);
		printf_serial_polling("\t offset_high\t%#06X\n", interrupt_descriptor_checker.offset_high);
	}
	new_line_serial_polling();
}

