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

Above `value` is come from `regval` generated in `frame.c` line 1356.

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

# I found the debuggee registers.

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
$1 = 0x20
(gdb) p/x m_descr->register_offset[regnum]
$2 = 0xf4
(gdb) p/x *(unsigned int*)(m_registers.get()+m_descr->register_offset[regnum])
$3 = 0x37f
(gdb) x/32g m_registers.get()
0x563230109990: 0x000056125b030129      0x0000000000000000
0x5632301099a0: 0x000056125b032df8      0x00007ffdd70e15d8
0x5632301099b0: 0x00007ffdd70e15c8      0x0000000000000001
0x5632301099c0: 0x0000000000000001      0x00007ffdd70e14b8
0x5632301099d0: 0x00007fd2c6173f10      0x00007fd2c618f040
0x5632301099e0: 0x00007fd2c6189908      0x00007fd2c61a4680
0x5632301099f0: 0x00007ffdd70e15c8      0x000056125b030129
0x563230109a00: 0x000056125b032df8      0x00007fd2c61c3040
0x563230109a10: 0x000056125b030129      0x0000003300000246
0x563230109a20: 0x000000000000002b      0x0000000000000000
0x563230109a30: 0x0000000000000000      0x0000000000000000
0x563230109a40: 0x0000000000000000      0x0000000000000000
0x563230109a50: 0x0000000000000000      0x0000000000000000
0x563230109a60: 0x0000000000000000      0x0000000000000000
0x563230109a70: 0x0000000000000000      0x0000000000000000
0x563230109a80: 0x0000037f00000000      0x0000ffff00000000
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
$1 = 0x18
(gdb) p/x m_descr->register_offset[regnum]
$2 = 0x90
(gdb) p/x *(unsigned int*)(m_registers.get()+m_descr->register_offset[regnum])
$3 = 0x0
(gdb) x/32g m_registers.get()
0x55e837f93de0: 0x000000000000aa55      0x0000000000000000
0x55e837f93df0: 0x0000000000006f00      0x0000000000000000
0x55e837f93e00: 0x0000020200007c00      0x0000000000000000
0x55e837f93e10: 0x0000000000000000      0x0000000000000000
0x55e837f93e20: 0x0000000000000000      0x0000000000000000
0x55e837f93e30: 0x0000000000000000      0x0000000000000000
0x55e837f93e40: 0x0000000000000000      0x0000000000000000
0x55e837f93e50: 0x0000000000000000      0x0000000000000000
0x55e837f93e60: 0x0000000000000000      0x0000037f00000000
0x55e837f93e70: 0x0000000000000000      0x0000000000000000
0x55e837f93e80: 0x0000000000000000      0x0000000000000000
0x55e837f93e90: 0x0000000000000000      0x0000001000000000
0x55e837f93ea0: 0x0000000000000000      0x0000000000000000
0x55e837f93eb0: 0x0000000000000000      0x0000000000000000
0x55e837f93ec0: 0x0000000000000000      0x0000000000000000
0x55e837f93ed0: 0x0000000000000000      0x0000000000000000
(gdb) p/x *(unsigned int*)(m_registers.get()+0x8c)
$4 = 0x37f
```

* This is debuggee registers in the GDB memory.
* `m_registers` is correct because GDB can print CPU registers correctly and there is a correct FPU control word `0x037f`.
* `regnum` is wrong.
* `m_descr->registers_offset[regnum]` is wrong because even if `regnum` is `0x20` then wrong FPU control word is got.

## What should I do next?

* I should research that ...
	* Why is `regnum` different?
	* Why is the offset of `0x037f` different?

## REGNUM (REGister NUMber) is different

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break i387-tdep.c : 229
(gdb) run fpu_test < debuggee_input.txt
(gdb) break read_frame_register_unsigned
(gdb) continue
(gdb) print regnum
32
```

```
~/hariboslinux # make debug
(gdb) break i387-tdep.c : 229
(gdb) run < debuggee_input.txt
(gdb) break read_frame_register_unsigned
(gdb) continue
(gdb) print regnum
24
```

## How is the REGNUM determined?

the REGNUM of FPU control register is given as `I387_FCTRL_REGNUM (tdep)` at line 229 of `~/binutils-gdb/gdb/i387-tdep.c`

```
  fctrl_p = read_frame_register_unsigned (frame, I387_FCTRL_REGNUM (tdep), &fctrl);
```

`I387_FCTRL_REGNUM` is defined at line 56 of `~/binutils-gdb/gdb/i387-tdep.h`.

```
#define I387_FCTRL_REGNUM(tdep) (I387_ST0_REGNUM (tdep) + 8)
```

And `I387_ST0_REGNUM` is defined at line 32 of the same header.

```
#define I387_ST0_REGNUM(tdep) ((tdep)->st0_regnum)
```

So, `I387_FCTRL_REGNUM(tdep)` is equal to `tdep->st0_regnum + 8`.

## `tdep->st0_regnum` is different

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break i387-tdep.c : 229
(gdb) run fpu_test < debuggee_input.txt
(gdb) print tdep->st0_regnum + 8
24
```

```
~/hariboslinux # make debug
(gdb) break i387-tdep.c : 229
(gdb) run < debuggee_input.txt
(gdb) print tdep->st0_regnum + 8
16
```

## What is `tdep`?

line 207 of `~/binutils-gdb/gdb/i387-tdep.c`

```
i386_gdbarch_tdep *tdep = gdbarch_tdep<i386_gdbarch_tdep>(gdbarch);
```

In other words,

```
i386_gdbarch_tdep *tdep = (i386_gdbarch_tdep *)(gdbarch->tdep);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break i387_print_float_info
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep *)gdbarch->tdep)->st0_regnum
24
```

```
~/hariboslinux # make debug
(gdb) break i387_print_float_info
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep *)gdbarch->tdep)->st0_regnum
16
```

## Where does `gdbarch` come from?

`~/binutils-gdb/gdb/infcmd.c` line 2908-2918.

```
static void
info_float_command (const char *args, int from_tty)
{
  struct frame_info *frame;

  if (!target_has_registers ())
    error (_("The program has no registers now."));

  frame = get_selected_frame (NULL);
  gdbarch_print_float_info (get_frame_arch (frame), gdb_stdout, frame, args);
}
```

* The `gdbarch` is given as `get_frame_arch(frame)` to `gdbarch_print_float_info`.
* Also, `get_frame_arch(frame)` is actually `frame->next->prev_arch.arch`.

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break infcmd.c : 2917
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break infcmd.c : 2917
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

## `get_selected_frame`

`~/binutils-gdb/gdb/frame.c` line 1802-1815.

```
struct frame_info *
get_selected_frame (const char *message)
{
  if (selected_frame == NULL)
    {
      if (message != NULL && !has_stack_frames ())
	error (("%s"), message);

      lookup_selected_frame (selected_frame_id, selected_frame_level);
    }
  /* There is always a frame.  */
  gdb_assert (selected_frame != NULL);
  return selected_frame;
}
```

* In the beginning of the function, `selected_frame` is `NULL`.
* `lookup_selected_frame` stores a valid address in a global variable `selected_frame` and return it as `frame`.

## `lookup_selected_frame`

`~/binutils-gdb/gdb/frame.c` line 1709-1722

```
void
lookup_selected_frame (struct frame_id a_frame_id, int frame_level)
{
  struct frame_info *frame = NULL;
  int count;

  /* This either means there was no selected frame, or the selected
     frame was the current frame.  In either case, select the current
     frame.  */
  if (frame_level == -1)
    {
      select_frame (get_current_frame ());
      return;
    }
...
}
```

* `select_frame` stores `get_current_frame()` to `selected_frame`

## `get_current_frame`

In `~/binutils-gdb/gdb/frame.c` line 1643, `get_prev_frame_always_1` outputs the frame.

```
  current_frame = get_prev_frame_always_1 (sentinel_frame);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_current_frame
(gdb) run fpu_test < debuggee_input.txt
(gdb) break 1644
(gdb) continue
(gdb) print ((i386_gdbarch_tdep*)current_frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break get_current_frame
(gdb) run < debuggee_input.txt
(gdb) break 1644
(gdb) continue
(gdb) print ((i386_gdbarch_tdep*)frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

## `get_prev_frame_always_1`

* There is the function `get_prev_frame_always_1` in `~/binutils-gdb/gdb/frame.c` line 2149-2297.
* In `~/binutils-gdb/gdb/frame.c` line 2296, `get_prev_frame_maybe_check_cycle` returns the frame.

```
return get_prev_frame_maybe_check_cycle (this_frame);
```

## `get_prev_frame_maybe_check_cycle`

* There is the function `get_prev_frame_maybe_check_cycle` in `~/binutils-gdb/gdb/frame.c` line 2061-2143.
* `get_prev_frame_raw` returns the frame in line 2064.

```
static struct frame_info *
get_prev_frame_maybe_check_cycle (struct frame_info *this_frame)
{
  struct frame_info *prev_frame = get_prev_frame_raw (this_frame);
...
  if (prev_frame->level == 0)
    return prev_frame;
...
}
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_prev_frame_maybe_check_cycle
(gdb) run fpu_test < debuggee_input.txt
(gdb) break 2075
(gdb) continue
(gdb) print ((i386_gdbarch_tdep*)prev_frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break get_prev_frame_maybe_check_cycle
(gdb) run < debuggee_input.txt
(gdb) break 2075
(gdb) continue
(gdb) print ((i386_gdbarch_tdep*)prev_frame->next->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

## `get_prev_frame_raw`

* There is the function `get_prev_frame_raw` in `~/binutils-gdb/gdb` line 2345-2394.
* The argument `this_frame` is stored in `prev_frame->next` in line 2389.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_prev_frame_raw
(gdb) run fpu_test < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break get_prev_frame_raw
(gdb) run < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

## Where does `this_frame` come from?

* `this_frame` comes from `this_frame`, the argument of `get_prev_frame_maybe_check_cycle`.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_prev_frame_maybe_check_cycle
(gdb) run fpu_test < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break get_prev_frame_maybe_check_cycle
(gdb) run < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

* Moreover, it comes from `get_prev_frame_always_1`
* By execution of `get_frame_arch` in `frame.c` line 2164 shown below, a valid address is stored to `this_frame->prev_arch.arch`.

```
struct gdbarch *gdbarch = get_frame_arch (this_frame);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 2164
(gdb) run fpu_test < debuggee_input.txt
(gdb) print this_frame->prev_arch.arch
$1 = (gdbarch*) 0x0
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$2 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 2164
(gdb) run < debuggee_input.txt
(gdb) print this_frame->prev_arch.arch
$1 = (gdbarch*) 0x0
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)this_frame->prev_arch.arch->tdep)->st0_regnum
$1 = 16
```

## `get_frame_arch`

* `get_frame_arch` in `frame.c` line 2164 is defined as `get_frame_arch` in `frame.c` line 2886.
* The function `get_frame_arch` calls `frame_unwind_arch` in `frame.c` line 2891.
* In the function `frame_unwind_arch`, in `frame.c` line 2900, `arch` is generated and finally its stored as `this_frame->prev_arch.arch`.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 2900
(gdb) run fpu_test < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 2900
(gdb) run < debuggee_input.txt
(gdb) next
(gdb) print ((i386_gdbarch_tdep*)arch->tdep)->st0_regnum
$1 = 16
```

## `sentinel_frame_prev_arch`

* A function `sentinel_frame_prev_arch` in `sentinel-frame.c` line 73 is called in `frame.c` line 2900.

```
static struct gdbarch *
sentinel_frame_prev_arch (struct frame_info *this_frame,
			  void **this_prologue_cache)
{
  struct frame_unwind_cache *cache
    = (struct frame_unwind_cache *) *this_prologue_cache;

  return cache->regcache->arch ();
}
```

## `reg_buffer::arch`

`cache->regcache->arch` in `~/binutils-gdb/gdb/sentinel-frame.c` line 77 is `reg_buffer::arch` in `~/binutils-gdb/gdb/regcache.c` line 229.

```
gdbarch *
reg_buffer::arch () const
{
  return m_descr->gdbarch;
}
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break regcache.c : 232
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)this->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break regcache.c : 232
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)this->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

Then, in the function `sentinel_frame_prev_arch` in `sentinel-frame.c` line 73, above `this` is `((struct frame_unwind_cache*)*this_prologue_cache)->regcache`.

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break sentinel_frame_prev_arch
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)*this_prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break sentinel_frame_prev_arch
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)*this_prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

Moreover, above `this_prologue_cache` is `&next_frame->prologue_cache` in `frame.c` line 2900.

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 2900
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)next_frame->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 2900
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)next_frame->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

Moreover, above `next_frame` is `this_frame->next` in `get_prev_frame_always_1` in `frame.c` line 2149.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break get_prev_frame_always_1
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)this_frame->next->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break get_prev_frame_always_1
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)this_frame->next->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

Moreover, above `this_frame` is `sentinel_frame` in `frame.c` line 1643

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1643
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)sentinel_frame->next->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1643
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)sentinel_frame->next->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

## `create_sentinel_frame`

Moreover, above `sentinel_frame->next->prologue` is generated in `frame.c` line 1576.

```
  frame->prologue_cache = sentinel_frame_cache (regcache);
```

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1578
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)frame->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1578
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)((struct frame_unwind_cache*)frame->prologue_cache)->regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

Moreover, above `((struct frame_unwind_cache*)frame->prologue_cache)->regcache` is the argument `regcache` of the function `create_sentinel_frame`.

So,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break create_sentinel_frame
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break create_sentinel_frame
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

## Where is `create_sentinel_frame` called?

The function `create_sentinel_frame` is called by `get_current_frame` in `frame.c` line 1629.

```
  if (sentinel_frame == NULL)
    sentinel_frame =
      create_sentinel_frame (current_program_space, get_current_regcache ());
```

So, `regcache` is return value of `get_current_regcache()`.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break frame.c : 1629
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)get_current_regcache()->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break frame.c : 1629
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)get_current_regcache()->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

* `get_current_regcache` in `frame.c` line 427 calls `get_thread_regcache` in `regcache.c` line 418.
* `get_thread_regcache` in `regcache.c` line 418 calls `get_thread_regcache` in `regcache.c` line 397.
* `get_thread_regcache` in `regcache.c` line 397 calls `get_thread_arch_regcache` in `regcache.c` line 381.
* `get_thread_arch_regcache` in `regcache.c` line 381 calls `get_thread_arch_aspace_regcache`. in `regcache.c` line 350.
* `get_thread_arch_aspace_regcache`. in `regcache.c` line 350 returns `new_regcache` at `redbache.c` line 378.

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break regcache.c : 378
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)new_regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break regcache.c : 378
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)new_regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

## Where is `new_regcache` created in?

Above `new_regcache` is created in `regcache.c` line 372.

```
  regcache *new_regcache = new regcache (target, arch, aspace);
```

Actually,

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break regcache.c : 373
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)new_regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break regcache.c : 373
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)new_regcache->m_descr->gdbarch->tdep)->st0_regnum
$1 = 16
```

And the second argument `arch` to `regcache` constructor in `regcache.c` line 372 is stored to `new_regcache->m_descr->gdbarch`.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break regcache.c : 372
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break regcache.c : 372
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)arch->tdep)->st0_regnum
$1 = 16
```

## Where does `arch` come from?

Above `arch` comes from the third argument `current_thread_arch` in `regcache.c` line 413.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break regcache.c : 413
(gdb) run fpu_test < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)current_thread_arch->tdep)->st0_regnum
$1 = 24
```

And

```
~/hariboslinux # make debug
(gdb) break regcache.c : 413
(gdb) run < debuggee_input.txt
(gdb) print ((i386_gdbarch_tdep*)current_thread_arch->tdep)->st0_regnum
$1 = 16
```

## Where does `current_thread_arch` come from?

Above `current_thread_arch` is come from `target_thread_architecture` in `target.c` line 437.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break target_thread_architecture
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)current_inferior()->top_target()->thread_architecture(ptid)->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break target_thread_architecture
(gdb) run < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)current_inferior()->top_target()->thread_architecture(ptid)->tdep)->st0_regnum
$1 = 0x10
```

Moreover, `current_inferior()->top_target()->thread_architecture` is `process_stratum_target::thread_architecture` in `process-stratum-target.c` line 47 and this function return `inf->gdbarch` in line 49.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break process-stratum-target.c : 47
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inf->gdbarch->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break process-stratum-target.c : 47
(gdb) run < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inf->gdbarch->tdep)->st0_regnum
$1 = 0x10
```

## Where does `inf` come from?

`find_inferior_pid` in `inferior.c` line 291 returns `inf` in line 299.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break inferior.c : 299
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inf->gdbarch->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break inferior.c : 299
(gdb) run < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inf->gdbarch->tdep)->st0_regnum
$1 = 0x10
```

Moreover, above `inf` is come from `all_inferiors`.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break find_inferior_pid
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)((inferior*)all_inferiors(targ).m_begin.m_inf_iter.m_elem)->gdbarch->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break find_inferior_pid
(gdb) run < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)((inferior*)all_inferiors(targ).m_begin.m_inf_iter.m_elem)->gdbarch->tdep)->st0_regnum
$1 = 0x10
```

Moreover, `(inferior*)all_inferiors(targ).m_begin.m_inf_iter.m_elem` is `inferior_list.m_frond`

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break find_inferior_pid
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break find_inferior_pid
(gdb) run < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
```

There is the `inferior_list` in `~/binutils-gdb/gdb/inferior.h` line 690.

## Watch `inferior_list`

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break main
(gdb) run fpu_test < debuggee_input.txt
(gdb) watch inferior_list
(gdb) continue
inferior_list.m_front is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 414
(gdb) print inferior_list.m_front->gdbarch
$1 = (gdbarch *) 0x0
(gdb) watch inferior_list.m_front->gdbarch
(gdb) continue
inferior_list.m_back is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 415
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x18
```

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) watch inferior_list
(gdb) continue
inferior_list.m_front is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 414
(gdb) print inferior_list.m_front->gdbarch
$1 = (gdbarch *) 0x0
(gdb) watch inferior_list.m_front->gdbarch
(gdb) continue
inferior_list.m_back is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 415
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
```

## Where does `gdbarch` come from?

`gdbarch` is created in `~/binutils-gdb/gdb/arch-utils.c` line 653 and stored into `inferior_list.m_front->gdbarch`.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break arch-utils.c : 655
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x ((i386_gdbarch_tdep*)gdbarch->tdep)->st0_regnum
$1 = 0x18
```

# I found the behavioral branch point between GDB debugging `hariboslinux` and it debugging `fpu_test`.

* They write `inferior_list.m_front->gdbarch` twice equally.
* But in the second writing of `inferior_list.m_front->gdbarch`, their backtrace is different.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break main
(gdb) run fpu_test < debuggee_input.txt
(gdb) watch inferior_list
(gdb) continue
inferior_list.m_front is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 414
(gdb) print inferior_list.m_front->gdbarch
$1 = (gdbarch *) 0x0
(gdb) watch inferior_list.m_front->gdbarch
(gdb) continue
inferior_list.m_back is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 415
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x18
(gdb) backtrace
#0  set_target_gdbarch (new_gdbarch=0x5581c1c63010) at arch-utils.c:1451
#1  0x00005581bf950927 in set_gdbarch_from_file (abfd=0x5581c1c61700) at arch-utils.c:657
#2  0x00005581bfbe6c0e in exec_file_attach (filename=0x7fffa03b08bd "fpu_test", from_tty=1) at exec.c:491
#3  0x00005581bfd0e4ef in catch_command_errors (command=0x5581bfbe66b5 <exec_file_attach(char const*, int)>,
		    arg=0x7fffa03b08bd "fpu_test", from_tty=1, do_bp_actions=false) at main.c:513
#4  0x00005581bfd0f81f in captured_main_1 (context=0x7fffa03af8c0) at main.c:1209
#5  0x00005581bfd0fef8 in captured_main (data=0x7fffa03af8c0) at main.c:1319
#6  0x00005581bfd0ff6f in gdb_main (args=0x7fffa03af8c0) at main.c:1344
#7  0x00005581bf8c4de6 in main (argc=2, argv=0x7fffa03af9f8) at gdb.c:32
```

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) watch inferior_list
(gdb) continue
inferior_list.m_front is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 414
(gdb) print inferior_list.m_front->gdbarch
$1 = (gdbarch *) 0x0
(gdb) watch inferior_list.m_front->gdbarch
(gdb) continue
inferior_list.m_back is written at ~/binutils-gdb/gdbsupport/intrusive_list.h : 415
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
inferior_list.m_front->gdbarch is written at ~/binutils-gdb/gdb/arch-utils.c : 1450
(gdb) p/x ((i386_gdbarch_tdep*)inferior_list.m_front->gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) backtrace
#0  set_target_gdbarch (new_gdbarch=0x561847a8e270) at arch-utils.c:1451
#1  0x000056184664882b in gdbarch_update_p (info=...) at arch-utils.c:625
#2  0x0000561846bc632a in target_find_description () at target-descriptions.c:569
#3  0x0000561846afc0ab in remote_target::start_remote_1 (this=0x561847a3a560, from_tty=1, extended_p=0)
    at remote.c:4833
#4  0x0000561846afcb98 in remote_target::start_remote (this=0x561847a3a560, from_tty=1, extended_p=0) at remote.c:5070
#5  0x0000561846afe3ad in remote_target::open_1 (name=0x5618479a7f7e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#6  0x0000561846afcc33 in remote_target::open (name=0x5618479a7f7e "localhost:2159", from_tty=1) at remote.c:5092
#7  0x0000561846bdefc2 in open_target (args=0x5618479a7f7e "localhost:2159", from_tty=1, command=0x561847a198d0)
	    at target.c:853
#8  0x00005618467388d7 in cmd_func (cmd=0x561847a198d0, args=0x5618479a7f7e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2516
#9  0x0000561846c0d734 in execute_command (p=0x5618479a7f8b "9", from_tty=1) at top.c:699
#10 0x00005618468db76e in command_handler (command=0x5618479a7f70 "") at event-top.c:598
#11 0x00005618468dbcac in command_line_handler (rl=...) at event-top.c:842
#12 0x00005618468dbe60 in gdb_readline_no_editing_callback (client_data=0x5618479ad560) at event-top.c:907
#13 0x00005618468db570 in stdin_event_handler (error=0, client_data=0x5618479ad560) at event-top.c:525
#14 0x0000561846e543c1 in handle_file_event (file_ptr=0x561847a83c80, ready_mask=1) at event-loop.cc:549
#15 0x0000561846e549b1 in gdb_wait_for_event (block=0) at event-loop.cc:670
#16 0x0000561846e53724 in gdb_do_one_event () at event-loop.cc:210
#17 0x0000561846a062f2 in start_event_loop () at main.c:411
#18 0x0000561846a0643f in captured_command_loop () at main.c:471
#19 0x0000561846a07efd in captured_main (data=0x7ffc07725960) at main.c:1329
#20 0x0000561846a07f6f in gdb_main (args=0x7ffc07725960) at main.c:1344
#21 0x00005618465bcde6 in main (argc=1, argv=0x7ffc07725a98) at gdb.c:32
```

## Where does `inferior_list.m_front->gdbarch` come from?

`inferior_list.m_front->gdbarch` is the first argument of `set_target_gdbarch` and it generated at `~/binutils-gdb/gdb/arch-urils.c` line 596.

```
~/hariboslinux # make debug
(gdb) break arch-utils.c : 599
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint
(gdb) continue
The second arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)->st0_regnum
$1 = 0x10
```

## Who generates `new_gdbarch`?

`gdbarch_find_by_info` at `~/binutils-gdb/gdb/arch-utils.c` line 1334 generates `new_gdbarch` and return at line 1440.

```
~/hariboslinux # make debug
(gdb) break arch-utils.c : 1440
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint
(gdb) continue
The second arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)st0_regnum
$1 = 0x10
```

Moreover, `new_gdbarch` is generated at `~/binutils-gdb/gdb/arch-utils.c` line 1375.

```
new_gdbarch = rego->init (info, rego->arches);
```

* Above `rego->init` is `i386_gdbarch_init` at `i386-tdep.c` line 8447.
* `i386_gdbarch_init` at `i386-tdep.c` line 8447 returns `gdbarch` at line 8803.

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8803
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint
(gdb) continue
The second arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)gdbarch->tdep)->st0_regnum
$1 = 0x10
```

# I found `st0_regnum` assignment.

`0x10` is stored to `st0_regnum` at `~/binutils-gdb/gdb/i386-tdep.c` line 8479.

```
  tdep->st0_regnum = I386_ST0_REGNUM;
```

And `I386_ST0_REGNUM` is defined as `0x10` at `~/binutils-gdb/gdb/i386-tdep.h` line 293.

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
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8465
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) watch tdep->st0_regnum
(gdb) continue
st0_regnum assignment is done.
(gdb) p/x tdep->st0_regnum
$1 = 0x10
```

## Why `st0_regnum` is `0x18` in case of GDB debugging `fpu_test`?

`new_gdbarch` is returned at `arch-utils.c` line 1440.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break arch-utils.c : 1440
(gdb) run fpu_test < debuggee_input.txt
The first arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
The secong arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)->st0_regnum
$1 = 0x18
```

And, `new_gdbarch` is stored at `arch-utils.c` line 1375.

```
new_gdbarch = rego->init (info, rego->arches);
```

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break arch-utils.c : 1379
(gdb) run fpu_test < debuggee_input.txt
The first arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
The secong arrival to the breakpoint
(gdb) p/x ((i386_gdbarch_tdep*)new_gdbarch->tdep)->st0_regnum
$1 = 0x18
```

Above `rego->init` is `i386_gdbarch_init` at `i386-tdep.c` line 8447.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break arch-utils.c : 1375
(gdb) run fpu_test < debuggee_input.txt
The first arrival to the breakpoint
(gdb) continue
The secong arrival to the breakpoint
(gdb) step
i386_gdbarch_init (info=..., arches=0x5558f5471fd0) at i386-tdep.c:8447
```

`i386_gdbarch_init` at `i386-tdep.c` line 8447 returns `gdbarch` at line 8803.

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break i386-tdep.c : 8803
(gdb) run fpu_test < debuggee_input.txt
(gdb) p/x((i386_gdbarch_tdep*)gdbarch->tdep)->st0_regnum
$1 = 0x10
(gdb) continue
(gdb) p/x((i386_gdbarch_tdep*)gdbarch->tdep)->st0_regnum
$2 = 0x18
```

## Watch `tdep->st0_regnum`

