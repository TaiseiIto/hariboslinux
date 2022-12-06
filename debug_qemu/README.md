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

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) backtrace
#0  gdb_read_register (cpu=0x55df000f5b70, buf=0x55df00f6f380, reg=0) at ../gdbstub/gdbstub.c:942
#1  0x000055defeab8df1 in handle_read_all_regs (params=<optimized out>, user_ctx=<optimized out>)
    at ../gdbstub/gdbstub.c:1744
#2  0x000055defeab6dce in process_string_cmd (data=data@entry=0x55deff4b7a84 <gdbserver_state+36> "g",
    cmds=cmds@entry=0x55deff0fae60 <read_all_regs_cmd_desc>, num_cmds=num_cmds@entry=1, user_ctx=0x0)
    at ../gdbstub/gdbstub.c:1394
#3  0x000055defeabb100 in run_cmd_parser (data=0x55deff4b7a84 <gdbserver_state+36> "g",
    cmd=0x55deff0fae60 <read_all_regs_cmd_desc>) at ../gdbstub/gdbstub.c:1412
#4  gdb_handle_packet (line_buf=0x55deff4b7a84 <gdbserver_state+36> "g") at ../gdbstub/gdbstub.c:2677
#5  gdb_read_byte (ch=55 '7') at ../gdbstub/gdbstub.c:3013
#6  gdb_chr_receive (opaque=<optimized out>, buf=<optimized out>, size=<optimized out>) at ../gdbstub/gdbstub.c:3314
#7  0x000055defec27b27 in tcp_chr_read (chan=<optimized out>, cond=<optimized out>, opaque=<optimized out>)
    at ../chardev/char-socket.c:508
#8  0x00007f02f7381c44 in g_main_context_dispatch () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
#9  0x000055defecd4b50 in glib_pollfds_poll () at ../util/main-loop.c:297
#10 os_host_main_loop_wait (timeout=113923000) at ../util/main-loop.c:320
#11 main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:606
#12 0x000055defe94f4a3 in qemu_main_loop () at ../softmmu/runstate.c:739
#13 0x000055defe79b0bb in qemu_default_main () at ../softmmu/main.c:37
#14 0x00007f02f701dd90 in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#15 0x00007f02f701de40 in __libc_start_main () from /lib/x86_64-linux-gnu/libc.so.6
#16 0x000055defe79afe5 in _start ()
```

