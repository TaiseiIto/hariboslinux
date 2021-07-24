#include "string.h"

void *memset(void *buf, char ch, unsigned int size)
{
	char *writer;
	for(writer = buf; size > 0; size--)*writer++ = ch;
	return buf;
}

int strcmp(char const *string1, char const *string2)
{
	while(*string1 && *string2)
	{
		if(*string1 > *string2)return 1;
		else if(*string1 < *string2)return -1;
		string1++;
		string2++;
	}
	if(*string1)return 1;
	else if(*string2)return -1;
	else return 0;
}

char *strcpy(char *dst, char const *src)
{
	char const *reader;
	char *writer = dst;
	for(reader = src; *reader; reader++)*(writer++) = *reader;
	*writer = '\0';
	return dst;
}

unsigned int strlen(char const *string)
{
	unsigned int length = 0;
	while(*string++)length++;
	return length;
}
