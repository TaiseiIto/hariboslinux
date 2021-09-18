#include "string.h"

void *memcpy(void *destination, void const *source, size_t size)
{
	void const *reader;
	void *writer;
	if(destination == source)return NULL;
	if((unsigned int)source < (unsigned int)destination && (unsigned int)destination < (unsigned int)source + size) // reverse copy
	{
		reader = source + size;
		writer = destination;
		while(size)
		{
			if(sizeof(unsigned int) <= size)
			{
				*--((unsigned int *)writer) = *--((unsigned int *)reader);
				size -= sizeof(unsigned int);
			}
			else if(sizeof(unsigned short) <= size)
			{
				*--((unsigned short *)writer) = *--((unsigned short *)reader);
				size -= sizeof(unsigned short);
			}
			else
			{
				*--((unsigned char *)writer) = *--((unsigned char *)reader);
				size -= sizeof(unsigned char);
			}
		}
	}
	else // forward copy
	{
		reader = source;
		writer = destination;
		while(size)
		{
			if(sizeof(unsigned int) <= size)
			{
				*((unsigned int *)writer)++ = *((unsigned int *)reader)++;
				size -= sizeof(unsigned int);
			}
			else if(sizeof(unsigned short) <= size)
			{
				*((unsigned short *)writer)++ = *((unsigned short *)reader)++;
				size -= sizeof(unsigned short);
			}
			else
			{
				*((unsigned char *)writer)++ = *((unsigned char *)reader)++;
				size -= sizeof(unsigned char);
			}
		}
	}
	return destination;
}

void *memset(void *buf, char ch, size_t size)
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

char *strcpy(char *destination, char const *source)
{
	char const *reader;
	char *writer = destination;
	for(reader = source; *reader; reader++)*(writer++) = *reader;
	*writer = '\0';
	return destination;
}

unsigned int strlen(char const *string)
{
	unsigned int length = 0;
	while(*string++)length++;
	return length;
}
