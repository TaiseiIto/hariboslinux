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

# Cause investigation in GDB

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

```
~/hariboslinux/test_os # make debug
(gdb) break i387-tdep.c : 285
(gdb) continue
(gdb) p/x regnum
$1 = 0x16
(gdb) x/10bx regval->contents.get()
0x565528ce9110: 0xfe    0x8a    0x1b    0xcd    0x4b    0x78    0x9a    0xd4
0x565528ce9118: 0x00    0x40
```

Correctly, R6 is equal to ST5 and has this value.
Actually, R7 is equal to ST6 and has this value.
I think gdb thinks this register is ST6 what is actually R6.

Bellow seems to try to get value of R7 which is ST6 and has gotten R6 which is ST5.

```
~/hariboslinux/test_os # make debug
(gdb) break i387-tdep.c : 283
	  regval = get_frame_register_value (frame, regnum);
(gdb) continue 
(gdb) break regcache.c : 613
    memcpy (buf, register_buffer (regnum),
	    m_descr->sizeof_register[regnum]);
(gdb) continue
(gdb) p/x regnum
$1 = 0x16
(gdb) p/x register_buffer(regnum)
$2 = 0x5640e37abcbc
(gdb) p/x m_descr->sizeof_register[regnum]
$3 = 0xa
(gdb) x/10bx register_buffer(regnum)
0x5640e37abcbc: 0xfe    0x8a    0x1b    0xcd    0x4b    0x78    0x9a    0xd4
0x5640e37abcc4: 0x00    0x40
```

`reg_buffer::register_buffer` returns register value.

```
~/hariboslinux/test_os # make debug
(gdb) break i387-tdep.c : 283
(gdb) continue
(gdb) break reg_buffer::register_buffer if regnum == 0x16
(gdb) continue
(gdb) p/x regnum
$1 = 0x16
(gdb) p/x m_descr->register_offset[regnum]
$2 = 0x7c
(gdb) x/32gx m_registers.get()
0x560476f54c40: 0x000000000000aa55      0x0000000000000000
0x560476f54c50: 0x0000000000006f04      0x0000000000000000
0x560476f54c60: 0x0000020200007c10      0x0000000000000000
0x560476f54c70: 0x0000000000000000      0x0000000000000000
0x560476f54c80: 0x0000000000000000      0x0000000000000000
0x560476f54c90: 0xd1cf79ac00000000      0xf7993ffeb17217f7
0x560476f54ca0: 0x3ffd9a209a84fbcf      0xc90fdaa22168c235
0x560476f54cb0: 0x3b295c17f0bc4000      0xcd1b8afe3fffb8aa
0x560476f54cc0: 0x00004000d49a784b      0x3fff800000000000
0x560476f54cd0: 0x000008000000037f      0x0000000000000000
0x560476f54ce0: 0x0000000000000000      0x0000000000000000
0x560476f54cf0: 0x0000000000000000      0x0000001000000000
0x560476f54d00: 0x0000000000000000      0x0000000000000000
0x560476f54d10: 0x0000000000000000      0x0000000000000000
0x560476f54d20: 0x0000000000000000      0x0000000000000000
0x560476f54d30: 0x0000000000000000      0x0000000000000000
```

According to `i386-tdep.h` line 275, `i387-tdep.h` line 56 and the above result, registers are deployed on memory as bellow.

| Register name | Register number | Register offset |
| :------------ | --------------: | --------------: |
| EAX           |            0x00 |            0x00 |
| ECX           |            0x01 |            0x04 |
| EDX           |            0x02 |            0x08 |
| EBX           |            0x03 |            0x0c |
| ESP           |            0x04 |            0x10 |
| EBP           |            0x05 |            0x14 |
| ESI           |            0x06 |            0x18 |
| EDI           |            0x07 |            0x1c |
| EIP           |            0x08 |            0x20 |
| EFLAGS        |            0x09 |            0x24 |
| CS            |            0x0a |            0x28 |
| SS            |            0x0b |            0x2c |
| DS            |            0x0c |            0x30 |
| ES            |            0x0d |            0x34 |
| FS            |            0x0e |            0x38 |
| GS            |            0x0f |            0x3c |
| ST0           |            0x10 |            0x40 |
| ST1           |            0x11 |            0x4a |
| ST2           |            0x12 |            0x54 |
| ST3           |            0x13 |            0x5e |
| ST4           |            0x14 |            0x68 |
| ST5           |            0x15 |            0x72 |
| ST6           |            0x16 |            0x7c |
| ST7           |            0x17 |            0x86 |
| FCTRL         |            0x18 |            0x90 |
| FSTAT         |            0x19 |            0x94 |
| FTAG          |            0x1a |            0x98 |
| FISEG         |            0x1b |            0x9c |
| FIOFF         |            0x1c |            0xa0 |
| FOSEG         |            0x1d |            0xa4 |
| FOOFF         |            0x1e |            0xa8 |
| FOP           |            0x1f |            0xac |

