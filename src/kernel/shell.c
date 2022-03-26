#include "io.h"
#include "memory.h"
#include "shell.h"
#include "stdio.h"

Shell *create_shell(Console *console)
{
	Shell *shell;
	shell = malloc(sizeof(*shell));
	if(console)
	{
		shell->console = console;
		shell->type = SHELL_TYPE_CONSOLE;
	}
	else
	{
		shell->console = NULL;
		shell->type = SHELL_TYPE_SERIAL;
	}
	return shell;
}

void delete_shell(Shell *shell)
{
	printf_serial("Delete shell %p\n", shell);
	free(shell);
}

void *execute_command(Shell *shell, char const *command)
{
	printf_shell(shell, "%s shell %p executes command \"%s\"\n", shell->type == SHELL_TYPE_CONSOLE ? "Console" : "Serial" , shell, command);
	return NULL;
}

void print_shell(Shell *shell, char const *string)
{
	for(; *string; string++)put_char_shell(shell, *string);
}

void printf_shell(Shell *shell, char const *format, ...)
{
	int arg_num = 2;
	char character;
	char const *input_string;
	VariadicArg integer;
	VariadicArg integer_destroyable;
	while(*format)
	{
		if(*format == '%')
		{
			unsigned char flags = 0;
			#define SPRINTF_MINUS_FLAG 0x01
			#define SPRINTF_TYPE_FLAG 0x02
			#define SPRINTF_ZERO_FLAG 0x04
			#define SPRINTF_LONG_FLAG 0x08
			#define SPRINTF_LONG_LONG_FLAG 0x10
			unsigned int length = 0;
			unsigned int num_of_digits = 0;
			format++;
			switch(*format)
			{
			case '%':
				put_char_shell(shell, '%');
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
			case 'l':
				flags |= SPRINTF_LONG_FLAG;
				format++;
			}
			switch(*format)
			{
			case 'l':
				if(flags | SPRINTF_LONG_FLAG)
				{
					flags &= ~SPRINTF_LONG_FLAG;
					flags |= SPRINTF_LONG_LONG_FLAG;
					format++;
				}
			}
			switch(*format)
			{
			case 'c':
				character = get_variadic_arg(arg_num++);
				put_char_shell(shell, character);
				break;
			case 'd':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.ints[1] = -(integer.ints[0] < 0);
				}
				if(integer.long_long_int < 0)
				{
					put_char_shell(shell, '-');
					integer.long_long_int *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer.long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.long_long_int; integer_destroyable.long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_shell(shell, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.long_long_int /= 10;
					put_char_shell(shell, '0' + integer_destroyable.long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'p':
				length = 10;
				integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
				integer.unsigned_ints[1] = 0;
				put_char_shell(shell, '0');
				if(0 < length)length--;
				put_char_shell(shell, 'x');
				if(0 < length)length--;
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_shell(shell, '0');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_shell(shell, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 's':
				input_string = (char const *)get_variadic_arg(arg_num++);
				while(*input_string && (!length || num_of_digits++ < length))put_char_shell(shell, *input_string++);
				break;
			case 'u':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_shell(shell, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 10;
					put_char_shell(shell, '0' + integer_destroyable.unsigned_long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'x':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_shell(shell, '0');
					if(0 < length)length--;
					put_char_shell(shell, 'x');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_shell(shell, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_shell(shell, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 'X':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_shell(shell, '0');
					if(0 < length)length--;
					put_char_shell(shell, 'X');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_shell(shell, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_shell(shell, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'A' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			default:
				break;
			}
		}
		else put_char_shell(shell, *format);
		format++;
	}
}

void put_char_shell(Shell *shell, char character)
{
	switch(shell->type)
	{
	case SHELL_TYPE_CONSOLE:
		text_box_insert_char_back(shell->console->text_box, shell->console->text_box->last_position, character);
		break;
	case SHELL_TYPE_SERIAL:
		put_char_serial(character);
		break;
	default:
		ERROR(); // Invalid shell type.
		break;
	}
}

