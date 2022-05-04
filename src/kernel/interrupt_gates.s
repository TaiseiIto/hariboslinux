# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.extern	alignment_check_exception_handler
	.extern	bound_range_exceeded_exception_handler
	.extern	breakpoint_exception_handler
	.extern cli_task_interrupt
	.extern com1_interrupt_handler
	.extern com2_interrupt_handler
	.extern	coprocessor_segment_overrun_exception_handler
	.extern	debug_exception_handler
	.extern	device_not_available_exception_handler
	.extern	devide_by_zero_exception_handler
	.extern disk_interrupt_handler
	.extern	double_fault_exception_handler
	.extern	fpu_error_exception_handler
	.extern	general_protection_fault_exception_handler
	.extern interrupt_handler0x30
	.extern interrupt_handler0x31
	.extern interrupt_handler0x32
	.extern interrupt_handler0x33
	.extern interrupt_handler0x34
	.extern interrupt_handler0x35
	.extern interrupt_handler0x36
	.extern interrupt_handler0x37
	.extern interrupt_handler0x38
	.extern interrupt_handler0x39
	.extern interrupt_handler0x3a
	.extern interrupt_handler0x3b
	.extern interrupt_handler0x3c
	.extern interrupt_handler0x3d
	.extern interrupt_handler0x3e
	.extern interrupt_handler0x3f
	.extern interrupt_handler0x40
	.extern interrupt_handler0x41
	.extern interrupt_handler0x42
	.extern interrupt_handler0x43
	.extern interrupt_handler0x44
	.extern interrupt_handler0x45
	.extern interrupt_handler0x46
	.extern interrupt_handler0x47
	.extern interrupt_handler0x48
	.extern interrupt_handler0x49
	.extern interrupt_handler0x4a
	.extern interrupt_handler0x4b
	.extern interrupt_handler0x4c
	.extern interrupt_handler0x4d
	.extern interrupt_handler0x4e
	.extern interrupt_handler0x4f
	.extern interrupt_handler0x50
	.extern interrupt_handler0x51
	.extern interrupt_handler0x52
	.extern interrupt_handler0x53
	.extern interrupt_handler0x54
	.extern interrupt_handler0x55
	.extern interrupt_handler0x56
	.extern interrupt_handler0x57
	.extern interrupt_handler0x58
	.extern interrupt_handler0x59
	.extern interrupt_handler0x5a
	.extern interrupt_handler0x5b
	.extern interrupt_handler0x5c
	.extern interrupt_handler0x5d
	.extern interrupt_handler0x5e
	.extern interrupt_handler0x5f
	.extern interrupt_handler0x60
	.extern interrupt_handler0x61
	.extern interrupt_handler0x62
	.extern interrupt_handler0x63
	.extern interrupt_handler0x64
	.extern interrupt_handler0x65
	.extern interrupt_handler0x66
	.extern interrupt_handler0x67
	.extern interrupt_handler0x68
	.extern interrupt_handler0x69
	.extern interrupt_handler0x6a
	.extern interrupt_handler0x6b
	.extern interrupt_handler0x6c
	.extern interrupt_handler0x6d
	.extern interrupt_handler0x6e
	.extern interrupt_handler0x6f
	.extern interrupt_handler0x70
	.extern interrupt_handler0x71
	.extern interrupt_handler0x72
	.extern interrupt_handler0x73
	.extern interrupt_handler0x74
	.extern interrupt_handler0x75
	.extern interrupt_handler0x76
	.extern interrupt_handler0x77
	.extern interrupt_handler0x78
	.extern interrupt_handler0x79
	.extern interrupt_handler0x7a
	.extern interrupt_handler0x7b
	.extern interrupt_handler0x7c
	.extern interrupt_handler0x7d
	.extern interrupt_handler0x7e
	.extern interrupt_handler0x7f
	.extern interrupt_handler0x81
	.extern interrupt_handler0x82
	.extern interrupt_handler0x83
	.extern interrupt_handler0x84
	.extern interrupt_handler0x85
	.extern interrupt_handler0x86
	.extern interrupt_handler0x87
	.extern interrupt_handler0x88
	.extern interrupt_handler0x89
	.extern interrupt_handler0x8a
	.extern interrupt_handler0x8b
	.extern interrupt_handler0x8c
	.extern interrupt_handler0x8d
	.extern interrupt_handler0x8e
	.extern interrupt_handler0x8f
	.extern interrupt_handler0x90
	.extern interrupt_handler0x91
	.extern interrupt_handler0x92
	.extern interrupt_handler0x93
	.extern interrupt_handler0x94
	.extern interrupt_handler0x95
	.extern interrupt_handler0x96
	.extern interrupt_handler0x97
	.extern interrupt_handler0x98
	.extern interrupt_handler0x99
	.extern interrupt_handler0x9a
	.extern interrupt_handler0x9b
	.extern interrupt_handler0x9c
	.extern interrupt_handler0x9d
	.extern interrupt_handler0x9e
	.extern interrupt_handler0x9f
	.extern interrupt_handler0xa0
	.extern interrupt_handler0xa1
	.extern interrupt_handler0xa2
	.extern interrupt_handler0xa3
	.extern interrupt_handler0xa4
	.extern interrupt_handler0xa5
	.extern interrupt_handler0xa6
	.extern interrupt_handler0xa7
	.extern interrupt_handler0xa8
	.extern interrupt_handler0xa9
	.extern interrupt_handler0xaa
	.extern interrupt_handler0xab
	.extern interrupt_handler0xac
	.extern interrupt_handler0xad
	.extern interrupt_handler0xae
	.extern interrupt_handler0xaf
	.extern interrupt_handler0xb0
	.extern interrupt_handler0xb1
	.extern interrupt_handler0xb2
	.extern interrupt_handler0xb3
	.extern interrupt_handler0xb4
	.extern interrupt_handler0xb5
	.extern interrupt_handler0xb6
	.extern interrupt_handler0xb7
	.extern interrupt_handler0xb8
	.extern interrupt_handler0xb9
	.extern interrupt_handler0xba
	.extern interrupt_handler0xbb
	.extern interrupt_handler0xbc
	.extern interrupt_handler0xbd
	.extern interrupt_handler0xbe
	.extern interrupt_handler0xbf
	.extern interrupt_handler0xc0
	.extern interrupt_handler0xc1
	.extern interrupt_handler0xc2
	.extern interrupt_handler0xc3
	.extern interrupt_handler0xc4
	.extern interrupt_handler0xc5
	.extern interrupt_handler0xc6
	.extern interrupt_handler0xc7
	.extern interrupt_handler0xc8
	.extern interrupt_handler0xc9
	.extern interrupt_handler0xca
	.extern interrupt_handler0xcb
	.extern interrupt_handler0xcc
	.extern interrupt_handler0xcd
	.extern interrupt_handler0xce
	.extern interrupt_handler0xcf
	.extern interrupt_handler0xd0
	.extern interrupt_handler0xd1
	.extern interrupt_handler0xd2
	.extern interrupt_handler0xd3
	.extern interrupt_handler0xd4
	.extern interrupt_handler0xd5
	.extern interrupt_handler0xd6
	.extern interrupt_handler0xd7
	.extern interrupt_handler0xd8
	.extern interrupt_handler0xd9
	.extern interrupt_handler0xda
	.extern interrupt_handler0xdb
	.extern interrupt_handler0xdc
	.extern interrupt_handler0xdd
	.extern interrupt_handler0xde
	.extern interrupt_handler0xdf
	.extern interrupt_handler0xe0
	.extern interrupt_handler0xe1
	.extern interrupt_handler0xe2
	.extern interrupt_handler0xe3
	.extern interrupt_handler0xe4
	.extern interrupt_handler0xe5
	.extern interrupt_handler0xe6
	.extern interrupt_handler0xe7
	.extern interrupt_handler0xe8
	.extern interrupt_handler0xe9
	.extern interrupt_handler0xea
	.extern interrupt_handler0xeb
	.extern interrupt_handler0xec
	.extern interrupt_handler0xed
	.extern interrupt_handler0xee
	.extern interrupt_handler0xef
	.extern interrupt_handler0xf0
	.extern interrupt_handler0xf1
	.extern interrupt_handler0xf2
	.extern interrupt_handler0xf3
	.extern interrupt_handler0xf4
	.extern interrupt_handler0xf5
	.extern interrupt_handler0xf6
	.extern interrupt_handler0xf7
	.extern interrupt_handler0xf8
	.extern interrupt_handler0xf9
	.extern interrupt_handler0xfa
	.extern interrupt_handler0xfb
	.extern interrupt_handler0xfc
	.extern interrupt_handler0xfd
	.extern interrupt_handler0xfe
	.extern interrupt_handler0xff
	.extern	invalid_opcode_exception_handler
	.extern	invalid_TSS_exception_handler
	.extern	keyboard_interrupt_handler
	.extern lpt1_interrupt_handler
	.extern lpt2_interrupt_handler
	.extern	machine_check_exception_handler
	.extern mouse_interrupt_handler
	.extern	non_maskable_interrupt_handler
	.extern	overflow_exception_handler
	.extern	page_fault_exeption_handler
	.extern peripheral0_interrupt_handler
	.extern peripheral1_interrupt_handler
	.extern peripheral2_interrupt_handler
	.extern pit_interrupt_handler
	.extern primary_ATA_hard_disk_interrupt_handler
	.extern rtc_interrupt_handler
	.extern reserved_exception_handler0x00
	.extern reserved_exception_handler0x01
	.extern reserved_exception_handler0x02
	.extern reserved_exception_handler0x03
	.extern reserved_exception_handler0x04
	.extern reserved_exception_handler0x05
	.extern reserved_exception_handler0x06
	.extern reserved_exception_handler0x07
	.extern reserved_exception_handler0x08
	.extern reserved_exception_handler0x09
	.extern reserved_exception_handler0x0a
	.extern secondary_ATA_hard_disk_interrupt_handler
	.extern	security_exception_handler
	.extern	segment_not_present_exception_handler
	.extern	simd_floating_point_exception_handler
	.extern slave_pic_interrupt_handler
	.extern	stack_segment_fault_exception_handler
	.extern sti_task_interrupt
	.extern system_call
	.extern	virtualization_exception_handler
	.extern	x87_floating_point_exception_handler

	.globl	interrupt_gate0x00
	.globl	interrupt_gate0x01
	.globl	interrupt_gate0x02
	.globl	interrupt_gate0x03
	.globl	interrupt_gate0x04
	.globl	interrupt_gate0x05
	.globl	interrupt_gate0x06
	.globl	interrupt_gate0x07
	.globl	interrupt_gate0x08
	.globl	interrupt_gate0x09
	.globl	interrupt_gate0x0a
	.globl	interrupt_gate0x0b
	.globl	interrupt_gate0x0c
	.globl	interrupt_gate0x0d
	.globl	interrupt_gate0x0e
	.globl	interrupt_gate0x0f
	.globl	interrupt_gate0x10
	.globl	interrupt_gate0x11
	.globl	interrupt_gate0x12
	.globl	interrupt_gate0x13
	.globl	interrupt_gate0x14
	.globl	interrupt_gate0x15
	.globl	interrupt_gate0x16
	.globl	interrupt_gate0x17
	.globl	interrupt_gate0x18
	.globl	interrupt_gate0x19
	.globl	interrupt_gate0x1a
	.globl	interrupt_gate0x1b
	.globl	interrupt_gate0x1c
	.globl	interrupt_gate0x1d
	.globl	interrupt_gate0x1e
	.globl	interrupt_gate0x1f
	.globl	interrupt_gate0x20
	.globl	interrupt_gate0x21
	.globl	interrupt_gate0x22
	.globl	interrupt_gate0x23
	.globl	interrupt_gate0x24
	.globl	interrupt_gate0x25
	.globl	interrupt_gate0x26
	.globl	interrupt_gate0x27
	.globl	interrupt_gate0x28
	.globl	interrupt_gate0x29
	.globl	interrupt_gate0x2a
	.globl	interrupt_gate0x2b
	.globl	interrupt_gate0x2c
	.globl	interrupt_gate0x2d
	.globl	interrupt_gate0x2e
	.globl	interrupt_gate0x2f
	.globl	interrupt_gate0x30
	.globl	interrupt_gate0x31
	.globl	interrupt_gate0x32
	.globl	interrupt_gate0x33
	.globl	interrupt_gate0x34
	.globl	interrupt_gate0x35
	.globl	interrupt_gate0x36
	.globl	interrupt_gate0x37
	.globl	interrupt_gate0x38
	.globl	interrupt_gate0x39
	.globl	interrupt_gate0x3a
	.globl	interrupt_gate0x3b
	.globl	interrupt_gate0x3c
	.globl	interrupt_gate0x3d
	.globl	interrupt_gate0x3e
	.globl	interrupt_gate0x3f
	.globl	interrupt_gate0x40
	.globl	interrupt_gate0x41
	.globl	interrupt_gate0x42
	.globl	interrupt_gate0x43
	.globl	interrupt_gate0x44
	.globl	interrupt_gate0x45
	.globl	interrupt_gate0x46
	.globl	interrupt_gate0x47
	.globl	interrupt_gate0x48
	.globl	interrupt_gate0x49
	.globl	interrupt_gate0x4a
	.globl	interrupt_gate0x4b
	.globl	interrupt_gate0x4c
	.globl	interrupt_gate0x4d
	.globl	interrupt_gate0x4e
	.globl	interrupt_gate0x4f
	.globl	interrupt_gate0x50
	.globl	interrupt_gate0x51
	.globl	interrupt_gate0x52
	.globl	interrupt_gate0x53
	.globl	interrupt_gate0x54
	.globl	interrupt_gate0x55
	.globl	interrupt_gate0x56
	.globl	interrupt_gate0x57
	.globl	interrupt_gate0x58
	.globl	interrupt_gate0x59
	.globl	interrupt_gate0x5a
	.globl	interrupt_gate0x5b
	.globl	interrupt_gate0x5c
	.globl	interrupt_gate0x5d
	.globl	interrupt_gate0x5e
	.globl	interrupt_gate0x5f
	.globl	interrupt_gate0x60
	.globl	interrupt_gate0x61
	.globl	interrupt_gate0x62
	.globl	interrupt_gate0x63
	.globl	interrupt_gate0x64
	.globl	interrupt_gate0x65
	.globl	interrupt_gate0x66
	.globl	interrupt_gate0x67
	.globl	interrupt_gate0x68
	.globl	interrupt_gate0x69
	.globl	interrupt_gate0x6a
	.globl	interrupt_gate0x6b
	.globl	interrupt_gate0x6c
	.globl	interrupt_gate0x6d
	.globl	interrupt_gate0x6e
	.globl	interrupt_gate0x6f
	.globl	interrupt_gate0x70
	.globl	interrupt_gate0x71
	.globl	interrupt_gate0x72
	.globl	interrupt_gate0x73
	.globl	interrupt_gate0x74
	.globl	interrupt_gate0x75
	.globl	interrupt_gate0x76
	.globl	interrupt_gate0x77
	.globl	interrupt_gate0x78
	.globl	interrupt_gate0x79
	.globl	interrupt_gate0x7a
	.globl	interrupt_gate0x7b
	.globl	interrupt_gate0x7c
	.globl	interrupt_gate0x7d
	.globl	interrupt_gate0x7e
	.globl	interrupt_gate0x7f
	.globl	interrupt_gate0x80
	.globl	interrupt_gate0x81
	.globl	interrupt_gate0x82
	.globl	interrupt_gate0x83
	.globl	interrupt_gate0x84
	.globl	interrupt_gate0x85
	.globl	interrupt_gate0x86
	.globl	interrupt_gate0x87
	.globl	interrupt_gate0x88
	.globl	interrupt_gate0x89
	.globl	interrupt_gate0x8a
	.globl	interrupt_gate0x8b
	.globl	interrupt_gate0x8c
	.globl	interrupt_gate0x8d
	.globl	interrupt_gate0x8e
	.globl	interrupt_gate0x8f
	.globl	interrupt_gate0x90
	.globl	interrupt_gate0x91
	.globl	interrupt_gate0x92
	.globl	interrupt_gate0x93
	.globl	interrupt_gate0x94
	.globl	interrupt_gate0x95
	.globl	interrupt_gate0x96
	.globl	interrupt_gate0x97
	.globl	interrupt_gate0x98
	.globl	interrupt_gate0x99
	.globl	interrupt_gate0x9a
	.globl	interrupt_gate0x9b
	.globl	interrupt_gate0x9c
	.globl	interrupt_gate0x9d
	.globl	interrupt_gate0x9e
	.globl	interrupt_gate0x9f
	.globl	interrupt_gate0xa0
	.globl	interrupt_gate0xa1
	.globl	interrupt_gate0xa2
	.globl	interrupt_gate0xa3
	.globl	interrupt_gate0xa4
	.globl	interrupt_gate0xa5
	.globl	interrupt_gate0xa6
	.globl	interrupt_gate0xa7
	.globl	interrupt_gate0xa8
	.globl	interrupt_gate0xa9
	.globl	interrupt_gate0xaa
	.globl	interrupt_gate0xab
	.globl	interrupt_gate0xac
	.globl	interrupt_gate0xad
	.globl	interrupt_gate0xae
	.globl	interrupt_gate0xaf
	.globl	interrupt_gate0xb0
	.globl	interrupt_gate0xb1
	.globl	interrupt_gate0xb2
	.globl	interrupt_gate0xb3
	.globl	interrupt_gate0xb4
	.globl	interrupt_gate0xb5
	.globl	interrupt_gate0xb6
	.globl	interrupt_gate0xb7
	.globl	interrupt_gate0xb8
	.globl	interrupt_gate0xb9
	.globl	interrupt_gate0xba
	.globl	interrupt_gate0xbb
	.globl	interrupt_gate0xbc
	.globl	interrupt_gate0xbd
	.globl	interrupt_gate0xbe
	.globl	interrupt_gate0xbf
	.globl	interrupt_gate0xc0
	.globl	interrupt_gate0xc1
	.globl	interrupt_gate0xc2
	.globl	interrupt_gate0xc3
	.globl	interrupt_gate0xc4
	.globl	interrupt_gate0xc5
	.globl	interrupt_gate0xc6
	.globl	interrupt_gate0xc7
	.globl	interrupt_gate0xc8
	.globl	interrupt_gate0xc9
	.globl	interrupt_gate0xca
	.globl	interrupt_gate0xcb
	.globl	interrupt_gate0xcc
	.globl	interrupt_gate0xcd
	.globl	interrupt_gate0xce
	.globl	interrupt_gate0xcf
	.globl	interrupt_gate0xd0
	.globl	interrupt_gate0xd1
	.globl	interrupt_gate0xd2
	.globl	interrupt_gate0xd3
	.globl	interrupt_gate0xd4
	.globl	interrupt_gate0xd5
	.globl	interrupt_gate0xd6
	.globl	interrupt_gate0xd7
	.globl	interrupt_gate0xd8
	.globl	interrupt_gate0xd9
	.globl	interrupt_gate0xda
	.globl	interrupt_gate0xdb
	.globl	interrupt_gate0xdc
	.globl	interrupt_gate0xdd
	.globl	interrupt_gate0xde
	.globl	interrupt_gate0xdf
	.globl	interrupt_gate0xe0
	.globl	interrupt_gate0xe1
	.globl	interrupt_gate0xe2
	.globl	interrupt_gate0xe3
	.globl	interrupt_gate0xe4
	.globl	interrupt_gate0xe5
	.globl	interrupt_gate0xe6
	.globl	interrupt_gate0xe7
	.globl	interrupt_gate0xe8
	.globl	interrupt_gate0xe9
	.globl	interrupt_gate0xea
	.globl	interrupt_gate0xeb
	.globl	interrupt_gate0xec
	.globl	interrupt_gate0xed
	.globl	interrupt_gate0xee
	.globl	interrupt_gate0xef
	.globl	interrupt_gate0xf0
	.globl	interrupt_gate0xf1
	.globl	interrupt_gate0xf2
	.globl	interrupt_gate0xf3
	.globl	interrupt_gate0xf4
	.globl	interrupt_gate0xf5
	.globl	interrupt_gate0xf6
	.globl	interrupt_gate0xf7
	.globl	interrupt_gate0xf8
	.globl	interrupt_gate0xf9
	.globl	interrupt_gate0xfa
	.globl	interrupt_gate0xfb
	.globl	interrupt_gate0xfc
	.globl	interrupt_gate0xfd
	.globl	interrupt_gate0xfe
	.globl	interrupt_gate0xff

	.set	kernel_data_segment_selector,0x0008

	.type	interrupt_gate0x00,	@function
	.type	interrupt_gate0x01,	@function
	.type	interrupt_gate0x02,	@function
	.type	interrupt_gate0x03,	@function
	.type	interrupt_gate0x04,	@function
	.type	interrupt_gate0x05,	@function
	.type	interrupt_gate0x06,	@function
	.type	interrupt_gate0x07,	@function
	.type	interrupt_gate0x08,	@function
	.type	interrupt_gate0x09,	@function
	.type	interrupt_gate0x0a,	@function
	.type	interrupt_gate0x0b,	@function
	.type	interrupt_gate0x0c,	@function
	.type	interrupt_gate0x0d,	@function
	.type	interrupt_gate0x0e,	@function
	.type	interrupt_gate0x0f,	@function
	.type	interrupt_gate0x10,	@function
	.type	interrupt_gate0x11,	@function
	.type	interrupt_gate0x12,	@function
	.type	interrupt_gate0x13,	@function
	.type	interrupt_gate0x14,	@function
	.type	interrupt_gate0x15,	@function
	.type	interrupt_gate0x16,	@function
	.type	interrupt_gate0x17,	@function
	.type	interrupt_gate0x18,	@function
	.type	interrupt_gate0x19,	@function
	.type	interrupt_gate0x1a,	@function
	.type	interrupt_gate0x1b,	@function
	.type	interrupt_gate0x1c,	@function
	.type	interrupt_gate0x1d,	@function
	.type	interrupt_gate0x1e,	@function
	.type	interrupt_gate0x1f,	@function
	.type	interrupt_gate0x20,	@function
	.type	interrupt_gate0x21,	@function
	.type	interrupt_gate0x22,	@function
	.type	interrupt_gate0x23,	@function
	.type	interrupt_gate0x24,	@function
	.type	interrupt_gate0x25,	@function
	.type	interrupt_gate0x26,	@function
	.type	interrupt_gate0x27,	@function
	.type	interrupt_gate0x28,	@function
	.type	interrupt_gate0x29,	@function
	.type	interrupt_gate0x2a,	@function
	.type	interrupt_gate0x2b,	@function
	.type	interrupt_gate0x2c,	@function
	.type	interrupt_gate0x2d,	@function
	.type	interrupt_gate0x2e,	@function
	.type	interrupt_gate0x2f,	@function
	.type	interrupt_gate0x30,	@function
	.type	interrupt_gate0x31,	@function
	.type	interrupt_gate0x32,	@function
	.type	interrupt_gate0x33,	@function
	.type	interrupt_gate0x34,	@function
	.type	interrupt_gate0x35,	@function
	.type	interrupt_gate0x36,	@function
	.type	interrupt_gate0x37,	@function
	.type	interrupt_gate0x38,	@function
	.type	interrupt_gate0x39,	@function
	.type	interrupt_gate0x3a,	@function
	.type	interrupt_gate0x3b,	@function
	.type	interrupt_gate0x3c,	@function
	.type	interrupt_gate0x3d,	@function
	.type	interrupt_gate0x3e,	@function
	.type	interrupt_gate0x3f,	@function
	.type	interrupt_gate0x40,	@function
	.type	interrupt_gate0x41,	@function
	.type	interrupt_gate0x42,	@function
	.type	interrupt_gate0x43,	@function
	.type	interrupt_gate0x44,	@function
	.type	interrupt_gate0x45,	@function
	.type	interrupt_gate0x46,	@function
	.type	interrupt_gate0x47,	@function
	.type	interrupt_gate0x48,	@function
	.type	interrupt_gate0x49,	@function
	.type	interrupt_gate0x4a,	@function
	.type	interrupt_gate0x4b,	@function
	.type	interrupt_gate0x4c,	@function
	.type	interrupt_gate0x4d,	@function
	.type	interrupt_gate0x4e,	@function
	.type	interrupt_gate0x4f,	@function
	.type	interrupt_gate0x50,	@function
	.type	interrupt_gate0x51,	@function
	.type	interrupt_gate0x52,	@function
	.type	interrupt_gate0x53,	@function
	.type	interrupt_gate0x54,	@function
	.type	interrupt_gate0x55,	@function
	.type	interrupt_gate0x56,	@function
	.type	interrupt_gate0x57,	@function
	.type	interrupt_gate0x58,	@function
	.type	interrupt_gate0x59,	@function
	.type	interrupt_gate0x5a,	@function
	.type	interrupt_gate0x5b,	@function
	.type	interrupt_gate0x5c,	@function
	.type	interrupt_gate0x5d,	@function
	.type	interrupt_gate0x5e,	@function
	.type	interrupt_gate0x5f,	@function
	.type	interrupt_gate0x60,	@function
	.type	interrupt_gate0x61,	@function
	.type	interrupt_gate0x62,	@function
	.type	interrupt_gate0x63,	@function
	.type	interrupt_gate0x64,	@function
	.type	interrupt_gate0x65,	@function
	.type	interrupt_gate0x66,	@function
	.type	interrupt_gate0x67,	@function
	.type	interrupt_gate0x68,	@function
	.type	interrupt_gate0x69,	@function
	.type	interrupt_gate0x6a,	@function
	.type	interrupt_gate0x6b,	@function
	.type	interrupt_gate0x6c,	@function
	.type	interrupt_gate0x6d,	@function
	.type	interrupt_gate0x6e,	@function
	.type	interrupt_gate0x6f,	@function
	.type	interrupt_gate0x70,	@function
	.type	interrupt_gate0x71,	@function
	.type	interrupt_gate0x72,	@function
	.type	interrupt_gate0x73,	@function
	.type	interrupt_gate0x74,	@function
	.type	interrupt_gate0x75,	@function
	.type	interrupt_gate0x76,	@function
	.type	interrupt_gate0x77,	@function
	.type	interrupt_gate0x78,	@function
	.type	interrupt_gate0x79,	@function
	.type	interrupt_gate0x7a,	@function
	.type	interrupt_gate0x7b,	@function
	.type	interrupt_gate0x7c,	@function
	.type	interrupt_gate0x7d,	@function
	.type	interrupt_gate0x7e,	@function
	.type	interrupt_gate0x7f,	@function
	.type	interrupt_gate0x80,	@function
	.type	interrupt_gate0x81,	@function
	.type	interrupt_gate0x82,	@function
	.type	interrupt_gate0x83,	@function
	.type	interrupt_gate0x84,	@function
	.type	interrupt_gate0x85,	@function
	.type	interrupt_gate0x86,	@function
	.type	interrupt_gate0x87,	@function
	.type	interrupt_gate0x88,	@function
	.type	interrupt_gate0x89,	@function
	.type	interrupt_gate0x8a,	@function
	.type	interrupt_gate0x8b,	@function
	.type	interrupt_gate0x8c,	@function
	.type	interrupt_gate0x8d,	@function
	.type	interrupt_gate0x8e,	@function
	.type	interrupt_gate0x8f,	@function
	.type	interrupt_gate0x90,	@function
	.type	interrupt_gate0x91,	@function
	.type	interrupt_gate0x92,	@function
	.type	interrupt_gate0x93,	@function
	.type	interrupt_gate0x94,	@function
	.type	interrupt_gate0x95,	@function
	.type	interrupt_gate0x96,	@function
	.type	interrupt_gate0x97,	@function
	.type	interrupt_gate0x98,	@function
	.type	interrupt_gate0x99,	@function
	.type	interrupt_gate0x9a,	@function
	.type	interrupt_gate0x9b,	@function
	.type	interrupt_gate0x9c,	@function
	.type	interrupt_gate0x9d,	@function
	.type	interrupt_gate0x9e,	@function
	.type	interrupt_gate0x9f,	@function
	.type	interrupt_gate0xa0,	@function
	.type	interrupt_gate0xa1,	@function
	.type	interrupt_gate0xa2,	@function
	.type	interrupt_gate0xa3,	@function
	.type	interrupt_gate0xa4,	@function
	.type	interrupt_gate0xa5,	@function
	.type	interrupt_gate0xa6,	@function
	.type	interrupt_gate0xa7,	@function
	.type	interrupt_gate0xa8,	@function
	.type	interrupt_gate0xa9,	@function
	.type	interrupt_gate0xaa,	@function
	.type	interrupt_gate0xab,	@function
	.type	interrupt_gate0xac,	@function
	.type	interrupt_gate0xad,	@function
	.type	interrupt_gate0xae,	@function
	.type	interrupt_gate0xaf,	@function
	.type	interrupt_gate0xb0,	@function
	.type	interrupt_gate0xb1,	@function
	.type	interrupt_gate0xb2,	@function
	.type	interrupt_gate0xb3,	@function
	.type	interrupt_gate0xb4,	@function
	.type	interrupt_gate0xb5,	@function
	.type	interrupt_gate0xb6,	@function
	.type	interrupt_gate0xb7,	@function
	.type	interrupt_gate0xb8,	@function
	.type	interrupt_gate0xb9,	@function
	.type	interrupt_gate0xba,	@function
	.type	interrupt_gate0xbb,	@function
	.type	interrupt_gate0xbc,	@function
	.type	interrupt_gate0xbd,	@function
	.type	interrupt_gate0xbe,	@function
	.type	interrupt_gate0xbf,	@function
	.type	interrupt_gate0xc0,	@function
	.type	interrupt_gate0xc1,	@function
	.type	interrupt_gate0xc2,	@function
	.type	interrupt_gate0xc3,	@function
	.type	interrupt_gate0xc4,	@function
	.type	interrupt_gate0xc5,	@function
	.type	interrupt_gate0xc6,	@function
	.type	interrupt_gate0xc7,	@function
	.type	interrupt_gate0xc8,	@function
	.type	interrupt_gate0xc9,	@function
	.type	interrupt_gate0xca,	@function
	.type	interrupt_gate0xcb,	@function
	.type	interrupt_gate0xcc,	@function
	.type	interrupt_gate0xcd,	@function
	.type	interrupt_gate0xce,	@function
	.type	interrupt_gate0xcf,	@function
	.type	interrupt_gate0xd0,	@function
	.type	interrupt_gate0xd1,	@function
	.type	interrupt_gate0xd2,	@function
	.type	interrupt_gate0xd3,	@function
	.type	interrupt_gate0xd4,	@function
	.type	interrupt_gate0xd5,	@function
	.type	interrupt_gate0xd6,	@function
	.type	interrupt_gate0xd7,	@function
	.type	interrupt_gate0xd8,	@function
	.type	interrupt_gate0xd9,	@function
	.type	interrupt_gate0xda,	@function
	.type	interrupt_gate0xdb,	@function
	.type	interrupt_gate0xdc,	@function
	.type	interrupt_gate0xdd,	@function
	.type	interrupt_gate0xde,	@function
	.type	interrupt_gate0xdf,	@function
	.type	interrupt_gate0xe0,	@function
	.type	interrupt_gate0xe1,	@function
	.type	interrupt_gate0xe2,	@function
	.type	interrupt_gate0xe3,	@function
	.type	interrupt_gate0xe4,	@function
	.type	interrupt_gate0xe5,	@function
	.type	interrupt_gate0xe6,	@function
	.type	interrupt_gate0xe7,	@function
	.type	interrupt_gate0xe8,	@function
	.type	interrupt_gate0xe9,	@function
	.type	interrupt_gate0xea,	@function
	.type	interrupt_gate0xeb,	@function
	.type	interrupt_gate0xec,	@function
	.type	interrupt_gate0xed,	@function
	.type	interrupt_gate0xee,	@function
	.type	interrupt_gate0xef,	@function
	.type	interrupt_gate0xf0,	@function
	.type	interrupt_gate0xf1,	@function
	.type	interrupt_gate0xf2,	@function
	.type	interrupt_gate0xf3,	@function
	.type	interrupt_gate0xf4,	@function
	.type	interrupt_gate0xf5,	@function
	.type	interrupt_gate0xf6,	@function
	.type	interrupt_gate0xf7,	@function
	.type	interrupt_gate0xf8,	@function
	.type	interrupt_gate0xf9,	@function
	.type	interrupt_gate0xfa,	@function
	.type	interrupt_gate0xfb,	@function
	.type	interrupt_gate0xfc,	@function
	.type	interrupt_gate0xfd,	@function
	.type	interrupt_gate0xfe,	@function
	.type	interrupt_gate0xff,	@function

				# // devide by 0 exception handler
