#include "stdio.h"
#include "io.h"
#include "serial.h"

int sprintf(char *str, const char *format, ...)
{
	int arg_num = 2;
	char character;
	char const *input_string;
	int integer;
	unsigned int unsigned_integer;
	while(*format)
	{
		if(*format == '%')
		{
			unsigned char flags = 0;
			#define HARIB_SPRINTF_MINUS_FLAG 0x01
			#define HARIB_SPRINTF_TYPE_FLAG 0x02
			#define HARIB_SPRINTF_ZERO_FLAG 0x04
			unsigned int length = 0;
			int num_of_digits = 0;
			char *digit;
			format++;
			switch(*format)
			{
			case '%':
				*str++ = '%';
				format++;
				continue;
			case '#':
				flags |= HARIB_SPRINTF_TYPE_FLAG;
				format++;
				break;
			}
			if(*format == '0')
			{
				flags |= HARIB_SPRINTF_ZERO_FLAG;
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
				*str++ = character;
				break;
			case 'd':
				integer = get_variadic_arg(arg_num++);
				if(integer < 0)
				{
					*str++ = '-';
					integer *= -1;
					if(0 < length)length--;
					flags |= HARIB_SPRINTF_MINUS_FLAG;
				}
				do
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0' + integer % 10;
					if(num_of_digits == length)break;
				}while(integer /= 10);
				if(flags & HARIB_SPRINTF_ZERO_FLAG)while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0';
				}
				else while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > (flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str); digit--)*digit = *(digit - 1);
					*(flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str) = ' ';
				}
				str += num_of_digits;
				break;
			case 's':
				input_string = (char const *)get_variadic_arg(arg_num++);
				while(*input_string && (!length || num_of_digits++ < length))*str++ = *input_string++;
				break;
			case 'u':
				unsigned_integer = get_variadic_arg(arg_num++);
				do
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0' + unsigned_integer % 10;
					if(num_of_digits == length)break;
				}while(unsigned_integer /= 10);
				if(flags & HARIB_SPRINTF_ZERO_FLAG)while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0';
				}
				else while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > (flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str); digit--)*digit = *(digit - 1);
					*(flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str) = ' ';
				}
				str += num_of_digits;
				break;
			case 'x':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & HARIB_SPRINTF_TYPE_FLAG)
				{
					*str++ = '0';
					if(0 < length)length--;
					*str++ = 'x';
					if(0 < length)length--;
				}
				do
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = (unsigned_integer % 0x10 < 10) ? '0' + unsigned_integer % 0x10 : unsigned_integer % 0x10 - 10 + 'a';
					if(num_of_digits == length)break;
				}while(unsigned_integer /= 0x10);
				if(flags & HARIB_SPRINTF_ZERO_FLAG)while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0';
				}
				else while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > (flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str); digit--)*digit = *(digit - 1);
					*(flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str) = ' ';
				}
				str += num_of_digits;
				break;
			case 'X':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & HARIB_SPRINTF_TYPE_FLAG)
				{
					*str++ = '0';
					if(0 < length)length--;
					*str++ = 'X';
					if(0 < length)length--;
				}
				do
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = (unsigned_integer % 0x10 < 10) ? '0' + unsigned_integer % 0x10 : unsigned_integer % 0x10 - 10 + 'A';
					if(num_of_digits == length)break;
				}while(unsigned_integer /= 0x10);
				if(flags & HARIB_SPRINTF_ZERO_FLAG)while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > str; digit--)*digit = *(digit - 1);
					*str = '0';
				}
				else while(num_of_digits < length)
				{
					for(digit = str + num_of_digits++; digit > (flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str); digit--)*digit = *(digit - 1);
					*(flags & HARIB_SPRINTF_MINUS_FLAG ? str - 1 : str) = ' ';
				}
				str += num_of_digits;
				break;
			default:
				return -1;
			}
		}
		else *str++ = *format;
		format++;
	}
	*str = '\0';
	return 0;
}

