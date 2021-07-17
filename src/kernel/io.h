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

// set memory size to $0x0008,	$0x7bf4
// return value : memory size (MiB)
unsigned short set_memory_size(void);
