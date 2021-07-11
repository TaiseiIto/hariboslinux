# tcp port
target remote localhost:2159

# real mode
# set tdesc filename target.xml

# goto mv2prtmd.bin label main32
break *0xcad7
continue

