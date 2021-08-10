#include "io.h"
#include "keyboard.h"
#include "mouse.h"

void init_mouse(void)
{
	wait_for_keyboard();
	outb(PORT_KEYBOARD_COMMAND, KEYBOARD_COMMAND_SEND_TO_MOUSE);
	wait_for_keyboard();
	outb(PORT_KEYBOARD_DATA, MOUSE_INIT);
}

