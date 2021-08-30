# This program is called from loaddisk.s to initialise the screen

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

# bibliography
# https://wiki.osdev.org/VESA_Video_Modes

	.include	"global.s"

	.code16					# real mode
	.text
main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	pushw	%es
	pushw	%fs
	subw	$0x0014,%sp
	movw	%sp,	%di
						# 0x00(%di) callee arguments
						# 0x02(%di) callee arguments
						# 0x04(%di) callee arguments
						# 0x06(%di) callee arguments
						# 0x08(%di) callee arguments
						# 0x0a(%di) current checked video mode number pointer
						# 0x0c(%di) current checked video mode number
						# 0x0e(%di) current best video mode number
						# 0x10(%di) current best video mode width
						# 0x12(%di) current best video mode height
1:						# print hello message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:						# init serial port 0x03f8 (COM1)
	call	init_serial_port_com1
	movw	$hello_serial_message,(%di)
	call	print_serial
	call	new_line_serial
3:						# check VBE
	pushw	%di
	xorw	%ax,	%ax
	movw	%ax,	%es
	movw	$0x0500,%di
	movw	%di,	%si
	movw	$0x4f00,%ax
	int	$0x0010
	popw	%di
	cmpw	$0x004f,%ax
	je	4f
	movw	$vbe_unavailable,(%di)
	call	print_serial
	jmp	10f
