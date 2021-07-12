# tcp port
target remote localhost:2159

# real mode
# set tdesc filename target.xml

# goto entry32.bin
break *0xce00
continue

