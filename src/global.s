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
	.set	file_entry_size,0x00000020	# file entry size (bytes)
	.set	load_disk,	0x4200		# address of loaddisk.bin on the floppy disk
						# (boot_sectors + fats * fat_size) * sector_size + files * file_entry_size
	.set	load_dest,	0x7c00	# memory address the disk loaded to
	.set	com1,		0x03f8		# serial port COM1
	.set	last_loaded_cylinder,	0x00000803
	.set	last_loaded_head,	0x00000804
	.set	last_loaded_sector,	0x00000805

