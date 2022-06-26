#include "chain_string.h"
#include "common.h"
#include "io.h"
#include "limits.h"
#include "memory.h"
#include "serial.h"
#include "stdio.h"
#include "string.h"

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
	char const *format = (char const *)get_caller_variadic_arg(arg_pos++);
	ChainString *output_chain_string = create_chain_string("");
	ChainCharacter *previous_character;
	ChainCharacter *sign_character;
	VariadicArg arg;
	unsigned int arg_size;
	unsigned char flags;
	#define FORMAT_FLAG_BLANK_SIGN		0x01
	#define FORMAT_FLAG_EXPLICIT_PLUS	0x02
	#define FORMAT_FLAG_EXPLICIT_RADIX	0x04
	#define FORMAT_FLAG_LEFT_JUSTIFIED	0x08
	#define FORMAT_FLAG_PRECISION_SPECIFIED 0x10
	#define FORMAT_FLAG_THOUSAND_SEPARATOR	0x20
	#define FORMAT_FLAG_WIDTH_SPECIFIED	0x40
	#define FORMAT_FLAG_ZERO_FILLED		0x80
	unsigned char format_phase;
	#define	FORMAT_PHASE_FLAGS	0x00
	#define	FORMAT_PHASE_WIDTH	0x01
	#define	FORMAT_PHASE_PRECISION	0x02
	#define	FORMAT_PHASE_MODIFIER	0x03
	#define	FORMAT_PHASE_TYPE	0x04
	#define	FORMAT_PHASE_FINISHED	0x05
	unsigned int precision;
	unsigned int width;
	unsigned int output_length;
	unsigned int num_of_digits;
	while(*format)switch(*format)
	{
	case '%':
		previous_character = output_chain_string->last_character;
		sign_character = output_chain_string->last_character;
		arg_size = 4;
		flags = 0;
		format_phase = FORMAT_PHASE_FLAGS;
		precision = 0;
		width = 0;
		output_length = 0;
		num_of_digits = 0;
		while(format_phase == FORMAT_PHASE_FLAGS)switch(*++format)
		{
		case ' ':
			flags |= FORMAT_FLAG_BLANK_SIGN;
			break;
		case '#':
			flags |= FORMAT_FLAG_EXPLICIT_RADIX;
			break;
		case '%':
			insert_char_back(output_chain_string, output_chain_string->last_character, '%');
			format_phase = FORMAT_PHASE_FINISHED;
			break;
		case '\'':
			flags |= FORMAT_FLAG_THOUSAND_SEPARATOR;
			break;
		case '+':
			flags |= FORMAT_FLAG_EXPLICIT_PLUS;
			break;
		case '-':
			flags |= FORMAT_FLAG_LEFT_JUSTIFIED;
			break;
		default:
			format_phase = FORMAT_PHASE_WIDTH;
			format--;
			break;
		}
		while(format_phase <= FORMAT_PHASE_WIDTH)switch(*++format)
		{
		case '*':
			width = get_caller_variadic_arg(arg_pos++);
			flags |= FORMAT_FLAG_WIDTH_SPECIFIED;
			format_phase = FORMAT_PHASE_PRECISION;
			break;
		default:
			if('0' <= *format && *format <= '9')
			{
				width *= 10;
				width += (int)(*format - '0');
				if(*format == '0' && !width)flags |= FORMAT_FLAG_ZERO_FILLED;
				flags |= FORMAT_FLAG_WIDTH_SPECIFIED;
			}
			else
			{
				format_phase = FORMAT_PHASE_PRECISION;
				format--;
			}
			break;
		}
		while(format_phase <= FORMAT_PHASE_PRECISION)switch(*++format)
		{
		case '*':
			precision = get_caller_variadic_arg(arg_pos++);
			flags |= FORMAT_FLAG_PRECISION_SPECIFIED;
			format_phase = FORMAT_PHASE_MODIFIER;
			break;
		case '.':
			break;
		default:
			if('0' <= *format && *format <= '9')
			{
				precision *= 10;
				precision += (int)(*format - '0');
				flags |= FORMAT_FLAG_PRECISION_SPECIFIED;
			}
			else
			{
				format_phase = FORMAT_PHASE_MODIFIER;
				format--;
			}
			break;
		}
		if(format_phase <= FORMAT_PHASE_MODIFIER)
		{
			switch(*++format)
			{
			case 'h':
				arg_size = 2;
				switch(*++format)
				{
				case 'h':
					arg_size = 1;
					break;
				default:
					format--;
					break;
				}
				break;
			case 'l':
				arg_size = 4;
				switch(*++format)
				{
				case 'l':
					arg_size = 8;
					break;
				default:
					format--;
					break;
				}
				break;
			case 'z':
				arg_size = 4;
				break;
			default:
				format--;
				break;
			}
			format_phase = FORMAT_PHASE_TYPE;
		}
		if(format_phase <= FORMAT_PHASE_TYPE)
		{
			arg.unsigned_ints[0] = get_caller_variadic_arg(arg_pos++);
			if(arg_size == 8)arg.unsigned_ints[1] = get_caller_variadic_arg(arg_pos++);
			switch(*++format)
			{
			case 'c':
			case 'C':
				insert_char_back(output_chain_string, output_chain_string->last_character, arg.chars[0]);
				break;
			case 'd':
			case 'i':
				if(arg_size < 8)arg.ints[1] = -(arg.ints[0] < 0);
				if(0 <= arg.long_long_int)
				{
					if(flags & FORMAT_FLAG_EXPLICIT_PLUS)
					{
						insert_char_back(output_chain_string, output_chain_string->last_character, '+');
						output_length++;
						sign_character = output_chain_string->last_character;
					}
					else if(flags & FORMAT_FLAG_BLANK_SIGN)
					{
						insert_char_back(output_chain_string, output_chain_string->last_character, ' ');
						output_length++;
						sign_character = output_chain_string->last_character;
					}
				}
				else
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '-');
					output_length++;
					sign_character = output_chain_string->last_character;
					arg.long_long_int *= -1;
				}
				while(arg.long_long_int)
				{
					insert_char_back(output_chain_string, sign_character, arg.long_long_int % 10 + '0');
					output_length++;
					num_of_digits++;
					arg.long_long_int /= 10;
				}
				if(!(flags & FORMAT_FLAG_PRECISION_SPECIFIED))precision = 1;
				while(num_of_digits < precision)
				{
					insert_char_back(output_chain_string, sign_character, '0');
					output_length++;
					num_of_digits++;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, flags & FORMAT_FLAG_ZERO_FILLED ? '0' : ' ');
					output_length++;
				}
				break;
			case 'n':
				*arg.unsigned_int_pointer = output_chain_string->length;
				break;
			case 'o':
				if(arg_size == 4)arg.unsigned_ints[1] = 0;
				if(flags & FORMAT_FLAG_EXPLICIT_RADIX)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '0');
					output_length++;
					sign_character = output_chain_string->last_character;
				}
				while(arg.unsigned_long_long_int)
				{
					insert_char_back(output_chain_string, sign_character, arg.unsigned_long_long_int % 8 + '0');
					output_length++;
					num_of_digits++;
					arg.unsigned_long_long_int /= 8;
				}
				if(!(flags & FORMAT_FLAG_PRECISION_SPECIFIED))precision = 1;
				while(num_of_digits < precision)
				{
					insert_char_back(output_chain_string, sign_character, '0');
					output_length++;
					num_of_digits++;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			case 'p':
				insert_char_array_back(output_chain_string, output_chain_string->last_character, "0x");
				output_length += 2;
				sign_character = output_chain_string->last_character;
				for(unsigned int digit_num = 0; digit_num < sizeof(void *) * CHAR_BIT / 4; digit_num++)
				{
					unsigned char digit = arg.unsigned_ints[0] % 0x10;
					insert_char_back(output_chain_string, sign_character, digit < 10 ? digit + '0' : digit - 10 + 'a');
					output_length++;
					arg.unsigned_ints[0] /= 0x10;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			case 's':
				while((!(flags & FORMAT_FLAG_PRECISION_SPECIFIED) && *arg.string) || (flags & FORMAT_FLAG_PRECISION_SPECIFIED && (output_length < precision && *arg.string)))
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, *arg.string++);
					output_length++;
				}
				while(flags & FORMAT_FLAG_WIDTH_SPECIFIED && output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			case 'u':
				if(arg_size < 8)arg.unsigned_ints[1] = 0;
				if(flags & FORMAT_FLAG_EXPLICIT_PLUS)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, '+');
					output_length++;
					sign_character = output_chain_string->last_character;
				}
				else if(flags & FORMAT_FLAG_BLANK_SIGN)
				{
					insert_char_back(output_chain_string, output_chain_string->last_character, ' ');
					output_length++;
					sign_character = output_chain_string->last_character;
				}
				while(arg.unsigned_long_long_int)
				{
					insert_char_back(output_chain_string, sign_character, arg.unsigned_long_long_int % 10 + '0');
					output_length++;
					num_of_digits++;
					arg.unsigned_long_long_int /= 10;
				}
				if(!(flags & FORMAT_FLAG_PRECISION_SPECIFIED))precision = 1;
				while(num_of_digits < precision)
				{
					insert_char_back(output_chain_string, sign_character, '0');
					output_length++;
					num_of_digits++;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			case 'x':
				if(arg_size < 8)arg.unsigned_ints[1] = 0;
				if(flags & FORMAT_FLAG_EXPLICIT_RADIX)
				{
					insert_char_array_back(output_chain_string, output_chain_string->last_character, "0x");
					output_length += 2;
					sign_character = output_chain_string->last_character;
				}
				while(arg.unsigned_long_long_int)
				{
					unsigned char digit = arg.unsigned_long_long_int % 0x10;
					insert_char_back(output_chain_string, sign_character, digit < 10 ? digit + '0' : digit - 10 + 'a');
					output_length++;
					num_of_digits++;
					arg.unsigned_long_long_int /= 0x10;
				}
				if(!(flags & FORMAT_FLAG_PRECISION_SPECIFIED))precision = 1;
				while(num_of_digits < precision)
				{
					insert_char_back(output_chain_string, sign_character, '0');
					output_length++;
					num_of_digits++;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			case 'X':
				if(arg_size < 8)arg.unsigned_ints[1] = 0;
				if(flags & FORMAT_FLAG_EXPLICIT_RADIX)
				{
					insert_char_array_back(output_chain_string, output_chain_string->last_character, "0X");
					output_length += 2;
					sign_character = output_chain_string->last_character;
				}
				while(arg.unsigned_long_long_int)
				{
					unsigned char digit = arg.unsigned_long_long_int % 0x10;
					insert_char_back(output_chain_string, sign_character, digit < 10 ? digit + '0' : digit - 10 + 'A');
					output_length++;
					num_of_digits++;
					arg.unsigned_long_long_int /= 0x10;
				}
				if(!(flags & FORMAT_FLAG_PRECISION_SPECIFIED))precision = 1;
				while(num_of_digits < precision)
				{
					insert_char_back(output_chain_string, sign_character, '0');
					output_length++;
					num_of_digits++;
				}
				while(output_length < width)
				{
					insert_char_back(output_chain_string, previous_character, ' ');
					output_length++;
				}
				break;
			}
		}
		format++;
		break;
	default:
		insert_char_back(output_chain_string, output_chain_string->last_character, *format++);
		break;
	}
	return output_chain_string;
}

