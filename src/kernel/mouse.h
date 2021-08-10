#ifndef _MOUSE_H_
#define _MOUSE_H_

void init_mouse(void);
void mouse_interrupt_handler(void);
void send_to_mouse(unsigned char data);

#endif

