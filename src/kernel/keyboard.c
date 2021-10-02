#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void decode_keyboard_interrupt(unsigned char signal)
{
	Event event;
	event.type = EVENT_TYPE_KEYBOARD_EVENT;
	event.event_union.keyboard_event.flags = 0;
	event.event_union.keyboard_event.keycode = signal;
	event.event_union.keyboard_event.keycode &= ~KEY_RELEASED;
	switch(event.event_union.keyboard_event.keycode)
	{
	case KEY_1:
		event.event_union.keyboard_event.character = '1';
		break;
	case KEY_2:
		event.event_union.keyboard_event.character = '2';
		break;
	case KEY_3:
		event.event_union.keyboard_event.character = '3';
		break;
	case KEY_4:
		event.event_union.keyboard_event.character = '4';
		break;
	case KEY_5:
		event.event_union.keyboard_event.character = '5';
		break;
	case KEY_6:
		event.event_union.keyboard_event.character = '6';
		break;
	case KEY_7:
		event.event_union.keyboard_event.character = '7';
		break;
	case KEY_8:
		event.event_union.keyboard_event.character = '8';
		break;
	case KEY_9:
		event.event_union.keyboard_event.character = '9';
		break;
	case KEY_0:
		event.event_union.keyboard_event.character = '0';
		break;
	case KEY_MINUS:
		event.event_union.keyboard_event.character = '-';
		break;
	case KEY_EQUAL:
		event.event_union.keyboard_event.character = '=';
		break;
	case KEY_BACKSPACE:
		event.event_union.keyboard_event.character = '\b';
		break;
	case KEY_TAB:
		event.event_union.keyboard_event.character = '\t';
		break;
	case KEY_Q:
		event.event_union.keyboard_event.character = 'q';
		break;
	case KEY_W:
		event.event_union.keyboard_event.character = 'w';
		break;
	case KEY_E:
		event.event_union.keyboard_event.character = 'e';
		break;
	case KEY_R:
		event.event_union.keyboard_event.character = 'r';
		break;
	case KEY_T:
		event.event_union.keyboard_event.character = 't';
		break;
	case KEY_Y:
		event.event_union.keyboard_event.character = 'y';
		break;
	case KEY_U:
		event.event_union.keyboard_event.character = 'u';
		break;
	case KEY_I:
		event.event_union.keyboard_event.character = 'i';
		break;
	case KEY_O:
		event.event_union.keyboard_event.character = 'o';
		break;
	case KEY_P:
		event.event_union.keyboard_event.character = 'p';
		break;
	case KEY_LEFT_SQUARE_BRACKET:
		event.event_union.keyboard_event.character = '[';
		break;
	case KEY_RIGHT_SQUARE_BRACKET:
		event.event_union.keyboard_event.character = ']';
		break;
	case KEY_ENTER:
		event.event_union.keyboard_event.character = '\n';
		break;
	case KEY_A:
		event.event_union.keyboard_event.character = 'a';
		break;
	case KEY_S:
		event.event_union.keyboard_event.character = 's';
		break;
	case KEY_D:
		event.event_union.keyboard_event.character = 'd';
		break;
	case KEY_F:
		event.event_union.keyboard_event.character = 'f';
		break;
	case KEY_G:
		event.event_union.keyboard_event.character = 'g';
		break;
	case KEY_H:
		event.event_union.keyboard_event.character = 'h';
		break;
	case KEY_J:
		event.event_union.keyboard_event.character = 'j';
		break;
	case KEY_K:
		event.event_union.keyboard_event.character = 'k';
		break;
	case KEY_L:
		event.event_union.keyboard_event.character = 'l';
		break;
	case KEY_SEMICOLON:
		event.event_union.keyboard_event.character = ';';
		break;
	case KEY_QUOTE:
		event.event_union.keyboard_event.character = '\'';
		break;
	case KEY_BACKQUOTE:
		event.event_union.keyboard_event.character = '`';
		break;
	case KEY_BACKSLASH:
		event.event_union.keyboard_event.character = '\\';
		break;
	case KEY_Z:
		event.event_union.keyboard_event.character = 'z';
		break;
	case KEY_X:
		event.event_union.keyboard_event.character = 'x';
		break;
	case KEY_C:
		event.event_union.keyboard_event.character = 'c';
		break;
	case KEY_V:
		event.event_union.keyboard_event.character = 'v';
		break;
	case KEY_B:
		event.event_union.keyboard_event.character = 'b';
		break;
	case KEY_N:
		event.event_union.keyboard_event.character = 'n';
		break;
	case KEY_M:
		event.event_union.keyboard_event.character = 'm';
		break;
	case KEY_COMMA:
		event.event_union.keyboard_event.character = ',';
		break;
	case KEY_PERIOD:
		event.event_union.keyboard_event.character = '.';
		break;
	case KEY_SLASH:
		event.event_union.keyboard_event.character = '/';
		break;
	case KEY_SPACE:
		event.event_union.keyboard_event.character = ' ';
		break;
	default:
		event.event_union.keyboard_event.character = '\0';
		break;
	}
	enqueue_event(&event);
}

void init_keyboard(void)
{
	send_to_keyboard(KEYBOARD_COMMAND_SET_MODE, KEYBOARD_MODE_KEYBOARD_INTERRUPT | KEYBOARD_MODE_MOUSE_INTERRUPT | KEYBOARD_MODE_SYSTEM_FLAG | KEYBOARD_MODE_SCANCODE01);
}

void keyboard_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_KEYBOARD);
	event.type = EVENT_TYPE_KEYBOARD_INTERRUPT;
	event.event_union.keyboard_interrupt.signal = inb(PORT_KEYBOARD_DATA);
	enqueue_event(&event);
}

unsigned char receive_from_keyboard(void)
{
	wait_to_receive_from_keyboard();
	return inb(PORT_KEYBOARD_DATA);
}

void send_to_keyboard(unsigned short command, unsigned char data)
{
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_COMMAND, command);
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_DATA, data);
}

void wait_to_send_to_keyboard(void)
{
	while(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_UNSENDABLE);
}

void wait_to_receive_from_keyboard(void)
{
	while(!(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_RECEIVED));
}

