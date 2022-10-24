// bibliography
// https://uefi.org/sites/default/files/resources/ACPI_Spec_6_4_Jan22.pdf
// https://wiki.osdev.org/FADT

#ifndef _ACPI_H_
#define _ACPI_H_

#include "chain_string.h"
#include "memory.h"
#include "stdbool.h"

#define PRINT_ACPI_TABLE_HEADER(x) print_acpi_table_header((x), _STRING(x))
#define PRINT_ACPI_TABLE_HEADER_P(x) print_acpi_table_header_p((x), _STRING(x))
#define PRINT_GENERIC_ADDRESS_STRUCTURE(x) print_generic_address_structure((x), _STRING(x))
#define PRINT_RSDP(x) print_rsdp((x), _STRING(x))

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

typedef struct _RSDP
{
	char signature[8];
	unsigned char checksum;
	char oemid[6];
	unsigned char revision;
	ACPITableHeader const *rsdt;
	unsigned int length;
	unsigned long long xsdt_addr;
	unsigned char extended_checksum;
	unsigned char reserved[3];
} __attribute__((packed)) RSDP;

// Structures related to AML

struct _AMLSymbol;

typedef enum _AMLSymbolType
{
	aml_alias_op,
	aml_arg_obj,
	aml_arg_op,
	aml_ascii_char,
	aml_ascii_char_list,
	aml_buff_pkg_str_obj,
	aml_buffer_op,
	aml_buffer_size,
	aml_byte_const,
	aml_byte_data,
	aml_byte_prefix,
	aml_computational_data,
	aml_const_obj,
	aml_data_object,
	aml_data_ref_object,
	aml_def_alias,
	aml_def_buffer,
	aml_def_deref_of,
	aml_def_device,
	aml_def_field,
	aml_def_if_else,
	aml_def_increment,
	aml_def_index,
	aml_def_l_less,
	aml_def_method,
	aml_def_name,
	aml_def_op_region,
	aml_def_scope,
	aml_def_shift_right,
	aml_def_size_of,
	aml_def_store,
	aml_def_subtract,
	aml_def_to_buffer,
	aml_def_to_hex_string,
	aml_def_while,
	aml_deref_of_op,
	aml_device_op,
	aml_device_op_suffix,
	aml_digit_char,
	aml_dual_name_path,
	aml_dual_name_prefix,
	aml_dword_const,
	aml_dword_data,
	aml_dword_prefix,
	aml_expression_opcode,
	aml_ext_op_prefix,
	aml_field_element,
	aml_field_flags,
	aml_field_list,
	aml_field_op,
	aml_field_op_suffix,
	aml_if_op,
	aml_increment_op,
	aml_index_op,
	aml_index_value,
	aml_lead_name_char,
	aml_l_less_op,
	aml_local_obj,
	aml_local_op,
	aml_method_flags,
	aml_method_invocation,
	aml_method_op,
	aml_multi_name_path,
	aml_multi_name_prefix,
	aml_name_char,
	aml_name_op,
	aml_name_path,
	aml_name_seg,
	aml_name_space_modifier_obj,
	aml_name_string,
	aml_named_field,
	aml_named_obj,
	aml_null_char,
	aml_null_name,
	aml_obj_reference,
	aml_object,
	aml_one_op,
	aml_ones_op,
	aml_op_region_op,
	aml_op_region_op_suffix,
	aml_operand,
	aml_parent_prefix_char,
	aml_pkg_lead_byte,
	aml_pkg_length,
	aml_predicate,
	aml_prefix_path,
	aml_qword_const,
	aml_qword_data,
	aml_qword_prefix,
	aml_region_len,
	aml_region_offset,
	aml_region_space,
	aml_revision_op,
	aml_revision_op_suffix,
	aml_root_char,
	aml_scope_op,
	aml_seg_count,
	aml_shift_count,
	aml_shift_right_op,
	aml_simple_name,
	aml_size_of_op,
	aml_statement_opcode,
	aml_store_op,
	aml_string,
	aml_string_prefix,
	aml_subtract_op,
	aml_super_name,
	aml_target,
	aml_term_arg,
	aml_term_arg_list,
	aml_term_list,
	aml_term_obj,
	aml_to_buffer_op,
	aml_to_hex_string_op,
	aml_while_op,
	aml_word_const,
	aml_word_data,
	aml_word_prefix,
	aml_zero_op,
} AMLSymbolType;

typedef struct _AMLSubstring
{
	unsigned char const *initial;
	size_t length;
} AMLSubstring;

typedef struct _AMLArgObj
{
	struct _AMLSymbol *arg_op;
	unsigned char arg_op_number;
} AMLArgObj;

typedef struct _AMLAsciiCharList
{
	struct _AMLSymbol *ascii_char;
	struct _AMLSymbol *ascii_char_list;
} AMLAsciiCharList;