interrupt_gate0x00:		# void interrupt_gate0x00(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	devide_by_zero_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // debug exception handler
interrupt_gate0x01:		# void interrupt_gate0x01(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	debug_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // non maskable interrupt handler
interrupt_gate0x02:		# void interrupt_gate0x02(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	non_maskable_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // breakpoint exception handler
interrupt_gate0x03:		# void interrupt_gate0x03(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	breakpoint_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // overflow exception handler
interrupt_gate0x04:		# void interrupt_gate0x04(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	overflow_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // bound range exceeded exception handler
interrupt_gate0x05:		# void interrupt_gate0x05(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	bound_range_exceeded_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // invalid opcode exception handler
interrupt_gate0x06:		# void interrupt_gate0x06(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	invalid_opcode_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // device not available exception handler
interrupt_gate0x07:		# void interrupt_gate0x07(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	device_not_available_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // double fault exception handler
interrupt_gate0x08:		# void interrupt_gate0x08(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	double_fault_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // coprocessor segment overrun exception handler
interrupt_gate0x09:		# void interrupt_gate0x09(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	coprocessor_segment_overrun_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // invalid TSS exception handler
interrupt_gate0x0a:		# void interrupt_gate0x0a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	invalid_TSS_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // segment not present exception handler
interrupt_gate0x0b:		# void interrupt_gate0x0b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	segment_not_present_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // stack segment fault exception handler
interrupt_gate0x0c:		# void interrupt_gate0x0c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	stack_segment_fault_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // general protection fault exception handler
interrupt_gate0x0d:		# void interrupt_gate0x0d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	sti
	call	general_protection_fault_exception_handler
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // page fault exception handler
interrupt_gate0x0e:		# void interrupt_gate0x0e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	page_fault_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x0f:		# void interrupt_gate0x0e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x00
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // x87 floating point exception handler
interrupt_gate0x10:		# void interrupt_gate0x10(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	x87_floating_point_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // alignment check exception handler
interrupt_gate0x11:		# void interrupt_gate0x11(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	alignment_check_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // machine check exception handler
interrupt_gate0x12:		# void interrupt_gate0x12(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	machine_check_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // SIMD floating point exception handler
interrupt_gate0x13:		# void interrupt_gate0x13(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	simd_floating_point_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // virtualization exception handler
interrupt_gate0x14:		# void interrupt_gate0x14(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	virtualization_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x15:		# void interrupt_gate0x15(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x01
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x16:		# void interrupt_gate0x16(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x02
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x17:		# void interrupt_gate0x17(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x03
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x18:		# void interrupt_gate0x18(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x04
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x19:		# void interrupt_gate0x19(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x05
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x1a:		# void interrupt_gate0x1a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x06
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x1b:		# void interrupt_gate0x1b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x07
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x1c:		# void interrupt_gate0x1c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x08
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x1d:		# void interrupt_gate0x1d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x09
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // security exception handler
interrupt_gate0x1e:		# void interrupt_gate0x1e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	security_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // reserved exception handler
interrupt_gate0x1f:		# void interrupt_gate0x1f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	reserved_exception_handler0x0a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // pit interrupt handler
interrupt_gate0x20:		# void interrupt_gate0x20(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	pit_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // keyboard interrupt handler
interrupt_gate0x21:		# void interrupt_gate0x21(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	keyboard_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // slave pic interrupt handler
interrupt_gate0x22:		# void interrupt_gate0x22(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	slave_pic_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // COM2 interrupt handler
interrupt_gate0x23:		# void interrupt_gate0x23(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	com2_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // COM1 interrupt handler
interrupt_gate0x24:		# void interrupt_gate0x24(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	com1_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // LPT2 interrupt handler
interrupt_gate0x25:		# void interrupt_gate0x25(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	lpt2_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // disk interrupt handler
interrupt_gate0x26:		# void interrupt_gate0x26(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	disk_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // LPT1 interrupt handler
interrupt_gate0x27:		# void interrupt_gate0x27(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	lpt1_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // rtc interrupt handler
interrupt_gate0x28:		# void interrupt_gate0x28(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	rtc_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret
				# // peripheral0 interrupt handler
