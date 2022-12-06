# Debug QEMU

## Where does QEMU specify `i386-32bit.xml`?

QEMU specify the XML at `~/qemu/target/i386/cpu.c` line 7145.

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) break target/i386/cpu.c : 7145
(gdb) continue
(gdb) backtrace
#0  x86_cpu_common_class_init (oc=0x5581fc141440, data=<optimized out>) at ../target/i386/cpu.c:7145
#1  0x00005581fae1324c in type_initialize (ti=0x5581fc108a90) at ../qom/object.c:299
#2  type_initialize (ti=ti@entry=0x5581fc0f4750) at ../qom/object.c:314
#3  0x00005581fae13ce0 in type_initialize (ti=0x5581fc0f4750) at ../qom/object.c:1094
#4  object_class_foreach_tramp (key=<optimized out>, value=0x5581fc0f4750, opaque=0x7ffc4fbc5490)
    at ../qom/object.c:1081
#5  0x00007fd7bac506b8 in g_hash_table_foreach () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
#6  0x00005581fae14205 in object_class_foreach (opaque=0x7ffc4fbc5488, include_abstract=false,
    implements_type=<optimized out>, fn=0x5581fae12250 <object_class_get_list_tramp>) at ../qom/object.c:87
#7  object_class_get_list (implements_type=implements_type@entry=0x5581fb13b30f "machine",
    include_abstract=include_abstract@entry=false) at ../qom/object.c:1160
#8  0x00005581fac00e17 in select_machine (errp=<optimized out>, qdict=0x5581fc1349f0) at ../softmmu/vl.c:1580
#9  qemu_create_machine (qdict=0x5581fc1349f0) at ../softmmu/vl.c:2015
#10 qemu_init (argc=<optimized out>, argv=0x7ffc4fbc5758) at ../softmmu/vl.c:3533
#11 0x00005581faa4475d in main (argc=<optimized out>, argv=<optimized out>) at ../softmmu/main.c:47
(gdb) next
(gdb) print cc->gdb_core_xml_file
$1 = 0x5581fb0a238c "i386-32bit.xml"
```

## Where does QEMU send "g" packet to GDB?

QEMU reads all registers at function `handle_read_all_regs` in `~/qemu/gdbstub/gdbstub.c` line 1744.

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) backtrace
#0  handle_read_all_regs (params=0x55c8f2e506a0, user_ctx=0x0) at ../gdbstub/gdbstub.c:1737
#1  0x000055c8f09cadce in process_string_cmd (data=data@entry=0x55c8f13cba84 <gdbserver_state+36> "g",
    cmds=cmds@entry=0x55c8f100ee60 <read_all_regs_cmd_desc>, num_cmds=num_cmds@entry=1, user_ctx=0x0)
    at ../gdbstub/gdbstub.c:1394
#2  0x000055c8f09cf100 in run_cmd_parser (data=0x55c8f13cba84 <gdbserver_state+36> "g",
    cmd=0x55c8f100ee60 <read_all_regs_cmd_desc>) at ../gdbstub/gdbstub.c:1412
#3  gdb_handle_packet (line_buf=0x55c8f13cba84 <gdbserver_state+36> "g") at ../gdbstub/gdbstub.c:2677
#4  gdb_read_byte (ch=55 '7') at ../gdbstub/gdbstub.c:3013
#5  gdb_chr_receive (opaque=<optimized out>, buf=<optimized out>, size=<optimized out>) at ../gdbstub/gdbstub.c:3314
#6  0x000055c8f0b3bb27 in tcp_chr_read (chan=<optimized out>, cond=<optimized out>, opaque=<optimized out>)
    at ../chardev/char-socket.c:508
#7  0x00007f7de1313c44 in g_main_context_dispatch () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
#8  0x000055c8f0be8b50 in glib_pollfds_poll () at ../util/main-loop.c:297
#9  os_host_main_loop_wait (timeout=904017000) at ../util/main-loop.c:320
#10 main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:606
#11 0x000055c8f08634a3 in qemu_main_loop () at ../softmmu/runstate.c:739
#12 0x000055c8f06af0bb in qemu_default_main () at ../softmmu/main.c:37
#13 0x00007f7de0fafd90 in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#14 0x00007f7de0fafe40 in __libc_start_main () from /lib/x86_64-linux-gnu/libc.so.6
#15 0x000055c8f06aefe5 in _start ()
```

