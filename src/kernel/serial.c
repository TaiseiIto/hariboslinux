#include "io.h"
#include "serial.h"

#define COM1 0x03f8

// print LF
void new_line_serial_polling(void)
{
	put_char_serial_polling('\n');
}

// printf
void printf_serial_polling(char const *format, ...)
{
	int arg_num = 1;
	char character;
	char const *input_string;
	int integer;
	int integer_destroyable;
	unsigned int unsigned_integer;
	unsigned int unsigned_integer_destroyable;
	while(*format)
	{
		if(*format == '%')
		{
			unsigned char flags = 0;
			#define SPRINTF_MINUS_FLAG 0x01
			#define SPRINTF_TYPE_FLAG 0x02
			#define SPRINTF_ZERO_FLAG 0x04
			unsigned int length = 0;
			unsigned int num_of_digits = 0;
			format++;
			switch(*format)
			{
			case '%':
				put_char_serial_polling('%');
				format++;
				continue;
			case '#':
				flags |= SPRINTF_TYPE_FLAG;
				format++;
				break;
			}
			if(*format == '0')
			{
				flags |= SPRINTF_ZERO_FLAG;
				format++;
			}
			while('0' <= *format && *format <= '9')
			{
				length *= 10;
				length += *format - '0';
				format++;
			}
			switch(*format)
			{
			case 'c':
				character = get_variadic_arg(arg_num++);
				put_char_serial_polling(character);
				break;
			case 'd':
				integer = get_variadic_arg(arg_num++);
				if(integer < 0)
				{
					put_char_serial_polling('-');
					integer *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer)for(num_of_digits = 0; 0 < integer_destroyable; integer_destroyable /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial_polling(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable /= 10;
					put_char_serial_polling('0' + integer_destroyable % 10);
					num_of_digits--;
				}
				break;
			case 's':
				input_string = (char const *)get_variadic_arg(arg_num++);
				while(*input_string && (!length || num_of_digits++ < length))put_char_serial_polling(*input_string++);
				break;
			case 'u':
				unsigned_integer = get_variadic_arg(arg_num++);
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial_polling(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 10;
					put_char_serial_polling('0' + unsigned_integer_destroyable % 10);
					num_of_digits--;
				}
				break;
			case 'x':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_serial_polling('0');
					if(0 < length)length--;
					put_char_serial_polling('x');
					if(0 < length)length--;
				}
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial_polling(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 0x10;
					put_char_serial_polling(unsigned_integer_destroyable % 0x10 < 10 ? '0' + unsigned_integer_destroyable % 0x10 : 'a' + unsigned_integer_destroyable % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 'X':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_serial_polling('0');
					if(0 < length)length--;
					put_char_serial_polling('X');
					if(0 < length)length--;
				}
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial_polling(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 0x10;
					put_char_serial_polling(unsigned_integer_destroyable % 0x10 < 10 ? '0' + unsigned_integer_destroyable % 0x10 : 'A' + unsigned_integer_destroyable % 0x10 - 10);
					num_of_digits--;
				}
				break;
			default:
				break;
			}
		}
		else put_char_serial_polling(*format);
		format++;
	}
}

// print value as hexadecimal
void print_byte_hex_serial_polling(unsigned char value)
{
	char high = value >> 4 & 0x0f;
	char low = value & 0x0f;
	if(high < 10)put_char_serial_polling(high + '0');
	else put_char_serial_polling(high + 'a' - 10);
	if(low < 10)put_char_serial_polling(low + '0');
	else put_char_serial_polling(low + 'a' - 10);
}

// print value as hexadecimal
void print_dword_hex_serial_polling(unsigned int value)
{
	print_word_hex_serial_polling((unsigned short)(value >> 16));
	print_word_hex_serial_polling((unsigned short)(value & 0x0000ffff));
}

// print string to serial port COM1
void print_serial_polling(char *string)
{
	while(*string)put_char_serial_polling(*string++);
}

// print value as hexadecimal
void print_word_hex_serial_polling(unsigned short value)
{
	print_byte_hex_serial_polling((unsigned char)(value >> 8));
	print_byte_hex_serial_polling((unsigned char)(value & 0x00ff));
}

// print a character to serial port COM1
void put_char_serial_polling(char character)
{
	while(!(inb(COM1 + 0x0005) & 0x20));
	outb(COM1, character);
}

