#include "font.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "sheet.h"
#include "stdio.h"
#include "task.h"

Sheet *mouse_cursor_sheet = NULL;
Sheet *background_sheet = NULL;

Color ask_color_screen(Sheet *sheet, unsigned short x, unsigned short y)
{
	if(get_video_information()->width <= x)ERROR_MESSAGE();
	if(get_video_information()->height <= y)ERROR_MESSAGE();
	if(sheet->x <= x && x < sheet->x + sheet->width && sheet->y <= y && y < sheet->y + sheet->height)return sheet->image[sheet->width * (y - sheet->y) + (x - sheet->x)];
	else if(sheet->lower_sheet)return ask_color_screen(sheet->lower_sheet, x, y);
	else
	{
		Color default_color;
		ERROR_MESSAGE();
		default_color.red = 0x00;
		default_color.green = 0x00;
		default_color.blue = 0x00;
		default_color.alpha = 0x00;
		return default_color;
	}
}

Sheet *create_sheet(short x, short y, unsigned short width, unsigned short height)
{
	Sheet *new_sheet = malloc(sizeof(*new_sheet));
	new_sheet->x = x;
	new_sheet->y = y;
	new_sheet->width = width;
	new_sheet->height = height;
	new_sheet->image = malloc(width * height * sizeof(*new_sheet->image));
	cli_task();
	if(background_sheet && mouse_cursor_sheet)
	{
		new_sheet->lower_sheet = mouse_cursor_sheet->lower_sheet;
		new_sheet->upper_sheet = mouse_cursor_sheet;
		mouse_cursor_sheet->lower_sheet->upper_sheet = new_sheet;
		mouse_cursor_sheet->lower_sheet = new_sheet;
	}
	else ERROR_MESSAGE();
	sti_task();
	return new_sheet;
}

void delete_sheet(Sheet *sheet)
{
	cli_task();
	if(background_sheet == sheet)background_sheet = sheet->upper_sheet;
	if(mouse_cursor_sheet == sheet)mouse_cursor_sheet = sheet->lower_sheet;
	if(sheet->lower_sheet)sheet->lower_sheet->upper_sheet = sheet->upper_sheet;
	if(sheet->upper_sheet)sheet->upper_sheet->lower_sheet = sheet->lower_sheet;
	sti_task();
	free(sheet->image);
	free(sheet);
}

void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color)
{
	for(short y_i = y; y_i < y + (short)height; y_i++)
	{
		if(y_i < 0)
		{
			y_i = -1;
			continue;
		}
		if(sheet->height <= y_i)break;
		for(short x_i = x; x_i < x + (short)width; x_i++)
		{
			if(x_i < 0)
			{
				x_i = -1;
				continue;
			}
			if(sheet->width <= x_i)break;
			put_dot_sheet(sheet, (unsigned short)x_i, (unsigned short)y_i, color);
		}
	}
}

Color get_color_screen(unsigned short x, unsigned short y)
{
	if(get_video_information()->width <= x)ERROR_MESSAGE();
	if(get_video_information()->height <= y)ERROR_MESSAGE();
	return ask_color_screen(mouse_cursor_sheet, x, y);
}

void init_sheets(Sheet **_background_sheet, Sheet **_mouse_cursor_sheet)
{
	Color red;
	red.red = 0xff;
	red.green = 0x00;
	red.blue = 0x00;
	red.alpha = 0xff;
	cli_task();
	background_sheet = malloc(sizeof(*background_sheet));
	background_sheet->x = 0;
	background_sheet->y = 0;
	background_sheet->width = get_video_information()->width;
	background_sheet->height = get_video_information()->height;
	background_sheet->image = malloc(background_sheet->width * background_sheet->height * sizeof(*background_sheet->image));
	mouse_cursor_sheet = malloc(sizeof(*mouse_cursor_sheet));
	mouse_cursor_sheet->x = get_video_information()->width / 2;
	mouse_cursor_sheet->y = get_video_information()->height / 2;
	mouse_cursor_sheet->width = 0x10;
	mouse_cursor_sheet->height = 0x10;
	mouse_cursor_sheet->image = malloc(mouse_cursor_sheet->width * mouse_cursor_sheet->height * sizeof(*mouse_cursor_sheet->image));
	background_sheet->lower_sheet = NULL;
	background_sheet->upper_sheet = mouse_cursor_sheet;
	mouse_cursor_sheet->lower_sheet = background_sheet;
	mouse_cursor_sheet->upper_sheet = NULL;
	*_background_sheet = background_sheet;
	*_mouse_cursor_sheet = mouse_cursor_sheet;
	fill_box_sheet(mouse_cursor_sheet, 0, 0, mouse_cursor_sheet->width, mouse_cursor_sheet->height, red);
	sti_task();
}