```
~/hariboslinux/fpu_test # gdb gdb
(gdb) break i386-tdep.c : 8465
(gdb) run fpu_test < debuggee_input.txt
(gdb) continue
(gdb) p/x tdep->st0_regnum
$1 = 0x0
(gdb) watch tdep->st0_regnum
(gdb) continue
~/binutils/gdb/i386-tdep.c : 8482
(gdb) p/x tdep->st0_regnum
$1 = 0x10
(gdb) continue
~/binutils/gdb/amd64-tdep.c : 3197
(gdb) p/x tdep->st0_regnum
$1 = 0x18
(gdb) backtrace
#0  amd64_init_abi (info=..., gdbarch=0x55cbb8e15ff0, default_tdesc=0x55cbb8e145b0) at amd64-tdep.c:3197
#1  0x000055cbb69ad95f in amd64_linux_init_abi (info=..., gdbarch=0x55cbb8e15ff0) at amd64-linux-tdep.c:1863
#2  0x000055cbb6dfea35 in gdbarch_init_osabi (info=..., gdbarch=0x55cbb8e15ff0) at osabi.c:395
#3  0x000055cbb6ccdc84 in i386_gdbarch_init (info=..., arches=0x0) at i386-tdep.c:8701
#4  0x000055cbb69d1c5a in gdbarch_find_by_info (info=...) at arch-utils.c:1375
#5  0x000055cbb69be8f0 in set_gdbarch_from_file (abfd=0x55cbb8e146e0) at arch-utils.c:653
#6  0x000055cbb6c54c0e in exec_file_attach (filename=0x7fff744b28ca "fpu_test", from_tty=1) at exec.c:491
#7  0x000055cbb6d7c4ef in catch_command_errors (command=0x55cbb6c546b5 <exec_file_attach(char const*, int)>,
		    arg=0x7fff744b28ca "fpu_test", from_tty=1, do_bp_actions=false) at main.c:513
#8  0x000055cbb6d7d81f in captured_main_1 (context=0x7fff744b17b0) at main.c:1209
#9  0x000055cbb6d7def8 in captured_main (data=0x7fff744b17b0) at main.c:1319
#10 0x000055cbb6d7df6f in gdb_main (args=0x7fff744b17b0) at main.c:1344
#11 0x000055cbb6932de6 in main (argc=2, argv=0x7fff744b18e8) at gdb.c:32
```

`AMD64_ST0_REGNUM` is stored to `tdep->st0_regnum` in `~/binutils/gdb/amd64-tdep.c` line 3196.

```
tdep->st0_regnum = AMD64_ST0_REGNUM;
```

And `AMD64_ST0_REGNUM` is defined at `~/binutils-gdb/gdb/amd64-tdep.h` line 59.

```
AMD64_ST0_REGNUM = 24,	/* %st0 */
```

# I think the `st0_regnum` difference between `i386` and `amd64` is not a problem because they have different register sets.

## I should analyse abound `regbuffer` and a map from `regnum` to `register_offset`

`~/binutils-gdb/gdb/regcache.c` line 240.

```
gdb_byte *
reg_buffer::register_buffer (int regnum) const
{
  return m_registers.get () + m_descr->register_offset[regnum];
}
```

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
$1 = 0x18
(gdb) p/x m_descr->register_offset[regnum]
$2 = 0x90
(gdb) p/x *(unsigned int*)(m_registers.get()+m_descr->register_offset[regnum])
$3 = 0x0
(gdb) x/32g m_registers.get()
0x55e837f93de0: 0x000000000000aa55      0x0000000000000000
0x55e837f93df0: 0x0000000000006f00      0x0000000000000000
0x55e837f93e00: 0x0000020200007c00      0x0000000000000000
0x55e837f93e10: 0x0000000000000000      0x0000000000000000
0x55e837f93e20: 0x0000000000000000      0x0000000000000000
0x55e837f93e30: 0x0000000000000000      0x0000000000000000
0x55e837f93e40: 0x0000000000000000      0x0000000000000000
0x55e837f93e50: 0x0000000000000000      0x0000000000000000
0x55e837f93e60: 0x0000000000000000      0x0000037f00000000
0x55e837f93e70: 0x0000000000000000      0x0000000000000000
0x55e837f93e80: 0x0000000000000000      0x0000000000000000
0x55e837f93e90: 0x0000000000000000      0x0000001000000000
0x55e837f93ea0: 0x0000000000000000      0x0000000000000000
0x55e837f93eb0: 0x0000000000000000      0x0000000000000000
0x55e837f93ec0: 0x0000000000000000      0x0000000000000000
0x55e837f93ed0: 0x0000000000000000      0x0000000000000000
(gdb) p/x *(unsigned int*)(m_registers.get()+0x8c)
$4 = 0x37f
```

* `m_descr->register_offset[regnum]` must be `0x8c` but it's `0x90`.
* `this` at this time is `cache->regcache` at `sentinel-frame.c` line 53.

```
value = cache->regcache->cooked_read_value (regnum);
```

Actually,

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) break i387-tdep.c : 229
(gdb) continue
(gdb) break sentinel-frame.c : 53
(gdb) continue
(gdb) p/x cache->regcache->m_descr->register_offset[regnum]
$1 = 0x90
(gdb) x/32g cache->regcache->m_registers.get()
0x56094e2d0dc0: 0x000000000000aa55      0x0000000000000000
0x56094e2d0dd0: 0x0000000000006f00      0x0000000000000000
0x56094e2d0de0: 0x0000020200007c00      0x0000000000000000
0x56094e2d0df0: 0x0000000000000000      0x0000000000000000
0x56094e2d0e00: 0x0000000000000000      0x0000000000000000
0x56094e2d0e10: 0x0000000000000000      0x0000000000000000
0x56094e2d0e20: 0x0000000000000000      0x0000000000000000
0x56094e2d0e30: 0x0000000000000000      0x0000000000000000
0x56094e2d0e40: 0x0000000000000000      0x0000037f00000000
0x56094e2d0e50: 0x0000000000000000      0x0000000000000000
0x56094e2d0e60: 0x0000000000000000      0x0000000000000000
0x56094e2d0e70: 0x0000000000000000      0x0000001000000000
0x56094e2d0e80: 0x0000000000000000      0x0000000000000000
0x56094e2d0e90: 0x0000000000000000      0x0000000000000000
0x56094e2d0ea0: 0x0000000000000000      0x0000000000000000
0x56094e2d0eb0: 0x0000000000000000      0x0000000000000000
```

