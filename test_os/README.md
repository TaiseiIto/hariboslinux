# Symptom

## Correct output

```
~/hariboslinux/test_os # make debug
(gdb) break i387_print_float_info
(gdb) continue
(gdb) finish
  R7: Valid   0x3fff8000000000000000 +1
  R6: Valid   0x4000d49a784bcd1b8afe +3.321928094887362348
  R5: Valid   0x3fffb8aa3b295c17f0bc +1.442695040888963407
  R4: Valid   0x4000c90fdaa22168c235 +3.141592653589793239
  R3: Valid   0x3ffd9a209a84fbcff799 +0.3010299956639811952
  R2: Valid   0x3ffeb17217f7d1cf79ac +0.6931471805599453094
  R1: Valid   0x00000000000000000000 +0
=>R0: Valid   0x00000000000000000000 +0

Status Word:         0x0800
                       TOP: 1
Control Word:        0x037f   IM DM ZM OM UM PM
                       PC: Extended Precision (64-bits)
                       RC: Round to nearest
Tag Word:            0x0000
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

## Actual output

```
~/hariboslinux/test_os # make debug
(gdb) break i387_print_float_info
(gdb) continue
(gdb) finish
  R7: Valid   0x4000d49a784bcd1b8afe +3.321928094887362348
  R6: Valid   0x3fffb8aa3b295c17f0bc +1.442695040888963407
  R5: Valid   0x4000c90fdaa22168c235 +3.141592653589793239
  R4: Valid   0x3ffd9a209a84fbcff799 +0.3010299956639811952
  R3: Valid   0x3ffeb17217f7d1cf79ac +0.6931471805599453094
  R2: Valid   0x00000000000000000000 +0
=>R1: Valid   0x00000000000000000000 +0
  R0: Valid   0x3fff8000000000000000 +1

Status Word:         0x0800
                       TOP: 1
Control Word:        0x037f   IM DM ZM OM UM PM
                       PC: Extended Precision (64-bits)
                       RC: Round to nearest
Tag Word:            0x0000
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

# Cause investigation

```
~/hariboslinux/test_os # make debug
(gdb) break i387_print_float_info
(gdb) continue
~/binutils-gdb/gdb/i387-tdep.c : 206 i387_print_float_info
(gdb) break 285
(gdb) continue
(gdb) p/x fpreg
= 0x7 // means R7
(gdb) p/x regnum
= 0x16
(gdb) break 289
(gdb) continue
(gdb) x/10bx raw // print content of R7
0x5652f25f90f0: 0xfe    0x8a    0x1b    0xcd    0x4b    0x78    0x9a    0xd4
0x5652f25f90f8: 0x00    0x40
(gdb) delete
(gdb) break print_i387_status_word
(gdb) continue
~/binutils-gdb/gdb/i387-tdep.c : 108 print_i387_status_word
(gdb) p/x status
= 0x800 // It means TOP = 1, ST0 = R1
```

In function `i387_print_float_info` in `~/binutils-gdb/gdb/i387-tdep.c`, register numbers of `STx` are converted to register number of `Rx`.

In `~/binutils-gdb/gdb/i387-tdep.c` line 231, get status word.

```
  fstat_p = read_frame_register_unsigned (frame,
					  I387_FSTAT_REGNUM (tdep), &fstat);
```

In `~/binutils-gdb/gdb/i387-tdep.c` line 248, get stack top position from status word.

```
      top = ((fstat >> 11) & 7);
```

In `~/binutils-gdb/gdb/i387-tdep.c` line 248, get `Rx` register number.

```
	  regnum = (fpreg + 8 - top) % 8 + I387_ST0_REGNUM (tdep);
```

It seems `I387_ST0_REGNUM` is already pointing `R0` and has applied map from `STx` wo `Rx` double times.

## Where does ST0 come from ? It seems R0.

`Rx` is gotten in `~/binutils-gdb/gdb/i387-tdep.c` line 282 and 283.

```
	  regnum = (fpreg + 8 - top) % 8 + I387_ST0_REGNUM (tdep);
	  regval = get_frame_register_value (frame, regnum);
```

