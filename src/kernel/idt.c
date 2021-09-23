#include "gdt.h"
#include "idt.h"
#include "interrupt_gates.h"
#include "io.h"
#include "serial.h"

#define IDT_ADDR ((InterruptDescriptor *)0x00268000)

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
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000000, interrupt_gate0x00, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// debug exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000001, interrupt_gate0x01, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// non maskable interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000002, interrupt_gate0x02, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// breakpoint exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000003, interrupt_gate0x03, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// overflow exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000004, interrupt_gate0x04, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// bound range exceeded exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000005, interrupt_gate0x05, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// invalid opcode exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000006, interrupt_gate0x06, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// device not available exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000007, interrupt_gate0x07, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// double fault exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000008, interrupt_gate0x08, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// coprocessor segment overrun exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000009, interrupt_gate0x09, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// invalid TSS exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000a, interrupt_gate0x0a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// segment not present exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000b, interrupt_gate0x0b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// stack segment fault exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000c, interrupt_gate0x0c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// general protection fault exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000d, interrupt_gate0x0d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// page fault exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000e, interrupt_gate0x0e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000000f, interrupt_gate0x0f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// x87 floating point exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000010, interrupt_gate0x10, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// alignment check exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000011, interrupt_gate0x11, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// machine check exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000012, interrupt_gate0x12, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// SIMD floating point exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000013, interrupt_gate0x13, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// virtualization exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000014, interrupt_gate0x14, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000015, interrupt_gate0x15, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000016, interrupt_gate0x16, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000017, interrupt_gate0x17, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000018, interrupt_gate0x18, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000019, interrupt_gate0x19, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001a, interrupt_gate0x1a, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001b, interrupt_gate0x1b, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001c, interrupt_gate0x1c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001d, interrupt_gate0x1d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// security exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001e, interrupt_gate0x1e, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// reserved exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000001f, interrupt_gate0x1f, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// timer interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000020, interrupt_gate0x20, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// kerboard interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000021, interrupt_gate0x21, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// slave pic interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x00000022, interrupt_gate0x22, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// mouse interrupt
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000002c, interrupt_gate0x2c, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);
	// FPU error exception
	set_gate((InterruptDescriptor *)0x00000000 + 0x0000002d, interrupt_gate0x2d, INTERRUPT_DESCRIPTOR_INTERRUPT_GATE);

	// load IDT
	lidt(0x07ff, IDT_ADDR);

	// check IDT
	for(InterruptDescriptor const *source = (InterruptDescriptor const *)0x00000000; source < (InterruptDescriptor const *)0x00000800; source++)
	{
		InterruptDescriptor interrupt_descriptor_checker;
		reads(idt_segment_selector, source, &interrupt_descriptor_checker, sizeof(interrupt_descriptor_checker));
		printf_serial_polling("Interrupt Descriptor %#010x\n", (unsigned int)source / sizeof(*source));
		printf_serial_polling("\t offset_low\t%#06x\n", interrupt_descriptor_checker.offset_low);
		printf_serial_polling("\t selector\t%#06x\n", interrupt_descriptor_checker.selector);
		printf_serial_polling("\t zero\t\t%#04x\n", interrupt_descriptor_checker.zero);
		printf_serial_polling("\t flags\t\t%#04x\n", interrupt_descriptor_checker.flags);
		printf_serial_polling("\t offset_high\t%#06x\n", interrupt_descriptor_checker.offset_high);
	}
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