Above `cache` is `(struct frame_unwind_cache*)next_frame->prologue_cache` at `frame.c` line 1233

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) break i387-tdep.c : 229
(gdb) continue
(gdb) break frame.c : 1233
(gdb) continue
(gdb) p/x ((struct frame_unwind_cache *)next_frame->prologue_cache)->regcache->m_descr->register_offset[regnum]
$1 = 0x90
(gdb) x/32f ((struct frame_unwind_cache *)next_frame->prologue_cache)->regcache->m_registers.get()
0x5619650e2dc0: 0x000000000000aa55      0x0000000000000000
0x5619650e2dd0: 0x0000000000006f00      0x0000000000000000
0x5619650e2de0: 0x0000020200007c00      0x0000000000000000
0x5619650e2df0: 0x0000000000000000      0x0000000000000000
0x5619650e2e00: 0x0000000000000000      0x0000000000000000
0x5619650e2e10: 0x0000000000000000      0x0000000000000000
0x5619650e2e20: 0x0000000000000000      0x0000000000000000
0x5619650e2e30: 0x0000000000000000      0x0000000000000000
0x5619650e2e40: 0x0000000000000000      0x0000037f00000000
0x5619650e2e50: 0x0000000000000000      0x0000000000000000
0x5619650e2e60: 0x0000000000000000      0x0000000000000000
0x5619650e2e70: 0x0000000000000000      0x0000001000000000
0x5619650e2e80: 0x0000000000000000      0x0000000000000000
0x5619650e2e90: 0x0000000000000000      0x0000000000000000
0x5619650e2ea0: 0x0000000000000000      0x0000000000000000
0x5619650e2eb0: 0x0000000000000000      0x0000000000000000
```

Above `next_frame` is `frame->next` at `frame.c` line 1283

```
~/hariboslinux # make debug
(gdb) break main
(gdb) run < debuggee_input.txt
(gdb) break i387-tdep.c : 229
(gdb) continue
(gdb) break frame.c : 1283
(gdb) continue
(gdb) p/x ((struct frame_unwind_cache *)frame->next->prologue_cache)->regcache->m_descr->register_offset[regnum]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache *)frame->next->prologue_cache)->regcache->m_registers.get()
0x55c6bed06dc0: 0x000000000000aa55      0x0000000000000000
0x55c6bed06dd0: 0x0000000000006f00      0x0000000000000000
0x55c6bed06de0: 0x0000020200007c00      0x0000000000000000
0x55c6bed06df0: 0x0000000000000000      0x0000000000000000
0x55c6bed06e00: 0x0000000000000000      0x0000000000000000
0x55c6bed06e10: 0x0000000000000000      0x0000000000000000
0x55c6bed06e20: 0x0000000000000000      0x0000000000000000
0x55c6bed06e30: 0x0000000000000000      0x0000000000000000
0x55c6bed06e40: 0x0000000000000000      0x0000037f00000000
0x55c6bed06e50: 0x0000000000000000      0x0000000000000000
0x55c6bed06e60: 0x0000000000000000      0x0000000000000000
0x55c6bed06e70: 0x0000000000000000      0x0000001000000000
0x55c6bed06e80: 0x0000000000000000      0x0000000000000000
0x55c6bed06e90: 0x0000000000000000      0x0000000000000000
0x55c6bed06ea0: 0x0000000000000000      0x0000000000000000
0x55c6bed06eb0: 0x0000000000000000      0x0000000000000000
```

Above `frame` is `frame` at `infcmg.c` line 2917.

```
~/hariboslinux # make debug
(gdb) break infcmd.c : 2917
(gdb) run < debuggee_input.txt
(gdb) p/x ((struct frame_unwind_cache *)frame->next->prologue_cache)->regcache->m_descr->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache *)frame->next->prologue_cache)->regcache->m_registers.get()
0x563dc7fa2de0: 0x000000000000aa55      0x0000000000000000
0x563dc7fa2df0: 0x0000000000006f00      0x0000000000000000
0x563dc7fa2e00: 0x0000020200007c00      0x0000000000000000
0x563dc7fa2e10: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e20: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e30: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e40: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e50: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e60: 0x0000000000000000      0x0000037f00000000
0x563dc7fa2e70: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e80: 0x0000000000000000      0x0000000000000000
0x563dc7fa2e90: 0x0000000000000000      0x0000001000000000
0x563dc7fa2ea0: 0x0000000000000000      0x0000000000000000
0x563dc7fa2eb0: 0x0000000000000000      0x0000000000000000
0x563dc7fa2ec0: 0x0000000000000000      0x0000000000000000
0x563dc7fa2ed0: 0x0000000000000000      0x0000000000000000
```

Above `frame` is a global variable `selected_frame` at `frame.c` line 1805.

```
~/hariboslinux # make debug
(gdb) break get_selected_frame
(gdb) run < debuggee_input.txt
(gdb) p/x ((struct frame_unwind_cache *)selected_frame->next->prologue_cache)->regcache->m_desct->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache *)selected_frame->next->prologue_cache)->regcache->m_registers.get()
0x560f954c8dc0: 0x0000000000000000      0x0000000000000663
0x560f954c8dd0: 0x0000000000000000      0x0000000000000000
0x560f954c8de0: 0x000000020000fff0      0x000000000000f000
0x560f954c8df0: 0x0000000000000000      0x0000000000000000
0x560f954c8e00: 0x0000000000000000      0x0000000000000000
0x560f954c8e10: 0x0000000000000000      0x0000000000000000
0x560f954c8e20: 0x0000000000000000      0x0000000000000000
0x560f954c8e30: 0x0000000000000000      0x0000000000000000
0x560f954c8e40: 0x0000000000000000      0x0000037f00000000
0x560f954c8e50: 0x0000000000000000      0x0000000000000000
0x560f954c8e60: 0x0000000000000000      0x0000000000000000
0x560f954c8e70: 0x0000000000000000      0x6000001000000000
0x560f954c8e80: 0x0000000000000000      0x0000000000000000
0x560f954c8e90: 0x0000000000000000      0x0000000000000000
0x560f954c8ea0: 0x0000000000000000      0x0000000000000000
0x560f954c8eb0: 0x0000000000000000      0x0000000000000000
```

# Watch `selected_frame`

```
~/hariboslinux # make debug
(gdb) watch selected_frame
(gdb) run < debuggee_input.txt
frame.c : 1835
(gdb) p/x ((struct frame_unwind_cache *)selected_frame->next->prologue_cache)->regcache->m_desct->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache *)selected_frame->next->prologue_cache)->regcache->m_registers.get()
0x5613f7db9dc0: 0x0000000000000000      0x0000000000000663
0x5613f7db9dd0: 0x0000000000000000      0x0000000000000000
0x5613f7db9de0: 0x000000020000fff0      0x000000000000f000
0x5613f7db9df0: 0x0000000000000000      0x0000000000000000
0x5613f7db9e00: 0x0000000000000000      0x0000000000000000
0x5613f7db9e10: 0x0000000000000000      0x0000000000000000
0x5613f7db9e20: 0x0000000000000000      0x0000000000000000
0x5613f7db9e30: 0x0000000000000000      0x0000000000000000
0x5613f7db9e40: 0x0000000000000000      0x0000037f00000000
0x5613f7db9e50: 0x0000000000000000      0x0000000000000000
0x5613f7db9e60: 0x0000000000000000      0x0000000000000000
0x5613f7db9e70: 0x0000000000000000      0x6000001000000000
0x5613f7db9e80: 0x0000000000000000      0x0000000000000000
0x5613f7db9e90: 0x0000000000000000      0x0000000000000000
0x5613f7db9ea0: 0x0000000000000000      0x0000000000000000
0x5613f7db9eb0: 0x0000000000000000      0x0000000000000000
```

# `prev_frame` is returned at `frame.c` line 2076.

```
~/hariboslinux # make debug
(gdb) break frame.c : 2076
(gdb) run < debuggee_input.txt
(gdb) p/x ((struct frame_unwind_cache*)prev_frame->next->prologue_cache)->regcache->m_descr->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache*)prev_frame->next->prologue_cache)->regcache->m_registers.get()
0x55a49472edc0: 0x0000000000000000      0x0000000000000663
0x55a49472edd0: 0x0000000000000000      0x0000000000000000
0x55a49472ede0: 0x000000020000fff0      0x000000000000f000
0x55a49472edf0: 0x0000000000000000      0x0000000000000000
0x55a49472ee00: 0x0000000000000000      0x0000000000000000
0x55a49472ee10: 0x0000000000000000      0x0000000000000000
0x55a49472ee20: 0x0000000000000000      0x0000000000000000
0x55a49472ee30: 0x0000000000000000      0x0000000000000000
0x55a49472ee40: 0x0000000000000000      0x0000037f00000000
0x55a49472ee50: 0x0000000000000000      0x0000000000000000
0x55a49472ee60: 0x0000000000000000      0x0000000000000000
0x55a49472ee70: 0x0000000000000000      0x6000001000000000
0x55a49472ee80: 0x0000000000000000      0x0000000000000000
0x55a49472ee90: 0x0000000000000000      0x0000000000000000
0x55a49472eea0: 0x0000000000000000      0x0000000000000000
0x55a49472eeb0: 0x0000000000000000      0x0000000000000000
```

Above `prev_frame->next` is `sentinel_frame` at `frame.c` line 1629.

```
~/hariboslinux # make debug
(gdb) break frame.c : 1643
(gdb) run < debuggee_input.txt
(gdb) p/x ((struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache->m_descr->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache->m_registers.get()
0x559471b37dc0: 0x0000000000000000      0x0000000000000663
0x559471b37dd0: 0x0000000000000000      0x0000000000000000
0x559471b37de0: 0x000000020000fff0      0x000000000000f000
0x559471b37df0: 0x0000000000000000      0x0000000000000000
0x559471b37e00: 0x0000000000000000      0x0000000000000000
0x559471b37e10: 0x0000000000000000      0x0000000000000000
0x559471b37e20: 0x0000000000000000      0x0000000000000000
0x559471b37e30: 0x0000000000000000      0x0000000000000000
0x559471b37e40: 0x0000000000000000      0x0000037f00000000
0x559471b37e50: 0x0000000000000000      0x0000000000000000
0x559471b37e60: 0x0000000000000000      0x0000000000000000
0x559471b37e70: 0x0000000000000000      0x6000001000000000
0x559471b37e80: 0x0000000000000000      0x0000000000000000
0x559471b37e90: 0x0000000000000000      0x0000000000000000
0x559471b37ea0: 0x0000000000000000      0x0000000000000000
0x559471b37eb0: 0x0000000000000000      0x0000000000000000
```

* Above `(struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache` is `new_regcache` at `regcache.c` line 378.
* The registers are not initialized yet.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 378
(gdb) run < debuggee_input.txt
(gdb) p/x new_regcache->m_descr->register_offset[0x18]
$1 = 0x90
(gdb) x/32g new_regcache->m_registers.get()
0x559d69cabdb0: 0x00005598301db89b      0x0000000000000000
0x559d69cabdc0: 0x0000000000000000      0x0000000000000000
0x559d69cabdd0: 0x0000559d69ca8f48      0x0000559d69cb6f50
0x559d69cabde0: 0x0000000000000006      0x0000000000000040
0x559d69cabdf0: 0x000000000000000c      0x0000559d69ca8f48
0x559d69cabe00: 0x0000559d69cb71b0      0x0000000000000006
0x559d69cabe10: 0x0000000000000040      0x000000000000000f
0x559d69cabe20: 0x0000559d69ca8f48      0x0000559d69cb38e0
0x559d69cabe30: 0x0000000000000006      0x0000000000000040
0x559d69cabe40: 0x0000000000000002      0x0000559d69ca8f48
0x559d69cabe50: 0x0000559d69cabf20      0x0000000000000000
0x559d69cabe60: 0x0000559d69cac32c      0x0000559d69cac04a
0x559d69cabe70: 0x0000559d69cac04a      0x0000559d69ca8f48
0x559d69cabe80: 0x0000000000000000      0x0000000000000000
0x559d69cabe90: 0x0000000000000000      0x0000000000000000
0x559d69cabea0: 0x0000000000000000      0x0000559d69ca8f48
```

Above `m_descr` is stored at `regcache.c` : 192.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 372
(gdb) run < debuggee_input.txt
(gdb) break regcache::regcache
(gdb) continue
(gdb) watch this->m_descr
(gdb) continue
regcache.c : 197
(gdb) p/x m_descr->register_offset[0x18]
$1 = 0x90
```

* `register_offset` is defined at `init_regcache_descr` in `regcache.c` line 79.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 138
(gdb) run < debuggee_input.txt
(gdb) p/x
(gdb) x/32g descr->register_offset
0x55be65dcb2c0: 0x0000000000000000      0x0000000000000004
0x55be65dcb2d0: 0x0000000000000008      0x000000000000000c
0x55be65dcb2e0: 0x0000000000000010      0x0000000000000014
0x55be65dcb2f0: 0x0000000000000018      0x000000000000001c
0x55be65dcb300: 0x0000000000000020      0x0000000000000024
0x55be65dcb310: 0x0000000000000028      0x000000000000002c
0x55be65dcb320: 0x0000000000000030      0x0000000000000034
0x55be65dcb330: 0x0000000000000038      0x000000000000003c
0x55be65dcb340: 0x0000000000000040      0x000000000000004a
0x55be65dcb350: 0x0000000000000054      0x000000000000005e
0x55be65dcb360: 0x0000000000000068      0x0000000000000072
0x55be65dcb370: 0x000000000000007c      0x0000000000000086
0x55be65dcb380: 0x0000000000000090      0x0000000000000094
0x55be65dcb390: 0x0000000000000098      0x000000000000009c
0x55be65dcb3a0: 0x00000000000000a0      0x00000000000000a4
0x55be65dcb3b0: 0x00000000000000a8      0x00000000000000ac
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
| R0            |            0x10 |            0x40 |
| R1            |            0x11 |            0x4a |
| R2            |            0x12 |            0x54 |
| R3            |            0x13 |            0x5e |
| R4            |            0x14 |            0x68 |
| R5            |            0x15 |            0x72 |
| R6            |            0x16 |            0x7c |
| R7            |            0x17 |            0x86 |
| FCTRL         |            0x18 |            0x90 |
| FSTAT         |            0x19 |            0x94 |
| FTAG          |            0x1a |            0x98 |
| FISEG         |            0x1b |            0x9c |
| FIOFF         |            0x1c |            0xa0 |
| FOSEG         |            0x1d |            0xa4 |
| FOOFF         |            0x1e |            0xa8 |
| FOP           |            0x1f |            0xac |

* I this there is no problem in the above table.
* So, I should observe writing to register cache.
* The register cache is generated in `frame.c` line 1629.

```
~/hariboslinux # make debug
(gdb) break frame.c : 1643
(gdb) run < debuggee_input.txt
(gdb) p/x ((struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache->m_descr->register_offset[0x18]
$1 = 0x90
(gdb) x/32g ((struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache->m_registers.get()
0x559471b37dc0: 0x0000000000000000      0x0000000000000663
0x559471b37dd0: 0x0000000000000000      0x0000000000000000
0x559471b37de0: 0x000000020000fff0      0x000000000000f000
0x559471b37df0: 0x0000000000000000      0x0000000000000000
0x559471b37e00: 0x0000000000000000      0x0000000000000000
0x559471b37e10: 0x0000000000000000      0x0000000000000000
0x559471b37e20: 0x0000000000000000      0x0000000000000000
0x559471b37e30: 0x0000000000000000      0x0000000000000000
0x559471b37e40: 0x0000000000000000      0x0000037f00000000
0x559471b37e50: 0x0000000000000000      0x0000000000000000
0x559471b37e60: 0x0000000000000000      0x0000000000000000
0x559471b37e70: 0x0000000000000000      0x6000001000000000
0x559471b37e80: 0x0000000000000000      0x0000000000000000
0x559471b37e90: 0x0000000000000000      0x0000000000000000
0x559471b37ea0: 0x0000000000000000      0x0000000000000000
0x559471b37eb0: 0x0000000000000000      0x0000000000000000
```

* `get_thread_arch_aspace_regcache` returns `(struct frame_unwind_cache*)sentinel_frame->prologue_cache)->regcache` in `~/binutils-gdb/gdb/regcache.c` line 351.
* First, the function returns uninitialized register cache at line 378.
* Second, the function returns initialized register cache at line 368.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 368
(gdb) break regcache.c : 378
(gdb) run < debuggee_input.txt
~/binutils-gdb/gdb/regcache.c : 378
(gdb) p/x
(gdb) x/32g new_regcache->m_registers.get()
0x559590cdadc0: 0x00005590c997189a      0x0000000000000000
0x559590cdadd0: 0x0000000000000000      0x0000000000000000
0x559590cdade0: 0x0000559590cd7f58      0x0000559590ce5f60
0x559590cdadf0: 0x0000000000000006      0x0000000000000040
0x559590cdae00: 0x000000000000000c      0x0000559590cd7f58
0x559590cdae10: 0x0000559590ce61c0      0x0000000000000006
0x559590cdae20: 0x0000000000000040      0x000000000000000f
0x559590cdae30: 0x0000559590cd7f58      0x0000559590ce28f0
0x559590cdae40: 0x0000000000000006      0x0000000000000040
0x559590cdae50: 0x0000000000000002      0x0000559590cd7f58
0x559590cdae60: 0x0000559590cdaf30      0x0000000000000000
0x559590cdae70: 0x0000559590cdb33c      0x0000559590cdb05a
0x559590cdae80: 0x0000559590cdb05a      0x0000559590cd7f58
0x559590cdae90: 0x0000000000000000      0x0000000000000000
0x559590cdaea0: 0x0000000000000000      0x0000000000000000
0x559590cdaeb0: 0x0000000000000000      0x0000559590cd7f58
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 368
(gdb) x/32g it->second.get()->m_registers.get()
0x559590cdadc0: 0x0000000000000000      0x0000000000000663
0x559590cdadd0: 0x0000000000000000      0x0000000000000000
0x559590cdade0: 0x000000020000fff0      0x000000000000f000
0x559590cdadf0: 0x0000000000000000      0x0000000000000000
0x559590cdae00: 0x0000000000000000      0x0000000000000000
0x559590cdae10: 0x0000000000000000      0x0000000000000000
0x559590cdae20: 0x0000000000000000      0x0000000000000000
0x559590cdae30: 0x0000000000000000      0x0000000000000000
0x559590cdae40: 0x0000000000000000      0x0000037f00000000
0x559590cdae50: 0x0000000000000000      0x0000000000000000
0x559590cdae60: 0x0000000000000000      0x0000000000000000
0x559590cdae70: 0x0000000000000000      0x6000001000000000
0x559590cdae80: 0x0000000000000000      0x0000000000000000
0x559590cdae90: 0x0000000000000000      0x0000000000000000
0x559590cdaea0: 0x0000000000000000      0x0000000000000000
0x559590cdaeb0: 0x0000000000000000      0x0000000000000000
```

Above `it` is `range.first` at `~/binutils-gdb/gdb/regcache.c` line 365.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 365
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint.
(gdb) continue
The second arrival to the breakpoint.
(gdb) p/x
(gdb) x/32g range.first->second.get()->m_registers.get()
0x55ebea6b5dc0: 0x0000000000000000      0x0000000000000663
0x55ebea6b5dd0: 0x0000000000000000      0x0000000000000000
0x55ebea6b5de0: 0x000000020000fff0      0x000000000000f000
0x55ebea6b5df0: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e00: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e10: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e20: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e30: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e40: 0x0000000000000000      0x0000037f00000000
0x55ebea6b5e50: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e60: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e70: 0x0000000000000000      0x6000001000000000
0x55ebea6b5e80: 0x0000000000000000      0x0000000000000000
0x55ebea6b5e90: 0x0000000000000000      0x0000000000000000
0x55ebea6b5ea0: 0x0000000000000000      0x0000000000000000
0x55ebea6b5eb0: 0x0000000000000000      0x0000000000000000
```

Above `range` is `ptid_regc_map.equal_range(ptid)` at `~/binutils-gdb/gdb/regcache.c` line 364.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 364
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) x/32gx ptid_regc_map.equal_range(ptid).first->second.get()->m_registers.get()
0x55aecc767dc0: 0x0000000000000000      0x0000000000000663
0x55aecc767dd0: 0x0000000000000000      0x0000000000000000
0x55aecc767de0: 0x000000020000fff0      0x000000000000f000
0x55aecc767df0: 0x0000000000000000      0x0000000000000000
0x55aecc767e00: 0x0000000000000000      0x0000000000000000
0x55aecc767e10: 0x0000000000000000      0x0000000000000000
0x55aecc767e20: 0x0000000000000000      0x0000000000000000
0x55aecc767e30: 0x0000000000000000      0x0000000000000000
0x55aecc767e40: 0x0000000000000000      0x0000037f00000000
0x55aecc767e50: 0x0000000000000000      0x0000000000000000
0x55aecc767e60: 0x0000000000000000      0x0000000000000000
0x55aecc767e70: 0x0000000000000000      0x6000001000000000
0x55aecc767e80: 0x0000000000000000      0x0000000000000000
0x55aecc767e90: 0x0000000000000000      0x0000000000000000
0x55aecc767ea0: 0x0000000000000000      0x0000000000000000
0x55aecc767eb0: 0x0000000000000000      0x0000000000000000
```

* Above `ptid_regc_map` is `pid_ptid_regc_map[ptid.pid ()]` at `~/binutils-gdb/gdb/regcache.c` line 361.
* But when I have tried to print `pid_ptid_regc_map[ptid.pid ()]`, An error has occured because `pid_ptid_regc_map` is a hash map implemented as C++ standard library.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 361
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) print pid_ptid_regc_map[1]
Attempt to take address of value not located in memory
```

* There are definitions of `pid_ptid_regc_map` and `ptid_regc_map` in `~/binutils-gdb/gdb/regcache.c` line 357.

```
  /* Find the map for this target.  */
  pid_ptid_regcache_map &pid_ptid_regc_map = regcaches[target];

  /* Find the map for this pid.  */
  ptid_regcache_map &ptid_regc_map = pid_ptid_regc_map[ptid.pid ()];
```

* And there are definitions of `ptid_regcache_map` and `pid_ptid_regcache_map` in `~/binutils-gdb/gdb/regcache.c` line 330.
* They are hash maps.

```
/* Type to map a ptid to a list of regcaches (one thread may have multiple
   regcaches, associated to different gdbarches).  */

using ptid_regcache_map
  = std::unordered_multimap<ptid_t, regcache_up, hash_ptid>;

/* Type holding regcaches for a given pid.  */

using pid_ptid_regcache_map = std::unordered_map<int, ptid_regcache_map>;
```

* They are got from a global variable `regcaches`
* Watch the glocal variable `regcaches`.

## I found `m_registers` initialization.

* A class `reg_buffer` is defined at `~/binutils-gdb/gdb/regcache.h` line 187.
* `m_registers` is defined at the same file line 264.
* `m_registers` is initialized at `~/binutils-gdb/gdb/regcache.c` line 205.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 205
(gdb) run < debuggee_input.c
The first arrival to the breapoint.
(gdb) continue
The second arrival to the breapoint.
(gdb) x/32gx m_registers.get()
0x0: Cannot access memory at address 0x0
(gdb) next
(gdb) x/32gx m_registers.get()
0x561caf8e3de0: 0x00005619ce445c83      0x0000000000000000
0x561caf8e3df0: 0x0000000000000000      0x0000000000000000
0x561caf8e3e00: 0x000000020000fff0      0x000000000000f000
0x561caf8e3e10: 0x0000000000000000      0x0000000000000000
0x561caf8e3e20: 0x0000000000000000      0x0000000000000000
0x561caf8e3e30: 0x0000000000000000      0x0000000000000000
0x561caf8e3e40: 0x0000000000000000      0x0000000000000000
0x561caf8e3e50: 0x0000000000000000      0x0000000000000000
0x561caf8e3e60: 0x0000000000000000      0x0000037f00000000
0x561caf8e3e70: 0x0000000000000000      0x0000000000000000
0x561caf8e3e80: 0x0000000000000000      0x0000000000000000
0x561caf8e3e90: 0x0000000000000000      0x6000001000000000
0x561caf8e3ea0: 0x0000000000000000      0x0000000000000000
0x561caf8e3eb0: 0x0000000000000000      0x0000000000000000
0x561caf8e3ec0: 0x0000000000000000      0x0000000000000000
0x561caf8e3ed0: 0x0000000000000000      0x0000000000000000
```

# I detected the debugger writing to EAX of the debuggee register cache.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 207
(gdb) run < debuggee_input.txt
(gdb) x/32gx m_registers.get()
0x557abc5badc0: 0x0000557febf7d1fa      0x0000000000000000
0x557abc5badd0: 0x0000000000000000      0x0000000000000000
0x557abc5bade0: 0x0000557abc5b7f58      0x0000557abc5c5f60
0x557abc5badf0: 0x0000000000000006      0x0000000000000040
0x557abc5bae00: 0x000000000000000c      0x0000557abc5b7f58
0x557abc5bae10: 0x0000557abc5c61c0      0x0000000000000006
0x557abc5bae20: 0x0000000000000040      0x000000000000000f
0x557abc5bae30: 0x0000557abc5b7f58      0x0000557abc5c28f0
0x557abc5bae40: 0x0000000000000006      0x0000000000000040
0x557abc5bae50: 0x0000000000000002      0x0000557abc5b7f58
0x557abc5bae60: 0x0000557abc5baf30      0x0000000000000000
0x557abc5bae70: 0x0000557abc5bb33c      0x0000557abc5bb05a
0x557abc5bae80: 0x0000557abc5bb05a      0x0000557abc5b7f58
0x557abc5bae90: 0x0000000000000000      0x0000000000000000
0x557abc5baea0: 0x0000000000000000      0x0000000000000000
0x557abc5baeb0: 0x0000000000000000      0x0000557abc5b7f58
(gdb) watch *(char*)0x557abc5badc0
(gdb) continue
(gdb) backtrace
#0  0x00007fc1c17ee99f in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#1  0x0000557aba305c4e in reg_buffer::raw_supply (this=0x557abc5ca8a0, regnum=0, buf=0x7ffdbadef870)
    at regcache.c:1067
#2  0x0000557aba32a3db in remote_target::process_g_packet (this=0x557abc56a980, regcache=0x557abc5ca8a0)
        at remote.c:8570
#3  0x0000557aba32a44d in remote_target::fetch_registers_using_g (this=0x557abc56a980, regcache=0x557abc5ca8a0)
	    at remote.c:8579
#4  0x0000557aba32a60b in remote_target::fetch_registers (this=0x557abc56a980, regcache=0x557abc5ca8a0, regnum=8)
	        at remote.c:8628
#5  0x0000557aba41b494 in target_fetch_registers (regcache=0x557abc5ca8a0, regno=8) at target.c:3948
#6  0x0000557aba304242 in regcache::raw_update (this=0x557abc5ca8a0, regnum=8) at regcache.c:594
#7  0x0000557aba3042fb in readable_regcache::raw_read (this=0x557abc5ca8a0, regnum=8,
		    buf=0x7ffdbadefc10 "`\374\336\272\375\177") at regcache.c:608
#8  0x0000557aba3046de in readable_regcache::cooked_read (this=0x557abc5ca8a0, regnum=8,
		    buf=0x7ffdbadefc10 "`\374\336\272\375\177") at regcache.c:697
#9  0x0000557aba30bb10 in readable_regcache::cooked_read<unsigned long, void> (this=0x557abc5ca8a0, regnum=8,
		    val=0x7ffdbadefce0) at regcache.c:782
#10 0x0000557aba304bed in regcache_cooked_read_unsigned (regcache=0x557abc5ca8a0, regnum=8, val=0x7ffdbadefce0)
    at regcache.c:796
#11 0x0000557aba3065e8 in regcache_read_pc (regcache=0x557abc5ca8a0) at regcache.c:1332
#12 0x0000557aba1a9441 in adjust_pc_after_break (thread=0x557abc5be300, ws=...) at infrun.c:4508
#13 0x0000557aba1ac105 in handle_inferior_event (ecs=0x7ffdbadeff80) at infrun.c:5407
#14 0x0000557aba1a7fa7 in wait_for_inferior (inf=0x557abc4dfb90) at infrun.c:4024
#15 0x0000557aba1a6195 in start_remote (from_tty=1) at infrun.c:3442
#16 0x0000557aba320735 in remote_target::start_remote_1 (this=0x557abc56a980, from_tty=1, extended_p=0)
        at remote.c:4979
#17 0x0000557aba320b7e in remote_target::start_remote (this=0x557abc56a980, from_tty=1, extended_p=0) at remote.c:5070
#18 0x0000557aba322393 in remote_target::open_1 (name=0x557abc4d7f7e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#19 0x0000557aba320c19 in remote_target::open (name=0x557abc4d7f7e "localhost:2159", from_tty=1) at remote.c:5092
#20 0x0000557aba40471c in open_target (args=0x557abc4d7f7e "localhost:2159", from_tty=1, command=0x557abc5498e0)
	        at target.c:853
#21 0x0000557ab9f5c895 in cmd_func (cmd=0x557abc5498e0, args=0x557abc4d7f7e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#22 0x0000557aba432e93 in execute_command (p=0x557abc4d7f8b "9", from_tty=1) at top.c:699
#23 0x0000557aba0ff74a in command_handler (command=0x557abc4d7f70 "") at event-top.c:598
#24 0x0000557aba0ffc88 in command_line_handler (rl=...) at event-top.c:842
#25 0x0000557aba0ffe3c in gdb_readline_no_editing_callback (client_data=0x557abc4dd560) at event-top.c:907
#26 0x0000557aba0ff54c in stdin_event_handler (error=0, client_data=0x557abc4dd560) at event-top.c:525
#27 0x0000557aba679607 in handle_file_event (file_ptr=0x557abc5b3c90, ready_mask=1) at event-loop.cc:574
#28 0x0000557aba679bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#29 0x0000557aba67889f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#30 0x0000557aba22a2d7 in start_event_loop () at main.c:411
#31 0x0000557aba22a424 in captured_command_loop () at main.c:471
#32 0x0000557aba22bee2 in captured_main (data=0x7ffdbadf08d0) at main.c:1329
#33 0x0000557aba22bf54 in gdb_main (args=0x7ffdbadf08d0) at main.c:1344
#34 0x0000557ab9de0de6 in main (argc=1, argv=0x7ffdbadf0a08) at gdb.c:32
```

* Watch *(short*)(m_registers.get()+0x90) where FCTRL should be.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 207
(gdb) run < debuggee_input.txt
(gdb) x/32gx m_registers.get()
0x5565bb8b6de0: 0x00005560edd06cd6      0x0000000000000000
0x5565bb8b6df0: 0x0000000000000000      0x0000000000000000
0x5565bb8b6e00: 0x00005565bb8b3f78      0x00005565bb8c1f80
0x5565bb8b6e10: 0x0000000000000006      0x0000000000000040
0x5565bb8b6e20: 0x000000000000000c      0x00005565bb8b3f78
0x5565bb8b6e30: 0x00005565bb8c21e0      0x0000000000000006
0x5565bb8b6e40: 0x0000000000000040      0x000000000000000f
0x5565bb8b6e50: 0x00005565bb8b3f78      0x00005565bb8be910
0x5565bb8b6e60: 0x0000000000000006      0x0000000000000040
0x5565bb8b6e70: 0x0000000000000002      0x00005565bb8b3f78
0x5565bb8b6e80: 0x00005565bb8b6f50      0x0000000000000000
0x5565bb8b6e90: 0x00005565bb8b735c      0x00005565bb8b707a
0x5565bb8b6ea0: 0x00005565bb8b707a      0x00005565bb8b3f78
0x5565bb8b6eb0: 0x0000000000000000      0x0000000000000000
0x5565bb8b6ec0: 0x0000000000000000      0x0000000000000000
0x5565bb8b6ed0: 0x0000000000000000      0x00005565bb8b3f78
(gdb) watch *(short*)(0x5565bb8b6de0+0x90)
(gdb) continue
(gdb) backtrace
#0  0x00007f531d63699f in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#1  0x00005565ba909c4e in reg_buffer::raw_supply (this=0x5565bb8c68c0, regnum=24, buf=0x7ffdc3e0dba8)
    at regcache.c:1067
#2  0x00005565ba92e3db in remote_target::process_g_packet (this=0x5565bb866980, regcache=0x5565bb8c68c0)
        at remote.c:8570
#3  0x00005565ba92e44d in remote_target::fetch_registers_using_g (this=0x5565bb866980, regcache=0x5565bb8c68c0)
	    at remote.c:8579
#4  0x00005565ba92e60b in remote_target::fetch_registers (this=0x5565bb866980, regcache=0x5565bb8c68c0, regnum=8)
	        at remote.c:8628
#5  0x00005565baa1f494 in target_fetch_registers (regcache=0x5565bb8c68c0, regno=8) at target.c:3948
#6  0x00005565ba908242 in regcache::raw_update (this=0x5565bb8c68c0, regnum=8) at regcache.c:594
#7  0x00005565ba9082fb in readable_regcache::raw_read (this=0x5565bb8c68c0, regnum=8,
		    buf=0x7ffdc3e0de90 "\340\336\340\303\375\177") at regcache.c:608
#8  0x00005565ba9086de in readable_regcache::cooked_read (this=0x5565bb8c68c0, regnum=8,
		    buf=0x7ffdc3e0de90 "\340\336\340\303\375\177") at regcache.c:697
#9  0x00005565ba90fb10 in readable_regcache::cooked_read<unsigned long, void> (this=0x5565bb8c68c0, regnum=8,
		    val=0x7ffdc3e0df60) at regcache.c:782
#10 0x00005565ba908bed in regcache_cooked_read_unsigned (regcache=0x5565bb8c68c0, regnum=8, val=0x7ffdc3e0df60)
    at regcache.c:796
#11 0x00005565ba90a5e8 in regcache_read_pc (regcache=0x5565bb8c68c0) at regcache.c:1332
#12 0x00005565ba7ad441 in adjust_pc_after_break (thread=0x5565bb8bf750, ws=...) at infrun.c:4508
#13 0x00005565ba7b0105 in handle_inferior_event (ecs=0x7ffdc3e0e200) at infrun.c:5407
#14 0x00005565ba7abfa7 in wait_for_inferior (inf=0x5565bb7dbb90) at infrun.c:4024
#15 0x00005565ba7aa195 in start_remote (from_tty=1) at infrun.c:3442
#16 0x00005565ba924735 in remote_target::start_remote_1 (this=0x5565bb866980, from_tty=1, extended_p=0)
        at remote.c:4979
#17 0x00005565ba924b7e in remote_target::start_remote (this=0x5565bb866980, from_tty=1, extended_p=0) at remote.c:5070
#18 0x00005565ba926393 in remote_target::open_1 (name=0x5565bb7d3f7e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#19 0x00005565ba924c19 in remote_target::open (name=0x5565bb7d3f7e "localhost:2159", from_tty=1) at remote.c:5092
#20 0x00005565baa0871c in open_target (args=0x5565bb7d3f7e "localhost:2159", from_tty=1, command=0x5565bb8458e0)
	        at target.c:853
#21 0x00005565ba560895 in cmd_func (cmd=0x5565bb8458e0, args=0x5565bb7d3f7e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#22 0x00005565baa36e93 in execute_command (p=0x5565bb7d3f8b "9", from_tty=1) at top.c:699
#23 0x00005565ba70374a in command_handler (command=0x5565bb7d3f70 "") at event-top.c:598
#24 0x00005565ba703c88 in command_line_handler (rl=...) at event-top.c:842
#25 0x00005565ba703e3c in gdb_readline_no_editing_callback (client_data=0x5565bb7d9560) at event-top.c:907
#26 0x00005565ba70354c in stdin_event_handler (error=0, client_data=0x5565bb7d9560) at event-top.c:525
#27 0x00005565bac7d607 in handle_file_event (file_ptr=0x5565bb8afcb0, ready_mask=1) at event-loop.cc:574
#28 0x00005565bac7dbf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#29 0x00005565bac7c89f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#30 0x00005565ba82e2d7 in start_event_loop () at main.c:411
#31 0x00005565ba82e424 in captured_command_loop () at main.c:471
#32 0x00005565ba82fee2 in captured_main (data=0x7ffdc3e0eb50) at main.c:1329
#33 0x00005565ba82ff54 in gdb_main (args=0x7ffdc3e0eb50) at main.c:1344
#34 0x00005565ba3e4de6 in main (argc=1, argv=0x7ffdc3e0ec88) at gdb.c:32
```

* Watch *(short*)(m_registers.get()+0x8c) where FCTRL actually is.

```
~/hariboslinux # make debug
(gdb) break regcache.c : 207
(gdb) run < debuggee_input.txt
(gdb) x/32gx m_registers.get()
0x55801cdb9de0: 0x0000558544ddc9d9      0x0000000000000000
0x55801cdb9df0: 0x0000000000000000      0x0000000000000000
0x55801cdb9e00: 0x000055801cdb6f78      0x000055801cdc4f80
0x55801cdb9e10: 0x0000000000000006      0x0000000000000040
0x55801cdb9e20: 0x000000000000000c      0x000055801cdb6f78
0x55801cdb9e30: 0x000055801cdc51e0      0x0000000000000006
0x55801cdb9e40: 0x0000000000000040      0x000000000000000f
0x55801cdb9e50: 0x000055801cdb6f78      0x000055801cdc1910
0x55801cdb9e60: 0x0000000000000006      0x0000000000000040
0x55801cdb9e70: 0x0000000000000002      0x000055801cdb6f78
0x55801cdb9e80: 0x000055801cdb9f50      0x0000000000000000
0x55801cdb9e90: 0x000055801cdba35c      0x000055801cdba07a
0x55801cdb9ea0: 0x000055801cdba07a      0x000055801cdb6f78
0x55801cdb9eb0: 0x0000000000000000      0x0000000000000000
0x55801cdb9ec0: 0x0000000000000000      0x0000000000000000
0x55801cdb9ed0: 0x0000000000000000      0x000055801cdb6f78
(gdb) watch *(short*)(0x55801cdb9de0+0x8c)
(gdb) continue
(gdb) backtrace
#0  0x00007f672d2589cb in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#1  0x000055801ba35c4e in reg_buffer::raw_supply (this=0x55801cdc98c0, regnum=23, buf=0x7ffd232e062e)
    at regcache.c:1067
#2  0x000055801ba5a3db in remote_target::process_g_packet (this=0x55801cd69980, regcache=0x55801cdc98c0)
        at remote.c:8570
#3  0x000055801ba5a44d in remote_target::fetch_registers_using_g (this=0x55801cd69980, regcache=0x55801cdc98c0)
	    at remote.c:8579
#4  0x000055801ba5a60b in remote_target::fetch_registers (this=0x55801cd69980, regcache=0x55801cdc98c0, regnum=8)
	        at remote.c:8628
#5  0x000055801bb4b494 in target_fetch_registers (regcache=0x55801cdc98c0, regno=8) at target.c:3948
#6  0x000055801ba34242 in regcache::raw_update (this=0x55801cdc98c0, regnum=8) at regcache.c:594
#7  0x000055801ba342fb in readable_regcache::raw_read (this=0x55801cdc98c0, regnum=8,
		    buf=0x7ffd232e0920 "p\t.#\375\177") at regcache.c:608
#8  0x000055801ba346de in readable_regcache::cooked_read (this=0x55801cdc98c0, regnum=8,
		    buf=0x7ffd232e0920 "p\t.#\375\177") at regcache.c:697
#9  0x000055801ba3bb10 in readable_regcache::cooked_read<unsigned long, void> (this=0x55801cdc98c0, regnum=8,
		    val=0x7ffd232e09f0) at regcache.c:782
#10 0x000055801ba34bed in regcache_cooked_read_unsigned (regcache=0x55801cdc98c0, regnum=8, val=0x7ffd232e09f0)
    at regcache.c:796
#11 0x000055801ba365e8 in regcache_read_pc (regcache=0x55801cdc98c0) at regcache.c:1332
#12 0x000055801b8d9441 in adjust_pc_after_break (thread=0x55801cdc2750, ws=...) at infrun.c:4508
#13 0x000055801b8dc105 in handle_inferior_event (ecs=0x7ffd232e0c90) at infrun.c:5407
#14 0x000055801b8d7fa7 in wait_for_inferior (inf=0x55801ccdeb90) at infrun.c:4024
#15 0x000055801b8d6195 in start_remote (from_tty=1) at infrun.c:3442
#16 0x000055801ba50735 in remote_target::start_remote_1 (this=0x55801cd69980, from_tty=1, extended_p=0)
        at remote.c:4979
#17 0x000055801ba50b7e in remote_target::start_remote (this=0x55801cd69980, from_tty=1, extended_p=0) at remote.c:5070
#18 0x000055801ba52393 in remote_target::open_1 (name=0x55801ccd6f7e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#19 0x000055801ba50c19 in remote_target::open (name=0x55801ccd6f7e "localhost:2159", from_tty=1) at remote.c:5092
#20 0x000055801bb3471c in open_target (args=0x55801ccd6f7e "localhost:2159", from_tty=1, command=0x55801cd488e0)
	        at target.c:853
#21 0x000055801b68c895 in cmd_func (cmd=0x55801cd488e0, args=0x55801ccd6f7e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#22 0x000055801bb62e93 in execute_command (p=0x55801ccd6f8b "9", from_tty=1) at top.c:699
#23 0x000055801b82f74a in command_handler (command=0x55801ccd6f70 "") at event-top.c:598
#24 0x000055801b82fc88 in command_line_handler (rl=...) at event-top.c:842
#25 0x000055801b82fe3c in gdb_readline_no_editing_callback (client_data=0x55801ccdc560) at event-top.c:907
#26 0x000055801b82f54c in stdin_event_handler (error=0, client_data=0x55801ccdc560) at event-top.c:525
#27 0x000055801bda9607 in handle_file_event (file_ptr=0x55801cdb2cb0, ready_mask=1) at event-loop.cc:574
#28 0x000055801bda9bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#29 0x000055801bda889f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#30 0x000055801b95a2d7 in start_event_loop () at main.c:411
#31 0x000055801b95a424 in captured_command_loop () at main.c:471
#32 0x000055801b95bee2 in captured_main (data=0x7ffd232e15e0) at main.c:1329
#33 0x000055801b95bf54 in gdb_main (args=0x7ffd232e15e0) at main.c:1344
#34 0x000055801b510de6 in main (argc=1, argv=0x7ffd232e1718) at gdb.c:32
```

`~/binutils-gdb/gdb/regcache.c` line 1067 writes registers.

```
      memcpy (regbuf, buf, size);
```

* Where does the above `buf` come from?
* The above `buf` is `regs+r->offset` at `~/binutils-gdb/gdb/remote.c` line 8570.

```
~/hariboslinux # make debug
(gdb) break remote.c : 8570 if r->regnum == 0x17
(gdb) run < debuggee_input.txt
(gdb) p/x r->regnum
$1 = 0x17
(gdb) p/x r->offset
$2 = 0xae
(gdb) x/gx regs+r->offset
0x7ffe371732be: 0x037f000000000000
```

* `rsa->regs` at `~/binutils-gdb/gdb/remote.c` line 8551.

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
| R0       | 0x10 |                0x10 |               0x68 |
| R1       | 0x11 |                0x11 |               0x72 |
| R2       | 0x12 |                0x12 |               0x7c |
| R3       | 0x13 |                0x13 |               0x86 |
| R4       | 0x14 |                0x14 |               0x90 |
| R5       | 0x15 |                0x15 |               0x9a |
| R6       | 0x16 |                0x16 |               0xa4 |
| R7       | 0x17 |                0x17 |               0xae |
| FCTRL    | 0x18 |                0x18 |               0xb8 |
| FSTAT    | 0x19 |                0x19 |               0xbc |
| FTAG     | 0x1a |                0x1a |               0xc0 |
| FISEG    | 0x1b |                0x1b |               0xc4 |
| FIOFF    | 0x1c |                0x1c |               0xc8 |
| FOSEG    | 0x1d |                0x1d |               0xcc |
| FOOFF    | 0x1e |                0x1e |               0xd0 |
| FOP      | 0x1f |                0x1f |               0xd4 |

At `~/binutils-gdb/gdb/remote.c` line 8570 I should research

* Where does the register values `regs` come from?
* Where does the register offsets `r->offset` come from?

# Where does the register values `regs` come from?

It comes from `p`, hexadecimal represented as string,

```
~/hariboslinux # make debug
(gdb) break remote.c : 8537
(gdb) run < debuggee_input.txt
(gdb) set print elements 0
(gdb) print p
$1 = 0x558369086260 '0' <repeats 16 times>, "6306", '0' <repeats 44 times>, "f0ff00000200000000f", '0' <repeats 69 times>, "1000006", '0' <repeats 201 times>, "7f03", '0' <repeats 316 times>, "801f0000"
```

`p` comes from `rs`


```
~/hariboslinux # make debug
(gdb) break remote.c : 8474
(gdb) run < debuggee_input.txt
(gdb) set print elements 0
(gdb) print rs->buf.data()
$1 = 0x55ae87435280 '0' <repeats 16 times>, "6306", '0' <repeats 44 times>, "f0ff00000200000000f", '0' <repeats 69 times>, "1000006", '0' <repeats 201 times>, "7f03", '0' <repeats 316 times>, "801f0000"
```

`get_remote_state` at `remote.c` line 1353 function returns `rs`.

```
~/hariboslinux # make debug
(gdb) break remote.c : 8473
(gdb) run < debuggee_input.txt
(gdb) step
(gdb) next
(gdb) print m_remote_state.buf.data()
$1 = 0x55d2398bc3f0 '0' <repeats 16 times>, "6306", '0' <repeats 44 times>, "f0ff00000200000000f", '0' <repeats 69 times>, "1000006", '0' <repeats 201 times>, "7f03", '0' <repeats 316 times>, "801f0000"
```

`rs` is `&this->m_remote_state`

```
~/hariboslinux # make debug
(gdb) break remote_target::process_g_packet
(gdb) run < debuggee_input.txt
(gdb) print this->m_remote_state.buf.data()
$1 = 0x55ba5427a3f0 '0' <repeats 16 times>, "6306", '0' <repeats 44 times>, "f0ff00000200000000f", '0' <repeats 69 times>, "1000006", '0' <repeats 201 times>, "7f03", '0' <repeats 316 times>, "801f0000"
(gdb) backtrace
#0  remote_target::process_g_packet (this=0x557f9c813980, regcache=0x557f9c873990) at remote.c:8471
#1  0x0000557f9a38844d in remote_target::fetch_registers_using_g (this=0x557f9c813980, regcache=0x557f9c873990)
    at remote.c:8579
#2  0x0000557f9a38860b in remote_target::fetch_registers (this=0x557f9c813980, regcache=0x557f9c873990, regnum=8)
        at remote.c:8628
#3  0x0000557f9a479494 in target_fetch_registers (regcache=0x557f9c873990, regno=8) at target.c:3948
#4  0x0000557f9a362242 in regcache::raw_update (this=0x557f9c873990, regnum=8) at regcache.c:594
#5  0x0000557f9a3622fb in readable_regcache::raw_read (this=0x557f9c873990, regnum=8,
		    buf=0x7ffc7339bb00 "P\273\071s\374\177") at regcache.c:608
#6  0x0000557f9a3626de in readable_regcache::cooked_read (this=0x557f9c873990, regnum=8,
		    buf=0x7ffc7339bb00 "P\273\071s\374\177") at regcache.c:697
#7  0x0000557f9a369b10 in readable_regcache::cooked_read<unsigned long, void> (this=0x557f9c873990, regnum=8,
		    val=0x7ffc7339bbd0) at regcache.c:782
#8  0x0000557f9a362bed in regcache_cooked_read_unsigned (regcache=0x557f9c873990, regnum=8, val=0x7ffc7339bbd0)
    at regcache.c:796
#9  0x0000557f9a3645e8 in regcache_read_pc (regcache=0x557f9c873990) at regcache.c:1332
#10 0x0000557f9a207441 in adjust_pc_after_break (thread=0x557f9c86c7f0, ws=...) at infrun.c:4508
#11 0x0000557f9a20a105 in handle_inferior_event (ecs=0x7ffc7339be70) at infrun.c:5407
#12 0x0000557f9a205fa7 in wait_for_inferior (inf=0x557f9c788b90) at infrun.c:4024
#13 0x0000557f9a204195 in start_remote (from_tty=1) at infrun.c:3442
#14 0x0000557f9a37e735 in remote_target::start_remote_1 (this=0x557f9c813980, from_tty=1, extended_p=0)
        at remote.c:4979
#15 0x0000557f9a37eb7e in remote_target::start_remote (this=0x557f9c813980, from_tty=1, extended_p=0) at remote.c:5070
#16 0x0000557f9a380393 in remote_target::open_1 (name=0x557f9c780f4e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#17 0x0000557f9a37ec19 in remote_target::open (name=0x557f9c780f4e "localhost:2159", from_tty=1) at remote.c:5092
#18 0x0000557f9a46271c in open_target (args=0x557f9c780f4e "localhost:2159", from_tty=1, command=0x557f9c7f28e0)
	        at target.c:853
#19 0x0000557f99fba895 in cmd_func (cmd=0x557f9c7f28e0, args=0x557f9c780f4e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#20 0x0000557f9a490e93 in execute_command (p=0x557f9c780f5b "9", from_tty=1) at top.c:699
#21 0x0000557f9a15d74a in command_handler (command=0x557f9c780f40 "") at event-top.c:598
#22 0x0000557f9a15dc88 in command_line_handler (rl=...) at event-top.c:842
#23 0x0000557f9a15de3c in gdb_readline_no_editing_callback (client_data=0x557f9c786560) at event-top.c:907
#24 0x0000557f9a15d54c in stdin_event_handler (error=0, client_data=0x557f9c786560) at event-top.c:525
#25 0x0000557f9a6d7607 in handle_file_event (file_ptr=0x557f9c85cc90, ready_mask=1) at event-loop.cc:574
#26 0x0000557f9a6d7bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#27 0x0000557f9a6d689f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#28 0x0000557f9a2882d7 in start_event_loop () at main.c:411
#29 0x0000557f9a288424 in captured_command_loop () at main.c:471
#30 0x0000557f9a289ee2 in captured_main (data=0x7ffc7339c7c0) at main.c:1329
#31 0x0000557f9a289f54 in gdb_main (args=0x7ffc7339c7c0) at main.c:1344
#32 0x0000557f99e3ede6 in main (argc=1, argv=0x7ffc7339c8f8) at gdb.c:32
```

* `remote.c` line 8443 writes `this->m_remote_state.buf.data()`

```
~/hariboslinux # make debug
(gdb) break remote.c : 8443
(gdb) run < debuggee_input.txt
(gdb) print this->m_remote_state.buf.data()
$1 = 0x55925c1583f0 "g"
(gdb) next
(gdb) print this->m_remote_state.buf.data()
$2 = 0x55925c1583f0 '0' <repeats 16 times>, "6306", '0' <repeats 44 times>, "f0ff00000200000000f", '0' <repeats 69 times>, "1000006", '0' <repeats 201 times>, "7f03", '0' <repeats 316 times>, "801f0000"
```

* watch `this->m_remote_state.buf.data()`

```
~/hariboslinux # make debug
(gdb) break remote_target::send_g_packet
(gdb) run < debuggee_input.txt
(gdb) print this->m_remote_state.buf.data()
$1 = 0x55799f7fe3f0 "OK"
(gdb) watch *(char*)(0x55799f7fe3f0+0x200)
(gdb) continue
remote_target::read_frame (this=0x55799f7ac980, buf_p=0x55799f7ac9a8) at remote.c:9857
(gdb) backtrace
#0  remote_target::read_frame (this=0x55799f7ac980, buf_p=0x55799f7ac9a8) at remote.c:9857
#1  0x000055799eaa14fd in remote_target::getpkt_or_notif_sane_1 (this=0x55799f7ac980, buf=0x55799f7ac9a8, forever=0,
		    expecting_notif=0, is_notif=0x0) at remote.c:9969
#2  0x000055799eaa1853 in remote_target::getpkt_sane (this=0x55799f7ac980, buf=0x55799f7ac9a8, forever=0)
    at remote.c:10048
#3  0x000055799eaa139f in remote_target::getpkt (this=0x55799f7ac980, buf=0x55799f7ac9a8, forever=0) at remote.c:9890
#4  0x000055799ea9dcc4 in remote_target::send_g_packet (this=0x55799f7ac980) at remote.c:8443
#5  0x000055799ea9e43a in remote_target::fetch_registers_using_g (this=0x55799f7ac980, regcache=0x55799f80d960)
        at remote.c:8578
#6  0x000055799ea9e60b in remote_target::fetch_registers (this=0x55799f7ac980, regcache=0x55799f80d960, regnum=8)
	    at remote.c:8628
#7  0x000055799eb8f494 in target_fetch_registers (regcache=0x55799f80d960, regno=8) at target.c:3948
#8  0x000055799ea78242 in regcache::raw_update (this=0x55799f80d960, regnum=8) at regcache.c:594
#9  0x000055799ea782fb in readable_regcache::raw_read (this=0x55799f80d960, regnum=8,
		    buf=0x7ffe961a3860 "\260\070\032\226\376\177") at regcache.c:608
#10 0x000055799ea786de in readable_regcache::cooked_read (this=0x55799f80d960, regnum=8,
		    buf=0x7ffe961a3860 "\260\070\032\226\376\177") at regcache.c:697
#11 0x000055799ea7fb10 in readable_regcache::cooked_read<unsigned long, void> (this=0x55799f80d960, regnum=8,
		    val=0x7ffe961a3930) at regcache.c:782
#12 0x000055799ea78bed in regcache_cooked_read_unsigned (regcache=0x55799f80d960, regnum=8, val=0x7ffe961a3930)
    at regcache.c:796
#13 0x000055799ea7a5e8 in regcache_read_pc (regcache=0x55799f80d960) at regcache.c:1332
#14 0x000055799e91d441 in adjust_pc_after_break (thread=0x55799f800470, ws=...) at infrun.c:4508
#15 0x000055799e920105 in handle_inferior_event (ecs=0x7ffe961a3bd0) at infrun.c:5407
#16 0x000055799e91bfa7 in wait_for_inferior (inf=0x55799f721b90) at infrun.c:4024
#17 0x000055799e91a195 in start_remote (from_tty=1) at infrun.c:3442
#18 0x000055799ea94735 in remote_target::start_remote_1 (this=0x55799f7ac980, from_tty=1, extended_p=0)
        at remote.c:4979
#19 0x000055799ea94b7e in remote_target::start_remote (this=0x55799f7ac980, from_tty=1, extended_p=0) at remote.c:5070
#20 0x000055799ea96393 in remote_target::open_1 (name=0x55799f719f4e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#21 0x000055799ea94c19 in remote_target::open (name=0x55799f719f4e "localhost:2159", from_tty=1) at remote.c:5092
#22 0x000055799eb7871c in open_target (args=0x55799f719f4e "localhost:2159", from_tty=1, command=0x55799f78b8e0)
	        at target.c:853
#23 0x000055799e6d0895 in cmd_func (cmd=0x55799f78b8e0, args=0x55799f719f4e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#24 0x000055799eba6e93 in execute_command (p=0x55799f719f5b "9", from_tty=1) at top.c:699
#25 0x000055799e87374a in command_handler (command=0x55799f719f40 "") at event-top.c:598
#26 0x000055799e873c88 in command_line_handler (rl=...) at event-top.c:842
#27 0x000055799e873e3c in gdb_readline_no_editing_callback (client_data=0x55799f71f560) at event-top.c:907
#28 0x000055799e87354c in stdin_event_handler (error=0, client_data=0x55799f71f560) at event-top.c:525
#29 0x000055799eded607 in handle_file_event (file_ptr=0x55799f7f5c90, ready_mask=1) at event-loop.cc:574
#30 0x000055799ededbf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#31 0x000055799edec89f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#32 0x000055799e99e2d7 in start_event_loop () at main.c:411
#33 0x000055799e99e424 in captured_command_loop () at main.c:471
#34 0x000055799e99fee2 in captured_main (data=0x7ffe961a4520) at main.c:1329
#35 0x000055799e99ff54 in gdb_main (args=0x7ffe961a4520) at main.c:1344
#36 0x000055799e554de6 in main (argc=1, argv=0x7ffe961a4658) at gdb.c:32
```

* `remote_target::read_frame` read characters from QEMU by serial communication.
* So, `this->m_remote_state.buf.data()`, hexadecimal values represented as string and meaning values of the registers of the debuggee, comes from packets sending QEMU.

# Where does the register offsets `r->offset` at `~/binutils-gdb/gdb/remote.c` line 8570 come from?

* The offset of FCTRL is `rsa->regs[0x18].offset` at `remote.c` line 8475.

```
~/hariboslinux # make debug
(gdb) break remote.c : 8475
(gdb) run debuggee_input.txt
(gdb) p/x rsa->regs[0x18].offset
$1 = 0xb8
```

* Above `rsa` is `struct remote_arch_state` at `~/binutils-gdb/gdb/remote.c` line 203 and its constructor is `remote_arch_state::remote_arch_state` defined at line 205 and and implemented at line 1469.

```
~/hariboslinux # make debug
(gdb) break remote_arch_state::remote_arch_state
(gdb) run < debuggee_input.txt
The first arribal to the breakpoint.
(gdb) continue
The second arribal to the breakpoint.
(gdb) break 1500
(gdb) continue
The end of the constructor.
(gdb) p/x this->regs[0x18].offset
$1 = 0xb8
```

* `this->regs[0x18].offset` is stored at `remote.c` line 1477.

```
~/hariboslinux # make debug
(gdb) break remote.c : 1477
(gdb) run < debuggee_input.txt
The first arrival to remote.c : 1477
(gdb) continue
The second arrival to remote.c : 1477
(gdb) p/x this->regs[0x18].offset
$1 = 0x0
(gdb) next
(gdb) p/x this->regs[0x18].offset
$1 = 0xb8
```

* `this->regs[0x18].offset` is stored at `~/binutils-gdb/gdb/remote.c` line 1440.

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
The first arrival to map_regcache_remote_table
(gdb) continue
The second arrival to map_regcache_remote_table
(gdb) p/x regs[0x18].offset
$1 = 0x0
(gdb) watch regs[0x18].offset
(gdb) continue
remote.c : 1440 executed.
(gdb) p/x regs[0x18].offset
$1 = 0xb8
```

* Register offsets are determined at `~/binutils-gdb/gdb/remote.c` line 1437-1442.

```
  for (regnum = 0, offset = 0; regnum < num_remote_regs; regnum++)
    {
      remote_regs[regnum]->in_g_packet = 1;
      remote_regs[regnum]->offset = offset;
      offset += register_size (gdbarch, remote_regs[regnum]->regnum);
    }
```

* `GDB` interprets register values list sent by `QEMU` as below.
* For example, `QEMU` writes `FCTRL` in offset `0xb4`. But `GDB` interprets there is it in offset `0xb8`.

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
| EFER        |   0x18 |                        0x51 |                      0x0060 |                                                0x08 |
| ST0         |   0x19 |                        0x10 |                      0x0068 |                                                0x0a |
| ST1         |   0x1a |                        0x11 |                      0x0072 |                                                0x0a |
| ST2         |   0x1b |                        0x12 |                      0x007c |                                                0x0a |
| ST3         |   0x1c |                        0x13 |                      0x0086 |                                                0x0a |
| ST4         |   0x1d |                        0x14 |                      0x0090 |                                                0x0a |
| ST5         |   0x1e |                        0x15 |                      0x009a |                                                0x0a |
| ST6         |   0x1f |                        0x16 |                      0x00a4 |                                                0x0a |
| ST7         |   0x20 |                        0x17 |                      0x00ae |                                                0x0a |
| FCTRL       |   0x21 |                        0x18 |                      0x00b8 |                                                0x04 |
| FSTAT       |   0x22 |                        0x19 |                      0x00bc |                                                0x04 |
| FTAG        |   0x23 |                        0x1a |                      0x00c0 |                                                0x04 |
| FISEG       |   0x24 |                        0x1b |                      0x00c4 |                                                0x04 |
| FIOFF       |   0x25 |                        0x1c |                      0x00c8 |                                                0x04 |
| FOSEG       |   0x26 |                        0x1d |                      0x00cc |                                                0x04 |
| FOOFF       |   0x27 |                        0x1e |                      0x00d0 |                                                0x04 |
| FOP         |   0x28 |                        0x1f |                      0x00d4 |                                                0x04 |
| XMM0        |   0x29 |                        0x52 |                      0x00d8 |                                                0x10 |
| XMM1        |   0x2a |                        0x53 |                      0x00e8 |                                                0x10 |
| XMM2        |   0x2b |                        0x54 |                      0x00f8 |                                                0x10 |
| XMM3        |   0x2c |                        0x55 |                      0x0108 |                                                0x10 |
| XMM4        |   0x2d |                        0x56 |                      0x0118 |                                                0x10 |
| XMM5        |   0x2e |                        0x57 |                      0x0128 |                                                0x10 |
| XMM6        |   0x2f |                        0x58 |                      0x0138 |                                                0x10 |
| XMM7        |   0x30 |                        0x59 |                      0x0148 |                                                0x10 |
| MXCSR       |   0x31 |                        0x5a |                      0x0158 |                                                0x04 |

* How is `remote_regs[regnum]->regnum` determined?
	* `remote_regs` is `regs` sorted by `pnum`.
	* `regnum` equals `remote_regs[regnum]->pnum` and also `regs[remote_regs[regnum]->regnum]` and means register order sent by `QEMU`.
	* `regs[x].pnum` is determined by `gdbarch_remote_register_number` at `~/binutils-gdb/gdb/remote.c` line 1417.
* How is `register_size(gdbarch, remote_regs[regnum]->regnum)` determined?

# How is `r->pnum` meaning register order sent by `QEMU` is determined at `~/binutils-gdb/gdb/remote.c` line 1417?

* `gdbarch_remote_register_number(gdbarch, regnum)` at `gdbarch.c` line 2939 returns `tdesc_remote_register_number(gdbarch, regno)`
* `tdepsc_remote_register_number(gdbarch, regno)` at `target-descriptions.c` line 985 returns `tdesc_find_register(gdbarch, regno)->target_regnum`
* `tdesc_find_register(gdbarch, regno)` at `target-descriptions.c` line 880 returns `tdesc_find_arch_register(gdbarch, regno)->reg`
* `tdesc_find_arch_register(gdbarch, regno)` at `target-descriptions.c` line 869 returns `&get_arch_data(gdbarch)->arch_regs[regno]`
* `get_arch_data(gdbarch)` at `target-descriptions.c` line 488 returns `tdesc_data.get(gdbarch)`
* `tdesc_data.get(gdbarch)` at `registry.h` line 110 returns `registry_accessor::get(gdbarch)->get (m_key)`
* `registry_accessor::get(gdbarch)` at `arch-utils.c` line 1206 returns `&gdbarch->registry_fields`
* `registry_accessor::get(gdbarch)->get (m_key)` at `registry.h` line 213 returns `registry_accessor::get(gdbarch)->m_fields[tdesc_data.m_key]`

Actually,

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$35 = 0x0
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$36 = 0xf
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$37 = 0x10
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$38 = 0x18
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$39 = 0x19
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$40 = 0x28
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$41 = 0x29
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$42 = 0x31
```

For example,

```
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[x].reg->target_regnum
 = y
```

Above means `x` is a register number in GDB and `y` is QEMU sends the register `y`th in the all registers.

* Where does `gdbarch` come from?

```
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) backtrace
#0  map_regcache_remote_table (gdbarch=0x564f4abeb410, regs=0x564f4abfac10) at remote.c:1405
#1  0x0000564f48e2145f in remote_arch_state::remote_arch_state (this=0x564f4ab0c440, gdbarch=0x564f4abeb410)
    at remote.c:1478
#2  0x0000564f48e510d1 in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, 0ul, gdbarch*&, 0ul> (
		    this=0x564f4ab0c438, __tuple1=..., __tuple2=...) at /usr/include/c++/11/tuple:1824
#3  0x0000564f48e50d9f in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, gdbarch*&> (
		    this=0x564f4ab0c438, __first=..., __second=...) at /usr/include/c++/11/tuple:1813
#4  0x0000564f48e507b8 in __gnu_cxx::new_allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x564f4ab97be0, __p=0x564f4ab0c438)
    at /usr/include/c++/11/ext/new_allocator.h:162
#5  0x0000564f48e4fccb in std::allocator_traits<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (__a=..., __p=0x564f4ab0c438)
        at /usr/include/c++/11/bits/alloc_traits.h:516
#6  0x0000564f48e4ed4d in std::__detail::_Hashtable_alloc<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::_M_allocate_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x564f4ab97be0) at /usr/include/c++/11/bits/hashtable_policy.h:1878
#7  0x0000564f48e4da5d in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_Scoped_node::_Scoped_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x7ffe0f185c60, __h=0x564f4ab97be0) at /usr/include/c++/11/bits/hashtable.h:304
#8  0x0000564f48e4bda1 in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_M_emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x564f4ab97be0) at /usr/include/c++/11/bits/hashtable.h:1966
#9  0x0000564f48e49828 in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x564f4ab97be0) at /usr/include/c++/11/bits/hashtable.h:915
#10 0x0000564f48e47746 in std::unordered_map<gdbarch*, remote_arch_state, std::hash<gdbarch*>, std::equal_to<gdbarch*>, std::allocator<std::pair<gdbarch* const, remote_arch_state> > >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x564f4ab97be0) at /usr/include/c++/11/bits/unordered_map.h:389
#11 0x0000564f48e20d88 in remote_state::get_remote_arch_state (this=0x564f4ab979a8, gdbarch=0x564f4abeb410)
	    at remote.c:1327
#12 0x0000564f48e20e39 in remote_target::get_remote_state (this=0x564f4ab97980) at remote.c:1353
#13 0x0000564f48e3246d in remote_target::set_remote_traceframe (this=0x564f4ab97980) at remote.c:8589
#14 0x0000564f48e38a74 in remote_target::xfer_partial (this=0x564f4ab97980, object=TARGET_OBJECT_AUXV, annex=0x0,
		    readbuf=0x564f4abf9c00 "\200SBk/\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7ffe0f185fe0)
    at remote.c:11235
#15 0x0000564f48f0e553 in target_xfer_partial (ops=0x564f4ab97980, object=TARGET_OBJECT_AUXV, annex=0x0,
		    readbuf=0x564f4abf9c00 "\200SBk/\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7ffe0f185fe0)
    at target.c:1730
#16 0x0000564f48f0ed5d in target_read_partial (ops=0x564f4ab97980, object=TARGET_OBJECT_AUXV, annex=0x0,
		    buf=0x564f4abf9c00 "\200SBk/\177", offset=0, len=4096, xfered_len=0x7ffe0f185fe0) at target.c:1964
#17 0x0000564f48f25ba8 in target_read_alloc_1<unsigned char> (ops=0x564f4ab97980, object=TARGET_OBJECT_AUXV,
		    annex=0x0) at target.c:2299
#18 0x0000564f48f0f766 in target_read_alloc (ops=0x564f4ab97980, object=TARGET_OBJECT_AUXV, annex=0x0)
    at target.c:2328
#19 0x0000564f4899de3c in get_auxv_inferior_data (ops=0x564f4ab97980) at auxv.c:367
#20 0x0000564f4899dea8 in target_auxv_search (ops=0x564f4ab97980, match=0, valp=0x7ffe0f186148) at auxv.c:381
#21 0x0000564f48d0bd82 in linux_is_uclinux () at linux-tdep.c:422
#22 0x0000564f48d0bdd9 in linux_has_shared_address_space (gdbarch=0x564f4abeb410) at linux-tdep.c:429
#23 0x0000564f489857b5 in gdbarch_has_shared_address_space (gdbarch=0x564f4abeb410)
        at /root/binutils-gdb/gdb/gdbarch.c:4843
#24 0x0000564f48ddf898 in update_address_spaces () at progspace.c:372
#25 0x0000564f48e28096 in remote_target::start_remote_1 (this=0x564f4ab97980, from_tty=1, extended_p=0)
	    at remote.c:4837
#26 0x0000564f48e28b7e in remote_target::start_remote (this=0x564f4ab97980, from_tty=1, extended_p=0) at remote.c:5070
#27 0x0000564f48e2a393 in remote_target::open_1 (name=0x564f4ab04f4e "localhost:2159", from_tty=1, extended_p=0)
	        at remote.c:5873
#28 0x0000564f48e28c19 in remote_target::open (name=0x564f4ab04f4e "localhost:2159", from_tty=1) at remote.c:5092
#29 0x0000564f48f0c71c in open_target (args=0x564f4ab04f4e "localhost:2159", from_tty=1, command=0x564f4ab768e0)
		    at target.c:853
#30 0x0000564f48a64895 in cmd_func (cmd=0x564f4ab768e0, args=0x564f4ab04f4e "localhost:2159", from_tty=1)
		        at cli/cli-decode.c:2516
#31 0x0000564f48f3ae93 in execute_command (p=0x564f4ab04f5b "9", from_tty=1) at top.c:699
#32 0x0000564f48c0774a in command_handler (command=0x564f4ab04f40 "") at event-top.c:598
#33 0x0000564f48c07c88 in command_line_handler (rl=...) at event-top.c:842
#34 0x0000564f48c07e3c in gdb_readline_no_editing_callback (client_data=0x564f4ab0a560) at event-top.c:907
#35 0x0000564f48c0754c in stdin_event_handler (error=0, client_data=0x564f4ab0a560) at event-top.c:525
#36 0x0000564f49181607 in handle_file_event (file_ptr=0x564f4abe0c90, ready_mask=1) at event-loop.cc:574
#37 0x0000564f49181bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#38 0x0000564f4918089f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#39 0x0000564f48d322d7 in start_event_loop () at main.c:411
#40 0x0000564f48d32424 in captured_command_loop () at main.c:471
#41 0x0000564f48d33ee2 in captured_main (data=0x7ffe0f186ac0) at main.c:1329
#42 0x0000564f48d33f54 in gdb_main (args=0x7ffe0f186ac0) at main.c:1344
#43 0x0000564f488e8de6 in main (argc=1, argv=0x7ffe0f186bf8) at gdb.c:32
```

* There is the same gdbarch address in `#23` in the above backtrace.

Actually,

```
~/hariboslinux # make debug
(gdb) break gdbarch_has_shared_address_space 
(gdb) run < debuggee_input.txt
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `gdbarch` is `current_inferior_.m_obj->gdbarch`

```
~/hariboslinux # make debug
(gdb) break update_address_spaces
(gdb) run < debuggee_input.txt
(gdb) break target_gdbarch
(gdb) continue
~/binutils-gdb/gdb/arch-utils.c : 1452
return current_inferior()->gdbarch;
(gdb) step
~/binutils-gdb/gdb/inferior.c : 55
return current_inferior_.get()
(gdb) step
~/binutils-gdb/gdbsupport/gdb_ref_ptr.h : 132
```

Actually,

```
~/hariboslinux # make debug
(gdb) break update_address_spaces
(gdb) run < debuggee_input.txt
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `current_inferior_` is initialized at `~/binutils-gdb/gdbsupport/gdb_reh_ptr.h` line 126.
* `current_inferior_.m_obj->gdbarch` is initialized at `~/binutils-gdb/gdb/arch-utils.c` line 1443 twice.

Actually,

```
~/hariboslinux # make debug
(gdb) watch current_inferior_
(gdb) run < debuggee_input.tx
~/binutils-gdb/gdbsupport/gdb_ref_ptr.h : 126
(gdb) delete 1
(gdb) watch current_inferior_.m_obj->gdbarch
(gdb) continue
~/binutils-gdb/gdb/arch-utils.c : 1443
(gdb) continue
~/binutils-gdb/gdb/arch-utils.c : 1443
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)current_inferior_.m_obj->gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
(gdb) backtrace
#0  set_target_gdbarch (new_gdbarch=0x5650fdd22410) at arch-utils.c:1443
#1  0x00005650fcd1682b in gdbarch_update_p (info=...) at arch-utils.c:625
#2  0x00005650fd295baa in target_find_description () at target-descriptions.c:569
#3  0x00005650fd1ca091 in remote_target::start_remote_1 (this=0x5650fdcce980, from_tty=1, extended_p=0)
    at remote.c:4833
#4  0x00005650fd1cab7e in remote_target::start_remote (this=0x5650fdcce980, from_tty=1, extended_p=0) at remote.c:5070
#5  0x00005650fd1cc393 in remote_target::open_1 (name=0x5650fdc3bf4e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#6  0x00005650fd1cac19 in remote_target::open (name=0x5650fdc3bf4e "localhost:2159", from_tty=1) at remote.c:5092
#7  0x00005650fd2ae71c in open_target (args=0x5650fdc3bf4e "localhost:2159", from_tty=1, command=0x5650fdcad8e0)
	    at target.c:853
#8  0x00005650fce06895 in cmd_func (cmd=0x5650fdcad8e0, args=0x5650fdc3bf4e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2516
#9  0x00005650fd2dce93 in execute_command (p=0x5650fdc3bf5b "9", from_tty=1) at top.c:699
#10 0x00005650fcfa974a in command_handler (command=0x5650fdc3bf40 "") at event-top.c:598
#11 0x00005650fcfa9c88 in command_line_handler (rl=...) at event-top.c:842
#12 0x00005650fcfa9e3c in gdb_readline_no_editing_callback (client_data=0x5650fdc41560) at event-top.c:907
#13 0x00005650fcfa954c in stdin_event_handler (error=0, client_data=0x5650fdc41560) at event-top.c:525
#14 0x00005650fd523607 in handle_file_event (file_ptr=0x5650fdd17c90, ready_mask=1) at event-loop.cc:574
#15 0x00005650fd523bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#16 0x00005650fd52289f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#17 0x00005650fd0d42d7 in start_event_loop () at main.c:411
#18 0x00005650fd0d4424 in captured_command_loop () at main.c:471
#19 0x00005650fd0d5ee2 in captured_main (data=0x7ffe8191c660) at main.c:1329
#20 0x00005650fd0d5f54 in gdb_main (args=0x7ffe8191c660) at main.c:1344
#21 0x00005650fcc8ade6 in main (argc=1, argv=0x7ffe8191c798) at gdb.c:32
```

* `new_gdbarch` is returned at `~/binutils-gdb/gdb/arch-utils.c` line 1432.

```
~/hariboslinux # make debug
(gdb) break arch-utils.c : 1432
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint.
(gdb) continue
The second arrival to the breakpoint.
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `new_gdbarch` is generated at `~/binutils-gdb/gdb/arch-utils.c` line 1367.

```
  new_gdbarch = rego->init (info, rego->arches);
```

Actually,

```
~/hariboslinux # make debug
(gdb) break arch-utils.c : 1371
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint.
(gdb) continue
The second arrival to the breakpoint.
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)new_gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `rego->init` at `~/binutils-gdb/gdb/arch-utils.c` line 1367 is `i386_gdbarch_init` at `~/binutils-gdb/gdb/i386-tdep.c` line 8445.
* `i386_gdbarch_init` at `~/binutils-gdb/gdb/i386-tdep.c` line 8445 returns `gdbarch` at line 8803.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8803
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint.
(gdb) continue
The second arrival to the breakpoint.
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs` is validated at `~/binutils-gdb/gdb/i386-tdep.c` line 8726.

```
tdesc_use_registers (gdbarch, tdesc, std::move (tdesc_data));
```

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8726
(gdb) run < debuggee_input.txt
The first arrival to the breakpoint.
(gdb) continue
The second arrival to the breakpoint.
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x00]
Cannot access memory at address 0x0
(gdb) next
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[::tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* `tdesc_use_registers` at `~/binutils-gdb/gdb/target-descriptions.c` line 1087 stores `target_regnum` at line 1162-1168.

Actually,

```
~/hariboslinux # make debug
(gdb) break target-descriptions.c : 1158
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) next
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg
$1 = NULL
(gdb) break 1171
(gdb) continue
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x00].reg->target_regnum
$2 = 0x0
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x0f].reg->target_regnum
$3 = 0xf
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x49].reg->target_regnum
$4 = 0x10
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x51].reg->target_regnum
$5 = 0x18
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x10].reg->target_regnum
$6 = 0x19
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x1f].reg->target_regnum
$7 = 0x28
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x52].reg->target_regnum
$8 = 0x29
(gdb) p/x ((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])->arch_regs[0x5a].reg->target_regnum
$9 = 0x31
```

* `((tdesc_arch_data*)gdbarch->registry_fields.m_fields[tdesc_data.m_key])` is equal to `data`.

Actually,

```
~/hariboslinux # make debug
(gdb) break target-descriptions.c : 1171
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x data->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x data->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x data->arch_regs[0x49].reg->target_regnum
$3 = 0x10
(gdb) p/x data->arch_regs[0x51].reg->target_regnum
$4 = 0x18
(gdb) p/x data->arch_regs[0x10].reg->target_regnum
$5 = 0x19
(gdb) p/x data->arch_regs[0x1f].reg->target_regnum
$6 = 0x28
(gdb) p/x data->arch_regs[0x52].reg->target_regnum
$7 = 0x29
(gdb) p/x data->arch_regs[0x5a].reg->target_regnum
$8 = 0x31
```

* The vector elements from `data->arch_regs[0x00]` to `data->arch_regs[0x1f]` are initialized at `~/binutils-gdb/gdb/target-descriptions.c` line 1098.

Actually,

```
~/hariboslinux # make debug
(gdb) break target-descriptions.c : 1098
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) print data->arch_regs[0x00]
Cannot access memory at address 0x0
(gdb) next
(gdb) p/x data->arch_regs[0x00].reg->target_regnum
$2 = 0x0
(gdb) p/x data->arch_regs[0x0f].reg->target_regnum
$3 = 0xf
(gdb) p/x data->arch_regs[0x10].reg->target_regnum
$4 = 0x19
(gdb) p/x data->arch_regs[0x1f].reg->target_regnum
$5 = 0x28
```

* The above `data` is the third argument `early_data` of the function.

Actually,

```
~/hariboslinux # make debug
(gdb) break tdesc_use_registers
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x early_data->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x early_data->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x early_data->arch_regs[0x10].reg->target_regnum
$3 = 0x19
(gdb) p/x early_data->arch_regs[0x1f].reg->target_regnum
$4 = 0x28
(gdb) backtrace
#0  tdesc_use_registers (gdbarch=0x559838dc1430, target_desc=0x559838dce6b0, early_data=..., unk_reg_cb=0x0)
    at target-descriptions.c:1087
