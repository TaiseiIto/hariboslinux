#ifndef _EVENT_H_
#define _EVENT_H_

struct _Event;

#include "keyboard.h"
#include "mouse.h"
#include "queue.h"
#include "rtc.h"
#include "sheet.h"
#include "task.h"
#include "timer.h"
#include "window.h"

typedef union _EventUnion
{
	CloseButtonClickedEvent close_button_clicked_event;
	KeyboardEvent keyboard_event;
	KeyboardInterrupt keyboard_interrupt;
	MouseEvent mouse_event;
	MouseInterrupt mouse_interrupt;
	RTCInterrupt rtc_interrupt;
	SheetClickedEvent sheet_clicked_event;
	SheetCreatedEvent sheet_created_event;
	SheetDeletionRequestEvent sheet_deletion_request_event;
	SheetDeletionResponseEvent sheet_deletion_response_event;
	SheetFocusedEvent sheet_focused_event;
	SheetKeyboardEvent sheet_keyboard_event;
	SheetMouseDragEvent sheet_mouse_drag_event;
	SheetMouseMoveEvent sheet_mouse_move_event;
	SheetUnfocusedEvent sheet_unfocused_event;
	SheetUserDefinedEvent sheet_user_defined_event;
	SheetVerticalWheelEvent sheet_vertical_wheel_event;
	TaskDeletionRequestEvent task_deletion_request_event;
	TaskDeletionResponseEvent task_deletion_response_event;
	TimerEvent timer_event;
	WindowDeletionRequestEvent window_deletion_request_event;
	WindowDeletionResponseEvent window_deletion_response_event;
	WindowFocusedEvent window_focused_event;
	WindowUnfocusedEvent window_unfocused_event;
} EventUnion;

typedef struct _Event
{
	unsigned char type;
	#define EVENT_TYPE_CLOSE_BUTTON_CLICKED 	0x00
	#define EVENT_TYPE_KEYBOARD_EVENT		0x01
	#define EVENT_TYPE_KEYBOARD_INTERRUPT		0x02
	#define EVENT_TYPE_MOUSE_EVENT			0x03
	#define EVENT_TYPE_MOUSE_INTERRUPT		0x04
	#define EVENT_TYPE_PIT_INTERRUPT		0x05
	#define EVENT_TYPE_RTC_INTERRUPT		0x06
	#define EVENT_TYPE_SHEET_CLICKED		0x07
	#define EVENT_TYPE_SHEET_CREATED		0x08
	#define EVENT_TYPE_SHEET_DELETION_REQUEST	0x09
	#define EVENT_TYPE_SHEET_DELETION_RESPONSE	0x0a
	#define EVENT_TYPE_SHEET_FOCUSED		0x0b
	#define EVENT_TYPE_SHEET_KEYBOARD		0x0c
	#define EVENT_TYPE_SHEET_MOUSE_DRAG		0x0d
	#define EVENT_TYPE_SHEET_MOUSE_MOVE		0x0e
	#define EVENT_TYPE_SHEET_UNFOCUSED		0x0f
	#define EVENT_TYPE_SHEET_USER_DEFINED		0x10
	#define EVENT_TYPE_SHEET_VERTICAL_WHEEL		0x11
	#define EVENT_TYPE_TASK_DELETION_REQUEST	0x12
	#define EVENT_TYPE_TASK_DELETION_RESPONSE	0x13
	#define EVENT_TYPE_TIMER_EVENT			0x14
	#define EVENT_TYPE_WINDOW_DELETION_REQUEST	0x15
	#define EVENT_TYPE_WINDOW_DELETION_RESPONSE	0x16
	#define EVENT_TYPE_WINDOW_FOCUSED		0x17
	#define EVENT_TYPE_WINDOW_UNFOCUSED		0x18
	EventUnion event_union;
} Event;

#endif

