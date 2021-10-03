#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "queue.h"
#include "serial.h"

typedef struct _KeyboardTransmission
{
	unsigned short port;
	unsigned char data;
} KeyboardTransmission;

char get_character(unsigned char signal);
unsigned short keyboard_flags = KEYBOARD_FLAG_LAYOUT_ENGLISH;
Queue *keyboard_send_buffer;
KeyboardTransmission last_keyboard_transmission;

char get_character(unsigned char signal)
{
	const char english_no_caps_lock_no_shift_character_map[0x100] = 
	{
		'\0',	// 0x00
		'\0',	// 0x01 KEY_ESC
		'1',	// 0x02 KEY_1
		'2',	// 0x03 KEY_2
		'3',	// 0x04 KEY_3
		'4',	// 0x05 KEY_4
		'5',	// 0x06 KEY_5
		'6',	// 0x07 KEY_6
		'7',	// 0x08 KEY_7
		'8',	// 0x09 KEY_8
		'9',	// 0x0a KEY_9
		'0',	// 0x0b KEY_0
		'-',	// 0x0c KEY_MINUS
		'=',	// 0x0d KEY_EQUAL
		'\b',	// 0x0e KEY_BACKSPACE
		'\t',	// 0x0f KEY_TAB
		'q',	// 0x10 KEY_Q
		'w',	// 0x11 KEY_W
		'e',	// 0x12 KEY_E
		'r',	// 0x13 KEY_R
		't',	// 0x14 KEY_T
		'y',	// 0x15 KEY_Y
		'u',	// 0x16 KEY_U
		'i',	// 0x17 KEY_I
		'o',	// 0x18 KEY_O
		'p',	// 0x19 KEY_P
		'[',	// 0x1a KEY_LEFT_SQUARE_BRACKET
		']',	// 0x1b KEY_RIGHT_SQUARE_BRACKET
		'\n',	// 0x1c KEY_ENTER
		'\0',	// 0x1d KEY_CONTROL
		'a',	// 0x1e KEY_A
		's',	// 0x1f KEY_S
		'd',	// 0x20 KEY_D
		'f',	// 0x21 KEY_F
		'g',	// 0x22 KEY_G
		'h',	// 0x23 KEY_H
		'j',	// 0x24 KEY_J
		'k',	// 0x25 KEY_K
		'l',	// 0x26 KEY_L
		';',	// 0x27 KEY_SEMICOLON
		'\'',	// 0x28 KEY_QUOTE
		'`',	// 0x29 KEY_BACKQUOTE
		'\0',	// 0x2a KEY_LEFT_SHIFT
		'\\',	// 0x2b KEY_BACKSLASH
		'z',	// 0x2c KEY_Z
		'x',	// 0x2d KEY_X
		'c',	// 0x2e KEY_C
		'v',	// 0x2f KEY_V
		'b',	// 0x30 KEY_B
		'n',	// 0x31 KEY_N
		'm',	// 0x32 KEY_M
		',',	// 0x33 KEY_COMMA
		'.',	// 0x34 KEY_PERIOD
		'/',	// 0x35 KEY_SLASH
		'\0',	// 0x36 KEY_RIGHT_SHIFT
		'*',	// 0x37 KEY_PAD_ASTERISK
		'\0',	// 0x38 KEY_ALT
		' ',	// 0x39 KEY_SPACE
		'\0',	// 0x3a KEY_CAPS_LOCK
		'\0',	// 0x3b KEY_F1
		'\0',	// 0x3c KEY_F2
		'\0',	// 0x3d KEY_F3
		'\0',	// 0x3e KEY_F4
		'\0',	// 0x3f KEY_F5
		'\0',	// 0x40 KEY_F6
		'\0',	// 0x41 KEY_F7
		'\0',	// 0x42 KEY_F8
		'\0',	// 0x43 KEY_F9
		'\0',	// 0x44 KEY_F10
		'\0',	// 0x45 KEY_NUMBER_LOCK
		'\0',	// 0x46 KEY_SCROLL_LOCK
		'7',	// 0x47 KEY_PAD_7
		'8',	// 0x48 KEY_PAD_8
		'9',	// 0x49 KEY_PAD_9
		'-',	// 0x4a KEY_PAD_MINUS
		'4',	// 0x4b KEY_PAD_4
		'5',	// 0x4c KEY_PAD_5
		'6',	// 0x4d KEY_PAD_6
		'+',	// 0x4e KEY_PAD_PLIS
		'1',	// 0x4f KEY_PAD_1
		'2',	// 0x50 KEY_PAD_2
		'3',	// 0x51 KEY_PAD_3
		'0',	// 0x52 KEY_PAD_0
		'.',	// 0x53 KEY_PAD_PERIOD
		'\0',	// 0x54
		'\0',	// 0x55
		'\0',	// 0x56
		'\0',	// 0x57 KEY_F11
		'\0',	// 0x58 KEY_F12
		'\0',	// 0x59
		'\0',	// 0x5a
		'\0',	// 0x5b KEY_LEFT_SUPER
		'\0',	// 0x5c KEY_RIGHT_SUPER
		'\0',	// 0x5d
		'\0',	// 0x5e
		'\0',	// 0x5f
		'\0',	// 0x60
		'\0',	// 0x61
		'\0',	// 0x62
		'\0',	// 0x63
		'\0',	// 0x64
		'\0',	// 0x65
		'\0',	// 0x66
		'\0',	// 0x67
		'\0',	// 0x68
		'\0',	// 0x69
		'\0',	// 0x6a
		'\0',	// 0x6b
		'\0',	// 0x6c
		'\0',	// 0x6d
		'\0',	// 0x6e
		'\0',	// 0x6f
		'\0',	// 0x70
		'\0',	// 0x71
		'\0',	// 0x72
		'\0',	// 0x73
		'\0',	// 0x74
		'\0',	// 0x75
		'\0',	// 0x76
		'\0',	// 0x77
		'\0',	// 0x78
		'\0',	// 0x79
		'\0',	// 0x7a
		'\0',	// 0x7b
		'\0',	// 0x7c
		'\0',	// 0x7d
		'\0',	// 0x7e
		'\0',	// 0x7f
		'\0',	// 0x80
		'\0',	// 0x81
		'\0',	// 0x82
		'\0',	// 0x83
		'\0',	// 0x84
		'\0',	// 0x85
		'\0',	// 0x86
		'\0',	// 0x87
		'\0',	// 0x88
		'\0',	// 0x89
		'\0',	// 0x8a
		'\0',	// 0x8b
		'\0',	// 0x8c
		'\0',	// 0x8d
		'\0',	// 0x8e
		'\0',	// 0x8f
		'\0',	// 0x90
		'\0',	// 0x91
		'\0',	// 0x92
		'\0',	// 0x93
		'\0',	// 0x94
		'\0',	// 0x95
		'\0',	// 0x96
		'\0',	// 0x97
		'\0',	// 0x98
		'\0',	// 0x99
		'\0',	// 0x9a
		'\0',	// 0x9b
		'\0',	// 0x9c
		'\0',	// 0x9d
		'\0',	// 0x9e
		'\0',	// 0x9f
		'\0',	// 0xa0
		'\0',	// 0xa1
		'\0',	// 0xa2
		'\0',	// 0xa3
		'\0',	// 0xa4
		'\0',	// 0xa5
		'\0',	// 0xa6
		'\0',	// 0xa7
		'\0',	// 0xa8
		'\0',	// 0xa9
		'\0',	// 0xaa
		'\0',	// 0xab
		'\0',	// 0xac
		'\0',	// 0xad
		'\0',	// 0xae
		'\0',	// 0xaf
		'\0',	// 0xb0
		'\0',	// 0xb1
		'\0',	// 0xb2
		'\0',	// 0xb3
		'\0',	// 0xb4
		'\0',	// 0xb5
		'\0',	// 0xb6
		'\0',	// 0xb7
		'\0',	// 0xb8
		'\0',	// 0xb9
		'\0',	// 0xba
		'\0',	// 0xbb
		'\0',	// 0xbc
		'\0',	// 0xbd
		'\0',	// 0xbe
		'\0',	// 0xbf
		'\0',	// 0xc0
		'\0',	// 0xc1
		'\0',	// 0xc2
		'\0',	// 0xc3
		'\0',	// 0xc4
		'\0',	// 0xc5
		'\0',	// 0xc6
		'\0',	// 0xc7
		'\0',	// 0xc8
		'\0',	// 0xc9
		'\0',	// 0xca
		'\0',	// 0xcb
		'\0',	// 0xcc
		'\0',	// 0xcd
		'\0',	// 0xce
		'\0',	// 0xcf
		'\0',	// 0xd0
		'\0',	// 0xd1
		'\0',	// 0xd2
		'\0',	// 0xd3
		'\0',	// 0xd4
		'\0',	// 0xd5
		'\0',	// 0xd6
		'\0',	// 0xd7
		'\0',	// 0xd8
		'\0',	// 0xd9
		'\0',	// 0xda
		'\0',	// 0xdb
		'\0',	// 0xdc
		'\0',	// 0xdd
		'\0',	// 0xde
		'\0',	// 0xdf
		'\0',	// 0xe0
		'\0',	// 0xe1
		'\0',	// 0xe2
		'\0',	// 0xe3
		'\0',	// 0xe4
		'\0',	// 0xe5
		'\0',	// 0xe6
		'\0',	// 0xe7
		'\0',	// 0xe8
		'\0',	// 0xe9
		'\0',	// 0xea
		'\0',	// 0xeb
		'\0',	// 0xec
		'\0',	// 0xed
		'\0',	// 0xee
		'\0',	// 0xef
		'\0',	// 0xf0
		'\0',	// 0xf1
		'\0',	// 0xf2
		'\0',	// 0xf3
		'\0',	// 0xf4
		'\0',	// 0xf5
		'\0',	// 0xf6
		'\0',	// 0xf7
		'\0',	// 0xf8
		'\0',	// 0xf9
		'\0',	// 0xfa
		'\0',	// 0xfb
		'\0',	// 0xfc
		'\0',	// 0xfd
		'\0',	// 0xfe
		'\0'	// 0xff
	};
	return english_no_caps_lock_no_shift_character_map[signal];
}

