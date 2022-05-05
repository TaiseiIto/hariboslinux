#include "ctype.h"

int isalnum(char c)
{
	return isalpha(c) || isdigit(c);
}

int isalpha(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

int isdigit(char c)
{
	return '0' <= c && c <= '9';
}

