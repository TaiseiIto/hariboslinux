#ifndef _DEV_H_
#define _DEV_H_

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} Color;

void clear_console(void);
unsigned int create_window(char const *title, short x, short y, unsigned short width, unsigned short height);
void fill_box_window(unsigned int window, short x, short y, unsigned short width, unsigned short height, Color color);
unsigned int get_free_memory_space_size(void);
void print_window(unsigned int window, short x, short y, Color foreground, Color background, char const *string);

#endif