#1  0x0000559836522d59 in i386_gdbarch_init (info=..., arches=0x0) at i386-tdep.c:8726
#2  0x0000559836226c2e in gdbarch_find_by_info (info=...) at arch-utils.c:1367
#3  0x0000559836213730 in gdbarch_update_p (info=...) at arch-utils.c:596
#4  0x0000559836792baa in target_find_description () at target-descriptions.c:569
#5  0x00005598366c7091 in remote_target::start_remote_1 (this=0x559838d6d980, from_tty=1, extended_p=0)
        at remote.c:4833
#6  0x00005598366c7b7e in remote_target::start_remote (this=0x559838d6d980, from_tty=1, extended_p=0) at remote.c:5070
#7  0x00005598366c9393 in remote_target::open_1 (name=0x559838cdaf4e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#8  0x00005598366c7c19 in remote_target::open (name=0x559838cdaf4e "localhost:2159", from_tty=1) at remote.c:5092
#9  0x00005598367ab71c in open_target (args=0x559838cdaf4e "localhost:2159", from_tty=1, command=0x559838d4c8e0)
	        at target.c:853
#10 0x0000559836303895 in cmd_func (cmd=0x559838d4c8e0, args=0x559838cdaf4e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2516
#11 0x00005598367d9e93 in execute_command (p=0x559838cdaf5b "9", from_tty=1) at top.c:699
#12 0x00005598364a674a in command_handler (command=0x559838cdaf40 "") at event-top.c:598
#13 0x00005598364a6c88 in command_line_handler (rl=...) at event-top.c:842
#14 0x00005598364a6e3c in gdb_readline_no_editing_callback (client_data=0x559838ce0560) at event-top.c:907
#15 0x00005598364a654c in stdin_event_handler (error=0, client_data=0x559838ce0560) at event-top.c:525
#16 0x0000559836a20607 in handle_file_event (file_ptr=0x559838db6cb0, ready_mask=1) at event-loop.cc:574
#17 0x0000559836a20bf7 in gdb_wait_for_event (block=0) at event-loop.cc:695
#18 0x0000559836a1f89f in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#19 0x00005598365d12d7 in start_event_loop () at main.c:411
#20 0x00005598365d1424 in captured_command_loop () at main.c:471
#21 0x00005598365d2ee2 in captured_main (data=0x7fffa6214b80) at main.c:1329
#22 0x00005598365d2f54 in gdb_main (args=0x7fffa6214b80) at main.c:1344
#23 0x0000559836187de6 in main (argc=1, argv=0x7fffa6214cb8) at gdb.c:32
```

* The above `early_data` is `tdesc_data` at `i386-tdep.c` line 8726.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8726
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x tdesc_data->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x tdesc_data->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x tdesc_data->arch_regs[0x10].reg->target_regnum
$3 = 0x19
(gdb) p/x tdesc_data->arch_regs[0x1f].reg->target_regnum
$4 = 0x28
```

