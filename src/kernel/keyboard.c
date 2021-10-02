#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void decode_keyboard_interrupt(unsigned char signal)
{
	Event event;
	static unsigned short keyboard_flags = 0;
	event.type = EVENT_TYPE_KEYBOARD_EVENT;
	// Set flags
	switch(signal)
	{
	case KEY_ALT:
		keyboard_flags |= KEYBOARD_FLAG_ALT_KEY_PUSHED;
		break;
	case KEY_ALT | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_ALT_KEY_PUSHED;
		break;
	case KEY_CAPS_LOCK:
		keyboard_flags ^= KEYBOARD_FLAG_CAPS_LOCK_ON;
		break;
	case KEY_CONTROL:
		keyboard_flags |= KEYBOARD_FLAG_CONTROL_KEY_PUSHED;
		break;
	case KEY_CONTROL | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_CONTROL_KEY_PUSHED;
		break;
	case KEY_LEFT_SHIFT:
		keyboard_flags |= KEYBOARD_FLAG_LEFT_SHIFT_KEY_PUSHED;
		break;
	case KEY_LEFT_SHIFT | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_LEFT_SHIFT_KEY_PUSHED;
		break;
	case KEY_LEFT_SUPER:
		keyboard_flags |= KEYBOARD_FLAG_LEFT_SUPER_KEY_PUSHED;
		break;
	case KEY_LEFT_SUPER | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_LEFT_SUPER_KEY_PUSHED;
		break;
	case KEY_RIGHT_SHIFT:
		keyboard_flags |= KEYBOARD_FLAG_RIGHT_SHIFT_KEY_PUSHED;
		break;
	case KEY_RIGHT_SHIFT | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_RIGHT_SHIFT_KEY_PUSHED;
		break;
	case KEY_RIGHT_SUPER:
		keyboard_flags |= KEYBOARD_FLAG_RIGHT_SUPER_KEY_PUSHED;
		break;
	case KEY_RIGHT_SUPER | KEY_RELEASED:
		keyboard_flags &= ~KEYBOARD_FLAG_RIGHT_SUPER_KEY_PUSHED;
		break;
	}
	if(keyboard_flags & (KEYBOARD_FLAG_LEFT_SHIFT_KEY_PUSHED | KEYBOARD_FLAG_RIGHT_SHIFT_KEY_PUSHED))keyboard_flags |= KEYBOARD_FLAG_SHIFT_KEY_PUSHED;
	else keyboard_flags &= ~KEYBOARD_FLAG_SHIFT_KEY_PUSHED;
	if(keyboard_flags & (KEYBOARD_FLAG_LEFT_SUPER_KEY_PUSHED | KEYBOARD_FLAG_RIGHT_SUPER_KEY_PUSHED))keyboard_flags |= KEYBOARD_FLAG_SUPER_KEY_PUSHED;
	else keyboard_flags &= ~KEYBOARD_FLAG_SUPER_KEY_PUSHED;
	// Determine upper or lower case
	if(keyboard_flags & KEYBOARD_FLAG_CAPS_LOCK_ON)
	{
		if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)keyboard_flags &= ~KEYBOARD_FLAG_UPPER_CASE;
		else keyboard_flags |= KEYBOARD_FLAG_UPPER_CASE;
	}
	else
	{
		if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)keyboard_flags |= KEYBOARD_FLAG_UPPER_CASE;
		else keyboard_flags &= ~KEYBOARD_FLAG_UPPER_CASE;
	}
	event.event_union.keyboard_event.flags = keyboard_flags;
	event.event_union.keyboard_event.keycode = signal;
	// Set chatacter
	switch(signal & ~KEY_RELEASED)
	{
	case KEY_1:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '!' : '1';
		break;
	case KEY_2:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '@' : '2';
		break;
	case KEY_3:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '#' : '3';
		break;
	case KEY_4:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '$' : '4';
		break;
	case KEY_5:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '%' : '5';
		break;
	case KEY_6:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '^' : '6';
		break;
	case KEY_7:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '&' : '7';
		break;
	case KEY_8:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '*' : '8';
		break;
	case KEY_9:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '(' : '9';
		break;
	case KEY_0:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? ')' : '0';
		break;
	case KEY_MINUS:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '_' : '-';
		break;
	case KEY_EQUAL:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '+' : '=';
		break;
	case KEY_BACKSPACE:
		event.event_union.keyboard_event.character = '\b';
		break;
	case KEY_TAB:
		event.event_union.keyboard_event.character = '\t';
		break;
	case KEY_Q:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'Q' : 'q';
		break;
	case KEY_W:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'W' : 'w';
		break;
	case KEY_E:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'E' : 'e';
		break;
	case KEY_R:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'R' : 'r';
		break;
	case KEY_T:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'T' : 't';
		break;
	case KEY_Y:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'Y' : 'y';
		break;
	case KEY_U:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'U' : 'u';
		break;
	case KEY_I:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'I' : 'i';
		break;
	case KEY_O:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'O' : 'o';
		break;
	case KEY_P:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'P' : 'p';
		break;
	case KEY_LEFT_SQUARE_BRACKET:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '{' : '[';
		break;
	case KEY_RIGHT_SQUARE_BRACKET:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '}' : ']';
		break;
	case KEY_ENTER:
		event.event_union.keyboard_event.character = '\n';
		break;
	case KEY_A:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'A' : 'a';
		break;
	case KEY_S:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'S' : 's';
		break;
	case KEY_D:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'D' : 'd';
		break;
	case KEY_F:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'F' : 'f';
		break;
	case KEY_G:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'G' : 'g';
		break;
	case KEY_H:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'H' : 'h';
		break;
	case KEY_J:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'J' : 'j';
		break;
	case KEY_K:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'K' : 'k';
		break;
	case KEY_L:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'L' : 'l';
		break;
	case KEY_SEMICOLON:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? ':' : ';';
		break;
	case KEY_QUOTE:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '\"' : '\'';
		break;
	case KEY_BACKQUOTE:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '~' : '`';
		break;
	case KEY_BACKSLASH:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '|' : '\\';
		break;
	case KEY_Z:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'Z' : 'z';
		break;
	case KEY_X:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'X' : 'x';
		break;
	case KEY_C:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'C' : 'c';
		break;
	case KEY_V:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'V' : 'v';
		break;
	case KEY_B:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'B' : 'b';
		break;
	case KEY_N:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'N' : 'n';
		break;
	case KEY_M:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? 'M' : 'm';
		break;
	case KEY_COMMA:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '<' : ',';
		break;
	case KEY_PERIOD:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '>' : '.';
		break;
	case KEY_SLASH:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? '?' : '/';
		break;
	case KEY_SPACE:
		event.event_union.keyboard_event.character = ' ';
		break;
	default:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_UPPER_CASE) ? ' ' : '\0';
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

