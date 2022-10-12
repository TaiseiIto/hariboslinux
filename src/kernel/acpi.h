// bibliography
// https://uefi.org/sites/default/files/resources/ACPI_Spec_6_4_Jan22.pdf
// https://wiki.osdev.org/FADT

#ifndef _ACPI_H_
#define _ACPI_H_

#include "chain_string.h"
#include "memory.h"

#define PRINT_ACPI_TABLE_HEADER(x) print_acpi_table_header((x), _STRING(x))
#define PRINT_ACPI_TABLE_HEADER_P(x) print_acpi_table_header_p((x), _STRING(x))
#define PRINT_GENERIC_ADDRESS_STRUCTURE(x) print_generic_address_structure((x), _STRING(x))

// ACPI structures

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
} __attribute__((packed)) ACPITableHeader;

typedef struct _GenericAddressStructure
{
	unsigned char address_space;
	#define GENERIC_ADDRESS_STRUCTURE_SYSTEM_MEMORY			0x00
	#define GENERIC_ADDRESS_STRUCTURE_SYSTEM_IO			0x01
	#define GENERIC_ADDRESS_STRUCTURE_PCI_CONFIGURATION		0x02
	#define GENERIC_ADDRESS_STRUCTURE_EMBEDDED_CONTROLLER		0x03
	#define GENERIC_ADDRESS_STRUCTURE_SYSTEM_MANAGEMENT_BUS		0x04
	#define GENERIC_ADDRESS_STRUCTURE_SYSTEM_CMOS			0x05
	#define GENERIC_ADDRESS_STRUCTURE_PCI_BAR_TARGET		0x06
	#define GENERIC_ADDRESS_STRUCTURE_IPMI				0x07
	#define GENERIC_ADDRESS_STRUCTURE_GENERAL_PURPOSE_IO		0x08
	#define GENERIC_ADDRESS_STRUCTURE_GENERIC_SERIAL_BUS		0x09
	#define GENERIC_ADDRESS_STRUCTURE_PCC				0x0a
	// 0x0b to 0x7e are reserved.
	#define GENERIC_ADDRESS_STRUCTURE_FUNCTIONAL_FIXED_HARDWARE	0x7f
	// 0x80 to 0xff are defined by each OEM.
	unsigned char bit_width;
	unsigned char bit_offset;
	unsigned char access_size;
	#define GENERIC_ADDRESS_STRUCTURE_ACCESS_SIZE_UNDEFINED	0x00
	#define GENERIC_ADDRESS_STRUCTURE_ACCESS_SIZE_BYTE	0x01
	#define GENERIC_ADDRESS_STRUCTURE_ACCESS_SIZE_WORD	0x02
	#define GENERIC_ADDRESS_STRUCTURE_ACCESS_SIZE_DWORD	0x03
	#define GENERIC_ADDRESS_STRUCTURE_ACCESS_SIZE_QWORD	0x04
	unsigned long long address;
} __attribute__((packed)) GenericAddressStructure;

