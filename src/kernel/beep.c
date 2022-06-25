#include "beep.h"
#include "io.h"
#include "pit.h"

#define BEEP_STATUS		0x61
#define BEEP_SWITCH		0x03
#define BEEP_SET_FREQUENCY	0xb6

void beep_off(void)
{
	outb(BEEP_STATUS, inb(BEEP_STATUS) & ~BEEP_SWITCH);
}

void beep_on(void)
{
	outb(BEEP_STATUS, inb(BEEP_STATUS) | BEEP_SWITCH);
}

void set_beep_frequency(unsigned int frequency/* mHz */)
{
	unsigned short interval_rate = 1000 * pit_interrupt_frequency / frequency;
	outb(PIT_COMMAND, BEEP_SET_FREQUENCY);
	outb(PIT_CHANNEL2, (unsigned char)(interval_rate & 0x00ff));
	outb(PIT_CHANNEL2, (unsigned char)(interval_rate >> 8 & 0x00ff));
}

