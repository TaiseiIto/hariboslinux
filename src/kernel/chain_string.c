#include "chain_string.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "stdio.h"

ChainString *create_chain_string(char const *string)
{
	ChainString *chain_string = malloc(sizeof(*chain_string));
	chain_string->first_character = NULL;
	chain_string->last_character = NULL;
	chain_string->length = 0;
	if(string)insert_char_array_front(chain_string, NULL, string);
	return chain_string;
}

ChainString *create_chain_substring(ChainCharacter *first_character, ChainCharacter *last_character)
{
	ChainString *chain_string = malloc(sizeof(*chain_string));
	chain_string->first_character = NULL;
	chain_string->last_character = NULL;
	chain_string->length = 0;
	if(!first_character && !last_character)return chain_string;
	else if(first_character && last_character)for(ChainCharacter *character = first_character; character != last_character->next; character = character->next)insert_char_front(chain_string, NULL, character->character);
	else ERROR(); // The arguments first_character and last_character are inconsistent.
	return chain_string;
}

char *create_char_array_from_chain_string(ChainString const *string)
{
	char *char_array;
	char *writer;
	if(!string)ERROR(); // The string doesn't exist.
	char_array = malloc(string->length + 1);
	writer = char_array;
	for(ChainCharacter *position = string->first_character; position; position = position->next)*writer++ = position->character;
	*writer = '\0';
	return char_array;
}

ChainString *create_caller_format_chain_string(unsigned int format_arg_pos)
{
	unsigned int arg_pos = format_arg_pos;
	char character;
	char const *format = (char const *)get_caller_variadic_arg(arg_pos++);
	char const *input_string;
	VariadicArg integer;
	VariadicArg integer_destroyable;
	ChainString *output_chain_string = create_chain_string("");
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
			unsigned int accuracy = 0;
			unsigned int length = 0;
			unsigned int num_of_digits = 0;
			ChainCharacter *previous_character;
			format++;
			switch(*format)
			{
			case '%':
				insert_char_back(output_chain_string, output_chain_string->last_character, '%');
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
			case '.':
				while('0' <= *format && *format <= '9')
				{
					accuracy *= 10;
					accuracy += *format - '0';
					format++;
				}
				break;
			}
			switch(*format)
			{
			case 'l':
				flags |= SPRINTF_LONG_FLAG;
				format++;
				break;
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
				break;
			}
			switch(*format)
			{
			case 'c':
				character = get_caller_variadic_arg(arg_pos++);
				insert_char_back(output_chain_string, output_chain_string->last_character, character);
				break;
			case 'd':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = get_caller_variadic_arg(arg_pos++);
				}
				else
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.ints[1] = -(integer.ints[0] < 0);
				}
				if(integer.long_long_int < 0)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '-');
					integer.long_long_int *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer.long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.long_long_int; integer_destroyable.long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.long_long_int /= 10;
					insert_char_back(output_chain_string, output_chain_string->last_character, '0' + integer_destroyable.long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'p':
				length = 10;
				integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
				integer.unsigned_ints[1] = 0;
				insert_char_back(output_chain_string, output_chain_string->last_character, '0');
				if(0 < length)length--;
				insert_char_back(output_chain_string, output_chain_string->last_character, 'x');
				if(0 < length)length--;
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '0');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					insert_char_back(output_chain_string, output_chain_string->last_character, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 's':
				previous_character = output_chain_string->last_character;
				input_string = (char const *)get_caller_variadic_arg(arg_pos++);
				while(*input_string && (!accuracy || num_of_digits++ < accuracy))insert_char_back(output_chain_string, output_chain_string->last_character, *input_string++);
				while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					num_of_digits++;
				}
				break;
			case 'u':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = get_caller_variadic_arg(arg_pos++);
				}
				else
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = 0;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 10;
					insert_char_back(output_chain_string, output_chain_string->last_character, '0' + integer_destroyable.unsigned_long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'x':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = get_caller_variadic_arg(arg_pos++);
				}
				else
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '0');
					if(0 < length)length--;
					insert_char_back(output_chain_string, output_chain_string->last_character, 'x');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					insert_char_back(output_chain_string, output_chain_string->last_character, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 'X':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = get_caller_variadic_arg(arg_pos++);
				}
				else
				{
					integer.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '0');
					if(0 < length)length--;
					insert_char_back(output_chain_string, output_chain_string->last_character, 'X');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					insert_char_back(output_chain_string, output_chain_string->last_character, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'A' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			default:
				break;
			}
		}
		else insert_char_back(output_chain_string, output_chain_string->last_character, *format);
		format++;
	}
	return output_chain_string;
}

