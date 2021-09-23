#ifndef _PIC_H_
#define _PIC_H_

// Programmable Interrupt Controller

#define IRQ_TIMER			0x00
#define IRQ_KEYBOARD			0x01
#define IRQ_SLAVE_PIC			0x02
#define IRQ_COM2			0x03
#define IRQ_COM1			0x04
#define IRQ_LPT2			0x05
#define IRQ_DISK			0x06
#define IRQ_LPT1			0x07
#define IRQ_REAL_TIME_CLOCK		0x08
#define IRQ_PERIPHERAL0			0x09
#define IRQ_PERIPHERAL1			0x0a
#define IRQ_PERIPHERAL2			0x0b
#define IRQ_MOUSE			0x0c
#define IRQ_FLOATING_POINT_UNIT		0x0d
#define IRQ_PRIMARY_ATA_HARD_DISK	0x0e
#define IRQ_SECONDARY_ATA_HARD_DISK	0x0f

void finish_interruption(unsigned char irq);
void init_pic(void);
void slave_pic_interrupt_handler(void);

#endif

