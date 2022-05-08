#include "stdio.h"
#include "string.h"

int main(void)
{
	unsigned int file_descriptor = fopen("console.dev", "w");
	char const * const command = "clear";
	fwrite(command, sizeof(command[0]), strlen(command), file_descriptor);
	fclose(file_descriptor);
	return 0;
}

