# bibliography
# https://wiki.osdev.org/FAT

	.code16					# real mode
	.set	sector_size,	0x0200		# The number of bytes per sector
	.set	cluster_size,	0x01		# Number of sectors per cluster
	.set	boot_clusters,	0x0001		# Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
	.set	fats,		0x02		# Number of FATs
	.set	files,		0x00e0		# Number of root directory entries
	.set	sectors,	0x0b40		# Number of sectors in the logical volume
	.set	media_type,	0xf0		# Media dexcriptor type
	.set	fat_size,	0x0009		# Number of sectors per FAT
	.set	track_size,	0x0012		# Number of sectors per track
	.set	heads,		0x0002		# Number of heads or sides on the storage media
	.set	hidden_sectors,	0x00000000	# Number of hidden sectors
	.set	large_sectors,	0x00000b40	# Number of sectors in the logical volume
	.set	drive_number,	0x00		# Drive number
	.set	boot_signature,	0x29		# boot sector signature
	.set	volume_serial,	0xffffffff	# volume serial number

	.text
stack:						# bottom of stack
	jmp	hello_world
	nop
	.ascii	"HARIBOTE"	# OEM identifier
	.word	sector_size	# The number of bytes per sector
	.byte	cluster_size	# Number of sectors per cluster
	.word	boot_clusters	# Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
	.byte	fats		# Number of FATs
	.word	files		# Number of root directory entries
	.word	sectors		# Number of sectors in the logical volume
	.byte	media_type	# Media dexcriptor type
	.word	fat_size	# Number of sectors per FAT
	.word	track_size	# Number of sectors per track
	.word	heads		# Number of heads or sides on the storage media
	.long	hidden_sectors	# Number of hidden sectors
	.long	large_sectors	# Number of sectors in the logical volume
	.byte	drive_number	# Drive number
	.byte	0		# Reserved
	.byte	boot_signature	# boot sector signature
	.long	volume_serial	# volume serial number
	.ascii	"HARIBOTEOS "	# volume label
	.ascii	"FAT12   "	# FAT file system type
hello_world:
	movw	$0,	%ax
	movw	%ax,	%ss
	movw	$stack,	%sp
	movw	%ax,	%ds
	movw	%ax, 	%es
	movw	$msg,	%si
putloop:
	movb	(%si),	%al
	inc	%si
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
	.string	"Hello, World!\r\n"

