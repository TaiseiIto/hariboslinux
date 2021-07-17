// wait for next interrupt
void io_hlt(void);

// inb	$address,	%al
unsigned char io_inb(unsigned short address);

// inw	$address,	%ax
unsigned short io_inw(unsigned short address);

// inl	$address,	%eax
unsigned int io_inl(unsigned short address);

// outb	$value,	$address
void io_outb(unsigned short address, unsigned char value);

// outw	$value,	$address
void io_outw(unsigned short address, unsigned short value);

// outl	$value,	$address
void io_outl(unsigned short address, unsigned int value);

// movb	$value,$segment:($address)
void io_writeb(unsigned short segment, void *address, unsigned char value);