ChainString *create_format_chain_string(char const *format, ...)
{
	UNUSED_ARGUMENT(format);
	return create_caller_format_chain_string(0);
}

char *create_format_char_array(char const *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(((unsigned int)&format - (unsigned int)&format) / sizeof(unsigned int));
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	delete_chain_string(output_chain_string);
	return output_string;
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
	for(unsigned int i = 0; i < length; i++)
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

void replace_chain_string(ChainString *string, char const *target, char const *replacement)
{
	ChainCharacter *target_candidate = NULL;
	char const *target_reader = target;
	if(!string)ERROR(); // The string doesn't exist.
	for(ChainCharacter *character = string->first_character; character; character = character->next)
	{
		if(target_candidate)
		{
			if(*target_reader)
			{
				if(character->character != *target_reader)
				{
					target_reader = target;
					target_candidate = NULL;
					if(character->character == *target)
					{
						target_candidate = character;
						target_reader = target;
					}
				}
			}
			else
			{
				insert_char_array_front(string, target_candidate, replacement);
				character = target_candidate->previous;
				delete_chars(string, target_candidate, strlen(target));
				target_candidate = NULL;
			}
		}
		else if(character->character == *target)
		{
			target_candidate = character;
			target_reader = target;
		}
		target_reader++;
	}
}