And makes "g" packet string.

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) break 1751
(gdb) continue
(gdb) print gdbserver_state.str_buf->str
$1 = (gchar *) 0x5642e482c800 "55aa0000000000000000000000000000046f0000000000000000000000000000107c0000020200000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ac79cfd1f71772b1fe3f99f7cffb849a209afd3f35c26821a2da0fc90040bcf0175c293baab8ff3ffe8a1bcd4b789ad400400000000000000080ff3f7f030000000800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000801f0000"
(gdb) x/43gx gdbserver_state.mem_buf->data
0x5574ddeaf200: 0x000000000000aa55      0x0000000000000000
0x5574ddeaf210: 0x0000000000006f04      0x0000000000000000
0x5574ddeaf220: 0x0000020200007c10      0x0000000000000000
0x5574ddeaf230: 0x0000000000000000      0x0000000000000000
0x5574ddeaf240: 0x0000000000000000      0x0000001000000000
0x5574ddeaf250: 0x0000000000000000      0x0000000000000000
0x5574ddeaf260: 0x0000000000000000      0x0000000000000000
0x5574ddeaf270: 0x0000000000000000      0xb17217f7d1cf79ac
0x5574ddeaf280: 0x9a84fbcff7993ffe      0x2168c2353ffd9a20
0x5574ddeaf290: 0xf0bc4000c90fdaa2      0x3fffb8aa3b295c17
0x5574ddeaf2a0: 0xd49a784bcd1b8afe      0x0000000000004000
0x5574ddeaf2b0: 0x0000037f3fff8000      0x0000000000000800
0x5574ddeaf2c0: 0x0000000000000000      0x0000000000000000
0x5574ddeaf2d0: 0x0000000000000000      0x0000000000000000
0x5574ddeaf2e0: 0x0000000000000000      0x0000000000000000
0x5574ddeaf2f0: 0x0000000000000000      0x0000000000000000
0x5574ddeaf300: 0x0000000000000000      0x0000000000000000
0x5574ddeaf310: 0x0000000000000000      0x0000000000000000
0x5574ddeaf320: 0x0000000000000000      0x0000000000000000
0x5574ddeaf330: 0x0000000000000000      0x0000000000000000
0x5574ddeaf340: 0x0000000000000000      0x0000000000000000
0x5574ddeaf350: 0x00001f8000000000
```

## Where does QEMU read `Rx` registers?

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) break x86_cpu_gdb_read_register if n == 0x19
(gdb) continue
~/qemu/target/i386/gdbstub.c : 100
```

`n` is a register number.

| Sent register from QEMU | Received register to GDB | Register number |
| :---------------------- | :----------------------- | --------------: |
| R0                      | ST0                      |            0x19 |
| R1                      | ST1                      |            0x1a |
| R2                      | ST2                      |            0x1b |
| R3                      | ST3                      |            0x1c |
| R4                      | ST4                      |            0x1d |
| R5                      | ST5                      |            0x1e |
| R6                      | ST6                      |            0x1f |
| R7                      | ST7                      |            0x20 |
| FSTAT                   | FSTAT                    |            0x22 |

FPU stack is read at lines from 123 to 138 in `~/qemu/target/i386/gdbstub.c`.

```
    } else if (n >= IDX_FP_REGS && n < IDX_FP_REGS + 8) {
        floatx80 *fp = (floatx80 *) &env->fpregs[n - IDX_FP_REGS];
        int len = gdb_get_reg64(mem_buf, cpu_to_le64(fp->low));
        len += gdb_get_reg16(mem_buf, cpu_to_le16(fp->high));
        return len;
    } else if (n >= IDX_XMM_REGS && n < IDX_XMM_REGS + CPU_NB_REGS) {
```

The above `env` is declared at `~/qemu/target/i386/gdbstub.c` line 102 and has emulated CPU states and its type is `CPUX86State` defined at `~/qemu/target/i386/cpu.h` line 1814.
`CPUX86State.fpregs` defined at `~/qemu/target/i386/cpu.h` line 1579 means x87 FPU stack.
`CPUX86State.fpstt` defined at `~/qemu/target/i386/cpu.h` line 1575 means top of stack index.

