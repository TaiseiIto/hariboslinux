# About this directory

This directory is related to the issue 105.
When I debug Haribote OS by GDB and print FPU registers, it's wrong.

```
Breakpoint 1, 0x00007c00 in ?? ()
(gdb) info float
  R7: Valid   0x0000037f000000000000 +9.182990457413707544e-4934 Denormal
  R6: Valid   0x00000000000000000000 +0
  R5: Valid   0x00000000000000000000 +0
  R4: Valid   0x00000000000000000000 +0
  R3: Valid   0x00000000000000000000 +0
  R2: Valid   0x00000000000000000000 +0
  R1: Valid   0x00000000000000000000 +0
=>R0: Valid   0x00000000000000000000 +0

Status Word:         0x0000
                       TOP: 0
Control Word:        0x0000
                       PC: Single Precision (24-bits)
                       RC: Round to nearest
Tag Word:            0x0000
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

But when I debug `fpu_test` in this directory and do same thing, it's right.

```
(gdb) break main
Breakpoint 1 at 0x1129: file fpu_test.s, line 4.
(gdb) run
Starting program: /root/hariboslinux/fpu_test/fpu_test
warning: Error disabling address space randomization: Operation not permitted
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, main () at fpu_test.s:4
4               movq    $0x000000000000003c,    %rax
(gdb) info float
  R7: Empty   0x00000000000000000000
  R6: Empty   0x00000000000000000000
  R5: Empty   0x00000000000000000000
  R4: Empty   0x00000000000000000000
  R3: Empty   0x00000000000000000000
  R2: Empty   0x00000000000000000000
  R1: Empty   0x00000000000000000000
=>R0: Empty   0x00000000000000000000

Status Word:         0x0000
                       TOP: 0
Control Word:        0x037f   IM DM ZM OM UM PM
                       PC: Extended Precision (64-bits)
                       RC: Round to nearest
Tag Word:            0xffff
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

To find the cause of the bug of GDB, I try to observe the process of gdb debugging `fpu_test` as bellow.

```
~/hariboslinux/fpu_test# gdb gdb
(gdb) break main
(gdb) run fpu_test < debuggee_input.txt
(gdb) break start_event_loop
(gdb) continue
Thread 1 "gdb" hit Breakpoint 2, 0x00005574c67e7b0f in start_event_loop () at main.c:428
428               gdb::observers::command_error.notify ();
(gdb)
```

# Findings

* There is a while loop statement that is the "event loop" in a function `start_event_loop` at `~/binutils-gdb/gdb/main.c`.
* A function `gdb_do_one_event` at `~/binutils-gdb/gdb/event-loop.cc` that process one event is called in the event loop.
* A function `i387_print_float_info` at `~/binutils-gdb/gdb/i387-tdep.c` prints FPU registers by gdb command `info float`.
* `i387_print_float_info` calls `print_i387_control_word` at `~/binutils-gdb/gdb/i387-tdep.c` to print control word register.
* `print_i387_control_word` calls `hex_string_custom` at `~/binutils-gdb/gdbsupport/print-utils.cc` to convert the control word `0x037f` given as the first argument into a hexadecimal string.
* The control word `0x037f` is stored to `fctrl` at `~/binutils-gdb/gdb/i387-tdep.c` line 229 `fctrl_p = read_frame_register_unsigned (frame, I387_FCTRL_REGNUM (tdep), &fctrl);` in a function `i387_print_float_info`.
* The control word `0x037f` is stored to `*val` at `~/binutils-gdb/gdb/frame.c` line 1365 `*val = extract_unsigned_integer (value_contents (regval).data (), size, byte_order);` in a function `read_frame_register_unsigned`.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1367
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x *val
$1 = 0x37f
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1367
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x *val
$1 = 0x0
```

* Moreover, `value_contents_writeable` returns the control value in `value.c` line 1418.

```
gdb::array_view<const gdb_byte> result = value_contents_writeable (value);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break value.c : 1419
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x *(unsigned int *)result->m_array
$1 = 0x37f
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1365
(gdb) run fpu_test < debuggee_input.txt
(gdb) break value.c : 1419
(gdb) continue
(gdb) p/x *(unsigned int *)result->m_array
$1 = 0x0
```

* `value_contents` in `~/binutils-gdb/gdb/value.c` line 1417 calls `value_contents_writeable` in `~/binutils-gdb/gdb/value.c` line 1427.
* `value_contents_writeable` in `~/binutils-gdb/gdb/value.c` line 1427 calls `value_contents_raw` in `~/binutils-gdb/gdb/value.c` line 1158.
* `value_contents_raw` in `~/binutils-gdb/gdb/value.c` line 1158 calls `make_array_view` in `~/binutils-gdb/gdbsupport/array-view.h` line 288.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1365
(gdb) run fpu_test < debuggee_input.txt
(gdb) break make_array_view
(gdb) continue
(gdb) p/x *(unsigned int *)array
$1 = 0x37f
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1365
(gdb) run fpu_test < debuggee_input.txt
(gdb) break make_array_view
(gdb) continue
(gdb) p/x *(unsigned int *)array
$1 = 0x0
```

