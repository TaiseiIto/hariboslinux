#include "font.h"
#include "graphic.h"
#include "io.h"
#include "segment.h"
#include "serial.h"

#define TAB_LENGTH 4

typedef struct
{
	unsigned short width;
	unsigned short height;
} ScreenInformation;

ScreenInformation screen_information;

// 1 byte per pixel
// 3 primary colors(RGB)
// 6 brightnesses (hardware constraints 0x00~0x3f)
// 216 colors
// red brightnesses:   0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// green brightnesses: 0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// blue brightnesses:  0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
void init_palette(void);

Color get_color(unsigned char red, unsigned char green, unsigned char blue)
{
	return 6 * (6 * blue + green) + red;
}

// fill box
// x and y can be negative
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color)
{
	for(short y_i = y; y_i < y + (short)height; y_i++)
	{
		if(y_i < 0)continue;
		if(screen_information.height <= y_i)break;
		for(short x_i = x; x_i < x + (short)width; x_i++)
		{
			if(x_i < 0)continue;
			if(screen_information.width <= x_i)break;
			put_dot((unsigned short)x_i, (unsigned short)y_i, color);
		}
	}
}

// 1 byte per pixel
// 3 primary colors(RGB)
// 6 brightnesses  (hardware constraints 0x00~0x3f)
// 216 colors
// red brightnesses:   0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// green brightnesses: 0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// blue brightnesses:  0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
void init_palette(void)
{
	unsigned int eflags = get_eflags();
	if(eflags & INTERRUPT_FLAG)cli();
	outb(0x03c8, 0x00);
	for(unsigned short color_number = 0x0000; color_number < 0x0100; color_number++)
	{
		if(color_number < 0x06 * 0x06 * 0x06)
		{
			unsigned char color_number_destroyable;
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			color_number_destroyable = (unsigned char)color_number;
			red = 0x0c * (color_number_destroyable % 6);
			color_number_destroyable /= 6;
			green = 0x0c * (color_number_destroyable % 6);
			color_number_destroyable /= 6;
			blue = 0x0c * (color_number_destroyable % 6);
			outb(0x03c9, red);
			outb(0x03c9, green);
			outb(0x03c9, blue);
		}
		else
		{
			outb(0x03c9, 0x00);
			outb(0x03c9, 0x00);
			outb(0x03c9, 0x00);
		}
	}
	if(eflags & INTERRUPT_FLAG)sti();
}

// init screen
void init_screen(unsigned short screen_width, unsigned short screen_height)
{
	screen_information.width = screen_width;
	screen_information.height = screen_height;
	print_serial_polling("screen_information.width = 0x");
	print_word_hex_serial_polling(screen_information.width);
	new_line_serial_polling();
	print_serial_polling("screen_information.height = 0x");
	print_word_hex_serial_polling(screen_information.height);
	new_line_serial_polling();
	init_palette();
}

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background)
{
	for(short y_i = 0; y_i < CHAR_HEIGHT; y_i++)
	{
		if(y + y_i < 0)continue;
		if(screen_information.height <= y + y_i)break;
		for(short x_i = 0; x_i < CHAR_WIDTH; x_i++)
		{
			if(x + x_i < 0)continue;
			if(screen_information.width <= x + x_i)break;
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
	if(screen_information.width <= x)return;
	if(screen_information.height <= y)return;
	writeb(VRAM_SEGMENT, (void *)(screen_information.width * y + x), color);
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

