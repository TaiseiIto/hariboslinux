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

