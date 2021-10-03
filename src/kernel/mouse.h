#ifndef _MOUSE_H_
#define _MOUSE_H_

typedef struct _MouseEvent
{
	unsigned char flags;
	#define MOUSE_LEFT_BUTTON_PUSHED	0x01
	#define MOUSE_MIDDLE_BUTTON_PUSHED	0x02
	#define MOUSE_RIGHT_BUTTON_PUSHED	0x04
	#define MOUSE_WHEEL_DOWN		0x08
	#define MOUSE_WHEEL_LEFT		0x10
	#define MOUSE_WHEEL_RIGHT		0x20
	#define MOUSE_WHEEL_UP			0x40
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