interrupt_gate0x29:		# void interrupt_gate0x29(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	peripheral0_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // peripheral1 interrupt handler
interrupt_gate0x2a:		# void interrupt_gate0x2a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	peripheral1_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // peripheral2 interrupt handler
interrupt_gate0x2b:		# void interrupt_gate0x2b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	peripheral2_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // mouse interrupt handler
interrupt_gate0x2c:		# void interrupt_gate0x2c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	mouse_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // FPU error exception handler
interrupt_gate0x2d:		# void interrupt_gate0x2d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	fpu_error_exception_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // primary ATA hard disk interrupt handler
interrupt_gate0x2e:		# void interrupt_gate0x2e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	primary_ATA_hard_disk_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

				# // secondary ATA hard disk interrupt handler
interrupt_gate0x2f:		# void interrupt_gate0x2f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	secondary_ATA_hard_disk_interrupt_handler
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x30:		# void interrupt_gate0x30(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x30
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x31:		# void interrupt_gate0x31(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x31
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x32:		# void interrupt_gate0x32(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x32
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x33:		# void interrupt_gate0x33(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x33
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x34:		# void interrupt_gate0x34(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x34
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x35:		# void interrupt_gate0x35(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x35
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x36:		# void interrupt_gate0x36(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x36
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x37:		# void interrupt_gate0x37(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x37
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x38:		# void interrupt_gate0x38(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x38
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x39:		# void interrupt_gate0x39(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x39
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3a:		# void interrupt_gate0x3a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3b:		# void interrupt_gate0x3b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3c:		# void interrupt_gate0x3c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3d:		# void interrupt_gate0x3d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3e:		# void interrupt_gate0x3e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x3f:		# void interrupt_gate0x3f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x3f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x40:		# void interrupt_gate0x40(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x40
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x41:		# void interrupt_gate0x41(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x41
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x42:		# void interrupt_gate0x42(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x42
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x43:		# void interrupt_gate0x43(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x43
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x44:		# void interrupt_gate0x44(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x44
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x45:		# void interrupt_gate0x45(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x45
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x46:		# void interrupt_gate0x46(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x46
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x47:		# void interrupt_gate0x47(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x47
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x48:		# void interrupt_gate0x48(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x48
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x49:		# void interrupt_gate0x49(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x49
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4a:		# void interrupt_gate0x4a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4b:		# void interrupt_gate0x4b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4c:		# void interrupt_gate0x4c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4d:		# void interrupt_gate0x4d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4e:		# void interrupt_gate0x4e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x4f:		# void interrupt_gate0x4f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x4f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x50:		# void interrupt_gate0x50(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x50
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x51:		# void interrupt_gate0x51(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x51
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x52:		# void interrupt_gate0x52(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x52
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x53:		# void interrupt_gate0x53(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x53
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x54:		# void interrupt_gate0x54(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x54
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x55:		# void interrupt_gate0x55(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x55
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x56:		# void interrupt_gate0x56(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x56
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x57:		# void interrupt_gate0x57(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x57
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x58:		# void interrupt_gate0x58(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x58
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x59:		# void interrupt_gate0x59(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x59
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5a:		# void interrupt_gate0x5a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5b:		# void interrupt_gate0x5b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5c:		# void interrupt_gate0x5c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5d:		# void interrupt_gate0x5d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5e:		# void interrupt_gate0x5e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x5f:		# void interrupt_gate0x5f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x5f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x60:		# void interrupt_gate0x60(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x60
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x61:		# void interrupt_gate0x61(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x61
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x62:		# void interrupt_gate0x62(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x62
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x63:		# void interrupt_gate0x63(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x63
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x64:		# void interrupt_gate0x64(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x64
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x65:		# void interrupt_gate0x65(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x65
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x66:		# void interrupt_gate0x66(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x66
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x67:		# void interrupt_gate0x67(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x67
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x68:		# void interrupt_gate0x68(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x68
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x69:		# void interrupt_gate0x69(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x69
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6a:		# void interrupt_gate0x6a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6b:		# void interrupt_gate0x6b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6c:		# void interrupt_gate0x6c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6d:		# void interrupt_gate0x6d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6e:		# void interrupt_gate0x6e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x6f:		# void interrupt_gate0x6f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x6f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x70:		# void interrupt_gate0x70(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x70
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x71:		# void interrupt_gate0x71(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x71
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x72:		# void interrupt_gate0x72(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x72
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x73:		# void interrupt_gate0x73(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x73
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x74:		# void interrupt_gate0x74(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x74
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x75:		# void interrupt_gate0x75(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x75
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x76:		# void interrupt_gate0x76(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x76
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x77:		# void interrupt_gate0x77(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x77
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x78:		# void interrupt_gate0x78(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x78
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x79:		# void interrupt_gate0x79(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x79
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7a:		# void interrupt_gate0x7a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7b:		# void interrupt_gate0x7b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7c:		# void interrupt_gate0x7c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7d:		# void interrupt_gate0x7d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7e:		# void interrupt_gate0x7e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x7f:		# void interrupt_gate0x7f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x7f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x80:		# void interrupt_gate0x80(void);
0:
	subl	$0x00000004,%esp	# Space for return value
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	# Push system_call arguments
	subl	$0x0000001c,%esp
	movl	0x30(%esp),%edx		# Push ebp
	movl	%edx,	0x18(%esp)
	movl	0x28(%esp),%edx		# Push edi
	movl	%edx,	0x14(%esp)
	movl	0x2c(%esp),%edx		# Push esi
	movl	%edx,	0x10(%esp)
	movl	0x3c(%esp),%edx		# Push edx
	movl	%edx,	0x0c(%esp)
	movl	0x40(%esp),%edx		# Push ecx
	movl	%edx,	0x08(%esp)
	movl	0x38(%esp),%edx		# Push ebx
	movl	%edx,	0x04(%esp)
	movl	0x44(%esp),%edx		# Push eax
	movl	%edx,	(%esp)
	sti
	call	system_call
	addl	$0x0000001c,%esp
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	movl	%eax,	0x20(%esp)
	popal
	popl	%eax			# Pop return value
	iret