// https://uefi.org/sites/default/files/resources/ACPI_6_3_final_Jan30.pdf
// page 126
// 5.2.9 Fixed ACPI Description Table (FADT)
typedef struct _FADT
{
	ACPITableHeader header;
	void *firmware_ctrl;
	ACPITableHeader const *dsdt;
	unsigned char reserved0;
	unsigned char preferred_pm_profile;
	#define FADT_PREFERRED_PM_PROFILE_UNSPECIFIED		0x00
	#define FADT_PREFERRED_PM_PROFILE_DESKTOP		0x01
	#define FADT_PREFERRED_PM_PROFILE_MOBILE		0x02
	#define FADT_PREFERRED_PM_PROFILE_WORKSTATION		0x03
	#define FADT_PREFERRED_PM_PROFILE_ENTERPRISE_SERVER	0x04
	#define FADT_PREFERRED_PM_PROFILE_SOHO_SERVER		0x05
	#define FADT_PREFERRED_PM_PROFILE_APPLIANCE_PC		0x06
	#define FADT_PREFERRED_PM_PROFILE_PERFORMANCE_SERVER	0x07
	#define FADT_PREFERRED_PM_PROFILE_PERFORMANCE_TABLET	0x08
	unsigned short sci_int;
	unsigned int smi_cmd;
	unsigned char acpi_enable;
	unsigned char acpi_disable;
	unsigned char s4_bios_req;
	unsigned char pstate_cnt;
	unsigned int pm1a_evt_blk;
	unsigned int pm1b_evt_blk;
	unsigned int pm1a_cnt_blk;
	unsigned int pm1b_cnt_blk;
	unsigned int pm2_cnt_blk;
	unsigned int pm_tmr_blk;
	unsigned int gpe0_blk;
	unsigned int gpe1_blk;
	unsigned char pm1_evt_len;
	unsigned char pm1_cnt_len;
	unsigned char pm2_cnt_len;
	unsigned char pm_tmr_len;
	unsigned char gpe0_blk_len;
	unsigned char gpe1_blk_len;
	unsigned char gpe1_base;
	unsigned char cst_cnt;
	unsigned short p_lvl2_lat;
	unsigned short p_lvl3_lat;
	unsigned short flush_size;
	unsigned short flush_stride;
	unsigned char duty_offset;
	unsigned char duty_width;
	unsigned char day_alrm;
	unsigned char mon_alrm;
	unsigned char century;
	unsigned short iapc_boot_arch;
	#define FADT_IAPC_BOOT_ARCH_LEGACY_DEVICES		0x0001
	#define FADT_IAPC_BOOT_ARCH_8042			0x0002
	#define FADT_IAPC_BOOT_ARCH_VGA_NOT_PRESENT		0x0004
	#define FADT_IAPC_BOOT_ARCH_MSI_NOT_SUPPORTED		0x0008
	#define FADT_IAPC_BOOT_ARCH_PCIE_ASPM_CONTROLS		0x0010
	#define FADT_IAPC_BOOT_ARCH_CMOS_RTC_NOT_PRESENT	0x0020
	unsigned char reserved1; // It must be 0.
	unsigned int flags;
	#define FADT_FLAG_WBINVD				0x00000001
	#define FADT_FLAG_WBINVD_FLUSH				0x00000002
	#define FADT_FLAG_PROC_C1				0x00000004
	#define FADT_FLAG_P_LVL2_UP				0x00000008
	#define FADT_FLAG_PWR_BUTTON				0x00000010
	#define FADT_FLAG_SLP_BUTTON				0x00000020
	#define FADT_FLAG_FIX_RTC				0x00000040
	#define FADT_FLAG_RTC_S4				0x00000080
	#define FADT_FLAG_TMR_VAL_EXT				0x00000100
	#define FADT_FLAG_DCK_CAP				0x00000200
	#define FADT_FLAG_RESET_REG_SUP				0x00000400
	#define FADT_FLAG_SEALED_CASE				0x00000800
	#define FADT_FLAG_HEADLESS				0x00001000
	#define FADT_FLAG_CPU_SW_SLP				0x00002000
	#define FADT_FLAG_PCI_EXP_WAK				0x00004000
	#define FADT_FLAG_USE_PLATFORM_CLOCK			0x00008000
	#define FADT_FLAG_S4_RTC_STS_VALID			0x00010000
	#define FADT_FLAG_REMOTE_POWER_ON_CAPABLE		0x00020000
	#define FADT_FLAG_FORCE_APIC_CLUSTER_MODEL		0x00040000
	#define FADT_FLAG_FORCE_APIC_PHYSICAL_DESTINATION_MODE	0x00080000
	#define FADT_FLAG_HW_REDUCED_ACPI			0x00100000
	#define FADT_FLAG_LOW_POWER_S0_IDLE_CAPABLE		0x00200000
	GenericAddressStructure reset_reg;
	unsigned char reset_value;
	unsigned short arm_boot_arch;
	#define FADT_ARM_BOOT_ARCH_PSCI_COMPLIANT	0x0001
	#define FADT_ARM_BOOT_ARCH_PSCI_USE_HVC		0x0002
	unsigned char fadt_minor_version;
	unsigned long long x_firmware_ctrl;
	unsigned long long x_dsdt;
	GenericAddressStructure x_pm1a_evt_blk;
	GenericAddressStructure x_pm1b_evt_blk;
	GenericAddressStructure x_pm1a_cnt_blk;
	GenericAddressStructure x_pm1b_cnt_blk;
	GenericAddressStructure x_pm2_cnt_blk;
	GenericAddressStructure x_pm_tmr_blk;
	GenericAddressStructure x_pm_gpe0_blk;
	GenericAddressStructure x_pm_gpe1_blk;
	GenericAddressStructure sleep_control_reg;
	GenericAddressStructure sleep_status_reg;
	unsigned long long hypervisor_vender_identity;
} __attribute__((packed)) FADT;

// Structures related to AML

