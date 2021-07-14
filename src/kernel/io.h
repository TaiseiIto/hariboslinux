// wait for next interrupt
void __attribute__((stdcall)) io_hlt(void);

// print LF
void __attribute__((stdcall)) new_line_serial(void);

// print string to serial port COM1
void __attribute__((stdcall)) print_serial(char *string);

// print a character to serial port COM1
void __attribute__((stdcall)) putchar_serial(char character);

