OUTPUT_FORMAT("binary");

BASE = 0x00007c00;

SECTIONS
{
            . = BASE;
            .text :
            {
                       test_os.o(.text)
            }
            . = BASE;
            . += 0x00000200;
            . -= 0x00000002;
            .boot_sector_sign :
            {
                       BYTE(0x55);
                       BYTE(0xaa);
            }
            /DISCARD/ :
            {
                       *(.eh_frame)
                       *(.note.gnu.property)
            }
}

