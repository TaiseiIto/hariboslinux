// Real Time Clock

#include "cmos.h"
#include "event.h"
#include "pic.h"
#include "rtc.h"
#include "serial.h"

Time current_time;
Queue *rtc_interrupt_queue;

unsigned int difference_from_UTC = 9; // Japan

#define RTC_HOUR_REGISTER_PM_FLAG	0xf0

unsigned char status_register_b;
#define RTC_STATUS_REGISTER_B_24_HOURS_MODE		0x02
#define RTC_STATUS_REGISTER_B_BINARY_MODE		0x04
#define RTC_STATUS_REGISTER_B_ENABLE_UPDATE_INTERRUPT	0x10
#define RTC_STATUS_REGISTER_B_ENABLE_PERIODIC_INTERRUPT	0x40

#define RTC_STATUS_REGISTER_C_TIME_UPDATED		0x10

unsigned int end_of_month(unsigned int year, unsigned int month);
bool is_leap_year(int year);

unsigned int end_of_month(unsigned int year, unsigned int month)
{
	switch(month)
	{
	case 2:
		if(is_leap_year(year))return 29;
		else return 28;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	default:
		ERROR(); // Invalid month
		return 0;
	}
}

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

Time get_current_time(void)
{
	return current_time;
}

unsigned int get_unix_time(void)
{
	unsigned int unix_day = 0;
	unsigned int unix_hour = 0;
	unsigned int unix_minute = 0;
	for(unsigned int year = 1970; year < current_time.year; year++)unix_day += 365 + (unsigned int)is_leap_year(year);
	for(unsigned int month = 1; month < current_time.month; month++)unix_day += end_of_month(current_time.year, month);
	unix_day += current_time.day - 1;
	unix_hour = 24 * unix_day + current_time.hour - difference_from_UTC;
	unix_minute = 60 * unix_hour + current_time.minute;
	return 60 * unix_minute + current_time.second;
}

void init_rtc(Queue *interrupt_queue)
{
	rtc_interrupt_queue = interrupt_queue;
	// Enable IRQ8
	status_register_b = read_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT) | RTC_STATUS_REGISTER_B_ENABLE_UPDATE_INTERRUPT;
	write_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT, status_register_b);
	// read status register c
	read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
}

bool is_leap_year(int year)
{
	if(year % 400 == 0)return true;
	if(year % 100 == 0)return false;
	if(year % 4 == 0)return true;
	return false;
}

void rtc_interrupt_handler(void)
{
	unsigned char status_register_c;
	finish_interruption(IRQ_REAL_TIME_CLOCK);
	// read status register c
	status_register_c = read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
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
		current_time = event.event_union.rtc_interrupt;
		enqueue(rtc_interrupt_queue, &event);
	}
}