typedef struct _AMLBufferSize
{
	struct _AMLSymbol *term_arg;
} AMLBufferSize;

typedef struct _AMLBuffPkgStrObj
{
	struct _AMLSymbol *term_arg;
} AMLBuffPkgStrObj;

typedef struct _AMLByteConst
{
	struct _AMLSymbol *byte_prefix;
	struct _AMLSymbol *byte_data;
} AMLByteConst;

typedef struct _AMLComputationalData
{
	struct _AMLSymbol *byte_const;
	struct _AMLSymbol *word_const;
	struct _AMLSymbol *dword_const;
	struct _AMLSymbol *qword_const;
	struct _AMLSymbol *string;
	struct _AMLSymbol *const_obj;
	struct _AMLSymbol *revision_op;
	struct _AMLSymbol *def_buffer;
} AMLComputationalData;

typedef struct _AMLConstObj
{
	struct _AMLSymbol *zero_op;
	struct _AMLSymbol *one_op;
	struct _AMLSymbol *ones_op;
} AMLConstObj;

typedef struct _AMLDataObject
{
	struct _AMLSymbol *computational_data;
	struct _AMLSymbol *def_package;
	struct _AMLSymbol *def_var_package;
} AMLDataObject;

typedef struct _AMLDataRefObject
{
	struct _AMLSymbol *data_object;
	struct _AMLSymbol *object_reference;
} AMLDataRefObject;

typedef struct _AMLDefAlias
{
	struct _AMLSymbol *alias_op;
	struct _AMLSymbol *name_string[2];
} AMLDefAlias;

typedef struct _AMLDefBuffer
{
	struct _AMLSymbol *buffer_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *buffer_size;
	struct _AMLSymbol *byte_list;
} AMLDefBuffer;

typedef struct _AMLDefDerefOf
{
	struct _AMLSymbol *deref_of_op;
	struct _AMLSymbol *obj_reference;
} AMLDefDerefOf;

typedef struct _AMLDefDevice
{
	struct _AMLSymbol *device_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *term_list;
} AMLDefDevice;

typedef struct _AMLDefField
{
	struct _AMLSymbol *field_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *field_flags;
	struct _AMLSymbol *field_list;
} AMLDefField;

typedef struct _AMLDefIfElse
{
	struct _AMLSymbol *if_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *predicate;
	struct _AMLSymbol *term_list;
	struct _AMLSymbol *def_else;
} AMLDefIfElse;

typedef struct _AMLDefIncrement
{
	struct _AMLSymbol *increment_op;
	struct _AMLSymbol *super_name;
} AMLDefIncrement;

typedef struct _AMLDefIndex
{
	struct _AMLSymbol *index_op;
	struct _AMLSymbol *buff_pkg_str_obj;
	struct _AMLSymbol *index_value;
	struct _AMLSymbol *target;
} AMLDefIndex;

typedef struct _AMLDefLLess
{
	struct _AMLSymbol *l_less_op;
	struct _AMLSymbol *operand[2];
} AMLDefLLess;

typedef struct _AMLDefMethod
{
	struct _AMLSymbol *method_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *method_flags;
	struct _AMLSymbol *term_list;
} AMLDefMethod;

typedef struct _AMLDefName
{
	struct _AMLSymbol *name_op;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *data_ref_object;
} AMLDefName;

typedef struct _AMLDefOpRegion
{
	struct _AMLSymbol *op_region_op;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *region_space;
	struct _AMLSymbol *region_offset;
	struct _AMLSymbol *region_len;
} AMLDefOpRegion;

typedef struct _AMLDefScope
{
	struct _AMLSymbol *scope_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *term_list;
} AMLDefScope;

typedef struct _AMLDefShiftRight
{
	struct _AMLSymbol *shift_right_op;
	struct _AMLSymbol *operand;
	struct _AMLSymbol *shift_count;
	struct _AMLSymbol *target;
} AMLDefShiftRight;

typedef struct _AMLDefSizeOf
{
	struct _AMLSymbol *size_of_op;
	struct _AMLSymbol *super_name;
} AMLDefSizeOf;

typedef struct _AMLDefStore
{
	struct _AMLSymbol *store_op;
	struct _AMLSymbol *term_arg;
	struct _AMLSymbol *super_name;
} AMLDefStore;

typedef struct _AMLDefSubtract
{
	struct _AMLSymbol *subtract_op;
	struct _AMLSymbol *operand[2];
	struct _AMLSymbol *target;
} AMLDefSubtract;

typedef struct _AMLDefToBuffer
{
	struct _AMLSymbol *to_buffer_op;
	struct _AMLSymbol *operand;
	struct _AMLSymbol *target;
} AMLDefToBuffer;

typedef struct _AMLDefToHexString
{
	struct _AMLSymbol *to_hex_string_op;
	struct _AMLSymbol *operand;
	struct _AMLSymbol *target;
} AMLDefToHexString;

