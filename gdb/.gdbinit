# tcp port
target remote localhost:2159

# real mode
# set tdesc filename target.xml

# far jump from mv2prtmd.bin to entry32.bin
break *0xcb01
continue
stepi