* Above `array` is the first argument in `~/binutils-gdb/gdb/value.c` line 1164.

```
  return gdb::make_array_view
    (value->contents.get () + value->embedded_offset * unit_size, length);
```

* `value->embedded_offset` is 0.

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1365
(gdb) run fpu_test < debuggee_input.txt
(gdb) break value.c : 1164
(gdb) continue
(gdb) p/x *(unsigned int *)value->contents.get()
$1 = 0x37f
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1365
(gdb) run fpu_test < debuggee_input.txt
(gdb) break value.c : 1164
(gdb) continue
(gdb) p/x *(unsigned int *)value->contents.get()
$1 = 0x0
```

Above `value` is came from `regval` generated in `frame.c` line 1356.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1358
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x *(unsigned int*)regval->contents.get()
$1 = 0x37f
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1358
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x *(unsigned int*)regval->contents.get()
$1 = 0x0
```

* `get_frame_register_value` in `frame.c` line 1283 returns above `regval`.
* `get_frame_register_value` in `frame.c` line 1283 calls `frame_unwind_register_value` in `frame.c` line 1219.
* `frame_unwind_register_value` in `frame.c` line 1219 returns `value`, above `regval`, in `frame.c` line 1277.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_frame_register_value
(gdb) run fpu_test < debuggee_input.txt
(gdb) break frame.c : 1277
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x37f
```

And,

```
~/hariboslinux # make debug
(gdb) break get_frame_register_value
(gdb) run debuggee_input.txt
(gdb) break frame.c : 1277
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x0
```

Above `value` is stored in `frame.c` line 1233.

```
value *value = next_frame->unwind->prev_register (next_frame,
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_frame_register_value
(gdb) run fpu_test < debuggee_input.txt
(gdb) break frame.c : 1237
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x37f
```

And,

```
~/hariboslinux # make debug
(gdb) break get_frame_register_value
(gdb) run debuggee_input.txt
(gdb) break frame.c : 1237
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x0
```

* Above `value` is stored in `sentinel-frame.c` line 53.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_frame_register_value
(gdb) run fpu_test < debuggee_input.txt
(gdb) break sentinel-frame.c : 54
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x37f
```

And,

```
~/hariboslinux # make debug
(gdb) break get_frame_register_value
(gdb) run debuggee_input.txt
(gdb) break sentinel-frame.c : 54
(gdb) continue
(gdb) p/x *(unsinged int*)value->contents.get()
$1 = 0x0
```

Above `value` is `result` in `regcache.c` line 760.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_frame_register_value
(gdb) run fpu_test < debuggee_input.txt
(gdb) break regcache.c : 760
(gdb) continue
(gdb) p/x *(unsinged int*)result->contents.get()
$1 = 0x37f
```

And,

```
~/hariboslinux # make debug
(gdb) break get_frame_register_value
(gdb) run debuggee_input.txt
(gdb) break regcache.c : 760
(gdb) continue
(gdb) p/x *(unsinged int*)result->contents.get()
$1 = 0x0
```

The control value `0x037f` is written to `buf` in `regcache.c` line 613.

```
    memcpy (buf, register_buffer (regnum),
	    m_descr->sizeof_register[regnum]);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_frame_register_value
(gdb) run fpu_test < debuggee_input.txt
(gdb) break regcache.c : 613
(gdb) continue
(gdb) p/x *(unsinged int*)buf
$1 = 0x0
(gdb) next
(gdb) p/x *(unsinged int*)buf
$1 = 0x37f
```

And,

```
~/hariboslinux # gdb gdb
(gdb) break get_frame_register_value
(gdb) run < debuggee_input.txt
(gdb) break regcache.c : 613
(gdb) continue
(gdb) p/x *(unsinged int*)buf
$1 = 0x0
(gdb) next
(gdb) p/x *(unsinged int*)buf
$1 = 0x0
```

`reg_buffer::register_buffer` in `~/binutils-gdb/gdb/regcache.c` line 240 returns the address of the register specified by `regnum`.

```
gdb_byte *
reg_buffer::register_buffer (int regnum) const
{
  return m_registers.get () + m_descr->register_offset[regnum];
}
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break main
(gdb) run fpu_test < debuggee_input.txt
(gdb) break i387-tdep.c : 229
(gdb) continue
(gdb) break regcache.c : 613
(gdb) continue
(gdb) break reg_buffer::register_buffer
(gdb) continue
(gdb) p/x regnum
(gdb) p/x m_descr->register_offset[regnum]
(gdb) p/x *(unsigned int*)(m_registers.get()+m_descr->register_offset[regnum])
(gdb) x/40g m_registers.get()
```

And,

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) break i387-tdep.c : 229
(gdb) continue
(gdb) break regcache.c : 613
(gdb) continue
(gdb) break reg_buffer::register_buffer
(gdb) continue
(gdb) p/x regnum
(gdb) p/x m_descr->register_offset[regnum]
(gdb) p/x *(unsigned int*)(m_registers.get()+m_descr->register_offset[regnum])
(gdb) x/40g m_registers.get()
(gdb) p/x *(unsigned int*)(m_registers.get()+0x8c)
```

