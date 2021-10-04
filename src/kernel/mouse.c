#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"

#define MOUSE_COMMAND_GET_ID			0xf2
#define MOUSE_COMMAND_SET_SAMPLE_RATE		0xf3
#define MOUSE_COMMAND_ENABLE_PACKET_STREAMING	0xf4
#define MOUSE_COMMAND_DISABLE_PACKET_STREAMING	0xf5
#define MOUSE_COMMAND_SET_DEFAULTS		0xf6
#define MOUSE_COMMAND_RESET			0xff

typedef union _MousePacket
{
	unsigned char signals[4];
	unsigned int packet;
	// Mouse signal format
	// This part is independent of mouse_id
	#define MOUSE_PACKET_LEFT_BUTTON_PUSHED		0x00000001
	#define MOUSE_PACKET_RIGHT_BUTTON_PUSHED	0x00000002
	#define MOUSE_PACKET_MIDDLE_BUTTON_PUSHED	0x00000004
	#define MOUSE_PACKET_MUST_BE_1			0x00000008
	#define MOUSE_PACKET_X_SIGN			0x00000010
	#define MOUSE_PACKET_Y_SIGN			0x00000020
	#define MOUSE_PACKET_X_OVERFLOW			0x00000040 // It must be 0
	#define MOUSE_PACKET_Y_OVERFLOW			0x00000080 // It must be 0
	#define MOUSE_PACKET_X_MOVEMENT			0x0000ff00
	#define MOUSE_PACKET_Y_MOVEMENT			0x00ff0000
	// This part is dependent on mouse_id
	// mouse_id == 3
	#define MOUSE_PACKET_ID3_VERTICAL_WHEEL		0xff000000
	// mouse_id == 4
	#define MOUSE_PACKET_ID4_WHEEL			0x0f000000
	#define MOUSE_PACKET_ID4_WHEEL_STOP		0x00000000
	#define MOUSE_PACKET_ID4_WHEEL_UP		0x01000000
	#define MOUSE_PACKET_ID4_WHEEL_DOWN		0x0f000000
	#define MOUSE_PACKET_ID4_WHEEL_RIGHT		0x02000000
	#define MOUSE_PACKET_ID4_WHEEL_LEFT		0x0e000000
	#define MOUSE_PACKET_ID4_4TH_BUTTON_PUSHED	0x10000000
	#define MOUSE_PACKET_ID4_5TH_BUTTON_PUSHED	0x20000000
	#define MOUSE_PACKET_ID4_MUST_BE_0		0xc0000000
} MousePacket;

unsigned char mouse_id;

void send_to_mouse(unsigned char data);
void set_mouse_sample_rate(unsigned char rate);

