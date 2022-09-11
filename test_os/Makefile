TEST_OS_NAME = test_os
TEST_OS_ASM = $(TEST_OS_NAME).s
TEST_OS_IMG = $(TEST_OS_NAME).img
TEST_OS_LNK = $(TEST_OS_NAME).ld
TEST_OS_MAP = $(TEST_OS_NAME).map
TEST_OS_OBJ = $(TEST_OS_NAME).o

all: $(TEST_OS_IMG)

test: $(TEST_OS_IMG) stop-qemu
	(qemu-system-i386 -boot order=a -drive file=$<,format=raw,if=floppy -S -gdb tcp::2159 &) && \
	gdb && \
	make stop-qemu

stop-qemu:
	for i in $$(ps ax | grep qemu | grep -v grep | awk '{print $$1}'); do kill $$i; done

$(TEST_OS_IMG): $(TEST_OS_OBJ) $(TEST_OS_LNK)
	ld $< -Map $(TEST_OS_MAP) -o $@ -T $(word 2, $^)

$(TEST_OS_OBJ): $(TEST_OS_ASM)
	gcc $^ -c -nostdlib -o $@ -Wall -Wextra

