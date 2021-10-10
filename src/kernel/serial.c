#include "io.h"
#include "limits.h"
#include "pic.h"
#include "queue.h"
#include "serial.h"
#include "stdio.h"

#define SERIAL_FREQUENCY 115200

// COM ports
#define COM1 0x03f8
#define COM2 0x02f8
#define COM3 0x03e8
#define COM4 0x02e8

// port offsets
#define DATA_REGISTER 0 // If DLAB == 0
#define INTERRUPT_ENABLE_REGISTER 1 // If DLAB == 0
	#define INTERRUPT_DATA_AVAILABLE 0x01
	#define INTERRUPT_TRANSMITTER_EMPTY 0x02
	#define INTERRUPT_LINE_STATUS 0x04
	#define INTERRUPT_MODEM_STATUS 0x08
#define BAUD_RATE_DIVISOR_LOW 0 // If DLAB == 1
#define BAUD_RATE_DIVISOR_HIGH 1 // If DLAB == 1
#define INTERRUPT_IDENTIFICATION_REGISTER 2 // Read only register
	#define INTERRUPT_PENDING 0x01
	#define MODEM_STATUS_INTERRUPT 0x00
	#define TRANSMITTER_EMPTY_INTERRUPT 0x02
	#define RECEIVED_DATA_INTERRUPT 0x04
	#define LINE_STATUS_INTERRUPT 0x06
	#define FIFOM 0x08
	#define FIFOE 0xc0
#define FIFO_CONTROL_REGISTER 2 // Write only register
	#define TRANSMITTED_AND_RECEIVED_FIFO_ENABLE 0x01
	#define CLEAR_RECEIVED_FIFO 0x02
	#define CLEAR_TRANSMITTED_FIFO 0x04
	#define TRIGGER_BYTE_1 0x00
	#define TRIGGER_BYTE_4 0x40
	#define TRIGGER_BYTE_8 0x80
	#define TRIGGER_BYTE_14 0xc0
#define LINE_CONTROL_REGISTER 3 // The most significant bit of this register is DLAB
	#define CHARACTER_LENGTH_5 0x00
	#define CHARACTER_LENGTH_6 0x01
	#define CHARACTER_LENGTH_7 0x02
	#define CHARACTER_LENGTH_8 0x03
	#define STOP_BYTE_1 0x00
	#define STOP_BYTE_2 0x04
	#define NO_PATIRY 0x00
	#define ODD_PATIRY 0x08
	#define EVEN_PATIRY 0x18
	#define MARK_PATIRY 0x28
	#define SPACE_PATIRY 0x38
	#define DLAB 0x80
#define MODEM_CONTROL_REGISTER 4
	#define DATA_TERMINAL_READY 0x01
	#define REQUEST_TO_SEND 0x02
	#define INTERRUPT_ENABLE 0x08
	#define LOOP_BACK_FEATURE 0x10
#define LINE_STATUS_REGISTER 5
	#define DATA_READY 0x01
	#define OVERRUN_ERROR 0x02
	#define PARITY_ERROR 0x04
	#define FRAMING_ERROR 0x08
	#define BREAK_INDICATOR 0x10
	#define TRANSMITTER_HOLDING_REGISTER_EMPTY 0x20
	#define TRANSMITTER_EMPTY 0x40
	#define IMPENDING_ERROR 0x80
#define MODEL_STATUS_REGISTER 6
	#define DELTA_CLEAR_TO_SEND 0x01
	#define DELTA_DATA_SET_READY 0x02
	#define TRAILING_EDGE_OF_RING_INDICATOR 0x04
	#define DELTA_DATA_CARRIER_DETECT 0x08
	#define CLEAR_TO_SEND 0x10
	#define DATA_SET_READY 0x20
	#define RING_INDICATOR 0x40
	#define DATA_CARRIER_DETECT 0x80

#define SCRATCH_REGISTER 7 // OS developper can use this register freely

unsigned char com1_flags = 0;
#define COM_AVAILABLE 0x01
#define COM_INTERRUPT 0x02
#define COM_WRITABLE 0x04

Queue *com1_transmission_queue;