struct _AMLSymbol;

typedef enum _AMLSymbolType
{
	aml_alias_op,
	aml_def_alias,
	aml_expression_opcode,
	aml_name_space_modifier_obj,
	aml_name_string,
	aml_object,
	aml_statement_opcode,
	aml_term_list,
	aml_term_obj,
} AMLSymbolType;

typedef struct _AMLSubstring
{
	unsigned char const *initial;
	size_t length;
} AMLSubstring;

typedef struct _AMLDefAlias
{
	struct _AMLSymbol *alias_op;
	struct _AMLSymbol *name_string_a;
	struct _AMLSymbol *name_string_b;
} AMLDefAlias;

typedef struct _AMLExpressionOpcode
{
	struct _AMLSymbol *def_add;
	struct _AMLSymbol *def_and;
	struct _AMLSymbol *def_aquire;
	struct _AMLSymbol *def_buffer;
	struct _AMLSymbol *def_concat;
	struct _AMLSymbol *def_concat_res;
	struct _AMLSymbol *def_cond_ref_of;
	struct _AMLSymbol *def_copy_object;
	struct _AMLSymbol *def_decrement;
	struct _AMLSymbol *def_deref_of;
	struct _AMLSymbol *def_divide;
	struct _AMLSymbol *def_find_set_left_bit;
	struct _AMLSymbol *def_find_set_right_bit;
	struct _AMLSymbol *def_from_bcd;
	struct _AMLSymbol *def_increment;
	struct _AMLSymbol *def_index;
	struct _AMLSymbol *def_l_and;
	struct _AMLSymbol *def_l_equal;
	struct _AMLSymbol *def_l_greater;
	struct _AMLSymbol *def_l_greater_equal;
	struct _AMLSymbol *def_l_less;
	struct _AMLSymbol *def_l_less_equal;
	struct _AMLSymbol *def_l_not;
	struct _AMLSymbol *def_l_not_equal;
	struct _AMLSymbol *def_l_or;
	struct _AMLSymbol *def_load_table;
	struct _AMLSymbol *def_match;
	struct _AMLSymbol *def_mid;
	struct _AMLSymbol *def_mod;
	struct _AMLSymbol *def_multiply;
	struct _AMLSymbol *def_nand;
	struct _AMLSymbol *def_nor;
	struct _AMLSymbol *def_not;
	struct _AMLSymbol *def_object_type;
	struct _AMLSymbol *def_or;
	struct _AMLSymbol *def_package;
	struct _AMLSymbol *def_ref_of;
	struct _AMLSymbol *def_shift_left;
	struct _AMLSymbol *def_shift_right;
	struct _AMLSymbol *def_size_of;
	struct _AMLSymbol *def_store;
	struct _AMLSymbol *def_subtract;
	struct _AMLSymbol *def_timer;
	struct _AMLSymbol *def_to_bcd;
	struct _AMLSymbol *def_to_buffer;
	struct _AMLSymbol *def_to_decimal_string;
	struct _AMLSymbol *def_to_hex_string;
	struct _AMLSymbol *def_to_integer;
	struct _AMLSymbol *def_to_string;
	struct _AMLSymbol *def_var_package;
	struct _AMLSymbol *def_wait;
	struct _AMLSymbol *def_xor;
	struct _AMLSymbol *method_invocation;
} AMLExpressionOpcode;

typedef struct _AMLNameSpaceModifierObj
{
	struct _AMLSymbol *def_alias;
	struct _AMLSymbol *def_name;
	struct _AMLSymbol *def_scope;
} AMLNameSpaceModifierObj;

typedef struct _AMLNameString
{
	struct _AMLSymbol *name_path;
	struct _AMLSymbol *prefix_path;
	struct _AMLSymbol *root_char;
} AMLNameString;

typedef struct _AMLObject
{
	struct _AMLSymbol *named_obj;
	struct _AMLSymbol *name_space_modifier_obj;
} AMLObject;

typedef struct _AMLStatementOpcode
{
	struct _AMLSymbol *def_break;
	struct _AMLSymbol *def_break_point;
	struct _AMLSymbol *def_continue;
	struct _AMLSymbol *def_fatal;
	struct _AMLSymbol *def_if_else;
	struct _AMLSymbol *def_noop;
	struct _AMLSymbol *def_notify;
	struct _AMLSymbol *def_release;
	struct _AMLSymbol *def_reset;
	struct _AMLSymbol *def_return;
	struct _AMLSymbol *def_signal;
	struct _AMLSymbol *def_sleep;
	struct _AMLSymbol *def_stall;
	struct _AMLSymbol *def_while;
} AMLStatementOpcode;

