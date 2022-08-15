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