* The above `target_regnum` is validated by `i386_validate_tdesc_p` at `i386-tdep.c` line 8703.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8703
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x tdesc_data->arch_regs[0x00]
Cannot access memory at address 0x0
(gdb) next
(gdb) p/x tdesc_data->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x tdesc_data->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x tdesc_data->arch_regs[0x10].reg->target_regnum
$3 = 0x19
(gdb) p/x tdesc_data->arch_regs[0x1f].reg->target_regnum
$4 = 0x28
```

* The above `target_regnum` is validate by a for statement at `i386-tdep.c` line 8358-8360.

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8357
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) next
(gdb) p/x tdesc_data->arch_regs[0x00]
Cannot access memory at address 0x0
(gdb) delete 1
(gdb) break 8362
(gdb) continue
(gdb) p/x tdesc_data->arch_regs[0x00].reg->target_regnum
$1 = 0x0
(gdb) p/x tdesc_data->arch_regs[0x0f].reg->target_regnum
$2 = 0xf
(gdb) p/x tdesc_data->arch_regs[0x10].reg->target_regnum
$3 = 0x19
(gdb) p/x tdesc_data->arch_regs[0x1f].reg->target_regnum
$4 = 0x28
```

* A function `tdesc_numbered_register` at `target-descriptions.c` line 805 validates the each elements of `tdesc_data->arch_regs`.
* The below table means relationship of the arguments `regno` and `name` of the function.

| regno | name   |
| ----: | :----- |
|   0x0 | eax    |
|   0x1 | ecx    |
|   0x2 | edx    |
|   0x3 | ebx    |
|   0x4 | esp    |
|   0x5 | ebp    |
|   0x6 | esi    |
|   0x7 | edi    |
|   0x8 | eip    |
|   0x9 | eflags |
|   0xa | cs     |
|   0xb | ss     |
|   0xc | ds     |
|   0xd | es     |
|   0xe | fs     |
|   0xf | gx     |
|  0x10 | st0    |
|  0x11 | st1    |
|  0x12 | st2    |
|  0x13 | st3    |
|  0x14 | st4    |
|  0x15 | st5    |
|  0x16 | st6    |
|  0x17 | st7    |
|  0x18 | fctrl  |
|  0x19 | fstat  |
|  0x1a | ftag   |
|  0x1b | fiseg  |
|  0x1c | fioff  |
|  0x1d | foseg  |
|  0x1e | fooff  |
|  0x1f | fop    |

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8357
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_numbered_register
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 0, name = "eax")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 1, name = "ecx")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 2, name = "edx")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 3, name = "ebx")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 4, name = "esp")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 5, name = "ebp")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 6, name = "esi")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 7, name = "edi")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 8, name = "eip")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 9, name = "eflags")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 10, name = "cs")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 11, name = "ss")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 12, name = "ds")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 13, name = "es")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 14, name = "fs")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 15, name = "gs")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 16, name = "st0")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 17, name = "st1")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 18, name = "st2")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 19, name = "st3")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 20, name = "st4")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 21, name = "st5")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 22, name = "st6")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 23, name = "st7")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 24, name = "fctrl")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 25, name = "fstat")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 26, name = "ftag")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 27, name = "fiseg")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 28, name = "fioff")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 29, name = "foseg")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 30, name = "fooff")
(gdb) continue
target-descriptions.c : 805 tdesc_numbered_register(..., regno = 31, name = "fop")
```

* `data->arch_regs[regno]` is stored at `target-descriptions.c` line 815.
* `regno` means register number in GDB.
* `data->arch_regs[regno].reg->target_regnum` means register order send from QEMU.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8357
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break target-descriptions.c : 817
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 0, name = "eax")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$1 = 0x0
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 1, name = "ecx")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$2 = 0x1
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 2, name = "edx")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$3 = 0x2
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 3, name = "ebx")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$4 = 0x3
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 4, name = "esp")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$5 = 0x4
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 5, name = "ebp")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$6 = 0x5
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 6, name = "esi")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$7 = 0x6
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 7, name = "edi")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$8 = 0x7
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 8, name = "eip")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$9 = 0x8
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 9, name = "eflags")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$10 = 0x9
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 10, name = "cs")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$11 = 0xa
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 11, name = "ss")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$12 = 0xb
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 12, name = "ds")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$13 = 0xc
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 13, name = "es")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$14 = 0xd
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 14, name = "fs")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$15 = 0xe
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 15, name = "gs")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$16 = 0xf
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 16, name = "st0")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$17 = 0x19
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 17, name = "st1")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$18 = 0x1a
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 18, name = "st2")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$19 = 0x1b
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 19, name = "st3")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$20 = 0x1c
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 20, name = "st4")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$21 = 0x1d
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 21, name = "st5")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$22 = 0x1e
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 22, name = "st6")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$23 = 0x1f
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 23, name = "st7")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$24 = 0x20
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 24, name = "fctrl")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$25 = 0x21
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 25, name = "fstat")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$26 = 0x22
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 26, name = "ftag")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$27 = 0x23
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 27, name = "fiseg")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$28 = 0x24
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 28, name = "fioff")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$29 = 0x25
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 29, name = "foseg")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$30 = 0x26
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 30, name = "fooff")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$31 = 0x27
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 31, name = "fop")
(gdb) p/x data->arch_regs[regno].reg->target_regnum
$32 = 0x28
```

* Avobe `data->arch_regs[regno].reg` equals `reg`

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8357
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break target-descriptions.c : 817
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 0, name = "eax")
(gdb) p/x reg->target_regnum
$1 = 0x0
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 1, name = "ecx")
(gdb) p/x reg->target_regnum
$2 = 0x1
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 2, name = "edx")
(gdb) p/x reg->target_regnum
$3 = 0x2
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 3, name = "ebx")
(gdb) p/x reg->target_regnum
$4 = 0x3
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 4, name = "esp")
(gdb) p/x reg->target_regnum
$5 = 0x4
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 5, name = "ebp")
(gdb) p/x reg->target_regnum
$6 = 0x5
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 6, name = "esi")
(gdb) p/x reg->target_regnum
$7 = 0x6
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 7, name = "edi")
(gdb) p/x reg->target_regnum
$8 = 0x7
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 8, name = "eip")
(gdb) p/x reg->target_regnum
$9 = 0x8
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 9, name = "eflags")
(gdb) p/x reg->target_regnum
$10 = 0x9
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 10, name = "cs")
(gdb) p/x reg->target_regnum
$11 = 0xa
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 11, name = "ss")
(gdb) p/x reg->target_regnum
$12 = 0xb
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 12, name = "ds")
(gdb) p/x reg->target_regnum
$13 = 0xc
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 13, name = "es")
(gdb) p/x reg->target_regnum
$14 = 0xd
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 14, name = "fs")
(gdb) p/x reg->target_regnum
$15 = 0xe
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 15, name = "gs")
(gdb) p/x reg->target_regnum
$16 = 0xf
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 16, name = "st0")
(gdb) p/x reg->target_regnum
$17 = 0x19
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 17, name = "st1")
(gdb) p/x reg->target_regnum
$18 = 0x1a
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 18, name = "st2")
(gdb) p/x reg->target_regnum
$19 = 0x1b
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 19, name = "st3")
(gdb) p/x reg->target_regnum
$20 = 0x1c
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 20, name = "st4")
(gdb) p/x reg->target_regnum
$21 = 0x1d
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 21, name = "st5")
(gdb) p/x reg->target_regnum
$22 = 0x1e
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 22, name = "st6")
(gdb) p/x reg->target_regnum
$23 = 0x1f
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 23, name = "st7")
(gdb) p/x reg->target_regnum
$24 = 0x20
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 24, name = "fctrl")
(gdb) p/x reg->target_regnum
$25 = 0x21
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 25, name = "fstat")
(gdb) p/x reg->target_regnum
$26 = 0x22
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 26, name = "ftag")
(gdb) p/x reg->target_regnum
$27 = 0x23
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 27, name = "fiseg")
(gdb) p/x reg->target_regnum
$28 = 0x24
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 28, name = "fioff")
(gdb) p/x reg->target_regnum
$29 = 0x25
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 29, name = "foseg")
(gdb) p/x reg->target_regnum
$30 = 0x26
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 30, name = "fooff")
(gdb) p/x reg->target_regnum
$31 = 0x27
(gdb) continue
target-descriptions.c : 817 tdesc_numbered_register(..., regno = 31, name = "fop")
(gdb) p/x reg->target_regnum
$32 = 0x28
```

* `tdesc_find_register_early` at `~/binutils-gdb/gdb/target-descriptions.c` line 788 returns the above `reg`.
* `feature->registers` at `~/binutils-gdb/gdb/target-descriptions.c` line 792 has informations of the registers.
* Type of the `feature` is `tdesc_feature` defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 284.
* Type of `feature->registers`, a member of `feature`, is `std::vector<tdesc_reg_up>`.
* The type `tdesc_reg_up` is defined as `std::unique_ptr<tdesc_reg>` at `~/binutils-gdb/gdbsupport/tdesc.h` line 132.
* The type `tdesc_reg` is defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 67.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8357
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_numbered_register
(gdb) continue
(gdb) print ((tdesc_reg_up*)feature->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)feature->registers.begin())->get()->target_regnum
$2 = 0
(gdb) backtrace
#0  tdesc_numbered_register (feature=0x562f1d5dcb40, data=0x562f1d4f1430, regno=0, name=0x562f1b8f472a "eax")
    at target-descriptions.c:805
#1  0x0000562f1b30f56c in i386_validate_tdesc_p (tdep=0x562f1d5cd480, tdesc_data=0x562f1d4f1430) at i386-tdep.c:8359
#2  0x0000562f1b30ffe0 in i386_gdbarch_init (info=..., arches=0x0) at i386-tdep.c:8703
#3  0x0000562f1b013c2e in gdbarch_find_by_info (info=...) at arch-utils.c:1367
#4  0x0000562f1b000730 in gdbarch_update_p (info=...) at arch-utils.c:596
#5  0x0000562f1b57fee8 in target_find_description () at target-descriptions.c:569
#6  0x0000562f1b4b43cf in remote_target::start_remote_1 (this=0x562f1d57cab0, from_tty=1, extended_p=0)
        at remote.c:4833
#7  0x0000562f1b4b4ebc in remote_target::start_remote (this=0x562f1d57cab0, from_tty=1, extended_p=0) at remote.c:5070
#8  0x0000562f1b4b66d1 in remote_target::open_1 (name=0x562f1d4e9f4e "localhost:2159", from_tty=1, extended_p=0)
	    at remote.c:5873
#9  0x0000562f1b4b4f57 in remote_target::open (name=0x562f1d4e9f4e "localhost:2159", from_tty=1) at remote.c:5092
#10 0x0000562f1b598a5a in open_target (args=0x562f1d4e9f4e "localhost:2159", from_tty=1, command=0x562f1d55ba10)
	        at target.c:853
#11 0x0000562f1b0f0a07 in cmd_func (cmd=0x562f1d55ba10, args=0x562f1d4e9f4e "localhost:2159", from_tty=1)
		    at cli/cli-decode.c:2543
#12 0x0000562f1b5c71d1 in execute_command (p=0x562f1d4e9f5b "9", from_tty=1) at top.c:699
#13 0x0000562f1b29391c in command_handler (command=0x562f1d4e9f40 "") at event-top.c:598
#14 0x0000562f1b293e5a in command_line_handler (rl=...) at event-top.c:842
#15 0x0000562f1b29400e in gdb_readline_no_editing_callback (client_data=0x562f1d4ef560) at event-top.c:907
#16 0x0000562f1b29371e in stdin_event_handler (error=0, client_data=0x562f1d4ef560) at event-top.c:525
#17 0x0000562f1b80d9f3 in handle_file_event (file_ptr=0x562f1d5c5de0, ready_mask=1) at event-loop.cc:574
#18 0x0000562f1b80dfe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#19 0x0000562f1b80cc8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#20 0x0000562f1b3be635 in start_event_loop () at main.c:411
#21 0x0000562f1b3be782 in captured_command_loop () at main.c:471
#22 0x0000562f1b3c0240 in captured_main (data=0x7ffdefd34370) at main.c:1329
#23 0x0000562f1b3c02b2 in gdb_main (args=0x7ffdefd34370) at main.c:1344
#24 0x0000562f1af74de6 in main (argc=1, argv=0x7ffdefd344a8) at gdb.c:32
```

* The above `feature` is generated at `~/binutils-gdb/gdb/i386-tdep.c` line 8360 as a return of `tdesc_find_feature` at `~/binutils-gdb/gdb/target-descriptions.c` line 733.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8260
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) step
target-desctiptions.c : 733 tdesc_find_feature
(gdb) break 736
(gdb) continue
(gdb) print ((tdesc_reg_up*)feature.get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)feature.get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The function `tdesc_find_features` finds a feature by the second argument `name` in the first argument `target_desc->features`.
* The type of the first argument `target_desc` of the function `tdesc_find_feature` at `~/binutils-gdb/gdb/target-descriptions.c` line 730 is `struct target_desc` defined at `~/binutils-gdb/gdbserver/tdesc.h` line 30.
* The type `struct target_desc` defined at `~/binutils-gdb/gdbserver/tdesc.h` line 30 has `std::vector<tdesc_feature_up> features`.
* The type `tdesc_feature_up` of `target_desc::features` is defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 314 as `std::unique_ptr<tdesc_feature>`.
* The type `tdesc_feature` is defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 284.
* The type `tdesc_feature` defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 284 has `std::vector<tdesc_reg_up> registers`.
* The type `tdesc_reg_up` is defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 132 as `std::unique_ptr<tdesc_reg>`.
* The type `tdesc_reg` is defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 67.
* The type `tdesc_reg` defined at `~/binutils-gdb/gdbsupport/tdesc.h` line 67 has `std::string name` and `long target_regnum` means order of sending by QEMU.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8260
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) step
target-desctiptions.c : 733 tdesc_find_feature
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)target_desc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)target_desc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The above `target_desc` is `tdep->tdesc` from the first argument `tdep` of a function `i386_validate_tdesc_p`.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386_validate_tdesc_p
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdep->tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdep->tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
(gdb) backtrace
#0  i386_validate_tdesc_p (tdep=0x564517a14480, tdesc_data=0x564517938430) at i386-tdep.c:8249
#1  0x0000564514f5afe0 in i386_gdbarch_init (info=..., arches=0x0) at i386-tdep.c:8703
#2  0x0000564514c5ec2e in gdbarch_find_by_info (info=...) at arch-utils.c:1367
#3  0x0000564514c4b730 in gdbarch_update_p (info=...) at arch-utils.c:596
#4  0x00005645151caee8 in target_find_description () at target-descriptions.c:569
#5  0x00005645150ff3cf in remote_target::start_remote_1 (this=0x5645179c3ab0, from_tty=1, extended_p=0)
    at remote.c:4833
#6  0x00005645150ffebc in remote_target::start_remote (this=0x5645179c3ab0, from_tty=1, extended_p=0) at remote.c:5070
#7  0x00005645151016d1 in remote_target::open_1 (name=0x564517930f4e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#8  0x00005645150fff57 in remote_target::open (name=0x564517930f4e "localhost:2159", from_tty=1) at remote.c:5092
#9  0x00005645151e3a5a in open_target (args=0x564517930f4e "localhost:2159", from_tty=1, command=0x5645179a2a10)
	    at target.c:853
#10 0x0000564514d3ba07 in cmd_func (cmd=0x5645179a2a10, args=0x564517930f4e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2543
#11 0x00005645152121d1 in execute_command (p=0x564517930f5b "9", from_tty=1) at top.c:699
#12 0x0000564514ede91c in command_handler (command=0x564517930f40 "") at event-top.c:598
#13 0x0000564514edee5a in command_line_handler (rl=...) at event-top.c:842
#14 0x0000564514edf00e in gdb_readline_no_editing_callback (client_data=0x564517936560) at event-top.c:907
#15 0x0000564514ede71e in stdin_event_handler (error=0, client_data=0x564517936560) at event-top.c:525
#16 0x00005645154589f3 in handle_file_event (file_ptr=0x564517a0cde0, ready_mask=1) at event-loop.cc:574
#17 0x0000564515458fe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#18 0x0000564515457c8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#19 0x0000564515009635 in start_event_loop () at main.c:411
#20 0x0000564515009782 in captured_command_loop () at main.c:471
#21 0x000056451500b240 in captured_main (data=0x7ffcef8dee20) at main.c:1329
#22 0x000056451500b2b2 in gdb_main (args=0x7ffcef8dee20) at main.c:1344
#23 0x0000564514bbfde6 in main (argc=1, argv=0x7ffcef8def58) at gdb.c:32
```

* The above `tdep->tdesc` is stored at `~/binutils-gdb/gdb/i386-tdep.c` line 8640.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386-tdep.c : 8640
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)info.target_desc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)info.target_desc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The above `info` is the first argument of the function `i386_gdbarch_init` at `i386-tdep.c` line 8447.

Actually,

```
~/hariboslinux # make debug
(gdb) break i386_gdbarch_init
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)info.target_desc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)info.target_desc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
(gdb) backtrace
#0  i386_gdbarch_init (info=..., arches=0x55dd958f8210) at i386-tdep.c:8447
#1  0x000055dd92af2c2e in gdbarch_find_by_info (info=...) at arch-utils.c:1367
#2  0x000055dd92adf730 in gdbarch_update_p (info=...) at arch-utils.c:596
#3  0x000055dd9305eee8 in target_find_description () at target-descriptions.c:569
#4  0x000055dd92f933cf in remote_target::start_remote_1 (this=0x55dd958b8ab0, from_tty=1, extended_p=0)
    at remote.c:4833
