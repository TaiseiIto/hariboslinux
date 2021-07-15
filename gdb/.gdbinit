# tcp port
target remote localhost:2159

# real mode
# set tdesc filename target.xml

# go to exit point of mv2prtmd.bin
break *0xcb01
continue

# far jump from mv2prtmd.bin to kernel.bin
stepi

# reach the entry point of kernel.bin