`m_descr->register_offset` is defined at function `init_regcache_descr` in `~/binutils-gdb/gdb/regcache.c` line 79.

### Where register offset in g packet is defined in?

Each register offset in g packet is defined at function `remote_target::process_g_packet` in `~/binutils-gdb/gdb/remote.c` line 8551.

```
~/hariboslinux/test_os # make debug
(gdb) break remote_target::process_g_packet
(gdb) continue
break at ~/binutils-gdb/gdb/remote.c : 8467
(gdb) continue
break at ~/binutils-gdb/gdb/remote.c : 8467
(gdb) break 8551
(gdb) continue
(gdb) p/x i
$0 = 0x0
(gdb) p/x rsa->regs[i].regnum
$1 = 0x0
(gdb) p/x rsa->regs[i].offset
$2 = 0x0
(gdb) continue
(gdb) p/x i
$3 = 0x1
(gdb) p/x rsa->regs[i].regnum
$4 = 0x1
(gdb) p/x rsa->regs[i].offset
$5 = 0x4
...
```

| Register | i    | rsa->regs[i].regnum | rsa->regs[i].offset|
| :------- | ---: | ------------------: | -----------------: |
| EAX      | 0x00 |                0x00 |               0x00 |
| ECX      | 0x01 |                0x01 |               0x04 |
| EDX      | 0x02 |                0x02 |               0x08 |
| EBX      | 0x03 |                0x03 |               0x0c |
| ESP      | 0x04 |                0x04 |               0x10 |
| EBP      | 0x05 |                0x05 |               0x14 |
| ESI      | 0x06 |                0x06 |               0x18 |
| EDI      | 0x07 |                0x07 |               0x1c |
| EIP      | 0x08 |                0x08 |               0x20 |
| EFLAGS   | 0x09 |                0x09 |               0x24 |
| CS       | 0x0a |                0x0a |               0x28 |
| SS       | 0x0b |                0x0b |               0x2c |
| DS       | 0x0c |                0x0c |               0x30 |
| ES       | 0x0d |                0x0d |               0x34 |
| FS       | 0x0e |                0x0e |               0x38 |
| GS       | 0x0f |                0x0f |               0x3c |
| ST0      | 0x10 |                0x10 |               0x64 |
| ST1      | 0x11 |                0x11 |               0x6e |
| ST2      | 0x12 |                0x12 |               0x78 |
| ST3      | 0x13 |                0x13 |               0x82 |
| ST4      | 0x14 |                0x14 |               0x8c |
| ST5      | 0x15 |                0x15 |               0x96 |
| ST6      | 0x16 |                0x16 |               0xa0 |
| ST7      | 0x17 |                0x17 |               0xaa |
| FCTRL    | 0x18 |                0x18 |               0xb4 |
| FSTAT    | 0x19 |                0x19 |               0xb8 |
| FTAG     | 0x1a |                0x1a |               0xbc |
| FISEG    | 0x1b |                0x1b |               0xc0 |
| FIOFF    | 0x1c |                0x1c |               0xc4 |
| FOSEG    | 0x1d |                0x1d |               0xc8 |
| FOOFF    | 0x1e |                0x1e |               0xcc |
| FOP      | 0x1f |                0x1f |               0xd0 |

Each register offset in g packet is defined at function `map_regcache_remote_table` in `~/binutils-gdb/gdb/remote.c` line from 1437 to 1442.

```
  for (regnum = 0, offset = 0; regnum < num_remote_regs; regnum++)
    {
      remote_regs[regnum]->in_g_packet = 1;
      remote_regs[regnum]->offset = offset;
      offset += register_size (gdbarch, remote_regs[regnum]->regnum);
    }
```

```
~/hariboslinux/test_os # make debug
(gdb) break map_regcache_remote_table
(gdb) continue
(gdb) continue
(gdb) break 1441
(gdb) continue
(gdb) p/x regnum
$0 = 0x0
(gdb) p/x remote_regs[regnum]->regnum
$1 = 0x0
(gdb) p/x remote_regs[regnum]->offset
$2 = 0x0
(gdb) p/x register_size(gdbarch, remote_regs[regnum]->regnum)
$3 = 0x4
...
```