#5  0x000055dd92f93ebc in remote_target::start_remote (this=0x55dd958b8ab0, from_tty=1, extended_p=0) at remote.c:5070
#6  0x000055dd92f956d1 in remote_target::open_1 (name=0x55dd95825f4e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#7  0x000055dd92f93f57 in remote_target::open (name=0x55dd95825f4e "localhost:2159", from_tty=1) at remote.c:5092
#8  0x000055dd93077a5a in open_target (args=0x55dd95825f4e "localhost:2159", from_tty=1, command=0x55dd95897a10)
	    at target.c:853
#9  0x000055dd92bcfa07 in cmd_func (cmd=0x55dd95897a10, args=0x55dd95825f4e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2543
#10 0x000055dd930a61d1 in execute_command (p=0x55dd95825f5b "9", from_tty=1) at top.c:699
#11 0x000055dd92d7291c in command_handler (command=0x55dd95825f40 "") at event-top.c:598
#12 0x000055dd92d72e5a in command_line_handler (rl=...) at event-top.c:842
#13 0x000055dd92d7300e in gdb_readline_no_editing_callback (client_data=0x55dd9582b560) at event-top.c:907
#14 0x000055dd92d7271e in stdin_event_handler (error=0, client_data=0x55dd9582b560) at event-top.c:525
#15 0x000055dd932ec9f3 in handle_file_event (file_ptr=0x55dd95901de0, ready_mask=1) at event-loop.cc:574
#16 0x000055dd932ecfe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#17 0x000055dd932ebc8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#18 0x000055dd92e9d635 in start_event_loop () at main.c:411
#19 0x000055dd92e9d782 in captured_command_loop () at main.c:471
#20 0x000055dd92e9f240 in captured_main (data=0x7ffd7e1c79d0) at main.c:1329
#21 0x000055dd92e9f2b2 in gdb_main (args=0x7ffd7e1c79d0) at main.c:1344
#22 0x000055dd92a53de6 in main (argc=1, argv=0x7ffd7e1c7b08) at gdb.c:32
```

* GDB stores `tdesc_info->tdesc` into the above `info.target_desc` at `~/binutils-gdb/gdb/tdesc-descriptions.c` line 568.

Actually,

```
~/hariboslinux # make debug
(gdb) break target-descriptions.c : 568
(gdb) run < debuggee_input.txt
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdesc_info->tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdesc_info->tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The above `tdesc_info->tdesc` is stored at `target-descriptions.c` line 554.

```
    tdesc_info->tdesc = target_read_description_xml
      (current_inferior ()->top_target ());
```

Actually,

```
~/hariboslinux # make debug
(gdb) break target_find_description
(gdb) run < debuggee_input.txt
(gdb) watch tdesc_info
(gdb) continue
tdesc_info is stored
target-descriptions.c : 537
(gdb) delete 2
(gdb) watch tdesc_info->tdesc
(gdb) continue
tdesc_info->tdesc is stored
target-descriptions.c : 558
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdesc_info->tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)tdesc_info->tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* `data.tdesc` is returned at `~/binutils-gdb/gdb/xml-tdesc.c` line 649 and stored to `tdesc_info->tdesc`.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-tdesc.c : 649
(gdb) run < debuggee_input.txt
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)data.tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)data.tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The register layaout is determined at `~/binutils-gdb/gdb/xml-support.c` line 627.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-support.c : 628
(gdb) run < debuggee_input.txt
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)((tdesc_parsing_data*)parser.m_user_data)->tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)((tdesc_parsing_data*)parser.m_user_data)->tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* The register layaout is determined at `~/binutils-gdb/gdb/xml-support.c` line 587.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-support.c : 627
(gdb) run < debuggee_input.txt
(gdb) break xml-support.c : 589
(gdb) continue
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)((tdesc_parsing_data*)this->m_user_data)->tdesc->features.begin())->get()->registers.begin())->get()->name._M_dataplus._M_p
$1 = "eax"
(gdb) print ((tdesc_reg_up*)((tdesc_feature_up*)((tdesc_parsing_data*)this->m_user_data)->tdesc->features.begin())->get()->registers.begin())->get()->target_regnum
$2 = 0
```

* `tdesc_reg::tdesc_reg` at `~/binutils-gdb/gdbsupport/tdesc.cc` line 26 creates the register informations.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-support.c : 627
(gdb) run < debuggee_input.txt
(gdb) break tdesc_reg::tdesc_reg
(gdb) continue
~/binutils-gdb/gdbsupport/tdesc.cc : 26 tdesc_reg::tdesc_reg
(gdb) print name_._M_dataplus._M_p
$1 = "eax"
(gdb) p/x regnum
$2 = 0x0
(gdb) p/x bitsize_
$2 = 0x20
(gdb) backtrace
#0  tdesc_reg::tdesc_reg (this=0x55e57094d3c0, feature=0x55e570963b20, name_=..., regnum=0, save_restore_=1,
		    group_=0x0, bitsize_=32, type_=0x55e570959b60 "int32") at tdesc.cc:23
#1  0x000055e56f52ee6f in tdesc_create_reg (feature=0x55e570963b20, name=0x55e570964590 "eax", regnum=0,
		    save_restore=1, group=0x0, bitsize=32, type=0x55e570959b60 "int32") at tdesc.cc:144
#2  0x000055e56f3ad98f in tdesc_start_reg (parser=0x7ffd67d47380, element=0x55e56f981da0 <feature_children>,
		    user_data=0x7ffd67d47440, attributes=...) at xml-tdesc.c:215
#3  0x000055e56f3a488f in gdb_xml_parser::start_element (this=0x7ffd67d47380, name=0x55e57095c13c "reg",
		    attrs=0x55e570959f60) at xml-support.c:367
#4  0x000055e56f3a4946 in gdb_xml_start_element_wrapper (data=0x7ffd67d47380, name=0x55e57095c13c "reg",
		    attrs=0x55e570959f60) at xml-support.c:389
#5  0x00007facdd38049c in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#6  0x00007facdd37ed7e in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#7  0x00007facdd37fc6c in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#8  0x00007facdd383db5 in XML_ParseBuffer () from /lib/x86_64-linux-gnu/libexpat.so.1
#9  0x000055e56f3a507b in gdb_xml_parser::parse (this=0x7ffd67d47380,
		    buffer=0x55e570967e70 "...") at xml-support.c:587
#10 0x000055e56f3a52c4 in gdb_xml_parse_quick (name=0x55e56f695cae "target description",
		    dtd_name=0x55e56f695cf8 "gdb-target.dtd", elements=0x55e56f982040 <tdesc_elements>,
		        document=0x55e570967e70 "...", user_data=0x7ffd67d47440)
    at xml-support.c:627
#11 0x000055e56f3ae548 in tdesc_parse_xml (
		    document=0x55e570957540 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>", fetcher=...) at xml-tdesc.c:644
#12 0x000055e56f3ae915 in target_read_description_xml (ops=0x55e570903ab0) at xml-tdesc.c:717
#13 0x000055e56f296e76 in target_find_description () at target-descriptions.c:555
#14 0x000055e56f1cb3cf in remote_target::start_remote_1 (this=0x55e570903ab0, from_tty=1, extended_p=0)
    at remote.c:4833
#15 0x000055e56f1cbebc in remote_target::start_remote (this=0x55e570903ab0, from_tty=1, extended_p=0) at remote.c:5070
#16 0x000055e56f1cd6d1 in remote_target::open_1 (name=0x55e570870f4e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#17 0x000055e56f1cbf57 in remote_target::open (name=0x55e570870f4e "localhost:2159", from_tty=1) at remote.c:5092
#18 0x000055e56f2afa5a in open_target (args=0x55e570870f4e "localhost:2159", from_tty=1, command=0x55e5708e2a10)
	    at target.c:853
#19 0x000055e56ee07a07 in cmd_func (cmd=0x55e5708e2a10, args=0x55e570870f4e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2543
#20 0x000055e56f2de1d1 in execute_command (p=0x55e570870f5b "9", from_tty=1) at top.c:699
#21 0x000055e56efaa91c in command_handler (command=0x55e570870f40 "") at event-top.c:598
#22 0x000055e56efaae5a in command_line_handler (rl=...) at event-top.c:842
#23 0x000055e56efab00e in gdb_readline_no_editing_callback (client_data=0x55e570876560) at event-top.c:907
#24 0x000055e56efaa71e in stdin_event_handler (error=0, client_data=0x55e570876560) at event-top.c:525
#25 0x000055e56f5249f3 in handle_file_event (file_ptr=0x55e57094cdc0, ready_mask=1) at event-loop.cc:574
#26 0x000055e56f524fe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#27 0x000055e56f523c8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#28 0x000055e56f0d5635 in start_event_loop () at main.c:411
#29 0x000055e56f0d5782 in captured_command_loop () at main.c:471
#30 0x000055e56f0d7240 in captured_main (data=0x7ffd67d47e00) at main.c:1329
#31 0x000055e56f0d72b2 in gdb_main (args=0x7ffd67d47e00) at main.c:1344
#32 0x000055e56ec8bde6 in main (argc=1, argv=0x7ffd67d47f38) at gdb.c:32
```

| name_._M_dataplus._M_p | regnum | bitsize_ |
| :--------------------- | -----: | -------: |
| eax                    | 0x00   | 0x20     |
| ecx                    | 0x01   | 0x20     |
| edx                    | 0x02   | 0x20     |
| ebx                    | 0x03   | 0x20     |
| esp                    | 0x04   | 0x20     |
| ebp                    | 0x05   | 0x20     |
| esi                    | 0x06   | 0x20     |
| edi                    | 0x07   | 0x20     |
| eip                    | 0x08   | 0x20     |
| eflags                 | 0x09   | 0x20     |
| cs                     | 0x0a   | 0x20     |
| ss                     | 0x0b   | 0x20     |
| ds                     | 0x0c   | 0x20     |
| es                     | 0x0d   | 0x20     |
| fs                     | 0x0e   | 0x20     |
| gs                     | 0x0f   | 0x20     |
| fs_base                | 0x10   | 0x20     |
| gs_base                | 0x11   | 0x20     |
| k_gs_base              | 0x12   | 0x20     |
| cr0                    | 0x13   | 0x20     |
| cr2                    | 0x14   | 0x20     |
| cr3                    | 0x15   | 0x20     |
| cr4                    | 0x16   | 0x20     |
| cr8                    | 0x17   | 0x20     |
| efer                   | 0x18   | 0x20     |
| st0                    | 0x19   | 0x50     |
| st1                    | 0x1a   | 0x50     |
| st2                    | 0x1b   | 0x50     |
| st3                    | 0x1c   | 0x50     |
| st4                    | 0x1d   | 0x50     |
| st5                    | 0x1e   | 0x50     |
| st6                    | 0x1f   | 0x50     |
| st7                    | 0x20   | 0x50     |
| fctrl                  | 0x21   | 0x20     |
| fstat                  | 0x22   | 0x20     |
| ftag                   | 0x23   | 0x20     |
| fiseg                  | 0x24   | 0x20     |
| fioff                  | 0x25   | 0x20     |
| foseg                  | 0x26   | 0x20     |
| fooff                  | 0x27   | 0x20     |
| fop                    | 0x28   | 0x20     |
| xmm0                   | 0x29   | 0x80     |
| xmm1                   | 0x2a   | 0x80     |
| xmm2                   | 0x2b   | 0x80     |
| xmm3                   | 0x2c   | 0x80     |
| xmm4                   | 0x2d   | 0x80     |
| xmm5                   | 0x2e   | 0x80     |
| xmm6                   | 0x2f   | 0x80     |
| xmm7                   | 0x30   | 0x80     |
| mxcsr                  | 0x31   | 0x20     |

* The informations of the above table are got from below xml by a function `tdesc_start_reg` at `~/binutils-gdb/gdb/xml-tdesc.c` line 173.
* The 4th argument of the function `gdb_xml_parse_quick` at `~/binutils-gdb/gdb/xml-support.c` line 622 is maybe xml representing register layout sent by QEMU.
* Where does the xml come from?

Actually,

```
~/hariboslinux # make debug
(gdb) break gdb_xml_parse_quick
(gdb) run < debuggee_input.txt
(gdb) printf "%s\n", document
<!DOCTYPE target SYSTEM "gdb-target.dtd"><target><architecture>i386</architecture>
<!-- Copyright (C) 2010-2017 Free Software Foundation, Inc.

     Copying and distribution of this file, with or without modification,
     are permitted in any medium without royalty provided the copyright
     notice and this notice are preserved.  -->

<!-- I386 with SSE -->


<feature name="org.gnu.gdb.i386.core">
  <flags id="i386_eflags" size="4">
        <field name="" start="22" end="31"/>
        <field name="ID" start="21" end="21"/>
        <field name="VIP" start="20" end="20"/>
        <field name="VIF" start="19" end="19"/>
        <field name="AC" start="18" end="18"/>
        <field name="VM" start="17" end="17"/>
        <field name="RF" start="16" end="16"/>
        <field name="" start="15" end="15"/>
        <field name="NT" start="14" end="14"/>
        <field name="IOPL" start="12" end="13"/>
        <field name="OF" start="11" end="11"/>
        <field name="DF" start="10" end="10"/>
        <field name="IF" start="9" end="9"/>
        <field name="TF" start="8" end="8"/>
        <field name="SF" start="7" end="7"/>
        <field name="ZF" start="6" end="6"/>
        <field name="" start="5" end="5"/>
        <field name="AF" start="4" end="4"/>
        <field name="" start="3" end="3"/>
        <field name="PF" start="2" end="2"/>
        <field name="" start="1" end="1"/>
        <field name="CF" start="0" end="0"/>
  </flags>

  <reg name="eax" bitsize="32" type="int32" regnum="0"/>
  <reg name="ecx" bitsize="32" type="int32"/>
  <reg name="edx" bitsize="32" type="int32"/>
  <reg name="ebx" bitsize="32" type="int32"/>
  <reg name="esp" bitsize="32" type="data_ptr"/>
  <reg name="ebp" bitsize="32" type="data_ptr"/>
  <reg name="esi" bitsize="32" type="int32"/>
  <reg name="edi" bitsize="32" type="int32"/>
  <reg name="eip" bitsize="32" type="code_ptr"/>
  <reg name="eflags" bitsize="32" type="i386_eflags"/>

  <reg name="cs" bitsize="32" type="int32"/>
  <reg name="ss" bitsize="32" type="int32"/>
  <reg name="ds" bitsize="32" type="int32"/>
  <reg name="es" bitsize="32" type="int32"/>
  <reg name="fs" bitsize="32" type="int32"/>
  <reg name="gs" bitsize="32" type="int32"/>

  <!-- Segment descriptor caches and TLS base MSRs -->

  <!--reg name="cs_base" bitsize="32" type="int32"/>
  <reg name="ss_base" bitsize="32" type="int32"/>
  <reg name="ds_base" bitsize="32" type="int32"/>
  <reg name="es_base" bitsize="32" type="int32"/-->
  <reg name="fs_base" bitsize="32" type="int32"/>
  <reg name="gs_base" bitsize="32" type="int32"/>
  <reg name="k_gs_base" bitsize="32" type="int32"/>

  <flags id="i386_cr0" size="4">
        <field name="PG" start="31" end="31"/>
        <field name="CD" start="30" end="30"/>
        <field name="NW" start="29" end="29"/>
        <field name="AM" start="18" end="18"/>
        <field name="WP" start="16" end="16"/>
        <field name="NE" start="5" end="5"/>
        <field name="ET" start="4" end="4"/>
        <field name="TS" start="3" end="3"/>
        <field name="EM" start="2" end="2"/>
        <field name="MP" start="1" end="1"/>
        <field name="PE" start="0" end="0"/>
  </flags>

  <flags id="i386_cr3" size="4">
        <field name="PDBR" start="12" end="31"/>
        <!--field name="" start="3" end="11"/>
        <field name="WT" start="2" end="2"/>
        <field name="CD" start="1" end="1"/>
        <field name="" start="0" end="0"/-->
        <field name="PCID" start="0" end="11"/>
  </flags>

  <flags id="i386_cr4" size="4">
        <field name="VME" start="0" end="0"/>
        <field name="PVI" start="1" end="1"/>
        <field name="TSD" start="2" end="2"/>
        <field name="DE" start="3" end="3"/>
        <field name="PSE" start="4" end="4"/>
        <field name="PAE" start="5" end="5"/>
        <field name="MCE" start="6" end="6"/>
        <field name="PGE" start="7" end="7"/>
        <field name="PCE" start="8" end="8"/>
        <field name="OSFXSR" start="9" end="9"/>
        <field name="OSXMMEXCPT" start="10" end="10"/>
        <field name="UMIP" start="11" end="11"/>
        <field name="LA57" start="12" end="12"/>
        <field name="VMXE" start="13" end="13"/>
        <field name="SMXE" start="14" end="14"/>
        <field name="FSGSBASE" start="16" end="16"/>
        <field name="PCIDE" start="17" end="17"/>
        <field name="OSXSAVE" start="18" end="18"/>
        <field name="SMEP" start="20" end="20"/>
        <field name="SMAP" start="21" end="21"/>
        <field name="PKE" start="22" end="22"/>
  </flags>

  <flags id="i386_efer" size="8">
        <field name="TCE" start="15" end="15"/>
        <field name="FFXSR" start="14" end="14"/>
        <field name="LMSLE" start="13" end="13"/>
        <field name="SVME" start="12" end="12"/>
        <field name="NXE" start="11" end="11"/>
        <field name="LMA" start="10" end="10"/>
        <field name="LME" start="8" end="8"/>
        <field name="SCE" start="0" end="0"/>
  </flags>

  <reg name="cr0" bitsize="32" type="i386_cr0"/>
  <reg name="cr2" bitsize="32" type="int32"/>
  <reg name="cr3" bitsize="32" type="i386_cr3"/>
  <reg name="cr4" bitsize="32" type="i386_cr4"/>
  <reg name="cr8" bitsize="32" type="int32"/>
  <reg name="efer" bitsize="32" type="i386_efer"/>

  <reg name="st0" bitsize="80" type="i387_ext"/>
  <reg name="st1" bitsize="80" type="i387_ext"/>
  <reg name="st2" bitsize="80" type="i387_ext"/>
  <reg name="st3" bitsize="80" type="i387_ext"/>
  <reg name="st4" bitsize="80" type="i387_ext"/>
  <reg name="st5" bitsize="80" type="i387_ext"/>
  <reg name="st6" bitsize="80" type="i387_ext"/>
  <reg name="st7" bitsize="80" type="i387_ext"/>

  <reg name="fctrl" bitsize="32" type="int" group="float"/>
  <reg name="fstat" bitsize="32" type="int" group="float"/>
  <reg name="ftag" bitsize="32" type="int" group="float"/>
  <reg name="fiseg" bitsize="32" type="int" group="float"/>
  <reg name="fioff" bitsize="32" type="int" group="float"/>
  <reg name="foseg" bitsize="32" type="int" group="float"/>
  <reg name="fooff" bitsize="32" type="int" group="float"/>
  <reg name="fop" bitsize="32" type="int" group="float"/>
<!--/feature>
<feature name="org.gnu.gdb.i386.32bit.sse"-->
  <vector id="v4f" type="ieee_single" count="4"/>
  <vector id="v2d" type="ieee_double" count="2"/>
  <vector id="v16i8" type="int8" count="16"/>
  <vector id="v8i16" type="int16" count="8"/>
  <vector id="v4i32" type="int32" count="4"/>
  <vector id="v2i64" type="int64" count="2"/>
  <union id="vec128">
        <field name="v4_float" type="v4f"/>
        <field name="v2_double" type="v2d"/>
        <field name="v16_int8" type="v16i8"/>
        <field name="v8_int16" type="v8i16"/>
        <field name="v4_int32" type="v4i32"/>
        <field name="v2_int64" type="v2i64"/>
        <field name="uint128" type="uint128"/>
  </union>
  <flags id="i386_mxcsr" size="4">
        <field name="IE" start="0" end="0"/>
        <field name="DE" start="1" end="1"/>
        <field name="ZE" start="2" end="2"/>
        <field name="OE" start="3" end="3"/>
        <field name="UE" start="4" end="4"/>
        <field name="PE" start="5" end="5"/>
        <field name="DAZ" start="6" end="6"/>
        <field name="IM" start="7" end="7"/>
        <field name="DM" start="8" end="8"/>
        <field name="ZM" start="9" end="9"/>
        <field name="OM" start="10" end="10"/>
        <field name="UM" start="11" end="11"/>
        <field name="PM" start="12" end="12"/>
        <field name="FZ" start="15" end="15"/>
  </flags>

  <reg name="xmm0" bitsize="128" type="vec128"/>
  <reg name="xmm1" bitsize="128" type="vec128"/>
  <reg name="xmm2" bitsize="128" type="vec128"/>
  <reg name="xmm3" bitsize="128" type="vec128"/>
  <reg name="xmm4" bitsize="128" type="vec128"/>
  <reg name="xmm5" bitsize="128" type="vec128"/>
  <reg name="xmm6" bitsize="128" type="vec128"/>
  <reg name="xmm7" bitsize="128" type="vec128"/>

  <reg name="mxcsr" bitsize="32" type="i386_mxcsr" group="vector"/>
</feature>
</target>
(gdb) backtrace
#0  gdb_xml_parse_quick (name=0x55ca87f0dcae "target description", dtd_name=0x55ca87f0dcf8 "gdb-target.dtd",
		    elements=0x55ca881fa040 <tdesc_elements>,
		        document=0x55ca888e3e70 "...", user_data=0x7ffc50746460)
    at xml-support.c:623
#1  0x000055ca87c26548 in tdesc_parse_xml (
		    document=0x55ca888d3540 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>", fetcher=...) at xml-tdesc.c:644
#2  0x000055ca87c26915 in target_read_description_xml (ops=0x55ca8887fab0) at xml-tdesc.c:717
#3  0x000055ca87b0ee76 in target_find_description () at target-descriptions.c:555
#4  0x000055ca87a433cf in remote_target::start_remote_1 (this=0x55ca8887fab0, from_tty=1, extended_p=0)
    at remote.c:4833
#5  0x000055ca87a43ebc in remote_target::start_remote (this=0x55ca8887fab0, from_tty=1, extended_p=0) at remote.c:5070
#6  0x000055ca87a456d1 in remote_target::open_1 (name=0x55ca887ecf4e "localhost:2159", from_tty=1, extended_p=0)
        at remote.c:5873
#7  0x000055ca87a43f57 in remote_target::open (name=0x55ca887ecf4e "localhost:2159", from_tty=1) at remote.c:5092
#8  0x000055ca87b27a5a in open_target (args=0x55ca887ecf4e "localhost:2159", from_tty=1, command=0x55ca8885ea10)
	    at target.c:853
#9  0x000055ca8767fa07 in cmd_func (cmd=0x55ca8885ea10, args=0x55ca887ecf4e "localhost:2159", from_tty=1)
	        at cli/cli-decode.c:2543
#10 0x000055ca87b561d1 in execute_command (p=0x55ca887ecf5b "9", from_tty=1) at top.c:699
#11 0x000055ca8782291c in command_handler (command=0x55ca887ecf40 "") at event-top.c:598
#12 0x000055ca87822e5a in command_line_handler (rl=...) at event-top.c:842
#13 0x000055ca8782300e in gdb_readline_no_editing_callback (client_data=0x55ca887f2560) at event-top.c:907
#14 0x000055ca8782271e in stdin_event_handler (error=0, client_data=0x55ca887f2560) at event-top.c:525
#15 0x000055ca87d9c9f3 in handle_file_event (file_ptr=0x55ca888c8dc0, ready_mask=1) at event-loop.cc:574
#16 0x000055ca87d9cfe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#17 0x000055ca87d9bc8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#18 0x000055ca8794d635 in start_event_loop () at main.c:411
#19 0x000055ca8794d782 in captured_command_loop () at main.c:471
#20 0x000055ca8794f240 in captured_main (data=0x7ffc50746e20) at main.c:1329
#21 0x000055ca8794f2b2 in gdb_main (args=0x7ffc50746e20) at main.c:1344
#22 0x000055ca87503de6 in main (argc=1, argv=0x7ffc50746f58) at gdb.c:32
```

* The above xml is `expanded_text` at `~/binutils-gdb/gdb/xml-tdesc.c` line 636.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-tdesc.c : 636
(gdb) run < debuggee_input.txt
(gdb) printf "%s\n", expanded_test._M_dataplus._M_p
<!DOCTYPE target SYSTEM "gdb-target.dtd"><target><architecture>i386</architecture>
...
```

* The above xml is `tdesc_str->data()` at `~/binutils-gdb/gdb/xml-tdesc.c` line 717.

Actually,

```
~/hariboslinux # make debug
(gdb) break xml-tdesc.c : 717
(gdb) run < debuggee_input.txt
(gdb) print tdesc_str->data()
$1 = 0x5601ace96540 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>"
```

* The above xml is `buf` at `~/binutils-gdb/gdb/target.c` line 2357.

