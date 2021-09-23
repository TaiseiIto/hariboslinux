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
	.extern primary_ATA_hard_disk_interrupt_handler
	.extern real_time_clock_interrupt_handler
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
	.extern timer_interrupt_handler
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
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	general_protection_fault_exception_handler
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

				# // kerboard interrupt handler
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
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	timer_interrupt_handler
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

				# // kerboard interrupt handler
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

				# // real time clock interrupt handler
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
	movw	%dx	,%ss
	call	cli_task_interrupt
	call	real_time_clock_interrupt_handler
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

