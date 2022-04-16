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

const char character_map_english_no_caps_lock_no_shift[0x80] = 
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0'	// 0x7f
};
const char character_map_english_no_caps_lock_shift[0x80] = 
{
	'\0',	// 0x00
	'\0',	// 0x01 KEY_ESC
	'!',	// 0x02 KEY_1
	'@',	// 0x03 KEY_2
	'#',	// 0x04 KEY_3
	'$',	// 0x05 KEY_4
	'%',	// 0x06 KEY_5
	'^',	// 0x07 KEY_6
	'&',	// 0x08 KEY_7
	'*',	// 0x09 KEY_8
	'(',	// 0x0a KEY_9
	')',	// 0x0b KEY_0
	'_',	// 0x0c KEY_MINUS
	'+',	// 0x0d KEY_EQUAL
	'\b',	// 0x0e KEY_BACKSPACE
	'\t',	// 0x0f KEY_TAB
	'Q',	// 0x10 KEY_Q
	'W',	// 0x11 KEY_W
	'E',	// 0x12 KEY_E
	'R',	// 0x13 KEY_R
	'T',	// 0x14 KEY_T
	'Y',	// 0x15 KEY_Y
	'U',	// 0x16 KEY_U
	'I',	// 0x17 KEY_I
	'O',	// 0x18 KEY_O
	'P',	// 0x19 KEY_P
	'{',	// 0x1a KEY_LEFT_SQUARE_BRACKET
	'}',	// 0x1b KEY_RIGHT_SQUARE_BRACKET
	'\n',	// 0x1c KEY_ENTER
	'\0',	// 0x1d KEY_CONTROL
	'A',	// 0x1e KEY_A
	'S',	// 0x1f KEY_S
	'D',	// 0x20 KEY_D
	'F',	// 0x21 KEY_F
	'G',	// 0x22 KEY_G
	'H',	// 0x23 KEY_H
	'J',	// 0x24 KEY_J
	'K',	// 0x25 KEY_K
	'L',	// 0x26 KEY_L
	':',	// 0x27 KEY_SEMICOLON
	'\"',	// 0x28 KEY_QUOTE
	'~',	// 0x29 KEY_BACKQUOTE
	'\0',	// 0x2a KEY_LEFT_SHIFT
	'|',	// 0x2b KEY_BACKSLASH
	'Z',	// 0x2c KEY_Z
	'X',	// 0x2d KEY_X
	'C',	// 0x2e KEY_C
	'V',	// 0x2f KEY_V
	'B',	// 0x30 KEY_B
	'N',	// 0x31 KEY_N
	'M',	// 0x32 KEY_M
	'<',	// 0x33 KEY_COMMA
	'>',	// 0x34 KEY_PERIOD
	'?',	// 0x35 KEY_SLASH
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0'	// 0x7f
};
const char character_map_english_caps_lock_no_shift[0x80] = 
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
	'Q',	// 0x10 KEY_Q
	'W',	// 0x11 KEY_W
	'E',	// 0x12 KEY_E
	'R',	// 0x13 KEY_R
	'T',	// 0x14 KEY_T
	'Y',	// 0x15 KEY_Y
	'U',	// 0x16 KEY_U
	'I',	// 0x17 KEY_I
	'O',	// 0x18 KEY_O
	'P',	// 0x19 KEY_P
	'[',	// 0x1a KEY_LEFT_SQUARE_BRACKET
	']',	// 0x1b KEY_RIGHT_SQUARE_BRACKET
	'\n',	// 0x1c KEY_ENTER
	'\0',	// 0x1d KEY_CONTROL
	'A',	// 0x1e KEY_A
	'S',	// 0x1f KEY_S
	'D',	// 0x20 KEY_D
	'F',	// 0x21 KEY_F
	'G',	// 0x22 KEY_G
	'H',	// 0x23 KEY_H
	'J',	// 0x24 KEY_J
	'K',	// 0x25 KEY_K
	'L',	// 0x26 KEY_L
	';',	// 0x27 KEY_SEMICOLON
	'\'',	// 0x28 KEY_QUOTE
	'`',	// 0x29 KEY_BACKQUOTE
	'\0',	// 0x2a KEY_LEFT_SHIFT
	'\\',	// 0x2b KEY_BACKSLASH
	'Z',	// 0x2c KEY_Z
	'X',	// 0x2d KEY_X
	'C',	// 0x2e KEY_C
	'V',	// 0x2f KEY_V
	'B',	// 0x30 KEY_B
	'N',	// 0x31 KEY_N
	'M',	// 0x32 KEY_M
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0'	// 0x7f
};
const char character_map_english_caps_lock_shift[0x80] = 
{
	'\0',	// 0x00
	'\0',	// 0x01 KEY_ESC
	'!',	// 0x02 KEY_1
	'@',	// 0x03 KEY_2
	'#',	// 0x04 KEY_3
	'$',	// 0x05 KEY_4
	'%',	// 0x06 KEY_5
	'^',	// 0x07 KEY_6
	'&',	// 0x08 KEY_7
	'*',	// 0x09 KEY_8
	'(',	// 0x0a KEY_9
	')',	// 0x0b KEY_0
	'_',	// 0x0c KEY_MINUS
	'+',	// 0x0d KEY_EQUAL
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
	'{',	// 0x1a KEY_LEFT_SQUARE_BRACKET
	'}',	// 0x1b KEY_RIGHT_SQUARE_BRACKET
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
	':',	// 0x27 KEY_SEMICOLON
	'\"',	// 0x28 KEY_QUOTE
	'~',	// 0x29 KEY_BACKQUOTE
	'\0',	// 0x2a KEY_LEFT_SHIFT
	'|',	// 0x2b KEY_BACKSLASH
	'z',	// 0x2c KEY_Z
	'x',	// 0x2d KEY_X
	'c',	// 0x2e KEY_C
	'v',	// 0x2f KEY_V
	'b',	// 0x30 KEY_B
	'n',	// 0x31 KEY_N
	'm',	// 0x32 KEY_M
	'<',	// 0x33 KEY_COMMA
	'>',	// 0x34 KEY_PERIOD
	'?',	// 0x35 KEY_SLASH
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0'	// 0x7f
};
const char character_map_japanese_no_caps_lock_no_shift[0x80] = 
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
	'^',	// 0x0d KEY_JAPANESE_CIRCUMFLEX
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
	'@',	// 0x1a KEY_JAPANESE_AT_SIGN
	'[',	// 0x1b KEY_JAPANESE_LEFT_SQUARE_BRACKET
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
	':',	// 0x28 KEY_JAPANESE_COLON
	'\0',	// 0x29 KEY_JAPANESE_ZENKAKU_HANKAKU
	'\0',	// 0x2a KEY_LEFT_SHIFT
	']',	// 0x2b KEY_JAPANESE_RIGHT_SQUARE_BRACKET
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0',	// 0x70 KEY_JAPANESE_HIRAGANA
	'\0',	// 0x71
	'\0',	// 0x72
	'\\',	// 0x73 KEY_JAPANESE_BACKSLASH
	'\0',	// 0x74
	'\0',	// 0x75
	'\0',	// 0x76
	'\0',	// 0x77
	'\0',	// 0x78
	'\0',	// 0x79 KEY_JAPANESE_HENKAN
	'\0',	// 0x7a
	'\0',	// 0x7b KEY_JAPANESE_MUHENKAN
	'\0',	// 0x7c
	'\\',	// 0x7d KEY_JAPANESE_YEN
	'\0',	// 0x7e
	'\0'	// 0x7f
};
const char character_map_japanese_no_caps_lock_shift[0x80] = 
{
	'\0',	// 0x00
	'\0',	// 0x01 KEY_ESC
	'!',	// 0x02 KEY_1
	'"',	// 0x03 KEY_2
	'#',	// 0x04 KEY_3
	'$',	// 0x05 KEY_4
	'%',	// 0x06 KEY_5
	'&',	// 0x07 KEY_6
	'\'',	// 0x08 KEY_7
	'(',	// 0x09 KEY_8
	')',	// 0x0a KEY_9
	'\0',	// 0x0b KEY_0
	'=',	// 0x0c KEY_MINUS
	'~',	// 0x0d KEY_JAPANESE_CIRCUMFLEX
	'\b',	// 0x0e KEY_BACKSPACE
	'\t',	// 0x0f KEY_TAB
	'Q',	// 0x10 KEY_Q
	'W',	// 0x11 KEY_W
	'E',	// 0x12 KEY_E
	'R',	// 0x13 KEY_R
	'T',	// 0x14 KEY_T
	'Y',	// 0x15 KEY_Y
	'U',	// 0x16 KEY_U
	'I',	// 0x17 KEY_I
	'O',	// 0x18 KEY_O
	'P',	// 0x19 KEY_P
	'`',	// 0x1a KEY_JAPANESE_AT_SIGN
	'{',	// 0x1b KEY_JAPANESE_LEFT_SQUARE_BRACKET
	'\n',	// 0x1c KEY_ENTER
	'\0',	// 0x1d KEY_CONTROL
	'A',	// 0x1e KEY_A
	'S',	// 0x1f KEY_S
	'D',	// 0x20 KEY_D
	'F',	// 0x21 KEY_F
	'G',	// 0x22 KEY_G
	'H',	// 0x23 KEY_H
	'J',	// 0x24 KEY_J
	'K',	// 0x25 KEY_K
	'L',	// 0x26 KEY_L
	'+',	// 0x27 KEY_SEMICOLON
	'*',	// 0x28 KEY_JAPANESE_COLON
	'\0',	// 0x29 KEY_JAPANESE_ZENKAKU_HANKAKU
	'\0',	// 0x2a KEY_LEFT_SHIFT
	'}',	// 0x2b KEY_JAPANESE_RIGHT_SQUARE_BRACKET
	'Z',	// 0x2c KEY_Z
	'X',	// 0x2d KEY_X
	'C',	// 0x2e KEY_C
	'V',	// 0x2f KEY_V
	'B',	// 0x30 KEY_B
	'N',	// 0x31 KEY_N
	'M',	// 0x32 KEY_M
	'<',	// 0x33 KEY_COMMA
	'>',	// 0x34 KEY_PERIOD
	'?',	// 0x35 KEY_SLASH
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0',	// 0x70 KEY_JAPANESE_HIRAGANA
	'\0',	// 0x71
	'\0',	// 0x72
	'_',	// 0x73 KEY_JAPANESE_BACKSLASH
	'\0',	// 0x74
	'\0',	// 0x75
	'\0',	// 0x76
	'\0',	// 0x77
	'\0',	// 0x78
	'\0',	// 0x79 KEY_JAPANESE_HENKAN
	'\0',	// 0x7a
	'\0',	// 0x7b KEY_JAPANESE_MUHENKAN
	'\0',	// 0x7c
	'|',	// 0x7d KEY_JAPANESE_YEN
	'\0',	// 0x7e
	'\0'	// 0x7f
};
const char character_map_japanese_caps_lock_no_shift[0x80] = 
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
	'^',	// 0x0d KEY_JAPANESE_CIRCUMFLEX
	'\b',	// 0x0e KEY_BACKSPACE
	'\t',	// 0x0f KEY_TAB
	'Q',	// 0x10 KEY_Q
	'W',	// 0x11 KEY_W
	'E',	// 0x12 KEY_E
	'R',	// 0x13 KEY_R
	'T',	// 0x14 KEY_T
	'Y',	// 0x15 KEY_Y
	'U',	// 0x16 KEY_U
	'I',	// 0x17 KEY_I
	'O',	// 0x18 KEY_O
	'P',	// 0x19 KEY_P
	'@',	// 0x1a KEY_JAPANESE_AT_SIGN
	'[',	// 0x1b KEY_JAPANESE_LEFT_SQUARE_BRACKET
	'\n',	// 0x1c KEY_ENTER
	'\0',	// 0x1d KEY_CONTROL
	'A',	// 0x1e KEY_A
	'S',	// 0x1f KEY_S
	'D',	// 0x20 KEY_D
	'F',	// 0x21 KEY_F
	'G',	// 0x22 KEY_G
	'H',	// 0x23 KEY_H
	'J',	// 0x24 KEY_J
	'K',	// 0x25 KEY_K
	'L',	// 0x26 KEY_L
	';',	// 0x27 KEY_SEMICOLON
	':',	// 0x28 KEY_JAPANESE_COLON
	'\0',	// 0x29 KEY_JAPANESE_ZENKAKU_HANKAKU
	'\0',	// 0x2a KEY_LEFT_SHIFT
	']',	// 0x2b KEY_JAPANESE_RIGHT_SQUARE_BRACKET
	'Z',	// 0x2c KEY_Z
	'X',	// 0x2d KEY_X
	'C',	// 0x2e KEY_C
	'V',	// 0x2f KEY_V
	'B',	// 0x30 KEY_B
	'N',	// 0x31 KEY_N
	'M',	// 0x32 KEY_M
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0',	// 0x70 KEY_JAPANESE_HIRAGANA
	'\0',	// 0x71
	'\0',	// 0x72
	'\\',	// 0x73 KEY_JAPANESE_BACKSLASH
	'\0',	// 0x74
	'\0',	// 0x75
	'\0',	// 0x76
	'\0',	// 0x77
	'\0',	// 0x78
	'\0',	// 0x79 KEY_JAPANESE_HENKAN
	'\0',	// 0x7a
	'\0',	// 0x7b KEY_JAPANESE_MUHENKAN
	'\0',	// 0x7c
	'\\',	// 0x7d KEY_JAPANESE_YEN
	'\0',	// 0x7e
	'\0'	// 0x7f
};
const char character_map_japanese_caps_lock_shift[0x80] = 
{
	'\0',	// 0x00
	'\0',	// 0x01 KEY_ESC
	'!',	// 0x02 KEY_1
	'"',	// 0x03 KEY_2
	'#',	// 0x04 KEY_3
	'$',	// 0x05 KEY_4
	'%',	// 0x06 KEY_5
	'&',	// 0x07 KEY_6
	'\'',	// 0x08 KEY_7
	'(',	// 0x09 KEY_8
	')',	// 0x0a KEY_9
	'\0',	// 0x0b KEY_0
	'=',	// 0x0c KEY_MINUS
	'~',	// 0x0d KEY_JAPANESE_CIRCUMFLEX
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
	'`',	// 0x1a KEY_JAPANESE_AT_SIGN
	'{',	// 0x1b KEY_JAPANESE_LEFT_SQUARE_BRACKET
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
	'+',	// 0x27 KEY_SEMICOLON
	'*',	// 0x28 KEY_JAPANESE_COLON
	'\0',	// 0x29 KEY_JAPANESE_ZENKAKU_HANKAKU
	'\0',	// 0x2a KEY_LEFT_SHIFT
	'}',	// 0x2b KEY_JAPANESE_RIGHT_SQUARE_BRACKET
	'z',	// 0x2c KEY_Z
	'x',	// 0x2d KEY_X
	'c',	// 0x2e KEY_C
	'v',	// 0x2f KEY_V
	'b',	// 0x30 KEY_B
	'n',	// 0x31 KEY_N
	'm',	// 0x32 KEY_M
	'<',	// 0x33 KEY_COMMA
	'>',	// 0x34 KEY_PERIOD
	'?',	// 0x35 KEY_SLASH
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
	'+',	// 0x4e KEY_PAD_PLUS
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
	'\0',	// 0x70 KEY_JAPANESE_HIRAGANA
	'\0',	// 0x71
	'\0',	// 0x72
	'_',	// 0x73 KEY_JAPANESE_BACKSLASH
	'\0',	// 0x74
	'\0',	// 0x75
	'\0',	// 0x76
	'\0',	// 0x77
	'\0',	// 0x78
	'\0',	// 0x79 KEY_JAPANESE_HENKAN
	'\0',	// 0x7a
	'\0',	// 0x7b KEY_JAPANESE_MUHENKAN
	'\0',	// 0x7c
	'|',	// 0x7d KEY_JAPANESE_YEN
	'\0',	// 0x7e
	'\0'	// 0x7f
};

