OUTPUT_FORMAT("binary");

IPL_BASE = 0x00007c00;	/* image fileの先頭sectorがこのmemory番地に読み込まれるため，memory address offsetを設定する */

SECTIONS {
	. = IPL_BASE;
}

