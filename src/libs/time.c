#include "dev.h"
#include "time.h"

time_t time(time_t *time)
{
	time_t unix_time = get_unix_time();
	if(time)*time = unix_time;
	return unix_time;
}

