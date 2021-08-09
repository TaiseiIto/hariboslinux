#ifndef _PIC_H_
#define _PIC_H_

// Programmable Interrupt Controller

#define IRQ_TIMER	0x00
#define IRQ_KEYBOARD	0x01
#define IRQ_SLAVE_PIC	0x02
#define IRQ_COM2	0x03
#define IRQ_COM1	0x04
#define IRQ_MOUSE	0x0b

void finish_interruption(unsigned char irq);
void init_pic(void);

#endif

