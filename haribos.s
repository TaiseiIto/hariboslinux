# bibliography
# https://wiki.osdev.org/FAT

.text
.code16				# real mode
	jmp	hello_world
	nop
.ascii	"HARIBOTE"		# OEM identifier
.word	512			# The number of bytes per sector
.byte	1			# Number of sectors per cluster
.word	1			# Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
.byte	2			# Number of FATs
.word	224			# Number of root directory entries
.word	2880			# Number of sectors in the logical volume
.byte	0xf0			# Media dexcriptor type
.word	9			# Number of sectors per FAT
.word	18			# Number of sectors per track
.word	2			# Number of heads or sides on the storage media
.long	0			# Number of hidden sectors
.long	2880			# Number of sectors in the logical volume
.byte	0			# Drive number
.byte	0			# Reserved
.byte	0x29			# boot sector signature
.long	0xffffffff		# volume serial number
.ascii	"HARIBOTEOS "		# volume label
.ascii	"FAT12   "		# FAT file system type
.space 18
hello_world:
	movw	$0,	%ax
	movw	%ax,	%ss
	movw	$0x7c00,%sp
	movw	%ax,	%ds
	movw	%ax, 	%es
	movw	$msg,	%si
putloop:
	movb	(%si),	%al
	add	$1,	%si
	cmp	$0,	%al
	je	fin
	movb	$0x0e,	%ah
	movw	$15,	%bx
	int	$0x10
	jmp	putloop
fin:
	hlt
	jmp	fin
msg:
.string	"\n\nHello, World\n"

