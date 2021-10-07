// Real Time Clock

#include "cmos.h"
#include "pic.h"
#include "rtc.h"
#include "serial.h"

#define RTC_STATUS_REGISTER_B_ENABLE_PERIODIC_INTERRUPT 0x40

void init_rtc(void)
{
	const unsigned short frequency = 2; // Hz
	unsigned int frequency_divisor = 0x00010000 / frequency;
	unsigned char frequency_index = 0;
	while(1 < frequency_divisor)
	{
		frequency_divisor >>= 1;
		frequency_index++;
	}
	frequency_index &= 0x0f;
	printf_serial("frequency_index = %#04x\n", frequency_index);
	// Enable IRQ8
	write_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT, read_cmos_register(CMOS_REGISTER_RTC_STATUS_B | CMOS_DISABLE_NON_MASKABLE_INTERRUPT) | RTC_STATUS_REGISTER_B_ENABLE_PERIODIC_INTERRUPT);
	// Set periodic interrupt frequency
	write_cmos_register(CMOS_REGISTER_RTC_STATUS_A | CMOS_DISABLE_NON_MASKABLE_INTERRUPT, (read_cmos_register(CMOS_REGISTER_RTC_STATUS_A | CMOS_DISABLE_NON_MASKABLE_INTERRUPT) & 0xf0) + frequency_index);
	// read statuc register c
	read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
}

void rtc_interrupt_handler(void)
{
	unsigned char status_register_c;
	finish_interruption(IRQ_REAL_TIME_CLOCK);
	// read statuc register c
	status_register_c = read_cmos_register(CMOS_REGISTER_RTC_STATUS_C | CMOS_DISABLE_NON_MASKABLE_INTERRUPT);
	printf_serial("RTC interrupt status register c = %#04x\n", status_register_c);
}

