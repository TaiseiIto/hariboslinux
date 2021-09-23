# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.extern	alignment_check_exception_handler
	.extern	bound_range_exceeded_exception_handler
	.extern	breakpoint_exception_handler
	.extern cli_task_interrupt
	.extern	coprocessor_segment_overrun_exception_handler
	.extern	debug_exception_handler
	.extern	device_not_available_exception_handler
	.extern	devide_by_zero_exception_handler
	.extern	double_fault_exception_handler
	.extern	fpu_error_exception_handler
	.extern	general_protection_fault_exception_handler
	.extern	invalid_opcode_exception_handler
	.extern	invalid_TSS_exception_handler
	.extern	keyboard_interrupt_handler
	.extern	machine_check_exception_handler
	.extern mouse_interrupt_handler
	.extern	non_maskable_interrupt_handler
	.extern	overflow_exception_handler
	.extern	page_fault_exeption_handler
	.extern	security_exception_handler
	.extern	segment_not_present_exception_handler
	.extern	simd_floating_point_exception_handler
	.extern	stack_segment_fault_exception_handler
	.extern sti_task_interrupt
	.extern	virtualization_exception_handler
	.extern	x87_floating_point_exception_handler

	.globl	interrupt_handler0x00
	.globl	interrupt_handler0x01
	.globl	interrupt_handler0x02
	.globl	interrupt_handler0x03
	.globl	interrupt_handler0x04
	.globl	interrupt_handler0x05
	.globl	interrupt_handler0x06
	.globl	interrupt_handler0x07
	.globl	interrupt_handler0x08
	.globl	interrupt_handler0x09
	.globl	interrupt_handler0x0a
	.globl	interrupt_handler0x0b
	.globl	interrupt_handler0x0c
	.globl	interrupt_handler0x0d
	.globl	interrupt_handler0x0e
	.globl	interrupt_handler0x10
	.globl	interrupt_handler0x11
	.globl	interrupt_handler0x12
	.globl	interrupt_handler0x13
	.globl	interrupt_handler0x14
	.globl	interrupt_handler0x1e
	.globl	interrupt_handler0x21
	.globl	interrupt_handler0x2c
	.globl	interrupt_handler0x2d

	.type	interrupt_handler0x00,	@function
	.type	interrupt_handler0x01,	@function
	.type	interrupt_handler0x02,	@function
	.type	interrupt_handler0x03,	@function
	.type	interrupt_handler0x04,	@function
	.type	interrupt_handler0x05,	@function
	.type	interrupt_handler0x06,	@function
	.type	interrupt_handler0x07,	@function
	.type	interrupt_handler0x08,	@function
	.type	interrupt_handler0x09,	@function
	.type	interrupt_handler0x0a,	@function
	.type	interrupt_handler0x0b,	@function
	.type	interrupt_handler0x0c,	@function
	.type	interrupt_handler0x0d,	@function
	.type	interrupt_handler0x0e,	@function
	.type	interrupt_handler0x10,	@function
	.type	interrupt_handler0x11,	@function
	.type	interrupt_handler0x12,	@function
	.type	interrupt_handler0x13,	@function
	.type	interrupt_handler0x14,	@function
	.type	interrupt_handler0x1e,	@function
	.type	interrupt_handler0x21,	@function
	.type	interrupt_handler0x2c,	@function
	.type	interrupt_handler0x2d,	@function

				# // devide by 0 exception handler
interrupt_handler0x00:		# void interrupt_handler0x00(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x01:		# void interrupt_handler0x01(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x02:		# void interrupt_handler0x02(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x03:		# void interrupt_handler0x03(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x04:		# void interrupt_handler0x04(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x05:		# void interrupt_handler0x05(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x06:		# void interrupt_handler0x06(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x07:		# void interrupt_handler0x07(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x08:		# void interrupt_handler0x08(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x09:		# void interrupt_handler0x09(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x0a:		# void interrupt_handler0x0a(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x0b:		# void interrupt_handler0x0b(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x0c:		# void interrupt_handler0x0c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x0d:		# void interrupt_handler0x0d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x0e:		# void interrupt_handler0x0e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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

				# // x87 floating point exception handler
interrupt_handler0x10:		# void interrupt_handler0x10(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x11:		# void interrupt_handler0x11(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x12:		# void interrupt_handler0x12(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x13:		# void interrupt_handler0x13(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x14:		# void interrupt_handler0x14(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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

				# // security exception handler
interrupt_handler0x1e:		# void interrupt_handler0x1e(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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

				# // kerboard interrupt handler
interrupt_handler0x21:		# void interrupt_handler0x21(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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

				# // mouse interrupt handler
interrupt_handler0x2c:		# void interrupt_handler0x2c(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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
interrupt_handler0x2d:		# void interrupt_handler0x2d(void);
0:
	pushal
	movw	%ss,	%dx
	pushl	%edx
	movw	%gs,	%dx
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