10 bytes (80 bits) value of `Rx` is gotten in `~/binutils-gdb/gdb/i387-tdep.c` line from 287 to 291.

```
	      const gdb_byte *raw = value_contents (regval).data ();

	      gdb_puts ("0x", file);
	      for (i = 9; i >= 0; i--)
		gdb_printf (file, "%02x", raw[i]);
```

`Rx` is gotten where `fpreg` is equal to x, `Rx` is equal to `STy` and `(fpreg + 8 - top) % 8` is equal to `y`.

### What does `I387_ST0_REGNUM(tdep)` in `~/binutils-gdb/gdb/i387-tdep.c` line 282 return?

`I387_ST0_REGNUM` is defined in `~/binutils-gdb/gdb/i387-tdep.h` line 32.

```
#define I387_ST0_REGNUM(tdep) ((tdep)->st0_regnum)
```

#### `tdep->st0_regnum` is stored at `~/binutils-gdb/gdb/i386-tdep.c` line 8477

```
  tdep->st0_regnum = I386_ST0_REGNUM;
```

And `I386_ST0_REGNUM` is defined at `~/binutils-gdb/gdb/i386-tdep.h` line293.

```
enum i386_regnum
{
  I386_EAX_REGNUM,		/* %eax */
  I386_ECX_REGNUM,		/* %ecx */
  I386_EDX_REGNUM,		/* %edx */
  I386_EBX_REGNUM,		/* %ebx */
  I386_ESP_REGNUM,		/* %esp */
  I386_EBP_REGNUM,		/* %ebp */
  I386_ESI_REGNUM,		/* %esi */
  I386_EDI_REGNUM,		/* %edi */
  I386_EIP_REGNUM,		/* %eip */
  I386_EFLAGS_REGNUM,		/* %eflags */
  I386_CS_REGNUM,		/* %cs */
  I386_SS_REGNUM,		/* %ss */
  I386_DS_REGNUM,		/* %ds */
  I386_ES_REGNUM,		/* %es */
  I386_FS_REGNUM,		/* %fs */
  I386_GS_REGNUM,		/* %gs */
  I386_ST0_REGNUM,		/* %st(0) */
  I386_MXCSR_REGNUM = 40,	/* %mxcsr */ 
  I386_YMM0H_REGNUM,		/* %ymm0h */
  I386_YMM7H_REGNUM = I386_YMM0H_REGNUM + 7,
  I386_BND0R_REGNUM,
  I386_BND3R_REGNUM = I386_BND0R_REGNUM + 3,
  I386_BNDCFGU_REGNUM,
  I386_BNDSTATUS_REGNUM,
  I386_K0_REGNUM,		/* %k0 */
  I386_K7_REGNUM = I386_K0_REGNUM + 7,
  I386_ZMM0H_REGNUM,		/* %zmm0h */
  I386_ZMM7H_REGNUM = I386_ZMM0H_REGNUM + 7,
  I386_PKRU_REGNUM,
  I386_FSBASE_REGNUM,
  I386_GSBASE_REGNUM
};
```

```
~/hariboslinux/test_os # make debug
(gdb) break i386-tdep.c : 8477
(gdb) break i387-tdep.c : 282
(gdb) continue
i386-tdep.c : 8477
(gdb) continue
i386-tdep.c : 8477
(gdb) p/x tdep
0x558b8d261820
(gdb) next
(gdb) p/x tdep->st0_regnum
0x0
(gdb) continue
i387-tdep.c : 282
(gdb) p/x tdep
0x558b8d261820
(gdb) p/x tdep->st0_regnum
0x0
```

### What does `get_frame_register_value (frame, regnum)` in `~/binutils-gdb/gdb/i387-tdep.c` line 283 return?

### What does `value_contents (regval).data ()` in `~/binutils-gdb/gdb/i387-tdep.c` line 287 return?

## Where does status word come from ?

The status word is gotten as `fstat` in `~/binutils-gdb/gdb/i387-tdep.c` line 232 and its bits from 11 to 13 means stack top pointer x where `ST0` is `Rx`.

```
  fstat_p = read_frame_register_unsigned (frame,
					  I387_FSTAT_REGNUM (tdep), &fstat);
```

