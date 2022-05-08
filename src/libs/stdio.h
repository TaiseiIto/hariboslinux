#ifndef _STDIO_H_
#define _STDIO_H_

#include "stddef.h"

#define _STRING(x) #x
#define _STRING2(x) _STRING(x)
#define ERROR() puts("ERROR " __FILE__ " : " _STRING2(__LINE__) "\n")

typedef struct _FileIntormation
{
	char name[8];
	char extension[3];
	unsigned char flags;
	#define FILE_INFORMATION_FLAG_READ_ONLY_FILE	0x01
	#define FILE_INFORMATION_FLAG_HIDDEN_FILE	0x02
	#define FILE_INFORMATION_FLAG_SYSTEM_FILE	0x04
	#define FILE_INFORMATION_FLAG_DISK_NAME		0x08
	#define FILE_INFORMATION_FLAG_DIRECTORY		0x10
	#define FILE_INFORMATION_FLAG_NORMAL_FILE	0x20
	char reserved[10];
	unsigned short time;
	#define FILE_INFORMATION_TIME_MASK_BISECOND	0x001f
	#define FILE_INFORMATION_TIME_MASK_MINUTE	0x07e0
	#define FILE_INFORMATION_TIME_MASK_HOUR		0xf800
	unsigned short date;
	#define FILE_INFORMATION_DATE_MASK_DAY		0x001f
	#define FILE_INFORMATION_DATE_MASK_MONTH	0x01e0
	#define FILE_INFORMATION_DATE_MASK_YEAR		0xfe00
	unsigned short cluster_number;
	unsigned int size;
} FileInformation;

int fclose(unsigned int file_descriptor);
unsigned int fopen(char const *file_name, char const *mode);
size_t fread(void *buffer, size_t size, size_t number, unsigned int file_descriptor);
int fputs(char const *string, unsigned int file_descriptor);
int printf(const char *format, ...);
int puts(char const *string);

#endif