```
~/hariboslinux # make debug
(gdb) break target.c : 2357
(gdb) run < debuggee_input.txt
(gdb) print annex
$1 = 0x556a35e37d1b "target.xml"
(gdb) printf "%s\n", buf->data()
<?xml version="1.0"?><!DOCTYPE target SYSTEM "gdb-target.dtd"><target><architecture>i386</architecture><xi:include href="i386-32bit.xml"/></target>
(gdb) continue
(gdb) print annex
$2 = 0x556a36aa66a0 "i386-32bit.xml"
(gdb) printf "%s\n", buf->data()
<?xml version="1.0"?>
<!-- Copyright (C) 2010-2017 Free Software Foundation, Inc.

     Copying and distribution of this file, with or without modification,
     are permitted in any medium without royalty provided the copyright
     notice and this notice are preserved.  -->

<!-- I386 with SSE -->

<!DOCTYPE target SYSTEM "gdb-target.dtd">
<feature name="org.gnu.gdb.i386.core">
  <flags id="i386_eflags" size="4">
        <field name="" start="22" end="31"/>
        <field name="ID" start="21" end="21"/>
        <field name="VIP" start="20" end="20"/>
        <field name="VIF" start="19" end="19"/>
        <field name="AC" start="18" end="18"/>
        <field name="VM" start="17" end="17"/>
        <field name="RF" start="16" end="16"/>
        <field name="" start="15" end="15"/>
        <field name="NT" start="14" end="14"/>
        <field name="IOPL" start="12" end="13"/>
        <field name="OF" start="11" end="11"/>
        <field name="DF" start="10" end="10"/>
        <field name="IF" start="9" end="9"/>
        <field name="TF" start="8" end="8"/>
        <field name="SF" start="7" end="7"/>
        <field name="ZF" start="6" end="6"/>
        <field name="" start="5" end="5"/>
        <field name="AF" start="4" end="4"/>
        <field name="" start="3" end="3"/>
        <field name="PF" start="2" end="2"/>
        <field name="" start="1" end="1"/>
        <field name="CF" start="0" end="0"/>
  </flags>

  <reg name="eax" bitsize="32" type="int32" regnum="0"/>
  <reg name="ecx" bitsize="32" type="int32"/>
  <reg name="edx" bitsize="32" type="int32"/>
  <reg name="ebx" bitsize="32" type="int32"/>
  <reg name="esp" bitsize="32" type="data_ptr"/>
  <reg name="ebp" bitsize="32" type="data_ptr"/>
  <reg name="esi" bitsize="32" type="int32"/>
  <reg name="edi" bitsize="32" type="int32"/>
  <reg name="eip" bitsize="32" type="code_ptr"/>
  <reg name="eflags" bitsize="32" type="i386_eflags"/>

  <reg name="cs" bitsize="32" type="int32"/>
  <reg name="ss" bitsize="32" type="int32"/>
  <reg name="ds" bitsize="32" type="int32"/>
  <reg name="es" bitsize="32" type="int32"/>
  <reg name="fs" bitsize="32" type="int32"/>
  <reg name="gs" bitsize="32" type="int32"/>

  <!-- Segment descriptor caches and TLS base MSRs -->

  <!--reg name="cs_base" bitsize="32" type="int32"/>
  <reg name="ss_base" bitsize="32" type="int32"/>
  <reg name="ds_base" bitsize="32" type="int32"/>
  <reg name="es_base" bitsize="32" type="int32"/-->
  <reg name="fs_base" bitsize="32" type="int32"/>
  <reg name="gs_base" bitsize="32" type="int32"/>
  <reg name="k_gs_base" bitsize="32" type="int32"/>

  <flags id="i386_cr0" size="4">
        <field name="PG" start="31" end="31"/>
        <field name="CD" start="30" end="30"/>
        <field name="NW" start="29" end="29"/>
        <field name="AM" start="18" end="18"/>
        <field name="WP" start="16" end="16"/>
        <field name="NE" start="5" end="5"/>
        <field name="ET" start="4" end="4"/>
        <field name="TS" start="3" end="3"/>
        <field name="EM" start="2" end="2"/>
        <field name="MP" start="1" end="1"/>
        <field name="PE" start="0" end="0"/>
  </flags>

  <flags id="i386_cr3" size="4">
        <field name="PDBR" start="12" end="31"/>
        <!--field name="" start="3" end="11"/>
        <field name="WT" start="2" end="2"/>
        <field name="CD" start="1" end="1"/>
        <field name="" start="0" end="0"/-->
        <field name="PCID" start="0" end="11"/>
  </flags>

  <flags id="i386_cr4" size="4">
        <field name="VME" start="0" end="0"/>
        <field name="PVI" start="1" end="1"/>
        <field name="TSD" start="2" end="2"/>
        <field name="DE" start="3" end="3"/>
        <field name="PSE" start="4" end="4"/>
        <field name="PAE" start="5" end="5"/>
        <field name="MCE" start="6" end="6"/>
        <field name="PGE" start="7" end="7"/>
        <field name="PCE" start="8" end="8"/>
        <field name="OSFXSR" start="9" end="9"/>
        <field name="OSXMMEXCPT" start="10" end="10"/>
        <field name="UMIP" start="11" end="11"/>
        <field name="LA57" start="12" end="12"/>
        <field name="VMXE" start="13" end="13"/>
        <field name="SMXE" start="14" end="14"/>
        <field name="FSGSBASE" start="16" end="16"/>
        <field name="PCIDE" start="17" end="17"/>
        <field name="OSXSAVE" start="18" end="18"/>
        <field name="SMEP" start="20" end="20"/>
        <field name="SMAP" start="21" end="21"/>
        <field name="PKE" start="22" end="22"/>
  </flags>

  <flags id="i386_efer" size="8">
        <field name="TCE" start="15" end="15"/>
        <field name="FFXSR" start="14" end="14"/>
        <field name="LMSLE" start="13" end="13"/>
        <field name="SVME" start="12" end="12"/>
        <field name="NXE" start="11" end="11"/>
        <field name="LMA" start="10" end="10"/>
        <field name="LME" start="8" end="8"/>
        <field name="SCE" start="0" end="0"/>
  </flags>

  <reg name="cr0" bitsize="32" type="i386_cr0"/>
  <reg name="cr2" bitsize="32" type="int32"/>
  <reg name="cr3" bitsize="32" type="i386_cr3"/>
  <reg name="cr4" bitsize="32" type="i386_cr4"/>
  <reg name="cr8" bitsize="32" type="int32"/>
  <reg name="efer" bitsize="32" type="i386_efer"/>

  <reg name="st0" bitsize="80" type="i387_ext"/>
  <reg name="st1" bitsize="80" type="i387_ext"/>
  <reg name="st2" bitsize="80" type="i387_ext"/>
  <reg name="st3" bitsize="80" type="i387_ext"/>
  <reg name="st4" bitsize="80" type="i387_ext"/>
  <reg name="st5" bitsize="80" type="i387_ext"/>
  <reg name="st6" bitsize="80" type="i387_ext"/>
  <reg name="st7" bitsize="80" type="i387_ext"/>

  <reg name="fctrl" bitsize="32" type="int" group="float"/>
  <reg name="fstat" bitsize="32" type="int" group="float"/>
  <reg name="ftag" bitsize="32" type="int" group="float"/>
  <reg name="fiseg" bitsize="32" type="int" group="float"/>
  <reg name="fioff" bitsize="32" type="int" group="float"/>
  <reg name="foseg" bitsize="32" type="int" group="float"/>
  <reg name="fooff" bitsize="32" type="int" group="float"/>
  <reg name="fop" bitsize="32" type="int" group="float"/>
<!--/feature>
<feature name="org.gnu.gdb.i386.32bit.sse"-->
  <vector id="v4f" type="ieee_single" count="4"/>
  <vector id="v2d" type="ieee_double" count="2"/>
  <vector id="v16i8" type="int8" count="16"/>
  <vector id="v8i16" type="int16" count="8"/>
  <vector id="v4i32" type="int32" count="4"/>
  <vector id="v2i64" type="int64" count="2"/>
  <union id="vec128">
        <field name="v4_float" type="v4f"/>
        <field name="v2_double" type="v2d"/>
        <field name="v16_int8" type="v16i8"/>
        <field name="v8_int16" type="v8i16"/>
        <field name="v4_int32" type="v4i32"/>
        <field name="v2_int64" type="v2i64"/>
        <field name="uint128" type="uint128"/>
  </union>
  <flags id="i386_mxcsr" size="4">
        <field name="IE" start="0" end="0"/>
        <field name="DE" start="1" end="1"/>
        <field name="ZE" start="2" end="2"/>
        <field name="OE" start="3" end="3"/>
        <field name="UE" start="4" end="4"/>
        <field name="PE" start="5" end="5"/>
        <field name="DAZ" start="6" end="6"/>
        <field name="IM" start="7" end="7"/>
        <field name="DM" start="8" end="8"/>
        <field name="ZM" start="9" end="9"/>
        <field name="OM" start="10" end="10"/>
        <field name="UM" start="11" end="11"/>
        <field name="PM" start="12" end="12"/>
        <field name="FZ" start="15" end="15"/>
  </flags>

  <reg name="xmm0" bitsize="128" type="vec128"/>
  <reg name="xmm1" bitsize="128" type="vec128"/>
  <reg name="xmm2" bitsize="128" type="vec128"/>
  <reg name="xmm3" bitsize="128" type="vec128"/>
  <reg name="xmm4" bitsize="128" type="vec128"/>
  <reg name="xmm5" bitsize="128" type="vec128"/>
  <reg name="xmm6" bitsize="128" type="vec128"/>
  <reg name="xmm7" bitsize="128" type="vec128"/>

  <reg name="mxcsr" bitsize="32" type="i386_mxcsr" group="vector"/>
</feature>
```

* The above xml is `buffer`, the first argument of the function `remote_unescape_input` at `~/binutils-gdb/gdbsupport/rsp-low.cc` line 235.

Actually,

```
~/hariboslinux # make debug
(gdb) break remote_unescape_input
(gdb) run < debuggee_input.txt
(gdb) printf "%s\n", buffer
<?xml version="1.0"?><!DOCTYPE target SYSTEM "gdb-target.dtd"><target><architecture>i386</architecture><xi:include href="i386-32bit.xml"/></target>
(gdb) continue
(gdb) printf "%s\n", buffer
<?xml version="1.0"?>
<!-- Copyright (C) 2010-2017 Free Software Foundation, Inc.

     Copying and distribution of this file, with or without modification,
     are permitted in any medium without royalty provided the copyright
     notice and this notice are preserved.  -->

<!-- I386 with SSE -->

<!DOCTYPE target SYSTEM "gdb-target.dtd">
<feature name="org.gnu.gdb.i386.core">
  <flags id="i386_eflags" size="4">
        <field name="" start="22" end="31"/>
        <field name="ID" start="21" end="21"/>
        <field name="VIP" start="20" end="20"/>
        <field name="VIF" start="19" end="19"/>
        <field name="AC" start="18" end="18"/>
        <field name="VM" start="17" end="17"/>
        <field name="RF" start="16" end="16"/>
        <field name="" start="15" end="15"/>
        <field name="NT" start="14" end="14"/>
        <field name="IOPL" start="12" end="13"/>
        <field name="OF" start="11" end="11"/>
        <field name="DF" start="10" end="10"/>
        <field name="IF" start="9" end="9"/>
        <field name="TF" start="8" end="8"/>
        <field name="SF" start="7" end="7"/>
        <field name="ZF" start="6" end="6"/>
        <field name="" start="5" end="5"/>
        <field name="AF" start="4" end="4"/>
        <field name="" start="3" end="3"/>
        <field name="PF" start="2" end="2"/>
        <field name="" start="1" end="1"/>
        <field name="CF" start="0" end="0"/>
  </flags>

  <reg name="eax" bitsize="32" type="int32" regnum="0"/>
  <reg name="ecx" bitsize="32" type="int32"/>
  <reg name="edx" bitsize="32" type="int32"/>
  <reg name="ebx" bitsize="32" type="int32"/>
  <reg name="esp" bitsize="32" type="data_ptr"/>
  <reg name="ebp" bitsize="32" type="data_ptr"/>
  <reg name="esi" bitsize="32" type="int32"/>
  <reg name="edi" bitsize="32" type="int32"/>

  <reg name="eip" bitsize="32" type="code_ptr"/>
  <reg name="eflags" bitsize="32" type="i386_eflags"/>

  <reg name="cs" bitsize="32" type="int32"/>
  <reg name="ss" bitsize="32" type="int32"/>
  <reg name="ds" bitsize="32" type="int32"/>
  <reg name="es" bitsize="32" type="int32"/>
  <reg name="fs" bitsize="32" type="int32"/>
  <reg name="gs" bitsize="32" type="int32"/>
(gdb) continue
(gdb) printf "%s\n", buffer


  <!-- Segment descriptor caches and TLS base MSRs -->

  <!--reg name="cs_base" bitsize="32" type="int32"/>
  <reg name="ss_base" bitsize="32" type="int32"/>
  <reg name="ds_base" bitsize="32" type="int32"/>
  <reg name="es_base" bitsize="32" type="int32"/-->
  <reg name="fs_base" bitsize="32" type="int32"/>
  <reg name="gs_base" bitsize="32" type="int32"/>
  <reg name="k_gs_base" bitsize="32" type="int32"/>

  <flags id="i386_cr0" size="4">
        <field name="PG" start="31" end="31"/>
        <field name="CD" start="30" end="30"/>
        <field name="NW" start="29" end="29"/>
        <field name="AM" start="18" end="18"/>
        <field name="WP" start="16" end="16"/>
        <field name="NE" start="5" end="5"/>
        <field name="ET" start="4" end="4"/>
        <field name="TS" start="3" end="3"/>
        <field name="EM" start="2" end="2"/>
        <field name="MP" start="1" end="1"/>
        <field name="PE" start="0" end="0"/>
  </flags>

  <flags id="i386_cr3" size="4">
        <field name="PDBR" start="12" end="31"/>
        <!--field name="" start="3" end="11"/>
        <field name="WT" start="2" end="2"/>
        <field name="CD" start="1" end="1"/>
        <field name="" start="0" end="0"/-->
        <field name="PCID" start="0" end="11"/>
  </flags>

  <flags id="i386_cr4" size="4">
        <field name="VME" start="0" end="0"/>
        <field name="PVI" start="1" end="1"/>
        <field name="TSD" start="2" end="2"/>
        <field name="DE" start="3" end="3"/>
        <field name="PSE" start="4" end="4"/>
        <field name="PAE" start="5" end="5"/>
        <field name="MCE" start="6" end="6"/>
        <field name="PGE" start="7" end="7"/>
        <field name="PCE" start="8" end="8"/>
        <field name="OSFXSR" start="9" end="9"/>
        <field name="OSXMMEXCPT" start="10" end="10"/>
        <field name="UMIP" start="11" end="11"/>
        <field name="LA57" start="12" end="12"/>
        <field name="VMXE" start="13" end="13"/>
        <field name="SMXE" start="14" end="14"/>
        <field name="FSGSBASE" start="16" end="16"/>
        <field name="PCIDE" start="17" end="17"/>
        <field name="OSXSAVE" start="18" end="18"/>
        <field name="SMEP" start="20" end="20"/>
        <field name="SMAP" start="21" end="21"/>
        <field name="P
(gdb) continue
(gdb) printf "%s\n", buffer
KE" start="22" end="22"/>
  </flags>

  <flags id="i386_efer" size="8">
        <field name="TCE" start="15" end="15"/>
        <field name="FFXSR" start="14" end="14"/>
        <field name="LMSLE" start="13" end="13"/>
        <field name="SVME" start="12" end="12"/>
        <field name="NXE" start="11" end="11"/>
        <field name="LMA" start="10" end="10"/>
        <field name="LME" start="8" end="8"/>
        <field name="SCE" start="0" end="0"/>
  </flags>

  <reg name="cr0" bitsize="32" type="i386_cr0"/>
  <reg name="cr2" bitsize="32" type="int32"/>
  <reg name="cr3" bitsize="32" type="i386_cr3"/>
  <reg name="cr4" bitsize="32" type="i386_cr4"/>
  <reg name="cr8" bitsize="32" type="int32"/>
  <reg name="efer" bitsize="32" type="i386_efer"/>

  <reg name="st0" bitsize="80" type="i387_ext"/>
  <reg name="st1" bitsize="80" type="i387_ext"/>
  <reg name="st2" bitsize="80" type="i387_ext"/>
  <reg name="st3" bitsize="80" type="i387_ext"/>
  <reg name="st4" bitsize="80" type="i387_ext"/>
  <reg name="st5" bitsize="80" type="i387_ext"/>
  <reg name="st6" bitsize="80" type="i387_ext"/>
  <reg name="st7" bitsize="80" type="i387_ext"/>

  <reg name="fctrl" bitsize="32" type="int" group="float"/>
  <reg name="fstat" bitsize="32" type="int" group="float"/>
  <reg name="ftag" bitsize="32" type="int" group="float"/>
  <reg name="fiseg" bitsize="32" type="int" group="float"/>
  <reg name="fioff" bitsize="32" type="int" group="float"/>
  <reg name="foseg" bitsize="32" type="int" group="float"/>
  <reg name="fooff" bitsize="32" type="int" group="float"/>
  <reg name="fop" bitsize="32" type="int" group="float"/>
<!--/feature>
<feature name="org.gnu.gdb.i386.32bit.sse"-->
  <vector id="v4f" type="ieee_single" count="4"/>
  <vector id="v2d" type="ieee_double" count="2"/>
  <vector id="v16i8" type="int8" count="16"/>
  <vector id="v8i16" type="int16" count="8"/>
  <vector id="v4i32" type="int32" count="4"/>
  <vector id="v2i64" type="int64" count="2"/>
  <union id="vec128">
        <field name="v4_float" type="v4f"/>
        <field name="v2_double" type="v2d"/>
        <field name="v16_int8" type="
(gdb) continue
(gdb) printf "%s\n", buffer
v16i8"/>
        <field name="v8_int16" type="v8i16"/>
        <field name="v4_int32" type="v4i32"/>
        <field name="v2_int64" type="v2i64"/>
        <field name="uint128" type="uint128"/>
  </union>
  <flags id="i386_mxcsr" size="4">
        <field name="IE" start="0" end="0"/>
        <field name="DE" start="1" end="1"/>
        <field name="ZE" start="2" end="2"/>
        <field name="OE" start="3" end="3"/>
        <field name="UE" start="4" end="4"/>
        <field name="PE" start="5" end="5"/>
        <field name="DAZ" start="6" end="6"/>
        <field name="IM" start="7" end="7"/>
        <field name="DM" start="8" end="8"/>
        <field name="ZM" start="9" end="9"/>
        <field name="OM" start="10" end="10"/>
        <field name="UM" start="11" end="11"/>
        <field name="PM" start="12" end="12"/>
        <field name="FZ" start="15" end="15"/>
  </flags>

  <reg name="xmm0" bitsize="128" type="vec128"/>
  <reg name="xmm1" bitsize="128" type="vec128"/>
  <reg name="xmm2" bitsize="128" type="vec128"/>
  <reg name="xmm3" bitsize="128" type="vec128"/>
  <reg name="xmm4" bitsize="128" type="vec128"/>
  <reg name="xmm5" bitsize="128" type="vec128"/>
  <reg name="xmm6" bitsize="128" type="vec128"/>
  <reg name="xmm7" bitsize="128" type="vec128"/>

  <reg name="mxcsr" bitsize="32" type="i386_mxcsr" group="vector"/>
</feature>
(gdb) backtrace
#0  remote_unescape_input (
    buffer=0x561134ebc541 "v16i8\"/>\n\t<field name=\"v8_int16\" type=\"v8i16\"/>\n\t<field name=\"v4_int32\" type=\"v4i32\"/>\n\t<field name=\"v2_int64\" type=\"v2i64\"/>\n\t<field name=\"uint128\" type=\"uint128\"/>\n  </union>\n  <flags id=\"i386_mxcsr\" size=\"4\">\n\t<field name=\"IE\" start=\"0\" end=\"0\"/>\n\t<field name=\"DE\" start=\"1\" end=\"1\"/>\n\t<field name=\"ZE\" start=\"2\" end=\"2\"/>\n\t<field name=\"OE\" start=\"3\" end=\"3\"/>\n\t<field name=\"UE\" start=\"4\" end=\"4\"/>\n\t<field name=\"PE\" start=\"5\" end=\"5\"/>\n\t<field name=\"DAZ\" start=\"6\" end=\"6\"/>\n\t<field name=\"IM\" start=\"7\" end=\"7\"/>\n\t<field name=\"DM\" start=\"8\" end=\"8\"/>\n\t<field name=\"ZM\" start=\"9\" end=\"9\"/>\n\t<field name=\"OM\" start=\"10\" end=\"10\"/>\n\t<field name=\"UM\" start=\"11\" end=\"11\"/>\n\t<field name=\"PM\" start=\"12\" end=\"12\"/>\n\t<field name=\"FZ\" start=\"15\" end=\"15\"/>\n  </flags>\n\n  <reg name=\"xmm0\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm1\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm2\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm3\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm4\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm5\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm6\" bitsize=\"128\" type=\"vec128\"/>\n  <reg name=\"xmm7\" bitsize=\"128\" type=\"vec128\"/>\n\n  <reg name=\"mxcsr\" bitsize=\"32\" type=\"i386_mxcsr\" group=\"vector\"/>\n</feature>\n", len=1237, out_buf=0x561134ec82d7 "", out_maxlen=4091)
    at rsp-low.cc:233
#1  0x000056113320dc98 in remote_target::remote_read_qxfer (this=0x561134e6aab0,
    object_name=0x561133677c42 "features", annex=0x561134ec16a0 "i386-32bit.xml", readbuf=0x561134ec82d7 "",
    offset=6135, len=4096, xfered_len=0x7ffe3130c410, packet=0x561133a9ac58 <remote_protocol_packets+760>)
    at remote.c:11201
#2  0x000056113320e08b in remote_target::xfer_partial (this=0x561134e6aab0, object=TARGET_OBJECT_AVAILABLE_FEATURES,
    annex=0x561134ec16a0 "i386-32bit.xml", readbuf=0x561134ec82d7 "", writebuf=0x0, offset=6135, len=4096,
    xfered_len=0x7ffe3130c410) at remote.c:11312
#3  0x00005611332e3891 in target_xfer_partial (ops=0x561134e6aab0, object=TARGET_OBJECT_AVAILABLE_FEATURES,
    annex=0x561134ec16a0 "i386-32bit.xml", readbuf=0x561134ec82d7 "", writebuf=0x0, offset=6135, len=4096,
    xfered_len=0x7ffe3130c410) at target.c:1730
#4  0x00005611332e409b in target_read_partial (ops=0x561134e6aab0, object=TARGET_OBJECT_AVAILABLE_FEATURES,
    annex=0x561134ec16a0 "i386-32bit.xml", buf=0x561134ec82d7 "", offset=6135, len=4096, xfered_len=0x7ffe3130c410)
    at target.c:1964
#5  0x00005611332fb06c in target_read_alloc_1<char> (ops=0x561134e6aab0, object=TARGET_OBJECT_AVAILABLE_FEATURES,
    annex=0x561134ec16a0 "i386-32bit.xml") at target.c:2299
#6  0x00005611332e4b02 in target_read_stralloc (ops=0x561134e6aab0, object=TARGET_OBJECT_AVAILABLE_FEATURES,
    annex=0x561134ec16a0 "i386-32bit.xml") at target.c:2338
#7  0x00005611333e0807 in fetch_available_features_from_target (name=0x561134ec16a0 "i386-32bit.xml",
    ops=0x561134e6aab0) at xml-tdesc.c:697
#8  0x00005611333e085f in operator() (__closure=0x7ffe3130cbc8, name=0x561134ec16a0 "i386-32bit.xml")
    at xml-tdesc.c:714
#9  0x00005611333e0e4b in operator() (__closure=0x0, ecall=..., args#0=0x561134ec16a0 "i386-32bit.xml")
    at ./../gdbsupport/function-view.h:304
#10 0x00005611333e0ea5 in _FUN () at ./../gdbsupport/function-view.h:298
#11 0x00005611333d892f in gdb::function_view<gdb::optional<std::vector<char, gdb::default_init_allocator<char, std::allocator<char> > > > (char const*)>::operator()(char const*) const (this=0x7ffe3130ca60,
    args#0=0x561134ec16a0 "i386-32bit.xml") at ./../gdbsupport/function-view.h:288
#12 0x00005611333d7668 in xinclude_start_include (parser=0x7ffe3130ca70, element=0x5611339b3b80 <xinclude_elements>,
    user_data=0x7ffe3130ca50, attributes=...) at xml-support.c:790
#13 0x00005611333d688f in gdb_xml_parser::start_element (this=0x7ffe3130ca70,
    name=0x561134ec1660 "http://www.w3.org/2001/XInclude!include", attrs=0x561134ebacf0) at xml-support.c:367
#14 0x00005611333d6946 in gdb_xml_start_element_wrapper (data=0x7ffe3130ca70,
    name=0x561134ec1660 "http://www.w3.org/2001/XInclude!include", attrs=0x561134ebacf0) at xml-support.c:389
#15 0x00007fe0554db49c in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#16 0x00007fe0554d9d7e in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#17 0x00007fe0554dac6c in ?? () from /lib/x86_64-linux-gnu/libexpat.so.1
#18 0x00007fe0554dedb5 in XML_ParseBuffer () from /lib/x86_64-linux-gnu/libexpat.so.1
#19 0x00005611333d707b in gdb_xml_parser::parse (this=0x7ffe3130ca70,
    buffer=0x561134ebe560 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>") at xml-support.c:587
#20 0x00005611333d7a28 in xml_process_xincludes(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&, char const*, char const*, gdb::function_view<gdb::optional<std::vector<char, gdb::default_init_allocator<char, std::allocator<char> > > > (char const*)>, int) (result=..., name=0x5611336c7cae "target description",
    text=0x561134ebe560 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>", fetcher=..., depth=0) at xml-support.c:900
#21 0x00005611333e0441 in tdesc_parse_xml (
    document=0x561134ebe560 "<?xml version=\"1.0\"?><!DOCTYPE target SYSTEM \"gdb-target.dtd\"><target><architecture>i386</architecture><xi:include href=\"i386-32bit.xml\"/></target>", fetcher=...) at xml-tdesc.c:626
#22 0x00005611333e0915 in target_read_description_xml (ops=0x561134e6aab0) at xml-tdesc.c:717
#23 0x00005611332c8e76 in target_find_description () at target-descriptions.c:555
#24 0x00005611331fd3cf in remote_target::start_remote_1 (this=0x561134e6aab0, from_tty=1, extended_p=0)
    at remote.c:4833
#25 0x00005611331fdebc in remote_target::start_remote (this=0x561134e6aab0, from_tty=1, extended_p=0) at remote.c:5070
#26 0x00005611331ff6d1 in remote_target::open_1 (name=0x561134dd7f4e "localhost:2159", from_tty=1, extended_p=0)
    at remote.c:5873
#27 0x00005611331fdf57 in remote_target::open (name=0x561134dd7f4e "localhost:2159", from_tty=1) at remote.c:5092
#28 0x00005611332e1a5a in open_target (args=0x561134dd7f4e "localhost:2159", from_tty=1, command=0x561134e49a10)
    at target.c:853
#29 0x0000561132e39a07 in cmd_func (cmd=0x561134e49a10, args=0x561134dd7f4e "localhost:2159", from_tty=1)
    at cli/cli-decode.c:2543
#30 0x00005611333101d1 in execute_command (p=0x561134dd7f5b "9", from_tty=1) at top.c:699
#31 0x0000561132fdc91c in command_handler (command=0x561134dd7f40 "") at event-top.c:598
#32 0x0000561132fdce5a in command_line_handler (rl=...) at event-top.c:842
#33 0x0000561132fdd00e in gdb_readline_no_editing_callback (client_data=0x561134ddd560) at event-top.c:907
#34 0x0000561132fdc71e in stdin_event_handler (error=0, client_data=0x561134ddd560) at event-top.c:525
#35 0x00005611335569f3 in handle_file_event (file_ptr=0x561134eb3de0, ready_mask=1) at event-loop.cc:574
#36 0x0000561133556fe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#37 0x0000561133555c8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#38 0x0000561133107635 in start_event_loop () at main.c:411
#39 0x0000561133107782 in captured_command_loop () at main.c:471
#40 0x0000561133109240 in captured_main (data=0x7ffe3130d4f0) at main.c:1329
#41 0x00005611331092b2 in gdb_main (args=0x7ffe3130d4f0) at main.c:1344
#42 0x0000561132cbdde6 in main (argc=1, argv=0x7ffe3130d628) at gdb.c:32
```

