OUTPUT_FORMAT("binary");

IPL_BASE = 0x00007c00;	/* memory address offset */

SECTIONS {
	. = IPL_BASE;
	.text :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
	}
}

