set print elements 0
set print repeats 0
break main
run -boot order=a -drive file=test_os.img,format=raw,if=floppy -S -gdb tcp::2159 -vnc localhost:0