void move_sheet(Sheet *sheet, short x, short y)
{
	short previous_x = sheet->x;
	short previous_y = sheet->y;
	sheet->x = x;
	sheet->y = y;
	refresh_rectangle(previous_x, previous_y, sheet->width, sheet->height);
	refresh_rectangle(sheet->x, sheet->y, sheet->width, sheet->height);
}

void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...)
{
	int arg_num = 6;
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
				put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '%');
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
					fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
					char_pos_x = char_pos_x_destroyable;
					break;
				case '\n':
					char_pos_x = 0;
					char_pos_y++;
					break;
				case ' ':
					fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
					char_pos_x++;
					break;
				default:
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, character);
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
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '-');
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
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.long_long_int /= 10;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0' + integer_destroyable.long_long_int % 10);
					char_pos_x++;
					num_of_digits--;
				}
				break;
			case 'p':
				length = 10;
				integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
				integer.unsigned_ints[1] = 0;
				put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0');
				char_pos_x++;
				if(0 < length)length--;
				put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, 'x');
				char_pos_x++;
				if(0 < length)length--;
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0');
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
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
						fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
						char_pos_x = char_pos_x_destroyable;
						break;
					case '\n':
						char_pos_x = 0;
						char_pos_y++;
						break;
					case ' ':
						fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
						char_pos_x++;
						break;
					default:
						put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, *input_string);
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
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 10;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0' + integer_destroyable.unsigned_long_long_int % 10);
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
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0');
					char_pos_x++;
					if(0 < length)length--;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, 'x');
					char_pos_x++;
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
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
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, '0');
					char_pos_x++;
					if(0 < length)length--;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, 'X');
					char_pos_x++;
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					char_pos_x++;
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'A' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
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
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, *format);
			char_pos_x++;
			break;
		}
		format++;
	}
}

void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string)
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
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, *string);
			char_pos_x++;
			break;
		}
		string++;
	}
}

void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character)
{
	switch(character)
	{
	case '\t':
	case '\n':
	case ' ':
		fill_box_sheet(sheet, x, y, CHAR_WIDTH, CHAR_HEIGHT, background);
		break;
	default:
		for(short y_i = 0; y_i < CHAR_HEIGHT; y_i++)
		{
			if(y + y_i < 0)
			{
				y_i = -y - 1;
				continue;
			}
			if(sheet->height <= y + y_i)break;
			for(short x_i = 0; x_i < CHAR_WIDTH; x_i++)
			{
				if(x + x_i < 0)
				{
					x_i = -x - 1;
					continue;
				}
				if(sheet->width <= x + x_i)break;
				if(get_font_pixel(character, x_i, y_i))put_dot_sheet(sheet, (unsigned short)(x + x_i), (unsigned short)(y + y_i), foreground);
				else put_dot_sheet(sheet, (unsigned short)(x + x_i), (unsigned short)(y + y_i), background);
			}
		}
		break;
	}
}

void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color)
{
	if(sheet->width <= x)ERROR_MESSAGE();
	if(sheet->height <= y)ERROR_MESSAGE();
	sheet->image[sheet->width * y + x] = color;
	if(0 <= sheet->x + x && sheet->x + x < get_video_information()->width && 0 <= sheet->y + y && sheet->y + y < get_video_information()->height)
	{
		if(sheet == mouse_cursor_sheet)put_dot_screen(sheet->x + x, sheet->y + y, color);
		else transmit_color_to_upper_sheet(sheet->upper_sheet, x + sheet->x, y + sheet->y, color);
	}
}

void refresh_dot(unsigned short x, unsigned short y)
{
	if(get_video_information()->width <= x)ERROR_MESSAGE();
	if(get_video_information()->height <= y)ERROR_MESSAGE();
	put_dot_screen(x, y, get_color_screen(x, y));
}

void refresh_rectangle(short x, short y, unsigned short width, unsigned short height)
{
	unsigned short x_start = (0 <= x) ? x : 0;
	unsigned short x_end = (x + width < get_video_information()->width) ? x + width : get_video_information()->width;
	unsigned short y_start = (0 <= y) ? y : 0;
	unsigned short y_end = (y + height < get_video_information()->height) ? y + height : get_video_information()->height;
	for(unsigned short y_i = y_start; y_i < y_end; y_i++)for(unsigned short x_i = x_start; x_i < x_end; x_i++)refresh_dot(x_i, y_i);
}

void transmit_color_to_upper_sheet(Sheet *upper_sheet, unsigned short x, unsigned short y, Color color)
{
	if(upper_sheet->x <= x && x < upper_sheet->x + upper_sheet->width && upper_sheet->y <= y && y < upper_sheet->y + upper_sheet->height)
	{
		// Point (x, y) on the screen doesn't change its color because the point is covered with upper_sheet
	}
	else if(upper_sheet == mouse_cursor_sheet)put_dot_screen(x, y, color);
	else transmit_color_to_upper_sheet(upper_sheet->upper_sheet, x, y, color);
}