// COM1 interrupt handler
void com1_interrupt_handler(void)
{
	unsigned char *character_to_send;
	unsigned char interrupt_ID = inb(COM1 + INTERRUPT_IDENTIFICATION_REGISTER);
	finish_interruption(IRQ_COM1);
	switch(interrupt_ID & 0x06)
	{
	case LINE_STATUS_INTERRUPT:
		break;
	case MODEM_STATUS_INTERRUPT:
		break;
	case RECEIVED_DATA_INTERRUPT:
		inb(COM1 + DATA_REGISTER);
		break;
	case TRANSMITTER_EMPTY_INTERRUPT:
		if(character_to_send = dequeue(com1_transmission_queue))outb(COM1 + DATA_REGISTER, *character_to_send);
		else com1_flags |= COM_WRITABLE;
		break;
	}
}

// COM2 interrupt handler
void com2_interrupt_handler(void)
{
	finish_interruption(IRQ_COM2);
}

// switch from polling to interrupt
void init_serial_interrupt(void)
{
	unsigned int baud_rate = 115200;
	unsigned short baud_rate_divisor = SERIAL_FREQUENCY / baud_rate;
	// create com1_transmission_queue
	com1_transmission_queue = create_queue(sizeof(char));
	// 8 bits per char
	outb(COM1 + LINE_CONTROL_REGISTER, CHARACTER_LENGTH_8);
	// enable interrupt
	outb(COM1 + MODEM_CONTROL_REGISTER, INTERRUPT_ENABLE);
	outb(COM1 + INTERRUPT_ENABLE_REGISTER, INTERRUPT_DATA_AVAILABLE | INTERRUPT_TRANSMITTER_EMPTY);
	// reset baud rate
	outb(COM1 + LINE_CONTROL_REGISTER, inb(COM1 + LINE_CONTROL_REGISTER) | DLAB);
	outb(COM1 + BAUD_RATE_DIVISOR_LOW, (unsigned char)(baud_rate_divisor & 0x00ff));
	outb(COM1 + BAUD_RATE_DIVISOR_HIGH, (unsigned char)(baud_rate_divisor >> CHAR_BIT & 0x00ff));
	outb(COM1 + LINE_CONTROL_REGISTER, inb(COM1 + LINE_CONTROL_REGISTER) & ~DLAB);
	// forsake the first received byte
	inb(COM1 + DATA_REGISTER);
	com1_flags |= COM_AVAILABLE | COM_INTERRUPT | COM_WRITABLE;
}

// print LF
void new_line_serial(void)
{
	put_char_serial('\n');
}

