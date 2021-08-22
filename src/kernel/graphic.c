#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "serial.h"

#define TAB_LENGTH 4

typedef struct
{
	unsigned short attributes;
	unsigned char window_a;
	unsigned char window_b;
	unsigned short granularity;
	unsigned short window_size;
	unsigned short segment_a;
	unsigned short segment_b;
	unsigned int win_func_ptr;
	unsigned short pitch;
	unsigned short width;
	unsigned short height;
	unsigned char w_char;
	unsigned char y_char;
	unsigned char planes;
	unsigned char bits_per_pixel;
	unsigned char banks;
	unsigned char memory_model;
	unsigned char bank_size;
	unsigned char image_pages;
	unsigned char reserved;
	unsigned char red_mask;
	unsigned char red_position;
	unsigned char green_mask;
	unsigned char green_position;
	unsigned char blue_mask;
	unsigned char blue_position;
	unsigned char reserved_mask;
	unsigned char reserved_position;
	unsigned char direct_color_attributes;
	unsigned int frame_buffer;
	unsigned int off_screen_mem_off;
	unsigned int off_screen_mem_size;
} __attribute__((packed)) VideoInformation;

VideoInformation video_information;

// fill box
// x and y can be negative
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color)
{
}

// init screen
void init_screen(void)
{
	reads(video_information_segment_selector, (void *)0x00000000, (void *)&video_information, sizeof(video_information));
	printf_serial_polling("video_information.attributes = %#06X\n", video_information.attributes);
}

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background)
{
}

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot(unsigned short x, unsigned short y, Color color)
{
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
	int integer;
	int integer_destroyable;
	unsigned int unsigned_integer;
	unsigned int unsigned_integer_destroyable;
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
				integer = get_variadic_arg(arg_num++);
				if(integer < 0)
				{
					put_char('-', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					integer *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer)for(num_of_digits = 0; 0 < integer_destroyable; integer_destroyable /= 10)num_of_digits++;
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
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable /= 10;
					put_char('0' + integer_destroyable % 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
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
				unsigned_integer = get_variadic_arg(arg_num++);
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 10;
					put_char('0' + unsigned_integer_destroyable % 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			case 'x':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
					put_char('x', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
				}
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 0x10;
					put_char(unsigned_integer_destroyable % 0x10 < 10 ? '0' + unsigned_integer_destroyable % 0x10 : 'a' + unsigned_integer_destroyable % 0x10 - 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			case 'X':
				unsigned_integer = get_variadic_arg(arg_num++);
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char('0', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
					put_char('X', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					if(0 < length)length--;
				}
				unsigned_integer_destroyable = unsigned_integer;
				if(unsigned_integer)for(num_of_digits = 0; 0 < unsigned_integer_destroyable; unsigned_integer_destroyable /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char(flags & SPRINTF_ZERO_FLAG ? '0' : ' ', x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					unsigned_integer_destroyable = unsigned_integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)unsigned_integer_destroyable /= 0x10;
					put_char(unsigned_integer_destroyable % 0x10 < 10 ? '0' + unsigned_integer_destroyable % 0x10 : 'A' + unsigned_integer_destroyable % 0x10 - 10, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background);
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