void decode_mouse_interrupt(unsigned char signal)
{
	Event event;
	static MousePacket mouse_packet;
	static unsigned char signal_index = 0;
	mouse_packet.signals[signal_index++] = signal;
	if(signal_index == 1 && (mouse_packet.packet & (MOUSE_PACKET_MUST_BE_1 | MOUSE_PACKET_X_OVERFLOW | MOUSE_PACKET_Y_OVERFLOW)) != MOUSE_PACKET_MUST_BE_1)signal_index = 0; // mouse_packet.signals[0] is wrong
	switch(mouse_id)
	{
	case 0:
		if(signal_index == 3)
		{
			event.type = EVENT_TYPE_MOUSE_EVENT;
			event.event_union.mouse_event.flags = 0;
			if(mouse_packet.packet & MOUSE_PACKET_LEFT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_LEFT_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_MIDDLE_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_MIDDLE_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_RIGHT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_RIGHT_BUTTON_PUSHED;
			event.event_union.mouse_event.x_movement = mouse_packet.signals[1];
			if(mouse_packet.packet & MOUSE_PACKET_X_SIGN)event.event_union.mouse_event.x_movement |= 0xff00;
			event.event_union.mouse_event.y_movement = mouse_packet.signals[2];
			if(mouse_packet.packet & MOUSE_PACKET_Y_SIGN)event.event_union.mouse_event.y_movement |= 0xff00;
			event.event_union.mouse_event.vertical_wheel_movement = 0;
			event.event_union.mouse_event.horizontal_wheel_movement = 0;
			enqueue_event(&event);
			signal_index = 0;
		}
		break;
	case 3:
		if(signal_index == 4)
		{
			event.type = EVENT_TYPE_MOUSE_EVENT;
			event.event_union.mouse_event.flags = 0;
			if(mouse_packet.packet & MOUSE_PACKET_LEFT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_LEFT_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_MIDDLE_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_MIDDLE_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_RIGHT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_RIGHT_BUTTON_PUSHED;
			event.event_union.mouse_event.x_movement = mouse_packet.signals[1];
			if(mouse_packet.packet & MOUSE_PACKET_X_SIGN)event.event_union.mouse_event.x_movement |= 0xff00;
			event.event_union.mouse_event.y_movement = mouse_packet.signals[2];
			if(mouse_packet.packet & MOUSE_PACKET_Y_SIGN)event.event_union.mouse_event.y_movement |= 0xff00;
			if(0 < (char)mouse_packet.signals[3])event.event_union.mouse_event.vertical_wheel_movement = 1;
			else if((char)mouse_packet.signals[3] < 0)event.event_union.mouse_event.vertical_wheel_movement = -1;
			event.event_union.mouse_event.horizontal_wheel_movement = 0;
			enqueue_event(&event);
			signal_index = 0;
		}
		break;
	case 4:
		if(signal_index == 4 && mouse_packet.packet & MOUSE_PACKET_ID4_MUST_BE_0)signal_index = 0; // mouse_packet.signals[3] is wrong
		if(signal_index == 4)
		{
			printf_serial("mouse packet = %#010x\n", mouse_packet.packet);
			event.type = EVENT_TYPE_MOUSE_EVENT;
			event.event_union.mouse_event.flags = 0;
			if(mouse_packet.packet & MOUSE_PACKET_LEFT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_LEFT_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_MIDDLE_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_MIDDLE_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_RIGHT_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_RIGHT_BUTTON_PUSHED;
			event.event_union.mouse_event.x_movement = mouse_packet.signals[1];
			if(mouse_packet.packet & MOUSE_PACKET_X_SIGN)event.event_union.mouse_event.x_movement |= 0xff00;
			event.event_union.mouse_event.y_movement = mouse_packet.signals[2];
			if(mouse_packet.packet & MOUSE_PACKET_Y_SIGN)event.event_union.mouse_event.y_movement |= 0xff00;
			switch(mouse_packet.packet & MOUSE_PACKET_ID4_WHEEL)
			{
			case MOUSE_PACKET_ID4_WHEEL_STOP:
				event.event_union.mouse_event.vertical_wheel_movement = 0;
				event.event_union.mouse_event.horizontal_wheel_movement = 0;
				break;
			case MOUSE_PACKET_ID4_WHEEL_DOWN:
				event.event_union.mouse_event.vertical_wheel_movement = -1;
				event.event_union.mouse_event.horizontal_wheel_movement = 0;
				break;
			case MOUSE_PACKET_ID4_WHEEL_LEFT:
				event.event_union.mouse_event.vertical_wheel_movement = 0;
				event.event_union.mouse_event.horizontal_wheel_movement = -1;
				break;
			case MOUSE_PACKET_ID4_WHEEL_RIGHT:
				event.event_union.mouse_event.vertical_wheel_movement = 0;
				event.event_union.mouse_event.horizontal_wheel_movement = 1;
				break;
			case MOUSE_PACKET_ID4_WHEEL_UP:
				event.event_union.mouse_event.vertical_wheel_movement = 1;
				event.event_union.mouse_event.horizontal_wheel_movement = 0;
				break;
			default:
				ERROR_MESSAGE(); // Wrong mouse wheel state!
				signal_index = 0;
				return;
			}
			if(mouse_packet.packet & MOUSE_PACKET_ID4_4TH_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_4TH_BUTTON_PUSHED;
			if(mouse_packet.packet & MOUSE_PACKET_ID4_5TH_BUTTON_PUSHED)event.event_union.mouse_event.flags |= MOUSE_5TH_BUTTON_PUSHED;
			enqueue_event(&event);
			signal_index = 0;
		}
		break;
	default:
		ERROR_MESSAGE(); // Wrong mouse id!
		break;
	}
}

unsigned char get_mouse_id(void)
{
	return mouse_id;
}

void init_mouse(void)
{
	// upgrade mouse ID from 0 to 3
	set_mouse_sample_rate(200);
	set_mouse_sample_rate(100);
	set_mouse_sample_rate(80);
	send_to_mouse(MOUSE_COMMAND_GET_ID);
	printf_serial("mouse ACK = %#04x\n", receive_from_keyboard());
	mouse_id = receive_from_keyboard();
	printf_serial("mouse ID = %#04x\n", mouse_id);

	if(mouse_id == 3)
	{
		// upgrade mouse ID from 3 to 4
		set_mouse_sample_rate(200);
		set_mouse_sample_rate(200);
		set_mouse_sample_rate(80);
		send_to_mouse(MOUSE_COMMAND_GET_ID);
		printf_serial("mouse ACK = %#04x\n", receive_from_keyboard());
		mouse_id = receive_from_keyboard();
		printf_serial("mouse ID = %#04x\n", mouse_id);
	}

	// enable packet streaming
	send_to_mouse(MOUSE_COMMAND_ENABLE_PACKET_STREAMING);
	printf_serial("mouse ACK = %#04x\n\n", receive_from_keyboard());
}

void mouse_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_MOUSE);
	event.type = EVENT_TYPE_MOUSE_INTERRUPT;
	event.event_union.mouse_interrupt.signal = inb(PORT_KEYBOARD_DATA);
	enqueue_event(&event);
}

void send_to_mouse(unsigned char data)
{
	send_command_to_keyboard(KEYBOARD_COMMAND_SEND_TO_MOUSE, data);
}

void set_mouse_sample_rate(unsigned char rate)
{
	send_to_mouse(MOUSE_COMMAND_SET_SAMPLE_RATE);
	printf_serial("mouse ACK = %#04x\n", receive_from_keyboard());
	send_to_mouse(rate);
	printf_serial("mouse ACK = %#04x\n", receive_from_keyboard());
}

