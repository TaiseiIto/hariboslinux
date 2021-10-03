#ifndef _MOUSE_H_
#define _MOUSE_H_

typedef struct _MouseEvent
{
	unsigned short flags;
	#define MOUSE_LEFT_BUTTON_PUSHED	0x0001
	#define MOUSE_MIDDLE_BUTTON_PUSHED	0x0002
	#define MOUSE_RIGHT_BUTTON_PUSHED	0x0004
	#define MOUSE_4TH_BUTTON_PUSHED		0x0008
	#define MOUSE_5TH_BUTTON_PUSHED		0x0010
	#define MOUSE_WHEEL_DOWN		0x0020
	#define MOUSE_WHEEL_LEFT		0x0040
	#define MOUSE_WHEEL_RIGHT		0x0080
	#define MOUSE_WHEEL_UP			0x0100
	short x_movement;
	short y_movement;
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

