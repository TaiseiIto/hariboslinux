#include "gdt.h"
#include "idt.h"
#include "io.h"

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
}

