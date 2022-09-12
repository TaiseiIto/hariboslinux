	.code16
	.text
main:
	fninit	# Initialize the FPU
	fld1	# Push 1.0
	fldl2t	# Push log 2 10
	fldl2e	# Push log 2 e
	fldpi	# Push pi
	fldlg2	# Push log 10 2
	fldln2	# Push log e 2
	fldz	# Push 0.0
loop:
	hlt
	jmp loop

