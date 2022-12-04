# Symptom

## Correct output

```
~/hariboslinux/test_os # make debug
(gdb) break i387_print_float_info
(gdb) continue
(gdb) finish
  R7: Valid   0x3fff8000000000000000 +1
  R6: Valid   0x4000d49a784bcd1b8afe +3.321928094887362348
  R5: Valid   0x3fffb8aa3b295c17f0bc +1.442695040888963407
  R4: Valid   0x4000c90fdaa22168c235 +3.141592653589793239
  R3: Valid   0x3ffd9a209a84fbcff799 +0.3010299956639811952
  R2: Valid   0x3ffeb17217f7d1cf79ac +0.6931471805599453094
  R1: Valid   0x00000000000000000000 +0
=>R0: Valid   0x00000000000000000000 +0

Status Word:         0x0800
                       TOP: 1
Control Word:        0x037f   IM DM ZM OM UM PM
                       PC: Extended Precision (64-bits)
                       RC: Round to nearest
Tag Word:            0x0000
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

## Actual output

```
~/hariboslinux/test_os # make debug
(gdb) break i387_print_float_info
(gdb) continue
(gdb) finish
  R7: Valid   0x4000d49a784bcd1b8afe +3.321928094887362348
  R6: Valid   0x3fffb8aa3b295c17f0bc +1.442695040888963407
  R5: Valid   0x4000c90fdaa22168c235 +3.141592653589793239
  R4: Valid   0x3ffd9a209a84fbcff799 +0.3010299956639811952
  R3: Valid   0x3ffeb17217f7d1cf79ac +0.6931471805599453094
  R2: Valid   0x00000000000000000000 +0
=>R1: Valid   0x00000000000000000000 +0
  R0: Valid   0x3fff8000000000000000 +1

Status Word:         0x0800
                       TOP: 1
Control Word:        0x037f   IM DM ZM OM UM PM
                       PC: Extended Precision (64-bits)
                       RC: Round to nearest
Tag Word:            0x0000
Instruction Pointer: 0x00:0x00000000
Operand Pointer:     0x00:0x00000000
Opcode:              0x0000
```

