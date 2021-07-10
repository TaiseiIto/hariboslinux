# tcp port
target remote localhost:2159

# real mode
set tdesc filename target.xml

# goto initscrn.bin
break *0xc400
continue