typedef struct _AMLDefWhile
{
	struct _AMLSymbol *while_op;
	struct _AMLSymbol *pkg_length;
	struct _AMLSymbol *predicate;
	struct _AMLSymbol *term_list;
} AMLDefWhile;

typedef struct _AMLDeviceOp
{
	struct _AMLSymbol *ext_op_prefix;
	struct _AMLSymbol *device_op_suffix;
} AMLDeviceOp;

typedef struct _AMLDualNamePath
{
	struct _AMLSymbol *dual_name_prefix;
	struct _AMLSymbol *name_seg[2];
} AMLDualNamePath;

typedef struct _AMLDWordConst
{
	struct _AMLSymbol *dword_prefix;
	struct _AMLSymbol *dword_data;
} AMLDWordConst;

typedef struct _AMLDWordData
{
	struct _AMLSymbol *word_data[2];
} AMLDWordData;

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

typedef struct _AMLFieldElement
{
	struct _AMLSymbol *named_field;
	struct _AMLSymbol *reserved_field;
	struct _AMLSymbol *access_field;
	struct _AMLSymbol *extended_access_field;
	struct _AMLSymbol *connect_field;
} AMLFieldElement;

typedef struct _AMLFieldList
{
	struct _AMLSymbol *field_element;
	struct _AMLSymbol *field_list;
} AMLFieldList;

typedef struct _AMLFieldOp
{
	struct _AMLSymbol *ext_op_prefix;
	struct _AMLSymbol *field_op_suffix;
} AMLFieldOp;

typedef struct _AMLIndexValue
{
	struct _AMLSymbol *term_arg;
} AMLIndexValue;

typedef struct _AMLLocalObj
{
	struct _AMLSymbol *local_op;
	unsigned char local_op_number;
} AMLLocalObj;

typedef struct _AMLMethodFlags
{
	unsigned char arg_count;
	bool serialize_flag;
	unsigned char sync_level;
} AMLMethodFlags;

typedef struct _AMLMethodInvocation
{
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *term_arg_list;
} AMLMethodInvocation;

typedef struct _AMLMultiNamePath
{
	struct _AMLSymbol *multi_name_prefix;
	struct _AMLSymbol *seg_count;
	struct _AMLSymbol **name_seg;
} AMLMultiNamePath;

typedef struct _AMLNameChar
{
	struct _AMLSymbol *digit_char;
	struct _AMLSymbol *lead_name_char;
} AMLNameChar;

typedef struct _AMLNamedField
{
	struct _AMLSymbol *name_seg;
	struct _AMLSymbol *pkg_length;
} AMLNamedField;

typedef struct _AMLNamedObj
{
	struct _AMLSymbol *def_bank_field;
	struct _AMLSymbol *def_create_bit_field;
	struct _AMLSymbol *def_create_byte_field;
	struct _AMLSymbol *def_create_dword_field;
	struct _AMLSymbol *def_create_field;
	struct _AMLSymbol *def_create_qword_field;
	struct _AMLSymbol *def_create_word_field;
	struct _AMLSymbol *def_data_region;
	struct _AMLSymbol *def_device;
	struct _AMLSymbol *def_external;
	struct _AMLSymbol *def_field;
	struct _AMLSymbol *def_method;
	struct _AMLSymbol *def_op_region;
	struct _AMLSymbol *def_power_res;
	struct _AMLSymbol *def_thermal_zone;
} AMLNamedObj;

typedef struct _AMLNamePath
{
	struct _AMLSymbol *name_seg;
	struct _AMLSymbol *dual_name_path;
	struct _AMLSymbol *multi_name_path;
	struct _AMLSymbol *null_name;
} AMLNamePath;

typedef struct _AMLNameSeg
{
	struct _AMLSymbol *lead_name_char;
	struct _AMLSymbol *name_char[3];
} AMLNameSeg;

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

typedef struct _AMLObjReference
{
	struct _AMLSymbol *term_arg;
} AMLObjReference;

typedef struct _AMLOperand
{
	struct _AMLSymbol *term_arg;
} AMLOperand;

typedef struct _AMLOpRegionOp
{
	struct _AMLSymbol *ext_op_prefix;
	struct _AMLSymbol *op_region_op_suffix;
} AMLOpRegionOp;

typedef struct _AMLPkgLength
{
	struct _AMLSymbol *pkg_lead_byte;
	struct _AMLSymbol *byte_data[3];
	unsigned char num_of_byte_data;
	unsigned int length;
} AMLPkgLength;

typedef struct _AMLPredicate
{
	struct _AMLSymbol *term_arg;
} AMLPredicate;

typedef struct _AMLPrefixPath
{
	struct _AMLSymbol *parent_prefix_char;
	struct _AMLSymbol *prefix_path;
} AMLPrefixPath;

