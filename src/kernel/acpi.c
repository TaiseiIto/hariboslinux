#include "acpi.h"
#include "serial.h"
#include "stdbool.h"
#include "string.h"

bool acpi_table_is_correct(ACPITableHeader const *header);

bool acpi_table_is_correct(ACPITableHeader const *header)
{
	unsigned char sum = 0;
	for(unsigned char const *byte = (unsigned char const *)header; byte != (unsigned char const *)((unsigned int)header + header->length); byte++)sum += *byte;
	if(sum)
	{
		ERROR(); // It's an incorrect ACPI table!
		return false;
	}
	else return true;
}

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

ACPITableHeader const *get_dsdt_header(void)
{
	ACPITableHeader const *dsdt_header = get_fadt()->dsdt;
	if(acpi_table_is_correct(dsdt_header))return dsdt_header;
	else
	{
		ERROR(); // DSDT is incorrect!
		return NULL;
	}
}

FADT const *get_fadt(void)
{
	return (FADT const *)get_sdt_header("FACP");
}

unsigned int get_num_of_sdt_headers(void)
{
	ACPITableHeader const *rsdt_header = get_rsdt_header();
	return (rsdt_header->length - sizeof(*rsdt_header)) / sizeof(ACPITableHeader*);
}

ACPITableHeader const *get_rsdt_header(void)
{
	ACPITableHeader const *rsdt_header = (ACPITableHeader const *)(unsigned int)get_acpi_memory_region_descriptor().base;
	if(acpi_table_is_correct(rsdt_header))return rsdt_header;
	else
	{
		ERROR(); // RSDT is incorrect!
		return NULL;
	}
}

ACPITableHeader const *get_sdt_header(char const *signature)
{
	ACPITableHeader const * const *sdt_headers = get_sdt_headers();
	unsigned int num_of_sdt_headers = get_num_of_sdt_headers();
	for(unsigned int sdt_header_index = 0; sdt_header_index < num_of_sdt_headers; sdt_header_index++)if(!strncmp(sdt_headers[sdt_header_index]->signature, signature, sizeof(sdt_headers[sdt_header_index]->signature)) && acpi_table_is_correct(sdt_headers[sdt_header_index]))return sdt_headers[sdt_header_index];
	ERROR(); // SDT that has a specified signature is not found.
	return NULL;
}

ACPITableHeader const * const *get_sdt_headers(void)
{
	ACPITableHeader const *rsdt_header =  get_rsdt_header();
	return (ACPITableHeader const * const *)((unsigned int)rsdt_header + sizeof(*rsdt_header));
}

void print_acpi_table_header(ACPITableHeader acpi_table_header, char const *name)
{
	Shell *shell = get_current_shell();
	printf_shell(shell, "%s.signature = %.*s\n", name, sizeof(acpi_table_header.signature), acpi_table_header.signature);
	printf_shell(shell, "%s.length = %#010.8x\n", name, acpi_table_header.length);
	printf_shell(shell, "%s.revision = %#04.2x\n", name, acpi_table_header.revision);
	printf_shell(shell, "%s.checksum = %#04.2x\n", name, acpi_table_header.checksum);
	printf_shell(shell, "%s.oem_id = %.*s\n", name, sizeof(acpi_table_header.oem_id), acpi_table_header.oem_id);
	printf_shell(shell, "%s.oem_table_id = %.*s\n", name, sizeof(acpi_table_header.oem_table_id), acpi_table_header.oem_table_id);
	printf_shell(shell, "%s.oem_revision = %#010.8x\n", name, acpi_table_header.oem_revision);
	printf_shell(shell, "%s.creater_id = %#010.8x\n", name, acpi_table_header.creater_id);
	printf_shell(shell, "%s.creater_revision = %#010.8x\n", name, acpi_table_header.creater_revision);
}

void print_acpi_table_header_p(ACPITableHeader const *acpi_table_header, char const *name)
{
	Shell *shell = get_current_shell();
	printf_shell(shell, "%s->signature = %.*s\n", name, sizeof(acpi_table_header->signature), acpi_table_header->signature);
	printf_shell(shell, "%s->length = %#010.8x\n", name, acpi_table_header->length);
	printf_shell(shell, "%s->revision = %#04.2x\n", name, acpi_table_header->revision);
	printf_shell(shell, "%s->checksum = %#04.2x\n", name, acpi_table_header->checksum);
	printf_shell(shell, "%s->oem_id = %.*s\n", name, sizeof(acpi_table_header->oem_id), acpi_table_header->oem_id);
	printf_shell(shell, "%s->oem_table_id = %.*s\n", name, sizeof(acpi_table_header->oem_table_id), acpi_table_header->oem_table_id);
	printf_shell(shell, "%s->oem_revision = %#010.8x\n", name, acpi_table_header->oem_revision);
	printf_shell(shell, "%s->creater_id = %#010.8x\n", name, acpi_table_header->creater_id);
	printf_shell(shell, "%s->creater_revision = %#010.8x\n", name, acpi_table_header->creater_revision);
}

void print_generic_address_structure(GenericAddressStructure generic_address_structure, char const *name)
{
	Shell *shell = get_current_shell();
	printf_shell(shell, "%s.address_space = %#04.2x\n", name, generic_address_structure.address_space);
	printf_shell(shell, "%s.bit_width = %#04.2x\n", name, generic_address_structure.bit_width);
	printf_shell(shell, "%s.bit_offset = %#04.2x\n", name, generic_address_structure.bit_offset);
	printf_shell(shell, "%s.access_size = %#04.2x\n", name, generic_address_structure.access_size);
	printf_shell(shell, "%s.address = %#018.16x\n", name, generic_address_structure.address);
}

