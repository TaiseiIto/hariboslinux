#include "io.h"
#include "serial.h"

#define COM1 0x03f8

void new_line_serial_polling(void)
{
	putchar_serial_polling('\n');
}

// print string to serial port COM1
void print_serial_polling(char *string)
{
	while(*string)
	{
		putchar_serial_polling(*string);
		string++;
	}
}

// print a character to serial port COM1
void putchar_serial_polling(char character)
{
	while(!(io_inb(COM1 + 0x0005) & 0x20));
	io_outb(COM1, character);
}

