# tcp port
target remote localhost:2159

# real mode
set tdesc filename target.xml

# go to entry point of loaddisk.bin
break *0xbe00
continue

# go to exit point of mv2prtmd.bin
# break *0xcb01
# continue

# far jump from mv2prtmd.bin to entry32.bin
# stepi

# reach the entry point of entry32.bin

