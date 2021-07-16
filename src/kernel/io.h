// wait for next interrupt
void io_hlt(void);

// inb	$address,	%al
unsigned char io_inb(unsigned short address);

// inw	$address,	%ax
unsigned short io_inw(unsigned short address);

// inl	$address,	%eax
unsigned int io_inl(unsigned short address);

// outb	%al,	$address
void io_outb(unsigned short address, unsigned char value);

// outw	%ax,	$address
void io_outw(unsigned short address, unsigned short value);

// outl	%eax,	$address
void io_outl(unsigned short address, unsigned int value);

// print LF
void new_line_serial(void);

// print string to serial port COM1
void print_serial(char *string);

// print a character to serial port COM1
void putchar_serial(char character);