typedef struct _AMLQWordConst
{
	struct _AMLSymbol *qword_prefix;
	struct _AMLSymbol *qword_data;
} AMLQWordConst;

typedef struct _AMLQWordData
{
	struct _AMLSymbol *dword_data[2];
} AMLQWordData;

typedef struct _AMLRegionLen
{
	struct _AMLSymbol *term_arg;
} AMLRegionLen;

typedef struct _AMLRegionOffset
{
	struct _AMLSymbol *term_arg;
} AMLRegionOffset;

typedef struct _AMLRevisionOp
{
	struct _AMLSymbol *ext_op_prefix;
	struct _AMLSymbol *revision_op_suffix;
} AMLRevisionOp;

typedef struct _AMLShiftCount
{
	struct _AMLSymbol *term_arg;
} AMLShiftCount;

typedef struct _AMLSimpleName
{
	struct _AMLSymbol *name_string;
	struct _AMLSymbol *arg_obj;
	struct _AMLSymbol *local_obj;
} AMLSimpleName;

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

typedef struct _AMLString
{
	struct _AMLSymbol *string_prefix;
	struct _AMLSymbol *ascii_char_list;
	struct _AMLSymbol *null_char;
} AMLString;

typedef struct _AMLSuperName
{
	struct _AMLSymbol *debug_obj;
	struct _AMLSymbol *reference_type_opcode;
	struct _AMLSymbol *simple_name;
} AMLSuperName;

typedef struct _AMLTarget
{
	struct _AMLSymbol *super_name;
	struct _AMLSymbol *null_name;
} AMLTarget;

typedef struct _AMLTermArg
{
	struct _AMLSymbol *expression_opcode;
	struct _AMLSymbol *data_object;
	struct _AMLSymbol *arg_obj;
	struct _AMLSymbol *local_obj;
} AMLTermArg;

typedef struct _AMLTermArgList
{
	struct _AMLSymbol *term_arg;
	struct _AMLSymbol *term_arg_list;
} AMLTermArgList;

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

typedef struct _AMLWordConst
{
	struct _AMLSymbol *word_prefix;
	struct _AMLSymbol *word_data;
} AMLWordConst;

typedef struct _AMLWordData
{
	struct _AMLSymbol *byte_data[2];
} AMLWordData;

