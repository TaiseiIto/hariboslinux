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

QEMU read all registers at function `handle_read_all_regs` in `~/qemu/gdbstub/gdbstub.c` line 1744.

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) backtrace
#0  handle_read_all_regs (params=0x55c8f3f23130, user_ctx=0x0) at ../gdbstub/gdbstub.c:1737
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
#9  os_host_main_loop_wait (timeout=90361000) at ../util/main-loop.c:320
#10 main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:606
#11 0x000055c8f08634a3 in qemu_main_loop () at ../softmmu/runstate.c:739
#12 0x000055c8f06af0bb in qemu_default_main () at ../softmmu/main.c:37
#13 0x00007f7de0fafd90 in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#14 0x00007f7de0fafe40 in __libc_start_main () from /lib/x86_64-linux-gnu/libc.so.6
#15 0x000055c8f06aefe5 in _start ()
(gdb) continue
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

