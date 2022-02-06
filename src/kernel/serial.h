#ifndef _SERIAL_H_
#define _SERIAL_H_

#define _STRING(x) #x
#define _STRING2(x) _STRING(x)
#define ERROR_MESSAGE() print_serial("ERROR " __FILE__ " : " _STRING2(__LINE__) "\n")

// COM1 interrupt handler
void com1_interrupt_handler(void);

// COM2 interrupt handler
void com2_interrupt_handler(void);

// switch from polling to interrupt
void init_serial_interrupt(void);

// print LF
void new_line_serial(void);

// printf
void printf_serial(char const *format, ...);

// print value as hexadecimal
void print_byte_hex_serial(unsigned char value);

// print value as hexadecimal
void print_dword_hex_serial(unsigned int value);

// print string to serial port COM1
void print_serial(char const *string);

// print value as hexadecimal
void print_word_hex_serial(unsigned short value);

// print a character to serial port COM1
void put_char_serial(char character);

#endif

