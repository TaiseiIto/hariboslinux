# bibliography
# https://wiki.osdev.org/FAT

	.text
	.code16			# real mode
	.set	sector_size,	512
	.set	cluster_size,	1
	.set	reserved_clusters, 1
	.set	fats,		2
	.set	files,		224
	.set	sectors,	2880
	.set	media_type,	0xf0
	.set	fat_size,	9
	.set	track_size,	18
	.set	heads,		2
	.set	hidden_sectors,	0
	.set	large_sectors,	2880
	.set	drive_number,	0
	.set	boot_signature,	0x29
	.set	volume_serial,	0xffffffff
stack:				# bottom of stack
	jmp	hello_world
	nop
	.ascii	"HARIBOTE"	# OEM identifier
	.word	sector_size	# The number of bytes per sector
	.byte	cluster_size	# Number of sectors per cluster
	.word	reserved_clusters # Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
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
	.string	"Hello, World!\r\n"