void decode_keyboard_interrupt(unsigned char signal)
{
	Event event;
	unsigned char keyboard_command;
	KeyboardTransmission *keyboard_transmission;
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
		// Control CAPS LOCK LED
		send_byte_to_keyboard(PORT_KEYBOARD_DATA, KEYBOARD_COMMAND_SET_LED);
		send_byte_to_keyboard(PORT_KEYBOARD_DATA, (keyboard_flags & KEYBOARD_FLAG_CAPS_LOCK_ON) ? KEYBOARD_LED_CAPS_LOCK : 0);
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
	case KEY_SPACE:
		if(keyboard_flags & KEYBOARD_FLAG_SUPER_KEY_PUSHED)keyboard_flags ^= KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE; // Switch keyboard layout
		break;
	case KEYBOARD_SUCCESS_ACK:
		if(keyboard_transmission = dequeue(keyboard_send_buffer))
		{
			outb(keyboard_transmission->port, keyboard_transmission->data);
			last_keyboard_transmission = *keyboard_transmission;
		}
		else keyboard_flags |= KEYBOARD_FLAG_SEND_READY;
		break;
	case KEYBOARD_FAILURE_ACK:
		outb(last_keyboard_transmission.port, last_keyboard_transmission.data);
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
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '!' : '1';
		break;
	case KEY_2:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '@' : '2';
		break;
	case KEY_3:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '#' : '3';
		break;
	case KEY_4:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '$' : '4';
		break;
	case KEY_5:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '%' : '5';
		break;
	case KEY_6:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '^' : '6';
		break;
	case KEY_7:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '&' : '7';
		break;
	case KEY_8:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '*' : '8';
		break;
	case KEY_9:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '(' : '9';
		break;
	case KEY_0:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? ')' : '0';
		break;
	case KEY_MINUS:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '_' : '-';
		break;
	case KEY_EQUAL:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '+' : '=';
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
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '{' : '[';
		break;
	case KEY_RIGHT_SQUARE_BRACKET:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '}' : ']';
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
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? ':' : ';';
		break;
	case KEY_QUOTE:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '\"' : '\'';
		break;
	case KEY_BACKQUOTE:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '~' : '`';
		break;
	case KEY_BACKSLASH:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '|' : '\\';
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
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '<' : ',';
		break;
	case KEY_PERIOD:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '>' : '.';
		break;
	case KEY_SLASH:
		event.event_union.keyboard_event.character = (keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED) ? '?' : '/';
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
	send_command_to_keyboard(KEYBOARD_COMMAND_SET_MODE, KEYBOARD_MODE_KEYBOARD_INTERRUPT | KEYBOARD_MODE_MOUSE_INTERRUPT | KEYBOARD_MODE_SYSTEM_FLAG | KEYBOARD_MODE_SCANCODE01);
	keyboard_send_buffer = create_queue(sizeof(KeyboardTransmission));
	keyboard_flags = KEYBOARD_FLAG_INTERRUPT_ENABLED | KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_SEND_READY;
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

void send_byte_to_keyboard(unsigned short port, unsigned char data)
{
	KeyboardTransmission keyboard_transmission;
	KeyboardTransmission *first_keyboard_transmission;
	if(get_eflags() & EFLAGS_INTERRUPT_FLAG)
	{
		keyboard_transmission.port = port;
		keyboard_transmission.data = data;
		enqueue(keyboard_send_buffer, &keyboard_transmission);
		if(keyboard_flags & KEYBOARD_FLAG_SEND_READY)if(first_keyboard_transmission = dequeue(keyboard_send_buffer))
		{
			keyboard_flags &= ~KEYBOARD_FLAG_SEND_READY;
			outb(first_keyboard_transmission->port, first_keyboard_transmission->data);
			last_keyboard_transmission = *first_keyboard_transmission;
		}
	}
	else
	{
		wait_to_send_to_keyboard();
		outb(port, data);
	}
}

void send_command_to_keyboard(unsigned char command, unsigned char data)
{
	send_byte_to_keyboard(PORT_KEYBOARD_COMMAND, command);
	send_byte_to_keyboard(PORT_KEYBOARD_DATA, data);
}

void wait_to_send_to_keyboard(void)
{
	while(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_UNSENDABLE);
}

void wait_to_receive_from_keyboard(void)
{
	while(!(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_RECEIVED));
}

