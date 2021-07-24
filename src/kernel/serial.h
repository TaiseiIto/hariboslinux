#ifndef _SERIAL_H_
#define _SERIAL_H_

// print LF
void new_line_serial_polling(void);

// print value as hexadecimal
void print_byte_hex_serial_polling(unsigned char value);

// print value as hexadecimal
void print_dword_hex_serial_polling(unsigned int value);

// print string to serial port COM1
void print_serial_polling(char *string);

// print value as hexadecimal
void print_word_hex_serial_polling(unsigned short value);

// print a character to serial port COM1
void putchar_serial_polling(char character);

#endif

