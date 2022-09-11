	.code16
	.text
main:
	fninit
	fld1
	fldl2t
	fldl2e
	fldpi
	fldlg2
	fldln2
	fldz
loop:
	hlt
	jmp loop

