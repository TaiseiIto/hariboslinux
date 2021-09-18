#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define PORT_KEYBOARD_COMMAND	0x0064
#define PORT_KEYBOARD_DATA	0x0060
#define PORT_KEYBOARD_STATUS	0x0064

#define KEYBOARD_COMMAND_GET_COMPAQ_STATUS		0x20
#define KEYBOARD_COMMAND_SET_MODE			0x60
#define KEYBOARD_COMMAND_ENABLE_AUXILIARY_DEVICE	0xa8
#define KEYBOARD_COMMAND_SEND_TO_MOUSE			0xd4

#define KEYBOARD_MODE_KEYBOARD_INTERRUPT	0x01
#define KEYBOARD_MODE_MOUSE_INTERRUPT		0x02
#define KEYBOARD_MODE_SYSTEM_FLAG		0x04
#define KEYBOARD_MODE_DISABLE_LOCK		0x08
#define KEYBOARD_MODE_DISABLE_KEYBOARD		0x10
#define KEYBOARD_MODE_DISABLE_MOUSE		0x20
#define KEYBOARD_MODE_SCANCODE01		0x40
#define KEYBOARD_MODE_RESERVED			0x80

#define KEYBOARD_STATUS_RECEIVED	0x01
#define KEYBOARD_STATUS_UNSENDABLE	0x02
#define KEYBOARD_STATUS_SYSTEM_FLAG	0x04
#define KEYBOARD_STATUS_SENDING_COMMAND	0x08
#define KEYBOARD_STATUS_ENABLE		0x10
#define KEYBOARD_STATUS_SEND_ERROR	0x20
#define KEYBOARD_STATUS_RECEIVE_ERROR	0x40
#define KEYBOARD_STATUS_PARITY		0x80

typedef struct _KeyboardInterruptEvent
{
	unsigned char signal;
} KeyboardInterruptEvent;

void init_keyboard(void);
void keyboard_interrupt_handler(void);
unsigned char receive_from_keyboard(void);
void send_to_keyboard(unsigned short command, unsigned char data);
void wait_to_send_to_keyboard(void);
void wait_to_receive_from_keyboard(void);

#endif

