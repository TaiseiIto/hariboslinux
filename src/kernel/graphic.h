#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

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

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Color;

// fill box
// x and y can be negative
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color);

// init screen
void init_screen(void);

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background);

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot(unsigned short x, unsigned short y, Color color);

// put string at screen(x, y)
void put_string(char const *string, unsigned short x, unsigned short y, Color foreground, Color background);

// printf to screen
void printf_screen(unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);

#endif

