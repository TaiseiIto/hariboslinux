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

```