// printf
void printf_serial(char const *format, ...)
{
	int arg_num = 1;
	char character;
	char const *input_string;
	VariadicArg integer;
	VariadicArg integer_destroyable;
	while(*format)
	{
		if(*format == '%')
		{
			unsigned char flags = 0;
			#define SPRINTF_MINUS_FLAG 0x01
			#define SPRINTF_TYPE_FLAG 0x02
			#define SPRINTF_ZERO_FLAG 0x04
			#define SPRINTF_LONG_FLAG 0x08
			#define SPRINTF_LONG_LONG_FLAG 0x10
			unsigned int length = 0;
			unsigned int num_of_digits = 0;
			format++;
			switch(*format)
			{
			case '%':
				put_char_serial('%');
				format++;
				continue;
			case '#':
				flags |= SPRINTF_TYPE_FLAG;
				format++;
				break;
			}
			if(*format == '0')
			{
				flags |= SPRINTF_ZERO_FLAG;
				format++;
			}
			while('0' <= *format && *format <= '9')
			{
				length *= 10;
				length += *format - '0';
				format++;
			}
			switch(*format)
			{
			case 'l':
				flags |= SPRINTF_LONG_FLAG;
				format++;
			}
			switch(*format)
			{
			case 'l':
				if(flags | SPRINTF_LONG_FLAG)
				{
					flags &= ~SPRINTF_LONG_FLAG;
					flags |= SPRINTF_LONG_LONG_FLAG;
					format++;
				}
			}
			switch(*format)
			{
			case 'c':
				character = get_variadic_arg(arg_num++);
				put_char_serial(character);
				break;
			case 'd':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.ints[1] = -(integer.ints[0] < 0);
				}
				if(integer.long_long_int < 0)
				{
					put_char_serial('-');
					integer.long_long_int *= -1;
					if(0 < length)length--;
					flags |= SPRINTF_MINUS_FLAG;
				}
				integer_destroyable = integer;
				if(integer.long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.long_long_int; integer_destroyable.long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.long_long_int /= 10;
					put_char_serial('0' + integer_destroyable.long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'p':
				length = 10;
				integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
				integer.unsigned_ints[1] = 0;
				put_char_serial('0');
				if(0 < length)length--;
				put_char_serial('x');
				if(0 < length)length--;
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial('0');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_serial(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 's':
				input_string = (char const *)get_variadic_arg(arg_num++);
				while(*input_string && (!length || num_of_digits++ < length))put_char_serial(*input_string++);
				break;
			case 'u':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 10;
					put_char_serial('0' + integer_destroyable.unsigned_long_long_int % 10);
					num_of_digits--;
				}
				break;
			case 'x':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_serial('0');
					if(0 < length)length--;
					put_char_serial('x');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_serial(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'a' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			case 'X':
				if(flags & SPRINTF_LONG_LONG_FLAG)
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = get_variadic_arg(arg_num++);
				}
				else
				{
					integer.unsigned_ints[0] = get_variadic_arg(arg_num++);
					integer.unsigned_ints[1] = 0;
				}
				if(flags & SPRINTF_TYPE_FLAG)
				{
					put_char_serial('0');
					if(0 < length)length--;
					put_char_serial('X');
					if(0 < length)length--;
				}
				integer_destroyable = integer;
				if(integer.unsigned_long_long_int)for(num_of_digits = 0; 0 < integer_destroyable.unsigned_long_long_int; integer_destroyable.unsigned_long_long_int /= 0x10)num_of_digits++;
				else num_of_digits = 1;
				if(num_of_digits < length)while(num_of_digits < length)
				{
					put_char_serial(flags & SPRINTF_ZERO_FLAG ? '0' : ' ');
					length--;
				}
				while(0 < num_of_digits)
				{
					integer_destroyable = integer;
					for(unsigned int i = 0; i + 1 < num_of_digits; i++)integer_destroyable.unsigned_long_long_int /= 0x10;
					put_char_serial(integer_destroyable.unsigned_long_long_int % 0x10 < 10 ? '0' + integer_destroyable.unsigned_long_long_int % 0x10 : 'A' + integer_destroyable.unsigned_long_long_int % 0x10 - 10);
					num_of_digits--;
				}
				break;
			default:
				break;
			}
		}
		else put_char_serial(*format);
		format++;
	}
}


// print value as hexadecimal
void print_byte_hex_serial(unsigned char value)
{
	char high = value >> 4 & 0x0f;
	char low = value & 0x0f;
	if(high < 10)put_char_serial(high + '0');
	else put_char_serial(high + 'a' - 10);
	if(low < 10)put_char_serial(low + '0');
	else put_char_serial(low + 'a' - 10);
}

// print value as hexadecimal
void print_dword_hex_serial(unsigned int value)
{
	print_word_hex_serial((unsigned short)(value >> 16));
	print_word_hex_serial((unsigned short)(value & 0x0000ffff));
}

// print string to serial port COM1
void print_serial(char const *string)
{
	while(*string)put_char_serial(*string++);
}

// print value as hexadecimal
void print_word_hex_serial(unsigned short value)
{
	print_byte_hex_serial((unsigned char)(value >> 8));
	print_byte_hex_serial((unsigned char)(value & 0x00ff));
}

// print a character to serial port COM1
void put_char_serial(char character)
{
	if(com1_flags & COM_AVAILABLE)
	{
		if(com1_flags & COM_INTERRUPT)
		{
			enqueue(com1_transmission_queue, &character);
			if(com1_flags & COM_WRITABLE)
			{
				com1_flags &= ~COM_WRITABLE;
				outb(COM1 + DATA_REGISTER, *(unsigned char *)dequeue(com1_transmission_queue));
			}
		}
		else
		{
			while(!(inb(COM1 + LINE_STATUS_REGISTER) & TRANSMITTER_HOLDING_REGISTER_EMPTY));
			outb(COM1, character);
		}
	}
}

