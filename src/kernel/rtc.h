#ifndef _RTC_H_
#define _RTC_H_

// Real Time Clock

#include "queue.h"

typedef struct _RTCInterrupt
{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned short year;
} RTCInterrupt;

typedef RTCInterrupt Time;

unsigned char get_day_of_week(unsigned short year, unsigned char month, unsigned char day);
#define RTC_SATURDAY	0x00
#define RTC_SUNDAY	0x01
#define RTC_MONDAY	0x02
#define RTC_TUESDAY	0x03
#define RTC_WEDNESDAY	0x04
#define RTC_THURSDAY	0x05
#define RTC_FRIDAY	0x06

char const *get_day_of_week_string(unsigned char day_of_week);
Time get_current_time(void);
unsigned int get_unix_time(void);
void init_rtc(Queue *interrupt_queue);
void rtc_interrupt_handler(void);

#endif