typedef struct _AMLTermList
{
	struct _AMLSymbol *term_list;
	struct _AMLSymbol *term_obj;
} AMLTermList;

typedef struct _AMLTermObj
{
	struct _AMLSymbol *object;
	struct _AMLSymbol *statement_opcode;
	struct _AMLSymbol *expression_opcode;
} AMLTermObj;

typedef union _AMLComponent
{
	AMLDefAlias def_alias;
	AMLExpressionOpcode expression_opcode;
	AMLNameSpaceModifierObj name_space_modifier_obj;
	AMLNameString name_string;
	AMLObject object;
	AMLStatementOpcode statement_opcode;
	AMLTermList term_list;
	AMLTermObj term_obj;
} AMLComponent;

typedef struct _AMLSymbol
{
	AMLSubstring string;
	AMLSymbolType type;
	AMLComponent component;
} AMLSymbol;

ChainString *aml_symbol_to_chain_string(AMLSymbol const *aml_symbol);
char *aml_symbol_to_string(AMLSymbol const *aml_symbol);
char const *aml_symbol_type_name(AMLSymbolType aml_symbol_type);
// <alias_op> := 0x06
AMLSymbol *analyse_aml_alias_op(AMLSubstring aml);
// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSubstring aml);
// <expression_opcode> := <def_aquire> | <def_add> | <def_and> | <def_buffer> | <def_concat> | <def_concat_res> | <def_cond_ref_of> | <def_copy_object> | <def_decrement> | <def_deref_of> | <def_divide> | <def_find_set_left_bit> | <def_find_set_right_bit> | <def_from_bcd> | <def_increment> | <def_index> | <def_l_and> | <def_l_equal> | <def_l_greater> | <def_l_greater_equal> | <def_l_less> | <def_l_less_equal> | <def_mid> | <def_l_not> | <def_l_not_equal> | <def_load_table> | <def_l_or> | <def_match> | <def_mod> | <def_multiply> | <def_nand> | <def_nor> | <def_not> | <def_object_type> | <def_or> | <def_package> | <def_var_package> | <def_ref_of> | <def_shift_left> | <def_shift_right> | <def_size_of> | <def_store> | <def_subtract> | <def_timer> | <def_to_bcd> | <def_to_buffer> | <def_to_decimal_string> | <def_to_hex_string> | <def_to_integer> | <def_to_string> | <def_wait> | <def_xor> | <method_invocation>
AMLSymbol *analyse_aml_expression_opcode(AMLSubstring aml);
// <name_space_modifier_obj> := <def_alias> | <def_name> | <def_scope>
AMLSymbol *analyse_aml_name_space_modifier_obj(AMLSubstring aml);
// <name_string> := <root_char> <name_path> | <prefix_path> <name_path>
AMLSymbol *analyse_aml_name_string(AMLSubstring aml);
// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSubstring aml);
// <statement_opcode> := <def_break> | <def_breakpoint> | <def_continue> | <def_fatal> | <def_if_else> | <def_noop> | <def_notify> | <def_release> | <def_reset> | <def_return> | <def_signal> | <def_sleep> | <def_stall> | <def_while>
AMLSymbol *analyse_aml_statement_opcode(AMLSubstring aml);
// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSubstring aml);
// <term_obj> := <object> | <statement_opcode> | <expression_opcode>
AMLSymbol *analyse_aml_term_obj(AMLSubstring aml);
AMLSymbol *create_dsdt_aml_syntax_tree(void);
void delete_aml_symbol(AMLSymbol *aml_symbol);
MemoryRegionDescriptor get_acpi_memory_region_descriptor(void);
AMLSubstring get_dsdt_aml(void);
ACPITableHeader const *get_dsdt_header(void);
FADT const *get_fadt(void);
unsigned int get_num_of_sdt_headers(void);
ACPITableHeader const *get_rsdt_header(void);
ACPITableHeader const *get_sdt_header(char const *signature);
ACPITableHeader const * const *get_sdt_headers(void);
void print_acpi_table_header(ACPITableHeader acpi_table_header, char const *name);
void print_acpi_table_header_p(ACPITableHeader const *acpi_table_header, char const *name);
void print_aml_symbol(AMLSymbol const *aml_symbol);
void print_generic_address_structure(GenericAddressStructure generic_address_structure, char const *name);

#endif

