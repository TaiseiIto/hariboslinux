# Debug QEMU

## Wait for GDB at `main_loop_wait` function

```
~/hariboslinux/debug_qemu # make debug-qemu
@ ~/qemu/softmmu/main.c : 46 main
(gdb) break qemu_default_main
(gdb) continue
@ ~/qemu/softmmu/main.c : 34 qemu_default_main
(gdb) break qemu_main_loop
(gdb) continue
@ ~/qemu/softmmu/runstate.c : 729 qemu_main_loop
(gdb) break main_loop_wait
(gdb) continue
@ ~/qemu/util/mail-loop.c : 578 main_loop_wait
(gdb) break os_host_main_loop_wait
(gdb) continue
@ ~/qemu/util/main-loop.c : 305 os_host_main_loop_wait
(gdb) break glib_pollfds_poll
(gdb) continue
```
