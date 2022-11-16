#include "dev.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char const * const * const argv)
{
	if(argc <= 1)
	{
		printf("%s : Satisfy a file name.\n", argv[0]);
		return -1;
	}
	char const *aml_file_name = argv[1];
	char const *aml_file_name_prefix = aml_file_name;
	unsigned int aml_file_name_prefix_length = (unsigned int)strchr(aml_file_name_prefix, '.') - (unsigned int)aml_file_name_prefix;
	char const *aml_file_name_suffix = aml_file_name_prefix + aml_file_name_prefix_length + 1;
	unsigned int aml_file_name_suffix_length = strlen(aml_file_name_suffix);
	FileInformation aml_file_information;
	char aml_file_name_on_aml_file_information[_countof(aml_file_information.name) + _countof(aml_file_information.extension)];
	memset(aml_file_name_on_aml_file_information, ' ', sizeof(aml_file_name_on_aml_file_information));
	strncpy(aml_file_name_on_aml_file_information, aml_file_name_prefix, aml_file_name_prefix_length);
	strncpy(aml_file_name_on_aml_file_information + _countof(aml_file_information.name), aml_file_name_suffix, aml_file_name_suffix_length);
	unsigned int root_directory = fopen("", "r");
	while(fread(&aml_file_information, sizeof(aml_file_information), 1, root_directory))if(!strncmp(aml_file_information.name, aml_file_name_on_aml_file_information, sizeof(aml_file_name_on_aml_file_information)))
	{
		unsigned int aml_file = fopen(aml_file_name, "r");
		unsigned char *aml = malloc(aml_file_information.size);
		fread(aml, 1, aml_file_information.size, aml_file);
		for(unsigned int i = 0; i < aml_file_information.size; i++)printf("%2.2X%c", aml[i], (i + 1) % 0x10 ? ' ' : '\n');
		printf("\n");
		free(aml);
		fclose(aml_file);
		break;
	}
	fclose(root_directory);
	return 0;
}

