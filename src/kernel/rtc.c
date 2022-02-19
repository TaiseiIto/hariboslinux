// Real Time Clock

#include "cmos.h"
#include "event.h"
#include "pic.h"
#include "rtc.h"
#include "serial.h"

Queue *rtc_interrupt_queue;

#define RTC_HOUR_REGISTER_PM_FLAG	0xf0

unsigned char status_register_b;
#define RTC_STATUS_REGISTER_B_24_HOURS_MODE		0x02
#define RTC_STATUS_REGISTER_B_BINARY_MODE		0x04
#define RTC_STATUS_REGISTER_B_ENABLE_UPDATE_INTERRUPT	0x10
#define RTC_STATUS_REGISTER_B_ENABLE_PERIODIC_INTERRUPT	0x40

#define RTC_STATUS_REGISTER_C_TIME_UPDATED		0x10

// https://en.wikipedia.org/wiki/Zeller%27s_congruence
unsigned char get_day_of_week(unsigned short year, unsigned char month, unsigned char day)
{
	unsigned int year_of_the_century;
	unsigned int zero_based_century;
	if(month <= 2)
	{
		month += 12;
		year--;
	}
	year_of_the_century = year % 100;
	zero_based_century = year / 100;
	return (day + 13 * (month + 1) / 5 + year_of_the_century + year_of_the_century / 4 + zero_based_century / 4 - 2 * zero_based_century) % 7;
}

char const *get_day_of_week_string(unsigned char day_of_week)
{
	static char const * const sunday = "Sunday";
	static char const * const monday = "Monday";
	static char const * const tuesday = "Tuesday";
	static char const * const wednesday = "Wednesday";
	static char const * const thursday = "Thursday";
	static char const * const friday = "Friday";
	static char const * const saturday = "Saturday";
	static char const * const invalid_day_of_week = "INVALID DAY OF WEEK!";
	switch(day_of_week)
	{
	case RTC_SUNDAY:
		return sunday;
	case RTC_MONDAY:
		return monday;
	case RTC_TUESDAY:
		return tuesday;
	case RTC_WEDNESDAY:
		return wednesday;
	case RTC_THURSDAY:
		return thursday;
	case RTC_FRIDAY:
		return friday;
	case RTC_SATURDAY:
		return saturday;
	default:
		return invalid_day_of_week;
	}
}

void init_rtc(Queue *interrupt_queue)
{
	rtc_interrupt_queue = interrupt_queue;
	// Enable IRQ8
	status_register_b = read_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT) | RTC_STATUS_REGISTER_B_ENABLE_UPDATE_INTERRUPT;
	write_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT, status_register_b);
	// read statuc register c
	read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
}

void rtc_interrupt_handler(void)
{
	unsigned char status_register_c;
	finish_interruption(IRQ_REAL_TIME_CLOCK);
	// read statuc register c
	status_register_c = read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
	#ifdef LOGGING
	printf_serial("RTC interrupt status register c = %#04x\n", status_register_c);
	#endif
	if(status_register_c & RTC_STATUS_REGISTER_C_TIME_UPDATED)
	{
		Event event;
		unsigned short century;
		event.type = EVENT_TYPE_RTC_INTERRUPT;
		event.event_union.rtc_interrupt.second = read_cmos_register(CMOS_REGISTER_RTC_SECOND);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.second = 10 * (event.event_union.rtc_interrupt.second >> 4) + (event.event_union.rtc_interrupt.second & 0x0f);
		event.event_union.rtc_interrupt.minute = read_cmos_register(CMOS_REGISTER_RTC_MINUTE);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.minute = 10 * (event.event_union.rtc_interrupt.minute >> 4) + (event.event_union.rtc_interrupt.minute & 0x0f);
		event.event_union.rtc_interrupt.hour = read_cmos_register(CMOS_REGISTER_RTC_HOUR);
		if(status_register_b & RTC_STATUS_REGISTER_B_24_HOURS_MODE)
		{
			if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.hour = 10 * (event.event_union.rtc_interrupt.hour >> 4) + (event.event_union.rtc_interrupt.hour & 0x0f);
		}
		else
		{
			if(event.event_union.rtc_interrupt.hour & RTC_HOUR_REGISTER_PM_FLAG) // PM
			{
				event.event_union.rtc_interrupt.hour &= ~RTC_HOUR_REGISTER_PM_FLAG;
				if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.hour = 10 * (event.event_union.rtc_interrupt.hour >> 4) + (event.event_union.rtc_interrupt.hour & 0x0f);
				event.event_union.rtc_interrupt.hour += 12;
			}
			else // AM
			{
				if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.hour = 10 * (event.event_union.rtc_interrupt.hour >> 4) + (event.event_union.rtc_interrupt.hour & 0x0f);
			}
		}
		event.event_union.rtc_interrupt.day = read_cmos_register(CMOS_REGISTER_RTC_DAY_OF_MONTH);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.day = 10 * (event.event_union.rtc_interrupt.day >> 4) + (event.event_union.rtc_interrupt.day & 0x0f);
		event.event_union.rtc_interrupt.month = read_cmos_register(CMOS_REGISTER_RTC_MONTH);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.month = 10 * (event.event_union.rtc_interrupt.month >> 4) + (event.event_union.rtc_interrupt.month & 0x0f);
		event.event_union.rtc_interrupt.year = read_cmos_register(CMOS_REGISTER_RTC_YEAR);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))event.event_union.rtc_interrupt.year = 10 * (event.event_union.rtc_interrupt.year >> 4) + (event.event_union.rtc_interrupt.year & 0x0f);
		century = read_cmos_register(CMOS_REGISTER_RTC_CENTURY);
		if(!(status_register_b & RTC_STATUS_REGISTER_B_BINARY_MODE))century = 10 * (century >> 4) + (century & 0x0f);
		event.event_union.rtc_interrupt.year += 100 * century;
		enqueue(rtc_interrupt_queue, &event);
	}
}