unsigned short keyboard_flags = KEYBOARD_FLAG_LAYOUT_ENGLISH;
Queue *keyboard_interrupt_queue;
Queue *keyboard_send_buffer;
KeyboardTransmission last_keyboard_transmission;

char get_character(unsigned char signal)
{
	if(keyboard_flags & KEYBOARD_FLAG_LAYOUT_ENGLISH)
	{
		if(keyboard_flags & KEYBOARD_FLAG_CAPS_LOCK_ON)
		{
			if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)return character_map_english_caps_lock_shift[signal & ~KEY_RELEASED];
			else return character_map_english_caps_lock_no_shift[signal & ~KEY_RELEASED];
		}
		else
		{
			if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)return character_map_english_no_caps_lock_shift[signal & ~KEY_RELEASED];
			else return character_map_english_no_caps_lock_no_shift[signal & ~KEY_RELEASED];
		}
	}
	else if(keyboard_flags & KEYBOARD_FLAG_LAYOUT_JAPANESE)
	{
		if(keyboard_flags & KEYBOARD_FLAG_CAPS_LOCK_ON)
		{
			if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)return character_map_japanese_caps_lock_shift[signal & ~KEY_RELEASED];
			else return character_map_japanese_caps_lock_no_shift[signal & ~KEY_RELEASED];
		}
		else
		{
			if(keyboard_flags & KEYBOARD_FLAG_SHIFT_KEY_PUSHED)return character_map_japanese_no_caps_lock_shift[signal & ~KEY_RELEASED];
			else return character_map_japanese_no_caps_lock_no_shift[signal & ~KEY_RELEASED];
		}
	}
	else return '\0';
}

