# tcp port
target remote localhost:2159

# real mode
# set tdesc filename target.xml

# go to entry point of kernel.bin
break *0xd000
continue
delete 1

