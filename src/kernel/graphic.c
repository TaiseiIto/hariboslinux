#include "chain_string.h"
#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "limits.h"
#include "memory.h"
#include "serial.h"
#include "stdio.h"

void *vram;
VideoInformation const * const video_information = (VideoInformation const * const)MEMORY_MAP_VIDEO_INFORMATION;

VideoInformation const *get_video_information(void)
{
	return video_information;
}

// fill box
// x and y can be negative
void fill_box_screen(short x, short y, unsigned short width, unsigned short height, Color color)
{
	for(short y_i = y; y_i < y + (short)height; y_i++)
	{
		if(y_i < 0)
		{
			y_i = -1;
			continue;
		}
		if(video_information->height <= y_i)break;
		for(short x_i = x; x_i < x + (short)width; x_i++)
		{
			if(x_i < 0)
			{
				x_i = -1;
				continue;
			}
			if(video_information->width <= x_i)break;
			put_dot_screen((unsigned short)x_i, (unsigned short)y_i, color);
		}
	}
}

// init screen
void init_screen(void)
{
	printf_serial("video_information->attributes = %#06x\n", video_information->attributes);
	printf_serial("video_information->pitch = %#06x\n", video_information->pitch);
	printf_serial("video_information->width = %#06x\n", video_information->width);
	printf_serial("video_information->height = %#06x\n", video_information->height);
	printf_serial("video_information->bits_per_pixel = %#04x\n", video_information->bits_per_pixel);
	printf_serial("video_information->memory_model = %#04x\n", video_information->memory_model);
	printf_serial("video_information->red_mask = %#04x\n", video_information->red_mask);
	printf_serial("video_information->red_positioin = %#04x\n", video_information->red_position);
	printf_serial("video_information->green_mask = %#04x\n", video_information->green_mask);
	printf_serial("video_information->green_positioin = %#04x\n", video_information->green_position);
	printf_serial("video_information->blue_mask = %#04x\n", video_information->blue_mask);
	printf_serial("video_information->blue_positioin = %#04x\n", video_information->blue_position);
	printf_serial("video_information->frame_buffer = %#010x\n\n", video_information->frame_buffer);
	vram = (void *)video_information->frame_buffer;
}

// printf to screen
void printf_screen(unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(4);
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	print_screen(x, y, foreground, background, output_string);
	free(output_string);
	delete_chain_string(output_chain_string);
}

void print_screen(unsigned short x, unsigned short y, Color foreground, Color background, char *string)
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
			fill_box_screen(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box_screen(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char_screen(x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, *string);
			char_pos_x++;
			break;
		}
		string++;
	}
}

// put character at screen(x, y)
void put_char_screen(unsigned short x, unsigned short y, Color foreground, Color background, char character)
{
	switch(character)
	{
	case '\t':
	case '\n':
	case ' ':
		fill_box_screen(x, y, CHAR_WIDTH, CHAR_HEIGHT, background);
		break;
	default:
		for(short y_i = 0; y_i < CHAR_HEIGHT; y_i++)
		{
			if(y + y_i < 0)
			{
				y_i = -y - 1;
				continue;
			}
			if(video_information->height <= y + y_i)break;
			for(short x_i = 0; x_i < CHAR_WIDTH; x_i++)
			{
				if(x + x_i < 0)
				{
					x_i = -x - 1;
					continue;
				}
				if(video_information->width <= x + x_i)break;
				if(get_font_pixel(character, x_i, y_i))put_dot_screen((unsigned short)(x + x_i), (unsigned short)(y + y_i), foreground);
				else put_dot_screen((unsigned short)(x + x_i), (unsigned short)(y + y_i), background);
			}
		}
		break;
	}
}

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot_screen(unsigned short x, unsigned short y, Color color)
{
	if(video_information->width <= x)ERROR();
	if(video_information->height <= y)ERROR();
	*(unsigned char *)(vram + video_information->pitch * y + (video_information->bits_per_pixel * x + video_information->red_position) / CHAR_BIT) = color.red;
	*(unsigned char *)(vram + video_information->pitch * y + (video_information->bits_per_pixel * x + video_information->green_position) / CHAR_BIT) = color.green;
	*(unsigned char *)(vram + video_information->pitch * y + (video_information->bits_per_pixel * x + video_information->blue_position) / CHAR_BIT) = color.blue;
}

