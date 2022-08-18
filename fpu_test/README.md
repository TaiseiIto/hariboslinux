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
(gdb) p/x descr->register_offset[0x18]
$1 = 0x90
```

