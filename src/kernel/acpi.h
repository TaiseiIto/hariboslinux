#ifndef _ACPI_H_
#define _ACPI_H_

#include "memory.h"

typedef struct _ACPITableHeader
{
	char signature[4];
	unsigned int length;
	unsigned char revision;
	unsigned char checksum;
	char oem_id[6];
	char oem_table_id[8];
	unsigned int oem_revision;
	unsigned int creater_id;
	unsigned int creater_revision;
} ACPITableHeader;

ACPITableHeader const *find_sdt_header(char const *signature);
MemoryRegionDescriptor get_acpi_memory_region_descriptor(void);
ACPITableHeader const *get_rsdt_header(void);
ACPITableHeader const * const *get_sdt_headers(void);
unsigned int get_num_of_sdt_headers(void);

#endif

