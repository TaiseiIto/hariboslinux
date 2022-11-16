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
	FileInformation file_information;
	char aml_file_name_on_file_information[_countof(file_information.name) + _countof(file_information.extension)];
	memset(aml_file_name_on_file_information, ' ', sizeof(aml_file_name_on_file_information));
	strncpy(aml_file_name_on_file_information, aml_file_name_prefix, aml_file_name_prefix_length);
	strncpy(aml_file_name_on_file_information + _countof(file_information.name), aml_file_name_suffix, aml_file_name_suffix_length);
	unsigned int root_directory = fopen("", "r");
	while(fread(&file_information, sizeof(file_information), 1, root_directory))if(!strncmp(file_information.name, aml_file_name_on_file_information, sizeof(aml_file_name_on_file_information)))
	{
		printf("%s is found!\n", aml_file_name);
		break;
	}
	fclose(root_directory);
	return 0;
}

