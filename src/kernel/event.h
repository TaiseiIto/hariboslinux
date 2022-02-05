#ifndef _EVENT_H_
#define _EVENT_H_

struct _Event;

#include "keyboard.h"
#include "mouse.h"
#include "queue.h"
#include "rtc.h"
#include "sheet.h"
#include "timer.h"

typedef union _EventUnion
{
	KeyboardEvent keyboard_event;
	KeyboardInterrupt keyboard_interrupt;
	MouseEvent mouse_event;
	MouseInterrupt mouse_interrupt;
	RTCInterrupt rtc_interrupt;
	SheetClickedEvent sheet_clicked_event;
	SheetCreatedEvent sheet_created_event;
	SheetMouseMoveEvent sheet_mouse_move_event;
	TimerEvent timer_event;
} EventUnion;

typedef struct _Event
{
	unsigned char type;
	#define EVENT_TYPE_KEYBOARD_EVENT	0x00
	#define EVENT_TYPE_KEYBOARD_INTERRUPT	0x01
	#define EVENT_TYPE_MOUSE_EVENT		0x02
	#define EVENT_TYPE_MOUSE_INTERRUPT	0x03
	#define EVENT_TYPE_PIT_INTERRUPT	0x04
	#define EVENT_TYPE_RTC_INTERRUPT	0x05
	#define EVENT_TYPE_SHEET_CLICKED	0x06
	#define EVENT_TYPE_SHEET_CREATED	0x07
	#define EVENT_TYPE_SHEET_MOUSE_MOVE	0x08
	#define EVENT_TYPE_TIMER_EVENT		0x09
	EventUnion event_union;
} Event;

extern Queue * event_queue;

void create_event_queue(void);
Event const *dequeue_event(void);
void enqueue_event(Event const *event);

#endif