typedef union _AMLComponent
{
	AMLArgObj arg_obj;
	AMLAsciiCharList ascii_char_list;
	AMLBufferSize buffer_size;
	AMLBuffPkgStrObj buff_pkg_str_obj;
	AMLByteConst byte_const;
	AMLComputationalData computational_data;
	AMLConstObj const_obj;
	AMLDataObject data_object;
	AMLDataRefObject data_ref_object;
	AMLDefAlias def_alias;
	AMLDefBuffer def_buffer;
	AMLDefDerefOf def_deref_of;
	AMLDefDevice def_device;
	AMLDefField def_field;
	AMLDefIfElse def_if_else;
	AMLDefIncrement def_increment;
	AMLDefIndex def_index;
	AMLDefLLess def_l_less;
	AMLDefMethod def_method;
	AMLDefName def_name;
	AMLDefOpRegion def_op_region;
	AMLDefScope def_scope;
	AMLDefShiftRight def_shift_right;
	AMLDefSizeOf def_size_of;
	AMLDefStore def_store;
	AMLDefSubtract def_subtract;
	AMLDefToBuffer def_to_buffer;
	AMLDefToHexString def_to_hex_string;
	AMLDefWhile def_while;
	AMLDeviceOp device_op;
	AMLDualNamePath dual_name_path;
	AMLDWordConst dword_const;
	AMLDWordData dword_data;
	AMLExpressionOpcode expression_opcode;
	AMLFieldElement field_element;
	AMLFieldList field_list;
	AMLFieldOp field_op;
	AMLIndexValue index_value;
	AMLLocalObj local_obj;
	AMLMethodFlags method_flags;
	AMLMethodInvocation method_invocation;
	AMLMultiNamePath multi_name_path;
	AMLNameChar name_char;
	AMLNamedField named_field;
	AMLNamedObj named_obj;
	AMLNamePath name_path;
	AMLNameSeg name_seg;
	AMLNameSpaceModifierObj name_space_modifier_obj;
	AMLNameString name_string;
	AMLObject object;
	AMLObjReference obj_reference;
	AMLOperand operand;
	AMLOpRegionOp op_region_op;
	AMLPkgLength pkg_length;
	AMLPredicate predicate;
	AMLPrefixPath prefix_path;
	AMLQWordConst qword_const;
	AMLQWordData qword_data;
	AMLRegionLen region_len;
	AMLRegionOffset region_offset;
	AMLRevisionOp revision_op;
	AMLShiftCount shift_count;
	AMLSimpleName simple_name;
	AMLStatementOpcode statement_opcode;
	AMLString string;
	AMLSuperName super_name;
	AMLTarget target;
	AMLTermArg term_arg;
	AMLTermArgList term_arg_list;
	AMLTermList term_list;
	AMLTermObj term_obj;
	AMLWordConst word_const;
	AMLWordData word_data;
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
// <alias_op> := AML_BYTE_ALIAS_OP
AMLSymbol *analyse_aml_alias_op(AMLSubstring aml);
// <arg_obj> := <arg_op>
AMLSymbol *analyse_aml_arg_obj(AMLSubstring aml);
// <arg_op> := 0x68 - 0x6e
AMLSymbol *analyse_aml_arg_op(AMLSubstring aml);
// <ascii_char> := 0x01 - 0x7f
AMLSymbol *analyse_aml_ascii_char(AMLSubstring aml);
// <ascii_char_list> := Nothing | <ascii_char> <ascii_char_list>
AMLSymbol *analyse_aml_ascii_char_list(AMLSubstring aml);
// <buff_pkg_str_obj> := <term_arg>
AMLSymbol *analyse_aml_buff_pkg_str_obj(AMLSubstring aml);
// <buffer_op> := AML_BYTE_BUFFER_OP
AMLSymbol *analyse_aml_buffer_op(AMLSubstring aml);
// <buffer_size> := <term_arg>
AMLSymbol *analyse_aml_buffer_size(AMLSubstring aml);
// <byte_const> := <byte_prefix> <byte_data>
AMLSymbol *analyse_aml_byte_const(AMLSubstring aml);
// <byte_data> := 0x00 - 0xff
AMLSymbol *analyse_aml_byte_data(AMLSubstring aml);
// <byte_prefix> := AML_BYTE_BYTE_PREFIX
AMLSymbol *analyse_aml_byte_prefix(AMLSubstring aml);
// <computational_data> := <byte_const> | <word_const> | <dword_const> | <qword_const> | <string> | <const_obj> | <revision_op> | <def_buffer>
AMLSymbol *analyse_aml_computatinoal_data(AMLSubstring aml);
// <const_obj> := <zero_op> | <one_op> | <ones_op>
AMLSymbol *analyse_aml_const_obj(AMLSubstring aml);
// <data_object> := <computational_data> | <def_package> | <def_var_package>
AMLSymbol *analyse_aml_data_object(AMLSubstring aml);
// <data_ref_object> := <data_object> | <object_reference>
AMLSymbol *analyse_aml_data_ref_object(AMLSubstring aml);
// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSubstring aml);
// <def_buffer> := <buffer_op> <pkg_length> <buffer_size> <byte_list>
AMLSymbol *analyse_aml_def_buffer(AMLSubstring aml);
// <def_deref_of> := <deref_of_op> <obj_reference>
AMLSymbol *analyse_aml_def_deref_of(AMLSubstring aml);
// <def_device> := <device_op> <pkg_length> <name_string> <term_list>
AMLSymbol *analyse_aml_def_device(AMLSubstring aml);
// <def_field> := <field_op> <pkg_length> <name_string> <field_flags> <field_list>
AMLSymbol *analyse_aml_def_field(AMLSubstring aml);
// <def_if_else> := <if_op> <pkg_length> <predicate> <term_list> <def_else>
AMLSymbol *analyse_aml_def_if_else(AMLSubstring aml);
// <def_increment> := <increment_op> <super_name>
AMLSymbol *analyse_aml_def_increment(AMLSubstring aml);
// <def_index> := <index_op> <buff_pkf_str_obj> <index_value> <target>
AMLSymbol *analyse_aml_def_index(AMLSubstring aml);
// <def_l_less> := <l_less_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_less(AMLSubstring aml);
// <def_method> := <method_op> <pkg_length> <name_string> <method_flags> <term_list>
AMLSymbol *analyse_aml_def_method(AMLSubstring aml);
// <def_name> := <name_op> <name_string> <data_ref_object>
AMLSymbol *analyse_aml_def_name(AMLSubstring aml);
// <def_op_region> := <op_region_op> <name_string> <region_space> <region_offset> <region_len>
AMLSymbol *analyse_aml_def_op_region(AMLSubstring aml);
// <def_scope> := <scope_op> <pkg_length> <name_string> <term_list>
AMLSymbol *analyse_aml_def_scope(AMLSubstring aml);
// <def_shift_right> := <shift_right_op> <operand> <shift_count> <target>
AMLSymbol *analyse_aml_def_shift_right(AMLSubstring aml);
// <def_size_of> := <size_of_op> <super_name>
AMLSymbol *analyse_aml_def_size_of(AMLSubstring aml);
// <def_store> := <store_op> <term_arg> <super_name>
AMLSymbol *analyse_aml_def_store(AMLSubstring aml);
// <def_subtract> := <subtract_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_subtract(AMLSubstring aml);
// <def_to_buffer> := <to_buffer_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_buffer(AMLSubstring aml);
// <def_to_hex_string> := <to_hex_string_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_hex_string(AMLSubstring aml);
// <def_while> := <while_op> <pkg_length> <predicate> <term_list>
AMLSymbol *analyse_aml_def_while(AMLSubstring aml);
// <deref_of_op> := AML_BYTE_DEREF_OF_OP
AMLSymbol *analyse_aml_deref_of_op(AMLSubstring aml);
// <device_op> := <ext_op_prefix> <device_op_suffix>
AMLSymbol *analyse_aml_device_op(AMLSubstring aml);
// <device_op_suffix> := AML_BYTE_DEVICE_OP
AMLSymbol *analyse_aml_device_op_suffix(AMLSubstring aml);
// <digit_char> := '0' - '9'
AMLSymbol *analyse_aml_digit_char(AMLSubstring aml);
// <dual_name_path> := <dual_name_prefix> <name_seg> <name_seg>
AMLSymbol *analyse_aml_dual_name_path(AMLSubstring aml);
// <dual_name_prefix> := AML_BYTE_DUAL_NAME_PREFIX
AMLSymbol *analyse_aml_dual_name_prefix(AMLSubstring aml);
// <dword_const> := <dword_prefix> <dword_data>
AMLSymbol *analyse_aml_dword_const(AMLSubstring aml);
// <dword_data> := <word_data> <word_data>
AMLSymbol *analyse_aml_dword_data(AMLSubstring aml);
// <dword_prefix> := AML_BYTE_DWORD_PREFIX
AMLSymbol *analyse_aml_dword_prefix(AMLSubstring aml);
// <expression_opcode> := <def_aquire> | <def_add> | <def_and> | <def_buffer> | <def_concat> | <def_concat_res> | <def_cond_ref_of> | <def_copy_object> | <def_decrement> | <def_deref_of> | <def_divide> | <def_find_set_left_bit> | <def_find_set_right_bit> | <def_from_bcd> | <def_increment> | <def_index> | <def_l_and> | <def_l_equal> | <def_l_greater> | <def_l_greater_equal> | <def_l_less> | <def_l_less_equal> | <def_mid> | <def_l_not> | <def_l_not_equal> | <def_load_table> | <def_l_or> | <def_match> | <def_mod> | <def_multiply> | <def_nand> | <def_nor> | <def_not> | <def_object_type> | <def_or> | <def_package> | <def_var_package> | <def_ref_of> | <def_shift_left> | <def_shift_right> | <def_size_of> | <def_store> | <def_subtract> | <def_timer> | <def_to_bcd> | <def_to_buffer> | <def_to_decimal_string> | <def_to_hex_string> | <def_to_integer> | <def_to_string> | <def_wait> | <def_xor> | <method_invocation>
AMLSymbol *analyse_aml_expression_opcode(AMLSubstring aml);
// <ext_op_prefix> := AML_BYTE_EXT_OP_PREFIX
AMLSymbol *analyse_aml_ext_op_prefix(AMLSubstring aml);
// <field_element> := <named_field> | <reserved_field> | <access_field> | <extended_access_field> | <connect_field>
AMLSymbol *analyse_aml_field_element(AMLSubstring aml);
// <field_flags>
AMLSymbol *analyse_aml_field_flags(AMLSubstring aml);
// <field_list> := Nothing | <field_element> <field_list>
AMLSymbol *analyse_aml_field_list(AMLSubstring aml);
// <field_op> := <ext_op_prefix> <field_op_suffix>
AMLSymbol *analyse_aml_field_op(AMLSubstring aml);
// <field_op_suffix> := AML_BYTE_FIELD_OP_PREFIX
AMLSymbol *analyse_aml_field_op_suffix(AMLSubstring aml);
// <if_op> := AML_BYTE_IF_OP
AMLSymbol *analyse_aml_if_op(AMLSubstring aml);
// <increment_op> := AML_BYTE_INCREMENT_OP
AMLSymbol *analyse_aml_increment_op(AMLSubstring aml);
// <index_op> := AML_BYTE_INDEX_OP
AMLSymbol *analyse_aml_index_op(AMLSubstring aml);
// <index_value> := <term_arg>
AMLSymbol *analyse_aml_index_value(AMLSubstring aml);
// <lead_char> := 'A' - 'Z' | '_'
AMLSymbol *analyse_aml_lead_name_char(AMLSubstring aml);
// <l_less_op> := AML_BYTE_L_LESS_OP
AMLSymbol *analyse_aml_l_less_op(AMLSubstring aml);
// <local_obj> := <local_op>
AMLSymbol *analyse_aml_local_obj(AMLSubstring aml);
// <local_op> := 0x60 - 0x67
AMLSymbol *analyse_aml_local_op(AMLSubstring aml);
// <method_flags>
AMLSymbol *analyse_aml_method_flags(AMLSubstring aml);
// <method_invocation> := <name_string> | <term_arg_list>
AMLSymbol *analyse_aml_method_invocation(AMLSubstring aml);
// <method_op> := 0x14
AMLSymbol *analyse_aml_method_op(AMLSubstring aml);
// <multi_name_path> := <multi_name_prefix> <seg_count> <name_seg>*
AMLSymbol *analyse_aml_multi_name_path(AMLSubstring aml);
// <mult_name_prefix> := AML_BYTE_MULTI_NAME_PREFIX
AMLSymbol *analyse_aml_multi_name_prefix(AMLSubstring aml);
// <name_char> := <digit_char> | <lead_name_char>
AMLSymbol *analyse_aml_name_char(AMLSubstring aml);
// <name_op> := AML_BYTE_NAME_OP
AMLSymbol *analyse_aml_name_op(AMLSubstring aml);
// <name_path> := <name_seg> | <dual_name_path> | <multi_name_path> | <null_name>
AMLSymbol *analyse_aml_name_path(AMLSubstring aml);
// <name_seg> := <lead_name_char> <name_char> <name_char> <name_char>
AMLSymbol *analyse_aml_name_seg(AMLSubstring aml);
// <name_space_modifier_obj> := <def_alias> | <def_name> | <def_scope>
AMLSymbol *analyse_aml_name_space_modifier_obj(AMLSubstring aml);
// <name_string> := <root_char> <name_path> | <prefix_path> <name_path>
AMLSymbol *analyse_aml_name_string(AMLSubstring aml);
// <named_field> := <name_seg> <pkg_length>
AMLSymbol *analyse_aml_named_field(AMLSubstring aml);
// <named_obj> := <def_bank_field> | <def_create_bit_field> | <def_create_byte_field> | <def_create_dword_field> | <def_create_field> | <def_create_qword_field> | <def_create_word_field> | <def_data_region> | <def_external> | <def_op_region> | <def_power_res> | <def_thermal_zone>
AMLSymbol *analyse_aml_named_obj(AMLSubstring aml);
// <null_char> := AML_BYTE_NULL_CHAR
AMLSymbol *analyse_aml_null_char(AMLSubstring aml);
// <null_name> := AML_BYTE_NULL_NAME
AMLSymbol *analyse_aml_null_name(AMLSubstring aml);
// <obj_reference> := <term_arg>
AMLSymbol *analyse_aml_obj_reference(AMLSubstring aml);
// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSubstring aml);
// <one_op> := AML_BYTE_ONE_OP
AMLSymbol *analyse_aml_one_op(AMLSubstring aml);
// <ones_op> := AML_BYTE_ONES_OP
AMLSymbol *analyse_aml_ones_op(AMLSubstring aml);
// <op_region_op> := <ext_op_prefix> <op_region_op_suffix>
AMLSymbol *analyse_aml_op_region_op(AMLSubstring aml);
// <op_region_op_suffix> := AML_BYTE_OP_REGION_OP
AMLSymbol *analyse_aml_op_region_op_suffix(AMLSubstring aml);
// <operand> := <term_arg>
AMLSymbol *analyse_aml_operand(AMLSubstring aml);
// <parent_prefix_char> := AML_BYTE_PARENT_PREFIX_CHAR
AMLSymbol *analyse_aml_parent_prefix_char(AMLSubstring aml);
// <pkg_lead_byte>
AMLSymbol *analyse_aml_pkg_lead_byte(AMLSubstring aml);
// <pkg_length> := <pkg_lead_byte> | <pkg_lead_byte> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> <byte_data>
AMLSymbol *analyse_aml_pkg_length(AMLSubstring aml);
// <predicate> := <term_arg>
AMLSymbol *analyse_aml_predicate(AMLSubstring aml);
// <prefix_path> := Nothing | <parent_prefix_char> <prefix_path>
AMLSymbol *analyse_aml_prefix_path(AMLSubstring aml);
// <qword_const> := <qword_prefix> <qword_data>
AMLSymbol *analyse_aml_qword_const(AMLSubstring aml);
// <qword_data> := <dword_data> <dword_data>
AMLSymbol *analyse_aml_qword_data(AMLSubstring aml);
// <qword_prefix> := AML_BYTE_QWORD_PREFIX
AMLSymbol *analyse_aml_qword_prefix(AMLSubstring aml);
// <region_len> := <term_arg>
AMLSymbol *analyse_aml_region_len(AMLSubstring aml);
// <region_offset> := <term_arg>
AMLSymbol *analyse_aml_region_offset(AMLSubstring aml);
// <region_space>
AMLSymbol *analyse_aml_region_space(AMLSubstring aml);
// <revision_op> := <ext_op_prefix> <revision_op_suffix>
AMLSymbol *analyse_aml_revision_op(AMLSubstring aml);
// <revision_op_suffix> := AML_BYTE_REVISION_OP
AMLSymbol *analyse_aml_revision_op_suffix(AMLSubstring aml);
// <root_char> := AML_BYTE_ROOT_CHAR
AMLSymbol *analyse_aml_root_char(AMLSubstring aml);
// <scope_op> := AML_TYPE_SCOPE_OP
AMLSymbol *analyse_aml_scope_op(AMLSubstring aml);
// <seg_count> := 0x01 - 0xff
AMLSymbol *analyse_aml_seg_count(AMLSubstring aml);
// <shift_count> := <term_arg>
AMLSymbol *analyse_aml_shift_count(AMLSubstring aml);
// <shift_right_op> := AML_BYTE_SHIFT_RIGHT
AMLSymbol *analyse_aml_shift_right_op(AMLSubstring aml);
// <simple_name> := <name_string> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_simple_name(AMLSubstring aml);
// <size_of_op> := AML_BYTE_SIZE_OF_OP
AMLSymbol *analyse_aml_size_of_op(AMLSubstring aml);
// <statement_opcode> := <def_break> | <def_breakpoint> | <def_continue> | <def_fatal> | <def_if_else> | <def_noop> | <def_notify> | <def_release> | <def_reset> | <def_return> | <def_signal> | <def_sleep> | <def_stall> | <def_while>
AMLSymbol *analyse_aml_statement_opcode(AMLSubstring aml);
// <store_op> := AML_BYTE_STORE_OP
AMLSymbol *analyse_aml_store_op(AMLSubstring aml);
// <string> := <string_prefix> <ascii_char_list> <null_char>
AMLSymbol *analyse_aml_string(AMLSubstring aml);
// <string_prefix> := AML_BYTE_STRING_PREFIX
AMLSymbol *analyse_aml_string_prefix(AMLSubstring aml);
// <subtract_op> := AML_BYTE_SUBTRACT_OP
AMLSymbol *analyse_aml_subtract_op(AMLSubstring aml);
// <super_name> := <simple_name> | <debug_obj> | <reference_type_opcode>
AMLSymbol *analyse_aml_super_name(AMLSubstring aml);
// <target> := <super_name> | <null_name>
AMLSymbol *analyse_aml_target(AMLSubstring aml);
// <term_arg> := <expression_op_code> | <data_object> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_term_arg(AMLSubstring aml);
// <term_arg_list> := Nothing | <term_arg> <term_arg_list>
AMLSymbol *analyse_aml_term_arg_list(AMLSubstring aml);
// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSubstring aml);
// <term_obj> := <object> | <statement_opcode> | <expression_opcode>
AMLSymbol *analyse_aml_term_obj(AMLSubstring aml);
// <to_buffer_op> := AML_BYTE_TO_BUFFER_OP
AMLSymbol *analyse_aml_to_buffer_op(AMLSubstring aml);
// <to_hex_string_op> := AML_BYTE_TO_HEX_STRING_OP
AMLSymbol *analyse_aml_to_hex_string_op(AMLSubstring aml);
// <while_op> := AML_BYTE_WHILE_OP
AMLSymbol *analyse_aml_while_op(AMLSubstring aml);
// <word_const> := <word_prefix> <word_data>
AMLSymbol *analyse_aml_word_const(AMLSubstring aml);
// <word_data> := <byte_data> <byte_data>
AMLSymbol *analyse_aml_word_data(AMLSubstring aml);
// <word_prefix> := AML_BYTE_WORD_PREFIX
AMLSymbol *analyse_aml_word_prefix(AMLSubstring aml);
// <zero_op> := AML_BYTE_ZERO_OP
AMLSymbol *analyse_aml_zero_op(AMLSubstring aml);
AMLSymbol *create_dsdt_aml_syntax_tree(void);
void delete_aml_symbol(AMLSymbol *aml_symbol);
MemoryRegionDescriptor get_acpi_memory_region_descriptor(void);
AMLSubstring get_dsdt_aml(void);
ACPITableHeader const *get_dsdt_header(void);
FADT const *get_fadt(void);
unsigned int get_num_of_sdt_headers(void);
RSDP const *get_rsdp(void);
ACPITableHeader const *get_rsdt_header(void);
ACPITableHeader const *get_sdt_header(char const *signature);
ACPITableHeader const * const *get_sdt_headers(void);
void print_acpi_table_header(ACPITableHeader acpi_table_header, char const *name);
void print_acpi_table_header_p(ACPITableHeader const *acpi_table_header, char const *name);
void print_aml_symbol(AMLSymbol const *aml_symbol);
void print_generic_address_structure(GenericAddressStructure generic_address_structure, char const *name);
void print_rsdp(RSDP const *rsdp, char const *name);

#endif

