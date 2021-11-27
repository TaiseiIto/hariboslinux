#include "font.h"
#include "io.h"
#include "memory.h"
#include "mouse_cursor.h"
#include "serial.h"
#include "sheet.h"
#include "stdio.h"
#include "task.h"

//			        {red ,green, blue,alpha}
const Color color_black	      = {0x00, 0x00, 0x00, 0xff};
const Color color_transparent = {0x00, 0x00, 0x00, 0x00};

Sheet *mouse_cursor_sheet = NULL;
Sheet *background_sheet = NULL;

Color alpha_blend(Color foreground, Color background);
void refresh_input(Sheet *sheet); // refresh sheet->input.
void refresh_input_dot(Sheet *sheet, unsigned short x, unsigned short y); // refresh sheet->input[x + y * sheet->width].
void transmit_family_output_dot(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->family_output[x + y * sheet->width].
void transmit_self_output(Sheet *sheet); // transmit image sheet->self_output.
void transmit_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->self_output[x + y * sheet->width].

Color alpha_blend(Color foreground, Color background)
{
	Color blended;
	blended.red = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.red + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.red) >> 8);
	blended.green = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.green + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.green) >> 8);
	blended.blue = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.blue + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.blue) >> 8);
	blended.alpha = 0xff;
	return blended;
}

Sheet *create_sheet(Sheet *parent, short x, short y, unsigned short width, unsigned short height)
{
	Sheet *new_sheet = malloc(sizeof(*new_sheet));
	new_sheet->image = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->input = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->self_output = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->family_output = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->x = x;
	new_sheet->y = y;
	new_sheet->width = width;
	new_sheet->height = height;
	new_sheet->uppest_child = NULL;
	new_sheet->lowest_child = NULL;
	cli_task();
	new_sheet->parent = parent;
	if(parent)
	{
		if(parent->uppest_child && parent->lowest_child)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = parent->uppest_child;
			parent->uppest_child->upper = new_sheet;
			parent->uppest_child = new_sheet;
		}
		else if(!parent->uppest_child && !parent->lowest_child)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = NULL;
			parent->uppest_child = new_sheet;
			parent->lowest_child = new_sheet;
		}
		else ERROR_MESSAGE();
	}
	else
	{
		if(background_sheet && mouse_cursor_sheet && background_sheet == mouse_cursor_sheet)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = mouse_cursor_sheet;
			mouse_cursor_sheet->upper = new_sheet;
			mouse_cursor_sheet = new_sheet;
		}
		else if(!background_sheet && !mouse_cursor_sheet)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = NULL;
			background_sheet = new_sheet;
			mouse_cursor_sheet = new_sheet;
		}
		else ERROR_MESSAGE();
	}
	sti_task();
	refresh_input(new_sheet);
	fill_box_sheet(new_sheet, 0, 0, new_sheet->width, new_sheet->height, color_transparent);
	return new_sheet;
}

