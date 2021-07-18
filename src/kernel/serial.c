#include "io.h"
#include "serial.h"

#define COM1 0x03f8

void new_line_serial_polling(void)
{
	putchar_serial_polling('\n');
}

// print value as hexadecimal
void print_byte_hex_serial_polling(unsigned char value)
{
	char high = value >> 4 & 0x0f;
	char low = value & 0x0f;
	if(high < 10)putchar_serial_polling(high + '0');
	else putchar_serial_polling(high + 'a');
	if(low < 10)putchar_serial_polling(low + '0');
	else putchar_serial_polling(low + 'a');
}

// print string to serial port COM1
void print_serial_polling(char *string)
{
	while(*string)putchar_serial_polling(*string++);
}

// print value as hexadecimal
void print_word_hex_serial_polling(unsigned short value)
{
	print_byte_hex_serial_polling((unsigned char)(value >> 8));
	print_byte_hex_serial_polling((unsigned char)(value & 0x00ff));
}

// print a character to serial port COM1
void putchar_serial_polling(char character)
{
	while(!(inb(COM1 + 0x0005) & 0x20));
	outb(COM1, character);
}

