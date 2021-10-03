#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define PORT_KEYBOARD_COMMAND	0x0064
#define PORT_KEYBOARD_DATA	0x0060
#define PORT_KEYBOARD_STATUS	0x0064

#define KEYBOARD_COMMAND_GET_COMPAQ_STATUS		0x20
#define KEYBOARD_COMMAND_SET_MODE			0x60
#define KEYBOARD_COMMAND_ENABLE_AUXILIARY_DEVICE	0xa8
#define KEYBOARD_COMMAND_SEND_TO_MOUSE			0xd4
#define KEYBOARD_COMMAND_SET_LED 0xed

#define KEYBOARD_LED_SCROLL_LOCK 0x01
#define KEYBOARD_LED_NUMBER_LOCK 0x02
#define KEYBOARD_LED_CAPS_LOCK 0x04

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

typedef struct _KeyboardEvent
{
	char character;
	unsigned short flags;
	#define KEYBOARD_FLAG_ALT_KEY_PUSHED 0x0001
	#define KEYBOARD_FLAG_CAPS_LOCK_ON 0x0002
	#define KEYBOARD_FLAG_CONTROL_KEY_PUSHED 0x0004
	#define KEYBOARD_FLAG_INTERRUPT_ENABLED 0x0008
	#define KEYBOARD_FLAG_KEY_PUSHED 0x0010
	#define KEYBOARD_FLAG_KEY_RELEASED 0x0020
	#define KEYBOARD_FLAG_LAYOUT_ENGLISH 0x0040
	#define KEYBOARD_FLAG_LAYOUT_JAPANESE 0x0080
	#define KEYBOARD_FLAG_LEFT_SHIFT_KEY_PUSHED 0x0100
	#define KEYBOARD_FLAG_LEFT_SUPER_KEY_PUSHED 0x0200
	#define KEYBOARD_FLAG_RIGHT_SHIFT_KEY_PUSHED 0x0400
	#define KEYBOARD_FLAG_RIGHT_SUPER_KEY_PUSHED 0x0800
	#define KEYBOARD_FLAG_SEND_READY 0x1000
	#define KEYBOARD_FLAG_SHIFT_KEY_PUSHED 0x2000
	#define KEYBOARD_FLAG_SUPER_KEY_PUSHED 0x4000
	#define KEYBOARD_FLAG_UPPER_CASE 0x8000
	unsigned char keycode;
	#define KEY_ESC 0x01
	#define KEY_1 0x02
	#define KEY_2 0x03
	#define KEY_3 0x04
	#define KEY_4 0x05
	#define KEY_5 0x06
	#define KEY_6 0x07
	#define KEY_7 0x08
	#define KEY_8 0x09
	#define KEY_9 0x0a
	#define KEY_0 0x0b
	#define KEY_MINUS 0x0c
	#define KEY_EQUAL 0x0d
	#define KEY_JAPANESE_CIRCUMFLEX 0x0d
	#define KEY_BACKSPACE 0x0e
	#define KEY_TAB 0x0f
	#define KEY_Q 0x10
	#define KEY_W 0x11
	#define KEY_E 0x12
	#define KEY_R 0x13
	#define KEY_T 0x14
	#define KEY_Y 0x15
	#define KEY_U 0x16
	#define KEY_I 0x17
	#define KEY_O 0x18
	#define KEY_P 0x19
	#define KEY_LEFT_SQUARE_BRACKET 0x1a
	#define KEY_JAPANESE_AT_SIGN 0x1a
	#define KEY_RIGHT_SQUARE_BRACKET 0x1b
	#define KEY_JAPANESE_LEFT_SQUARE_BRACKET 0x1b
	#define KEY_ENTER 0x1c
	#define KEY_CONTROL 0x1d
	#define KEY_A 0x1e
	#define KEY_S 0x1f
	#define KEY_D 0x20
	#define KEY_F 0x21
	#define KEY_G 0x22
	#define KEY_H 0x23
	#define KEY_J 0x24
	#define KEY_K 0x25
	#define KEY_L 0x26
	#define KEY_SEMICOLON 0x27
	#define KEY_QUOTE 0x28
	#define KEY_JAPANESE_COLON 0x28
	#define KEY_BACKQUOTE 0x29
	#define KEY_JAPANESE_ZENKAKU_HANKAKU 0x29
	#define KEY_LEFT_SHIFT 0x2a
	#define KEY_BACKSLASH 0x2b
	#define KEY_JAPANESE_RIGHT_SQUARE_BRACKET 0x2b
	#define KEY_Z 0x2c
	#define KEY_X 0x2d
	#define KEY_C 0x2e
	#define KEY_V 0x2f
	#define KEY_B 0x30
	#define KEY_N 0x31
	#define KEY_M 0x32
	#define KEY_COMMA 0x33
	#define KEY_PERIOD 0x34
	#define KEY_SLASH 0x35
	#define KEY_RIGHT_SHIFT 0x36
	#define KEY_PAD_ASTERISK 0x37
	#define KEY_ALT 0x38
	#define KEY_SPACE 0x39
	#define KEY_CAPS_LOCK 0x3a
	#define KEY_F1 0x3b
	#define KEY_F2 0x3c
	#define KEY_F3 0x3d
	#define KEY_F4 0x3e
	#define KEY_F5 0x3f
	#define KEY_F6 0x40
	#define KEY_F7 0x41
	#define KEY_F8 0x42
	#define KEY_F9 0x43
	#define KEY_F10 0x44
	#define KEY_NUMBER_LOCK 0x45
	#define KEY_SCROLL_LOCK 0x46
	#define KEY_PAD_7 0x47
	#define KEY_HOME 0x47
	#define KEY_PAD_8 0x48
	#define KEY_UP_ARROW 0x48
	#define KEY_PAD_9 0x49
	#define KEY_PAGE_UP 0x49
	#define KEY_PAD_MINUS 0x4a
	#define KEY_PAD_4 0x4b
	#define KEY_LEFT_ARROW 0x4b
	#define KEY_PAD_5 0x4c
	#define KEY_PAD_6 0x4d
	#define KEY_RIGHT_ARROW 0x4d
	#define KEY_PAD_PLUS 0x4e
	#define KEY_PAD_1 0x4f
	#define KEY_END 0x4f
	#define KEY_PAD_2 0x50
	#define KEY_DOWN_ARROW 0x50
	#define KEY_PAD_3 0x51
	#define KEY_PAGE_DOWN 0x51
	#define KEY_PAD_0 0x52
	#define KEY_INSERT 0x52
	#define KEY_PAD_PERIOD 0x53
	#define KEY_DELETE 0x53
	#define KEY_F11 0x57
	#define KEY_F12 0x58
	#define KEY_LEFT_SUPER 0x5b
	#define KEY_RIGHT_SUPER 0x5c
	#define KEY_JAPANESE_HIRAGANA 0x70
	#define KEY_JAPANESE_UNDERSCORE 0x73
	#define KEY_JAPANESE_HENKAN 0x79
	#define KEY_JAPANESE_MUHENKAN 0x7b
	#define KEY_JAPANESE_YEN 0x7d
	#define KEY_RELEASED 0x80
	#define KEYBOARD_SUCCESS_ACK 0xfa
	#define KEYBOARD_FAILURE_ACK 0xfe
} KeyboardEvent;

typedef struct _KeyboardInterruptEvent
{
	unsigned char signal;
} KeyboardInterruptEvent;

void decode_keyboard_interrupt(unsigned char signal);
void init_keyboard(void);
void keyboard_interrupt_handler(void);
unsigned char receive_from_keyboard(void);
void send_byte_to_keyboard(unsigned short port, unsigned char data);
void send_command_to_keyboard(unsigned char command, unsigned char data);
void wait_to_send_to_keyboard(void);
void wait_to_receive_from_keyboard(void);

#endif

