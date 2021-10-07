#ifndef _CMOS_H_
#define _CMOS_H_

// Registers
#define CMOS_REGISTER_RTC_SECOND		0x00
#define CMOS_REGISTER_RTC_MINUTE		0x02
#define CMOS_REGISTER_RTC_HOUR			0x04
#define CMOS_REGISTER_RTC_DAY_OF_WEEK		0x06
#define CMOS_REGISTER_RTC_DAY_OF_MONTH		0x07
#define CMOS_REGISTER_RTC_MONTH			0x08
#define CMOS_REGISTER_RTC_YEAR			0x09
#define CMOS_REGISTER_RTC_STATUS_A		0x0a
#define CMOS_REGISTER_RTC_STATUS_B		0x0a
#define CMOS_REGISTER_RTC_STATUS_C		0x0a
#define CMOS_REGISTER_RTC_CENTURY		0x32
#define CMOS_DISABLE_NON_MASKABLE_INTERRUPT	0x80

unsigned char read_cmos_register(unsigned char register_number);
void write_cmos_register(unsigned char register_number, unsigned char value);

#endif