void delete_sheet(Sheet *sheet)
{
	if(sheet == background_sheet || sheet == mouse_cursor_sheet)ERROR_MESSAGE();
	while(sheet->uppest_child && sheet->lowest_child)delete_sheet(sheet->uppest_child);
	if(sheet->uppest_child || sheet->lowest_child)ERROR_MESSAGE();
	cli_task();
	if(sheet->parent)
	{
		if(sheet == sheet->parent->uppest_child)sheet->parent->uppest_child = sheet->lower;
		if(sheet == sheet->parent->lowest_child)sheet->parent->lowest_child = sheet->upper;
	}
	if(sheet->upper)sheet->upper->lower = sheet->lower;
	if(sheet->lower)sheet->lower->upper = sheet->upper;
	sti_task();
	free(sheet->image);
	free(sheet->input);
	free(sheet->self_output);
	free(sheet->family_output);
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

void init_sheets(Sheet **_background_sheet, Sheet **_mouse_cursor_sheet)
{
	*_background_sheet = create_sheet(NULL, 0, 0, get_video_information()->width, get_video_information()->height);
	fill_box_sheet(background_sheet, 0, 0, background_sheet->width, background_sheet->height, color_black);
	*_mouse_cursor_sheet = create_sheet(NULL, get_video_information()->width / 2, get_video_information()->height / 2, 0x08, 0x10);
	for(unsigned short mouse_cursor_image_y = 0; mouse_cursor_image_y < mouse_cursor_sheet->height; mouse_cursor_image_y++)for(unsigned short mouse_cursor_image_x = 0; mouse_cursor_image_x < mouse_cursor_sheet->width; mouse_cursor_image_x++)put_dot_sheet(mouse_cursor_sheet, mouse_cursor_image_x, mouse_cursor_image_y, mouse_cursor_image[mouse_cursor_image_x + mouse_cursor_image_y * mouse_cursor_sheet->width]);
}

void move_sheet(Sheet *sheet, short x, short y)
{
	sheet->x = x;
	sheet->y = y;
	transmit_self_output(sheet);
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
	if(x < sheet->width && y < sheet->height)
	{
		sheet->image[x + y * sheet->width] = color;
		if(color.alpha)
		{
			sheet->self_output[x + y * sheet->width] = alpha_blend(color, sheet->input[x + y * sheet->width]);
			transmit_self_output_dot(sheet, x, y);
		}
	}
	else ERROR_MESSAGE();
}

void refresh_input(Sheet *sheet) // refresh sheet->input.
{
	for(unsigned short x = 0; x < sheet->width; x++)for(unsigned short y = 0; y < sheet->height; y++)refresh_input_dot(sheet, x, y);
}

void refresh_input_dot(Sheet *sheet, unsigned short x, unsigned short y) // refresh sheet->input[x + y * sheet->width].
{
	if(x < sheet->width && y < sheet->height)
	{
		if(sheet->parent || sheet->lower)
		{
			unsigned short parent_width = (sheet->parent ? sheet->parent->width : get_video_information()->width);
			unsigned short parent_height = (sheet->parent ? sheet->parent->height : get_video_information()->height);
			short x_seen_from_parent = x + sheet->x;
			short y_seen_from_parent = y + sheet->y;
			if(0 <= x_seen_from_parent && x_seen_from_parent < parent_width && 0 <= y_seen_from_parent && y_seen_from_parent < parent_height)
			{
				for(Sheet *lower = sheet->lower; lower; lower = lower->lower)
				{
					short x_seen_from_lower = x_seen_from_parent - lower->x;
					short y_seen_from_lower = y_seen_from_parent - lower->y;
					if(0 <= x_seen_from_lower && x_seen_from_lower < lower->width && 0 <= y_seen_from_lower && y_seen_from_lower < lower->height)
					{
						sheet->input[x + y * sheet->width] = lower->family_output[x_seen_from_lower + y_seen_from_lower * lower->width];
						return;
					}
				}
				sheet->input[x + y * sheet->width] = (sheet->parent ? sheet->parent->self_output[x_seen_from_parent + y_seen_from_parent * sheet->parent->width] : color_black);
			}
		}
		else sheet->input[x + y * sheet->width] = color_black;
	}
	else ERROR_MESSAGE();
}

void transmit_family_output_dot(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->family_output[x + y * sheet->width].
{
	short x_seen_from_screen;
	short y_seen_from_screen;
	if(sheet->upper)
	{
		for(Sheet *upper = sheet->upper; upper; upper = upper->upper)
		{
			short x_seen_from_upper = (short)x + sheet->x - upper->x;
			short y_seen_from_upper = (short)y + sheet->y - upper->y;
			if(0 <= x_seen_from_upper && x_seen_from_upper < upper->width && 0 <= y_seen_from_upper && y_seen_from_upper < upper->height)
			{
				upper->input[x_seen_from_upper + y_seen_from_upper * upper->width] = sheet->self_output[x + y * sheet->width];
				upper->self_output[x_seen_from_upper + y_seen_from_upper * upper->width] = alpha_blend(upper->image[x_seen_from_upper + y_seen_from_upper * upper->width], upper->input[x_seen_from_upper + y_seen_from_upper * upper->width]);
				transmit_self_output_dot(upper, x_seen_from_upper, y_seen_from_upper);
				return;
			}
		}
	}
	if(sheet->parent)
	{
		short x_seen_from_parent = x + sheet->x;
		short y_seen_from_parent = y + sheet->y;
		if(0 <= x_seen_from_parent && x_seen_from_parent < sheet->parent->width && 0 <= y_seen_from_parent && y_seen_from_parent < sheet->parent->height)
		{
			sheet->parent->family_output[x_seen_from_parent + y_seen_from_parent * sheet->parent->width] = sheet->family_output[x + y * sheet->width];
			transmit_family_output_dot(sheet->parent, x_seen_from_parent, y_seen_from_parent);
			return;
		}
	}
	x_seen_from_screen = x + sheet->x;
	y_seen_from_screen = y + sheet->y;
	if(0 <= x_seen_from_screen && x_seen_from_screen < get_video_information()->width && 0 <= y_seen_from_screen && y_seen_from_screen < get_video_information()->height)put_dot_screen(x_seen_from_screen, y_seen_from_screen, sheet->family_output[x + y * sheet->width]);
}

void transmit_self_output(Sheet *sheet) // transmit image sheet->self_output.
{
	for(unsigned short x = 0; x < sheet->width; x++)for(unsigned short y = 0; y < sheet->height; y++)transmit_self_output_dot(sheet, x, y);
}

void transmit_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->self_output[x + y * sheet->width].
{
	for(Sheet *child = sheet->lowest_child; child; child = child->upper)
	{
		short x_seen_from_child = x - child->x;
		short y_seen_from_child = y - child->y;
		if(0 <= x_seen_from_child && x_seen_from_child < child->width && 0 <= y_seen_from_child && y_seen_from_child < child->height)
		{
			child->input[x_seen_from_child + y_seen_from_child * child->width] = sheet->self_output[x + y * sheet->width];
			child->self_output[x_seen_from_child + y_seen_from_child * child->width] = alpha_blend(child->image[x_seen_from_child + y_seen_from_child * child->width], child->input[x_seen_from_child + y_seen_from_child * child->width]);
			transmit_self_output_dot(child, x_seen_from_child, y_seen_from_child);
			return;
		}
	}
	sheet->family_output[x + y * sheet->width] = sheet->self_output[x + y * sheet->width];
	transmit_family_output_dot(sheet, x, y);
}

