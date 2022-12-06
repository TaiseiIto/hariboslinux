# Debug QEMU

```
~/hariboslinux/debug_qemu # make debug-qemu
(gdb) continue
Ctrl c
(gdb) backtrace
#0  0x00007f39b05d3e7e in ppoll () from /lib/x86_64-linux-gnu/libc.so.6
#1  0x000055772cf14355 in ppoll (__ss=0x0, __timeout=0x7fff4a8e2b00, __nfds=<optimized out>, __fds=<optimized out>)
    at /usr/include/x86_64-linux-gnu/bits/poll2.h:64
#2  qemu_poll_ns (fds=<optimized out>, nfds=<optimized out>, timeout=<optimized out>) at ../util/qemu-timer.c:351
#3  0x000055772cf11aa6 in os_host_main_loop_wait (timeout=2007414300) at ../util/main-loop.c:315
#4  main_loop_wait (nonblocking=nonblocking@entry=0) at ../util/main-loop.c:606
#5  0x000055772cb8c4a3 in qemu_main_loop () at ../softmmu/runstate.c:739
#6  0x000055772c9d80bb in qemu_default_main () at ../softmmu/main.c:37
#7  0x00007f39b04e4d90 in ?? () from /lib/x86_64-linux-gnu/libc.so.6
#8  0x00007f39b04e4e40 in __libc_start_main () from /lib/x86_64-linux-gnu/libc.so.6
#9  0x000055772c9d7fe5 in _start ()
```

