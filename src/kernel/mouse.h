#ifndef _MOUSE_H_
#define _MOUSE_H_

typedef struct _MouseEvent
{
	unsigned int flags;
	#define MOUSE_LEFT_BUTTON_PUSHED		0x00000001
	#define MOUSE_MIDDLE_BUTTON_PUSHED		0x00000002
	#define MOUSE_RIGHT_BUTTON_PUSHED		0x00000004
	#define MOUSE_4TH_BUTTON_PUSHED			0x00000008
	#define MOUSE_5TH_BUTTON_PUSHED			0x00000010
	#define MOUSE_LEFT_BUTTON_PUSHED_NOW		0x00000020
	#define MOUSE_MIDDLE_BUTTON_PUSHED_NOW		0x00000040
	#define MOUSE_RIGHT_BUTTON_PUSHED_NOW		0x00000080
	#define MOUSE_4TH_BUTTON_PUSHED_NOW		0x00000100
	#define MOUSE_5TH_BUTTON_PUSHED_NOW		0x00000200
	#define MOUSE_LEFT_BUTTON_RELEASED_NOW		0x00000400
	#define MOUSE_MIDDLE_BUTTON_RELEASED_NOW	0x00000800
	#define MOUSE_RIGHT_BUTTON_RELEASED_NOW		0x00001000
	#define MOUSE_4TH_BUTTON_RELEASED_NOW		0x00002000
	#define MOUSE_5TH_BUTTON_RELEASED_NOW		0x00004000
	short x;
	short y;
	short x_movement;
	short y_movement;
	char vertical_wheel_movement;
	char horizontal_wheel_movement;
} MouseEvent;

typedef struct _MouseInterrupt
{
	unsigned char signal;
} MouseInterrupt;

void decode_mouse_interrupt(unsigned char signal);
unsigned char get_mouse_id(void);
void init_mouse(void);
void mouse_interrupt_handler(void);

#endif