void decode_keyboard_interrupt(unsigned char signal)
{
	Event event;
	KeyboardTransmission *keyboard_transmission;
	event.type = EVENT_TYPE_KEYBOARD_EVENT;
	// Pushed or released
	if(signal & KEY_RELEASED)
	{
		keyboard_flags &= ~KEYBOARD_FLAG_KEY_PUSHED;
		keyboard_flags |= KEYBOARD_FLAG_KEY_RELEASED;
	}
	else
	{
		keyboard_flags |= KEYBOARD_FLAG_KEY_PUSHED;
		keyboard_flags &= ~KEYBOARD_FLAG_KEY_RELEASED;
	}
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
		if((keyboard_transmission = dequeue(keyboard_send_buffer)))
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
	event.event_union.keyboard_event.character = get_character(signal);
	enqueue(keyboard_interrupt_queue, &event);
}

void init_keyboard(Queue *interrupt_queue, Task *task)
{
	send_command_to_keyboard(KEYBOARD_COMMAND_SET_MODE, KEYBOARD_MODE_KEYBOARD_INTERRUPT | KEYBOARD_MODE_MOUSE_INTERRUPT | KEYBOARD_MODE_SYSTEM_FLAG | KEYBOARD_MODE_SCANCODE01);
	keyboard_interrupt_queue = interrupt_queue;
	keyboard_send_buffer = create_queue(sizeof(KeyboardTransmission), task);
	keyboard_flags = KEYBOARD_FLAG_INTERRUPT_ENABLED | KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_SEND_READY;
}

void keyboard_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_KEYBOARD);
	event.type = EVENT_TYPE_KEYBOARD_INTERRUPT;
	event.event_union.keyboard_interrupt.signal = inb(PORT_KEYBOARD_DATA);
	enqueue(keyboard_interrupt_queue, &event);
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
		if(keyboard_flags & KEYBOARD_FLAG_SEND_READY)if((first_keyboard_transmission = dequeue(keyboard_send_buffer)))
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