4:						# check struct VbeInfoBlock
	movw	$vbe_available,(%di)
	call	print_serial
	movw	$vbe_signature,(%di)		# check VBE signature
	call	print_serial
	movb	%es:0x00(%si),%dl
	movb	%dl,	(%di)
	call	putchar_serial
	movb	%es:0x01(%si),%dl
	movb	%dl,	(%di)
	call	putchar_serial
	movb	%es:0x02(%si),%dl
	movb	%dl,	(%di)
	call	putchar_serial
	movb	%es:0x03(%si),%dl
	movb	%dl,	(%di)
	call	putchar_serial
	call	new_line_serial
	movw	$vbe_version,(%di)		# check VBE version
	call	print_serial
	movw	%es:0x04(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$vbe_oem_string_pointer,(%di)	# check VBE OEM string pointer
	call	print_serial
	movw	%es:0x06(%si),%dx
	movw	%dx,	(%di)
	movw	%es:0x08(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex_serial
	call	new_line_serial
	movw	$vbe_capabilities,(%di)		# check VBE capabilities
	call	print_serial
	movw	%es:0x0a(%si),%dx
	movw	%dx,	(%di)
	movw	%es:0x0c(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex_serial
	call	new_line_serial
	movw	$vbe_video_mode_pointer,(%di)	# check VBE video mode pointer
	call	print_serial
	movw	%es:0x0e(%si),%dx
	movw	%dx,	(%di)
	movw	%es:0x10(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex_serial
	call	new_line_serial
	movw	$vbe_total_memory,(%di)		# check VBE total memory
	call	print_serial
	movw	%es:0x12(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
5:						# check video modes
	xorw	%dx,	%dx
	movw	%dx,	0x0e(%di)		# init current best video mode number
	movw	%dx,	0x10(%di)		# init current best video mode width
	movw	%dx,	0x12(%di)		# init current best video mode height
	movw	%es:0x10(%si),%fs		# get VBE video mode pointer segment
	movw	%es:0x0e(%si),%si		# get VBE video mode pointer offset
6:
	movw	%fs:(%si),%cx
	cmp	$0xffff,%cx
	je	11f
	movw	%cx,	0x0c(%di)
	movw	$vbe_video_mode,(%di)		# check VBE video mode
	call	print_serial
	movw	0x0c(%di),%cx
	movw	%cx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$0x4f01,%ax			# get vbe_mode_info_structure
	movw	0x0c(%di),%cx
	movw	%si,	0x0a(%di)
	pushw	%di
	movw	$0x0600,%di
	movw	%di,	%si
	int	$0x0010
	popw	%di
	cmp	$0x004f,%ax
	jne	10f
7:						# print the video mode informations
	movw	$video_mode_check_attributes,(%di)# check vbe_mode_info_structure.attributes
	call	print_serial
	movw	%es:(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$video_mode_check_pitch,(%di)	# check vbe_mode_info_structure.putch
	call	print_serial
	movw	%es:0x10(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$video_mode_check_width,(%di)	# check vbe_mode_info_structure.width
	call	print_serial
	movw	%es:0x12(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$video_mode_check_height,(%di)	# check vbe_mode_info_structure.height
	call	print_serial
	movw	%es:0x14(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
	movw	$video_mode_check_bpp,(%di)	# check vbe_mode_info_structure.bpp
	call	print_serial
	movb	%es:0x19(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_memory_model,(%di)# check vbe_mode_info_structure.memory_model
	call	print_serial
	movb	%es:0x1b(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_red_mask,(%di)# check vbe_mode_info_structure.red_mask
	call	print_serial
	movb	%es:0x1f(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_red_position,(%di)# check vbe_mode_info_structure.red_position
	call	print_serial
	movb	%es:0x20(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_green_mask,(%di)# check vbe_mode_info_structure.green_mask
	call	print_serial
	movb	%es:0x21(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_green_position,(%di)# check vbe_mode_info_structure.green_position
	call	print_serial
	movb	%es:0x22(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_blue_mask,(%di)# check vbe_mode_info_structure.blue_mask
	call	print_serial
	movb	%es:0x23(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_blue_position,(%di)# check vbe_mode_info_structure.blue_position
	call	print_serial
	movb	%es:0x24(%si),%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
	movw	$video_mode_check_frame_buffer,(%di)# check vbe_mode_info_structure.framebuffer
	call	print_serial
	movw	%es:0x28(%si),%dx
	movw	%dx,	(%di)
	movw	%es:0x2a(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex_serial
	call	new_line_serial
	movw	$video_mode_check_off_screen_mem_off,(%di)# check vbe_mode_info_structure.off_screen_mem_off
	call	print_serial
	movw	%es:0x2c(%si),%dx
	movw	%dx,	(%di)
	movw	%es:0x2e(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex_serial
	call	new_line_serial
	movw	$video_mode_check_off_screen_mem_size,(%di)# check vbe_mode_info_structure.off_screen_mem_size
	call	print_serial
	movw	%es:0x30(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
8:						# check the video mode conditions
	movw	%es:(%si),%dx			# check linear frame buffer support (vbe_mode_info_structure.attributes & 0x0090)
	andw	$0x0090,%dx
	cmpw	$0x0090,%dx
	jne	10f
	cmpb	$0x06,%es:0x1b(%si)		# check direct color mode (vbe_mode_info_structure.memory_model == 0x06)
	jne	10f
	cmpb	$0x18,%es:0x19(%si)		# check bits per pixel (0x18 <= vbe_mode_info_structure.bpp)
	jb	10f
	movw	$video_mode_meets_conditions,(%di)
	call	print_serial
9:						# compare to current with video mode
	movw	0x10(%di),%dx
	cmpw	%dx,	%es:0x12(%si)		# current best video mode width < current checked video mode width
	jbe	10f
	movw	0x12(%di),%dx
	cmpw	%dx,	%es:0x14(%si)		# && current best video mode height < current checked video mode height
	jbe	10f
	movw	0x0c(%di),%dx			# update current best video mode
	movw	%dx,	0x0e(%di)
	movw	%es:0x12(%si),%dx
	movw	%dx,	0x10(%di)
	movw	%es:0x14(%si),%dx
	movw	%dx,	0x12(%di)
10:
	movw	0x0a(%di),%si
	addw	$0x0002,%si			# next video mode
	jmp	6b
11:						# check the best video mode
	call	new_line_serial
	movw	$the_best_video_mode,(%di)
	call	print_serial
	movw	0x0e(%di),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
12:						# write selected video mode informations
	movw	$0x4f01,%ax			# get vbe_mode_info_structure
	movw	0x0e(%di),%cx
	pushw	%di
	movw	$0x0600,%di
	int	$0x0010
	popw	%di
13:						# enter the best video mode
	movw	$0x4f02,%ax
	movw	0x0e(%di),%bx
	orw	$0x4000,%bx			# use linear frame buffer
	int	$0x0010
14:						# write BootInforamtion structure
						#
						# 0x0800 unsigned short memory_size;	// MiB
						# 0x0802 unsigned char keyboard_state;

	movw	$0x8800,%ax			# memory size
	int	$0x0015				# get extended memory size
	addw	$0x0480,%ax			# add first 0x480KiB memory
	shr	$0x000a,%ax			# convert KiB to MiB
	movw	$0x0800,%si
	movw	%ax,	(%si)
	movw	$0x0200,%ax			# keyboard_state
	int	$0x0016
	movb	%al,	0x02(%si)
15:						# check extended memroy size
	call	new_line_serial
	movw	(%si),%dx
	cmp	$0x0000,%dx
	jne	17f
16:						# memory size is bigger than or equals to 64MiB
	movw	$big_memory_message,(%di)
	call	print_serial
	movw	$0xe801,%ax
	int	$0x15
	addw	$0x0102,%dx
	shrw	$0x04,	%dx
	movw	%dx,	(%si)
17:						# print memory size
	movw	$extended_memory_size_message,(%di)
	call	print_serial
	movw	(%si),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	call	new_line_serial
18:						# check keyboard state
	movw	$keyboard_message,(%di)
	call	print_serial
	movb	0x02(%si),	%dl
	movb	%dl,	(%di)
	call	print_byte_hex_serial
	call	new_line_serial
19:						# free stack frame
	addw	$0x0014,%sp
	popw	%fs
	popw	%es
	popw	%di
	popw	%si
	leave
20:						# jump to mv2prtmd.bin
	jmp	mv2prtmd

init_serial_port_com1:		# void init_serial_port_com1(void)
0:
	pushw	%bp
	movw	%sp,	%bp
	xorb	%al,	%al	# disable all serial port interrupts
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outb	%al,	%dx
	movb	$0x80,	%al	# enable DLAB
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outb	%al,	%dx
	movb	$0x01,	%al	# low byte of baudrate
	movw	$com1,	%dx
	outb	%al,	%dx
	xorb	%al,	%al	# high byte of baudrate
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outb	%al,	%dx
	movb	$0x03,	%al	# 8bit per character
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outb	%al,	%dx
	leave
	ret

memcpy:				# void memcpy(unsigned short dest_seg, void *dest_addr, unsigned short src_seg, void *src_addr, unsigned short size);
0:
	push	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	pushw	%es
	pushw	%fs
	movw	%es,	0x04(%bp)	# dest_seg
	movw	%di,	0x06(%bp)	# dest_addr
	movw	%fs,	0x08(%bp)	# src_seg
	movw	%si,	0x0a(%bp)	# src_addr
	movw	%cx,	0x0c(%bp)	# size
1:
	jcxz	4f
	cmp	$0x0002,%cx
	jb	3f
2:
	movw	%fs:(%si),%dx
	movw	%dx,	%es:(%di)
	addw	$0x0002,%si
	addw	$0x0002,%di
	subw	$0x0002,%cx
	jmp	1b
3:
	movb	%fs:(%si),%dl
	movb	%dl,	%es:(%di)
4:
	popw	%fs
	popw	%es
	popw	%di
	popw	%si
	leave
	ret

memset:				# void memset(unsugned short segment, void *buf, unsigned char value, unsigned short size);	
0:
	push	%bp
	movw	%sp,	%bp
	pushw	%di
	pushw	%es
	movw	%es,	0x04(%bp)	# segment
	movw	%di,	0x06(%bp)	# buf
	movb	%dl,	0x08(%bp)	# value
	movb	%dh,	0x09(%bp)	# value
	movw	%cx,	0x0a(%bp)	# size
1:
	jcxz	4f
	cmpw	$0x0002,%cx
	jb	3f
2:				# set next 2 bytes
	movw	%dx,	%es:(%di)
	addw	$0x0002,%di
	subw	$0x0002,%cx
	jmp	1b
3:				# set a last byte
	movb	%dl,	%es:(%di)
4:
	popw	%es
	popw	%di
	leave
	ret

				# // print CRLF
new_line:			# void new_line(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000d,(%di)	# print CR
	call	putchar
	movw	$0x000a,(%di)	# print LF
	call	putchar
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print LF
new_line_serial:		# void new_line_serial(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000a,(%di)	# print LF
	call	putchar_serial
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print string to console
print:				# void print(char *string);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%si
1:				# put loop
	xorb	%ah,	%ah
	movb	(%si),	%al
	cmpb	$0x0a,	%al
	je	2f		# print CRLF
	cmpb	$0x00,	%al
	je	4f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar
	jmp	3f
2:				# print CRLF
	movw	$0x000d,(%di)
	call	putchar
	movw	$0x000a,(%di)
	call	putchar
3:
	incw	%si
	jmp	1b		# put next character
4:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print value as hexadecimal
print_byte_hex_serial:		# void print_byte_hex_serial(unsigned short value);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
	movw	$0x0001,%cx	# if %cx == 1, then print the digit of 0x10s place, else print the digit of 0x01s place.
	movw	0x04(%bp),%dx	# get the byte
	shrw	$0x0004,%dx
1:
	andw	$0x000f,%dx
	cmpw	$0x000a,%dx
	jge	3f
2:				# the digit is less than 0x0a
	addw	$0x0030,%dx
	jmp	4f
3:				# the digit is greater than or equal to 0x0a
	subw	$0x000a,%dx
	addw	$0x0061,%dx
4:				# print the digit
	movw	%cx,	0x02(%di)
	movw	%dx,	(%di)
	call	putchar_serial
	movw	0x02(%di),%cx
	jcxz	5f
	movw	0x04(%bp),%dx	# get the byte
	decw	%cx
	jmp	1b
5:				# finish printing
	addw	$0x0004,%sp
	popw	%di
	leave
	ret

print_dword_hex_serial:		# void print_dword_hex_serial(unsigned low, unsigned short high);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x06(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	movw	0x04(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print value as hexadecimal
print_word_hex_serial:		# void print_word_hex_serial(unsigned short value);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%dx
	movw	%dx,	0x02(%di)
	shr	$0x0008,%dx
	movw	%dx,	(%di)
	call	print_byte_hex_serial
	movw	0x02(%di),%dx
	andw	$0x00ff,%dx
	movw	%dx,	(%di)
	call	print_byte_hex_serial
	addw	$0x0004,%sp
	popw	%di
	leave
	ret

				# // print string to serial port COM1
print_serial:			# void print_serial(char *string);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%si
1:				# put loop
	xorb	%ah,	%ah
	movb	(%si),	%al
	cmpb	$0x00,	%al
	je	2f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar_serial
	incw	%si
	jmp	1b		# put next character
2:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print a character to console
putchar:			# void putchar(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	movb	0x04(%bp),%al
	movb	$0x0e,	%ah
	movw	$0x000f,%bx
	int	$0x10
	popw	%bx
	leave
	ret

				# // print a character to console
putchar_serial:			# void putchar_serial(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
1:				# wait for device
	movw	$com1,	%dx
	addw	$0x0005,%dx
	inb	%dx,	%al
	andb	$0x20,	%al
	jz	1b
2:				# send the character
	movb	0x04(%bp),%al
	movw	$com1,	%dx
	outb	%al,	%dx
3:				# free stack frame
	leave
	ret

	.data
big_memory_message:
	.string "memory size is bigger then or equals to 64MiB\n"
extended_memory_size_message:
	.string "extended memory size = 0x"
hello_message:
	.string	"Hello, initscrn.bin!\n"
hello_serial_message:
	.string "Hello, serial port COM1!\n"
keyboard_message:
	.string "keyboard state = 0x"
the_best_video_mode:
	.string "The best video mode = 0x"
vbe_available:
	.string "VBE available\n"
vbe_capabilities:
	.string "VBE capabilities = 0x"
vbe_oem_string_pointer:
	.string "VBE OEM string pointer = 0x"
vbe_signature:
	.string "VBE signature = 0x"
vbe_total_memory:
	.string "VBE total memory = 0x"
vbe_unavailable:
	.string "VBE unavailable\n"
vbe_version:
	.string "VBE version = 0x"
vbe_video_mode:
	.string "VBE video mode = 0x"
vbe_video_mode_pointer:
	.string "VBE video mode pointer = 0x"
video_mode_check_attributes:
	.string "\tattributes = 0x"
video_mode_check_bpp:
	.string "\tbits per pixel = 0x"
video_mode_check_blue_mask:
	.string "\tblue mask = 0x"
video_mode_check_blue_position:
	.string "\tblue position = 0x"
video_mode_check_frame_buffer:
	.string "\tframe buffer = 0x"
video_mode_check_green_mask:
	.string "\tgreen mask = 0x"
video_mode_check_green_position:
	.string "\tgreen position = 0x"
video_mode_check_height:
	.string "\theight = 0x"
video_mode_check_memory_model:
	.string "\tmemory model = 0x"
video_mode_check_off_screen_mem_off:
	.string "\toff screen mem off = 0x"
video_mode_check_off_screen_mem_size:
	.string "\toff screen mem size = 0x"
video_mode_check_pitch:
	.string "\tpitch = 0x"
video_mode_check_red_mask:
	.string "\tred mask = 0x"
video_mode_check_red_position:
	.string "\tred position = 0x"
video_mode_check_width:
	.string "\twidth = 0x"
video_mode_meets_conditions:
	.string "\tMEETS THE CONDITIONS!!!\n"
	.align 0x0200
mv2prtmd:

