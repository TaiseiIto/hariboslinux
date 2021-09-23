#ifndef _MOUSE_H_
#define _MOUSE_H_

typedef struct _MouseInterruptEvent
{
	unsigned char signal;
} MouseInterruptEvent;

unsigned char get_mouse_id(void);
void init_mouse(void);
void mouse_interrupt_handler(void);

#endif

