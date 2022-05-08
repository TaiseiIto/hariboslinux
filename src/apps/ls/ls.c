#include "stdio.h"
#include "string.h"

int main(void)
{
	FileInformation file_information;
	unsigned int file_descriptor = fopen("", "r");
	while(fread(&file_information, sizeof(file_information), 1, file_descriptor) && strcmp(file_information.name, ""))printf("%.*s.%.*s\n", sizeof(file_information.name), file_information.name, sizeof(file_information.extension), file_information.extension);
	fclose(file_descriptor);
	return 0;
}