interrupt_gate0x81:		# void interrupt_gate0x81(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x81
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x82:		# void interrupt_gate0x82(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x82
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x83:		# void interrupt_gate0x83(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x83
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x84:		# void interrupt_gate0x84(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x84
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x85:		# void interrupt_gate0x85(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x85
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x86:		# void interrupt_gate0x86(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x86
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x87:		# void interrupt_gate0x87(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x87
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x88:		# void interrupt_gate0x88(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x88
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x89:		# void interrupt_gate0x89(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x89
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8a:		# void interrupt_gate0x8a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8b:		# void interrupt_gate0x8b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8c:		# void interrupt_gate0x8c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8d:		# void interrupt_gate0x8d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8e:		# void interrupt_gate0x8e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x8f:		# void interrupt_gate0x8f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x8f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x90:		# void interrupt_gate0x90(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x90
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x91:		# void interrupt_gate0x91(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x91
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x92:		# void interrupt_gate0x92(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x92
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x93:		# void interrupt_gate0x93(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x93
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x94:		# void interrupt_gate0x94(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x94
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x95:		# void interrupt_gate0x95(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x95
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x96:		# void interrupt_gate0x96(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x96
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x97:		# void interrupt_gate0x97(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x97
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x98:		# void interrupt_gate0x98(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x98
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x99:		# void interrupt_gate0x99(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x99
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9a:		# void interrupt_gate0x9a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9a
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9b:		# void interrupt_gate0x9b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9b
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9c:		# void interrupt_gate0x9c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9c
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9d:		# void interrupt_gate0x9d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9d
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9e:		# void interrupt_gate0x9e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9e
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0x9f:		# void interrupt_gate0x9f(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0x9f
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa0:		# void interrupt_gate0xa0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa1:		# void interrupt_gate0xa1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa2:		# void interrupt_gate0xa2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa3:		# void interrupt_gate0xa3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa4:		# void interrupt_gate0xa4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa5:		# void interrupt_gate0xa5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa6:		# void interrupt_gate0xa6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa7:		# void interrupt_gate0xa7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa8:		# void interrupt_gate0xa8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xa9:		# void interrupt_gate0xa9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xa9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xaa:		# void interrupt_gate0xaa(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xaa
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xab:		# void interrupt_gate0xab(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xab
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xac:		# void interrupt_gate0xac(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xac
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xad:		# void interrupt_gate0xad(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xad
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xae:		# void interrupt_gate0xae(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xae
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xaf:		# void interrupt_gate0xaf(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xaf
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb0:		# void interrupt_gate0xb0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb1:		# void interrupt_gate0xb1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb2:		# void interrupt_gate0xb2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb3:		# void interrupt_gate0xb3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb4:		# void interrupt_gate0xb4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb5:		# void interrupt_gate0xb5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb6:		# void interrupt_gate0xb6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb7:		# void interrupt_gate0xb7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb8:		# void interrupt_gate0xb8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xb9:		# void interrupt_gate0xb9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xb9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xba:		# void interrupt_gate0xba(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xba
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xbb:		# void interrupt_gate0xbb(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xbb
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xbc:		# void interrupt_gate0xbc(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xbc
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xbd:		# void interrupt_gate0xbd(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xbd
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xbe:		# void interrupt_gate0xbe(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xbe
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xbf:		# void interrupt_gate0xbf(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xbf
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc0:		# void interrupt_gate0xc0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc1:		# void interrupt_gate0xc1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc2:		# void interrupt_gate0xc2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc3:		# void interrupt_gate0xc3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc4:		# void interrupt_gate0xc4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc5:		# void interrupt_gate0xc5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc6:		# void interrupt_gate0xc6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc7:		# void interrupt_gate0xc7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc8:		# void interrupt_gate0xc8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xc9:		# void interrupt_gate0xc9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xc9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xca:		# void interrupt_gate0xca(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xca
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xcb:		# void interrupt_gate0xcb(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xcb
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xcc:		# void interrupt_gate0xcc(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xcc
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xcd:		# void interrupt_gate0xcd(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xcd
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xce:		# void interrupt_gate0xce(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xce
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xcf:		# void interrupt_gate0xcf(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xcf
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd0:		# void interrupt_gate0xd0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd1:		# void interrupt_gate0xd1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd2:		# void interrupt_gate0xd2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd3:		# void interrupt_gate0xd3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd4:		# void interrupt_gate0xd4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd5:		# void interrupt_gate0xd5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd6:		# void interrupt_gate0xd6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd7:		# void interrupt_gate0xd7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd8:		# void interrupt_gate0xd8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xd9:		# void interrupt_gate0xd9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xd9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xda:		# void interrupt_gate0xda(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xda
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xdb:		# void interrupt_gate0xdb(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xdb
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xdc:		# void interrupt_gate0xdc(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xdc
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xdd:		# void interrupt_gate0xdd(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xdd
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xde:		# void interrupt_gate0xde(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xde
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xdf:		# void interrupt_gate0xdf(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xdf
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe0:		# void interrupt_gate0xe0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe1:		# void interrupt_gate0xe1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe2:		# void interrupt_gate0xe2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe3:		# void interrupt_gate0xe3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe4:		# void interrupt_gate0xe4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe5:		# void interrupt_gate0xe5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe6:		# void interrupt_gate0xe6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe7:		# void interrupt_gate0xe7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe8:		# void interrupt_gate0xe8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xe9:		# void interrupt_gate0xe9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xe9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xea:		# void interrupt_gate0xea(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xea
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xeb:		# void interrupt_gate0xeb(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xeb
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xec:		# void interrupt_gate0xec(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xec
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xed:		# void interrupt_gate0xed(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xed
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xee:		# void interrupt_gate0xee(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xee
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xef:		# void interrupt_gate0xef(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xef
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf0:		# void interrupt_gate0xf0(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf0
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf1:		# void interrupt_gate0xf1(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf1
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf2:		# void interrupt_gate0xf2(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf2
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf3:		# void interrupt_gate0xf3(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf3
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf4:		# void interrupt_gate0xf4(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf4
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf5:		# void interrupt_gate0xf5(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf5
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf6:		# void interrupt_gate0xf6(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf6
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf7:		# void interrupt_gate0xf7(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf7
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf8:		# void interrupt_gate0xf8(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf8
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xf9:		# void interrupt_gate0xf9(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xf9
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xfa:		# void interrupt_gate0xfa(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xfa
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xfb:		# void interrupt_gate0xfb(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xfb
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xfc:		# void interrupt_gate0xfc(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xfc
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xfd:		# void interrupt_gate0xfd(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xfd
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xfe:		# void interrupt_gate0xfe(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xfe
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

interrupt_gate0xff:		# void interrupt_gate0xff(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%ds,	%dx
	pushl	%edx
	movw	$kernel_data_segment_selector,%dx
	movw	%dx	,%ds
	movw	%dx	,%es
	movw	%dx	,%fs
	movw	%dx	,%gs
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	interrupt_handler0xff
	call	sti_task_interrupt
	popl	%edx
	movw	%dx,	%ds
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%gs
	popl	%edx
	movw	%dx,	%ss
	popal
	iret

