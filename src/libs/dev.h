#ifndef _DEV_H_
#define _DEV_H_

typedef struct _ApplicationWindowCreatedEvent
{
	unsigned int window;
} ApplicationWindowCreatedEvent;

typedef struct _ApplicationWindowDeletionResponseEvent
{
	unsigned int window;
} ApplicationWindowDeletionResponseEvent;

typedef union _ApplicationEventUnion
{
	ApplicationWindowCreatedEvent window_created_event;
	ApplicationWindowDeletionResponseEvent window_deletion_response_event;
} ApplicationEventUnion;

typedef struct _ApplicationEvent
{
	ApplicationEventUnion event_union;
	unsigned char type;
	#define APPLICATION_EVENT_TYPE_NOTHING			0x00
	#define APPLICATION_EVENT_TYPE_WINDOW_CREATED		0x01
	#define APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE	0x02
} ApplicationEvent;

typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} Color;

void clear_console(void);
unsigned int create_window(char const *title, short x, short y, unsigned short width, unsigned short height);
ApplicationEvent dequeue_application_event(void);
void draw_line_window(unsigned int window, short x1, short y1, short x2, short y2, Color color);
void exit_console(void);
void fill_box_window(unsigned int window, short x, short y, unsigned short width, unsigned short height, Color color);
unsigned int get_free_memory_space_size(void);
unsigned int get_unix_time(void);
void hlt_application(void);
void print_window(unsigned int window, short x, short y, Color foreground, Color background, char const *string);
void process_event(void);
void put_dot_window(unsigned int window, unsigned short x, unsigned short y, Color color);

#endif

