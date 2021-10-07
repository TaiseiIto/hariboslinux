#include "cmos.h"
#include "io.h"

#define PORT_CMOS_REGISTER	0x0070
#define PORT_CMOS_PORT		0x0071

unsigned char read_cmos_register(unsigned char register_number)
{
	outb(PORT_CMOS_REGISTER, register_number);
	return inb(PORT_CMOS_PORT);
}

void write_cmos_register(unsigned char register_number, unsigned char value)
{
	outb(PORT_CMOS_REGISTER, register_number);
	outb(PORT_CMOS_PORT, value);
}