| Register    | regnum | remote_regs[regnum]->regnum | remote_regs[regnum]->offset | register_size(gdbarch, remote_regs[regnum]->regnum) |
| :---------- | -----: | --------------------------: | --------------------------: | --------------------------------------------------: |
| EAX         |   0x00 |                        0x00 |                      0x0000 |                                                0x04 |
| ECX         |   0x01 |                        0x01 |                      0x0004 |                                                0x04 |
| EDX         |   0x02 |                        0x02 |                      0x0008 |                                                0x04 |
| EBX         |   0x03 |                        0x03 |                      0x000c |                                                0x04 |
| ESP         |   0x04 |                        0x04 |                      0x0010 |                                                0x04 |
| EBP         |   0x05 |                        0x05 |                      0x0014 |                                                0x04 |
| ESI         |   0x06 |                        0x06 |                      0x0018 |                                                0x04 |
| EDI         |   0x07 |                        0x07 |                      0x001c |                                                0x04 |
| EIP         |   0x08 |                        0x08 |                      0x0020 |                                                0x04 |
| EFLAGS      |   0x09 |                        0x09 |                      0x0024 |                                                0x04 |
| CS          |   0x0a |                        0x0a |                      0x0028 |                                                0x04 |
| SS          |   0x0b |                        0x0b |                      0x002c |                                                0x04 |
| DS          |   0x0c |                        0x0c |                      0x0030 |                                                0x04 |
| ES          |   0x0d |                        0x0d |                      0x0034 |                                                0x04 |
| FS          |   0x0e |                        0x0e |                      0x0038 |                                                0x04 |
| GS          |   0x0f |                        0x0f |                      0x003c |                                                0x04 |
| FS\_BASE    |   0x10 |                        0x49 |                      0x0040 |                                                0x04 |
| GS\_BASE    |   0x11 |                        0x4a |                      0x0044 |                                                0x04 |
| K\_GS\_BASE |   0x12 |                        0x4b |                      0x0048 |                                                0x04 |
| CR0         |   0x13 |                        0x4c |                      0x004c |                                                0x04 |
| CR2         |   0x14 |                        0x4d |                      0x0050 |                                                0x04 |
| CR3         |   0x15 |                        0x4e |                      0x0054 |                                                0x04 |
| CR4         |   0x16 |                        0x4f |                      0x0058 |                                                0x04 |
| CR8         |   0x17 |                        0x50 |                      0x005c |                                                0x04 |
| EFER        |   0x18 |                        0x51 |                      0x0060 |                                                0x04 |
| ST0         |   0x19 |                        0x10 |                      0x0064 |                                                0x0a |
| ST1         |   0x1a |                        0x11 |                      0x006e |                                                0x0a |
| ST2         |   0x1b |                        0x12 |                      0x0078 |                                                0x0a |
| ST3         |   0x1c |                        0x13 |                      0x0082 |                                                0x0a |
| ST4         |   0x1d |                        0x14 |                      0x008c |                                                0x0a |
| ST5         |   0x1e |                        0x15 |                      0x0096 |                                                0x0a |
| ST6         |   0x1f |                        0x16 |                      0x00a0 |                                                0x0a |
| ST7         |   0x20 |                        0x17 |                      0x00aa |                                                0x0a |
| FCTRL       |   0x21 |                        0x18 |                      0x00b4 |                                                0x04 |
| FSTAT       |   0x22 |                        0x19 |                      0x00b8 |                                                0x04 |
| FTAG        |   0x23 |                        0x1a |                      0x00bc |                                                0x04 |
| FISEG       |   0x24 |                        0x1b |                      0x00c0 |                                                0x04 |
| FIOFF       |   0x25 |                        0x1c |                      0x00c4 |                                                0x04 |
| FOSEG       |   0x26 |                        0x1d |                      0x00c8 |                                                0x04 |
| FOOFF       |   0x27 |                        0x1e |                      0x00cc |                                                0x04 |
| FOP         |   0x28 |                        0x1f |                      0x00d0 |                                                0x04 |
| XMM0        |   0x29 |                        0x52 |                      0x00d4 |                                                0x10 |
| XMM1        |   0x2a |                        0x53 |                      0x00e4 |                                                0x10 |
| XMM2        |   0x2b |                        0x54 |                      0x00f4 |                                                0x10 |
| XMM3        |   0x2c |                        0x55 |                      0x0104 |                                                0x10 |
| XMM4        |   0x2d |                        0x56 |                      0x0114 |                                                0x10 |
| XMM5        |   0x2e |                        0x57 |                      0x0124 |                                                0x10 |
| XMM6        |   0x2f |                        0x58 |                      0x0134 |                                                0x10 |
| XMM7        |   0x30 |                        0x59 |                      0x0144 |                                                0x10 |
| MXCSR       |   0x31 |                        0x5a |                      0x0154 |                                                0x04 |

### Where are g packet contents?

```
~/hariboslinux/test_os # make debug
(gdb) break remote_target::process_g_packet
(gdb) continue
(gdb) continue
(gdb) print this->m_remote_state.buf.data()
$1 = 0x556580e950a0 "55aa0000000000000000000000000000046f0000000000000000000000000000107c0000020200000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ac79cfd1f71772b1fe3f99f7cffb849a209afd3f35c26821a2da0fc90040bcf0175c293baab8ff3ffe8a1bcd4b789ad400400000000000000080ff3f7f030000000800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000801f0000"
```

# Cause investigation in QEMU

I think QEMU sends Rx registers pretended as STx on g packets to GDB.
As a result, GDB misunderstands received registers are STx which is actually Rx.

