#ifndef _RTC_H_
#define _RTC_H_

// Real Time Clock

typedef struct _RTCInterrupt
{
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day_of_week;
	#define RTC_SUNDAY	0x01
	#define RTC_MONDAY	0x02
	#define RTC_TUESDAY	0x03
	#define RTC_WEDNESDAY	0x04
	#define RTC_THURSDAY	0x05
	#define RTC_FRIDAY	0x06
	#define RTC_SATURDAY	0x07
	unsigned char day_of_month;
	unsigned char month;
	unsigned short year;
} RTCInterrupt;

void init_rtc(void);
void rtc_interrupt_handler(void);

#endif

