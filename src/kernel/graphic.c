#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "limits.h"
#include "serial.h"
#include "stdio.h"

#define TAB_LENGTH 4

unsigned short vram_segment_selector;
VideoInformation video_information;

// fill box
// x and y can be negative
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color)
{
	for(short y_i = y; y_i < y + (short)height; y_i++)
	{
		if(y_i < 0)
		{
			y_i = -1;
			continue;
		}
		if(video_information.height <= y_i)break;
		for(short x_i = x; x_i < x + (short)width; x_i++)
		{
			if(x_i < 0)
			{
				x_i = -1;
				continue;
			}
			if(video_information.width <= x_i)break;
			put_dot((unsigned short)x_i, (unsigned short)y_i, color);
		}
	}
}

// init screen
void init_screen(void)
{
	reads(video_information_segment_selector, (void *)0x00000000, (void *)&video_information, sizeof(video_information));
	vram_segment_selector = set_segment(video_information.frame_buffer, video_information.pitch * video_information.height, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	new_line_serial();
	printf_serial("video_information.attributes = %#06x\n", video_information.attributes);
	printf_serial("video_information.pitch = %#06x\n", video_information.pitch);
	printf_serial("video_information.width = %#06x\n", video_information.width);
	printf_serial("video_information.height = %#06x\n", video_information.height);
	printf_serial("video_information.bits_per_pixel = %#04x\n", video_information.bits_per_pixel);
	printf_serial("video_information.memory_model = %#04x\n", video_information.memory_model);
	printf_serial("video_information.red_mask = %#04x\n", video_information.red_mask);
	printf_serial("video_information.red_positioin = %#04x\n", video_information.red_position);
	printf_serial("video_information.green_mask = %#04x\n", video_information.green_mask);
	printf_serial("video_information.green_positioin = %#04x\n", video_information.green_position);
	printf_serial("video_information.blue_mask = %#04x\n", video_information.blue_mask);
	printf_serial("video_information.blue_positioin = %#04x\n", video_information.blue_position);
	printf_serial("video_information.frame_buffer = %#010x\n", video_information.frame_buffer);
}

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background)
{
	for(short y_i = 0; y_i < CHAR_HEIGHT; y_i++)
	{
		if(y + y_i < 0)
		{
			y_i = -y - 1;
			continue;
		}
		if(video_information.height <= y + y_i)break;
		for(short x_i = 0; x_i < CHAR_WIDTH; x_i++)
		{
			if(x + x_i < 0)
			{
				x_i = -x - 1;
				continue;
			}
			if(video_information.width <= x + x_i)break;
			if(get_font_pixel(character, x_i, y_i))put_dot((unsigned short)(x + x_i), (unsigned short)(y + y_i), foreground);
			else put_dot((unsigned short)(x + x_i), (unsigned short)(y + y_i), background);
		}
	}
}

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot(unsigned short x, unsigned short y, Color color)
{
	writeb(vram_segment_selector, (void *)(video_information.pitch * y + video_information.bits_per_pixel / CHAR_BIT * x + video_information.red_position / CHAR_BIT), color.red);
	writeb(vram_segment_selector, (void *)(video_information.pitch * y + video_information.bits_per_pixel / CHAR_BIT * x + video_information.green_position / CHAR_BIT), color.green);
	writeb(vram_segment_selector, (void *)(video_information.pitch * y + video_information.bits_per_pixel / CHAR_BIT * x + video_information.blue_position / CHAR_BIT), color.blue);
}

// put string at screen(x, y)
void put_string(char const *string, unsigned short x, unsigned short y, Color foreground, Color background)
{
	unsigned int char_pos_x = 0;
	unsigned int char_pos_y = 0;
	unsigned int char_pos_x_destroyable;
	while(*string)
	{
		switch(*string)
		{
		case '\t':
			char_pos_x_destroyable = (char_pos_x / TAB_LENGTH + 1) * TAB_LENGTH;
			fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char(*string, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
			char_pos_x++;
			break;
		}
		string++;
	}
}

// printf to screen
void printf_screen(unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...)
{
	int arg_num = 5;
	char character;
	char const *input_string;
	VariadicArg integer;
	VariadicArg integer_destroyable;
	unsigned int char_pos_x = 0;
	unsigned int char_pos_y = 0;
	unsigned int char_pos_x_destroyable;
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
				put_char('%', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
				char_pos_x++;
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
				switch(character)
				{
				case '\t':
					char_pos_x_destroyable = (char_pos_x / TAB_LENGTH + 1) * TAB_LENGTH;
					fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
					char_pos_x = char_pos_x_destroyable;
					break;
				case '\n':
					char_pos_x = 0;
					char_pos_y++;
					break;
				case ' ':
					fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
					char_pos_x++;
					break;
				default:
					put_char(character, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					break;
				}
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
					put_char('-', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					integer.long_long_int *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer.long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.long_long_int; integer_destroyable.long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.long_long_int /= 10;
					put_char('0' + integer_destroyable.long_long_int % 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			case 'p':
				length = 10;
				integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
				integer.unsigned_ints[1] = 0;
				put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
				char_pos_x++;
				if(0 < length)length--;
				put_char('x', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
				char_pos_x++;
				if(0 < length)length--;
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			case 's':
				input_string = (char const *)get_variadic_arg(arg_num++);
				while(*input_string && (!length || num_of_digits++ < length))
				{
					switch(*input_string)
					{
					case '\t':
						char_pos_x_destroyable = (char_pos_x / TAB_LENGTH + 1) * TAB_LENGTH;
						fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
						char_pos_x = char_pos_x_destroyable;
						break;
					case '\n':
						char_pos_x = 0;
						char_pos_y++;
						break;
					case ' ':
						fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
						char_pos_x++;
						break;
					default:
						put_char(*input_string, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
						char_pos_x++;
						break;
					}
					input_string++;
				}
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
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 10;
					put_char('0' + integer_destroyable.unsigned_long_long_int % 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
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
					put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
					put_char('x', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
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
					put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
					put_char('X', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'A' + integer_destroyable.unsigned_long_long_int % 0x10 - 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			default:
				break;
			}
		}
		else switch(*format)
		{
		case '\t':
			char_pos_x_destroyable = (char_pos_x / TAB_LENGTH + 1) * TAB_LENGTH;
			fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char(*format, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
			char_pos_x++;
			break;
		}
		format++;
	}
}

