#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(void)
{
	FileInformation file_information;
	unsigned int file_descriptor = fopen("", "r");
	while(fread(&file_information, sizeof(file_information), 1, file_descriptor))
	{
 		if(!strcmp(file_information.name, ""))continue;
		for(char *file_name_manipulator = file_information.name; file_name_manipulator < file_information.name + _countof(file_information.name); file_name_manipulator++)if(*file_name_manipulator == ' ')*file_name_manipulator = '\0';
		for(char *file_name_manipulator = file_information.extension; file_name_manipulator < file_information.extension + _countof(file_information.extension); file_name_manipulator++)if(*file_name_manipulator == ' ')*file_name_manipulator = '\0';
		printf("%.*s.%.*s\n", sizeof(file_information.name), file_information.name, sizeof(file_information.extension), file_information.extension);
	}
	fclose(file_descriptor);
	return 0;
}

