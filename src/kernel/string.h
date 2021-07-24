#ifndef _HARIB_STRING_H_
#define _HARIB_STRING_H_

void *harib_memset(void *buf, char ch, unsigned int size);
int harib_strcmp(char const *string1, char const *string2);
char *harib_strcpy(char *dst, char const *src);
unsigned int harib_strlen(char const *string);

#endif
