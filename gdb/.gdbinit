# tcp port
target remote localhost:2159

# real mode
set tdesc filename target.xml

# goto mv2prtmd.bin
break *0xca00
continue

