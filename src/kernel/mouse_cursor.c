#include "mouse_cursor.h"

//        {red,  green, blue, alpha}
const Color b = {0x00, 0x00,  0x00, 0xff}; // black
const Color t = {0x00, 0x00,  0x00, 0x00}; // transparent
const Color w = {0xff, 0xff,  0xff, 0xff}; // white

const Color mouse_cursor_image[MOUSE_CURSOR_HEIGHT * MOUSE_CURSOR_WIDTH] =
{
	b,t,t,t,t,t,t,t,
	b,b,t,t,t,t,t,t,
	b,w,b,t,t,t,t,t,
	b,w,w,b,t,t,t,t,
	b,w,w,w,b,t,t,t,
	b,w,w,w,w,b,t,t,
	b,w,w,w,w,w,b,t,
	b,w,w,w,w,w,w,b,
	b,w,w,w,w,b,b,b,
	b,w,w,b,w,b,t,t,
	b,w,b,b,w,w,b,t,
	b,b,t,t,b,w,b,t,
	t,t,t,t,b,w,w,b,
	t,t,t,t,t,b,w,b,
	t,t,t,t,t,b,w,b,
	t,t,t,t,t,t,b,b
};

