// Programmable Interrupt Controller

// master PIC
// |---IRQ0
// |---IRQ1
// |---IRQ2---slave PIC
// |---IRQ3
// |---IRQ4
// |---IRQ5
// |---IRQ6
// |---IRQ7

// slave PIC
// |---IRQ8
// |---IRQ9
// |---IRQ10
// |---IRQ11
// |---IRQ12
// |---IRQ13
// |---IRQ14
// |---IRQ15

#include "io.h"
#include "pic.h"

// Interrupt Mask Registers Ports
#define PIC0_IMR 0x0021
#define PIC1_IMR 0x00a1

// Initial Control Word Ports
#define PIC0_ICW1 0x0020
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC0_OCW2 0x0020
#define PIC1_ICW1 0x00a0
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1
#define PIC1_OCW2 0x00a0

#define IRQ0_DISABLE 0x01
#define IRQ1_DISABLE 0x02
#define IRQ2_DISABLE 0x04
#define IRQ3_DISABLE 0x08
#define IRQ4_DISABLE 0x10
#define IRQ5_DISABLE 0x20
#define IRQ6_DISABLE 0x40
#define IRQ7_DISABLE 0x80
#define IRQ8_DISABLE 0x01
#define IRQ9_DISABLE 0x02
#define IRQ10_DISABLE 0x04
#define IRQ11_DISABLE 0x08
#define IRQ12_DISABLE 0x10
#define IRQ13_DISABLE 0x20
#define IRQ14_DISABLE 0x40
#define IRQ15_DISABLE 0x80

void finish_interruption(unsigned char irq)
{
	if(0x00 <= irq && irq < 0x08)outb(PIC0_OCW2, irq + 0x60);
	else if(0x08 <= irq && irq < 0xf0)
	{
		outb(PIC1_OCW2, irq + 0x58);
		finish_interruption(IRQ_SLAVE_PIC);
	}
}

void init_pic(void)
{
	// The master PIC disables all interrupts
	outb(PIC0_IMR, IRQ0_DISABLE | IRQ1_DISABLE | IRQ2_DISABLE | IRQ3_DISABLE | IRQ4_DISABLE | IRQ5_DISABLE | IRQ6_DISABLE | IRQ7_DISABLE);
	// The slave PIC disables all interrupts
	outb(PIC1_IMR, IRQ8_DISABLE | IRQ9_DISABLE | IRQ10_DISABLE | IRQ11_DISABLE | IRQ12_DISABLE | IRQ13_DISABLE | IRQ14_DISABLE | IRQ15_DISABLE);

	// Init the master PIC
	outb(PIC0_ICW1, 0x11);	// edge trigger mode
	outb(PIC0_ICW2, 0x20);	// IRQ0~7 executes INT 0x20~0x27
	outb(PIC0_ICW3, 0x04);	// IRQ2---slave PIC
	outb(PIC0_ICW4, 0x01);	// non buffer mode

	// Init the slave PIC
	outb(PIC1_ICW1, 0x11);	// edge trigger mode
	outb(PIC1_ICW2, 0x28);	// IRQ8~15 executes INT 0x28~0x2f
	outb(PIC1_ICW3, 0x02);	// IRQ2---slave PIC
	outb(PIC1_ICW4, 0x01);	// non buffer mode

	// Enable master PIC interrupts
	outb(PIC0_IMR, IRQ0_DISABLE | IRQ3_DISABLE | IRQ4_DISABLE | IRQ5_DISABLE | IRQ6_DISABLE | IRQ7_DISABLE);
	// Enable slave PIC interrupts
	outb(PIC1_IMR, IRQ8_DISABLE | IRQ9_DISABLE | IRQ10_DISABLE | IRQ11_DISABLE | IRQ14_DISABLE | IRQ15_DISABLE);
}

