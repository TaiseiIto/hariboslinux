#include "dev.h"
#include "stdio.h"
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
	printf("aml_file_name_prefix = %.*s\n", aml_file_name_prefix_length, aml_file_name_prefix);
	printf("aml_file_name_suffix = %.*s\n", aml_file_name_suffix_length, aml_file_name_suffix);
	return 0;
}

