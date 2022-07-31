debug : fpu_test
	gdb gdb < debugger_input.txt

fpu_test : fpu_test.s
	gcc $^ -g -O0 -o $@

