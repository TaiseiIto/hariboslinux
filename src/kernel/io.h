// disable all interrupts
void cli(void);

// get eflags register
unsigned int get_eflags(void);

// get nth arg in variadic arg function
// the first arg is 0th
unsigned int get_variadic_arg(unsigned int n);

// wait for next interrupt
void hlt(void);

// inb	$address,	%al
unsigned char inb(unsigned short address);

// inw	$address,	%ax
unsigned short inw(unsigned short address);

// inl	$address,	%eax
unsigned int inl(unsigned short address);

// outb	$value,	$address
void outb(unsigned short address, unsigned char value);

// outw	$value,	$address
void outw(unsigned short address, unsigned short value);

// outl	$value,	$address
void outl(unsigned short address, unsigned int value);

// movb	$segment:($address),%al
unsigned char readb(unsigned short segment, void *address);

// movw	$segment:($address),%ax
unsigned short readw(unsigned short segment, void *address);

// movl	$segment:($address),%eax
unsigned int readl(unsigned short segment, void *address);

// read size bytes from $source_segment:$source to $destination 
void reads(unsigned short source_segment, void *source, void *destination, unsigned int size);

// enable all interrupts
void sti(void);

// movb	$value,$segment:($address)
void writeb(unsigned short segment, void *address, unsigned char value);

// movw	$value,$segment:($address)
void writew(unsigned short segment, void *address, unsigned char value);

// movl	$value,$segment:($address)
void writel(unsigned short segment, void *address, unsigned char value);

// write size bytes from $source to $destination_segment:$destination
void writes(void *source, unsigned short destination_segment, void *destination, unsigned int size);