void delete_chain_string(ChainString *string)
{
	if(!string)ERROR(); // The string doesn't exist.
	delete_chars(string, string->first_character, string->length);
	free(string);
}

void delete_char(ChainString *string, ChainCharacter *position)
{
	if(!string || !position)ERROR(); // The string or the position doesn't exist.
	if(!string->length)ERROR(); // There is no char in the string.
	if(position == string->first_character)string->first_character = position->next;
	if(position == string->last_character)string->last_character = position->previous;
	if(position->next)position->next->previous = position->previous;
	if(position->previous)position->previous->next = position->next;
	string->length--;
	free(position);
}

void delete_chars(ChainString *string, ChainCharacter *position, unsigned int length)
{
	ChainCharacter *next_position;
	if(!string)ERROR(); // The string doesn't exitst.
	for(int i = 0; i < length; i++)
	{
		next_position = position->next;
		if(!position)ERROR(); // Deletion length overflows.
		delete_char(string, position);
		position = next_position;
	}
}

void insert_char_front(ChainString *string, ChainCharacter *position, char wedge)
{
	ChainCharacter *new_chain_character;
	if(!string)ERROR(); // The string doesn't exist.
	new_chain_character = malloc(sizeof(*new_chain_character));
	new_chain_character->character = wedge;
	if(!string->first_character && !string->last_character)
	{
		new_chain_character->previous = NULL;
		new_chain_character->next = NULL;
		string->first_character = new_chain_character;
		string->last_character = new_chain_character;
	}
	else if(string->first_character && string->last_character)
	{
		if(!position) // Insert the wedge after the last character.
		{
			new_chain_character->previous = string->last_character;
			new_chain_character->next = NULL;
			string->last_character->next = new_chain_character;
			string->last_character = new_chain_character;
		}
		else
		{
			new_chain_character->previous = position->previous;
			new_chain_character->next = position;
			if(position->previous)position->previous->next = new_chain_character;
			position->previous = new_chain_character;
			if(position == string->first_character)string->first_character = new_chain_character;
		}
	}
	else ERROR(); // The string is broken.
	string->length++;
}

void insert_char_array_front(ChainString *string, ChainCharacter *position, char const *wedge)
{
	for(; *wedge; wedge++)insert_char_front(string, position, *wedge);
}

void insert_chain_string_front(ChainString *string, ChainCharacter *position, ChainString const *wedge)
{
	for(ChainCharacter const *wedge_character = wedge->first_character; wedge_character; wedge_character = wedge_character->next)insert_char_front(string, position, wedge_character->character);
}

void insert_char_back(ChainString *string, ChainCharacter *position, char wedge)
{
	ChainCharacter *new_chain_character;
	if(!string)ERROR(); // The string doesn't exist.
	new_chain_character = malloc(sizeof(*new_chain_character));
	new_chain_character->character = wedge;
	if(!string->first_character && !string->last_character)
	{
		new_chain_character->previous = NULL;
		new_chain_character->next = NULL;
		string->first_character = new_chain_character;
		string->last_character = new_chain_character;
	}
	else if(string->first_character && string->last_character)
	{
		if(!position) // Insert the wedge before the first character
		{
			new_chain_character->previous = NULL;
			new_chain_character->next = string->first_character;
			string->first_character->previous = new_chain_character;
			string->first_character = new_chain_character;
		}
		else
		{
			new_chain_character->previous = position;
			new_chain_character->next = position->next;
			if(position->next)position->next->previous = new_chain_character;
			position->next = new_chain_character;
			if(position == string->last_character)string->last_character = new_chain_character;
		}
	}
	else ERROR(); // The string is broken.
	string->length++;
}

void insert_char_array_back(ChainString *string, ChainCharacter *position, char const *wedge)
{
	for(; *wedge; wedge++)
	{
		insert_char_back(string, position, *wedge);
		if(position)position = position->next;
		else position = string->first_character;
	}
}

void insert_chain_string_back(ChainString *string, ChainCharacter *position, ChainString const *wedge)
{
	for(ChainCharacter const *wedge_character = wedge->first_character; wedge_character; wedge_character = wedge_character->next)
	{
		insert_char_back(string, position, wedge_character->character);
		if(position)position = position->next;
		else position = string->first_character;
	}
}