So, the above xml is sent from QEMU.

# Why does GDB think that size of EFER register sent by QEMU is 0x08 bytes?

* The register number of EFER is 0x18.
* So I can observe decision of the size of EFER register.

Actually,

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
The first arrival to ~/binutils-gdb/gdb/remote.c : 1405 map_regcache_remote_table
(gdb) continue
The second arrival to the same breakpoint
(gdb) break 1441 if regnum == 0x18
(gdb) continue
(gdb) p/x regnum
$1 = 0x18
(gdb) p/x remote_regs[regnum]->regnum
$2 = 0x51
(gdb) step
~/binutils-gdb/gdb/regcache.c : 172 register_size
(gdb) p/x regnum
$3 = 0x51
(gdb) p/x regcache_descr(gdbarch)->sizeof_register[regnum]
$4 = 0x08
```

* Above `regcache_descr(gdbarch)` is `regcache_descr_handle.get(gdbarch)` in a function `regcache_descr` at `~/binutils-gdb/gdb/regcache.c` line 144.

Actually,

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
~/binutils-gdb/gdb/remote.c : 1405
(gdb) continue
~/binutils-gdb/gdb/remote.c : 1405
(gdb) break register_size if regnum == 0x51
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 172
(gdb) break regcache_descr
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 144
(gdb) p/x regcache_descr_handle.get(gdbarch)->sizeof_register[0x51]
$1 = 0x8
```

* The above `regcache_descr_handle.get(gdbarch)` is `((struct regcache_descr *)reg_obj->get(this->m_key))` at `~/binutils-gdb/gdb/registry.h` line 111.

Actually,

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
~/binutils-gdb/gdb/remote.c : 1405
(gdb) continue
~/binutils-gdb/gdb/remote.c : 1405
(gdb) break register_size if regnum == 0x51
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 172
(gdb) break regcache_descr
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 144
(gdb) step
~/binutils-gdb/gdb/registry.h : 110
(gdb) next
~/binutils-gdb/gdb/registry.h : 111
(gdb) p/x ((struct regcache_descr*)reg_obj->get(this->m_key))->sizeof_register[0x51]
$1 = 0x8
```

* The above `reg_obj` is `arch->registry_fields` in a function `registry_accessor<gdbarch>::get` at `~/binutils-gdb/gdb/arch-utils.c` line 1206.

Actually,

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
~/binutils-gdb/gdb/remote.c : 1405
(gdb) continue
~/binutils-gdb/gdb/remote.c : 1405
(gdb) break register_size if regnum == 0x51
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 172
(gdb) break regcache_descr
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 144
(gdb) step
~/binutils-gdb/gdb/registry.h : 110
(gdb) step
~/binutils-gdb/gdb/arch-utils.c : 1206
(gdb) p/x ((struct regcache_descr*)arch->registry_fields.get(regcache_descr_handle.m_key))->sizeof_register[0x51]
$1 = 0x8
```

So, the size of EFER in GDB is shown as below.

```
~/hariboslinux # make debug
(gdb) break map_regcache_remote_table
(gdb) run < debuggee_input.txt
The first arrival to ~/binutils-gdb/gdb/remote.c : 1405 map_regcache_remote_table
(gdb) continue
The second arrival to the same breakpoint
(gdb) break 1441 if regnum == 0x18
(gdb) continue
(gdb) p/x ((struct regcache_descr*)gdbarch->registry_fields.get(regcache_descr_handle.m_key))->sizeof_register[0x51]
$1 = 0x8
(gdb) backtrace
#0  map_regcache_remote_table (gdbarch=0x5562a517b540, regs=0x5562a51818d0) at remote.c:1441
#1  0x00005562a2f2179d in remote_arch_state::remote_arch_state (this=0x5562a509c440, gdbarch=0x5562a517b540)
    at remote.c:1478
#2  0x00005562a2f5140f in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, 0ul, gdbarch*&, 0ul> (
    this=0x5562a509c438, __tuple1=..., __tuple2=...) at /usr/include/c++/11/tuple:1824
#3  0x00005562a2f510dd in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, gdbarch*&> (
    this=0x5562a509c438, __first=..., __second=...) at /usr/include/c++/11/tuple:1813
#4  0x00005562a2f50af6 in __gnu_cxx::new_allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x5562a5127d10, __p=0x5562a509c438)
    at /usr/include/c++/11/ext/new_allocator.h:162
#5  0x00005562a2f50009 in std::allocator_traits<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (__a=..., __p=0x5562a509c438)
    at /usr/include/c++/11/bits/alloc_traits.h:516
#6  0x00005562a2f4f08b in std::__detail::_Hashtable_alloc<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::_M_allocate_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x5562a5127d10) at /usr/include/c++/11/bits/hashtable_policy.h:1878
#7  0x00005562a2f4dd9b in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_Scoped_node::_Scoped_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x7fff97a5e490, __h=0x5562a5127d10) at /usr/include/c++/11/bits/hashtable.h:304
#8  0x00005562a2f4c0df in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_M_emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x5562a5127d10) at /usr/include/c++/11/bits/hashtable.h:1966
#9  0x00005562a2f49b66 in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x5562a5127d10) at /usr/include/c++/11/bits/hashtable.h:915
#10 0x00005562a2f47a84 in std::unordered_map<gdbarch*, remote_arch_state, std::hash<gdbarch*>, std::equal_to<gdbarch*>, std::allocator<std::pair<gdbarch* const, remote_arch_state> > >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x5562a5127d10) at /usr/include/c++/11/bits/unordered_map.h:389
#11 0x00005562a2f210c6 in remote_state::get_remote_arch_state (this=0x5562a5127ad8, gdbarch=0x5562a517b540)
    at remote.c:1327
#12 0x00005562a2f21177 in remote_target::get_remote_state (this=0x5562a5127ab0) at remote.c:1353
#13 0x00005562a2f327ab in remote_target::set_remote_traceframe (this=0x5562a5127ab0) at remote.c:8589
#14 0x00005562a2f38db2 in remote_target::xfer_partial (this=0x5562a5127ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
    readbuf=0x5562a51808c0 "\200\323\025\327&\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7fff97a5e810)
    at remote.c:11235
#15 0x00005562a300e891 in target_xfer_partial (ops=0x5562a5127ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
    readbuf=0x5562a51808c0 "\200\323\025\327&\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7fff97a5e810)
    at target.c:1730
#16 0x00005562a300f09b in target_read_partial (ops=0x5562a5127ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
    buf=0x5562a51808c0 "\200\323\025\327&\177", offset=0, len=4096, xfered_len=0x7fff97a5e810) at target.c:1964
#17 0x00005562a3025ee6 in target_read_alloc_1<unsigned char> (ops=0x5562a5127ab0, object=TARGET_OBJECT_AUXV,
    annex=0x0) at target.c:2299
#18 0x00005562a300faa4 in target_read_alloc (ops=0x5562a5127ab0, object=TARGET_OBJECT_AUXV, annex=0x0)
    at target.c:2328
#19 0x00005562a2a9de3c in get_auxv_inferior_data (ops=0x5562a5127ab0) at auxv.c:367
#20 0x00005562a2a9dea8 in target_auxv_search (ops=0x5562a5127ab0, match=0, valp=0x7fff97a5e978) at auxv.c:381
#21 0x00005562a2e0c0e0 in linux_is_uclinux () at linux-tdep.c:422
#22 0x00005562a2e0c137 in linux_has_shared_address_space (gdbarch=0x5562a517b540) at linux-tdep.c:429
#23 0x00005562a2a857b5 in gdbarch_has_shared_address_space (gdbarch=0x5562a517b540)
    at /root/binutils-gdb/gdb/gdbarch.c:4843
#24 0x00005562a2edfbf6 in update_address_spaces () at progspace.c:372
#25 0x00005562a2f283d4 in remote_target::start_remote_1 (this=0x5562a5127ab0, from_tty=1, extended_p=0)
    at remote.c:4837
#26 0x00005562a2f28ebc in remote_target::start_remote (this=0x5562a5127ab0, from_tty=1, extended_p=0) at remote.c:5070
#27 0x00005562a2f2a6d1 in remote_target::open_1 (name=0x5562a5094f4e "localhost:2159", from_tty=1, extended_p=0)
    at remote.c:5873
#28 0x00005562a2f28f57 in remote_target::open (name=0x5562a5094f4e "localhost:2159", from_tty=1) at remote.c:5092
#29 0x00005562a300ca5a in open_target (args=0x5562a5094f4e "localhost:2159", from_tty=1, command=0x5562a5106a10)
    at target.c:853
#30 0x00005562a2b64a07 in cmd_func (cmd=0x5562a5106a10, args=0x5562a5094f4e "localhost:2159", from_tty=1)
    at cli/cli-decode.c:2543
#31 0x00005562a303b1d1 in execute_command (p=0x5562a5094f5b "9", from_tty=1) at top.c:699
#32 0x00005562a2d0791c in command_handler (command=0x5562a5094f40 "") at event-top.c:598
#33 0x00005562a2d07e5a in command_line_handler (rl=...) at event-top.c:842
#34 0x00005562a2d0800e in gdb_readline_no_editing_callback (client_data=0x5562a509a560) at event-top.c:907
#35 0x00005562a2d0771e in stdin_event_handler (error=0, client_data=0x5562a509a560) at event-top.c:525
#36 0x00005562a32819f3 in handle_file_event (file_ptr=0x5562a5170dc0, ready_mask=1) at event-loop.cc:574
#37 0x00005562a3281fe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#38 0x00005562a3280c8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#39 0x00005562a2e32635 in start_event_loop () at main.c:411
#40 0x00005562a2e32782 in captured_command_loop () at main.c:471
#41 0x00005562a2e34240 in captured_main (data=0x7fff97a5f2f0) at main.c:1329
#42 0x00005562a2e342b2 in gdb_main (args=0x7fff97a5f2f0) at main.c:1344
#43 0x00005562a29e8de6 in main (argc=1, argv=0x7fff97a5f428) at gdb.c:32
```

* The above `(struct regcache_descr*)gdbarch->registry_fields.get(regcache_descr_handle.m_key)` is `descr` at `~/binutils-gdb/gdb/regcache.c` line 138.

Actually,

```
~/hariboslinux # make debug
(gdb) break regcache.c : 138
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) p/x descr->sizeof_register[0x51]
$1 = 0x8
```

* The size of EFER register is stored at `~/binutils-gdb/gdb/regcache.c` line 121.

```
descr->sizeof_register[i] = TYPE_LENGTH (descr->register_type[i]);
```

Actually,

```
~/hariboslinux # make debug
(gdb) break regcache_descr::regcache_descr
(gdb) run < debuggee_input.txt
regcache.c : 44
(gdb) continue
regcache.c : 44
(gdb) finish
regcahce.c : 87
(gdb) next
regcahce.c : 88
(gdb) watch descr->sizeof_register
(gdb) continue
regcache.c : 118
(gdb) delete 2
(gdb) watch descr->sizeof_register[0x51]
(gdb) continue
regcache.c : 122
(gdb) p/x descr->sizeof_register[0x51]
$1 = 0x8
```

* The macro function `TYPE_LENGTH` returning size of the registers is defined at `~/binutils-gdb/gdb/gdbtypes.h` line 2099.

```
#define TYPE_LENGTH(thistype) (thistype)->length
```

So,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
regcache.c : 84
(gdb) continue
regcache.c : 84
(gdb) break 121 if i == 0x51
(gdb) continue
regcache.c : 121
(gdb) p/x descr->register_type[i]->length
$1 = 0x8
```

* The above `descr->register_type[i]` is stored at `~/binutils-gdb/gdb/regcache.c` line 100.

```
descr->register_type[i] = gdbarch_register_type (gdbarch, i);
```

Actually,

```
~/hariboslinux # make debug
(gdb) break regcache.c : 88
(gdb) run < debuggee_input.txt
~/binutils-gdb/gdb/regcache.c : 88
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 88
(gdb) watch descr->register_type
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 99
(gdb) delete 2
(gdb) watch descr->register_type[0x51]
(gdb) continue
~/binutils-gdb/gdb/regcache.c : 99
(gdb) p/x descr->register_type[0x51]->length
$1 = 0x8
```

* The above `descr->register_type[0x51]` is `arch_reg->type` at `~/binutils-gdb/gdb/target-descriptions.c` line 979.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break 100 if i == 0x51
(gdb) continue
(gdb) step
~/binutils-gdb/gdb/gdbarch.c : 2247 gdbarch_register_type
(gdb) break 2251
(gdb) continue
(gdb) step
~/binutils-gdb/gdb/target-descriptions.c : 911 tdesc_register_type
(gdb) break 979
(gdb) continue
(gdb) p/x arch_reg->type->length
$1 = 0x8
```

* The above `arch_reg->type` is created at `~/binutils-gdb/gdb/target-descriptions.c` line 932.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break 100 if i == 0x51
(gdb) continue
(gdb) break tdesc_register_type
(gdb) continue
(gdb) watch arch_reg
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 912
(gdb) delete 4
delete "watch arch_reg"
(gdb) watch arch_reg->type
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 932
(gdb) p/x arch_reg->type->length
$1 = 0x8
```

* The above `arch_reg->type` is `gdb_type->m_type` at `~/binutils-gdb/gdb/target-descriptions.c` line 314.
* The function `make_gdb_type` at `~/binutils-gdb/gdb/target-descriptions.c` line 56 is recursive.

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break make_gdb_type
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 56 make_gdb_type
(gdb) delete 1
(gdb) delete 2
(gdb) delete 3
(gdb) p/x $rbp
$1 = 0x7ffee9755030
(gdb) break 314 if $rbp == 0x7ffee9755030
(gdb) continue
(gdb) p/x gdb_type.m_type->length
$2 = 0x8
```

* The size of EFER is 4 bytes at `~/binutils-gdb/gdb/target-descriptions.c` line 313.
* The size of EFER is 8 bytes at `~/binutils-gdb/gdb/target-descriptions.c` line 314.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break make_gdb_type
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 56 make_gdb_type
(gdb) delete 1
(gdb) delete 2
(gdb) delete 3
(gdb) next
~/binutils-gdb/gdb/target-descriptions.c : 312
(gdb) next
~/binutils-gdb/gdb/target-descriptions.c : 313
(gdb) p/x gdb_type.m_type->length
$1 = 0x4
(gdb) next
~/binutils-gdb/gdb/target-descriptions.c : 314
(gdb) p/x gdb_type.m_type->length
$2 = 0x8
```

# Whe The size of EFER is 4 bytes at `~/binutils-gdb/gdb/target-descriptions.c` line 313?

# Why The size of EFER is 8 bytes at `~/binutils-gdb/gdb/target-descriptions.c` line 314?

* `m_type->length` gets 8 at `~/binutils-gdb/gdb/target-descriptions.c` line 273.

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break make_gdb_type
(gdb) continue
(gdb) delete 1
(gdb) delete 2
(gdb) delete 3
(gdb) p/x $rbp
$1 = 0x7ffe508c7cf0
(gdb) break 313 if $rbp == 0x7ffe508c7cf0
(gdb) continue
(gdb) delete 4
(gdb) watch gdb_type.m_type
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 171
gdb_type.m_type == NULL
(gdb) continue
~/binutils-gdb/gdb/target-descriptions.c : 276
(gdb) p/x m_type->length
$2 = 0x8
```

* The above `m_type` is created at `~/binutils-gdb/gdb/gdbtypes.c` line 5939.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break make_gdb_type
(gdb) continue
(gdb) delete 1
(gdb) delete 2
(gdb) delete 3
(gdb) p/x $rbp
$1 = 0x7ffe508c7cf0
(gdb) break 313 if $rbp == 0x7ffe508c7cf0
(gdb) continue
(gdb) break arch_flags_type
(gdb) continue
~/binutils-gdb/gdb/gdbtype.c : 5939
(gdb) next
~/binutils-gdb/gdb/gdbtype.c : 5940
(gdb) p/x type->length
$2 = 0x8
```

* The above `type->length` is calculated from the third argument `bit` of the function `arch_flags_type`.
* The size of the register is `e->size` in `make_gdb_type_flags` at `~/binutils-gdb/gdb/target-descriptions.c` line 271.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break make_gdb_type
(gdb) continue
(gdb) delete 1
(gdb) delete 2
(gdb) delete 3
(gdb) p/x $rbp
$1 = 0x7ffe508c7cf0
(gdb) break 313 if $rbp == 0x7ffe508c7cf0
(gdb) continue
(gdb) break make_gdb_type_flags
(gdb) continue
(gdb) p/x e->size
$1 = 0x8
(gdb) backtrace
#0  gdb_type_creator::make_gdb_type_flags (this=0x7ffc3cc03d70, e=0x55fcb1167150) at target-descriptions.c:271
#1  0x000055fcafbf73cf in gdb_type_creator::visit (this=0x7ffc3cc03d70, e=0x55fcb1167150) at target-descriptions.c:183
#2  0x000055fcafe92518 in tdesc_type_with_fields::accept (this=0x55fcb1167150, v=...) at ../gdbsupport/tdesc.h:274
#3  0x000055fcafbf7c04 in make_gdb_type (gdbarch=0x55fcb115a560, ttype=0x55fcb1167150) at target-descriptions.c:313
#4  0x000055fcafbf8cff in tdesc_register_type (gdbarch=0x55fcb115a560, regno=81) at target-descriptions.c:932
#5  0x000055fcaf67ffe6 in gdbarch_register_type (gdbarch=0x55fcb115a560, reg_nr=81)
    at /root/binutils-gdb/gdb/gdbarch.c:2251
#6  0x000055fcafb0ee50 in init_regcache_descr (gdbarch=0x55fcb115a560) at regcache.c:100
#7  0x000055fcafb0f025 in regcache_descr (gdbarch=0x55fcb115a560) at regcache.c:147
#8  0x000055fcafb0f0f2 in register_size (gdbarch=0x55fcb115a560, regnum=0) at regcache.c:172
#9  0x000055fcafb25322 in map_regcache_remote_table (gdbarch=0x55fcb115a560, regs=0x55fcb1169e60) at remote.c:1413
#10 0x000055fcafb2579d in remote_arch_state::remote_arch_state (this=0x55fcb107b440, gdbarch=0x55fcb115a560)
        at remote.c:1478
#11 0x000055fcafb5540f in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, 0ul, gdbarch*&, 0ul> (
		    this=0x55fcb107b438, __tuple1=..., __tuple2=...) at /usr/include/c++/11/tuple:1824
#12 0x000055fcafb550dd in std::pair<gdbarch* const, remote_arch_state>::pair<gdbarch*&, gdbarch*&> (
		    this=0x55fcb107b438, __first=..., __second=...) at /usr/include/c++/11/tuple:1813
#13 0x000055fcafb54af6 in __gnu_cxx::new_allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x55fcb1106d10, __p=0x55fcb107b438)
    at /usr/include/c++/11/ext/new_allocator.h:162
#14 0x000055fcafb54009 in std::allocator_traits<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::construct<std::pair<gdbarch* const, remote_arch_state>, std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (__a=..., __p=0x55fcb107b438)
        at /usr/include/c++/11/bits/alloc_traits.h:516
#15 0x000055fcafb5308b in std::__detail::_Hashtable_alloc<std::allocator<std::__detail::_Hash_node<std::pair<gdbarch* const, remote_arch_state>, false> > >::_M_allocate_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x55fcb1106d10) at /usr/include/c++/11/bits/hashtable_policy.h:1878
#16 0x000055fcafb51d9b in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_Scoped_node::_Scoped_node<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x7ffc3cc04190, __h=0x55fcb1106d10) at /usr/include/c++/11/bits/hashtable.h:304
#17 0x000055fcafb500df in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::_M_emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x55fcb1106d10) at /usr/include/c++/11/bits/hashtable.h:1966
#18 0x000055fcafb4db66 in std::_Hashtable<gdbarch*, std::pair<gdbarch* const, remote_arch_state>, std::allocator<std::pair<gdbarch* const, remote_arch_state> >, std::__detail::_Select1st, std::equal_to<gdbarch*>, std::hash<gdbarch*>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x55fcb1106d10) at /usr/include/c++/11/bits/hashtable.h:915
#19 0x000055fcafb4ba84 in std::unordered_map<gdbarch*, remote_arch_state, std::hash<gdbarch*>, std::equal_to<gdbarch*>,std::allocator<std::pair<gdbarch* const, remote_arch_state> > >::emplace<std::piecewise_construct_t const&, std::tuple<gdbarch*&>, std::tuple<gdbarch*&> > (this=0x55fcb1106d10) at /usr/include/c++/11/bits/unordered_map.h:389
#20 0x000055fcafb250c6 in remote_state::get_remote_arch_state (this=0x55fcb1106ad8, gdbarch=0x55fcb115a560)
	    at remote.c:1327
#21 0x000055fcafb25177 in remote_target::get_remote_state (this=0x55fcb1106ab0) at remote.c:1353
#22 0x000055fcafb367ab in remote_target::set_remote_traceframe (this=0x55fcb1106ab0) at remote.c:8589
#23 0x000055fcafb3cdb2 in remote_target::xfer_partial (this=0x55fcb1106ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
		    readbuf=0x55fcb1168e50 "\200C\006\340\327\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7ffc3cc04510)
    at remote.c:11235
#24 0x000055fcafc12891 in target_xfer_partial (ops=0x55fcb1106ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
		    readbuf=0x55fcb1168e50 "\200C\006\340\327\177", writebuf=0x0, offset=0, len=4096, xfered_len=0x7ffc3cc04510)
    at target.c:1730
#25 0x000055fcafc1309b in target_read_partial (ops=0x55fcb1106ab0, object=TARGET_OBJECT_AUXV, annex=0x0,
		    buf=0x55fcb1168e50 "\200C\006\340\327\177", offset=0, len=4096, xfered_len=0x7ffc3cc04510) at target.c:1964
#26 0x000055fcafc29ee6 in target_read_alloc_1<unsigned char> (ops=0x55fcb1106ab0, object=TARGET_OBJECT_AUXV,
		    annex=0x0) at target.c:2299
#27 0x000055fcafc13aa4 in target_read_alloc (ops=0x55fcb1106ab0, object=TARGET_OBJECT_AUXV, annex=0x0)
    at target.c:2328
#28 0x000055fcaf6a1e3c in get_auxv_inferior_data (ops=0x55fcb1106ab0) at auxv.c:367
#29 0x000055fcaf6a1ea8 in target_auxv_search (ops=0x55fcb1106ab0, match=0, valp=0x7ffc3cc04678) at auxv.c:381
#30 0x000055fcafa100e0 in linux_is_uclinux () at linux-tdep.c:422
#31 0x000055fcafa10137 in linux_has_shared_address_space (gdbarch=0x55fcb115a560) at linux-tdep.c:429
#32 0x000055fcaf6897b5 in gdbarch_has_shared_address_space (gdbarch=0x55fcb115a560)
        at /root/binutils-gdb/gdb/gdbarch.c:4843
#33 0x000055fcafae3bf6 in update_address_spaces () at progspace.c:372
#34 0x000055fcafb2c3d4 in remote_target::start_remote_1 (this=0x55fcb1106ab0, from_tty=1, extended_p=0)
	    at remote.c:4837
#35 0x000055fcafb2cebc in remote_target::start_remote (this=0x55fcb1106ab0, from_tty=1, extended_p=0) at remote.c:5070
#36 0x000055fcafb2e6d1 in remote_target::open_1 (name=0x55fcb1073f4e "localhost:2159", from_tty=1, extended_p=0)
	        at remote.c:5873
#37 0x000055fcafb2cf57 in remote_target::open (name=0x55fcb1073f4e "localhost:2159", from_tty=1) at remote.c:5092
#38 0x000055fcafc10a5a in open_target (args=0x55fcb1073f4e "localhost:2159", from_tty=1, command=0x55fcb10e5a10)
		    at target.c:853
#39 0x000055fcaf768a07 in cmd_func (cmd=0x55fcb10e5a10, args=0x55fcb1073f4e "localhost:2159", from_tty=1)
		        at cli/cli-decode.c:2543
#40 0x000055fcafc3f1d1 in execute_command (p=0x55fcb1073f5b "9", from_tty=1) at top.c:699
#41 0x000055fcaf90b91c in command_handler (command=0x55fcb1073f40 "") at event-top.c:598
#42 0x000055fcaf90be5a in command_line_handler (rl=...) at event-top.c:842
#43 0x000055fcaf90c00e in gdb_readline_no_editing_callback (client_data=0x55fcb1079560) at event-top.c:907
#44 0x000055fcaf90b71e in stdin_event_handler (error=0, client_data=0x55fcb1079560) at event-top.c:525
#45 0x000055fcafe859f3 in handle_file_event (file_ptr=0x55fcb114fde0, ready_mask=1) at event-loop.cc:574
#46 0x000055fcafe85fe3 in gdb_wait_for_event (block=0) at event-loop.cc:695
#47 0x000055fcafe84c8b in gdb_do_one_event (mstimeout=-1) at event-loop.cc:217
#48 0x000055fcafa36635 in start_event_loop () at main.c:411
#49 0x000055fcafa36782 in captured_command_loop () at main.c:471
#50 0x000055fcafa38240 in captured_main (data=0x7ffc3cc04ff0) at main.c:1329
#51 0x000055fcafa382b2 in gdb_main (args=0x7ffc3cc04ff0) at main.c:1344
#52 0x000055fcaf5ecde6 in main (argc=1, argv=0x7ffc3cc05128) at gdb.c:32
```

* The above `e` is `(tdesc_type_with_fields*)reg->tdesc_type` at `~/binutils-gdb/gdb/target-descriptions.c` line 932.

Actually,

```
~/hariboslinux # make debug
(gdb) break init_regcache_descr
(gdb) run < debuggee_input.txt
(gdb) continue
(gdb) break tdesc_register_type if regno == 0x51
(gdb) continue
(gdb) break target-descriptions.c : 932
(gdb) continue
(gdb) p/x ((tdesc_type_with_fields*)reg->tdesc_type)->size
$1 = 0x8
```

