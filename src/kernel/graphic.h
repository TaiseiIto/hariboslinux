// fill box
// x and y can be negative
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void fill_box(short x, short y, unsigned short width, unsigned short height, unsigned char red, unsigned char green, unsigned char blue);

// init screen
void init_screen(unsigned short screen_width, unsigned short screen_height);

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void put_dot(unsigned short x, unsigned short y, unsigned char red, unsigned char green, unsigned char blue);

