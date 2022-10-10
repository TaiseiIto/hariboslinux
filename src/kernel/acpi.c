#include "acpi.h"
#include "serial.h"

MemoryRegionDescriptor get_acpi_memory_region_descriptor(void)
{
	MemoryRegionDescriptor acpi_memory_region_descriptor;
	unsigned int acpi_memory_region_descriptor_index;
	for(acpi_memory_region_descriptor = get_memory_region_descriptor(acpi_memory_region_descriptor_index = 0); acpi_memory_region_descriptor.base != 0 || acpi_memory_region_descriptor.length != 0 || acpi_memory_region_descriptor.type != 0 || acpi_memory_region_descriptor.attribute != 0; acpi_memory_region_descriptor = get_memory_region_descriptor(++acpi_memory_region_descriptor_index))if(acpi_memory_region_descriptor.type == MEMORY_REGION_ACPI)return acpi_memory_region_descriptor;
	ERROR(); // ACPI is not found.
	acpi_memory_region_descriptor.base = 0;
	acpi_memory_region_descriptor.length = 0;
	acpi_memory_region_descriptor.type = 0;
	acpi_memory_region_descriptor.attribute = 0;
	return acpi_memory_region_descriptor;
}

ACPITableHeader const *get_rsdt_header(void)
{
	return (ACPITableHeader const *)(unsigned int)get_acpi_memory_region_descriptor().base;
}

ACPITableHeader const *get_sdt_header(unsigned int index)
{
	ACPITableHeader const *rsdt_header =  get_rsdt_header();
	ACPITableHeader const * const *sdt_headers = (ACPITableHeader const * const *)((unsigned int)rsdt_header + sizeof(*rsdt_header));
	if(get_num_of_sdts() <= index)
	{
		ERROR(); // index is too big.
		return NULL;
	}
	return sdt_headers[index];
}

unsigned int get_num_of_sdts(void)
{
	ACPITableHeader const *rsdt_header =  get_rsdt_header();
	return (rsdt_header->length - sizeof(*rsdt_header)) / sizeof(ACPITableHeader*);
}

