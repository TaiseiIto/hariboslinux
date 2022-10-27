#include "acpi.h"
#include "bios.h"
#include "serial.h"
#include "shell.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#define AML_BYTE_NULL_CHAR		0x00
#define AML_BYTE_NULL_NAME		0x00
#define AML_BYTE_ZERO_OP		0x00
#define AML_BYTE_ONE_OP			0x01
#define AML_BYTE_ALIAS_OP		0x06
#define AML_BYTE_NAME_OP		0x08
#define AML_BYTE_BYTE_PREFIX		0x0a
#define AML_BYTE_WORD_PREFIX		0x0b
#define AML_BYTE_DWORD_PREFIX		0x0c
#define AML_BYTE_STRING_PREFIX		0x0d
#define AML_BYTE_QWORD_PREFIX		0x0e
#define AML_BYTE_SCOPE_OP		0x10
#define AML_BYTE_BUFFER_OP		0x11
#define AML_BYTE_PACKAGE_OP		0x12
#define AML_BYTE_VAR_PACKAGE_OP		0x13
#define AML_BYTE_METHOD_OP		0x14
#define AML_BYTE_EXTERNAL_OP		0x15
#define AML_BYTE_DUAL_NAME_PREFIX	0x2e
#define AML_BYTE_MULTI_NAME_PREFIX	0x2f
#define AML_BYTE_EXT_OP_PREFIX		0x5b
#define AML_BYTE_MUTEX_OP		0x01
#define AML_BYTE_EVENT_OP		0x02
#define AML_BYTE_COND_REF_OF_OP		0x12
#define AML_BYTE_CREATE_FIELD_OP	0x13
#define AML_BYTE_LOAD_TABLE_OP		0x1f
#define AML_BYTE_LOAD_OP		0x20
#define AML_BYTE_STALL_OP		0x21
#define AML_BYTE_SLEEP_OP		0x22
#define AML_BYTE_ACQUIRE_OP		0x23
#define AML_BYTE_SIGNAL_OP		0x24
#define AML_BYTE_WAIT_OP		0x25
#define AML_BYTE_RESET_OP		0x26
#define AML_BYTE_RELEASE_OP		0x27
#define AML_BYTE_FROM_BCD_OP		0x28
#define AML_BYTE_TO_BCD			0x29
#define AML_BYTE_REVISION_OP		0x30
#define AML_BYTE_DEBUG_OP		0x31
#define AML_BYTE_FATAL_OP		0x32
#define AML_BYTE_TIMER_OP		0x33
#define AML_BYTE_OP_REGION_OP		0x80
#define AML_BYTE_FIELD_OP		0x81
#define AML_BYTE_DEVICE_OP		0x82
#define AML_BYTE_POWER_RES_OP		0x84
#define AML_BYTE_THERMAL_ZONE_OP	0x85
#define AML_BYTE_INDEX_FIELD_OP		0x86
#define AML_BYTE_BANK_FIELD_OP		0x87
#define AML_BYTE_DATA_REGION_OP		0x88
#define AML_BYTE_ROOT_CHAR		0x5c
#define AML_BYTE_PARENT_PREFIX_CHAR	0x5e
#define AML_BYTE_NAME_CHAR		0x5f
#define AML_BYTE_LOCAL_0_OP		0x60
#define AML_BYTE_LOCAL_1_OP		0x61
#define AML_BYTE_LOCAL_2_OP		0x62
#define AML_BYTE_LOCAL_3_OP		0x63
#define AML_BYTE_LOCAL_4_OP		0x64
#define AML_BYTE_LOCAL_5_OP		0x65
#define AML_BYTE_LOCAL_6_OP		0x66
#define AML_BYTE_LOCAL_7_OP		0x67
#define AML_BYTE_ARG_0_OP		0x68
#define AML_BYTE_ARG_1_OP		0x69
#define AML_BYTE_ARG_2_OP		0x6a
#define AML_BYTE_ARG_3_OP		0x6b
#define AML_BYTE_ARG_4_OP		0x6c
#define AML_BYTE_ARG_5_OP		0x6d
#define AML_BYTE_ARG_6_OP		0x6e
#define AML_BYTE_STORE_OP		0x70
#define AML_BYTE_REF_OF_OP		0x71
#define AML_BYTE_ADD_OP			0x72
#define AML_BYTE_CONCAT_OP		0x73
#define AML_BYTE_SUBTRACT_OP		0x74
#define AML_BYTE_INCREMENT_OP		0x75
#define AML_BYTE_DECREMENT_OP		0x76
#define AML_BYTE_MULTIPLY_OP		0x77
#define AML_BYTE_DIVIDE_OP		0x78
#define AML_BYTE_SHIFT_LEFT_OP		0x79
#define AML_BYTE_SHIFT_RIGHT_OP		0x7a
#define AML_BYTE_AND_OP			0x7b
#define AML_BYTE_NAND_OP		0x7c
#define AML_BYTE_OR_OP			0x7d
#define AML_BYTE_NOR_OP			0x7e
#define AML_BYTE_XOR_OP			0x7f
#define AML_BYTE_NOT_OP			0x80
#define AML_BYTE_FIND_SET_LEFT_BIT_OP	0x81
#define AML_BYTE_FIND_SET_RIGHT_BIT_OP	0x82
#define AML_BYTE_DEREF_OF_OP		0x83
#define AML_BYTE_CONCAT_RES_OP		0x84
#define AML_BYTE_MOD_OP			0x85
#define AML_BYTE_NOTIFY_OP		0x86
#define AML_BYTE_SIZE_OF_OP		0x87
#define AML_BYTE_INDEX_OP		0x88
#define AML_BYTE_MATCH_OP		0x89
#define AML_BYTE_CREATE_DWORD_FIELD_OP	0x8a
#define AML_BYTE_CREATE_WORD_FIELD_OP	0x8b
#define AML_BYTE_CREATE_BYTE_FIELD_OP	0x8c
#define AML_BYTE_CREATE_BIT_FIELD_OP	0x8d
#define AML_BYTE_OBJECT_TYPE_OP		0x8e
#define AML_BYTE_CREATE_QWORD_FIELD_OP	0x8f
#define AML_BYTE_L_AND_OP		0x90
#define AML_BYTE_L_OR_OP		0x91
#define AML_BYTE_L_NOT_OP		0x92
#define AML_BYTE_L_NOT_EQUAL_OP		0x93
#define AML_BYTE_L_LESS_EQUAL_OP	0x94
#define AML_BYTE_L_GREATER_EQUAL_OP	0x95
#define AML_BYTE_L_EQUAL_OP		0x93
#define AML_BYTE_L_GREATER_OP		0x94
#define AML_BYTE_L_LESS_OP		0x95
#define AML_BYTE_TO_BUFFER_OP		0x96
#define AML_BYTE_TO_DECIMAL_STRING_OP	0x97
#define AML_BYTE_TO_HEX_STRING_OP	0x98
#define AML_BYTE_TO_INTEGER_OP		0x99
#define AML_BYTE_TO_STRING_OP		0x9c
#define AML_BYTE_COPY_OBJECT_OP		0x9d
#define AML_BYTE_MID_OP			0x9e
#define AML_BYTE_COMTINUE_OP		0x9f
#define AML_BYTE_IF_OP			0xa0
#define AML_BYTE_ELSE_OP		0xa1
#define AML_BYTE_WHILE_OP		0xa2
#define AML_BYTE_NOOP_OP		0xa3
#define AML_BYTE_RETURN_OP		0xa4
#define AML_BYTE_BREAK_OP		0xa5
#define AML_BYTE_BREAK_POINT_OP		0xcc
#define AML_BYTE_ONES_OP		0xff

#define AML_REGION_SPACE_SYSTEM_MEMORY		0x00
#define AML_REGION_SPACE_SYSTEM_IO		0x01
#define AML_REGION_SPACE_PCI_CONFIG		0x02
#define AML_REGION_SPACE_EMBEDDED_CONTROL	0x03
#define AML_REGION_SPACE_SM_BUS			0x04
#define AML_REGION_SPACE_SYSTEM_CMOS		0x05
#define AML_REGION_SPACE_PCI_BAR_TARGET		0x06
#define AML_REGION_SPACE_IPMI			0x07
#define AML_REGION_SPACE_GENERAL_PURPOSE_IO	0x08
#define AML_REGION_SPACE_GENERIC_SERIAL_BUS	0x09
#define AML_REGION_SPACE_PCC			0x0a

bool acpi_table_is_correct(ACPITableHeader const *header);
bool rsdp_is_correct(RSDP const *rsdp);

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

ChainString *aml_symbol_to_chain_string(AMLSymbol const *aml_symbol)
{
	ChainString **bytes_data_chain_string;
	ChainString **dwords_data_chain_string;
	ChainString **name_chars_chain_string;
	ChainString **name_segs_chain_string;
	ChainString **name_strings_chain_string;
	ChainString **operands_chain_string;
	ChainString **words_data_chain_string;
	ChainString *access_field_chain_string;
	ChainString *acquire_op_chain_string;
	ChainString *acquire_op_suffix_chain_string;
	ChainString *alias_op_chain_string;
	ChainString *arg_obj_chain_string;
	ChainString *arg_object_chain_string;
	ChainString *arg_op_chain_string;
	ChainString *ascii_char_chain_string;
	ChainString *ascii_char_list_chain_string;
	ChainString *buff_pkg_str_obj_chain_string;
	ChainString *buffer_op_chain_string;
	ChainString *buffer_size_chain_string;
	ChainString *byte_const_chain_string;
	ChainString *byte_data_chain_string;
	ChainString *byte_list_chain_string;
	ChainString *byte_prefix_chain_string;
	ChainString *computational_data_chain_string;
	ChainString *connect_field_chain_string;
	ChainString *const_obj_chain_string;
	ChainString *data_object_chain_string;
	ChainString *data_ref_object_chain_string;
	ChainString *debug_obj_chain_string;
	ChainString *def_acquire_chain_string;
	ChainString *def_add_chain_string;
	ChainString *def_alias_chain_string;
	ChainString *def_and_chain_string;
	ChainString *def_bank_field_chain_string;
	ChainString *def_break_chain_string;
	ChainString *def_break_point_chain_string;
	ChainString *def_buffer_chain_string;
	ChainString *def_concat_chain_string;
	ChainString *def_concat_res_chain_string;
	ChainString *def_cond_ref_of_chain_string;
	ChainString *def_continue_chain_string;
	ChainString *def_copy_object_chain_string;
	ChainString *def_create_bit_field_chain_string;
	ChainString *def_create_byte_field_chain_string;
	ChainString *def_create_dword_field_chain_string;
	ChainString *def_create_field_chain_string;
	ChainString *def_create_qword_field_chain_string;
	ChainString *def_create_word_field_chain_string;
	ChainString *def_data_region_chain_string;
	ChainString *def_decrement_chain_string;
	ChainString *def_deref_of_chain_string;
	ChainString *def_device_chain_string;
	ChainString *def_divide_chain_string;
	ChainString *def_else_chain_string;
	ChainString *def_external_chain_string;
	ChainString *def_fatal_chain_string;
	ChainString *def_field_chain_string;
	ChainString *def_find_set_left_bit_chain_string;
	ChainString *def_find_set_right_bit_chain_string;
	ChainString *def_from_bcd_chain_string;
	ChainString *def_if_else_chain_string;
	ChainString *def_increment_chain_string;
	ChainString *def_index_chain_string;
	ChainString *def_l_and_chain_string;
	ChainString *def_l_equal_chain_string;
	ChainString *def_l_greater_chain_string;
	ChainString *def_l_greater_equal_chain_string;
	ChainString *def_l_less_chain_string;
	ChainString *def_l_less_equal_chain_string;
	ChainString *def_l_not_chain_string;
	ChainString *def_l_not_equal_chain_string;
	ChainString *def_l_or_chain_string;
	ChainString *def_load_table_chain_string;
	ChainString *def_match_chain_string;
	ChainString *def_method_chain_string;
	ChainString *def_mid_chain_string;
	ChainString *def_mod_chain_string;
	ChainString *def_multiply_chain_string;
	ChainString *def_mutex_chain_string;
	ChainString *def_name_chain_string;
	ChainString *def_nand_chain_string;
	ChainString *def_noop_chain_string;
	ChainString *def_nor_chain_string;
	ChainString *def_not_chain_string;
	ChainString *def_notify_chain_string;
	ChainString *def_object_type_chain_string;
	ChainString *def_op_region_chain_string;
	ChainString *def_or_chain_string;
	ChainString *def_package_chain_string;
	ChainString *def_power_res_chain_string;
	ChainString *def_ref_of_chain_string;
	ChainString *def_release_chain_string;
	ChainString *def_reset_chain_string;
	ChainString *def_return_chain_string;
	ChainString *def_scope_chain_string;
	ChainString *def_shift_left_chain_string;
	ChainString *def_shift_right_chain_string;
	ChainString *def_signal_chain_string;
	ChainString *def_size_of_chain_string;
	ChainString *def_sleep_chain_string;
	ChainString *def_stall_chain_string;
	ChainString *def_store_chain_string;
	ChainString *def_subtract_chain_string;
	ChainString *def_thermal_zone_chain_string;
	ChainString *def_timer_chain_string;
	ChainString *def_to_bcd_chain_string;
	ChainString *def_to_buffer_chain_string;
	ChainString *def_to_decimal_string_chain_string;
	ChainString *def_to_hex_string_chain_string;
	ChainString *def_to_integer_chain_string;
	ChainString *def_to_string_chain_string;
	ChainString *def_var_package_chain_string;
	ChainString *def_wait_chain_string;
	ChainString *def_while_chain_string;
	ChainString *def_xor_chain_string;
	ChainString *deref_of_op_chain_string;
	ChainString *device_op_chain_string;
	ChainString *device_op_suffix_chain_string;
	ChainString *digit_char_chain_string;
	ChainString *dual_name_path_chain_string;
	ChainString *dual_name_prefix_chain_string;
	ChainString *dword_const_chain_string;
	ChainString *dword_data_chain_string;
	ChainString *dword_prefix_chain_string;
	ChainString *expression_opcode_chain_string;
	ChainString *ext_op_prefix_chain_string;
	ChainString *extended_access_field_chain_string;
	ChainString *field_element_chain_string;
	ChainString *field_flags_chain_string;
	ChainString *field_list_chain_string;
	ChainString *field_op_chain_string;
	ChainString *field_op_suffix_chain_string;
	ChainString *if_op_chain_string;
	ChainString *increment_op_chain_string;
	ChainString *index_op_chain_string;
	ChainString *index_value_chain_string;
	ChainString *l_equal_op_chain_string;
	ChainString *l_greater_op_chain_string;
	ChainString *l_less_op_chain_string;
	ChainString *l_or_op_chain_string;
	ChainString *lead_name_char_chain_string;
	ChainString *local_obj_chain_string;
	ChainString *local_op_chain_string;
	ChainString *method_flags_chain_string;
	ChainString *method_invocation_chain_string;
	ChainString *method_op_chain_string;
	ChainString *multi_name_path_chain_string;
	ChainString *multi_name_prefix_chain_string;
	ChainString *mutex_object_chain_string;
	ChainString *mutex_op_chain_string;
	ChainString *mutex_op_suffix_chain_string;
	ChainString *name_op_chain_string;
	ChainString *name_path_chain_string;
	ChainString *name_seg_chain_string;
	ChainString *name_space_modifier_obj_chain_string;
	ChainString *name_string_chain_string;
	ChainString *named_field_chain_string;
	ChainString *named_obj_chain_string;
	ChainString *null_char_chain_string;
	ChainString *null_name_chain_string;
	ChainString *num_elements_chain_string;
	ChainString *obj_reference_chain_string;
	ChainString *object_chain_string;
	ChainString *object_reference_chain_string;
	ChainString *one_op_chain_string;
	ChainString *ones_op_chain_string;
	ChainString *op_region_op_chain_string;
	ChainString *op_region_op_suffix_chain_string;
	ChainString *operand_chain_string;
	ChainString *output;
	ChainString *package_element_chain_string;
	ChainString *package_element_list_chain_string;
	ChainString *package_op_chain_string;
	ChainString *parent_prefix_char_chain_string;
	ChainString *pkg_lead_byte_chain_string;
	ChainString *pkg_length_chain_string;
	ChainString *predicate_chain_string;
	ChainString *prefix_path_chain_string;
	ChainString *qword_const_chain_string;
	ChainString *qword_data_chain_string;
	ChainString *qword_prefix_chain_string;
	ChainString *reference_type_opcode_chain_string;
	ChainString *region_len_chain_string;
	ChainString *region_offset_chain_string;
	ChainString *region_space_chain_string;
	ChainString *release_op_chain_string;
	ChainString *release_op_suffix_chain_string;
	ChainString *reserved_field_chain_string;
	ChainString *return_op_chain_string;
	ChainString *revision_op_chain_string;
	ChainString *revision_op_suffix_chain_string;
	ChainString *root_char_chain_string;
	ChainString *scope_op_chain_string;
	ChainString *seg_count_chain_string;
	ChainString *shift_count_chain_string;
	ChainString *shift_left_op_chain_string;
	ChainString *shift_right_op_chain_string;
	ChainString *simple_name_chain_string;
	ChainString *size_of_op_chain_string;
	ChainString *statement_opcode_chain_string;
	ChainString *store_op_chain_string;
	ChainString *string_chain_string;
	ChainString *string_prefix_chain_string;
	ChainString *subtract_op_chain_string;
	ChainString *super_name_chain_string;
	ChainString *sync_flags_chain_string;
	ChainString *target_chain_string;
	ChainString *term_arg_chain_string;
	ChainString *term_arg_list_chain_string;
	ChainString *term_list_chain_string;
	ChainString *term_obj_chain_string;
	ChainString *time_out_chain_string;
	ChainString *to_buffer_op_chain_string;
	ChainString *to_hex_string_op_chain_string;
	ChainString *while_op_chain_string;
	ChainString *word_const_chain_string;
	ChainString *word_data_chain_string;
	ChainString *word_prefix_chain_string;
	ChainString *zero_op_chain_string;
	char **bytes_data_char_array;
	char **dwords_data_char_array;
	char **name_chars_char_array;
	char **name_segs_char_array;
	char **name_strings_char_array;
	char **operands_char_array;
	char **words_data_char_array;
	char *access_field_char_array;
	char *acquire_op_char_array;
	char *acquire_op_suffix_char_array;
	char *alias_op_char_array;
	char *arg_obj_char_array;
	char *arg_object_char_array;
	char *arg_op_char_array;
	char *ascii_char_char_array;
	char *ascii_char_list_char_array;
	char *buff_pkg_str_obj_char_array;
	char *buffer_op_char_array;
	char *buffer_size_char_array;
	char *byte_const_char_array;
	char *byte_data_char_array;
	char *byte_list_char_array;
	char *byte_prefix_char_array;
	char *computational_data_char_array;
	char *connect_field_char_array;
	char *const_obj_char_array;
	char *data_object_char_array;
	char *data_ref_object_char_array;
	char *debug_obj_char_array;
	char *def_acquire_char_array;
	char *def_add_char_array;
	char *def_alias_char_array;
	char *def_and_char_array;
	char *def_bank_field_char_array;
	char *def_break_char_array;
	char *def_break_point_char_array;
	char *def_buffer_char_array;
	char *def_concat_char_array;
	char *def_concat_res_char_array;
	char *def_cond_ref_of_char_array;
	char *def_continue_char_array;
	char *def_copy_object_char_array;
	char *def_create_bit_field_char_array;
	char *def_create_byte_field_char_array;
	char *def_create_dword_field_char_array;
	char *def_create_field_char_array;
	char *def_create_qword_field_char_array;
	char *def_create_word_field_char_array;
	char *def_data_region_char_array;
	char *def_decrement_char_array;
	char *def_deref_of_char_array;
	char *def_device_char_array;
	char *def_divide_char_array;
	char *def_else_char_array;
	char *def_external_char_array;
	char *def_fatal_char_array;
	char *def_field_char_array;
	char *def_find_set_left_bit_char_array;
	char *def_find_set_right_bit_char_array;
	char *def_from_bcd_char_array;
	char *def_if_else_char_array;
	char *def_increment_char_array;
	char *def_index_char_array;
	char *def_l_and_char_array;
	char *def_l_equal_char_array;
	char *def_l_greater_char_array;
	char *def_l_greater_equal_char_array;
	char *def_l_less_char_array;
	char *def_l_less_equal_char_array;
	char *def_l_not_char_array;
	char *def_l_not_equal_char_array;
	char *def_l_or_char_array;
	char *def_load_table_char_array;
	char *def_match_char_array;
	char *def_method_char_array;
	char *def_mid_char_array;
	char *def_mod_char_array;
	char *def_multiply_char_array;
	char *def_mutex_char_array;
	char *def_name_char_array;
	char *def_nand_char_array;
	char *def_noop_char_array;
	char *def_nor_char_array;
	char *def_not_char_array;
	char *def_notify_char_array;
	char *def_object_type_char_array;
	char *def_op_region_char_array;
	char *def_or_char_array;
	char *def_package_char_array;
	char *def_power_res_char_array;
	char *def_ref_of_char_array;
	char *def_release_char_array;
	char *def_reset_char_array;
	char *def_return_char_array;
	char *def_scope_char_array;
	char *def_shift_left_char_array;
	char *def_shift_right_char_array;
	char *def_signal_char_array;
	char *def_size_of_char_array;
	char *def_sleep_char_array;
	char *def_stall_char_array;
	char *def_store_char_array;
	char *def_subtract_char_array;
	char *def_thermal_zone_char_array;
	char *def_timer_char_array;
	char *def_to_bcd_char_array;
	char *def_to_buffer_char_array;
	char *def_to_decimal_string_char_array;
	char *def_to_hex_string_char_array;
	char *def_to_integer_char_array;
	char *def_to_string_char_array;
	char *def_var_package_char_array;
	char *def_wait_char_array;
	char *def_while_char_array;
	char *def_xor_char_array;
	char *deref_of_op_char_array;
	char *device_op_char_array;
	char *device_op_suffix_char_array;
	char *digit_char_char_array;
	char *dual_name_path_char_array;
	char *dual_name_prefix_char_array;
	char *dword_const_char_array;
	char *dword_data_char_array;
	char *dword_prefix_char_array;
	char *expression_opcode_char_array;
	char *ext_op_prefix_char_array;
	char *extended_access_field_char_array;
	char *field_element_char_array;
	char *field_flags_char_array;
	char *field_list_char_array;
	char *field_op_char_array;
	char *field_op_suffix_char_array;
	char *if_op_char_array;
	char *increment_op_char_array;
	char *index_op_char_array;
	char *index_value_char_array;
	char *l_equal_op_char_array;
	char *l_greater_op_char_array;
	char *l_less_op_char_array;
	char *l_or_op_char_array;
	char *lead_name_char_char_array;
	char *local_obj_char_array;
	char *local_op_char_array;
	char *method_flags_char_array;
	char *method_invocation_char_array;
	char *method_op_char_array;
	char *multi_name_path_char_array;
	char *multi_name_prefix_char_array;
	char *mutex_object_char_array;
	char *mutex_op_char_array;
	char *mutex_op_suffix_char_array;
	char *name_op_char_array;
	char *name_path_char_array;
	char *name_seg_char_array;
	char *name_space_modifier_obj_char_array;
	char *name_string_char_array;
	char *named_field_char_array;
	char *named_obj_char_array;
	char *null_char_char_array;
	char *null_name_char_array;
	char *num_elements_char_array;
	char *obj_reference_char_array;
	char *object_char_array;
	char *object_reference_char_array;
	char *one_op_char_array;
	char *ones_op_char_array;
	char *op_region_op_char_array;
	char *op_region_op_suffix_char_array;
	char *operand_char_array;
	char *package_element_char_array;
	char *package_element_list_char_array;
	char *package_op_char_array;
	char *parent_prefix_char_char_array;
	char *pkg_lead_byte_char_array;
	char *pkg_length_char_array;
	char *predicate_char_array;
	char *prefix_path_char_array;
	char *qword_const_char_array;
	char *qword_data_char_array;
	char *qword_prefix_char_array;
	char *reference_type_opcode_char_array;
	char *region_len_char_array;
	char *region_offset_char_array;
	char *region_space_char_array;
	char *release_op_char_array;
	char *release_op_suffix_char_array;
	char *reserved_field_char_array;
	char *return_op_char_array;
	char *revision_op_char_array;
	char *revision_op_suffix_char_array;
	char *root_char_char_array;
	char *scope_op_char_array;
	char *seg_count_char_array;
	char *shift_count_char_array;
	char *shift_left_op_char_array;
	char *shift_right_op_char_array;
	char *simple_name_char_array;
	char *size_of_op_char_array;
	char *statement_opcode_char_array;
	char *store_op_char_array;
	char *string_char_array;
	char *string_prefix_char_array;
	char *subtract_op_char_array;
	char *super_name_char_array;
	char *sync_flags_char_array;
	char *target_char_array;
	char *term_arg_char_array;
	char *term_arg_list_char_array;
	char *term_list_char_array;
	char *term_obj_char_array;
	char *time_out_char_array;
	char *to_buffer_op_char_array;
	char *to_hex_string_op_char_array;
	char *while_op_char_array;
	char *word_const_char_array;
	char *word_data_char_array;
	char *word_prefix_char_array;
	char *zero_op_char_array;
	switch(aml_symbol->type)
	{
	case aml_acquire_op:
		if(aml_symbol->component.acquire_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.acquire_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.acquire_op.acquire_op_suffix)
		{
			acquire_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.acquire_op.acquire_op_suffix);
			insert_char_front(acquire_op_suffix_chain_string, acquire_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(acquire_op_suffix_chain_string, "\n", "\n ");
			acquire_op_suffix_char_array = create_char_array_from_chain_string(acquire_op_suffix_chain_string);
		}
		else acquire_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, acquire_op_suffix_char_array);
		if(aml_symbol->component.acquire_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.acquire_op.acquire_op_suffix)
		{
			delete_chain_string(acquire_op_suffix_chain_string);
			free(acquire_op_suffix_char_array);
		}
		break;
	case aml_acquire_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_alias_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_arg_obj:
		if(aml_symbol->component.arg_obj.arg_op)
		{
			arg_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.arg_obj.arg_op);
			insert_char_front(arg_op_chain_string, arg_op_chain_string->first_character, ' ');
			replace_chain_string(arg_op_chain_string, "\n", "\n ");
			arg_op_char_array = create_char_array_from_chain_string(arg_op_chain_string);
		}
		else arg_op_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), arg_op_char_array);
		if(aml_symbol->component.arg_obj.arg_op)
		{
			delete_chain_string(arg_op_chain_string);
			free(arg_op_char_array);
		}
		break;
	case aml_arg_object:
		if(aml_symbol->component.arg_object.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.arg_object.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.arg_object.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_arg_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_ascii_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_ascii_char_list:
		if(aml_symbol->component.ascii_char_list.ascii_char)
		{
			ascii_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.ascii_char_list.ascii_char);
			insert_char_front(ascii_char_chain_string, ascii_char_chain_string->first_character, ' ');
			replace_chain_string(ascii_char_chain_string, "\n", "\n ");
			ascii_char_char_array = create_char_array_from_chain_string(ascii_char_chain_string);
		}
		else ascii_char_char_array = "";
		if(aml_symbol->component.ascii_char_list.ascii_char_list)
		{
			ascii_char_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.ascii_char_list.ascii_char_list);
			insert_char_front(ascii_char_list_chain_string, ascii_char_list_chain_string->first_character, ' ');
			replace_chain_string(ascii_char_list_chain_string, "\n", "\n ");
			ascii_char_list_char_array = create_char_array_from_chain_string(ascii_char_list_chain_string);
		}
		else ascii_char_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ascii_char_char_array, ascii_char_list_char_array);
		if(aml_symbol->component.ascii_char_list.ascii_char)
		{
			delete_chain_string(ascii_char_chain_string);
			free(ascii_char_char_array);
		}
		if(aml_symbol->component.ascii_char_list.ascii_char_list)
		{
			delete_chain_string(ascii_char_list_chain_string);
			free(ascii_char_char_array);
		}
		break;
	case aml_buff_pkg_str_obj:
		if(aml_symbol->component.buff_pkg_str_obj.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.buff_pkg_str_obj.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.buff_pkg_str_obj.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_buffer_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_buffer_size:
		if(aml_symbol->component.buffer_size.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.buffer_size.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.buffer_size.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_byte_const:
		if(aml_symbol->component.byte_const.byte_prefix)
		{
			byte_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.byte_const.byte_prefix);
			insert_char_front(byte_prefix_chain_string, byte_prefix_chain_string->first_character, ' ');
			replace_chain_string(byte_prefix_chain_string, "\n", "\n ");
			byte_prefix_char_array = create_char_array_from_chain_string(byte_prefix_chain_string);
		}
		else byte_prefix_char_array = "";
		if(aml_symbol->component.byte_const.byte_data)
		{
			byte_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.byte_const.byte_data);
			insert_char_front(byte_data_chain_string, byte_data_chain_string->first_character, ' ');
			replace_chain_string(byte_data_chain_string, "\n", "\n ");
			byte_data_char_array = create_char_array_from_chain_string(byte_data_chain_string);
		}
		else byte_data_char_array = "";
		output = create_format_chain_string("%s %#04.2x\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.byte_const.value, byte_prefix_char_array, byte_data_char_array);
		if(aml_symbol->component.byte_const.byte_prefix)
		{
			delete_chain_string(byte_prefix_chain_string);
			free(byte_prefix_char_array);
		}
		if(aml_symbol->component.byte_const.byte_data)
		{
			delete_chain_string(byte_data_chain_string);
			free(byte_data_char_array);
		}
		break;
	case aml_byte_data:
		output = create_format_chain_string("%s %#04.2x\n", aml_symbol_type_name(aml_symbol->type), *aml_symbol->string.initial);
		break;
	case aml_byte_list:
		if(aml_symbol->component.byte_list.byte_data)
		{
			byte_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.byte_list.byte_data);
			insert_char_front(byte_data_chain_string, byte_data_chain_string->first_character, ' ');
			replace_chain_string(byte_data_chain_string, "\n", "\n ");
			byte_data_char_array = create_char_array_from_chain_string(byte_data_chain_string);
		}
		else byte_data_char_array = "";
		if(aml_symbol->component.byte_list.byte_list)
		{
			byte_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.byte_list.byte_list);
			insert_char_front(byte_list_chain_string, byte_list_chain_string->first_character, ' ');
			replace_chain_string(byte_list_chain_string, "\n", "\n ");
			byte_list_char_array = create_char_array_from_chain_string(byte_list_chain_string);
		}
		else byte_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), byte_data_char_array, byte_list_char_array);
		if(aml_symbol->component.byte_list.byte_data)
		{
			delete_chain_string(byte_data_chain_string);
			free(byte_data_char_array);
		}
		if(aml_symbol->component.byte_list.byte_list)
		{
			delete_chain_string(byte_list_chain_string);
			free(byte_list_char_array);
		}
		break;
	case aml_byte_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_computational_data:
		if(aml_symbol->component.computational_data.byte_const)
		{
			byte_const_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.byte_const);
			insert_char_front(byte_const_chain_string, byte_const_chain_string->first_character, ' ');
			replace_chain_string(byte_const_chain_string, "\n", "\n ");
			byte_const_char_array = create_char_array_from_chain_string(byte_const_chain_string);
		}
		else byte_const_char_array = "";
		if(aml_symbol->component.computational_data.word_const)
		{
			word_const_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.word_const);
			insert_char_front(word_const_chain_string, word_const_chain_string->first_character, ' ');
			replace_chain_string(word_const_chain_string, "\n", "\n ");
			word_const_char_array = create_char_array_from_chain_string(word_const_chain_string);
		}
		else word_const_char_array = "";
		if(aml_symbol->component.computational_data.dword_const)
		{
			dword_const_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.dword_const);
			insert_char_front(dword_const_chain_string, dword_const_chain_string->first_character, ' ');
			replace_chain_string(dword_const_chain_string, "\n", "\n ");
			dword_const_char_array = create_char_array_from_chain_string(dword_const_chain_string);
		}
		else dword_const_char_array = "";
		if(aml_symbol->component.computational_data.qword_const)
		{
			qword_const_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.qword_const);
			insert_char_front(qword_const_chain_string, qword_const_chain_string->first_character, ' ');
			replace_chain_string(qword_const_chain_string, "\n", "\n ");
			qword_const_char_array = create_char_array_from_chain_string(qword_const_chain_string);
		}
		else qword_const_char_array = "";
		if(aml_symbol->component.computational_data.string)
		{
			string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.string);
			insert_char_front(string_chain_string, string_chain_string->first_character, ' ');
			replace_chain_string(string_chain_string, "\n", "\n ");
			string_char_array = create_char_array_from_chain_string(string_chain_string);
		}
		else string_char_array = "";
		if(aml_symbol->component.computational_data.const_obj)
		{
			const_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.const_obj);
			insert_char_front(const_obj_chain_string, const_obj_chain_string->first_character, ' ');
			replace_chain_string(const_obj_chain_string, "\n", "\n ");
			const_obj_char_array = create_char_array_from_chain_string(const_obj_chain_string);
		}
		else const_obj_char_array = "";
		if(aml_symbol->component.computational_data.revision_op)
		{
			revision_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.revision_op);
			insert_char_front(revision_op_chain_string, revision_op_chain_string->first_character, ' ');
			replace_chain_string(revision_op_chain_string, "\n", "\n ");
			revision_op_char_array = create_char_array_from_chain_string(revision_op_chain_string);
		}
		else revision_op_char_array = "";
		if(aml_symbol->component.computational_data.def_buffer)
		{
			def_buffer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.computational_data.def_buffer);
			insert_char_front(def_buffer_chain_string, def_buffer_chain_string->first_character, ' ');
			replace_chain_string(def_buffer_chain_string, "\n", "\n ");
			def_buffer_char_array = create_char_array_from_chain_string(def_buffer_chain_string);
		}
		else def_buffer_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), byte_const_char_array, word_const_char_array, dword_const_char_array, qword_const_char_array, string_char_array, const_obj_char_array, revision_op_char_array, def_buffer_char_array);
		if(aml_symbol->component.computational_data.byte_const)
		{
			delete_chain_string(byte_const_chain_string);
			free(byte_const_char_array);
		}
		if(aml_symbol->component.computational_data.word_const)
		{
			delete_chain_string(word_const_chain_string);
			free(word_const_char_array);
		}
		if(aml_symbol->component.computational_data.dword_const)
		{
			delete_chain_string(dword_const_chain_string);
			free(dword_const_char_array);
		}
		if(aml_symbol->component.computational_data.qword_const)
		{
			delete_chain_string(qword_const_chain_string);
			free(qword_const_char_array);
		}
		if(aml_symbol->component.computational_data.string)
		{
			delete_chain_string(string_chain_string);
			free(string_char_array);
		}
		if(aml_symbol->component.computational_data.const_obj)
		{
			delete_chain_string(const_obj_chain_string);
			free(const_obj_char_array);
		}
		if(aml_symbol->component.computational_data.revision_op)
		{
			delete_chain_string(revision_op_chain_string);
			free(revision_op_char_array);
		}
		if(aml_symbol->component.computational_data.def_buffer)
		{
			delete_chain_string(def_buffer_chain_string);
			free(def_buffer_char_array);
		}
		break;
	case aml_const_obj:
		if(aml_symbol->component.const_obj.zero_op)
		{
			zero_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.const_obj.zero_op);
			insert_char_front(zero_op_chain_string, zero_op_chain_string->first_character, ' ');
			replace_chain_string(zero_op_chain_string, "\n", "\n ");
			zero_op_char_array = create_char_array_from_chain_string(zero_op_chain_string);
		}
		else zero_op_char_array = "";
		if(aml_symbol->component.const_obj.one_op)
		{
			one_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.const_obj.one_op);
			insert_char_front(one_op_chain_string, one_op_chain_string->first_character, ' ');
			replace_chain_string(one_op_chain_string, "\n", "\n ");
			one_op_char_array = create_char_array_from_chain_string(one_op_chain_string);
		}
		else one_op_char_array = "";
		if(aml_symbol->component.const_obj.ones_op)
		{
			ones_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.const_obj.ones_op);
			insert_char_front(ones_op_chain_string, ones_op_chain_string->first_character, ' ');
			replace_chain_string(ones_op_chain_string, "\n", "\n ");
			ones_op_char_array = create_char_array_from_chain_string(ones_op_chain_string);
		}
		else ones_op_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), zero_op_char_array, one_op_char_array, ones_op_char_array);
		if(aml_symbol->component.const_obj.zero_op)
		{
			delete_chain_string(zero_op_chain_string);
			free(zero_op_char_array);
		}
		if(aml_symbol->component.const_obj.one_op)
		{
			delete_chain_string(one_op_chain_string);
			free(one_op_char_array);
		}
		if(aml_symbol->component.const_obj.ones_op)
		{
			delete_chain_string(ones_op_chain_string);
			free(ones_op_char_array);
		}
		break;
	case aml_data_object:
		if(aml_symbol->component.data_object.computational_data)
		{
			computational_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.data_object.computational_data);
			insert_char_front(computational_data_chain_string, computational_data_chain_string->first_character, ' ');
			replace_chain_string(computational_data_chain_string, "\n", "\n ");
			computational_data_char_array = create_char_array_from_chain_string(computational_data_chain_string);
		}
		else computational_data_char_array = "";
		if(aml_symbol->component.data_object.def_package)
		{
			def_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.data_object.def_package);
			insert_char_front(def_package_chain_string, def_package_chain_string->first_character, ' ');
			replace_chain_string(def_package_chain_string, "\n", "\n ");
			def_package_char_array = create_char_array_from_chain_string(def_package_chain_string);
		}
		else def_package_char_array = "";
		if(aml_symbol->component.data_object.def_var_package)
		{
			def_var_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.data_object.def_var_package);
			insert_char_front(def_var_package_chain_string, def_var_package_chain_string->first_character, ' ');
			replace_chain_string(def_var_package_chain_string, "\n", "\n ");
			def_var_package_char_array = create_char_array_from_chain_string(def_var_package_chain_string);
		}
		else def_var_package_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), computational_data_char_array, def_package_char_array, def_var_package_char_array);
		if(aml_symbol->component.data_object.computational_data)
		{
			delete_chain_string(computational_data_chain_string);
			free(computational_data_char_array);
		}
		if(aml_symbol->component.data_object.def_package)
		{
			delete_chain_string(def_package_chain_string);
			free(def_package_char_array);
		}
		if(aml_symbol->component.data_object.def_var_package)
		{
			delete_chain_string(def_var_package_chain_string);
			free(def_var_package_char_array);
		}
		break;
	case aml_data_ref_object:
		if(aml_symbol->component.data_ref_object.data_object)
		{
			data_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.data_ref_object.data_object);
			insert_char_front(data_object_chain_string, data_object_chain_string->first_character, ' ');
			replace_chain_string(data_object_chain_string, "\n", "\n ");
			data_object_char_array = create_char_array_from_chain_string(data_object_chain_string);
		}
		else data_object_char_array = "";
		if(aml_symbol->component.data_ref_object.object_reference)
		{
			object_reference_chain_string = aml_symbol_to_chain_string(aml_symbol->component.data_ref_object.object_reference);
			insert_char_front(object_reference_chain_string, object_reference_chain_string->first_character, ' ');
			replace_chain_string(object_reference_chain_string, "\n", "\n ");
			object_reference_char_array = create_char_array_from_chain_string(object_reference_chain_string);
		}
		else object_reference_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), data_object_char_array, object_reference_char_array);
		if(aml_symbol->component.data_ref_object.data_object)
		{
			delete_chain_string(data_object_chain_string);
			free(data_object_char_array);
		}
		if(aml_symbol->component.data_ref_object.object_reference)
		{
			delete_chain_string(object_reference_chain_string);
			free(object_reference_char_array);
		}
		break;
	case aml_def_alias:
		name_strings_chain_string = malloc(_countof(aml_symbol->component.def_alias.name_string) * sizeof(*name_strings_chain_string));
		name_strings_char_array = malloc(_countof(aml_symbol->component.def_alias.name_string) * sizeof(*name_strings_char_array));
		if(aml_symbol->component.def_alias.alias_op)
		{
			alias_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_alias.alias_op);
			insert_char_front(alias_op_chain_string, alias_op_chain_string->first_character, ' ');
			replace_chain_string(alias_op_chain_string, "\n", "\n ");
			alias_op_char_array = create_char_array_from_chain_string(alias_op_chain_string);
		}
		else alias_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_alias.name_string); i++)if(aml_symbol->component.def_alias.name_string[i])
		{
			name_strings_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_alias.name_string[i]);
			insert_char_front(name_strings_chain_string[i], name_strings_chain_string[i]->first_character, ' ');
			replace_chain_string(name_strings_chain_string[i], "\n", "\n ");
			name_strings_char_array[i] = create_char_array_from_chain_string(name_strings_chain_string[i]);
		}
		else name_strings_char_array[i] = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), alias_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_alias.name_string); i++)if(aml_symbol->component.def_alias.name_string[i])insert_char_array_back(output, output->last_character, name_strings_char_array[i]);
		if(aml_symbol->component.def_alias.alias_op)
		{
			delete_chain_string(alias_op_chain_string);
			free(alias_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_alias.name_string); i++)if(aml_symbol->component.def_alias.name_string[i])
		{
			delete_chain_string(name_strings_chain_string[i]);
			free(name_strings_char_array[i]);
		}
		free(name_strings_chain_string);
		free(name_strings_char_array);
		break;
	case aml_def_acquire:
		if(aml_symbol->component.def_acquire.acquire_op)
		{
			acquire_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_acquire.acquire_op);
			insert_char_front(acquire_op_chain_string, acquire_op_chain_string->first_character, ' ');
			replace_chain_string(acquire_op_chain_string, "\n", "\n ");
			acquire_op_char_array = create_char_array_from_chain_string(acquire_op_chain_string);
		}
		else acquire_op_char_array = "";
		if(aml_symbol->component.def_acquire.mutex_object)
		{
			mutex_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_acquire.mutex_object);
			insert_char_front(mutex_object_chain_string, mutex_object_chain_string->first_character, ' ');
			replace_chain_string(mutex_object_chain_string, "\n", "\n ");
			mutex_object_char_array = create_char_array_from_chain_string(mutex_object_chain_string);
		}
		else mutex_object_char_array = "";
		if(aml_symbol->component.def_acquire.time_out)
		{
			time_out_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_acquire.time_out);
			insert_char_front(time_out_chain_string, time_out_chain_string->first_character, ' ');
			replace_chain_string(time_out_chain_string, "\n", "\n ");
			time_out_char_array = create_char_array_from_chain_string(time_out_chain_string);
		}
		else time_out_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), acquire_op_char_array, mutex_object_char_array, time_out_char_array);
		if(aml_symbol->component.def_acquire.acquire_op)
		{
			delete_chain_string(acquire_op_chain_string);
			free(acquire_op_char_array);
		}
		if(aml_symbol->component.def_acquire.mutex_object)
		{
			delete_chain_string(mutex_object_chain_string);
			free(mutex_object_char_array);
		}
		if(aml_symbol->component.def_acquire.time_out)
		{
			delete_chain_string(time_out_chain_string);
			free(time_out_char_array);
		}
		break;
	case aml_def_buffer:
		if(aml_symbol->component.def_buffer.buffer_op)
		{
			buffer_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_buffer.buffer_op);
			insert_char_front(buffer_op_chain_string, buffer_op_chain_string->first_character, ' ');
			replace_chain_string(buffer_op_chain_string, "\n", "\n ");
			buffer_op_char_array = create_char_array_from_chain_string(buffer_op_chain_string);
		}
		else buffer_op_char_array = "";
		if(aml_symbol->component.def_buffer.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_buffer.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_buffer.buffer_size)
		{
			buffer_size_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_buffer.buffer_size);
			insert_char_front(buffer_size_chain_string, buffer_size_chain_string->first_character, ' ');
			replace_chain_string(buffer_size_chain_string, "\n", "\n ");
			buffer_size_char_array = create_char_array_from_chain_string(buffer_size_chain_string);
		}
		else buffer_size_char_array = "";
		if(aml_symbol->component.def_buffer.byte_list)
		{
			byte_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_buffer.byte_list);
			insert_char_front(byte_list_chain_string, byte_list_chain_string->first_character, ' ');
			replace_chain_string(byte_list_chain_string, "\n", "\n ");
			byte_list_char_array = create_char_array_from_chain_string(byte_list_chain_string);
		}
		else byte_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, buffer_op_char_array, pkg_length_char_array, buffer_size_char_array, byte_list_char_array);
		if(aml_symbol->component.def_buffer.buffer_op)
		{
			delete_chain_string(buffer_op_chain_string);
			free(buffer_op_char_array);
		}
		if(aml_symbol->component.def_buffer.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_buffer.buffer_size)
		{
			delete_chain_string(buffer_size_chain_string);
			free(buffer_size_char_array);
		}
		if(aml_symbol->component.def_buffer.byte_list)
		{
			delete_chain_string(byte_list_chain_string);
			free(byte_list_char_array);
		}
		break;
	case aml_def_deref_of:
		if(aml_symbol->component.def_deref_of.deref_of_op)
		{
			deref_of_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_deref_of.deref_of_op);
			insert_char_front(deref_of_op_chain_string, deref_of_op_chain_string->first_character, ' ');
			replace_chain_string(deref_of_op_chain_string, "\n", "\n ");
			deref_of_op_char_array = create_char_array_from_chain_string(deref_of_op_chain_string);
		}
		else deref_of_op_char_array = "";
		if(aml_symbol->component.def_deref_of.obj_reference)
		{
			obj_reference_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_deref_of.obj_reference);
			insert_char_front(obj_reference_chain_string, obj_reference_chain_string->first_character, ' ');
			replace_chain_string(obj_reference_chain_string, "\n", "\n ");
			obj_reference_char_array = create_char_array_from_chain_string(obj_reference_chain_string);
		}
		else obj_reference_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), deref_of_op_char_array, obj_reference_char_array);
		if(aml_symbol->component.def_deref_of.deref_of_op)
		{
			delete_chain_string(deref_of_op_chain_string);
			free(deref_of_op_char_array);
		}
		if(aml_symbol->component.def_deref_of.obj_reference)
		{
			delete_chain_string(obj_reference_chain_string);
			free(obj_reference_char_array);
		}
		break;
	case aml_def_device:
		if(aml_symbol->component.def_device.device_op)
		{
			device_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_device.device_op);
			insert_char_front(device_op_chain_string, device_op_chain_string->first_character, ' ');
			replace_chain_string(device_op_chain_string, "\n", "\n ");
			device_op_char_array = create_char_array_from_chain_string(device_op_chain_string);
		}
		else device_op_char_array = "";
		if(aml_symbol->component.def_device.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_device.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_device.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_device.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_device.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_device.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, device_op_char_array, pkg_length_char_array, name_string_char_array, term_list_char_array);
		if(aml_symbol->component.def_device.device_op)
		{
			delete_chain_string(device_op_chain_string);
			free(device_op_char_array);
		}
		if(aml_symbol->component.def_device.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_device.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_device.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		break;
	case aml_def_field:
		if(aml_symbol->component.def_field.field_op)
		{
			field_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_field.field_op);
			insert_char_front(field_op_chain_string, field_op_chain_string->first_character, ' ');
			replace_chain_string(field_op_chain_string, "\n", "\n ");
			field_op_char_array = create_char_array_from_chain_string(field_op_chain_string);
		}
		else field_op_char_array = "";
		if(aml_symbol->component.def_field.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_field.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_field.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_field.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_field.field_flags)
		{
			field_flags_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_field.field_flags);
			insert_char_front(field_flags_chain_string, field_flags_chain_string->first_character, ' ');
			replace_chain_string(field_flags_chain_string, "\n", "\n ");
			field_flags_char_array = create_char_array_from_chain_string(field_flags_chain_string);
		}
		else field_flags_char_array = "";
		if(aml_symbol->component.def_field.field_list)
		{
			field_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_field.field_list);
			insert_char_front(field_list_chain_string, field_list_chain_string->first_character, ' ');
			replace_chain_string(field_list_chain_string, "\n", "\n ");
			field_list_char_array = create_char_array_from_chain_string(field_list_chain_string);
		}
		else field_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, field_op_char_array, pkg_length_char_array, name_string_char_array, field_flags_char_array, field_list_char_array);
		if(aml_symbol->component.def_field.field_op)
		{
			delete_chain_string(field_op_chain_string);
			free(field_op_char_array);
		}
		if(aml_symbol->component.def_field.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_field.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_field.field_flags)
		{
			delete_chain_string(field_flags_chain_string);
			free(field_flags_char_array);
		}
		if(aml_symbol->component.def_field.field_list)
		{
			delete_chain_string(field_list_chain_string);
			free(field_list_char_array);
		}
		break;
	case aml_def_if_else:
		if(aml_symbol->component.def_if_else.if_op)
		{
			if_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_if_else.if_op);
			insert_char_front(if_op_chain_string, if_op_chain_string->first_character, ' ');
			replace_chain_string(if_op_chain_string, "\n", "\n ");
			if_op_char_array = create_char_array_from_chain_string(if_op_chain_string);
		}
		else if_op_char_array = "";
		if(aml_symbol->component.def_if_else.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_if_else.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_if_else.predicate)
		{
			predicate_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_if_else.predicate);
			insert_char_front(predicate_chain_string, predicate_chain_string->first_character, ' ');
			replace_chain_string(predicate_chain_string, "\n", "\n ");
			predicate_char_array = create_char_array_from_chain_string(predicate_chain_string);
		}
		else predicate_char_array = "";
		if(aml_symbol->component.def_if_else.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_if_else.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		if(aml_symbol->component.def_if_else.def_else)
		{
			def_else_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_if_else.def_else);
			insert_char_front(def_else_chain_string, def_else_chain_string->first_character, ' ');
			replace_chain_string(def_else_chain_string, "\n", "\n ");
			def_else_char_array = create_char_array_from_chain_string(def_else_chain_string);
		}
		else def_else_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, if_op_char_array, pkg_length_char_array, predicate_char_array, term_list_char_array, def_else_char_array);
		if(aml_symbol->component.def_if_else.if_op)
		{
			delete_chain_string(if_op_chain_string);
			free(if_op_char_array);
		}
		if(aml_symbol->component.def_if_else.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_if_else.predicate)
		{
			delete_chain_string(predicate_chain_string);
			free(predicate_char_array);
		}
		if(aml_symbol->component.def_if_else.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		if(aml_symbol->component.def_if_else.def_else)
		{
			delete_chain_string(def_else_chain_string);
			free(def_else_char_array);
		}
		break;
	case aml_def_increment:
		if(aml_symbol->component.def_increment.increment_op)
		{
			increment_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_increment.increment_op);
			insert_char_front(increment_op_chain_string, increment_op_chain_string->first_character, ' ');
			replace_chain_string(increment_op_chain_string, "\n", "\n ");
			increment_op_char_array = create_char_array_from_chain_string(increment_op_chain_string);
		}
		else increment_op_char_array = "";
		if(aml_symbol->component.def_increment.super_name)
		{
			super_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_increment.super_name);
			insert_char_front(super_name_chain_string, super_name_chain_string->first_character, ' ');
			replace_chain_string(super_name_chain_string, "\n", "\n ");
			super_name_char_array = create_char_array_from_chain_string(super_name_chain_string);
		}
		else super_name_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), increment_op_char_array, super_name_char_array);
		if(aml_symbol->component.def_increment.increment_op)
		{
			delete_chain_string(increment_op_chain_string);
			free(increment_op_char_array);
		}
		if(aml_symbol->component.def_increment.super_name)
		{
			delete_chain_string(super_name_chain_string);
			free(super_name_char_array);
		}
		break;
	case aml_def_index:
		if(aml_symbol->component.def_index.index_op)
		{
			index_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_index.index_op);
			insert_char_front(index_op_chain_string, index_op_chain_string->first_character, ' ');
			replace_chain_string(index_op_chain_string, "\n", "\n ");
			index_op_char_array = create_char_array_from_chain_string(index_op_chain_string);
		}
		else index_op_char_array = "";
		if(aml_symbol->component.def_index.buff_pkg_str_obj)
		{
			buff_pkg_str_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_index.buff_pkg_str_obj);
			insert_char_front(buff_pkg_str_obj_chain_string, buff_pkg_str_obj_chain_string->first_character, ' ');
			replace_chain_string(buff_pkg_str_obj_chain_string, "\n", "\n ");
			buff_pkg_str_obj_char_array = create_char_array_from_chain_string(buff_pkg_str_obj_chain_string);
		}
		else buff_pkg_str_obj_char_array = "";
		if(aml_symbol->component.def_index.index_value)
		{
			index_value_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_index.index_value);
			insert_char_front(index_value_chain_string, index_value_chain_string->first_character, ' ');
			replace_chain_string(index_value_chain_string, "\n", "\n ");
			index_value_char_array = create_char_array_from_chain_string(index_value_chain_string);
		}
		else index_value_char_array = "";
		if(aml_symbol->component.def_index.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_index.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), index_op_char_array, buff_pkg_str_obj_char_array, index_value_char_array, target_char_array);
		if(aml_symbol->component.def_index.index_op)
		{
			delete_chain_string(index_op_chain_string);
			free(index_op_char_array);
		}
		if(aml_symbol->component.def_index.buff_pkg_str_obj)
		{
			delete_chain_string(buff_pkg_str_obj_chain_string);
			free(buff_pkg_str_obj_char_array);
		}
		if(aml_symbol->component.def_index.index_value)
		{
			delete_chain_string(index_value_chain_string);
			free(index_value_char_array);
		}
		if(aml_symbol->component.def_index.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		break;
	case aml_def_l_equal:
		operands_chain_string = malloc(_countof(aml_symbol->component.def_l_equal.operand) * sizeof(*operands_chain_string));
		operands_char_array = malloc(_countof(aml_symbol->component.def_l_equal.operand) * sizeof(*operands_char_array));
		if(aml_symbol->component.def_l_equal.l_equal_op)
		{
			l_equal_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_l_equal.l_equal_op);
			insert_char_front(l_equal_op_chain_string, l_equal_op_chain_string->first_character, ' ');
			replace_chain_string(l_equal_op_chain_string, "\n", "\n ");
			l_equal_op_char_array = create_char_array_from_chain_string(l_equal_op_chain_string);
		}
		else l_equal_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_equal.operand); i++)if(aml_symbol->component.def_l_equal.operand[i])
		{
			operands_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_l_equal.operand[i]);
			insert_char_front(operands_chain_string[i], operands_chain_string[i]->first_character, ' ');
			replace_chain_string(operands_chain_string[i], "\n", "\n ");
			operands_char_array[i] = create_char_array_from_chain_string(operands_chain_string[i]);
		}
		else operands_char_array[i] = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), l_equal_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_equal.operand); i++)insert_char_array_back(output, output->last_character, operands_char_array[i]);
		if(aml_symbol->component.def_l_equal.l_equal_op)
		{
			delete_chain_string(l_equal_op_chain_string);
			free(l_equal_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_equal.operand); i++)if(aml_symbol->component.def_l_equal.operand[i])
		{
			delete_chain_string(operands_chain_string[i]);
			free(operands_char_array[i]);
		}
		free(operands_chain_string);
		free(operands_char_array);
		break;
	case aml_def_l_greater:
		operands_chain_string = malloc(_countof(aml_symbol->component.def_l_greater.operand) * sizeof(*operands_chain_string));
		operands_char_array = malloc(_countof(aml_symbol->component.def_l_greater.operand) * sizeof(*operands_char_array));
		if(aml_symbol->component.def_l_greater.l_greater_op)
		{
			l_greater_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_l_greater.l_greater_op);
			insert_char_front(l_greater_op_chain_string, l_greater_op_chain_string->first_character, ' ');
			replace_chain_string(l_greater_op_chain_string, "\n", "\n ");
			l_greater_op_char_array = create_char_array_from_chain_string(l_greater_op_chain_string);
		}
		else l_greater_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_greater.operand); i++)if(aml_symbol->component.def_l_greater.operand[i])
		{
			operands_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_l_greater.operand[i]);
			insert_char_front(operands_chain_string[i], operands_chain_string[i]->first_character, ' ');
			replace_chain_string(operands_chain_string[i], "\n", "\n ");
			operands_char_array[i] = create_char_array_from_chain_string(operands_chain_string[i]);
		}
		else operands_char_array[i] = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), l_greater_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_greater.operand); i++)insert_char_array_back(output, output->last_character, operands_char_array[i]);
		if(aml_symbol->component.def_l_greater.l_greater_op)
		{
			delete_chain_string(l_greater_op_chain_string);
			free(l_greater_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_greater.operand); i++)if(aml_symbol->component.def_l_greater.operand[i])
		{
			delete_chain_string(operands_chain_string[i]);
			free(operands_char_array[i]);
		}
		free(operands_chain_string);
		free(operands_char_array);
		break;
	case aml_def_l_less:
		operands_chain_string = malloc(_countof(aml_symbol->component.def_l_less.operand) * sizeof(*operands_chain_string));
		operands_char_array = malloc(_countof(aml_symbol->component.def_l_less.operand) * sizeof(*operands_char_array));
		if(aml_symbol->component.def_l_less.l_less_op)
		{
			l_less_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_l_less.l_less_op);
			insert_char_front(l_less_op_chain_string, l_less_op_chain_string->first_character, ' ');
			replace_chain_string(l_less_op_chain_string, "\n", "\n ");
			l_less_op_char_array = create_char_array_from_chain_string(l_less_op_chain_string);
		}
		else l_less_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_less.operand); i++)if(aml_symbol->component.def_l_less.operand[i])
		{
			operands_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_l_less.operand[i]);
			insert_char_front(operands_chain_string[i], operands_chain_string[i]->first_character, ' ');
			replace_chain_string(operands_chain_string[i], "\n", "\n ");
			operands_char_array[i] = create_char_array_from_chain_string(operands_chain_string[i]);
		}
		else operands_char_array[i] = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), l_less_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_less.operand); i++)insert_char_array_back(output, output->last_character, operands_char_array[i]);
		if(aml_symbol->component.def_l_less.l_less_op)
		{
			delete_chain_string(l_less_op_chain_string);
			free(l_less_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_less.operand); i++)if(aml_symbol->component.def_l_less.operand[i])
		{
			delete_chain_string(operands_chain_string[i]);
			free(operands_char_array[i]);
		}
		free(operands_chain_string);
		free(operands_char_array);
		break;
	case aml_def_l_or:
		operands_chain_string = malloc(_countof(aml_symbol->component.def_l_or.operand) * sizeof(*operands_chain_string));
		operands_char_array = malloc(_countof(aml_symbol->component.def_l_or.operand) * sizeof(*operands_char_array));
		if(aml_symbol->component.def_l_or.l_or_op)
		{
			l_or_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_l_or.l_or_op);
			insert_char_front(l_or_op_chain_string, l_or_op_chain_string->first_character, ' ');
			replace_chain_string(l_or_op_chain_string, "\n", "\n ");
			l_or_op_char_array = create_char_array_from_chain_string(l_or_op_chain_string);
		}
		else l_or_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)if(aml_symbol->component.def_l_or.operand[i])
		{
			operands_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_l_or.operand[i]);
			insert_char_front(operands_chain_string[i], operands_chain_string[i]->first_character, ' ');
			replace_chain_string(operands_chain_string[i], "\n", "\n ");
			operands_char_array[i] = create_char_array_from_chain_string(operands_chain_string[i]);
		}
		else operands_char_array[i] = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), l_or_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)insert_char_array_back(output, output->last_character, operands_char_array[i]);
		if(aml_symbol->component.def_l_or.l_or_op)
		{
			delete_chain_string(l_or_op_chain_string);
			free(l_or_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)if(aml_symbol->component.def_l_or.operand[i])
		{
			delete_chain_string(operands_chain_string[i]);
			free(operands_char_array[i]);
		}
		free(operands_chain_string);
		free(operands_char_array);
		break;
	case aml_def_method:
		if(aml_symbol->component.def_method.method_op)
		{
			method_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_method.method_op);
			insert_char_front(method_op_chain_string, method_op_chain_string->first_character, ' ');
			replace_chain_string(method_op_chain_string, "\n", "\n ");
			method_op_char_array = create_char_array_from_chain_string(method_op_chain_string);
		}
		else method_op_char_array = "";
		if(aml_symbol->component.def_method.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_method.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_method.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_method.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_method.method_flags)
		{
			method_flags_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_method.method_flags);
			insert_char_front(method_flags_chain_string, method_flags_chain_string->first_character, ' ');
			replace_chain_string(method_flags_chain_string, "\n", "\n ");
			method_flags_char_array = create_char_array_from_chain_string(method_flags_chain_string);
		}
		else method_flags_char_array = "";
		if(aml_symbol->component.def_method.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_method.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, method_op_char_array, pkg_length_char_array, name_string_char_array, method_flags_char_array, term_list_char_array);
		if(aml_symbol->component.def_method.method_op)
		{
			delete_chain_string(method_op_chain_string);
			free(method_op_char_array);
		}
		if(aml_symbol->component.def_method.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_method.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_method.method_flags)
		{
			delete_chain_string(method_flags_chain_string);
			free(method_flags_char_array);
		}
		if(aml_symbol->component.def_method.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		break;
	case aml_def_mutex:
		if(aml_symbol->component.def_mutex.mutex_op)
		{
			mutex_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_mutex.mutex_op);
			insert_char_front(mutex_op_chain_string, mutex_op_chain_string->first_character, ' ');
			replace_chain_string(mutex_op_chain_string, "\n", "\n ");
			mutex_op_char_array = create_char_array_from_chain_string(mutex_op_chain_string);
		}
		else mutex_op_char_array = "";
		if(aml_symbol->component.def_mutex.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_mutex.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_mutex.sync_flags)
		{
			sync_flags_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_mutex.sync_flags);
			insert_char_front(sync_flags_chain_string, sync_flags_chain_string->first_character, ' ');
			replace_chain_string(sync_flags_chain_string, "\n", "\n ");
			sync_flags_char_array = create_char_array_from_chain_string(sync_flags_chain_string);
		}
		else sync_flags_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), mutex_op_char_array, name_string_char_array, sync_flags_char_array);
		if(aml_symbol->component.def_mutex.mutex_op)
		{
			delete_chain_string(mutex_op_chain_string);
			free(mutex_op_char_array);
		}
		if(aml_symbol->component.def_mutex.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_mutex.sync_flags)
		{
			delete_chain_string(sync_flags_chain_string);
			free(sync_flags_char_array);
		}
		break;
	case aml_def_name:
		if(aml_symbol->component.def_name.name_op)
		{
			name_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_name.name_op);
			insert_char_front(name_op_chain_string, name_op_chain_string->first_character, ' ');
			replace_chain_string(name_op_chain_string, "\n", "\n ");
			name_op_char_array = create_char_array_from_chain_string(name_op_chain_string);
		}
		else name_op_char_array = "";
		if(aml_symbol->component.def_name.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_name.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_name.data_ref_object)
		{
			data_ref_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_name.data_ref_object);
			insert_char_front(data_ref_object_chain_string, data_ref_object_chain_string->first_character, ' ');
			replace_chain_string(data_ref_object_chain_string, "\n", "\n ");
			data_ref_object_char_array = create_char_array_from_chain_string(data_ref_object_chain_string);
		}
		else data_ref_object_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), name_op_char_array, name_string_char_array, data_ref_object_char_array);
		if(aml_symbol->component.def_name.name_op)
		{
			delete_chain_string(name_op_chain_string);
			free(name_op_char_array);
		}
		if(aml_symbol->component.def_name.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_name.data_ref_object)
		{
			delete_chain_string(data_ref_object_chain_string);
			free(data_ref_object_char_array);
		}
		break;
	case aml_def_op_region:
		if(aml_symbol->component.def_op_region.op_region_op)
		{
			op_region_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_op_region.op_region_op);
			insert_char_front(op_region_op_chain_string, op_region_op_chain_string->first_character, ' ');
			replace_chain_string(op_region_op_chain_string, "\n", "\n ");
			op_region_op_char_array = create_char_array_from_chain_string(op_region_op_chain_string);
		}
		else op_region_op_char_array = "";
		if(aml_symbol->component.def_op_region.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_op_region.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_op_region.region_space)
		{
			region_space_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_op_region.region_space);
			insert_char_front(region_space_chain_string, region_space_chain_string->first_character, ' ');
			replace_chain_string(region_space_chain_string, "\n", "\n ");
			region_space_char_array = create_char_array_from_chain_string(region_space_chain_string);
		}
		else region_space_char_array = "";
		if(aml_symbol->component.def_op_region.region_offset)
		{
			region_offset_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_op_region.region_offset);
			insert_char_front(region_offset_chain_string, region_offset_chain_string->first_character, ' ');
			replace_chain_string(region_offset_chain_string, "\n", "\n ");
			region_offset_char_array = create_char_array_from_chain_string(region_offset_chain_string);
		}
		else region_offset_char_array = "";
		if(aml_symbol->component.def_op_region.region_len)
		{
			region_len_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_op_region.region_len);
			insert_char_front(region_len_chain_string, region_len_chain_string->first_character, ' ');
			replace_chain_string(region_len_chain_string, "\n", "\n ");
			region_len_char_array = create_char_array_from_chain_string(region_len_chain_string);
		}
		else region_len_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), op_region_op_char_array, name_string_char_array, region_space_char_array, region_offset_char_array, region_len_char_array);
		if(aml_symbol->component.def_op_region.op_region_op)
		{
			delete_chain_string(op_region_op_chain_string);
			free(op_region_op_char_array);
		}
		if(aml_symbol->component.def_op_region.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_op_region.region_space)
		{
			delete_chain_string(region_space_chain_string);
			free(region_space_char_array);
		}
		if(aml_symbol->component.def_op_region.region_offset)
		{
			delete_chain_string(region_offset_chain_string);
			free(region_offset_char_array);
		}
		if(aml_symbol->component.def_op_region.region_len)
		{
			delete_chain_string(region_len_chain_string);
			free(region_len_char_array);
		}
		break;
	case aml_def_package:
		if(aml_symbol->component.def_package.package_op)
		{
			package_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_package.package_op);
			insert_char_front(package_op_chain_string, package_op_chain_string->first_character, ' ');
			replace_chain_string(package_op_chain_string, "\n", "\n ");
			package_op_char_array = create_char_array_from_chain_string(package_op_chain_string);
		}
		else package_op_char_array = "";
		if(aml_symbol->component.def_package.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_package.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_package.num_elements)
		{
			num_elements_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_package.num_elements);
			insert_char_front(num_elements_chain_string, num_elements_chain_string->first_character, ' ');
			replace_chain_string(num_elements_chain_string, "\n", "\n ");
			num_elements_char_array = create_char_array_from_chain_string(num_elements_chain_string);
		}
		else num_elements_char_array = "";
		if(aml_symbol->component.def_package.package_element_list)
		{
			package_element_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_package.package_element_list);
			insert_char_front(package_element_list_chain_string, package_element_list_chain_string->first_character, ' ');
			replace_chain_string(package_element_list_chain_string, "\n", "\n ");
			package_element_list_char_array = create_char_array_from_chain_string(package_element_list_chain_string);
		}
		else package_element_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, package_op_char_array, pkg_length_char_array, num_elements_char_array, package_element_list_char_array);
		if(aml_symbol->component.def_package.package_op)
		{
			delete_chain_string(package_op_chain_string);
			free(package_op_char_array);
		}
		if(aml_symbol->component.def_package.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_package.num_elements)
		{
			delete_chain_string(num_elements_chain_string);
			free(num_elements_char_array);
		}
		if(aml_symbol->component.def_package.package_element_list)
		{
			delete_chain_string(package_element_list_chain_string);
			free(package_element_list_char_array);
		}
		break;
	case aml_def_release:
		if(aml_symbol->component.def_release.release_op)
		{
			release_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_release.release_op);
			insert_char_front(release_op_chain_string, release_op_chain_string->first_character, ' ');
			replace_chain_string(release_op_chain_string, "\n", "\n ");
			release_op_char_array = create_char_array_from_chain_string(release_op_chain_string);
		}
		else release_op_char_array = "";
		if(aml_symbol->component.def_release.mutex_object)
		{
			mutex_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_release.mutex_object);
			insert_char_front(mutex_object_chain_string, mutex_object_chain_string->first_character, ' ');
			replace_chain_string(mutex_object_chain_string, "\n", "\n ");
			mutex_object_char_array = create_char_array_from_chain_string(mutex_object_chain_string);
		}
		else mutex_object_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), release_op_char_array, mutex_object_char_array);
		if(aml_symbol->component.def_release.release_op)
		{
			delete_chain_string(release_op_chain_string);
			free(release_op_char_array);
		}
		if(aml_symbol->component.def_release.mutex_object)
		{
			delete_chain_string(mutex_object_chain_string);
			free(mutex_object_char_array);
		}
		break;
	case aml_def_return:
		if(aml_symbol->component.def_return.return_op)
		{
			return_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_return.return_op);
			insert_char_front(return_op_chain_string, return_op_chain_string->first_character, ' ');
			replace_chain_string(return_op_chain_string, "\n", "\n ");
			return_op_char_array = create_char_array_from_chain_string(return_op_chain_string);
		}
		else return_op_char_array = "";
		if(aml_symbol->component.def_return.arg_object)
		{
			arg_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_return.arg_object);
			insert_char_front(arg_object_chain_string, arg_object_chain_string->first_character, ' ');
			replace_chain_string(arg_object_chain_string, "\n", "\n ");
			arg_object_char_array = create_char_array_from_chain_string(arg_object_chain_string);
		}
		else arg_object_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), return_op_char_array, arg_object_char_array);
		if(aml_symbol->component.def_return.return_op)
		{
			delete_chain_string(return_op_chain_string);
			free(return_op_char_array);
		}
		if(aml_symbol->component.def_return.arg_object)
		{
			delete_chain_string(arg_object_chain_string);
			free(arg_object_char_array);
		}
		break;
	case aml_def_scope:
		if(aml_symbol->component.def_scope.scope_op)
		{
			scope_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_scope.scope_op);
			insert_char_front(scope_op_chain_string, scope_op_chain_string->first_character, ' ');
			replace_chain_string(scope_op_chain_string, "\n", "\n ");
			scope_op_char_array = create_char_array_from_chain_string(scope_op_chain_string);
		}
		else scope_op_char_array = "";
		if(aml_symbol->component.def_scope.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_scope.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_scope.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_scope.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.def_scope.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_scope.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, scope_op_char_array, pkg_length_char_array, name_string_char_array, term_list_char_array);
		if(aml_symbol->component.def_scope.scope_op)
		{
			delete_chain_string(scope_op_chain_string);
			free(scope_op_char_array);
		}
		if(aml_symbol->component.def_scope.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_scope.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.def_scope.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		break;
	case aml_def_shift_left:
		if(aml_symbol->component.def_shift_left.shift_left_op)
		{
			shift_left_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_left.shift_left_op);
			insert_char_front(shift_left_op_chain_string, shift_left_op_chain_string->first_character, ' ');
			replace_chain_string(shift_left_op_chain_string, "\n", "\n ");
			shift_left_op_char_array = create_char_array_from_chain_string(shift_left_op_chain_string);
		}
		else shift_left_op_char_array = "";
		if(aml_symbol->component.def_shift_left.operand)
		{
			operand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_left.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		if(aml_symbol->component.def_shift_left.shift_count)
		{
			shift_count_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_left.shift_count);
			insert_char_front(shift_count_chain_string, shift_count_chain_string->first_character, ' ');
			replace_chain_string(shift_count_chain_string, "\n", "\n ");
			shift_count_char_array = create_char_array_from_chain_string(shift_count_chain_string);
		}
		else shift_count_char_array = "";
		if(aml_symbol->component.def_shift_left.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_left.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), shift_left_op_char_array, operand_char_array, shift_count_char_array, target_char_array);
		if(aml_symbol->component.def_shift_left.shift_left_op)
		{
			delete_chain_string(shift_left_op_chain_string);
			free(shift_left_op_char_array);
		}
		if(aml_symbol->component.def_shift_left.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		if(aml_symbol->component.def_shift_left.shift_count)
		{
			delete_chain_string(shift_count_chain_string);
			free(shift_count_char_array);
		}
		if(aml_symbol->component.def_shift_left.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		break;
	case aml_def_shift_right:
		if(aml_symbol->component.def_shift_right.shift_right_op)
		{
			shift_right_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_right.shift_right_op);
			insert_char_front(shift_right_op_chain_string, shift_right_op_chain_string->first_character, ' ');
			replace_chain_string(shift_right_op_chain_string, "\n", "\n ");
			shift_right_op_char_array = create_char_array_from_chain_string(shift_right_op_chain_string);
		}
		else shift_right_op_char_array = "";
		if(aml_symbol->component.def_shift_right.operand)
		{
			operand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_right.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		if(aml_symbol->component.def_shift_right.shift_count)
		{
			shift_count_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_right.shift_count);
			insert_char_front(shift_count_chain_string, shift_count_chain_string->first_character, ' ');
			replace_chain_string(shift_count_chain_string, "\n", "\n ");
			shift_count_char_array = create_char_array_from_chain_string(shift_count_chain_string);
		}
		else shift_count_char_array = "";
		if(aml_symbol->component.def_shift_right.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_shift_right.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), shift_right_op_char_array, operand_char_array, shift_count_char_array, target_char_array);
		if(aml_symbol->component.def_shift_right.shift_right_op)
		{
			delete_chain_string(shift_right_op_chain_string);
			free(shift_right_op_char_array);
		}
		if(aml_symbol->component.def_shift_right.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		if(aml_symbol->component.def_shift_right.shift_count)
		{
			delete_chain_string(shift_count_chain_string);
			free(shift_count_char_array);
		}
		if(aml_symbol->component.def_shift_right.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		break;
	case aml_def_size_of:
		if(aml_symbol->component.def_size_of.size_of_op)
		{
			size_of_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_size_of.size_of_op);
			insert_char_front(size_of_op_chain_string, size_of_op_chain_string->first_character, ' ');
			replace_chain_string(size_of_op_chain_string, "\n", "\n ");
			size_of_op_char_array = create_char_array_from_chain_string(size_of_op_chain_string);
		}
		else size_of_op_char_array = "";
		if(aml_symbol->component.def_size_of.super_name)
		{
			super_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_size_of.super_name);
			insert_char_front(super_name_chain_string, super_name_chain_string->first_character, ' ');
			replace_chain_string(super_name_chain_string, "\n", "\n ");
			super_name_char_array = create_char_array_from_chain_string(super_name_chain_string);
		}
		else super_name_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), size_of_op_char_array, super_name_char_array);
		if(aml_symbol->component.def_size_of.size_of_op)
		{
			delete_chain_string(size_of_op_chain_string);
			free(size_of_op_char_array);
		}
		if(aml_symbol->component.def_size_of.super_name)
		{
			delete_chain_string(super_name_chain_string);
			free(super_name_char_array);
		}
		break;
	case aml_def_store:
		if(aml_symbol->component.def_store.store_op)
		{
			store_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_store.store_op);
			insert_char_front(store_op_chain_string, store_op_chain_string->first_character, ' ');
			replace_chain_string(store_op_chain_string, "\n", "\n ");
			store_op_char_array = create_char_array_from_chain_string(store_op_chain_string);
		}
		else store_op_char_array = "";
		if(aml_symbol->component.def_store.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_store.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		if(aml_symbol->component.def_store.super_name)
		{
			super_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_store.super_name);
			insert_char_front(super_name_chain_string, super_name_chain_string->first_character, ' ');
			replace_chain_string(super_name_chain_string, "\n", "\n ");
			super_name_char_array = create_char_array_from_chain_string(super_name_chain_string);
		}
		else super_name_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), store_op_char_array, term_arg_char_array, super_name_char_array);
		if(aml_symbol->component.def_store.store_op)
		{
			delete_chain_string(store_op_chain_string);
			free(store_op_char_array);
		}
		if(aml_symbol->component.def_store.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		if(aml_symbol->component.def_store.super_name)
		{
			delete_chain_string(super_name_chain_string);
			free(super_name_char_array);
		}
		break;
	case aml_def_subtract:
		operands_chain_string = malloc(_countof(aml_symbol->component.def_subtract.operand) * sizeof(*operands_chain_string));
		operands_char_array = malloc(_countof(aml_symbol->component.def_subtract.operand) * sizeof(*operands_char_array));
		if(aml_symbol->component.def_subtract.subtract_op)
		{
			subtract_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_subtract.subtract_op);
			insert_char_front(subtract_op_chain_string, subtract_op_chain_string->first_character, ' ');
			replace_chain_string(subtract_op_chain_string, "\n", "\n ");
			subtract_op_char_array = create_char_array_from_chain_string(subtract_op_chain_string);
		}
		else subtract_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_subtract.operand); i++)if(aml_symbol->component.def_subtract.operand[i])
		{
			operands_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_subtract.operand[i]);
			insert_char_front(operands_chain_string[i], operands_chain_string[i]->first_character, ' ');
			replace_chain_string(operands_chain_string[i], "\n", "\n ");
			operands_char_array[i] = create_char_array_from_chain_string(operands_chain_string[i]);
		}
		else operands_char_array[i] = "";
		if(aml_symbol->component.def_subtract.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_subtract.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), subtract_op_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_subtract.operand); i++)insert_char_array_back(output, output->last_character, operands_char_array[i]);
		insert_char_array_back(output, output->last_character, target_char_array);
		if(aml_symbol->component.def_subtract.subtract_op)
		{
			delete_chain_string(subtract_op_chain_string);
			free(subtract_op_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_subtract.operand); i++)if(aml_symbol->component.def_subtract.operand[i])
		{
			delete_chain_string(operands_chain_string[i]);
			free(operands_char_array[i]);
		}
		if(aml_symbol->component.def_subtract.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		free(operands_chain_string);
		free(operands_char_array);
		break;
	case aml_def_to_buffer:
		if(aml_symbol->component.def_to_buffer.to_buffer_op)
		{
			to_buffer_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_buffer.to_buffer_op);
			insert_char_front(to_buffer_op_chain_string, to_buffer_op_chain_string->first_character, ' ');
			replace_chain_string(to_buffer_op_chain_string, "\n", "\n ");
			to_buffer_op_char_array = create_char_array_from_chain_string(to_buffer_op_chain_string);
		}
		else to_buffer_op_char_array = "";
		if(aml_symbol->component.def_to_buffer.operand)
		{
			operand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_buffer.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		if(aml_symbol->component.def_to_buffer.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_buffer.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), to_buffer_op_char_array, operand_char_array, target_char_array);
		if(aml_symbol->component.def_to_buffer.to_buffer_op)
		{
			delete_chain_string(to_buffer_op_chain_string);
			free(to_buffer_op_char_array);
		}
		if(aml_symbol->component.def_to_buffer.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		if(aml_symbol->component.def_to_buffer.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		break;
	case aml_def_to_hex_string:
		if(aml_symbol->component.def_to_hex_string.to_hex_string_op)
		{
			to_hex_string_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_hex_string.to_hex_string_op);
			insert_char_front(to_hex_string_op_chain_string, to_hex_string_op_chain_string->first_character, ' ');
			replace_chain_string(to_hex_string_op_chain_string, "\n", "\n ");
			to_hex_string_op_char_array = create_char_array_from_chain_string(to_hex_string_op_chain_string);
		}
		else to_hex_string_op_char_array = "";
		if(aml_symbol->component.def_to_hex_string.operand)
		{
			operand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_hex_string.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		if(aml_symbol->component.def_to_hex_string.target)
		{
			target_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_to_hex_string.target);
			insert_char_front(target_chain_string, target_chain_string->first_character, ' ');
			replace_chain_string(target_chain_string, "\n", "\n ");
			target_char_array = create_char_array_from_chain_string(target_chain_string);
		}
		else target_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), to_hex_string_op_char_array, operand_char_array, target_char_array);
		if(aml_symbol->component.def_to_hex_string.to_hex_string_op)
		{
			delete_chain_string(to_hex_string_op_chain_string);
			free(to_hex_string_op_char_array);
		}
		if(aml_symbol->component.def_to_hex_string.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		if(aml_symbol->component.def_to_hex_string.target)
		{
			delete_chain_string(target_chain_string);
			free(target_char_array);
		}
		break;
	case aml_def_while:
		if(aml_symbol->component.def_while.while_op)
		{
			while_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_while.while_op);
			insert_char_front(while_op_chain_string, while_op_chain_string->first_character, ' ');
			replace_chain_string(while_op_chain_string, "\n", "\n ");
			while_op_char_array = create_char_array_from_chain_string(while_op_chain_string);
		}
		else while_op_char_array = "";
		if(aml_symbol->component.def_while.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_while.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		if(aml_symbol->component.def_while.predicate)
		{
			predicate_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_while.predicate);
			insert_char_front(predicate_chain_string, predicate_chain_string->first_character, ' ');
			replace_chain_string(predicate_chain_string, "\n", "\n ");
			predicate_char_array = create_char_array_from_chain_string(predicate_chain_string);
		}
		else predicate_char_array = "";
		if(aml_symbol->component.def_while.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_while.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, while_op_char_array, pkg_length_char_array, predicate_char_array, term_list_char_array);
		if(aml_symbol->component.def_while.while_op)
		{
			delete_chain_string(while_op_chain_string);
			free(while_op_char_array);
		}
		if(aml_symbol->component.def_while.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		if(aml_symbol->component.def_while.predicate)
		{
			delete_chain_string(predicate_chain_string);
			free(predicate_char_array);
		}
		if(aml_symbol->component.def_while.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		break;
	case aml_deref_of_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_device_op:
		if(aml_symbol->component.device_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.device_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.device_op.device_op_suffix)
		{
			device_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.device_op.device_op_suffix);
			insert_char_front(device_op_suffix_chain_string, device_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(device_op_suffix_chain_string, "\n", "\n ");
			device_op_suffix_char_array = create_char_array_from_chain_string(device_op_suffix_chain_string);
		}
		else device_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, device_op_suffix_char_array);
		if(aml_symbol->component.device_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.device_op.device_op_suffix)
		{
			delete_chain_string(device_op_suffix_chain_string);
			free(device_op_suffix_char_array);
		}
		break;
	case aml_device_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_digit_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_dual_name_path:
		name_segs_chain_string = malloc(_countof(aml_symbol->component.dual_name_path.name_seg) * sizeof(*name_segs_chain_string));
		name_segs_char_array = malloc(_countof(aml_symbol->component.dual_name_path.name_seg) * sizeof(*name_segs_char_array));
		if(aml_symbol->component.dual_name_path.dual_name_prefix)
		{
			dual_name_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.dual_name_path.dual_name_prefix);
			insert_char_front(dual_name_prefix_chain_string, dual_name_prefix_chain_string->first_character, ' ');
			replace_chain_string(dual_name_prefix_chain_string, "\n", "\n ");
			dual_name_prefix_char_array = create_char_array_from_chain_string(dual_name_prefix_chain_string);
		}
		else dual_name_prefix_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dual_name_path.name_seg); i++)if(aml_symbol->component.dual_name_path.name_seg[i])
		{
			name_segs_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.dual_name_path.name_seg[i]);
			insert_char_front(name_segs_chain_string[i], name_segs_chain_string[i]->first_character, ' ');
			replace_chain_string(name_segs_chain_string[i], "\n", "\n ");
			name_segs_char_array[i] = create_char_array_from_chain_string(name_segs_chain_string[i]);
		}
		else name_segs_char_array[i] = "";
		output = create_format_chain_string("%s \"%s\"\n%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.dual_name_path.string, dual_name_prefix_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dual_name_path.name_seg); i++)if(aml_symbol->component.dual_name_path.name_seg[i])insert_char_array_back(output, output->last_character, name_segs_char_array[i]);
		if(aml_symbol->component.dual_name_path.dual_name_prefix)
		{
			delete_chain_string(dual_name_prefix_chain_string);
			free(dual_name_prefix_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dual_name_path.name_seg); i++)if(aml_symbol->component.dual_name_path.name_seg[i])
		{
			delete_chain_string(name_segs_chain_string[i]);
			free(name_segs_char_array[i]);
		}
		free(name_segs_chain_string);
		free(name_segs_char_array);
		break;
	case aml_dual_name_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_dword_const:
		if(aml_symbol->component.dword_const.dword_prefix)
		{
			dword_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.dword_const.dword_prefix);
			insert_char_front(dword_prefix_chain_string, dword_prefix_chain_string->first_character, ' ');
			replace_chain_string(dword_prefix_chain_string, "\n", "\n ");
			dword_prefix_char_array = create_char_array_from_chain_string(dword_prefix_chain_string);
		}
		else dword_prefix_char_array = "";
		if(aml_symbol->component.dword_const.dword_data)
		{
			dword_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.dword_const.dword_data);
			insert_char_front(dword_data_chain_string, dword_data_chain_string->first_character, ' ');
			replace_chain_string(dword_data_chain_string, "\n", "\n ");
			dword_data_char_array = create_char_array_from_chain_string(dword_data_chain_string);
		}
		else dword_prefix_char_array = "";
		output = create_format_chain_string("%s %#010.8x\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.dword_const.value, dword_prefix_char_array, dword_data_char_array);
		if(aml_symbol->component.dword_const.dword_prefix)
		{
			delete_chain_string(dword_prefix_chain_string);
			free(dword_prefix_char_array);
		}
		if(aml_symbol->component.dword_const.dword_data)
		{
			delete_chain_string(dword_data_chain_string);
			free(dword_data_char_array);
		}
		break;
	case aml_dword_data:
		words_data_chain_string = malloc(_countof(aml_symbol->component.dword_data.word_data) * sizeof(*words_data_chain_string));
		words_data_char_array = malloc(_countof(aml_symbol->component.dword_data.word_data) * sizeof(*words_data_char_array));
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dword_data.word_data); i++)if(aml_symbol->component.dword_data.word_data)
		{
			words_data_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.dword_data.word_data[i]);
			insert_char_front(words_data_chain_string[i], words_data_chain_string[i]->first_character, ' ');
			replace_chain_string(words_data_chain_string[i], "\n", "\n ");
			words_data_char_array[i] = create_char_array_from_chain_string(words_data_chain_string[i]);
		}
		else words_data_char_array[i] = "";
		output = create_format_chain_string("%s %#010.8x\n", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.dword_data.value);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dword_data.word_data); i++)insert_char_array_back(output, output->last_character, words_data_char_array[i]);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dword_data.word_data); i++)if(aml_symbol->component.dword_data.word_data[i])
		{
			delete_chain_string(words_data_chain_string[i]);
			free(words_data_char_array[i]);
		}
		free(words_data_chain_string);
		free(words_data_char_array);
		break;
	case aml_dword_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_expression_opcode:
		if(aml_symbol->component.expression_opcode.def_add)
		{
			def_add_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_add);
			insert_char_front(def_add_chain_string, def_add_chain_string->first_character, ' ');
			replace_chain_string(def_add_chain_string, "\n", "\n ");
			def_add_char_array = create_char_array_from_chain_string(def_add_chain_string);
		}
		else def_add_char_array = "";
		if(aml_symbol->component.expression_opcode.def_and)
		{
			def_and_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_and);
			insert_char_front(def_and_chain_string, def_and_chain_string->first_character, ' ');
			replace_chain_string(def_and_chain_string, "\n", "\n ");
			def_and_char_array = create_char_array_from_chain_string(def_and_chain_string);
		}
		else def_and_char_array = "";
		if(aml_symbol->component.expression_opcode.def_acquire)
		{
			def_acquire_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_acquire);
			insert_char_front(def_acquire_chain_string, def_acquire_chain_string->first_character, ' ');
			replace_chain_string(def_acquire_chain_string, "\n", "\n ");
			def_acquire_char_array = create_char_array_from_chain_string(def_acquire_chain_string);
		}
		else def_acquire_char_array = "";
		if(aml_symbol->component.expression_opcode.def_buffer)
		{
			def_buffer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_buffer);
			insert_char_front(def_buffer_chain_string, def_buffer_chain_string->first_character, ' ');
			replace_chain_string(def_buffer_chain_string, "\n", "\n ");
			def_buffer_char_array = create_char_array_from_chain_string(def_buffer_chain_string);
		}
		else def_buffer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_concat)
		{
			def_concat_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_concat);
			insert_char_front(def_concat_chain_string, def_concat_chain_string->first_character, ' ');
			replace_chain_string(def_concat_chain_string, "\n", "\n ");
			def_concat_char_array = create_char_array_from_chain_string(def_concat_chain_string);
		}
		else def_concat_char_array = "";
		if(aml_symbol->component.expression_opcode.def_concat_res)
		{
			def_concat_res_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_concat_res);
			insert_char_front(def_concat_res_chain_string, def_concat_res_chain_string->first_character, ' ');
			replace_chain_string(def_concat_res_chain_string, "\n", "\n ");
			def_concat_res_char_array = create_char_array_from_chain_string(def_concat_res_chain_string);
		}
		else def_concat_res_char_array = "";
		if(aml_symbol->component.expression_opcode.def_cond_ref_of)
		{
			def_cond_ref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_cond_ref_of);
			insert_char_front(def_cond_ref_of_chain_string, def_cond_ref_of_chain_string->first_character, ' ');
			replace_chain_string(def_cond_ref_of_chain_string, "\n", "\n ");
			def_cond_ref_of_char_array = create_char_array_from_chain_string(def_cond_ref_of_chain_string);
		}
		else def_cond_ref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_copy_object)
		{
			def_copy_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_copy_object);
			insert_char_front(def_copy_object_chain_string, def_copy_object_chain_string->first_character, ' ');
			replace_chain_string(def_copy_object_chain_string, "\n", "\n ");
			def_copy_object_char_array = create_char_array_from_chain_string(def_copy_object_chain_string);
		}
		else def_copy_object_char_array = "";
		if(aml_symbol->component.expression_opcode.def_decrement)
		{
			def_decrement_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_decrement);
			insert_char_front(def_decrement_chain_string, def_decrement_chain_string->first_character, ' ');
			replace_chain_string(def_decrement_chain_string, "\n", "\n ");
			def_decrement_char_array = create_char_array_from_chain_string(def_decrement_chain_string);
		}
		else def_decrement_char_array = "";
		if(aml_symbol->component.expression_opcode.def_deref_of)
		{
			def_deref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_deref_of);
			insert_char_front(def_deref_of_chain_string, def_deref_of_chain_string->first_character, ' ');
			replace_chain_string(def_deref_of_chain_string, "\n", "\n ");
			def_deref_of_char_array = create_char_array_from_chain_string(def_deref_of_chain_string);
		}
		else def_deref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_divide)
		{
			def_divide_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_divide);
			insert_char_front(def_divide_chain_string, def_divide_chain_string->first_character, ' ');
			replace_chain_string(def_divide_chain_string, "\n", "\n ");
			def_divide_char_array = create_char_array_from_chain_string(def_divide_chain_string);
		}
		else def_divide_char_array = "";
		if(aml_symbol->component.expression_opcode.def_find_set_left_bit)
		{
			def_find_set_left_bit_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_find_set_left_bit);
			insert_char_front(def_find_set_left_bit_chain_string, def_find_set_left_bit_chain_string->first_character, ' ');
			replace_chain_string(def_find_set_left_bit_chain_string, "\n", "\n ");
			def_find_set_left_bit_char_array = create_char_array_from_chain_string(def_find_set_left_bit_chain_string);
		}
		else def_find_set_left_bit_char_array = "";
		if(aml_symbol->component.expression_opcode.def_find_set_right_bit)
		{
			def_find_set_right_bit_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_find_set_right_bit);
			insert_char_front(def_find_set_right_bit_chain_string, def_find_set_right_bit_chain_string->first_character, ' ');
			replace_chain_string(def_find_set_right_bit_chain_string, "\n", "\n ");
			def_find_set_right_bit_char_array = create_char_array_from_chain_string(def_find_set_right_bit_chain_string);
		}
		else def_find_set_right_bit_char_array = "";
		if(aml_symbol->component.expression_opcode.def_from_bcd)
		{
			def_from_bcd_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_from_bcd);
			insert_char_front(def_from_bcd_chain_string, def_from_bcd_chain_string->first_character, ' ');
			replace_chain_string(def_from_bcd_chain_string, "\n", "\n ");
			def_from_bcd_char_array = create_char_array_from_chain_string(def_from_bcd_chain_string);
		}
		else def_from_bcd_char_array = "";
		if(aml_symbol->component.expression_opcode.def_increment)
		{
			def_increment_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_increment);
			insert_char_front(def_increment_chain_string, def_increment_chain_string->first_character, ' ');
			replace_chain_string(def_increment_chain_string, "\n", "\n ");
			def_increment_char_array = create_char_array_from_chain_string(def_increment_chain_string);
		}
		else def_increment_char_array = "";
		if(aml_symbol->component.expression_opcode.def_index)
		{
			def_index_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_index);
			insert_char_front(def_index_chain_string, def_index_chain_string->first_character, ' ');
			replace_chain_string(def_index_chain_string, "\n", "\n ");
			def_index_char_array = create_char_array_from_chain_string(def_index_chain_string);
		}
		else def_index_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_and)
		{
			def_l_and_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_and);
			insert_char_front(def_l_and_chain_string, def_l_and_chain_string->first_character, ' ');
			replace_chain_string(def_l_and_chain_string, "\n", "\n ");
			def_l_and_char_array = create_char_array_from_chain_string(def_l_and_chain_string);
		}
		else def_l_and_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_equal)
		{
			def_l_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_equal);
			insert_char_front(def_l_equal_chain_string, def_l_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_equal_chain_string, "\n", "\n ");
			def_l_equal_char_array = create_char_array_from_chain_string(def_l_equal_chain_string);
		}
		else def_l_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_greater)
		{
			def_l_greater_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_greater);
			insert_char_front(def_l_greater_chain_string, def_l_greater_chain_string->first_character, ' ');
			replace_chain_string(def_l_greater_chain_string, "\n", "\n ");
			def_l_greater_char_array = create_char_array_from_chain_string(def_l_greater_chain_string);
		}
		else def_l_greater_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_greater_equal)
		{
			def_l_greater_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_greater_equal);
			insert_char_front(def_l_greater_equal_chain_string, def_l_greater_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_greater_equal_chain_string, "\n", "\n ");
			def_l_greater_equal_char_array = create_char_array_from_chain_string(def_l_greater_equal_chain_string);
		}
		else def_l_greater_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_less)
		{
			def_l_less_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_less);
			insert_char_front(def_l_less_chain_string, def_l_less_chain_string->first_character, ' ');
			replace_chain_string(def_l_less_chain_string, "\n", "\n ");
			def_l_less_char_array = create_char_array_from_chain_string(def_l_less_chain_string);
		}
		else def_l_less_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_less_equal)
		{
			def_l_less_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_less_equal);
			insert_char_front(def_l_less_equal_chain_string, def_l_less_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_less_equal_chain_string, "\n", "\n ");
			def_l_less_equal_char_array = create_char_array_from_chain_string(def_l_less_equal_chain_string);
		}
		else def_l_less_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_not)
		{
			def_l_not_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_not);
			insert_char_front(def_l_not_chain_string, def_l_not_chain_string->first_character, ' ');
			replace_chain_string(def_l_not_chain_string, "\n", "\n ");
			def_l_not_char_array = create_char_array_from_chain_string(def_l_not_chain_string);
		}
		else def_l_not_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_not_equal)
		{
			def_l_not_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_not_equal);
			insert_char_front(def_l_not_equal_chain_string, def_l_not_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_not_equal_chain_string, "\n", "\n ");
			def_l_not_equal_char_array = create_char_array_from_chain_string(def_l_not_equal_chain_string);
		}
		else def_l_not_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_or)
		{
			def_l_or_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_or);
			insert_char_front(def_l_or_chain_string, def_l_or_chain_string->first_character, ' ');
			replace_chain_string(def_l_or_chain_string, "\n", "\n ");
			def_l_or_char_array = create_char_array_from_chain_string(def_l_or_chain_string);
		}
		else def_l_or_char_array = "";
		if(aml_symbol->component.expression_opcode.def_load_table)
		{
			def_load_table_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_load_table);
			insert_char_front(def_load_table_chain_string, def_load_table_chain_string->first_character, ' ');
			replace_chain_string(def_load_table_chain_string, "\n", "\n ");
			def_load_table_char_array = create_char_array_from_chain_string(def_load_table_chain_string);
		}
		else def_load_table_char_array = "";
		if(aml_symbol->component.expression_opcode.def_match)
		{
			def_match_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_match);
			insert_char_front(def_match_chain_string, def_match_chain_string->first_character, ' ');
			replace_chain_string(def_match_chain_string, "\n", "\n ");
			def_match_char_array = create_char_array_from_chain_string(def_match_chain_string);
		}
		else def_match_char_array = "";
		if(aml_symbol->component.expression_opcode.def_mid)
		{
			def_mid_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_mid);
			insert_char_front(def_mid_chain_string, def_mid_chain_string->first_character, ' ');
			replace_chain_string(def_mid_chain_string, "\n", "\n ");
			def_mid_char_array = create_char_array_from_chain_string(def_mid_chain_string);
		}
		else def_mid_char_array = "";
		if(aml_symbol->component.expression_opcode.def_mod)
		{
			def_mod_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_mod);
			insert_char_front(def_mod_chain_string, def_mod_chain_string->first_character, ' ');
			replace_chain_string(def_mod_chain_string, "\n", "\n ");
			def_mod_char_array = create_char_array_from_chain_string(def_mod_chain_string);
		}
		else def_mod_char_array = "";
		if(aml_symbol->component.expression_opcode.def_multiply)
		{
			def_multiply_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_multiply);
			insert_char_front(def_multiply_chain_string, def_multiply_chain_string->first_character, ' ');
			replace_chain_string(def_multiply_chain_string, "\n", "\n ");
			def_multiply_char_array = create_char_array_from_chain_string(def_multiply_chain_string);
		}
		else def_multiply_char_array = "";
		if(aml_symbol->component.expression_opcode.def_nand)
		{
			def_nand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_nand);
			insert_char_front(def_nand_chain_string, def_nand_chain_string->first_character, ' ');
			replace_chain_string(def_nand_chain_string, "\n", "\n ");
			def_nand_char_array = create_char_array_from_chain_string(def_nand_chain_string);
		}
		else def_nand_char_array = "";
		if(aml_symbol->component.expression_opcode.def_nor)
		{
			def_nor_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_nor);
			insert_char_front(def_nor_chain_string, def_nor_chain_string->first_character, ' ');
			replace_chain_string(def_nor_chain_string, "\n", "\n ");
			def_nor_char_array = create_char_array_from_chain_string(def_nor_chain_string);
		}
		else def_nor_char_array = "";
		if(aml_symbol->component.expression_opcode.def_not)
		{
			def_not_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_not);
			insert_char_front(def_not_chain_string, def_not_chain_string->first_character, ' ');
			replace_chain_string(def_not_chain_string, "\n", "\n ");
			def_not_char_array = create_char_array_from_chain_string(def_not_chain_string);
		}
		else def_not_char_array = "";
		if(aml_symbol->component.expression_opcode.def_object_type)
		{
			def_object_type_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_object_type);
			insert_char_front(def_object_type_chain_string, def_object_type_chain_string->first_character, ' ');
			replace_chain_string(def_object_type_chain_string, "\n", "\n ");
			def_object_type_char_array = create_char_array_from_chain_string(def_object_type_chain_string);
		}
		else def_object_type_char_array = "";
		if(aml_symbol->component.expression_opcode.def_or)
		{
			def_or_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_or);
			insert_char_front(def_or_chain_string, def_or_chain_string->first_character, ' ');
			replace_chain_string(def_or_chain_string, "\n", "\n ");
			def_or_char_array = create_char_array_from_chain_string(def_or_chain_string);
		}
		else def_or_char_array = "";
		if(aml_symbol->component.expression_opcode.def_package)
		{
			def_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_package);
			insert_char_front(def_package_chain_string, def_package_chain_string->first_character, ' ');
			replace_chain_string(def_package_chain_string, "\n", "\n ");
			def_package_char_array = create_char_array_from_chain_string(def_package_chain_string);
		}
		else def_package_char_array = "";
		if(aml_symbol->component.expression_opcode.def_ref_of)
		{
			def_ref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_ref_of);
			insert_char_front(def_ref_of_chain_string, def_ref_of_chain_string->first_character, ' ');
			replace_chain_string(def_ref_of_chain_string, "\n", "\n ");
			def_ref_of_char_array = create_char_array_from_chain_string(def_ref_of_chain_string);
		}
		else def_ref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_shift_left)
		{
			def_shift_left_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_shift_left);
			insert_char_front(def_shift_left_chain_string, def_shift_left_chain_string->first_character, ' ');
			replace_chain_string(def_shift_left_chain_string, "\n", "\n ");
			def_shift_left_char_array = create_char_array_from_chain_string(def_shift_left_chain_string);
		}
		else def_shift_left_char_array = "";
		if(aml_symbol->component.expression_opcode.def_shift_right)
		{
			def_shift_right_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_shift_right);
			insert_char_front(def_shift_right_chain_string, def_shift_right_chain_string->first_character, ' ');
			replace_chain_string(def_shift_right_chain_string, "\n", "\n ");
			def_shift_right_char_array = create_char_array_from_chain_string(def_shift_right_chain_string);
		}
		else def_shift_right_char_array = "";
		if(aml_symbol->component.expression_opcode.def_size_of)
		{
			def_size_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_size_of);
			insert_char_front(def_size_of_chain_string, def_size_of_chain_string->first_character, ' ');
			replace_chain_string(def_size_of_chain_string, "\n", "\n ");
			def_size_of_char_array = create_char_array_from_chain_string(def_size_of_chain_string);
		}
		else def_size_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_store)
		{
			def_store_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_store);
			insert_char_front(def_store_chain_string, def_store_chain_string->first_character, ' ');
			replace_chain_string(def_store_chain_string, "\n", "\n ");
			def_store_char_array = create_char_array_from_chain_string(def_store_chain_string);
		}
		else def_store_char_array = "";
		if(aml_symbol->component.expression_opcode.def_subtract)
		{
			def_subtract_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_subtract);
			insert_char_front(def_subtract_chain_string, def_subtract_chain_string->first_character, ' ');
			replace_chain_string(def_subtract_chain_string, "\n", "\n ");
			def_subtract_char_array = create_char_array_from_chain_string(def_subtract_chain_string);
		}
		else def_subtract_char_array = "";
		if(aml_symbol->component.expression_opcode.def_timer)
		{
			def_timer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_timer);
			insert_char_front(def_timer_chain_string, def_timer_chain_string->first_character, ' ');
			replace_chain_string(def_timer_chain_string, "\n", "\n ");
			def_timer_char_array = create_char_array_from_chain_string(def_timer_chain_string);
		}
		else def_timer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_bcd)
		{
			def_to_bcd_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_bcd);
			insert_char_front(def_to_bcd_chain_string, def_to_bcd_chain_string->first_character, ' ');
			replace_chain_string(def_to_bcd_chain_string, "\n", "\n ");
			def_to_bcd_char_array = create_char_array_from_chain_string(def_to_bcd_chain_string);
		}
		else def_to_bcd_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_buffer)
		{
			def_to_buffer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_buffer);
			insert_char_front(def_to_buffer_chain_string, def_to_buffer_chain_string->first_character, ' ');
			replace_chain_string(def_to_buffer_chain_string, "\n", "\n ");
			def_to_buffer_char_array = create_char_array_from_chain_string(def_to_buffer_chain_string);
		}
		else def_to_buffer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_decimal_string)
		{
			def_to_decimal_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_decimal_string);
			insert_char_front(def_to_decimal_string_chain_string, def_to_decimal_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_decimal_string_chain_string, "\n", "\n ");
			def_to_decimal_string_char_array = create_char_array_from_chain_string(def_to_decimal_string_chain_string);
		}
		else def_to_decimal_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_hex_string)
		{
			def_to_hex_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_hex_string);
			insert_char_front(def_to_hex_string_chain_string, def_to_hex_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_hex_string_chain_string, "\n", "\n ");
			def_to_hex_string_char_array = create_char_array_from_chain_string(def_to_hex_string_chain_string);
		}
		else def_to_hex_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_integer)
		{
			def_to_integer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_integer);
			insert_char_front(def_to_integer_chain_string, def_to_integer_chain_string->first_character, ' ');
			replace_chain_string(def_to_integer_chain_string, "\n", "\n ");
			def_to_integer_char_array = create_char_array_from_chain_string(def_to_integer_chain_string);
		}
		else def_to_integer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_string)
		{
			def_to_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_string);
			insert_char_front(def_to_string_chain_string, def_to_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_string_chain_string, "\n", "\n ");
			def_to_string_char_array = create_char_array_from_chain_string(def_to_string_chain_string);
		}
		else def_to_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_var_package)
		{
			def_var_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_var_package);
			insert_char_front(def_var_package_chain_string, def_var_package_chain_string->first_character, ' ');
			replace_chain_string(def_var_package_chain_string, "\n", "\n ");
			def_var_package_char_array = create_char_array_from_chain_string(def_var_package_chain_string);
		}
		else def_var_package_char_array = "";
		if(aml_symbol->component.expression_opcode.def_wait)
		{
			def_wait_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_wait);
			insert_char_front(def_wait_chain_string, def_wait_chain_string->first_character, ' ');
			replace_chain_string(def_wait_chain_string, "\n", "\n ");
			def_wait_char_array = create_char_array_from_chain_string(def_wait_chain_string);
		}
		else def_wait_char_array = "";
		if(aml_symbol->component.expression_opcode.def_xor)
		{
			def_xor_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_xor);
			insert_char_front(def_xor_chain_string, def_xor_chain_string->first_character, ' ');
			replace_chain_string(def_xor_chain_string, "\n", "\n ");
			def_xor_char_array = create_char_array_from_chain_string(def_xor_chain_string);
		}
		else def_xor_char_array = "";
		if(aml_symbol->component.expression_opcode.method_invocation)
		{
			method_invocation_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.method_invocation);
			insert_char_front(method_invocation_chain_string, method_invocation_chain_string->first_character, ' ');
			replace_chain_string(method_invocation_chain_string, "\n", "\n ");
			method_invocation_char_array = create_char_array_from_chain_string(method_invocation_chain_string);
		}
		else method_invocation_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), def_add_char_array, def_and_char_array, def_acquire_char_array, def_buffer_char_array, def_concat_char_array, def_concat_res_char_array, def_cond_ref_of_char_array, def_copy_object_char_array, def_decrement_char_array, def_deref_of_char_array, def_divide_char_array, def_find_set_left_bit_char_array, def_find_set_right_bit_char_array, def_from_bcd_char_array, def_increment_char_array, def_index_char_array, def_l_and_char_array, def_l_equal_char_array, def_l_greater_char_array, def_l_greater_equal_char_array, def_l_less_char_array, def_l_less_equal_char_array, def_l_not_char_array, def_l_not_equal_char_array, def_l_or_char_array, def_load_table_char_array, def_match_char_array, def_mid_char_array, def_mod_char_array, def_multiply_char_array, def_nand_char_array, def_nor_char_array, def_not_char_array, def_object_type_char_array, def_or_char_array, def_package_char_array, def_ref_of_char_array, def_shift_left_char_array, def_shift_right_char_array, def_size_of_char_array, def_store_char_array, def_subtract_char_array, def_timer_char_array, def_to_bcd_char_array, def_to_buffer_char_array, def_to_decimal_string_char_array, def_to_hex_string_char_array, def_to_integer_char_array, def_to_string_char_array, def_var_package_char_array, def_wait_char_array, def_xor_char_array, method_invocation_char_array);
		if(aml_symbol->component.expression_opcode.def_add)
		{
			delete_chain_string(def_add_chain_string);
			free(def_add_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_and)
		{
			delete_chain_string(def_and_chain_string);
			free(def_and_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_acquire)
		{
			delete_chain_string(def_acquire_chain_string);
			free(def_acquire_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_buffer)
		{
			delete_chain_string(def_buffer_chain_string);
			free(def_buffer_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_concat)
		{
			delete_chain_string(def_concat_chain_string);
			free(def_concat_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_concat_res)
		{
			delete_chain_string(def_concat_res_chain_string);
			free(def_concat_res_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_cond_ref_of)
		{
			delete_chain_string(def_cond_ref_of_chain_string);
			free(def_cond_ref_of_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_copy_object)
		{
			delete_chain_string(def_copy_object_chain_string);
			free(def_copy_object_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_decrement)
		{
			delete_chain_string(def_decrement_chain_string);
			free(def_decrement_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_deref_of)
		{
			delete_chain_string(def_deref_of_chain_string);
			free(def_deref_of_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_divide)
		{
			delete_chain_string(def_divide_chain_string);
			free(def_divide_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_find_set_left_bit)
		{
			delete_chain_string(def_find_set_left_bit_chain_string);
			free(def_find_set_left_bit_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_find_set_right_bit)
		{
			delete_chain_string(def_find_set_right_bit_chain_string);
			free(def_find_set_right_bit_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_from_bcd)
		{
			delete_chain_string(def_from_bcd_chain_string);
			free(def_from_bcd_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_increment)
		{
			delete_chain_string(def_increment_chain_string);
			free(def_increment_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_index)
		{
			delete_chain_string(def_index_chain_string);
			free(def_index_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_and)
		{
			delete_chain_string(def_l_and_chain_string);
			free(def_l_and_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_equal)
		{
			delete_chain_string(def_l_equal_chain_string);
			free(def_l_equal_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_greater)
		{
			delete_chain_string(def_l_greater_chain_string);
			free(def_l_greater_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_greater_equal)
		{
			delete_chain_string(def_l_greater_equal_chain_string);
			free(def_l_greater_equal_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_less)
		{
			delete_chain_string(def_l_less_chain_string);
			free(def_l_less_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_less_equal)
		{
			delete_chain_string(def_l_less_equal_chain_string);
			free(def_l_less_equal_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_not)
		{
			delete_chain_string(def_l_not_chain_string);
			free(def_l_not_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_not_equal)
		{
			delete_chain_string(def_l_not_equal_chain_string);
			free(def_l_not_equal_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_l_or)
		{
			delete_chain_string(def_l_or_chain_string);
			free(def_l_or_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_load_table)
		{
			delete_chain_string(def_load_table_chain_string);
			free(def_load_table_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_match)
		{
			delete_chain_string(def_match_chain_string);
			free(def_match_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_mid)
		{
			delete_chain_string(def_mid_chain_string);
			free(def_mid_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_mod)
		{
			delete_chain_string(def_mod_chain_string);
			free(def_mod_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_multiply)
		{
			delete_chain_string(def_multiply_chain_string);
			free(def_multiply_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_nand)
		{
			delete_chain_string(def_nand_chain_string);
			free(def_nand_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_nor)
		{
			delete_chain_string(def_nor_chain_string);
			free(def_nor_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_not)
		{
			delete_chain_string(def_not_chain_string);
			free(def_not_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_object_type)
		{
			delete_chain_string(def_object_type_chain_string);
			free(def_object_type_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_or)
		{
			delete_chain_string(def_or_chain_string);
			free(def_or_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_package)
		{
			delete_chain_string(def_package_chain_string);
			free(def_package_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_ref_of)
		{
			delete_chain_string(def_ref_of_chain_string);
			free(def_ref_of_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_shift_left)
		{
			delete_chain_string(def_shift_left_chain_string);
			free(def_shift_left_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_shift_right)
		{
			delete_chain_string(def_shift_right_chain_string);
			free(def_shift_right_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_size_of)
		{
			delete_chain_string(def_size_of_chain_string);
			free(def_size_of_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_store)
		{
			delete_chain_string(def_store_chain_string);
			free(def_store_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_subtract)
		{
			delete_chain_string(def_subtract_chain_string);
			free(def_subtract_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_timer)
		{
			delete_chain_string(def_timer_chain_string);
			free(def_timer_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_bcd)
		{
			delete_chain_string(def_to_bcd_chain_string);
			free(def_to_bcd_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_buffer)
		{
			delete_chain_string(def_to_buffer_chain_string);
			free(def_to_buffer_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_decimal_string)
		{
			delete_chain_string(def_to_decimal_string_chain_string);
			free(def_to_decimal_string_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_hex_string)
		{
			delete_chain_string(def_to_hex_string_chain_string);
			free(def_to_hex_string_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_integer)
		{
			delete_chain_string(def_to_integer_chain_string);
			free(def_to_integer_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_to_string)
		{
			delete_chain_string(def_to_string_chain_string);
			free(def_to_string_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_var_package)
		{
			delete_chain_string(def_var_package_chain_string);
			free(def_var_package_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_wait)
		{
			delete_chain_string(def_wait_chain_string);
			free(def_wait_char_array);
		}
		if(aml_symbol->component.expression_opcode.def_xor)
		{
			delete_chain_string(def_xor_chain_string);
			free(def_xor_char_array);
		}
		if(aml_symbol->component.expression_opcode.method_invocation)
		{
			delete_chain_string(method_invocation_chain_string);
			free(method_invocation_char_array);
		}
		break;
	case aml_ext_op_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_field_element:
		if(aml_symbol->component.field_element.named_field)
		{
			named_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_element.named_field);
			insert_char_front(named_field_chain_string, named_field_chain_string->first_character, ' ');
			replace_chain_string(named_field_chain_string, "\n", "\n ");
			named_field_char_array = create_char_array_from_chain_string(named_field_chain_string);
		}
		else named_field_char_array = "";
		if(aml_symbol->component.field_element.reserved_field)
		{
			reserved_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_element.reserved_field);
			insert_char_front(reserved_field_chain_string, reserved_field_chain_string->first_character, ' ');
			replace_chain_string(reserved_field_chain_string, "\n", "\n ");
			reserved_field_char_array = create_char_array_from_chain_string(reserved_field_chain_string);
		}
		else reserved_field_char_array = "";
		if(aml_symbol->component.field_element.access_field)
		{
			access_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_element.access_field);
			insert_char_front(access_field_chain_string, access_field_chain_string->first_character, ' ');
			replace_chain_string(access_field_chain_string, "\n", "\n ");
			access_field_char_array = create_char_array_from_chain_string(access_field_chain_string);
		}
		else access_field_char_array = "";
		if(aml_symbol->component.field_element.extended_access_field)
		{
			extended_access_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_element.extended_access_field);
			insert_char_front(extended_access_field_chain_string, extended_access_field_chain_string->first_character, ' ');
			replace_chain_string(extended_access_field_chain_string, "\n", "\n ");
			extended_access_field_char_array = create_char_array_from_chain_string(extended_access_field_chain_string);
		}
		else extended_access_field_char_array = "";
		if(aml_symbol->component.field_element.connect_field)
		{
			connect_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_element.connect_field);
			insert_char_front(connect_field_chain_string, connect_field_chain_string->first_character, ' ');
			replace_chain_string(connect_field_chain_string, "\n", "\n ");
			connect_field_char_array = create_char_array_from_chain_string(connect_field_chain_string);
		}
		else connect_field_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), named_field_char_array, reserved_field_char_array, access_field_char_array, extended_access_field_char_array, connect_field_char_array);
		if(aml_symbol->component.field_element.named_field)
		{
			delete_chain_string(named_field_chain_string);
			free(named_field_char_array);
		}
		if(aml_symbol->component.field_element.reserved_field)
		{
			delete_chain_string(reserved_field_chain_string);
			free(reserved_field_char_array);
		}
		if(aml_symbol->component.field_element.access_field)
		{
			delete_chain_string(access_field_chain_string);
			free(access_field_char_array);
		}
		if(aml_symbol->component.field_element.extended_access_field)
		{
			delete_chain_string(extended_access_field_chain_string);
			free(extended_access_field_char_array);
		}
		if(aml_symbol->component.field_element.connect_field)
		{
			delete_chain_string(connect_field_chain_string);
			free(connect_field_char_array);
		}
		break;
	case aml_field_flags:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_field_list:
		if(aml_symbol->component.field_list.field_element)
		{
			field_element_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_list.field_element);
			insert_char_front(field_element_chain_string, field_element_chain_string->first_character, ' ');
			replace_chain_string(field_element_chain_string, "\n", "\n ");
			field_element_char_array = create_char_array_from_chain_string(field_element_chain_string);
		}
		else field_element_char_array = "";
		if(aml_symbol->component.field_list.field_list)
		{
			field_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_list.field_list);
			insert_char_front(field_list_chain_string, field_list_chain_string->first_character, ' ');
			replace_chain_string(field_list_chain_string, "\n", "\n ");
			field_list_char_array = create_char_array_from_chain_string(field_list_chain_string);
		}
		else field_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), field_element_char_array, field_list_char_array);
		if(aml_symbol->component.field_list.field_element)
		{
			delete_chain_string(field_element_chain_string);
			free(field_element_char_array);
		}
		if(aml_symbol->component.field_list.field_list)
		{
			delete_chain_string(field_list_chain_string);
			free(field_list_char_array);
		}
		break;
	case aml_field_op:
		if(aml_symbol->component.field_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.field_op.field_op_suffix)
		{
			field_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_op.field_op_suffix);
			insert_char_front(field_op_suffix_chain_string, field_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(field_op_suffix_chain_string, "\n", "\n ");
			field_op_suffix_char_array = create_char_array_from_chain_string(field_op_suffix_chain_string);
		}
		else field_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, field_op_suffix_char_array);
		if(aml_symbol->component.field_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.field_op.field_op_suffix)
		{
			delete_chain_string(field_op_suffix_chain_string);
			free(field_op_suffix_char_array);
		}
		break;
	case aml_field_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_if_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_increment_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_index_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_index_value:
		if(aml_symbol->component.index_value.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.index_value.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.index_value.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_lead_name_char:
		output = create_format_chain_string("%s '%c'\n", aml_symbol_type_name(aml_symbol->type), *aml_symbol->string.initial);
		break;
	case aml_l_equal_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_l_greater_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_l_less_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_l_or_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_local_obj:
		if(aml_symbol->component.local_obj.local_op)
		{
			local_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.local_obj.local_op);
			insert_char_front(local_op_chain_string, local_op_chain_string->first_character, ' ');
			replace_chain_string(local_op_chain_string, "\n", "\n ");
			local_op_char_array = create_char_array_from_chain_string(local_op_chain_string);
		}
		else local_op_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), local_op_char_array);
		if(aml_symbol->component.local_obj.local_op)
		{
			delete_chain_string(local_op_chain_string);
			free(local_op_char_array);
		}
		break;
	case aml_local_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_method_flags:
		output = create_format_chain_string("%s ArgCount = %01.1x, %s, SyncLevel = %01.1x\n", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.method_flags.arg_count, aml_symbol->component.method_flags.serialize_flag ? "Serialized" : "Not Serialized", aml_symbol->component.method_flags.sync_level);
		break;
	case aml_method_invocation:
		if(aml_symbol->component.method_invocation.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.method_invocation.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.method_invocation.term_arg_list)
		{
			term_arg_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.method_invocation.term_arg_list);
			insert_char_front(term_arg_list_chain_string, term_arg_list_chain_string->first_character, ' ');
			replace_chain_string(term_arg_list_chain_string, "\n", "\n ");
			term_arg_list_char_array = create_char_array_from_chain_string(term_arg_list_chain_string);
		}
		else term_arg_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), name_string_char_array, term_arg_list_char_array);
		if(aml_symbol->component.method_invocation.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.method_invocation.term_arg_list)
		{
			delete_chain_string(term_arg_list_chain_string);
			free(term_arg_list_char_array);
		}
		break;
	case aml_method_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_multi_name_path:
		name_segs_chain_string = malloc(*aml_symbol->component.multi_name_path.seg_count->string.initial * sizeof(*name_segs_chain_string));
		name_segs_char_array = malloc(*aml_symbol->component.multi_name_path.seg_count->string.initial * sizeof(*name_segs_char_array));
		if(aml_symbol->component.multi_name_path.multi_name_prefix)
		{
			multi_name_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.multi_name_path.multi_name_prefix);
			insert_char_front(multi_name_prefix_chain_string, multi_name_prefix_chain_string->first_character, ' ');
			replace_chain_string(multi_name_prefix_chain_string, "\n", "\n ");
			multi_name_prefix_char_array = create_char_array_from_chain_string(multi_name_prefix_chain_string);
		}
		else multi_name_prefix_char_array = "";
		if(aml_symbol->component.multi_name_path.seg_count)
		{
			seg_count_chain_string = aml_symbol_to_chain_string(aml_symbol->component.multi_name_path.seg_count);
			insert_char_front(seg_count_chain_string, seg_count_chain_string->first_character, ' ');
			replace_chain_string(seg_count_chain_string, "\n", "\n ");
			seg_count_char_array = create_char_array_from_chain_string(seg_count_chain_string);
		}
		else seg_count_char_array = "";
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])
		{
			name_segs_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.multi_name_path.name_seg[i]);
			insert_char_front(name_segs_chain_string[i], name_segs_chain_string[i]->first_character, ' ');
			replace_chain_string(name_segs_chain_string[i], "\n", "\n ");
			name_segs_char_array[i] = create_char_array_from_chain_string(name_segs_chain_string[i]);
		}
		else name_segs_char_array[i] = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), multi_name_prefix_char_array, seg_count_char_array);
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])insert_char_array_back(output, output->last_character, name_segs_char_array[i]);
		if(aml_symbol->component.multi_name_path.multi_name_prefix)
		{
			delete_chain_string(multi_name_prefix_chain_string);
			free(multi_name_prefix_char_array);
		}
		if(aml_symbol->component.multi_name_path.seg_count)
		{
			delete_chain_string(seg_count_chain_string);
			free(seg_count_char_array);
		}
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])
		{
			delete_chain_string(name_segs_chain_string[i]);
			free(name_segs_char_array[i]);
		}
		free(name_segs_chain_string);
		free(name_segs_char_array);
		break;
	case aml_multi_name_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_mutex_object:
		if(aml_symbol->component.mutex_object.super_name)
		{
			super_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.mutex_object.super_name);
			insert_char_front(super_name_chain_string, super_name_chain_string->first_character, ' ');
			replace_chain_string(super_name_chain_string, "\n", "\n ");
			super_name_char_array = create_char_array_from_chain_string(super_name_chain_string);
		}
		else super_name_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), super_name_char_array);
		if(aml_symbol->component.mutex_object.super_name)
		{
			delete_chain_string(super_name_chain_string);
			free(super_name_char_array);
		}
		break;
	case aml_mutex_op:
		if(aml_symbol->component.mutex_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.mutex_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.mutex_op.mutex_op_suffix)
		{
			mutex_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.mutex_op.mutex_op_suffix);
			insert_char_front(mutex_op_suffix_chain_string, mutex_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(mutex_op_suffix_chain_string, "\n", "\n ");
			mutex_op_suffix_char_array = create_char_array_from_chain_string(mutex_op_suffix_chain_string);
		}
		else mutex_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, mutex_op_suffix_char_array);
		if(aml_symbol->component.mutex_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.mutex_op.mutex_op_suffix)
		{
			delete_chain_string(mutex_op_suffix_chain_string);
			free(mutex_op_suffix_char_array);
		}
		break;
	case aml_mutex_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_name_char:
		if(aml_symbol->component.name_char.digit_char)
		{
			digit_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_char.digit_char);
			insert_char_front(digit_char_chain_string, digit_char_chain_string->first_character, ' ');
			replace_chain_string(digit_char_chain_string, "\n", "\n ");
			digit_char_char_array = create_char_array_from_chain_string(digit_char_chain_string);
		}
		else digit_char_char_array = "";
		if(aml_symbol->component.name_char.lead_name_char)
		{
			lead_name_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_char.lead_name_char);
			insert_char_front(lead_name_char_chain_string, lead_name_char_chain_string->first_character, ' ');
			replace_chain_string(lead_name_char_chain_string, "\n", "\n ");
			lead_name_char_char_array = create_char_array_from_chain_string(lead_name_char_chain_string);
		}
		else lead_name_char_char_array = "";
		output = create_format_chain_string("%s '%c'\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.name_char.character, digit_char_char_array, lead_name_char_char_array);
		if(aml_symbol->component.name_char.digit_char)
		{
			delete_chain_string(digit_char_chain_string);
			free(digit_char_char_array);
		}
		if(aml_symbol->component.name_char.lead_name_char)
		{
			delete_chain_string(lead_name_char_chain_string);
			free(lead_name_char_char_array);
		}
		break;
	case aml_name_path:
		if(aml_symbol->component.name_path.name_seg)
		{
			name_seg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.name_seg);
			insert_char_front(name_seg_chain_string, name_seg_chain_string->first_character, ' ');
			replace_chain_string(name_seg_chain_string, "\n", "\n ");
			name_seg_char_array = create_char_array_from_chain_string(name_seg_chain_string);
		}
		else name_seg_char_array = "";
		if(aml_symbol->component.name_path.dual_name_path)
		{
			dual_name_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.dual_name_path);
			insert_char_front(dual_name_path_chain_string, dual_name_path_chain_string->first_character, ' ');
			replace_chain_string(dual_name_path_chain_string, "\n", "\n ");
			dual_name_path_char_array = create_char_array_from_chain_string(dual_name_path_chain_string);
		}
		else dual_name_path_char_array = "";
		if(aml_symbol->component.name_path.multi_name_path)
		{
			multi_name_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.multi_name_path);
			insert_char_front(multi_name_path_chain_string, multi_name_path_chain_string->first_character, ' ');
			replace_chain_string(multi_name_path_chain_string, "\n", "\n ");
			multi_name_path_char_array = create_char_array_from_chain_string(multi_name_path_chain_string);
		}
		else multi_name_path_char_array = "";
		if(aml_symbol->component.name_path.null_name)
		{
			null_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.null_name);
			insert_char_front(null_name_chain_string, null_name_chain_string->first_character, ' ');
			replace_chain_string(null_name_chain_string, "\n", "\n ");
			null_name_char_array = create_char_array_from_chain_string(null_name_chain_string);
		}
		else null_name_char_array = "";
		output = create_format_chain_string("%s \"%s\"\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.name_path.string, name_seg_char_array, dual_name_path_char_array, multi_name_path_char_array, null_name_char_array);
		if(aml_symbol->component.name_path.name_seg)
		{
			delete_chain_string(name_seg_chain_string);
			free(name_seg_char_array);
		}
		if(aml_symbol->component.name_path.dual_name_path)
		{
			delete_chain_string(dual_name_path_chain_string);
			free(dual_name_path_char_array);
		}
		if(aml_symbol->component.name_path.multi_name_path)
		{
			delete_chain_string(multi_name_path_chain_string);
			free(multi_name_path_char_array);
		}
		if(aml_symbol->component.name_path.null_name)
		{
			delete_chain_string(null_name_chain_string);
			free(null_name_char_array);
		}
		break;
	case aml_name_seg:
		name_chars_chain_string = malloc(_countof(aml_symbol->component.name_seg.name_char) * sizeof(*name_chars_chain_string));
		name_chars_char_array = malloc(_countof(aml_symbol->component.name_seg.name_char) * sizeof(*name_chars_char_array));
		if(aml_symbol->component.name_seg.lead_name_char)
		{
			lead_name_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_seg.lead_name_char);
			insert_char_front(lead_name_char_chain_string, lead_name_char_chain_string->first_character, ' ');
			replace_chain_string(lead_name_char_chain_string, "\n", "\n ");
			lead_name_char_char_array = create_char_array_from_chain_string(lead_name_char_chain_string);
		}
		else lead_name_char_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.name_seg.name_char); i++)if(aml_symbol->component.name_seg.name_char[i])
		{
			name_chars_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.name_seg.name_char[i]);
			insert_char_front(name_chars_chain_string[i], name_chars_chain_string[i]->first_character, ' ');
			replace_chain_string(name_chars_chain_string[i], "\n", "\n ");
			name_chars_char_array[i] = create_char_array_from_chain_string(name_chars_chain_string[i]);
		}
		else name_chars_char_array[i] = "";
		output = create_format_chain_string("%s \"%s\"\n%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.name_seg.string, lead_name_char_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.name_seg.name_char); i++)insert_char_array_back(output, output->last_character, name_chars_char_array[i]);
		if(aml_symbol->component.name_seg.lead_name_char)
		{
			delete_chain_string(lead_name_char_chain_string);
			free(lead_name_char_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.name_seg.name_char); i++)if(aml_symbol->component.name_seg.name_char[i])
		{
			delete_chain_string(name_chars_chain_string[i]);
			free(name_chars_char_array[i]);
		}
		free(name_chars_chain_string);
		free(name_chars_char_array);
		break;
	case aml_name_space_modifier_obj:
		if(aml_symbol->component.name_space_modifier_obj.def_alias)
		{
			def_alias_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_alias);
			insert_char_front(def_alias_chain_string, def_alias_chain_string->first_character, ' ');
			replace_chain_string(def_alias_chain_string, "\n", "\n ");
			def_alias_char_array = create_char_array_from_chain_string(def_alias_chain_string);
		}
		else def_alias_char_array = "";
		if(aml_symbol->component.name_space_modifier_obj.def_name)
		{
			def_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_name);
			insert_char_front(def_name_chain_string, def_name_chain_string->first_character, ' ');
			replace_chain_string(def_name_chain_string, "\n", "\n ");
			def_name_char_array = create_char_array_from_chain_string(def_name_chain_string);
		}
		else def_name_char_array = "";
		if(aml_symbol->component.name_space_modifier_obj.def_scope)
		{
			def_scope_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_scope);
			insert_char_front(def_scope_chain_string, def_scope_chain_string->first_character, ' ');
			replace_chain_string(def_scope_chain_string, "\n", "\n ");
			def_scope_char_array = create_char_array_from_chain_string(def_scope_chain_string);
		}
		else def_scope_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), def_alias_char_array, def_name_char_array, def_scope_char_array);
		if(aml_symbol->component.name_space_modifier_obj.def_alias)
		{
			delete_chain_string(def_alias_chain_string);
			free(def_alias_char_array);
		}
		if(aml_symbol->component.name_space_modifier_obj.def_name)
		{
			delete_chain_string(def_name_chain_string);
			free(def_name_char_array);
		}
		if(aml_symbol->component.name_space_modifier_obj.def_scope)
		{
			delete_chain_string(def_scope_chain_string);
			free(def_scope_char_array);
		}
		break;
	case aml_name_string:
		if(aml_symbol->component.name_string.name_path)
		{
			name_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_string.name_path);
			insert_char_front(name_path_chain_string, name_path_chain_string->first_character, ' ');
			replace_chain_string(name_path_chain_string, "\n", "\n ");
			name_path_char_array = create_char_array_from_chain_string(name_path_chain_string);
		}
		else name_path_char_array = "";
		if(aml_symbol->component.name_string.prefix_path)
		{
			prefix_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_string.prefix_path);
			insert_char_front(prefix_path_chain_string, prefix_path_chain_string->first_character, ' ');
			replace_chain_string(prefix_path_chain_string, "\n", "\n ");
			prefix_path_char_array = create_char_array_from_chain_string(prefix_path_chain_string);
		}
		else prefix_path_char_array = "";
		if(aml_symbol->component.name_string.root_char)
		{
			root_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_string.root_char);
			insert_char_front(root_char_chain_string, root_char_chain_string->first_character, ' ');
			replace_chain_string(root_char_chain_string, "\n", "\n ");
			root_char_char_array = create_char_array_from_chain_string(root_char_chain_string);
		}
		else root_char_char_array = "";
		output = create_format_chain_string("%s \"%s\"\n%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.name_string.string, root_char_char_array, prefix_path_char_array, name_path_char_array);
		if(aml_symbol->component.name_string.name_path)
		{
			delete_chain_string(name_path_chain_string);
			free(name_path_char_array);
		}
		if(aml_symbol->component.name_string.prefix_path)
		{
			delete_chain_string(prefix_path_chain_string);
			free(prefix_path_char_array);
		}
		if(aml_symbol->component.name_string.root_char)
		{
			delete_chain_string(root_char_chain_string);
			free(root_char_char_array);
		}
		break;
	case aml_name_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_named_field:
		if(aml_symbol->component.named_field.name_seg)
		{
			name_seg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_field.name_seg);
			insert_char_front(name_seg_chain_string, name_seg_chain_string->first_character, ' ');
			replace_chain_string(name_seg_chain_string, "\n", "\n ");
			name_seg_char_array = create_char_array_from_chain_string(name_seg_chain_string);
		}
		else name_seg_char_array = "";
		if(aml_symbol->component.named_field.pkg_length)
		{
			pkg_length_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_field.pkg_length);
			insert_char_front(pkg_length_chain_string, pkg_length_chain_string->first_character, ' ');
			replace_chain_string(pkg_length_chain_string, "\n", "\n ");
			pkg_length_char_array = create_char_array_from_chain_string(pkg_length_chain_string);
		}
		else pkg_length_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), name_seg_char_array, pkg_length_char_array);
		if(aml_symbol->component.named_field.name_seg)
		{
			delete_chain_string(name_seg_chain_string);
			free(name_seg_char_array);
		}
		if(aml_symbol->component.named_field.pkg_length)
		{
			delete_chain_string(pkg_length_chain_string);
			free(pkg_length_char_array);
		}
		break;
	case aml_named_obj:
		if(aml_symbol->component.named_obj.def_bank_field)
		{
			def_bank_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_bank_field);
			insert_char_front(def_bank_field_chain_string, def_bank_field_chain_string->first_character, ' ');
			replace_chain_string(def_bank_field_chain_string, "\n", "\n ");
			def_bank_field_char_array = create_char_array_from_chain_string(def_bank_field_chain_string);
		}
		else def_bank_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_bit_field)
		{
			def_create_bit_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_bit_field);
			insert_char_front(def_create_bit_field_chain_string, def_create_bit_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_bit_field_chain_string, "\n", "\n ");
			def_create_bit_field_char_array = create_char_array_from_chain_string(def_create_bit_field_chain_string);
		}
		else def_create_bit_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_byte_field)
		{
			def_create_byte_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_byte_field);
			insert_char_front(def_create_byte_field_chain_string, def_create_byte_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_byte_field_chain_string, "\n", "\n ");
			def_create_byte_field_char_array = create_char_array_from_chain_string(def_create_byte_field_chain_string);
		}
		else def_create_byte_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_dword_field)
		{
			def_create_dword_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_dword_field);
			insert_char_front(def_create_dword_field_chain_string, def_create_dword_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_dword_field_chain_string, "\n", "\n ");
			def_create_dword_field_char_array = create_char_array_from_chain_string(def_create_dword_field_chain_string);
		}
		else def_create_dword_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_field)
		{
			def_create_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_field);
			insert_char_front(def_create_field_chain_string, def_create_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_field_chain_string, "\n", "\n ");
			def_create_field_char_array = create_char_array_from_chain_string(def_create_field_chain_string);
		}
		else def_create_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_qword_field)
		{
			def_create_qword_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_qword_field);
			insert_char_front(def_create_qword_field_chain_string, def_create_qword_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_qword_field_chain_string, "\n", "\n ");
			def_create_qword_field_char_array = create_char_array_from_chain_string(def_create_qword_field_chain_string);
		}
		else def_create_qword_field_char_array = "";
		if(aml_symbol->component.named_obj.def_create_word_field)
		{
			def_create_word_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_create_word_field);
			insert_char_front(def_create_word_field_chain_string, def_create_word_field_chain_string->first_character, ' ');
			replace_chain_string(def_create_word_field_chain_string, "\n", "\n ");
			def_create_word_field_char_array = create_char_array_from_chain_string(def_create_word_field_chain_string);
		}
		else def_create_word_field_char_array = "";
		if(aml_symbol->component.named_obj.def_data_region)
		{
			def_data_region_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_data_region);
			insert_char_front(def_data_region_chain_string, def_data_region_chain_string->first_character, ' ');
			replace_chain_string(def_data_region_chain_string, "\n", "\n ");
			def_data_region_char_array = create_char_array_from_chain_string(def_data_region_chain_string);
		}
		else def_data_region_char_array = "";
		if(aml_symbol->component.named_obj.def_device)
		{
			def_device_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_device);
			insert_char_front(def_device_chain_string, def_device_chain_string->first_character, ' ');
			replace_chain_string(def_device_chain_string, "\n", "\n ");
			def_device_char_array = create_char_array_from_chain_string(def_device_chain_string);
		}
		else def_device_char_array = "";
		if(aml_symbol->component.named_obj.def_external)
		{
			def_external_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_external);
			insert_char_front(def_external_chain_string, def_external_chain_string->first_character, ' ');
			replace_chain_string(def_external_chain_string, "\n", "\n ");
			def_external_char_array = create_char_array_from_chain_string(def_external_chain_string);
		}
		else def_external_char_array = "";
		if(aml_symbol->component.named_obj.def_field)
		{
			def_field_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_field);
			insert_char_front(def_field_chain_string, def_field_chain_string->first_character, ' ');
			replace_chain_string(def_field_chain_string, "\n", "\n ");
			def_field_char_array = create_char_array_from_chain_string(def_field_chain_string);
		}
		else def_field_char_array = "";
		if(aml_symbol->component.named_obj.def_method)
		{
			def_method_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_method);
			insert_char_front(def_method_chain_string, def_method_chain_string->first_character, ' ');
			replace_chain_string(def_method_chain_string, "\n", "\n ");
			def_method_char_array = create_char_array_from_chain_string(def_method_chain_string);
		}
		else def_method_char_array = "";
		if(aml_symbol->component.named_obj.def_mutex)
		{
			def_mutex_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_mutex);
			insert_char_front(def_mutex_chain_string, def_mutex_chain_string->first_character, ' ');
			replace_chain_string(def_mutex_chain_string, "\n", "\n ");
			def_mutex_char_array = create_char_array_from_chain_string(def_mutex_chain_string);
		}
		else def_mutex_char_array = "";
		if(aml_symbol->component.named_obj.def_op_region)
		{
			def_op_region_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_op_region);
			insert_char_front(def_op_region_chain_string, def_op_region_chain_string->first_character, ' ');
			replace_chain_string(def_op_region_chain_string, "\n", "\n ");
			def_op_region_char_array = create_char_array_from_chain_string(def_op_region_chain_string);
		}
		else def_op_region_char_array = "";
		if(aml_symbol->component.named_obj.def_power_res)
		{
			def_power_res_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_power_res);
			insert_char_front(def_power_res_chain_string, def_power_res_chain_string->first_character, ' ');
			replace_chain_string(def_power_res_chain_string, "\n", "\n ");
			def_power_res_char_array = create_char_array_from_chain_string(def_power_res_chain_string);
		}
		else def_power_res_char_array = "";
		if(aml_symbol->component.named_obj.def_thermal_zone)
		{
			def_thermal_zone_chain_string = aml_symbol_to_chain_string(aml_symbol->component.named_obj.def_thermal_zone);
			insert_char_front(def_thermal_zone_chain_string, def_thermal_zone_chain_string->first_character, ' ');
			replace_chain_string(def_thermal_zone_chain_string, "\n", "\n ");
			def_thermal_zone_char_array = create_char_array_from_chain_string(def_thermal_zone_chain_string);
		}
		else def_thermal_zone_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), def_bank_field_char_array, def_create_bit_field_char_array, def_create_byte_field_char_array, def_create_dword_field_char_array, def_create_field_char_array, def_create_qword_field_char_array, def_create_word_field_char_array, def_data_region_char_array, def_device_char_array, def_external_char_array,  def_field_char_array, def_method_char_array, def_mutex_char_array, def_op_region_char_array, def_power_res_char_array, def_thermal_zone_char_array);
		if(aml_symbol->component.named_obj.def_bank_field)
		{
			delete_chain_string(def_bank_field_chain_string);
			free(def_bank_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_bit_field)
		{
			delete_chain_string(def_create_bit_field_chain_string);
			free(def_create_bit_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_byte_field)
		{
			delete_chain_string(def_create_byte_field_chain_string);
			free(def_create_byte_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_dword_field)
		{
			delete_chain_string(def_create_dword_field_chain_string);
			free(def_create_dword_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_field)
		{
			delete_chain_string(def_create_field_chain_string);
			free(def_create_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_qword_field)
		{
			delete_chain_string(def_create_qword_field_chain_string);
			free(def_create_qword_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_create_word_field)
		{
			delete_chain_string(def_create_word_field_chain_string);
			free(def_create_word_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_data_region)
		{
			delete_chain_string(def_data_region_chain_string);
			free(def_data_region_char_array);
		}
		if(aml_symbol->component.named_obj.def_device)
		{
			delete_chain_string(def_device_chain_string);
			free(def_device_char_array);
		}
		if(aml_symbol->component.named_obj.def_external)
		{
			delete_chain_string(def_external_chain_string);
			free(def_external_char_array);
		}
		if(aml_symbol->component.named_obj.def_field)
		{
			delete_chain_string(def_field_chain_string);
			free(def_field_char_array);
		}
		if(aml_symbol->component.named_obj.def_method)
		{
			delete_chain_string(def_method_chain_string);
			free(def_method_char_array);
		}
		if(aml_symbol->component.named_obj.def_mutex)
		{
			delete_chain_string(def_mutex_chain_string);
			free(def_mutex_char_array);
		}
		if(aml_symbol->component.named_obj.def_op_region)
		{
			delete_chain_string(def_op_region_chain_string);
			free(def_op_region_char_array);
		}
		if(aml_symbol->component.named_obj.def_power_res)
		{
			delete_chain_string(def_power_res_chain_string);
			free(def_power_res_char_array);
		}
		if(aml_symbol->component.named_obj.def_thermal_zone)
		{
			delete_chain_string(def_thermal_zone_chain_string);
			free(def_thermal_zone_char_array);
		}
		break;
	case aml_null_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_null_name:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_num_elements:
		if(aml_symbol->component.num_elements.byte_data)
		{
			byte_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.num_elements.byte_data);
			insert_char_front(byte_data_chain_string, byte_data_chain_string->first_character, ' ');
			replace_chain_string(byte_data_chain_string, "\n", "\n ");
			byte_data_char_array = create_char_array_from_chain_string(byte_data_chain_string);
		}
		else byte_data_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), byte_data_char_array);
		if(aml_symbol->component.num_elements.byte_data)
		{
			delete_chain_string(byte_data_chain_string);
			free(byte_data_char_array);
		}
		break;
	case aml_obj_reference:
		if(aml_symbol->component.obj_reference.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.obj_reference.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.obj_reference.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_object:
		if(aml_symbol->component.object.named_obj)
		{
			named_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.object.named_obj);
			insert_char_front(named_obj_chain_string, named_obj_chain_string->first_character, ' ');
			replace_chain_string(named_obj_chain_string, "\n", "\n ");
			named_obj_char_array = create_char_array_from_chain_string(named_obj_chain_string);
		}
		else named_obj_char_array = "";
		if(aml_symbol->component.object.name_space_modifier_obj)
		{
			name_space_modifier_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.object.name_space_modifier_obj);
			insert_char_front(name_space_modifier_obj_chain_string, name_space_modifier_obj_chain_string->first_character, ' ');
			replace_chain_string(name_space_modifier_obj_chain_string, "\n", "\n ");
			name_space_modifier_obj_char_array = create_char_array_from_chain_string(name_space_modifier_obj_chain_string);
		}
		else name_space_modifier_obj_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), named_obj_char_array, name_space_modifier_obj_char_array);
		if(aml_symbol->component.object.named_obj)
		{
			delete_chain_string(named_obj_chain_string);
			free(named_obj_char_array);
		}
		if(aml_symbol->component.object.name_space_modifier_obj)
		{
			delete_chain_string(name_space_modifier_obj_chain_string);
			free(name_space_modifier_obj_char_array);
		}
		break;
	case aml_one_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_ones_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_op_region_op:
		if(aml_symbol->component.op_region_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.op_region_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.op_region_op.op_region_op_suffix)
		{
			op_region_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.op_region_op.op_region_op_suffix);
			insert_char_front(op_region_op_suffix_chain_string, op_region_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(op_region_op_suffix_chain_string, "\n", "\n ");
			op_region_op_suffix_char_array = create_char_array_from_chain_string(op_region_op_suffix_chain_string);
		}
		else op_region_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, op_region_op_suffix_char_array);
		if(aml_symbol->component.op_region_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.op_region_op.op_region_op_suffix)
		{
			delete_chain_string(op_region_op_suffix_chain_string);
			free(op_region_op_suffix_char_array);
		}
		break;
	case aml_op_region_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_operand:
		if(aml_symbol->component.operand.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.operand.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.operand.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_package_element:
		if(aml_symbol->component.package_element.data_ref_object)
		{
			data_ref_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.package_element.data_ref_object);
			insert_char_front(data_ref_object_chain_string, data_ref_object_chain_string->first_character, ' ');
			replace_chain_string(data_ref_object_chain_string, "\n", "\n ");
			data_ref_object_char_array = create_char_array_from_chain_string(data_ref_object_chain_string);
		}
		else data_ref_object_char_array = "";
		if(aml_symbol->component.package_element.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.package_element.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), data_ref_object_char_array, name_string_char_array);
		if(aml_symbol->component.package_element.data_ref_object)
		{
			delete_chain_string(data_ref_object_chain_string);
			free(data_ref_object_char_array);
		}
		if(aml_symbol->component.package_element.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		break;
	case aml_package_element_list:
		if(aml_symbol->component.package_element_list.package_element)
		{
			package_element_chain_string = aml_symbol_to_chain_string(aml_symbol->component.package_element_list.package_element);
			insert_char_front(package_element_chain_string, package_element_chain_string->first_character, ' ');
			replace_chain_string(package_element_chain_string, "\n", "\n ");
			package_element_char_array = create_char_array_from_chain_string(package_element_chain_string);
		}
		else package_element_char_array = "";
		if(aml_symbol->component.package_element_list.package_element_list)
		{
			package_element_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.package_element_list.package_element_list);
			insert_char_front(package_element_list_chain_string, package_element_list_chain_string->first_character, ' ');
			replace_chain_string(package_element_list_chain_string, "\n", "\n ");
			package_element_list_char_array = create_char_array_from_chain_string(package_element_list_chain_string);
		}
		else package_element_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), package_element_char_array, package_element_list_char_array);
		if(aml_symbol->component.package_element_list.package_element)
		{
			delete_chain_string(package_element_chain_string);
			free(package_element_char_array);
		}
		if(aml_symbol->component.package_element_list.package_element_list)
		{
			delete_chain_string(package_element_list_chain_string);
			free(package_element_list_char_array);
		}
		break;
	case aml_package_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_parent_prefix_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_pkg_lead_byte:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_pkg_length:
		bytes_data_chain_string = malloc(_countof(aml_symbol->component.pkg_length.byte_data) * sizeof(*bytes_data_chain_string));
		bytes_data_char_array = malloc(_countof(aml_symbol->component.pkg_length.byte_data) * sizeof(*bytes_data_char_array));
		if(aml_symbol->component.pkg_length.pkg_lead_byte)
		{
			pkg_lead_byte_chain_string = aml_symbol_to_chain_string(aml_symbol->component.pkg_length.pkg_lead_byte);
			insert_char_front(pkg_lead_byte_chain_string, pkg_lead_byte_chain_string->first_character, ' ');
			replace_chain_string(pkg_lead_byte_chain_string, "\n", "\n ");
			pkg_lead_byte_char_array = create_char_array_from_chain_string(pkg_lead_byte_chain_string);
		}
		else pkg_lead_byte_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.pkg_length.byte_data); i++)if(aml_symbol->component.pkg_length.byte_data[i])
		{
			bytes_data_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.pkg_length.byte_data[i]);
			insert_char_front(bytes_data_chain_string[i], bytes_data_chain_string[i]->first_character, ' ');
			replace_chain_string(bytes_data_chain_string[i], "\n", "\n ");
			bytes_data_char_array[i] = create_char_array_from_chain_string(bytes_data_chain_string[i]);
		}
		else bytes_data_char_array[i] = "";
		output = create_format_chain_string("%s %#010.8x\n%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.pkg_length.length, pkg_lead_byte_char_array);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.pkg_length.byte_data); i++)insert_char_array_back(output, output->last_character, bytes_data_char_array[i]);
		if(aml_symbol->component.pkg_length.pkg_lead_byte)
		{
			delete_chain_string(pkg_lead_byte_chain_string);
			free(pkg_lead_byte_char_array);
		}
		for(unsigned int i = 0; i < _countof(aml_symbol->component.pkg_length.byte_data); i++)if(aml_symbol->component.pkg_length.byte_data[i])
		{
			delete_chain_string(bytes_data_chain_string[i]);
			free(bytes_data_char_array[i]);
		}
		free(bytes_data_chain_string);
		free(bytes_data_char_array);
		break;
	case aml_predicate:
		if(aml_symbol->component.predicate.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.predicate.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.predicate.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)
		{
			parent_prefix_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.prefix_path.parent_prefix_char);
			insert_char_front(parent_prefix_char_chain_string, parent_prefix_char_chain_string->first_character, ' ');
			replace_chain_string(parent_prefix_char_chain_string, "\n", "\n ");
			parent_prefix_char_char_array = create_char_array_from_chain_string(parent_prefix_char_chain_string);
		}
		else parent_prefix_char_char_array = "";
		if(aml_symbol->component.prefix_path.prefix_path)
		{
			prefix_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.prefix_path.prefix_path);
			insert_char_front(prefix_path_chain_string, prefix_path_chain_string->first_character, ' ');
			replace_chain_string(prefix_path_chain_string, "\n", "\n ");
			prefix_path_char_array = create_char_array_from_chain_string(prefix_path_chain_string);
		}
		else prefix_path_char_array = "";
		output = create_format_chain_string("%s \"%s\"\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.prefix_path.string, parent_prefix_char_char_array, prefix_path_char_array);
		if(aml_symbol->component.prefix_path.parent_prefix_char)
		{
			delete_chain_string(parent_prefix_char_chain_string);
			free(parent_prefix_char_char_array);
		}
		if(aml_symbol->component.prefix_path.parent_prefix_char)
		{
			delete_chain_string(parent_prefix_char_chain_string);
			free(parent_prefix_char_char_array);
		}
		break;
	case aml_qword_const:
		if(aml_symbol->component.qword_const.qword_prefix)
		{
			qword_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.qword_const.qword_prefix);
			insert_char_front(qword_prefix_chain_string, qword_prefix_chain_string->first_character, ' ');
			replace_chain_string(qword_prefix_chain_string, "\n", "\n ");
			qword_prefix_char_array = create_char_array_from_chain_string(qword_prefix_chain_string);
		}
		else qword_prefix_char_array = "";
		if(aml_symbol->component.qword_const.qword_data)
		{
			qword_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.qword_const.qword_data);
			insert_char_front(qword_data_chain_string, qword_data_chain_string->first_character, ' ');
			replace_chain_string(qword_data_chain_string, "\n", "\n ");
			qword_data_char_array = create_char_array_from_chain_string(qword_data_chain_string);
		}
		else qword_prefix_char_array = "";
		output = create_format_chain_string("%s %#018.16x\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.qword_const.value, qword_prefix_char_array, qword_data_char_array);
		if(aml_symbol->component.qword_const.qword_prefix)
		{
			delete_chain_string(qword_prefix_chain_string);
			free(qword_prefix_char_array);
		}
		if(aml_symbol->component.qword_const.qword_data)
		{
			delete_chain_string(qword_data_chain_string);
			free(qword_data_char_array);
		}
		break;
	case aml_qword_data:
		dwords_data_chain_string = malloc(_countof(aml_symbol->component.qword_data.dword_data) * sizeof(*dwords_data_chain_string));
		dwords_data_char_array = malloc(_countof(aml_symbol->component.qword_data.dword_data) * sizeof(*dwords_data_char_array));
		for(unsigned int i = 0; i < _countof(aml_symbol->component.qword_data.dword_data); i++)if(aml_symbol->component.qword_data.dword_data)
		{
			dwords_data_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.qword_data.dword_data[i]);
			insert_char_front(dwords_data_chain_string[i], dwords_data_chain_string[i]->first_character, ' ');
			replace_chain_string(dwords_data_chain_string[i], "\n", "\n ");
			dwords_data_char_array[i] = create_char_array_from_chain_string(dwords_data_chain_string[i]);
		}
		else dwords_data_char_array[i] = "";
		output = create_format_chain_string("%s %#018.16x\n", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.qword_data.value);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.qword_data.dword_data); i++)insert_char_array_back(output, output->last_character, dwords_data_char_array[i]);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.qword_data.dword_data); i++)if(aml_symbol->component.qword_data.dword_data[i])
		{
			delete_chain_string(dwords_data_chain_string[i]);
			free(dwords_data_char_array[i]);
		}
		free(dwords_data_chain_string);
		free(dwords_data_char_array);
		break;
	case aml_qword_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_region_len:
		if(aml_symbol->component.region_len.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.region_len.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.region_len.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_region_offset:
		if(aml_symbol->component.region_offset.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.region_offset.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.region_offset.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_region_space:
		output = create_format_chain_string("%s %#04.2x %s\n", aml_symbol_type_name(aml_symbol->type), *aml_symbol->string.initial, region_space_type_name(*aml_symbol->string.initial));
		break;
	case aml_release_op:
		if(aml_symbol->component.release_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.release_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.release_op.release_op_suffix)
		{
			release_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.release_op.release_op_suffix);
			insert_char_front(release_op_suffix_chain_string, release_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(release_op_suffix_chain_string, "\n", "\n ");
			release_op_suffix_char_array = create_char_array_from_chain_string(release_op_suffix_chain_string);
		}
		else release_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, release_op_suffix_char_array);
		if(aml_symbol->component.release_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.release_op.release_op_suffix)
		{
			delete_chain_string(release_op_suffix_chain_string);
			free(release_op_suffix_char_array);
		}
		break;
	case aml_release_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_return_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_revision_op:
		if(aml_symbol->component.revision_op.ext_op_prefix)
		{
			ext_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.revision_op.ext_op_prefix);
			insert_char_front(ext_op_prefix_chain_string, ext_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(ext_op_prefix_chain_string, "\n", "\n ");
			ext_op_prefix_char_array = create_char_array_from_chain_string(ext_op_prefix_chain_string);
		}
		else ext_op_prefix_char_array = "";
		if(aml_symbol->component.revision_op.revision_op_suffix)
		{
			revision_op_suffix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.revision_op.revision_op_suffix);
			insert_char_front(revision_op_suffix_chain_string, revision_op_suffix_chain_string->first_character, ' ');
			replace_chain_string(revision_op_suffix_chain_string, "\n", "\n ");
			revision_op_suffix_char_array = create_char_array_from_chain_string(revision_op_suffix_chain_string);
		}
		else revision_op_suffix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, revision_op_suffix_char_array);
		if(aml_symbol->component.revision_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.revision_op.revision_op_suffix)
		{
			delete_chain_string(revision_op_suffix_chain_string);
			free(revision_op_suffix_char_array);
		}
		break;
	case aml_revision_op_suffix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_root_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_scope_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_seg_count:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_shift_count:
		if(aml_symbol->component.shift_count.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.shift_count.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array);
		if(aml_symbol->component.shift_count.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		break;
	case aml_shift_left_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_shift_right_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_simple_name:
		if(aml_symbol->component.simple_name.name_string)
		{
			name_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.simple_name.name_string);
			insert_char_front(name_string_chain_string, name_string_chain_string->first_character, ' ');
			replace_chain_string(name_string_chain_string, "\n", "\n ");
			name_string_char_array = create_char_array_from_chain_string(name_string_chain_string);
		}
		else name_string_char_array = "";
		if(aml_symbol->component.simple_name.arg_obj)
		{
			arg_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.simple_name.arg_obj);
			insert_char_front(arg_obj_chain_string, arg_obj_chain_string->first_character, ' ');
			replace_chain_string(arg_obj_chain_string, "\n", "\n ");
			arg_obj_char_array = create_char_array_from_chain_string(arg_obj_chain_string);
		}
		else arg_obj_char_array = "";
		if(aml_symbol->component.simple_name.local_obj)
		{
			local_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.simple_name.local_obj);
			insert_char_front(local_obj_chain_string, local_obj_chain_string->first_character, ' ');
			replace_chain_string(local_obj_chain_string, "\n", "\n ");
			local_obj_char_array = create_char_array_from_chain_string(local_obj_chain_string);
		}
		else local_obj_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), name_string_char_array, arg_obj_char_array, local_obj_char_array);
		if(aml_symbol->component.simple_name.name_string)
		{
			delete_chain_string(name_string_chain_string);
			free(name_string_char_array);
		}
		if(aml_symbol->component.simple_name.arg_obj)
		{
			delete_chain_string(arg_obj_chain_string);
			free(arg_obj_char_array);
		}
		if(aml_symbol->component.simple_name.local_obj)
		{
			delete_chain_string(local_obj_chain_string);
			free(local_obj_char_array);
		}
		break;
	case aml_size_of_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_statement_opcode:
		if(aml_symbol->component.statement_opcode.def_break)
		{
			def_break_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_break);
			insert_char_front(def_break_chain_string, def_break_chain_string->first_character, ' ');
			replace_chain_string(def_break_chain_string, "\n", "\n ");
			def_break_char_array = create_char_array_from_chain_string(def_break_chain_string);
		}
		else def_break_char_array = "";
		if(aml_symbol->component.statement_opcode.def_break_point)
		{
			def_break_point_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_break_point);
			insert_char_front(def_break_point_chain_string, def_break_point_chain_string->first_character, ' ');
			replace_chain_string(def_break_point_chain_string, "\n", "\n ");
			def_break_point_char_array = create_char_array_from_chain_string(def_break_point_chain_string);
		}
		else def_break_point_char_array = "";
		if(aml_symbol->component.statement_opcode.def_continue)
		{
			def_continue_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_continue);
			insert_char_front(def_continue_chain_string, def_continue_chain_string->first_character, ' ');
			replace_chain_string(def_continue_chain_string, "\n", "\n ");
			def_continue_char_array = create_char_array_from_chain_string(def_continue_chain_string);
		}
		else def_continue_char_array = "";
		if(aml_symbol->component.statement_opcode.def_fatal)
		{
			def_fatal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_fatal);
			insert_char_front(def_fatal_chain_string, def_fatal_chain_string->first_character, ' ');
			replace_chain_string(def_fatal_chain_string, "\n", "\n ");
			def_fatal_char_array = create_char_array_from_chain_string(def_fatal_chain_string);
		}
		else def_fatal_char_array = "";
		if(aml_symbol->component.statement_opcode.def_if_else)
		{
			def_if_else_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_if_else);
			insert_char_front(def_if_else_chain_string, def_if_else_chain_string->first_character, ' ');
			replace_chain_string(def_if_else_chain_string, "\n", "\n ");
			def_if_else_char_array = create_char_array_from_chain_string(def_if_else_chain_string);
		}
		else def_if_else_char_array = "";
		if(aml_symbol->component.statement_opcode.def_noop)
		{
			def_noop_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_noop);
			insert_char_front(def_noop_chain_string, def_noop_chain_string->first_character, ' ');
			replace_chain_string(def_noop_chain_string, "\n", "\n ");
			def_noop_char_array = create_char_array_from_chain_string(def_noop_chain_string);
		}
		else def_noop_char_array = "";
		if(aml_symbol->component.statement_opcode.def_notify)
		{
			def_notify_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_notify);
			insert_char_front(def_notify_chain_string, def_notify_chain_string->first_character, ' ');
			replace_chain_string(def_notify_chain_string, "\n", "\n ");
			def_notify_char_array = create_char_array_from_chain_string(def_notify_chain_string);
		}
		else def_notify_char_array = "";
		if(aml_symbol->component.statement_opcode.def_release)
		{
			def_release_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_release);
			insert_char_front(def_release_chain_string, def_release_chain_string->first_character, ' ');
			replace_chain_string(def_release_chain_string, "\n", "\n ");
			def_release_char_array = create_char_array_from_chain_string(def_release_chain_string);
		}
		else def_release_char_array = "";
		if(aml_symbol->component.statement_opcode.def_reset)
		{
			def_reset_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_reset);
			insert_char_front(def_reset_chain_string, def_reset_chain_string->first_character, ' ');
			replace_chain_string(def_reset_chain_string, "\n", "\n ");
			def_reset_char_array = create_char_array_from_chain_string(def_reset_chain_string);
		}
		else def_reset_char_array = "";
		if(aml_symbol->component.statement_opcode.def_return)
		{
			def_return_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_return);
			insert_char_front(def_return_chain_string, def_return_chain_string->first_character, ' ');
			replace_chain_string(def_return_chain_string, "\n", "\n ");
			def_return_char_array = create_char_array_from_chain_string(def_return_chain_string);
		}
		else def_return_char_array = "";
		if(aml_symbol->component.statement_opcode.def_signal)
		{
			def_signal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_signal);
			insert_char_front(def_signal_chain_string, def_signal_chain_string->first_character, ' ');
			replace_chain_string(def_signal_chain_string, "\n", "\n ");
			def_signal_char_array = create_char_array_from_chain_string(def_signal_chain_string);
		}
		else def_signal_char_array = "";
		if(aml_symbol->component.statement_opcode.def_sleep)
		{
			def_sleep_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_sleep);
			insert_char_front(def_sleep_chain_string, def_sleep_chain_string->first_character, ' ');
			replace_chain_string(def_sleep_chain_string, "\n", "\n ");
			def_sleep_char_array = create_char_array_from_chain_string(def_sleep_chain_string);
		}
		else def_sleep_char_array = "";
		if(aml_symbol->component.statement_opcode.def_stall)
		{
			def_stall_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_stall);
			insert_char_front(def_stall_chain_string, def_stall_chain_string->first_character, ' ');
			replace_chain_string(def_stall_chain_string, "\n", "\n ");
			def_stall_char_array = create_char_array_from_chain_string(def_stall_chain_string);
		}
		else def_stall_char_array = "";
		if(aml_symbol->component.statement_opcode.def_while)
		{
			def_while_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_while);
			insert_char_front(def_while_chain_string, def_while_chain_string->first_character, ' ');
			replace_chain_string(def_while_chain_string, "\n", "\n ");
			def_while_char_array = create_char_array_from_chain_string(def_while_chain_string);
		}
		else def_while_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), def_break_char_array, def_break_point_char_array, def_continue_char_array, def_fatal_char_array, def_if_else_char_array, def_noop_char_array, def_notify_char_array, def_release_char_array, def_reset_char_array, def_return_char_array, def_signal_char_array, def_sleep_char_array, def_stall_char_array, def_while_char_array);
		if(aml_symbol->component.statement_opcode.def_break)
		{
			delete_chain_string(def_break_chain_string);
			free(def_break_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_break_point)
		{
			delete_chain_string(def_break_point_chain_string);
			free(def_break_point_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_continue)
		{
			delete_chain_string(def_continue_chain_string);
			free(def_continue_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_fatal)
		{
			delete_chain_string(def_fatal_chain_string);
			free(def_fatal_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_if_else)
		{
			delete_chain_string(def_if_else_chain_string);
			free(def_if_else_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_noop)
		{
			delete_chain_string(def_noop_chain_string);
			free(def_noop_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_notify)
		{
			delete_chain_string(def_notify_chain_string);
			free(def_notify_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_release)
		{
			delete_chain_string(def_release_chain_string);
			free(def_release_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_reset)
		{
			delete_chain_string(def_reset_chain_string);
			free(def_reset_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_return)
		{
			delete_chain_string(def_return_chain_string);
			free(def_return_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_signal)
		{
			delete_chain_string(def_signal_chain_string);
			free(def_signal_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_sleep)
		{
			delete_chain_string(def_sleep_chain_string);
			free(def_sleep_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_stall)
		{
			delete_chain_string(def_stall_chain_string);
			free(def_stall_char_array);
		}
		if(aml_symbol->component.statement_opcode.def_while)
		{
			delete_chain_string(def_while_chain_string);
			free(def_while_char_array);
		}
		break;
	case aml_store_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_string:
		if(aml_symbol->component.string.string_prefix)
		{
			string_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.string.string_prefix);
			insert_char_front(string_prefix_chain_string, string_prefix_chain_string->first_character, ' ');
			replace_chain_string(string_prefix_chain_string, "\n", "\n ");
			string_prefix_char_array = create_char_array_from_chain_string(string_prefix_chain_string);
		}
		else string_prefix_char_array = "";
		if(aml_symbol->component.string.ascii_char_list)
		{
			ascii_char_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.string.ascii_char_list);
			insert_char_front(ascii_char_list_chain_string, ascii_char_list_chain_string->first_character, ' ');
			replace_chain_string(ascii_char_list_chain_string, "\n", "\n ");
			ascii_char_list_char_array = create_char_array_from_chain_string(ascii_char_list_chain_string);
		}
		else ascii_char_list_char_array = "";
		if(aml_symbol->component.string.null_char)
		{
			null_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.string.null_char);
			insert_char_front(null_char_chain_string, null_char_chain_string->first_character, ' ');
			replace_chain_string(null_char_chain_string, "\n", "\n ");
			null_char_char_array = create_char_array_from_chain_string(null_char_chain_string);
		}
		else null_char_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", string_prefix_char_array, ascii_char_list_char_array, null_char_char_array);
		if(aml_symbol->component.string.string_prefix)
		{
			delete_chain_string(string_prefix_chain_string);
			free(string_prefix_char_array);
		}
		if(aml_symbol->component.string.ascii_char_list)
		{
			delete_chain_string(ascii_char_list_chain_string);
			free(ascii_char_list_char_array);
		}
		if(aml_symbol->component.string.null_char)
		{
			delete_chain_string(null_char_chain_string);
			free(null_char_char_array);
		}
		break;
	case aml_string_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_subtract_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_super_name:
		if(aml_symbol->component.super_name.debug_obj)
		{
			debug_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.super_name.debug_obj);
			insert_char_front(debug_obj_chain_string, debug_obj_chain_string->first_character, ' ');
			replace_chain_string(debug_obj_chain_string, "\n", "\n ");
			debug_obj_char_array = create_char_array_from_chain_string(debug_obj_chain_string);
		}
		else debug_obj_char_array = "";
		if(aml_symbol->component.super_name.reference_type_opcode)
		{
			reference_type_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.super_name.reference_type_opcode);
			insert_char_front(reference_type_opcode_chain_string, reference_type_opcode_chain_string->first_character, ' ');
			replace_chain_string(reference_type_opcode_chain_string, "\n", "\n ");
			reference_type_opcode_char_array = create_char_array_from_chain_string(reference_type_opcode_chain_string);
		}
		else reference_type_opcode_char_array = "";
		if(aml_symbol->component.super_name.simple_name)
		{
			simple_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.super_name.simple_name);
			insert_char_front(simple_name_chain_string, simple_name_chain_string->first_character, ' ');
			replace_chain_string(simple_name_chain_string, "\n", "\n ");
			simple_name_char_array = create_char_array_from_chain_string(simple_name_chain_string);
		}
		else simple_name_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), debug_obj_char_array, reference_type_opcode_char_array, simple_name_char_array);
		if(aml_symbol->component.super_name.debug_obj)
		{
			delete_chain_string(debug_obj_chain_string);
			free(debug_obj_char_array);
		}
		if(aml_symbol->component.super_name.reference_type_opcode)
		{
			delete_chain_string(reference_type_opcode_chain_string);
			free(reference_type_opcode_char_array);
		}
		if(aml_symbol->component.super_name.simple_name)
		{
			delete_chain_string(simple_name_chain_string);
			free(simple_name_char_array);
		}
		break;
	case aml_sync_flags:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_target:
		if(aml_symbol->component.target.super_name)
		{
			super_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.target.super_name);
			insert_char_front(super_name_chain_string, super_name_chain_string->first_character, ' ');
			replace_chain_string(super_name_chain_string, "\n", "\n ");
			super_name_char_array = create_char_array_from_chain_string(super_name_chain_string);
		}
		else super_name_char_array = "";
		if(aml_symbol->component.target.null_name)
		{
			null_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.target.null_name);
			insert_char_front(null_name_chain_string, null_name_chain_string->first_character, ' ');
			replace_chain_string(null_name_chain_string, "\n", "\n ");
			null_name_char_array = create_char_array_from_chain_string(null_name_chain_string);
		}
		else null_name_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), super_name_char_array, null_name_char_array);
		if(aml_symbol->component.target.super_name)
		{
			delete_chain_string(super_name_chain_string);
			free(super_name_char_array);
		}
		if(aml_symbol->component.target.null_name)
		{
			delete_chain_string(null_name_chain_string);
			free(null_name_char_array);
		}
		break;
	case aml_term_arg:
		if(aml_symbol->component.term_arg.expression_opcode)
		{
			expression_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg.expression_opcode);
			insert_char_front(expression_opcode_chain_string, expression_opcode_chain_string->first_character, ' ');
			replace_chain_string(expression_opcode_chain_string, "\n", "\n ");
			expression_opcode_char_array = create_char_array_from_chain_string(expression_opcode_chain_string);
		}
		else expression_opcode_char_array = "";
		if(aml_symbol->component.term_arg.data_object)
		{
			data_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg.data_object);
			insert_char_front(data_object_chain_string, data_object_chain_string->first_character, ' ');
			replace_chain_string(data_object_chain_string, "\n", "\n ");
			data_object_char_array = create_char_array_from_chain_string(data_object_chain_string);
		}
		else data_object_char_array = "";
		if(aml_symbol->component.term_arg.arg_obj)
		{
			arg_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg.arg_obj);
			insert_char_front(arg_obj_chain_string, arg_obj_chain_string->first_character, ' ');
			replace_chain_string(arg_obj_chain_string, "\n", "\n ");
			arg_obj_char_array = create_char_array_from_chain_string(arg_obj_chain_string);
		}
		else arg_obj_char_array = "";
		if(aml_symbol->component.term_arg.local_obj)
		{
			local_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg.local_obj);
			insert_char_front(local_obj_chain_string, local_obj_chain_string->first_character, ' ');
			replace_chain_string(local_obj_chain_string, "\n", "\n ");
			local_obj_char_array = create_char_array_from_chain_string(local_obj_chain_string);
		}
		else local_obj_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), expression_opcode_char_array, data_object_char_array, arg_obj_char_array, local_obj_char_array);
		if(aml_symbol->component.term_arg.expression_opcode)
		{
			delete_chain_string(expression_opcode_chain_string);
			free(expression_opcode_char_array);
		}
		if(aml_symbol->component.term_arg.data_object)
		{
			delete_chain_string(data_object_chain_string);
			free(data_object_char_array);
		}
		if(aml_symbol->component.term_arg.arg_obj)
		{
			delete_chain_string(arg_obj_chain_string);
			free(arg_obj_char_array);
		}
		if(aml_symbol->component.term_arg.local_obj)
		{
			delete_chain_string(local_obj_chain_string);
			free(local_obj_char_array);
		}
		break;
	case aml_term_arg_list:
		if(aml_symbol->component.term_arg_list.term_arg)
		{
			term_arg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg_list.term_arg);
			insert_char_front(term_arg_chain_string, term_arg_chain_string->first_character, ' ');
			replace_chain_string(term_arg_chain_string, "\n", "\n ");
			term_arg_char_array = create_char_array_from_chain_string(term_arg_chain_string);
		}
		else term_arg_char_array = "";
		if(aml_symbol->component.term_arg_list.term_arg_list)
		{
			term_arg_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_arg_list.term_arg_list);
			insert_char_front(term_arg_list_chain_string, term_arg_list_chain_string->first_character, ' ');
			replace_chain_string(term_arg_list_chain_string, "\n", "\n ");
			term_arg_list_char_array = create_char_array_from_chain_string(term_arg_list_chain_string);
		}
		else term_arg_list_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), term_arg_char_array, term_arg_list_char_array);
		if(aml_symbol->component.term_arg_list.term_arg)
		{
			delete_chain_string(term_arg_chain_string);
			free(term_arg_char_array);
		}
		if(aml_symbol->component.term_arg_list.term_arg_list)
		{
			delete_chain_string(term_arg_list_chain_string);
			free(term_arg_list_char_array);
		}
		break;
	case aml_term_list:
		if(aml_symbol->component.term_list.term_obj)
		{
			term_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_list.term_obj);
			insert_char_front(term_obj_chain_string, term_obj_chain_string->first_character, ' ');
			replace_chain_string(term_obj_chain_string, "\n", "\n ");
			term_obj_char_array = create_char_array_from_chain_string(term_obj_chain_string);
		}
		else term_obj_char_array = "";
		if(aml_symbol->component.term_list.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_list.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", "\n ");
			term_list_char_array = create_char_array_from_chain_string(term_list_chain_string);
		}
		else term_list_char_array = "";
		output = create_format_chain_string("%s length = %#010.8x\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, term_obj_char_array, term_list_char_array);
		if(aml_symbol->component.term_list.term_obj)
		{
			delete_chain_string(term_obj_chain_string);
			free(term_obj_char_array);
		}
		if(aml_symbol->component.term_list.term_list)
		{
			delete_chain_string(term_list_chain_string);
			free(term_list_char_array);
		}
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object)
		{
			object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_obj.object);
			insert_char_front(object_chain_string, object_chain_string->first_character, ' ');
			replace_chain_string(object_chain_string, "\n", "\n ");
			object_char_array = create_char_array_from_chain_string(object_chain_string);
		}
		else object_char_array = "";
		if(aml_symbol->component.term_obj.statement_opcode)
		{
			statement_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_obj.statement_opcode);
			insert_char_front(statement_opcode_chain_string, statement_opcode_chain_string->first_character, ' ');
			replace_chain_string(statement_opcode_chain_string, "\n", "\n ");
			statement_opcode_char_array = create_char_array_from_chain_string(statement_opcode_chain_string);
		}
		else statement_opcode_char_array = "";
		if(aml_symbol->component.term_obj.expression_opcode)
		{
			expression_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_obj.expression_opcode);
			insert_char_front(expression_opcode_chain_string, expression_opcode_chain_string->first_character, ' ');
			replace_chain_string(expression_opcode_chain_string, "\n", "\n ");
			expression_opcode_char_array = create_char_array_from_chain_string(expression_opcode_chain_string);
		}
		else expression_opcode_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", aml_symbol_type_name(aml_symbol->type), object_char_array, statement_opcode_char_array, expression_opcode_char_array);
		if(aml_symbol->component.term_obj.object)
		{
			delete_chain_string(object_chain_string);
			free(object_char_array);
		}
		if(aml_symbol->component.term_obj.statement_opcode)
		{
			delete_chain_string(statement_opcode_chain_string);
			free(statement_opcode_char_array);
		}
		if(aml_symbol->component.term_obj.expression_opcode)
		{
			delete_chain_string(expression_opcode_chain_string);
			free(expression_opcode_char_array);
		}
		break;
	case aml_time_out:
		if(aml_symbol->component.time_out.word_data)
		{
			word_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.time_out.word_data);
			insert_char_front(word_data_chain_string, word_data_chain_string->first_character, ' ');
			replace_chain_string(word_data_chain_string, "\n", "\n ");
			word_data_char_array = create_char_array_from_chain_string(word_data_chain_string);
		}
		else word_data_char_array = "";
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), word_data_char_array);
		if(aml_symbol->component.time_out.word_data)
		{
			delete_chain_string(word_data_chain_string);
			free(word_data_char_array);
		}
		break;
	case aml_to_buffer_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_to_hex_string_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_while_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_word_const:
		if(aml_symbol->component.word_const.word_prefix)
		{
			word_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.word_const.word_prefix);
			insert_char_front(word_prefix_chain_string, word_prefix_chain_string->first_character, ' ');
			replace_chain_string(word_prefix_chain_string, "\n", "\n ");
			word_prefix_char_array = create_char_array_from_chain_string(word_prefix_chain_string);
		}
		else word_prefix_char_array = "";
		if(aml_symbol->component.word_const.word_data)
		{
			word_data_chain_string = aml_symbol_to_chain_string(aml_symbol->component.word_const.word_data);
			insert_char_front(word_data_chain_string, word_data_chain_string->first_character, ' ');
			replace_chain_string(word_data_chain_string, "\n", "\n ");
			word_data_char_array = create_char_array_from_chain_string(word_data_chain_string);
		}
		else word_data_char_array = "";
		output = create_format_chain_string("%s %#06.4x\n%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.word_const.value, word_prefix_char_array, word_data_char_array);
		if(aml_symbol->component.word_const.word_prefix)
		{
			delete_chain_string(word_prefix_chain_string);
			free(word_prefix_char_array);
		}
		if(aml_symbol->component.word_const.word_data)
		{
			delete_chain_string(word_data_chain_string);
			free(word_data_char_array);
		}
		break;
	case aml_word_data:
		bytes_data_chain_string = malloc(_countof(aml_symbol->component.word_data.byte_data) * sizeof(*bytes_data_chain_string));
		bytes_data_char_array = malloc(_countof(aml_symbol->component.word_data.byte_data) * sizeof(*bytes_data_char_array));
		for(unsigned int i = 0; i < _countof(aml_symbol->component.word_data.byte_data); i++)if(aml_symbol->component.word_data.byte_data[i])
		{
			bytes_data_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.word_data.byte_data[i]);
			insert_char_front(bytes_data_chain_string[i], bytes_data_chain_string[i]->first_character, ' ');
			replace_chain_string(bytes_data_chain_string[i], "\n", "\n ");
			bytes_data_char_array[i] = create_char_array_from_chain_string(bytes_data_chain_string[i]);
		}
		else bytes_data_char_array[i] = "";
		output = create_format_chain_string("%s %#06.4x\n", aml_symbol_type_name(aml_symbol->type), aml_symbol->component.word_data.value);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.word_data.byte_data); i++)insert_char_array_back(output, output->last_character, bytes_data_char_array[i]);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.word_data.byte_data); i++)if(aml_symbol->component.word_data.byte_data[i])
		{
			delete_chain_string(bytes_data_chain_string[i]);
			free(bytes_data_char_array[i]);
		}
		free(bytes_data_chain_string);
		free(bytes_data_char_array);
		break;
	case aml_word_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_zero_op:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	default:
		ERROR(); // Invalid AML symbol type
		break;
	}
	return output;
}

char *aml_symbol_to_string(AMLSymbol const *aml_symbol)
{
	ChainString *aml_symbol_chain_string = aml_symbol_to_chain_string(aml_symbol);
	char *aml_symbol_string = create_char_array_from_chain_string(aml_symbol_chain_string);
	delete_chain_string(aml_symbol_chain_string);
	return aml_symbol_string;
}

char const *aml_symbol_type_name(AMLSymbolType aml_symbol_type)
{
	static char const * const aml_acquire_op_name = "AcquireOp";
	static char const * const aml_acquire_op_suffix_name = "AcquireOpSuffix";
	static char const * const aml_alias_op_name = "AliasOp";
	static char const * const aml_arg_obj_name = "ArgObj";
	static char const * const aml_arg_object_name = "ArgObject";
	static char const * const aml_arg_op_name = "ArgOp";
	static char const * const aml_ascii_char_name = "AsciiChar";
	static char const * const aml_ascii_char_list_name = "AsciiCharList";
	static char const * const aml_buff_pkg_str_obj_name = "BuffPkgStrObj";
	static char const * const aml_buffer_op_name = "BufferOp";
	static char const * const aml_buffer_size_name = "BufferSize";
	static char const * const aml_byte_const_name = "ByteConst";
	static char const * const aml_byte_data_name = "ByteData";
	static char const * const aml_byte_list_name = "ByteList";
	static char const * const aml_byte_prefix_name = "BytePrefix";
	static char const * const aml_computational_data_name = "ComputationalData";
	static char const * const aml_const_obj_name = "ConstObj";
	static char const * const aml_data_object_name = "DataObject";
	static char const * const aml_data_ref_object_name = "DataRefObject";
	static char const * const aml_def_alias_name = "DefAlias";
	static char const * const aml_def_acquire_name = "DefAcquire";
	static char const * const aml_def_buffer_name = "DefBuffer";
	static char const * const aml_def_deref_of_name = "DefDerefOf";
	static char const * const aml_def_device_name = "DefDevice";
	static char const * const aml_def_field_name = "DefField";
	static char const * const aml_def_if_else_name = "DefIfElse";
	static char const * const aml_def_increment_name = "DefIncrement";
	static char const * const aml_def_index_name = "DefIndex";
	static char const * const aml_def_l_equal_name = "DefLEqual";
	static char const * const aml_def_l_greater_name = "DefLGreater";
	static char const * const aml_def_l_less_name = "DefLLess";
	static char const * const aml_def_l_or_name = "DefLOr";
	static char const * const aml_def_method_name = "DefMethod";
	static char const * const aml_def_mutex_name = "DefMutex";
	static char const * const aml_def_name_name = "DefName";
	static char const * const aml_def_op_region_name = "DefOpRegion";
	static char const * const aml_def_package_name = "DefPackage";
	static char const * const aml_def_release_name = "DefRelease";
	static char const * const aml_def_return_name = "DefReturn";
	static char const * const aml_def_scope_name = "DefScope";
	static char const * const aml_def_shift_left_name = "DefShiftLeft";
	static char const * const aml_def_shift_right_name = "DefShiftRight";
	static char const * const aml_def_size_of_name = "DefSizeOf";
	static char const * const aml_def_store_name = "DefStore";
	static char const * const aml_def_subtract_name = "DefSubtract";
	static char const * const aml_def_to_buffer_name = "DefToBuffer";
	static char const * const aml_def_to_hex_string_name = "DefToHexString";
	static char const * const aml_def_while_name = "DefWhile";
	static char const * const aml_deref_of_op_name = "DerefOfOp";
	static char const * const aml_device_op_name = "DeviceOp";
	static char const * const aml_device_op_suffix_name = "DeviceOpSuffix";
	static char const * const aml_digit_char_name = "DigitChar";
	static char const * const aml_dual_name_path_name = "DualNamePath";
	static char const * const aml_dual_name_prefix_name = "DualNamePrefix";
	static char const * const aml_dword_const_name = "DWordConst";
	static char const * const aml_dword_data_name = "DWordData";
	static char const * const aml_dword_prefix_name = "DWordPrefix";
	static char const * const aml_expression_opcode_name = "ExpressionOpcode";
	static char const * const aml_ext_op_prefix_name = "ExtOpPrefix";
	static char const * const aml_field_element_name = "FieldElement";
	static char const * const aml_field_flags_name = "FieldFlags";
	static char const * const aml_field_list_name = "FieldList";
	static char const * const aml_field_op_name = "FieldOp";
	static char const * const aml_field_op_suffix_name = "FieldOpSuffix";
	static char const * const aml_if_op_name = "IfOp";
	static char const * const aml_increment_op_name = "IncrementOp";
	static char const * const aml_index_op_name = "IndexOp";
	static char const * const aml_index_value_name = "IndexValue";
	static char const * const aml_lead_name_char_name = "LeadNameChar";
	static char const * const aml_l_equal_op_name = "LEqualOp";
	static char const * const aml_l_greater_op_name = "LGreaterOp";
	static char const * const aml_l_less_op_name = "LLessOp";
	static char const * const aml_l_or_op_name = "LOrOp";
	static char const * const aml_local_obj_name = "LocalObj";
	static char const * const aml_local_op_name = "LocalOp";
	static char const * const aml_method_flags_name = "MethodFlags";
	static char const * const aml_method_invocation_name = "MethodInvocation";
	static char const * const aml_method_op_name = "MethodOp";
	static char const * const aml_multi_name_path_name = "MultiNamePath";
	static char const * const aml_multi_name_prefix_name = "MultiNamePrefix";
	static char const * const aml_mutex_object_name = "MutexObject";
	static char const * const aml_mutex_op_name = "MutexOp";
	static char const * const aml_mutex_op_suffix_name = "MutexOpSuffix";
	static char const * const aml_name_char_name = "NameChar";
	static char const * const aml_name_op_name = "NameOp";
	static char const * const aml_name_path_name = "NamePath";
	static char const * const aml_name_seg_name = "NameSeg";
	static char const * const aml_name_space_modifier_obj_name = "NameSpaceModifierObj";
	static char const * const aml_name_string_name = "NameString";
	static char const * const aml_named_field_name = "NamedField";
	static char const * const aml_named_obj_name = "NamedObj";
	static char const * const aml_null_char_name = "NullChar";
	static char const * const aml_null_name_name = "NullName";
	static char const * const aml_num_elements_name = "NumElements";
	static char const * const aml_obj_reference_name = "ObjReference";
	static char const * const aml_object_name = "Object";
	static char const * const aml_one_op_name = "OneOp";
	static char const * const aml_ones_op_name = "OnesOp";
	static char const * const aml_op_region_op_name = "OpRegionOp";
	static char const * const aml_op_region_op_suffix_name = "OpRegionOpSuffix";
	static char const * const aml_operand_name = "Operand";
	static char const * const aml_package_element_name = "PackageElement";
	static char const * const aml_package_element_list_name = "PackageElementList";
	static char const * const aml_package_op_name = "PackageOp";
	static char const * const aml_parent_prefix_char_name = "ParentPrefixChar";
	static char const * const aml_pkg_lead_byte_name = "PkgLeadByte";
	static char const * const aml_pkg_length_name = "PkgLength";
	static char const * const aml_predicate_name = "Predicate";
	static char const * const aml_prefix_path_name = "PrefixPath";
	static char const * const aml_qword_const_name = "QWordConst";
	static char const * const aml_qword_data_name = "QWordData";
	static char const * const aml_qword_prefix_name = "QWordPrefix";
	static char const * const aml_region_len_name = "RegionLen";
	static char const * const aml_region_offset_name = "RegionOffset";
	static char const * const aml_region_space_name = "RegionSpace";
	static char const * const aml_release_op_name = "ReleaseOp";
	static char const * const aml_release_op_suffix_name = "ReleaseOpSuffix";
	static char const * const aml_return_op_name = "ReturnOp";
	static char const * const aml_revision_op_name = "RevisionOp";
	static char const * const aml_revision_op_suffix_name = "RevisionOpSuffix";
	static char const * const aml_root_char_name = "RootChar";
	static char const * const aml_scope_op_name = "ScopeOp";
	static char const * const aml_seg_count_name = "SegCount";
	static char const * const aml_shift_count_name = "ShiftCount";
	static char const * const aml_shift_left_op_name = "ShiftLeftOp";
	static char const * const aml_shift_right_op_name = "ShiftRightOp";
	static char const * const aml_simple_name_name = "SimpleName";
	static char const * const aml_size_of_op_name = "SizeOfOp";
	static char const * const aml_statement_opcode_name = "StatementOpcode";
	static char const * const aml_store_op_name = "StoreOp";
	static char const * const aml_string_name = "String";
	static char const * const aml_string_prefix_name = "StringPrefix";
	static char const * const aml_subtract_op_name = "SubtractOp";
	static char const * const aml_super_name_name = "SuperName";
	static char const * const aml_sync_flags_name = "SyncFlags";
	static char const * const aml_target_name = "Target";
	static char const * const aml_term_arg_name = "TermArg";
	static char const * const aml_term_arg_list_name = "TermArgList";
	static char const * const aml_term_list_name = "TermList";
	static char const * const aml_term_obj_name = "TermObj";
	static char const * const aml_time_out_name = "TimeOut";
	static char const * const aml_to_buffer_op_name = "ToBufferOp";
	static char const * const aml_to_hex_string_op_name = "ToHexStringOp";
	static char const * const aml_while_op_name = "WhileOp";
	static char const * const aml_word_const_name = "WordConst";
	static char const * const aml_word_data_name = "WordData";
	static char const * const aml_word_prefix_name = "WordPrefix";
	static char const * const aml_zero_op_name = "ZeroOp";
	switch(aml_symbol_type)
	{
	case aml_acquire_op:
		return aml_acquire_op_name;
	case aml_acquire_op_suffix:
		return aml_acquire_op_suffix_name;
	case aml_alias_op:
		return aml_alias_op_name;
	case aml_arg_obj:
		return aml_arg_obj_name;
	case aml_arg_object:
		return aml_arg_object_name;
	case aml_arg_op:
		return aml_arg_op_name;
	case aml_ascii_char:
		return aml_ascii_char_name;
	case aml_ascii_char_list:
		return aml_ascii_char_list_name;
	case aml_buff_pkg_str_obj:
		return aml_buff_pkg_str_obj_name;
	case aml_buffer_op:
		return aml_buffer_op_name;
	case aml_buffer_size:
		return aml_buffer_size_name;
	case aml_byte_const:
		return aml_byte_const_name;
	case aml_byte_data:
		return aml_byte_data_name;
	case aml_byte_list:
		return aml_byte_list_name;
	case aml_byte_prefix:
		return aml_byte_prefix_name;
	case aml_computational_data:
		return aml_computational_data_name;
	case aml_const_obj:
		return aml_const_obj_name;
	case aml_data_object:
		return aml_data_object_name;
	case aml_data_ref_object:
		return aml_data_ref_object_name;
	case aml_def_alias:
		return aml_def_alias_name;
	case aml_def_acquire:
		return aml_def_acquire_name;
	case aml_def_buffer:
		return aml_def_buffer_name;
	case aml_def_deref_of:
		return aml_def_deref_of_name;
	case aml_def_device:
		return aml_def_device_name;
	case aml_def_field:
		return aml_def_field_name;
	case aml_def_if_else:
		return aml_def_if_else_name;
	case aml_def_increment:
		return aml_def_increment_name;
	case aml_def_index:
		return aml_def_index_name;
	case aml_def_l_equal:
		return aml_def_l_equal_name;
	case aml_def_l_greater:
		return aml_def_l_greater_name;
	case aml_def_l_less:
		return aml_def_l_less_name;
	case aml_def_l_or:
		return aml_def_l_or_name;
	case aml_def_method:
		return aml_def_method_name;
	case aml_def_mutex:
		return aml_def_mutex_name;
	case aml_def_name:
		return aml_def_name_name;
	case aml_def_op_region:
		return aml_def_op_region_name;
	case aml_def_package:
		return aml_def_package_name;
	case aml_def_release:
		return aml_def_release_name;
	case aml_def_return:
		return aml_def_return_name;
	case aml_def_scope:
		return aml_def_scope_name;
	case aml_def_shift_left:
		return aml_def_shift_left_name;
	case aml_def_shift_right:
		return aml_def_shift_right_name;
	case aml_def_size_of:
		return aml_def_size_of_name;
	case aml_def_store:
		return aml_def_store_name;
	case aml_def_subtract:
		return aml_def_subtract_name;
	case aml_def_to_buffer:
		return aml_def_to_buffer_name;
	case aml_def_to_hex_string:
		return aml_def_to_hex_string_name;
	case aml_def_while:
		return aml_def_while_name;
	case aml_deref_of_op:
		return aml_deref_of_op_name;
	case aml_device_op:
		return aml_device_op_name;
	case aml_device_op_suffix:
		return aml_device_op_suffix_name;
	case aml_digit_char:
		return aml_digit_char_name;
	case aml_dual_name_path:
		return aml_dual_name_path_name;
	case aml_dual_name_prefix:
		return aml_dual_name_prefix_name;
	case aml_dword_const:
		return aml_dword_const_name;
	case aml_dword_data:
		return aml_dword_data_name;
	case aml_dword_prefix:
		return aml_dword_prefix_name;
	case aml_expression_opcode:
		return aml_expression_opcode_name;
	case aml_ext_op_prefix:
		return aml_ext_op_prefix_name;
	case aml_field_element:
		return aml_field_element_name;
	case aml_field_flags:
		return aml_field_flags_name;
	case aml_field_list:
		return aml_field_list_name;
	case aml_field_op:
		return aml_field_op_name;
	case aml_field_op_suffix:
		return aml_field_op_suffix_name;
	case aml_if_op:
		return aml_if_op_name;
	case aml_increment_op:
		return aml_increment_op_name;
	case aml_index_op:
		return aml_index_op_name;
	case aml_index_value:
		return aml_index_value_name;
	case aml_lead_name_char:
		return aml_lead_name_char_name;
	case aml_l_equal_op:
		return aml_l_equal_op_name;
	case aml_l_greater_op:
		return aml_l_greater_op_name;
	case aml_l_less_op:
		return aml_l_less_op_name;
	case aml_l_or_op:
		return aml_l_or_op_name;
	case aml_local_obj:
		return aml_local_obj_name;
	case aml_local_op:
		return aml_local_op_name;
	case aml_method_flags:
		return aml_method_flags_name;
	case aml_method_invocation:
		return aml_method_invocation_name;
	case aml_method_op:
		return aml_method_op_name;
	case aml_multi_name_path:
		return aml_multi_name_path_name;
	case aml_multi_name_prefix:
		return aml_multi_name_prefix_name;
	case aml_mutex_object:
		return aml_mutex_object_name;
	case aml_mutex_op:
		return aml_mutex_op_name;
	case aml_mutex_op_suffix:
		return aml_mutex_op_suffix_name;
	case aml_name_char:
		return aml_name_char_name;
	case aml_name_op:
		return aml_name_op_name;
	case aml_name_path:
		return aml_name_path_name;
	case aml_name_seg:
		return aml_name_seg_name;
	case aml_name_space_modifier_obj:
		return aml_name_space_modifier_obj_name;
	case aml_name_string:
		return aml_name_string_name;
	case aml_named_field:
		return aml_named_field_name;
	case aml_named_obj:
		return aml_named_obj_name;
	case aml_null_char:
		return aml_null_char_name;
	case aml_null_name:
		return aml_null_name_name;
	case aml_num_elements:
		return aml_num_elements_name;
	case aml_obj_reference:
		return aml_obj_reference_name;
	case aml_object:
		return aml_object_name;
	case aml_one_op:
		return aml_one_op_name;
	case aml_ones_op:
		return aml_ones_op_name;
	case aml_op_region_op:
		return aml_op_region_op_name;
	case aml_op_region_op_suffix:
		return aml_op_region_op_suffix_name;
	case aml_operand:
		return aml_operand_name;
	case aml_package_element:
		return aml_package_element_name;
	case aml_package_element_list:
		return aml_package_element_list_name;
	case aml_package_op:
		return aml_package_op_name;
	case aml_parent_prefix_char:
		return aml_parent_prefix_char_name;
	case aml_pkg_lead_byte:
		return aml_pkg_lead_byte_name;
	case aml_pkg_length:
		return aml_pkg_length_name;
	case aml_predicate:
		return aml_predicate_name;
	case aml_prefix_path:
		return aml_prefix_path_name;
	case aml_qword_const:
		return aml_qword_const_name;
	case aml_qword_data:
		return aml_qword_data_name;
	case aml_qword_prefix:
		return aml_qword_prefix_name;
	case aml_region_len:
		return aml_region_len_name;
	case aml_region_offset:
		return aml_region_offset_name;
	case aml_region_space:
		return aml_region_space_name;
	case aml_release_op:
		return aml_release_op_name;
	case aml_release_op_suffix:
		return aml_release_op_suffix_name;
	case aml_return_op:
		return aml_return_op_name;
	case aml_revision_op:
		return aml_revision_op_name;
	case aml_revision_op_suffix:
		return aml_revision_op_suffix_name;
	case aml_root_char:
		return aml_root_char_name;
	case aml_scope_op:
		return aml_scope_op_name;
	case aml_seg_count:
		return aml_seg_count_name;
	case aml_shift_count:
		return aml_shift_count_name;
	case aml_shift_left_op:
		return aml_shift_left_op_name;
	case aml_shift_right_op:
		return aml_shift_right_op_name;
	case aml_simple_name:
		return aml_simple_name_name;
	case aml_size_of_op:
		return aml_size_of_op_name;
	case aml_statement_opcode:
		return aml_statement_opcode_name;
	case aml_store_op:
		return aml_store_op_name;
	case aml_string:
		return aml_string_name;
	case aml_string_prefix:
		return aml_string_prefix_name;
	case aml_subtract_op:
		return aml_subtract_op_name;
	case aml_super_name:
		return aml_super_name_name;
	case aml_sync_flags:
		return aml_sync_flags_name;
	case aml_target:
		return aml_target_name;
	case aml_term_arg:
		return aml_term_arg_name;
	case aml_term_arg_list:
		return aml_term_arg_list_name;
	case aml_term_list:
		return aml_term_list_name;
	case aml_term_obj:
		return aml_term_obj_name;
	case aml_time_out:
		return aml_time_out_name;
	case aml_to_buffer_op:
		return aml_to_buffer_op_name;
	case aml_to_hex_string_op:
		return aml_to_hex_string_op_name;
	case aml_while_op:
		return aml_while_op_name;
	case aml_word_const:
		return aml_word_const_name;
	case aml_word_data:
		return aml_word_data_name;
	case aml_word_prefix:
		return aml_word_prefix_name;
	case aml_zero_op:
		return aml_zero_op_name;
	default:
		ERROR(); // Invalid AML symbol type
		printf_serial("aml_symbol_type = %#010.8x\n", aml_symbol_type);
		return NULL;
	}
}

// <acquire_op> := <ext_op_prefix> <acquire_op_suffix>
AMLSymbol *analyse_aml_acquire_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *acquire_op = malloc(sizeof(*acquire_op));
	acquire_op->parent = parent;
	acquire_op->string.initial = aml.initial;
	acquire_op->string.length = 0;
	acquire_op->type = aml_acquire_op;
	acquire_op->component.acquire_op.ext_op_prefix = analyse_aml_ext_op_prefix(acquire_op, aml);
	acquire_op->string.length += acquire_op->component.acquire_op.ext_op_prefix->string.length;
	aml.initial += acquire_op->component.acquire_op.ext_op_prefix->string.length;
	aml.length -= acquire_op->component.acquire_op.ext_op_prefix->string.length;
	acquire_op->component.acquire_op.acquire_op_suffix = analyse_aml_acquire_op_suffix(acquire_op, aml);
	acquire_op->string.length += acquire_op->component.acquire_op.acquire_op_suffix->string.length;
	aml.initial += acquire_op->component.acquire_op.acquire_op_suffix->string.length;
	aml.length -= acquire_op->component.acquire_op.acquire_op_suffix->string.length;
	return acquire_op;
}

// <acquire_op_suffix> := AML_BYTE_ACQUIRE_OP
AMLSymbol *analyse_aml_acquire_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *acquire_op_suffix = malloc(sizeof(*acquire_op_suffix));
	acquire_op_suffix->parent = parent;
	acquire_op_suffix->string.initial = aml.initial;
	acquire_op_suffix->string.length = 1;
	acquire_op_suffix->type = aml_acquire_op_suffix;
	if(*aml.initial != AML_BYTE_ACQUIRE_OP)ERROR(); // Incorrect acquire_op_suffix
	return acquire_op_suffix;
}

// <alias_op> := AML_BYTE_ALIAS_OP
AMLSymbol *analyse_aml_alias_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *alias_op = malloc(sizeof(*alias_op));
	alias_op->parent = parent;
	alias_op->string.initial = aml.initial;
	alias_op->string.length = 1;
	alias_op->type = aml_alias_op;
	if(*alias_op->string.initial != AML_BYTE_ALIAS_OP)ERROR(); // Incorrect alias_op
	return alias_op;
}

// <arg_obj> := <arg_op>
AMLSymbol *analyse_aml_arg_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *arg_obj = malloc(sizeof(*arg_obj));
	arg_obj->parent = parent;
	arg_obj->string.initial = aml.initial;
	arg_obj->string.length = 0;
	arg_obj->type = aml_arg_obj;
	arg_obj->component.arg_obj.arg_op = analyse_aml_arg_op(arg_obj, aml);
	arg_obj->string.length += arg_obj->component.arg_obj.arg_op->string.length;
	arg_obj->component.arg_obj.arg_op_number = *arg_obj->component.arg_obj.arg_op->string.initial - AML_BYTE_ARG_0_OP;
	return arg_obj;
}

// <arg_object> := <term_arg>
AMLSymbol *analyse_aml_arg_object(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *arg_object = malloc(sizeof(*arg_object));
	arg_object->parent = parent;
	arg_object->string.initial = aml.initial;
	arg_object->string.length = 0;
	arg_object->type = aml_arg_object;
	arg_object->component.arg_object.term_arg = analyse_aml_term_arg(arg_object, aml);
	arg_object->string.length += arg_object->component.arg_object.term_arg->string.length;
	return arg_object;
}

// <arg_op> := 0x68 - 0x6e
AMLSymbol *analyse_aml_arg_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *arg_op = malloc(sizeof(*arg_op));
	arg_op->parent = parent;
	arg_op->string.initial = aml.initial;
	arg_op->string.length = 1;
	arg_op->type = aml_arg_op;
	switch(*arg_op->string.initial)
	{
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
		break;
	default:
		ERROR(); // Incorrect arg op
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return arg_op;
}

// <ascii_char> := 0x01 - 0x7f
AMLSymbol *analyse_aml_ascii_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *ascii_char = malloc(sizeof(*ascii_char));
	ascii_char->parent = parent;
	ascii_char->string.initial = aml.initial;
	ascii_char->string.length = 1;
	ascii_char->type = aml_ascii_char;
	if(!(0x01 <= *ascii_char->string.initial && *ascii_char->string.initial <= 0x7f))ERROR(); // Incorrect ascii char
	return ascii_char;
}

// <ascii_char_list> := Nothing | <ascii_char> <ascii_char_list>
AMLSymbol *analyse_aml_ascii_char_list(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *ascii_char_list = malloc(sizeof(*ascii_char_list));
	ascii_char_list->parent = parent;
	ascii_char_list->string.initial = aml.initial;
	ascii_char_list->string.length = 0;
	ascii_char_list->type = aml_ascii_char_list;
	if(0x01 <= *ascii_char_list->string.initial && *ascii_char_list->string.initial <= 0x7f)
	{
		ascii_char_list->component.ascii_char_list.ascii_char = analyse_aml_ascii_char(ascii_char_list, aml);
		ascii_char_list->string.length += ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		aml.initial += ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		aml.length -= ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		ascii_char_list->component.ascii_char_list.ascii_char_list = analyse_aml_ascii_char_list(ascii_char_list, aml);
		ascii_char_list->string.length += ascii_char_list->component.ascii_char_list.ascii_char_list->string.length;
		aml.initial += ascii_char_list->component.ascii_char_list.ascii_char_list->string.length;
		aml.length -= ascii_char_list->component.ascii_char_list.ascii_char_list->string.length;
	}
	else
	{
		ascii_char_list->component.ascii_char_list.ascii_char = NULL;
		ascii_char_list->component.ascii_char_list.ascii_char_list = NULL;
	}
	return ascii_char_list;
}

// <buff_pkg_str_obj> := <term_arg>
AMLSymbol *analyse_aml_buff_pkg_str_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *buff_pkg_str_obj = malloc(sizeof(*buff_pkg_str_obj));
	buff_pkg_str_obj->parent = parent;
	buff_pkg_str_obj->string.initial = aml.initial;
	buff_pkg_str_obj->string.length = 0;
	buff_pkg_str_obj->type = aml_buff_pkg_str_obj;
	buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg = analyse_aml_term_arg(buff_pkg_str_obj, aml);
	buff_pkg_str_obj->string.length += buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	aml.initial += buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	aml.length -= buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	return buff_pkg_str_obj;
}

// <buffer_op> := AML_BYTE_BUFFER_OP
AMLSymbol *analyse_aml_buffer_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *buffer_op = malloc(sizeof(*buffer_op));
	buffer_op->parent = parent;
	buffer_op->string.initial = aml.initial;
	buffer_op->string.length = 1;
	buffer_op->type = aml_buffer_op;
	if(*buffer_op->string.initial != AML_BYTE_BUFFER_OP)ERROR(); // Incorrect buffer op
	return buffer_op;
}

// <buffer_size> := <term_arg>
AMLSymbol *analyse_aml_buffer_size(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *buffer_size = malloc(sizeof(*buffer_size));
	buffer_size->parent = parent;
	buffer_size->string.initial = aml.initial;
	buffer_size->string.length = 0;
	buffer_size->type = aml_buffer_size;
	buffer_size->component.buffer_size.term_arg = analyse_aml_term_arg(buffer_size, aml);
	buffer_size->string.length += buffer_size->component.buffer_size.term_arg->string.length;
	return buffer_size;
}

// <byte_const> := <byte_prefix> <byte_data>
AMLSymbol *analyse_aml_byte_const(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *byte_const = malloc(sizeof(*byte_const));
	byte_const->parent = parent;
	byte_const->string.initial = aml.initial;
	byte_const->string.length = 0;
	byte_const->type = aml_byte_const;
	byte_const->component.byte_const.byte_prefix = analyse_aml_byte_prefix(byte_const, aml);
	byte_const->string.length += byte_const->component.byte_const.byte_prefix->string.length;
	aml.initial += byte_const->component.byte_const.byte_prefix->string.length;
	aml.length -= byte_const->component.byte_const.byte_prefix->string.length;
	byte_const->component.byte_const.byte_data = analyse_aml_byte_data(byte_const, aml);
	byte_const->string.length += byte_const->component.byte_const.byte_data->string.length;
	aml.initial += byte_const->component.byte_const.byte_data->string.length;
	aml.length -= byte_const->component.byte_const.byte_data->string.length;
	byte_const->component.byte_const.value = *byte_const->component.byte_const.byte_data->string.initial;
	return byte_const;
}

// <byte_data> := 0x00 - 0xff
AMLSymbol *analyse_aml_byte_data(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *byte_data = malloc(sizeof(*byte_data));
	byte_data->parent = parent;
	byte_data->string.initial = aml.initial;
	byte_data->string.length = 1;
	byte_data->type = aml_byte_data;
	return byte_data;
}

// <byte_list> := Nothing | <byte_data> <byte_list>
AMLSymbol *analyse_aml_byte_list(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *byte_list = malloc(sizeof(*byte_list));
	byte_list->parent = parent;
	byte_list->string.initial = aml.initial;
	byte_list->string.length = 0;
	byte_list->type = aml_byte_list;
	byte_list->component.byte_list.byte_data = NULL;
	byte_list->component.byte_list.byte_list = NULL;
	if(aml.length)
	{
		byte_list->component.byte_list.byte_data = analyse_aml_byte_data(byte_list, aml);
		byte_list->string.length += byte_list->component.byte_list.byte_data->string.length;
		aml.initial += byte_list->component.byte_list.byte_data->string.length;
		aml.length -= byte_list->component.byte_list.byte_data->string.length;
		byte_list->component.byte_list.byte_list = analyse_aml_byte_list(byte_list, aml);
		byte_list->string.length += byte_list->component.byte_list.byte_list->string.length;
		aml.initial += byte_list->component.byte_list.byte_list->string.length;
		aml.length -= byte_list->component.byte_list.byte_list->string.length;
	}
	return byte_list;
}

// <byte_prefix> := AML_BYTE_BYTE_PREFIX
AMLSymbol *analyse_aml_byte_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *byte_prefix = malloc(sizeof(*byte_prefix));
	byte_prefix->parent = parent;
	byte_prefix->string.initial = aml.initial;
	byte_prefix->string.length = 1;
	byte_prefix->type = aml_byte_prefix;
	if(*byte_prefix->string.initial != AML_BYTE_BYTE_PREFIX)ERROR(); // Incorrect byte prefix
	return byte_prefix;
}

// <computational_data> := <byte_const> | <word_const> | <dword_const> | <qword_const> | <string> | <const_obj> | <revision_op> | <def_buffer>
AMLSymbol *analyse_aml_computational_data(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *computational_data = malloc(sizeof(*computational_data));
	computational_data->parent = parent;
	computational_data->string.initial = aml.initial;
	computational_data->string.length = 0;
	computational_data->type = aml_computational_data;
	computational_data->component.computational_data.byte_const = NULL;
	computational_data->component.computational_data.word_const = NULL;
	computational_data->component.computational_data.dword_const = NULL;
	computational_data->component.computational_data.qword_const = NULL;
	computational_data->component.computational_data.string = NULL;
	computational_data->component.computational_data.const_obj = NULL;
	computational_data->component.computational_data.revision_op = NULL;
	computational_data->component.computational_data.def_buffer = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_BUFFER_OP:
		computational_data->component.computational_data.def_buffer = analyse_aml_def_buffer(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.def_buffer->string.length;
		break;
	case AML_BYTE_BYTE_PREFIX:
		computational_data->component.computational_data.byte_const = analyse_aml_byte_const(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.byte_const->string.length;
		break;
	case AML_BYTE_DWORD_PREFIX:
		computational_data->component.computational_data.dword_const = analyse_aml_dword_const(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.dword_const->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		computational_data->component.computational_data.revision_op = analyse_aml_revision_op(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.revision_op->string.length;
		break;
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ZERO_OP:
		computational_data->component.computational_data.const_obj = analyse_aml_const_obj(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.const_obj->string.length;
		break;
	case AML_BYTE_QWORD_PREFIX:
		computational_data->component.computational_data.qword_const = analyse_aml_qword_const(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.qword_const->string.length;
		break;
	case AML_BYTE_STRING_PREFIX:
		computational_data->component.computational_data.string = analyse_aml_string(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.string->string.length;
		break;
	case AML_BYTE_WORD_PREFIX:
		computational_data->component.computational_data.word_const = analyse_aml_word_const(computational_data, aml);
		computational_data->string.length += computational_data->component.computational_data.word_const->string.length;
		break;
	default:
		ERROR(); // Incorrect computational data
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return computational_data;
}

// <const_obj> := <zero_op> | <one_op> | <ones_op>
AMLSymbol *analyse_aml_const_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *const_obj = malloc(sizeof(*const_obj));
	const_obj->parent = parent;
	const_obj->string.initial = aml.initial;
	const_obj->string.length = 0;
	const_obj->type = aml_const_obj;
	const_obj->component.const_obj.zero_op = NULL;
	const_obj->component.const_obj.one_op = NULL;
	const_obj->component.const_obj.ones_op = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ONE_OP:
		const_obj->component.const_obj.one_op = analyse_aml_one_op(const_obj, aml);
		const_obj->string.length += const_obj->component.const_obj.one_op->string.length;
		aml.initial += const_obj->component.const_obj.one_op->string.length;
		aml.length -= const_obj->component.const_obj.one_op->string.length;
		break;
	case AML_BYTE_ONES_OP:
		const_obj->component.const_obj.ones_op = analyse_aml_ones_op(const_obj, aml);
		const_obj->string.length += const_obj->component.const_obj.ones_op->string.length;
		aml.initial += const_obj->component.const_obj.ones_op->string.length;
		aml.length -= const_obj->component.const_obj.ones_op->string.length;
		break;
	case AML_BYTE_ZERO_OP:
		const_obj->component.const_obj.zero_op = analyse_aml_zero_op(const_obj, aml);
		const_obj->string.length += const_obj->component.const_obj.zero_op->string.length;
		aml.initial += const_obj->component.const_obj.zero_op->string.length;
		aml.length -= const_obj->component.const_obj.zero_op->string.length;
		break;
	default:
		ERROR(); // Incorrect const obj
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return const_obj;
}

// <data_object> := <computational_data> | <def_package> | <def_var_package>
AMLSymbol *analyse_aml_data_object(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *data_object = malloc(sizeof(*data_object));
	data_object->parent = parent;
	data_object->string.initial = aml.initial;
	data_object->string.length = 0;
	data_object->type = aml_data_object;
	data_object->component.data_object.computational_data = NULL;
	data_object->component.data_object.def_package = NULL;
	data_object->component.data_object.def_var_package = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		data_object->component.data_object.computational_data = analyse_aml_computational_data(data_object, aml);
		data_object->string.length += data_object->component.data_object.computational_data->string.length;
		break;
	case AML_BYTE_PACKAGE_OP:
		data_object->component.data_object.def_package = analyse_aml_def_package(data_object, aml);
		data_object->string.length += data_object->component.data_object.def_package->string.length;
		break;
	default:
		ERROR(); // Incorrect data object
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return data_object;
}

// <data_ref_object> := <data_object> | <object_reference>
AMLSymbol *analyse_aml_data_ref_object(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *data_ref_object = malloc(sizeof(*data_ref_object));
	data_ref_object->parent = parent;
	data_ref_object->string.initial = aml.initial;
	data_ref_object->string.length = 0;
	data_ref_object->type = aml_data_ref_object;
	data_ref_object->component.data_ref_object.data_object = NULL;
	data_ref_object->component.data_ref_object.object_reference = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		data_ref_object->component.data_ref_object.data_object = analyse_aml_data_object(data_ref_object, aml);
		data_ref_object->string.length += data_ref_object->component.data_ref_object.data_object->string.length;
		break;
	default:
		ERROR(); // Incorrect data ref object
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return data_ref_object;
}

// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_alias = malloc(sizeof(*def_alias));
	def_alias->parent = parent;
	def_alias->string.initial = aml.initial;
	def_alias->string.length = 0;
	def_alias->type = aml_def_alias;
	def_alias->component.def_alias.alias_op = analyse_aml_alias_op(def_alias, aml);
	def_alias->string.length += def_alias->component.def_alias.alias_op->string.length;
	aml.initial += def_alias->component.def_alias.alias_op->string.length;
	aml.length -= def_alias->component.def_alias.alias_op->string.length;
	for(AMLSymbol **name_string = def_alias->component.def_alias.name_string; name_string != def_alias->component.def_alias.name_string + _countof(def_alias->component.def_alias.name_string); name_string++)
	{
		switch(*aml.initial)
		{
		case AML_BYTE_PARENT_PREFIX_CHAR:
		case AML_BYTE_ROOT_CHAR:
			break;
		default:
			ERROR(); // Syntax error
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
			break;
		}
		*name_string = analyse_aml_name_string(def_alias, aml);
		def_alias->string.length += (*name_string)->string.length;
		aml.initial += (*name_string)->string.length;
		aml.length -= (*name_string)->string.length;
	}
	return def_alias;
}

// <def_acquire> := <acquire_op> <mutex_object> <time_out>
AMLSymbol *analyse_aml_def_acquire(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_acquire = malloc(sizeof(*def_acquire));
	def_acquire->parent = parent;
	def_acquire->string.initial = aml.initial;
	def_acquire->string.length = 0;
	def_acquire->type = aml_def_acquire;
	def_acquire->component.def_acquire.acquire_op = analyse_aml_acquire_op(def_acquire, aml);
	def_acquire->string.length += def_acquire->component.def_acquire.acquire_op->string.length;
	aml.initial += def_acquire->component.def_acquire.acquire_op->string.length;
	aml.length -= def_acquire->component.def_acquire.acquire_op->string.length;
	def_acquire->component.def_acquire.mutex_object = analyse_aml_mutex_object(def_acquire, aml);
	def_acquire->string.length += def_acquire->component.def_acquire.mutex_object->string.length;
	aml.initial += def_acquire->component.def_acquire.mutex_object->string.length;
	aml.length -= def_acquire->component.def_acquire.mutex_object->string.length;
	def_acquire->component.def_acquire.time_out = analyse_aml_time_out(def_acquire, aml);
	def_acquire->string.length += def_acquire->component.def_acquire.time_out->string.length;
	aml.initial += def_acquire->component.def_acquire.time_out->string.length;
	aml.length -= def_acquire->component.def_acquire.time_out->string.length;
	return def_acquire;
}

// <def_buffer> := <buffer_op> <pkg_length> <buffer_size> <byte_list>
AMLSymbol *analyse_aml_def_buffer(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_buffer = malloc(sizeof(*def_buffer));
	def_buffer->parent = parent;
	def_buffer->string.initial = aml.initial;
	def_buffer->string.length = 0;
	def_buffer->type = aml_def_buffer;
	def_buffer->component.def_buffer.buffer_op = analyse_aml_buffer_op(def_buffer, aml);
	def_buffer->string.length += def_buffer->component.def_buffer.buffer_op->string.length;
	aml.initial += def_buffer->component.def_buffer.buffer_op->string.length;
	aml.length -= def_buffer->component.def_buffer.buffer_op->string.length;
	def_buffer->component.def_buffer.pkg_length = analyse_aml_pkg_length(def_buffer, aml);
	def_buffer->string.length += def_buffer->component.def_buffer.pkg_length->string.length;
	aml.initial += def_buffer->component.def_buffer.pkg_length->string.length;
	aml.length = def_buffer->component.def_buffer.pkg_length->component.pkg_length.length - def_buffer->component.def_buffer.pkg_length->string.length;
	def_buffer->component.def_buffer.buffer_size = analyse_aml_buffer_size(def_buffer, aml);
	def_buffer->string.length += def_buffer->component.def_buffer.buffer_size->string.length;
	aml.initial += def_buffer->component.def_buffer.buffer_size->string.length;
	aml.length -= def_buffer->component.def_buffer.buffer_size->string.length;
	def_buffer->component.def_buffer.byte_list = analyse_aml_byte_list(def_buffer, aml);
	def_buffer->string.length += def_buffer->component.def_buffer.byte_list->string.length;
	aml.initial += def_buffer->component.def_buffer.byte_list->string.length;
	aml.length -= def_buffer->component.def_buffer.byte_list->string.length;
	return def_buffer;
}

// <def_deref_of> := <deref_of_op> <obj_reference>
AMLSymbol *analyse_aml_def_deref_of(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_deref_of = malloc(sizeof(*def_deref_of));
	def_deref_of->parent = parent;
	def_deref_of->string.initial = aml.initial;
	def_deref_of->string.length = 0;
	def_deref_of->type = aml_def_deref_of;
	def_deref_of->component.def_deref_of.deref_of_op = analyse_aml_deref_of_op(def_deref_of, aml);
	def_deref_of->string.length += def_deref_of->component.def_deref_of.deref_of_op->string.length;
	aml.initial += def_deref_of->component.def_deref_of.deref_of_op->string.length;
	aml.length -= def_deref_of->component.def_deref_of.deref_of_op->string.length;
	def_deref_of->component.def_deref_of.obj_reference = analyse_aml_obj_reference(def_deref_of, aml);
	def_deref_of->string.length += def_deref_of->component.def_deref_of.obj_reference->string.length;
	aml.initial += def_deref_of->component.def_deref_of.obj_reference->string.length;
	aml.length -= def_deref_of->component.def_deref_of.obj_reference->string.length;
	return def_deref_of;
}

// <def_device> := <device_op> <pkg_length> <name_string> <term_list>
AMLSymbol *analyse_aml_def_device(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_device = malloc(sizeof(*def_device));
	def_device->parent = parent;
	def_device->string.initial = aml.initial;
	def_device->string.length = 0;
	def_device->type = aml_def_device;
	def_device->component.def_device.device_op = analyse_aml_device_op(def_device, aml);
	def_device->string.length += def_device->component.def_device.device_op->string.length;
	aml.initial += def_device->component.def_device.device_op->string.length;
	aml.length -= def_device->component.def_device.device_op->string.length;
	def_device->component.def_device.pkg_length = analyse_aml_pkg_length(def_device, aml);
	def_device->string.length += def_device->component.def_device.pkg_length->string.length;
	aml.initial += def_device->component.def_device.pkg_length->string.length;
	aml.length = def_device->component.def_device.pkg_length->component.pkg_length.length - def_device->component.def_device.pkg_length->string.length;
	def_device->component.def_device.name_string = analyse_aml_name_string(def_device, aml);
	def_device->string.length += def_device->component.def_device.name_string->string.length;
	aml.initial += def_device->component.def_device.name_string->string.length;
	aml.length -= def_device->component.def_device.name_string->string.length;
	def_device->component.def_device.term_list = analyse_aml_term_list(def_device, aml);
	def_device->string.length += def_device->component.def_device.term_list->string.length;
	aml.initial += def_device->component.def_device.term_list->string.length;
	aml.length -= def_device->component.def_device.term_list->string.length;
	return def_device;
}

// <def_field> := <field_op> <pkg_length> <name_string> <field_flags> <field_list>
AMLSymbol *analyse_aml_def_field(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_field = malloc(sizeof(*def_field));
	def_field->parent = parent;
	def_field->string.initial = aml.initial;
	def_field->string.length = 0;
	def_field->type = aml_def_field;
	def_field->component.def_field.field_op = analyse_aml_field_op(def_field, aml);
	def_field->string.length += def_field->component.def_field.field_op->string.length;
	aml.initial += def_field->component.def_field.field_op->string.length;
	aml.length -= def_field->component.def_field.field_op->string.length;
	def_field->component.def_field.pkg_length = analyse_aml_pkg_length(def_field, aml);
	def_field->string.length += def_field->component.def_field.pkg_length->string.length;
	aml.initial += def_field->component.def_field.pkg_length->string.length;
	aml.length = def_field->component.def_field.pkg_length->component.pkg_length.length - def_field->component.def_field.pkg_length->string.length;
	def_field->component.def_field.name_string = analyse_aml_name_string(def_field, aml);
	def_field->string.length += def_field->component.def_field.name_string->string.length;
	aml.initial += def_field->component.def_field.name_string->string.length;
	aml.length -= def_field->component.def_field.name_string->string.length;
	def_field->component.def_field.field_flags = analyse_aml_field_flags(def_field, aml);
	def_field->string.length += def_field->component.def_field.field_flags->string.length;
	aml.initial += def_field->component.def_field.field_flags->string.length;
	aml.length -= def_field->component.def_field.field_flags->string.length;
	def_field->component.def_field.field_list = analyse_aml_field_list(def_field, aml);
	def_field->string.length += def_field->component.def_field.field_list->string.length;
	aml.initial += def_field->component.def_field.field_list->string.length;
	aml.length -= def_field->component.def_field.field_list->string.length;
	return def_field;
}

// <def_if_else> := <if_op> <pkg_length> <predicate> <term_list> <def_else>
AMLSymbol *analyse_aml_def_if_else(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_if_else = malloc(sizeof(*def_if_else));
	def_if_else->parent = parent;
	def_if_else->string.initial = aml.initial;
	def_if_else->string.length = 0;
	def_if_else->type = aml_def_if_else;
	def_if_else->component.def_if_else.if_op = NULL;
	def_if_else->component.def_if_else.pkg_length = NULL;
	def_if_else->component.def_if_else.predicate = NULL;
	def_if_else->component.def_if_else.term_list = NULL;
	def_if_else->component.def_if_else.def_else = NULL;
	def_if_else->component.def_if_else.if_op = analyse_aml_if_op(def_if_else, aml);
	def_if_else->string.length += def_if_else->component.def_if_else.if_op->string.length;
	aml.initial += def_if_else->component.def_if_else.if_op->string.length;
	aml.length -= def_if_else->component.def_if_else.if_op->string.length;
	def_if_else->component.def_if_else.pkg_length = analyse_aml_pkg_length(def_if_else, aml);
	def_if_else->string.length += def_if_else->component.def_if_else.pkg_length->string.length;
	aml.initial += def_if_else->component.def_if_else.pkg_length->string.length;
	aml.length = def_if_else->component.def_if_else.pkg_length->component.pkg_length.length - def_if_else->component.def_if_else.pkg_length->string.length;
	def_if_else->component.def_if_else.predicate = analyse_aml_predicate(def_if_else, aml);
	def_if_else->string.length += def_if_else->component.def_if_else.predicate->string.length;
	aml.initial += def_if_else->component.def_if_else.predicate->string.length;
	aml.length -= def_if_else->component.def_if_else.predicate->string.length;
	def_if_else->component.def_if_else.term_list = analyse_aml_term_list(def_if_else, aml);
	def_if_else->string.length += def_if_else->component.def_if_else.term_list->string.length;
	aml.initial += def_if_else->component.def_if_else.term_list->string.length;
	aml.length -= def_if_else->component.def_if_else.term_list->string.length;
	if(aml.length)ERROR(); // def_else is unimplemented
	return def_if_else;
}

// <def_increment> := <increment_op> <super_name>
AMLSymbol *analyse_aml_def_increment(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_increment = malloc(sizeof(*def_increment));
	def_increment->parent = parent;
	def_increment->string.initial = aml.initial;
	def_increment->string.length = 0;
	def_increment->type = aml_def_increment;
	def_increment->component.def_increment.increment_op = analyse_aml_increment_op(def_increment, aml);
	def_increment->string.length += def_increment->component.def_increment.increment_op->string.length;
	aml.initial += def_increment->component.def_increment.increment_op->string.length;
	aml.length -= def_increment->component.def_increment.increment_op->string.length;
	def_increment->component.def_increment.super_name = analyse_aml_super_name(def_increment, aml);
	def_increment->string.length += def_increment->component.def_increment.super_name->string.length;
	aml.initial += def_increment->component.def_increment.super_name->string.length;
	aml.length -= def_increment->component.def_increment.super_name->string.length;
	return def_increment;
}

// <def_index> := <index_op> <buff_pkf_str_obj> <index_value> <target>
AMLSymbol *analyse_aml_def_index(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_index = malloc(sizeof(*def_index));
	def_index->parent = parent;
	def_index->string.initial = aml.initial;
	def_index->string.length = 0;
	def_index->type = aml_def_index;
	def_index->component.def_index.index_op = analyse_aml_index_op(def_index, aml);
	def_index->string.length += def_index->component.def_index.index_op->string.length;
	aml.initial += def_index->component.def_index.index_op->string.length;
	aml.length -= def_index->component.def_index.index_op->string.length;
	def_index->component.def_index.buff_pkg_str_obj = analyse_aml_buff_pkg_str_obj(def_index, aml);
	def_index->string.length += def_index->component.def_index.buff_pkg_str_obj->string.length;
	aml.initial += def_index->component.def_index.buff_pkg_str_obj->string.length;
	aml.length -= def_index->component.def_index.buff_pkg_str_obj->string.length;
	def_index->component.def_index.index_value = analyse_aml_index_value(def_index, aml);
	def_index->string.length += def_index->component.def_index.index_value->string.length;
	aml.initial += def_index->component.def_index.index_value->string.length;
	aml.length -= def_index->component.def_index.index_value->string.length;
	def_index->component.def_index.target = analyse_aml_target(def_index, aml);
	def_index->string.length += def_index->component.def_index.target->string.length;
	aml.initial += def_index->component.def_index.target->string.length;
	aml.length -= def_index->component.def_index.target->string.length;
	return def_index;
}

// <def_l_equal> := <l_equal_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_equal(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_l_equal = malloc(sizeof(*def_l_equal));
	def_l_equal->parent = parent;
	def_l_equal->string.initial = aml.initial;
	def_l_equal->string.length = 0;
	def_l_equal->type = aml_def_l_equal;
	def_l_equal->component.def_l_equal.l_equal_op = analyse_aml_l_equal_op(def_l_equal, aml);
	def_l_equal->string.length += def_l_equal->component.def_l_equal.l_equal_op->string.length;
	aml.initial += def_l_equal->component.def_l_equal.l_equal_op->string.length;
	aml.length -= def_l_equal->component.def_l_equal.l_equal_op->string.length;
	for(unsigned int i = 0; i < _countof(def_l_equal->component.def_l_equal.operand); i++)
	{
		def_l_equal->component.def_l_equal.operand[i] = analyse_aml_operand(def_l_equal, aml);
		def_l_equal->string.length += def_l_equal->component.def_l_equal.operand[i]->string.length;
		aml.initial += def_l_equal->component.def_l_equal.operand[i]->string.length;
		aml.length -= def_l_equal->component.def_l_equal.operand[i]->string.length;
	}
	return def_l_equal;
}

// <def_l_greater> := <l_greater_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_greater(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_l_greater = malloc(sizeof(*def_l_greater));
	def_l_greater->parent = parent;
	def_l_greater->string.initial = aml.initial;
	def_l_greater->string.length = 0;
	def_l_greater->type = aml_def_l_greater;
	def_l_greater->component.def_l_greater.l_greater_op = analyse_aml_l_greater_op(def_l_greater, aml);
	def_l_greater->string.length += def_l_greater->component.def_l_greater.l_greater_op->string.length;
	aml.initial += def_l_greater->component.def_l_greater.l_greater_op->string.length;
	aml.length -= def_l_greater->component.def_l_greater.l_greater_op->string.length;
	for(unsigned int i = 0; i < _countof(def_l_greater->component.def_l_greater.operand); i++)
	{
		def_l_greater->component.def_l_greater.operand[i] = analyse_aml_operand(def_l_greater, aml);
		def_l_greater->string.length += def_l_greater->component.def_l_greater.operand[i]->string.length;
		aml.initial += def_l_greater->component.def_l_greater.operand[i]->string.length;
		aml.length -= def_l_greater->component.def_l_greater.operand[i]->string.length;
	}
	return def_l_greater;
}

// <def_l_less> := <l_less_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_less(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_l_less = malloc(sizeof(*def_l_less));
	def_l_less->parent = parent;
	def_l_less->string.initial = aml.initial;
	def_l_less->string.length = 0;
	def_l_less->type = aml_def_l_less;
	def_l_less->component.def_l_less.l_less_op = analyse_aml_l_less_op(def_l_less, aml);
	def_l_less->string.length += def_l_less->component.def_l_less.l_less_op->string.length;
	aml.initial += def_l_less->component.def_l_less.l_less_op->string.length;
	aml.length -= def_l_less->component.def_l_less.l_less_op->string.length;
	for(unsigned int i = 0; i < _countof(def_l_less->component.def_l_less.operand); i++)
	{
		def_l_less->component.def_l_less.operand[i] = analyse_aml_operand(def_l_less, aml);
		def_l_less->string.length += def_l_less->component.def_l_less.operand[i]->string.length;
		aml.initial += def_l_less->component.def_l_less.operand[i]->string.length;
		aml.length -= def_l_less->component.def_l_less.operand[i]->string.length;
	}
	return def_l_less;
}

// <def_l_or> := <l_or_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_or(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_l_or = malloc(sizeof(*def_l_or));
	def_l_or->parent = parent;
	def_l_or->string.initial = aml.initial;
	def_l_or->string.length = 0;
	def_l_or->type = aml_def_l_or;
	def_l_or->component.def_l_or.l_or_op = analyse_aml_l_or_op(def_l_or, aml);
	def_l_or->string.length += def_l_or->component.def_l_or.l_or_op->string.length;
	aml.initial += def_l_or->component.def_l_or.l_or_op->string.length;
	aml.length -= def_l_or->component.def_l_or.l_or_op->string.length;
	for(unsigned int i = 0; i < _countof(def_l_or->component.def_l_or.operand); i++)
	{
		def_l_or->component.def_l_or.operand[i] = analyse_aml_operand(def_l_or, aml);
		def_l_or->string.length += def_l_or->component.def_l_or.operand[i]->string.length;
		aml.initial += def_l_or->component.def_l_or.operand[i]->string.length;
		aml.length -= def_l_or->component.def_l_or.operand[i]->string.length;
	}
	return def_l_or;
}

// <def_method> := <method_op> <pkg_length> <name_string> <method_flags> <term_list>
AMLSymbol *analyse_aml_def_method(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_method = malloc(sizeof(*def_method));
	def_method->parent = parent;
	def_method->string.initial = aml.initial;
	def_method->string.length = 0;
	def_method->type = aml_def_method;
	def_method->component.def_method.method_op = analyse_aml_method_op(def_method, aml);
	def_method->string.length += def_method->component.def_method.method_op->string.length;
	aml.initial += def_method->component.def_method.method_op->string.length;
	aml.length -= def_method->component.def_method.method_op->string.length;
	def_method->component.def_method.pkg_length = analyse_aml_pkg_length(def_method, aml);
	def_method->string.length += def_method->component.def_method.pkg_length->string.length;
	aml.initial += def_method->component.def_method.pkg_length->string.length;
	aml.length = def_method->component.def_method.pkg_length->component.pkg_length.length - def_method->component.def_method.pkg_length->string.length;
	def_method->component.def_method.name_string = analyse_aml_name_string(def_method, aml);
	def_method->string.length += def_method->component.def_method.name_string->string.length;
	aml.initial += def_method->component.def_method.name_string->string.length;
	aml.length -= def_method->component.def_method.name_string->string.length;
	def_method->component.def_method.method_flags = analyse_aml_method_flags(def_method, aml);
	def_method->string.length += def_method->component.def_method.method_flags->string.length;
	aml.initial += def_method->component.def_method.method_flags->string.length;
	aml.length -= def_method->component.def_method.method_flags->string.length;
	def_method->component.def_method.term_list = analyse_aml_term_list(def_method, aml);
	def_method->string.length += def_method->component.def_method.term_list->string.length;
	aml.initial += def_method->component.def_method.term_list->string.length;
	aml.length -= def_method->component.def_method.term_list->string.length;
	return def_method;
}

// <def_mutex> := <mutex_op> <name_string> <sync_flags>
AMLSymbol *analyse_aml_def_mutex(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_mutex = malloc(sizeof(*def_mutex));
	def_mutex->parent = parent;
	def_mutex->string.initial = aml.initial;
	def_mutex->string.length = 0;
	def_mutex->type = aml_def_mutex;
	def_mutex->component.def_mutex.mutex_op = analyse_aml_mutex_op(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.mutex_op->string.length;
	def_mutex->string.length += def_mutex->component.def_mutex.mutex_op->string.length;
	aml.initial += def_mutex->component.def_mutex.mutex_op->string.length;
	aml.length -= def_mutex->component.def_mutex.mutex_op->string.length;
	def_mutex->component.def_mutex.name_string = analyse_aml_name_string(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.name_string->string.length;
	def_mutex->string.length += def_mutex->component.def_mutex.name_string->string.length;
	aml.initial += def_mutex->component.def_mutex.name_string->string.length;
	def_mutex->component.def_mutex.sync_flags = analyse_aml_sync_flags(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.sync_flags->string.length;
	def_mutex->string.length += def_mutex->component.def_mutex.sync_flags->string.length;
	aml.initial += def_mutex->component.def_mutex.sync_flags->string.length;
	return def_mutex;
}

// <def_name> := <name_op> <name_string> <data_ref_object>
AMLSymbol *analyse_aml_def_name(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_name = malloc(sizeof(*def_name));
	def_name->parent = parent;
	def_name->string.initial = aml.initial;
	def_name->string.length = 0;
	def_name->type = aml_def_name;
	def_name->component.def_name.name_op = analyse_aml_name_op(def_name, aml);
	def_name->string.length += def_name->component.def_name.name_op->string.length;
	aml.initial += def_name->component.def_name.name_op->string.length;
	aml.length -= def_name->component.def_name.name_op->string.length;
	def_name->component.def_name.name_string = analyse_aml_name_string(def_name, aml);;
	def_name->string.length += def_name->component.def_name.name_string->string.length;
	aml.initial += def_name->component.def_name.name_string->string.length;
	aml.length -= def_name->component.def_name.name_string->string.length;
	def_name->component.def_name.data_ref_object = analyse_aml_data_ref_object(def_name, aml);;
	def_name->string.length += def_name->component.def_name.data_ref_object->string.length;
	aml.initial += def_name->component.def_name.data_ref_object->string.length;
	aml.length -= def_name->component.def_name.data_ref_object->string.length;
	return def_name;
}

// <def_op_region> := <op_region_op> <name_string> <region_space> <region_offset> <region_len>
AMLSymbol *analyse_aml_def_op_region(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_op_region = malloc(sizeof(*def_op_region));
	def_op_region->parent = parent;
	def_op_region->string.initial = aml.initial;
	def_op_region->string.length = 0;
	def_op_region->type = aml_def_op_region;
	def_op_region->component.def_op_region.op_region_op = analyse_aml_op_region_op(def_op_region, aml);
	def_op_region->string.length += def_op_region->component.def_op_region.op_region_op->string.length;
	aml.initial += def_op_region->component.def_op_region.op_region_op->string.length;
	aml.length -= def_op_region->component.def_op_region.op_region_op->string.length;
	def_op_region->component.def_op_region.name_string = analyse_aml_name_string(def_op_region, aml);
	def_op_region->string.length += def_op_region->component.def_op_region.name_string->string.length;
	aml.initial += def_op_region->component.def_op_region.name_string->string.length;
	aml.length -= def_op_region->component.def_op_region.name_string->string.length;
	def_op_region->component.def_op_region.region_space = analyse_aml_region_space(def_op_region, aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_space->string.length;
	aml.initial += def_op_region->component.def_op_region.region_space->string.length;
	aml.length -= def_op_region->component.def_op_region.region_space->string.length;
	def_op_region->component.def_op_region.region_offset = analyse_aml_region_offset(def_op_region, aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_offset->string.length;
	aml.initial += def_op_region->component.def_op_region.region_offset->string.length;
	aml.length -= def_op_region->component.def_op_region.region_offset->string.length;
	def_op_region->component.def_op_region.region_len = analyse_aml_region_len(def_op_region, aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_len->string.length;
	aml.initial += def_op_region->component.def_op_region.region_len->string.length;
	aml.length -= def_op_region->component.def_op_region.region_len->string.length;
	return def_op_region;
}

// <def_package> := <package_op> <pkg_length> <num_elements> <package_element_list>
AMLSymbol *analyse_aml_def_package(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_package = malloc(sizeof(*def_package));
	def_package->parent = parent;
	def_package->string.initial = aml.initial;
	def_package->string.length = 0;
	def_package->type = aml_def_package;
	def_package->component.def_package.package_op = analyse_aml_package_op(def_package, aml);
	def_package->string.length += def_package->component.def_package.package_op->string.length;
	aml.initial += def_package->component.def_package.package_op->string.length;
	aml.length -= def_package->component.def_package.package_op->string.length;
	def_package->component.def_package.pkg_length = analyse_aml_pkg_length(def_package, aml);
	def_package->string.length += def_package->component.def_package.pkg_length->string.length;
	aml.initial += def_package->component.def_package.pkg_length->string.length;
	aml.length -= def_package->component.def_package.pkg_length->string.length;
	def_package->component.def_package.num_elements = analyse_aml_num_elements(def_package, aml);
	def_package->string.length += def_package->component.def_package.num_elements->string.length;
	aml.initial += def_package->component.def_package.num_elements->string.length;
	aml.length -= def_package->component.def_package.num_elements->string.length;
	def_package->component.def_package.package_element_list = analyse_aml_package_element_list(def_package, aml);
	def_package->string.length += def_package->component.def_package.package_element_list->string.length;
	aml.initial += def_package->component.def_package.package_element_list->string.length;
	aml.length -= def_package->component.def_package.package_element_list->string.length;
	return def_package;
}

// <def_release> := <release_op> <mutex_object>
AMLSymbol *analyse_aml_def_release(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_release = malloc(sizeof(*def_release));
	def_release->parent = parent;
	def_release->string.initial = aml.initial;
	def_release->string.length = 0;
	def_release->type = aml_def_release;
	def_release->component.def_release.release_op = analyse_aml_release_op(def_release, aml);
	def_release->string.length += def_release->component.def_release.release_op->string.length;
	aml.initial += def_release->component.def_release.release_op->string.length;
	aml.length -= def_release->component.def_release.release_op->string.length;
	def_release->component.def_release.mutex_object = analyse_aml_mutex_object(def_release, aml);
	def_release->string.length += def_release->component.def_release.mutex_object->string.length;
	aml.initial += def_release->component.def_release.mutex_object->string.length;
	aml.length -= def_release->component.def_release.mutex_object->string.length;
	return def_release;
}

// <def_return> := <return_op> <arg_object>
AMLSymbol *analyse_aml_def_return(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_return = malloc(sizeof(*def_return));
	def_return->parent = parent;
	def_return->string.initial = aml.initial;
	def_return->string.length = 0;
	def_return->type = aml_def_return;
	def_return->component.def_return.return_op = analyse_aml_return_op(def_return, aml);
	def_return->string.length += def_return->component.def_return.return_op->string.length;
	aml.initial += def_return->component.def_return.return_op->string.length;
	aml.length -= def_return->component.def_return.return_op->string.length;
	def_return->component.def_return.arg_object = analyse_aml_arg_object(def_return, aml);
	def_return->string.length += def_return->component.def_return.arg_object->string.length;
	aml.initial += def_return->component.def_return.arg_object->string.length;
	aml.length -= def_return->component.def_return.arg_object->string.length;
	return def_return;
}

// <def_scope> := <scope_op> <pkg_length> <name_string> <term_list>
AMLSymbol *analyse_aml_def_scope(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_scope = malloc(sizeof(*def_scope));
	def_scope->parent = parent;
	def_scope->string.initial = aml.initial;
	def_scope->string.length = 0;
	def_scope->type = aml_def_scope;
	def_scope->component.def_scope.scope_op = analyse_aml_scope_op(def_scope, aml);
	def_scope->string.length += def_scope->component.def_scope.scope_op->string.length;
	aml.initial += def_scope->component.def_scope.scope_op->string.length;
	aml.length -= def_scope->component.def_scope.scope_op->string.length;
	def_scope->component.def_scope.pkg_length = analyse_aml_pkg_length(def_scope, aml);
	def_scope->string.length += def_scope->component.def_scope.pkg_length->string.length;
	aml.initial += def_scope->component.def_scope.pkg_length->string.length;
	aml.length = def_scope->component.def_scope.pkg_length->component.pkg_length.length - def_scope->component.def_scope.pkg_length->string.length;
	def_scope->component.def_scope.name_string = analyse_aml_name_string(def_scope, aml);
	def_scope->string.length += def_scope->component.def_scope.name_string->string.length;
	aml.initial += def_scope->component.def_scope.name_string->string.length;
	aml.length -= def_scope->component.def_scope.name_string->string.length;
	def_scope->component.def_scope.term_list = analyse_aml_term_list(def_scope, aml);
	def_scope->string.length += def_scope->component.def_scope.term_list->string.length;
	aml.initial += def_scope->component.def_scope.term_list->string.length;
	aml.length -= def_scope->component.def_scope.term_list->string.length;
	return def_scope;
}

// <def_shift_left> := <shift_left_op> <operand> <shift_count> <target>
AMLSymbol *analyse_aml_def_shift_left(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_shift_left = malloc(sizeof(*def_shift_left));
	def_shift_left->parent = parent;
	def_shift_left->string.initial = aml.initial;
	def_shift_left->string.length = 0;
	def_shift_left->type = aml_def_shift_left;
	def_shift_left->component.def_shift_left.shift_left_op = analyse_aml_shift_left_op(def_shift_left, aml);
	def_shift_left->string.length += def_shift_left->component.def_shift_left.shift_left_op->string.length;
	aml.initial += def_shift_left->component.def_shift_left.shift_left_op->string.length;
	aml.length -= def_shift_left->component.def_shift_left.shift_left_op->string.length;
	def_shift_left->component.def_shift_left.operand = analyse_aml_operand(def_shift_left, aml);
	def_shift_left->string.length += def_shift_left->component.def_shift_left.operand->string.length;
	aml.initial += def_shift_left->component.def_shift_left.operand->string.length;
	aml.length -= def_shift_left->component.def_shift_left.operand->string.length;
	def_shift_left->component.def_shift_left.shift_count = analyse_aml_shift_count(def_shift_left, aml);
	def_shift_left->string.length += def_shift_left->component.def_shift_left.shift_count->string.length;
	aml.initial += def_shift_left->component.def_shift_left.shift_count->string.length;
	aml.length -= def_shift_left->component.def_shift_left.shift_count->string.length;
	def_shift_left->component.def_shift_left.target = analyse_aml_target(def_shift_left, aml);
	def_shift_left->string.length += def_shift_left->component.def_shift_left.target->string.length;
	aml.initial += def_shift_left->component.def_shift_left.target->string.length;
	aml.length -= def_shift_left->component.def_shift_left.target->string.length;
	return def_shift_left;
}

// <def_shift_right> := <shift_right_op> <operand> <shift_count> <target>
AMLSymbol *analyse_aml_def_shift_right(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_shift_right = malloc(sizeof(*def_shift_right));
	def_shift_right->parent = parent;
	def_shift_right->string.initial = aml.initial;
	def_shift_right->string.length = 0;
	def_shift_right->type = aml_def_shift_right;
	def_shift_right->component.def_shift_right.shift_right_op = analyse_aml_shift_right_op(def_shift_right, aml);
	def_shift_right->string.length += def_shift_right->component.def_shift_right.shift_right_op->string.length;
	aml.initial += def_shift_right->component.def_shift_right.shift_right_op->string.length;
	aml.length -= def_shift_right->component.def_shift_right.shift_right_op->string.length;
	def_shift_right->component.def_shift_right.operand = analyse_aml_operand(def_shift_right, aml);
	def_shift_right->string.length += def_shift_right->component.def_shift_right.operand->string.length;
	aml.initial += def_shift_right->component.def_shift_right.operand->string.length;
	aml.length -= def_shift_right->component.def_shift_right.operand->string.length;
	def_shift_right->component.def_shift_right.shift_count = analyse_aml_shift_count(def_shift_right, aml);
	def_shift_right->string.length += def_shift_right->component.def_shift_right.shift_count->string.length;
	aml.initial += def_shift_right->component.def_shift_right.shift_count->string.length;
	aml.length -= def_shift_right->component.def_shift_right.shift_count->string.length;
	def_shift_right->component.def_shift_right.target = analyse_aml_target(def_shift_right, aml);
	def_shift_right->string.length += def_shift_right->component.def_shift_right.target->string.length;
	aml.initial += def_shift_right->component.def_shift_right.target->string.length;
	aml.length -= def_shift_right->component.def_shift_right.target->string.length;
	return def_shift_right;
}

// <def_size_of> := <size_of_op> <super_name>
AMLSymbol *analyse_aml_def_size_of(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_size_of = malloc(sizeof(*def_size_of));
	def_size_of->parent = parent;
	def_size_of->string.initial = aml.initial;
	def_size_of->string.length = 0;
	def_size_of->type = aml_def_size_of;
	def_size_of->component.def_size_of.size_of_op = analyse_aml_size_of_op(def_size_of, aml);
	def_size_of->string.length += def_size_of->component.def_size_of.size_of_op->string.length;
	aml.initial += def_size_of->component.def_size_of.size_of_op->string.length;
	aml.length -= def_size_of->component.def_size_of.size_of_op->string.length;
	def_size_of->component.def_size_of.super_name = analyse_aml_super_name(def_size_of, aml);
	def_size_of->string.length += def_size_of->component.def_size_of.super_name->string.length;
	aml.initial += def_size_of->component.def_size_of.super_name->string.length;
	aml.length -= def_size_of->component.def_size_of.super_name->string.length;
	return def_size_of;
}

// <def_store> := <store_op> <term_arg> <super_name>
AMLSymbol *analyse_aml_def_store(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_store = malloc(sizeof(*def_store));
	def_store->parent = parent;
	def_store->string.initial = aml.initial;
	def_store->string.length = 0;
	def_store->type = aml_def_store;
	def_store->component.def_store.store_op = analyse_aml_store_op(def_store, aml);
	def_store->string.length += def_store->component.def_store.store_op->string.length;
	aml.initial += def_store->component.def_store.store_op->string.length;
	aml.length -= def_store->component.def_store.store_op->string.length;
	def_store->component.def_store.term_arg = analyse_aml_term_arg(def_store, aml);
	def_store->string.length += def_store->component.def_store.term_arg->string.length;
	aml.initial += def_store->component.def_store.term_arg->string.length;
	aml.length -= def_store->component.def_store.term_arg->string.length;
	def_store->component.def_store.super_name = analyse_aml_super_name(def_store, aml);
	def_store->string.length += def_store->component.def_store.super_name->string.length;
	aml.initial += def_store->component.def_store.super_name->string.length;
	aml.length -= def_store->component.def_store.super_name->string.length;
	return def_store;
}

// <def_subtract> := <subtract_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_subtract(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_subtract = malloc(sizeof(*def_subtract));
	def_subtract->parent = parent;
	def_subtract->string.initial = aml.initial;
	def_subtract->string.length = 0;
	def_subtract->type = aml_def_subtract;
	def_subtract->component.def_subtract.subtract_op = analyse_aml_subtract_op(def_subtract, aml);
	def_subtract->string.length += def_subtract->component.def_subtract.subtract_op->string.length;
	aml.initial += def_subtract->component.def_subtract.subtract_op->string.length;
	aml.length -= def_subtract->component.def_subtract.subtract_op->string.length;
	for(unsigned int i = 0; i < _countof(def_subtract->component.def_subtract.operand); i++)
	{
		def_subtract->component.def_subtract.operand[i] = analyse_aml_operand(def_subtract, aml);
		def_subtract->string.length += def_subtract->component.def_subtract.operand[i]->string.length;
		aml.initial += def_subtract->component.def_subtract.operand[i]->string.length;
		aml.length -= def_subtract->component.def_subtract.operand[i]->string.length;
	}
	def_subtract->component.def_subtract.target = analyse_aml_target(def_subtract, aml);
	def_subtract->string.length += def_subtract->component.def_subtract.target->string.length;
	aml.initial += def_subtract->component.def_subtract.target->string.length;
	aml.length -= def_subtract->component.def_subtract.target->string.length;
	return def_subtract;
}

// <def_to_buffer> := <to_buffer_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_buffer(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_to_buffer = malloc(sizeof(*def_to_buffer));
	def_to_buffer->parent = parent;
	def_to_buffer->string.initial = aml.initial;
	def_to_buffer->string.length = 0;
	def_to_buffer->type = aml_def_to_buffer;
	def_to_buffer->component.def_to_buffer.to_buffer_op = analyse_aml_to_buffer_op(def_to_buffer, aml);
	def_to_buffer->string.length += def_to_buffer->component.def_to_buffer.to_buffer_op->string.length;
	aml.initial += def_to_buffer->component.def_to_buffer.to_buffer_op->string.length;
	aml.length -= def_to_buffer->component.def_to_buffer.to_buffer_op->string.length;
	def_to_buffer->component.def_to_buffer.operand = analyse_aml_operand(def_to_buffer, aml);
	def_to_buffer->string.length += def_to_buffer->component.def_to_buffer.operand->string.length;
	aml.initial += def_to_buffer->component.def_to_buffer.operand->string.length;
	aml.length -= def_to_buffer->component.def_to_buffer.operand->string.length;
	def_to_buffer->component.def_to_buffer.target = analyse_aml_target(def_to_buffer, aml);
	def_to_buffer->string.length += def_to_buffer->component.def_to_buffer.target->string.length;
	aml.initial += def_to_buffer->component.def_to_buffer.target->string.length;
	aml.length -= def_to_buffer->component.def_to_buffer.target->string.length;
	return def_to_buffer;
}

// <def_to_hex_string> := <to_hex_string_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_hex_string(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_to_hex_string = malloc(sizeof(*def_to_hex_string));
	def_to_hex_string->parent = parent;
	def_to_hex_string->string.initial = aml.initial;
	def_to_hex_string->string.length = 0;
	def_to_hex_string->type = aml_def_to_hex_string;
	def_to_hex_string->component.def_to_hex_string.to_hex_string_op = analyse_aml_to_hex_string_op(def_to_hex_string, aml);
	def_to_hex_string->string.length += def_to_hex_string->component.def_to_hex_string.to_hex_string_op->string.length;
	aml.initial += def_to_hex_string->component.def_to_hex_string.to_hex_string_op->string.length;
	aml.length -= def_to_hex_string->component.def_to_hex_string.to_hex_string_op->string.length;
	def_to_hex_string->component.def_to_hex_string.operand = analyse_aml_operand(def_to_hex_string, aml);
	def_to_hex_string->string.length += def_to_hex_string->component.def_to_hex_string.operand->string.length;
	aml.initial += def_to_hex_string->component.def_to_hex_string.operand->string.length;
	aml.length -= def_to_hex_string->component.def_to_hex_string.operand->string.length;
	def_to_hex_string->component.def_to_hex_string.target = analyse_aml_target(def_to_hex_string, aml);
	def_to_hex_string->string.length += def_to_hex_string->component.def_to_hex_string.target->string.length;
	aml.initial += def_to_hex_string->component.def_to_hex_string.target->string.length;
	aml.length -= def_to_hex_string->component.def_to_hex_string.target->string.length;
	return def_to_hex_string;
}

// <def_while> := <while_op> <pkg_length> <predicate> <term_list>
AMLSymbol *analyse_aml_def_while(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *def_while = malloc(sizeof(*def_while));
	def_while->parent = parent;
	def_while->string.initial = aml.initial;
	def_while->string.length = 0;
	def_while->type = aml_def_while;
	def_while->component.def_while.while_op = analyse_aml_while_op(def_while, aml);
	def_while->string.length += def_while->component.def_while.while_op->string.length;
	aml.initial += def_while->component.def_while.while_op->string.length;
	aml.length -= def_while->component.def_while.while_op->string.length;
	def_while->component.def_while.pkg_length = analyse_aml_pkg_length(def_while, aml);
	def_while->string.length += def_while->component.def_while.pkg_length->string.length;
	aml.initial += def_while->component.def_while.pkg_length->string.length;
	aml.length = def_while->component.def_while.pkg_length->component.pkg_length.length - def_while->component.def_while.pkg_length->string.length;
	def_while->component.def_while.predicate = analyse_aml_predicate(def_while, aml);
	def_while->string.length += def_while->component.def_while.predicate->string.length;
	aml.initial += def_while->component.def_while.predicate->string.length;
	aml.length -= def_while->component.def_while.predicate->string.length;
	def_while->component.def_while.term_list = analyse_aml_term_list(def_while, aml);
	def_while->string.length += def_while->component.def_while.term_list->string.length;
	aml.initial += def_while->component.def_while.term_list->string.length;
	aml.length -= def_while->component.def_while.term_list->string.length;
	return def_while;
}

// <deref_of_op> := AML_BYTE_DEREF_OF_OP
AMLSymbol *analyse_aml_deref_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *deref_of_op = malloc(sizeof(*deref_of_op));
	deref_of_op->parent = parent;
	deref_of_op->string.initial = aml.initial;
	deref_of_op->string.length = 1;
	deref_of_op->type = aml_deref_of_op;
	if(*deref_of_op->string.initial != AML_BYTE_DEREF_OF_OP)ERROR(); // Incorrect deref of op
	return deref_of_op;
}

// <device_op> := <ext_op_prefix> <device_op_suffix>
AMLSymbol *analyse_aml_device_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *device_op = malloc(sizeof(*device_op));
	device_op->parent = parent;
	device_op->string.initial = aml.initial;
	device_op->string.length = 0;
	device_op->type = aml_device_op;
	device_op->component.device_op.ext_op_prefix = analyse_aml_ext_op_prefix(device_op, aml);
	device_op->string.length += device_op->component.device_op.ext_op_prefix->string.length;
	aml.initial += device_op->component.device_op.ext_op_prefix->string.length;
	aml.length -= device_op->component.device_op.ext_op_prefix->string.length;
	device_op->component.device_op.device_op_suffix = analyse_aml_device_op_suffix(device_op, aml);
	device_op->string.length += device_op->component.device_op.device_op_suffix->string.length;
	aml.initial += device_op->component.device_op.device_op_suffix->string.length;
	aml.length -= device_op->component.device_op.device_op_suffix->string.length;
	return device_op;
}

// <device_op_suffix> := AML_BYTE_DEVICE_OP
AMLSymbol *analyse_aml_device_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *device_op_suffix = malloc(sizeof(*device_op_suffix));
	device_op_suffix->parent = parent;
	device_op_suffix->string.initial = aml.initial;
	device_op_suffix->string.length = 1;
	device_op_suffix->type = aml_device_op_suffix;
	if(*aml.initial != AML_BYTE_DEVICE_OP)ERROR(); // Incorrect device op
	return device_op_suffix;
}

// <digit_char> := '0' - '9'
AMLSymbol *analyse_aml_digit_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *digit_char = malloc(sizeof(*digit_char));
	digit_char->parent = parent;
	digit_char->string.initial = aml.initial;
	digit_char->string.length = 1;
	digit_char->type = aml_digit_char;
	if(!('0' <= *digit_char->string.initial && *digit_char->string.initial <= '9'))ERROR(); // Incorrect digit char
	return digit_char;
}

// <dual_name_path> := <dual_name_prefix> <name_seg> <name_seg>
AMLSymbol *analyse_aml_dual_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *dual_name_path = malloc(sizeof(*dual_name_path));
	char *string_writer = dual_name_path->component.dual_name_path.string;
	dual_name_path->parent = parent;
	dual_name_path->string.initial = aml.initial;
	dual_name_path->string.length = 0;
	dual_name_path->type = aml_dual_name_path;
	dual_name_path->component.dual_name_path.dual_name_prefix = analyse_aml_dual_name_prefix(dual_name_path, aml);
	dual_name_path->string.length += dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	aml.initial += dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	aml.length -= dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	for(AMLSymbol **name_seg = dual_name_path->component.dual_name_path.name_seg; name_seg != dual_name_path->component.dual_name_path.name_seg + _countof(dual_name_path->component.dual_name_path.name_seg); name_seg++)
	{
		*name_seg = analyse_aml_name_seg(dual_name_path, aml);
		dual_name_path->string.length += (*name_seg)->string.length;
		aml.initial += (*name_seg)->string.length;
		aml.length -= (*name_seg)->string.length;
		strcpy(string_writer, (*name_seg)->component.name_seg.string);
		string_writer += strlen(string_writer);
	}
	*string_writer = '\0';
	return dual_name_path;
}

// <dual_name_prefix> := AML_BYTE_DUAL_NAME_PREFIX
AMLSymbol *analyse_aml_dual_name_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *dual_name_prefix = malloc(sizeof(*dual_name_prefix));
	dual_name_prefix->parent = parent;
	dual_name_prefix->string.initial = aml.initial;
	dual_name_prefix->string.length = 1;
	dual_name_prefix->type = aml_dual_name_prefix;
	if(*dual_name_prefix->string.initial != AML_BYTE_DUAL_NAME_PREFIX)ERROR(); // Incorrect dual name prefix
	return dual_name_prefix;
}

// <dword_const> := <dword_prefix> <dword_data>
AMLSymbol *analyse_aml_dword_const(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *dword_const = malloc(sizeof(*dword_const));
	dword_const->parent = parent;
	dword_const->string.initial = aml.initial;
	dword_const->string.length = 0;
	dword_const->type = aml_dword_const;
	dword_const->component.dword_const.dword_prefix = analyse_aml_dword_prefix(dword_const, aml);
	dword_const->string.length += dword_const->component.dword_const.dword_prefix->string.length;
	aml.initial += dword_const->component.dword_const.dword_prefix->string.length;
	aml.length -= dword_const->component.dword_const.dword_prefix->string.length;
	dword_const->component.dword_const.dword_data = analyse_aml_dword_data(dword_const, aml);
	dword_const->string.length += dword_const->component.dword_const.dword_data->string.length;
	aml.initial += dword_const->component.dword_const.dword_data->string.length;
	aml.length -= dword_const->component.dword_const.dword_data->string.length;
	dword_const->component.dword_const.value = dword_const->component.dword_const.dword_data->component.dword_data.value;
	return dword_const;
}

// <dword_data> := <word_data> <word_data>
AMLSymbol *analyse_aml_dword_data(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *dword_data = malloc(sizeof(*dword_data));
	dword_data->parent = parent;
	dword_data->string.initial = aml.initial;
	dword_data->string.length = 0;
	dword_data->type = aml_dword_data;
	for(AMLSymbol **word_data = dword_data->component.dword_data.word_data; word_data != dword_data->component.dword_data.word_data + _countof(dword_data->component.dword_data.word_data); word_data++)
	{
		*word_data = analyse_aml_word_data(dword_data, aml);
		dword_data->string.length += (*word_data)->string.length;
		aml.initial += (*word_data)->string.length;
		aml.length -= (*word_data)->string.length;
	}
	dword_data->component.dword_data.value = *(unsigned int const *)dword_data->component.dword_data.word_data[0]->string.initial;
	return dword_data;
}

// <dword_prefix> := AML_BYTE_DWORD_PREFIX
AMLSymbol *analyse_aml_dword_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *dword_prefix = malloc(sizeof(*dword_prefix));
	dword_prefix->parent = parent;
	dword_prefix->string.initial = aml.initial;
	dword_prefix->string.length = 1;
	dword_prefix->type = aml_dword_prefix;
	if(*dword_prefix->string.initial != AML_BYTE_DWORD_PREFIX)ERROR(); // Incorrect dword prefix
	return dword_prefix;
}

// <expression_opcode> := <def_acquire> | <def_add> | <def_and> | <def_buffer> | <def_concat> | <def_concat_res> | <def_cond_ref_of> | <def_copy_object> | <def_decrement> | <def_deref_of> | <def_divide> | <def_find_set_left_bit> | <def_find_set_right_bit> | <def_from_bcd> | <def_increment> | <def_index> | <def_l_and> | <def_l_equal> | <def_l_greater> | <def_l_greater_equal> | <def_l_less> | <def_l_less_equal> | <def_mid> | <def_l_not> | <def_l_not_equal> | <def_load_table> | <def_l_or> | <def_match> | <def_mod> | <def_multiply> | <def_nand> | <def_nor> | <def_not> | <def_object_type> | <def_or> | <def_package> | <def_var_package> | <def_ref_of> | <def_shift_left> | <def_shift_right> | <def_size_of> | <def_store> | <def_subtract> | <def_timer> | <def_to_bcd> | <def_to_buffer> | <def_to_decimal_string> | <def_to_hex_string> | <def_to_integer> | <def_to_string> | <def_wait> | <def_xor> | <method_invocation>
AMLSymbol *analyse_aml_expression_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *expression_opcode = malloc(sizeof(*expression_opcode));
	expression_opcode->parent = parent;
	expression_opcode->string.initial = aml.initial;
	expression_opcode->string.length = 0;
	expression_opcode->type = aml_expression_opcode;
	expression_opcode->component.expression_opcode.def_add = NULL;
	expression_opcode->component.expression_opcode.def_and = NULL;
	expression_opcode->component.expression_opcode.def_acquire = NULL;
	expression_opcode->component.expression_opcode.def_buffer = NULL;
	expression_opcode->component.expression_opcode.def_concat = NULL;
	expression_opcode->component.expression_opcode.def_concat_res = NULL;
	expression_opcode->component.expression_opcode.def_cond_ref_of = NULL;
	expression_opcode->component.expression_opcode.def_copy_object = NULL;
	expression_opcode->component.expression_opcode.def_decrement = NULL;
	expression_opcode->component.expression_opcode.def_deref_of = NULL;
	expression_opcode->component.expression_opcode.def_divide = NULL;
	expression_opcode->component.expression_opcode.def_find_set_left_bit = NULL;
	expression_opcode->component.expression_opcode.def_find_set_right_bit = NULL;
	expression_opcode->component.expression_opcode.def_from_bcd = NULL;
	expression_opcode->component.expression_opcode.def_increment = NULL;
	expression_opcode->component.expression_opcode.def_index = NULL;
	expression_opcode->component.expression_opcode.def_l_and = NULL;
	expression_opcode->component.expression_opcode.def_l_equal = NULL;
	expression_opcode->component.expression_opcode.def_l_greater = NULL;
	expression_opcode->component.expression_opcode.def_l_greater_equal = NULL;
	expression_opcode->component.expression_opcode.def_l_less = NULL;
	expression_opcode->component.expression_opcode.def_l_less_equal = NULL;
	expression_opcode->component.expression_opcode.def_l_not = NULL;
	expression_opcode->component.expression_opcode.def_l_not_equal = NULL;
	expression_opcode->component.expression_opcode.def_l_or = NULL;
	expression_opcode->component.expression_opcode.def_load_table = NULL;
	expression_opcode->component.expression_opcode.def_match = NULL;
	expression_opcode->component.expression_opcode.def_mid = NULL;
	expression_opcode->component.expression_opcode.def_mod = NULL;
	expression_opcode->component.expression_opcode.def_multiply = NULL;
	expression_opcode->component.expression_opcode.def_nand = NULL;
	expression_opcode->component.expression_opcode.def_nor = NULL;
	expression_opcode->component.expression_opcode.def_not = NULL;
	expression_opcode->component.expression_opcode.def_object_type = NULL;
	expression_opcode->component.expression_opcode.def_or = NULL;
	expression_opcode->component.expression_opcode.def_package = NULL;
	expression_opcode->component.expression_opcode.def_ref_of = NULL;
	expression_opcode->component.expression_opcode.def_shift_left = NULL;
	expression_opcode->component.expression_opcode.def_shift_right = NULL;
	expression_opcode->component.expression_opcode.def_size_of = NULL;
	expression_opcode->component.expression_opcode.def_store = NULL;
	expression_opcode->component.expression_opcode.def_subtract = NULL;
	expression_opcode->component.expression_opcode.def_timer = NULL;
	expression_opcode->component.expression_opcode.def_to_bcd = NULL;
	expression_opcode->component.expression_opcode.def_to_buffer = NULL;
	expression_opcode->component.expression_opcode.def_to_decimal_string = NULL;
	expression_opcode->component.expression_opcode.def_to_hex_string = NULL;
	expression_opcode->component.expression_opcode.def_to_integer = NULL;
	expression_opcode->component.expression_opcode.def_to_string = NULL;
	expression_opcode->component.expression_opcode.def_var_package = NULL;
	expression_opcode->component.expression_opcode.def_wait = NULL;
	expression_opcode->component.expression_opcode.def_xor = NULL;
	expression_opcode->component.expression_opcode.method_invocation = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_DEREF_OF_OP:
		expression_opcode->component.expression_opcode.def_deref_of = analyse_aml_def_deref_of(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_deref_of->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_ACQUIRE_OP:
			expression_opcode->component.expression_opcode.def_acquire = analyse_aml_def_acquire(expression_opcode, aml);
			expression_opcode->string.length += expression_opcode->component.expression_opcode.def_acquire->string.length;
			break;
		default:
			ERROR(); // Syntax error
			printf_serial("aml.initial[0] = %#04.2x\n", aml.initial[0]);
			printf_serial("aml.initial[1] = %#04.2x\n", aml.initial[1]);
			break;
		}
		break;
	case AML_BYTE_INCREMENT_OP:
		expression_opcode->component.expression_opcode.def_increment = analyse_aml_def_increment(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_increment->string.length;
		break;
	case AML_BYTE_INDEX_OP:
		expression_opcode->component.expression_opcode.def_index = analyse_aml_def_index(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_index->string.length;
		break;
	case AML_BYTE_L_EQUAL_OP:
		expression_opcode->component.expression_opcode.def_l_equal = analyse_aml_def_l_equal(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_equal->string.length;
		break;
	case AML_BYTE_L_GREATER_OP:
		expression_opcode->component.expression_opcode.def_l_greater = analyse_aml_def_l_greater(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_greater->string.length;
		break;
	case AML_BYTE_L_LESS_OP:
		expression_opcode->component.expression_opcode.def_l_less = analyse_aml_def_l_less(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_less->string.length;
		break;
	case AML_BYTE_L_OR_OP:
		expression_opcode->component.expression_opcode.def_l_or = analyse_aml_def_l_or(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_or->string.length;
		break;
	case AML_BYTE_PACKAGE_OP:
		expression_opcode->component.expression_opcode.def_package = analyse_aml_def_package(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_package->string.length;
		break;
	case AML_BYTE_SHIFT_LEFT_OP:
		expression_opcode->component.expression_opcode.def_shift_left = analyse_aml_def_shift_left(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_shift_left->string.length;
		break;
	case AML_BYTE_SHIFT_RIGHT_OP:
		expression_opcode->component.expression_opcode.def_shift_right = analyse_aml_def_shift_right(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_shift_right->string.length;
		break;
	case AML_BYTE_SIZE_OF_OP:
		expression_opcode->component.expression_opcode.def_size_of = analyse_aml_def_size_of(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_size_of->string.length;
		break;
	case AML_BYTE_STORE_OP:
		expression_opcode->component.expression_opcode.def_store = analyse_aml_def_store(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_store->string.length;
		break;
	case AML_BYTE_SUBTRACT_OP:
		expression_opcode->component.expression_opcode.def_subtract = analyse_aml_def_subtract(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_subtract->string.length;
		break;
	case AML_BYTE_TO_BUFFER_OP:
		expression_opcode->component.expression_opcode.def_to_buffer = analyse_aml_def_to_buffer(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_to_buffer->string.length;
		break;
	case AML_BYTE_TO_HEX_STRING_OP:
		expression_opcode->component.expression_opcode.def_to_hex_string = analyse_aml_def_to_hex_string(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_to_hex_string->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			expression_opcode->component.expression_opcode.method_invocation = analyse_aml_method_invocation(expression_opcode, aml);
			expression_opcode->string.length += expression_opcode->component.expression_opcode.method_invocation->string.length;
		}
		else
		{
			ERROR(); // Syntax error
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
			break;
		}
	}
	return expression_opcode;
}

// <ext_op_prefix> := AML_BYTE_EXT_OP_PREFIX
AMLSymbol *analyse_aml_ext_op_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *ext_op_prefix = malloc(sizeof(*ext_op_prefix));
	ext_op_prefix->parent = parent;
	ext_op_prefix->string.initial = aml.initial;
	ext_op_prefix->string.length = 1;
	ext_op_prefix->type = aml_ext_op_prefix;
	if(*ext_op_prefix->string.initial != AML_BYTE_EXT_OP_PREFIX)ERROR(); // Incorrect ext op prefix
	return ext_op_prefix;
}

// <field_element> := <named_field> | <reserved_field> | <access_field> | <extended_access_field> | <connect_field>
AMLSymbol *analyse_aml_field_element(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *field_element = malloc(sizeof(*field_element));
	field_element->parent = parent;
	field_element->string.initial = aml.initial;
	field_element->string.length = 0;
	field_element->type = aml_field_element;
	field_element->component.field_element.named_field = NULL;
	field_element->component.field_element.reserved_field = NULL;
	field_element->component.field_element.access_field = NULL;
	field_element->component.field_element.extended_access_field = NULL;
	field_element->component.field_element.connect_field = NULL;
	if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
	{
		field_element->component.field_element.named_field = analyse_aml_named_field(field_element, aml);
		field_element->string.length += field_element->component.field_element.named_field->string.length;
		aml.initial += field_element->component.field_element.named_field->string.length;
		aml.length -= field_element->component.field_element.named_field->string.length;
	}
	else ERROR(); // Syntax error or unimplemented pattern
	return field_element;
}

// <field_flags>
AMLSymbol *analyse_aml_field_flags(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *field_flags = malloc(sizeof(*field_flags));
	field_flags->parent = parent;
	field_flags->string.initial = aml.initial;
	field_flags->string.length = 1;
	field_flags->type = aml_field_flags;
	if(0x80 <= *field_flags->string.initial)ERROR(); // Incorrect field flags
	return field_flags;
}

// <field_list> := Nothing | <field_element> <field_list>
AMLSymbol *analyse_aml_field_list(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *field_list = malloc(sizeof(*field_list));
	field_list->parent = parent;
	field_list->string.initial = aml.initial;
	field_list->string.length = 0;
	field_list->type = aml_field_list;
	field_list->component.field_list.field_element = NULL;
	field_list->component.field_list.field_list = NULL;
	if(aml.length && (('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_'))
	{
		field_list->component.field_list.field_element = analyse_aml_field_element(field_list, aml);
		field_list->string.length += field_list->component.field_list.field_element->string.length;
		aml.initial += field_list->component.field_list.field_element->string.length;
		aml.length -= field_list->component.field_list.field_element->string.length;
		field_list->component.field_list.field_list = analyse_aml_field_list(field_list, aml);
		field_list->string.length += field_list->component.field_list.field_list->string.length;
		aml.initial += field_list->component.field_list.field_list->string.length;
		aml.length -= field_list->component.field_list.field_list->string.length;
	}
	return field_list;
}

// <field_op> := <ext_op_prefix> <field_op_suffix>
AMLSymbol *analyse_aml_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *field_op = malloc(sizeof(*field_op));
	field_op->parent = parent;
	field_op->string.initial = aml.initial;
	field_op->string.length = 0;
	field_op->type = aml_field_op;
	field_op->component.field_op.ext_op_prefix = analyse_aml_ext_op_prefix(field_op, aml);
	field_op->string.length += field_op->component.field_op.ext_op_prefix->string.length;
	aml.initial += field_op->component.field_op.ext_op_prefix->string.length;
	aml.length -= field_op->component.field_op.ext_op_prefix->string.length;
	field_op->component.field_op.field_op_suffix = analyse_aml_field_op_suffix(field_op, aml);
	field_op->string.length += field_op->component.field_op.field_op_suffix->string.length;
	aml.initial += field_op->component.field_op.field_op_suffix->string.length;
	aml.length -= field_op->component.field_op.field_op_suffix->string.length;
	return field_op;
}

// <field_op_suffix> := AML_BYTE_FIELD_OP_PREFIX
AMLSymbol *analyse_aml_field_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *field_op_suffix = malloc(sizeof(*field_op_suffix));
	field_op_suffix->parent = parent;
	field_op_suffix->string.initial = aml.initial;
	field_op_suffix->string.length = 1;
	field_op_suffix->type = aml_field_op_suffix;
	if(*aml.initial != AML_BYTE_FIELD_OP)ERROR(); // Incorrect field op prefix
	return field_op_suffix;
}

// <if_op> := AML_BYTE_IF_OP
AMLSymbol *analyse_aml_if_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *if_op = malloc(sizeof(*if_op));
	if_op->parent = parent;
	if_op->string.initial = aml.initial;
	if_op->string.length = 1;
	if_op->type = aml_if_op;
	if(*aml.initial != AML_BYTE_IF_OP)ERROR(); // Incorrect if_op
	return if_op;
}

// <increment_op> := AML_BYTE_INCREMENT_OP
AMLSymbol *analyse_aml_increment_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *increment_op = malloc(sizeof(*increment_op));
	increment_op->parent = parent;
	increment_op->string.initial = aml.initial;
	increment_op->string.length = 1;
	increment_op->type = aml_increment_op;
	if(*increment_op->string.initial != AML_BYTE_INCREMENT_OP)ERROR(); // Incorrect increment op
	return increment_op;
}

// <index_op> := AML_BYTE_INDEX_OP
AMLSymbol *analyse_aml_index_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *index_op = malloc(sizeof(*index_op));
	index_op->parent = parent;
	index_op->string.initial = aml.initial;
	index_op->string.length = 1;
	index_op->type = aml_index_op;
	if(*index_op->string.initial != AML_BYTE_INDEX_OP)ERROR(); // Incorrect index op
	return index_op;
}

// <index_value> := <term_arg>
AMLSymbol *analyse_aml_index_value(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *index_value = malloc(sizeof(*index_value));
	index_value->parent = parent;
	index_value->string.initial = aml.initial;
	index_value->string.length = 0;
	index_value->type = aml_index_value;
	index_value->component.index_value.term_arg = analyse_aml_term_arg(index_value, aml);
	index_value->string.length += index_value->component.index_value.term_arg->string.length;
	return index_value;
}

// <lead_name_char> := 'A' - 'Z' | '_'
AMLSymbol *analyse_aml_lead_name_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *lead_name_char = malloc(sizeof(*lead_name_char));
	lead_name_char->parent = parent;
	lead_name_char->string.initial = aml.initial;
	lead_name_char->string.length = 1;
	lead_name_char->type = aml_lead_name_char;
	if(!(('A' <= *lead_name_char->string.initial && *lead_name_char->string.initial <= 'Z') || *lead_name_char->string.initial == '_'))ERROR(); // Incorrect lead name char
	return lead_name_char;
}

// <l_equal_op> := AML_BYTE_L_EQUAL_OP
AMLSymbol *analyse_aml_l_equal_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *l_equal_op = malloc(sizeof(*l_equal_op));
	l_equal_op->parent = parent;
	l_equal_op->string.initial = aml.initial;
	l_equal_op->string.length = 1;
	l_equal_op->type = aml_l_equal_op;
	if(*l_equal_op->string.initial != AML_BYTE_L_EQUAL_OP)ERROR(); // Incorrect l_equal_op
	return l_equal_op;
}

// <l_greater_op> := AML_BYTE_L_EQUAL_OP
AMLSymbol *analyse_aml_l_greater_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *l_greater_op = malloc(sizeof(*l_greater_op));
	l_greater_op->parent = parent;
	l_greater_op->string.initial = aml.initial;
	l_greater_op->string.length = 1;
	l_greater_op->type = aml_l_greater_op;
	if(*l_greater_op->string.initial != AML_BYTE_L_GREATER_OP)ERROR(); // Incorrect l_greater_op
	return l_greater_op;
}

// <l_less_op> := AML_BYTE_L_LESS_OP
AMLSymbol *analyse_aml_l_less_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *l_less_op = malloc(sizeof(*l_less_op));
	l_less_op->parent = parent;
	l_less_op->string.initial = aml.initial;
	l_less_op->string.length = 1;
	l_less_op->type = aml_l_less_op;
	if(*l_less_op->string.initial != AML_BYTE_L_LESS_OP)ERROR(); // Incorrect l_less_op
	return l_less_op;
}

// <l_or_op> := AML_BYTE_L_OR_OP
AMLSymbol *analyse_aml_l_or_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *l_or_op = malloc(sizeof(*l_or_op));
	l_or_op->parent = parent;
	l_or_op->string.initial = aml.initial;
	l_or_op->string.length = 1;
	l_or_op->type = aml_l_or_op;
	if(*l_or_op->string.initial != AML_BYTE_L_OR_OP)ERROR(); // Incorrect l_or_op
	return l_or_op;
}

// <local_obj> := <local_op>
AMLSymbol *analyse_aml_local_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *local_obj = malloc(sizeof(*local_obj));
	local_obj->parent = parent;
	local_obj->string.initial = aml.initial;
	local_obj->string.length = 0;
	local_obj->type = aml_local_obj;
	local_obj->component.local_obj.local_op = analyse_aml_local_op(local_obj, aml);
	local_obj->string.length += local_obj->component.local_obj.local_op->string.length;
	aml.initial += local_obj->component.local_obj.local_op->string.length;
	aml.length -= local_obj->component.local_obj.local_op->string.length;
	local_obj->component.local_obj.local_op_number = *local_obj->string.initial - AML_BYTE_LOCAL_0_OP;
	return local_obj;
}

// <local_op> := 0x60 - 0x67
AMLSymbol *analyse_aml_local_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *local_op = malloc(sizeof(*local_op));
	local_op->parent = parent;
	local_op->string.initial = aml.initial;
	local_op->string.length = 1;
	local_op->type = aml_local_op;
	switch(*local_op->string.initial)
	{
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
		break;
	default:
		ERROR(); // Incorrect local op
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return local_op;
}

// <method_flags>
AMLSymbol *analyse_aml_method_flags(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *method_flags = malloc(sizeof(*method_flags));
	method_flags->parent = parent;
	method_flags->string.initial = aml.initial;
	method_flags->string.length = 1;
	method_flags->type = aml_method_flags;
	method_flags->component.method_flags.arg_count = *aml.initial & 0x07;
	method_flags->component.method_flags.serialize_flag = *aml.initial & 0x08 ? true : false;
	method_flags->component.method_flags.sync_level = *aml.initial >> 4;
	return method_flags;
}

// <method_invocation> := <name_string> <term_arg_list>
AMLSymbol *analyse_aml_method_invocation(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *method_invocation = malloc(sizeof(*method_invocation));
	method_invocation->parent = parent;
	method_invocation->string.initial = aml.initial;
	method_invocation->string.length = 0;
	method_invocation->type = aml_method_invocation;
	method_invocation->component.method_invocation.name_string = analyse_aml_name_string(method_invocation, aml);
	method_invocation->string.length += method_invocation->component.method_invocation.name_string->string.length;
	aml.initial += method_invocation->component.method_invocation.name_string->string.length;
	aml.length -= method_invocation->component.method_invocation.name_string->string.length;
	method_invocation->component.method_invocation.term_arg_list = analyse_aml_term_arg_list(method_invocation, aml, 0);
	WARNING(); // Number of arguments is pseudo
	printf_serial("def_method = %p\n", get_aml_method(method_invocation->component.method_invocation.name_string->component.name_string.string, method_invocation));
	printf_serial("Method name = \"%.*s\"\n", method_invocation->component.method_invocation.name_string->string.length, method_invocation->component.method_invocation.name_string->string.initial);
	method_invocation->string.length += method_invocation->component.method_invocation.term_arg_list->string.length;
	aml.initial += method_invocation->component.method_invocation.term_arg_list->string.length;
	aml.length -= method_invocation->component.method_invocation.term_arg_list->string.length;
	return method_invocation;
}

// <method_op> := 0x14
AMLSymbol *analyse_aml_method_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *method_op = malloc(sizeof(*method_op));
	method_op->parent = parent;
	method_op->string.initial = aml.initial;
	method_op->string.length = 1;
	method_op->type = aml_method_op;
	if(*method_op->string.initial != AML_BYTE_METHOD_OP)ERROR(); // Incorrect method op
	return method_op;
}

// <multi_name_path> := <multi_name_prefix> <seg_count> <name_seg>*
AMLSymbol *analyse_aml_multi_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *multi_name_path = malloc(sizeof(*multi_name_path));
	char *string_writer;
	multi_name_path->parent = parent;
	multi_name_path->string.initial = aml.initial;
	multi_name_path->string.length = 0;
	multi_name_path->type = aml_multi_name_path;
	multi_name_path->component.multi_name_path.multi_name_prefix = analyse_aml_multi_name_prefix(multi_name_path, aml);
	multi_name_path->string.length += multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	aml.initial += multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	aml.length -= multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	multi_name_path->component.multi_name_path.seg_count = analyse_aml_seg_count(multi_name_path, aml);
	multi_name_path->string.length += multi_name_path->component.multi_name_path.seg_count->string.length;
	aml.initial += multi_name_path->component.multi_name_path.seg_count->string.length;
	aml.length -= multi_name_path->component.multi_name_path.seg_count->string.length;
	multi_name_path->component.multi_name_path.name_seg = malloc(*multi_name_path->component.multi_name_path.seg_count->string.initial * sizeof(*multi_name_path->component.multi_name_path.name_seg));
	multi_name_path->component.multi_name_path.string = malloc(*multi_name_path->component.multi_name_path.seg_count->string.initial * (sizeof((*multi_name_path->component.multi_name_path.name_seg)->component.name_seg.string) - 1) + 1);
	string_writer = multi_name_path->component.multi_name_path.string;
	for(unsigned int i = 0; i < *multi_name_path->component.multi_name_path.seg_count->string.initial; i++)
	{
		multi_name_path->component.multi_name_path.name_seg[i] = analyse_aml_name_seg(multi_name_path, aml);
		multi_name_path->string.length += multi_name_path->component.multi_name_path.name_seg[i]->string.length;
		aml.initial += multi_name_path->component.multi_name_path.name_seg[i]->string.length;
		aml.length -= multi_name_path->component.multi_name_path.name_seg[i]->string.length;
		strcpy(string_writer, multi_name_path->component.multi_name_path.name_seg[i]->component.name_seg.string);
		string_writer += strlen(string_writer);
	}
	*string_writer = '\0';
	return multi_name_path;
}

// <mult_name_prefix> := AML_BYTE_MULTI_NAME_PREFIX
AMLSymbol *analyse_aml_multi_name_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *multi_name_prefix = malloc(sizeof(*multi_name_prefix));
	multi_name_prefix->parent = parent;
	multi_name_prefix->string.initial = aml.initial;
	multi_name_prefix->string.length = 1;
	multi_name_prefix->type = aml_multi_name_prefix;
	if(*multi_name_prefix->string.initial != AML_BYTE_MULTI_NAME_PREFIX)ERROR(); // Incorrect munti name prefix
	return multi_name_prefix;
}

// <mutex_object> := <super_name>
AMLSymbol *analyse_aml_mutex_object(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *mutex_object = malloc(sizeof(*mutex_object));
	mutex_object->parent = parent;
	mutex_object->string.initial = aml.initial;
	mutex_object->string.length = 0;
	mutex_object->type = aml_mutex_object;
	mutex_object->component.mutex_object.super_name = analyse_aml_super_name(mutex_object, aml);
	mutex_object->string.length += mutex_object->component.mutex_object.super_name->string.length;
	return mutex_object;
}

// <mutex_op> := <ext_op_prefix> <mutex_op_suffix>
AMLSymbol *analyse_aml_mutex_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *mutex_op = malloc(sizeof(*mutex_op));
	mutex_op->parent = parent;
	mutex_op->string.initial = aml.initial;
	mutex_op->string.length = 0;
	mutex_op->type = aml_mutex_op;
	mutex_op->component.mutex_op.ext_op_prefix = analyse_aml_ext_op_prefix(mutex_op, aml);
	mutex_op->string.length += mutex_op->component.mutex_op.ext_op_prefix->string.length;
	aml.initial += mutex_op->component.mutex_op.ext_op_prefix->string.length;
	aml.length -= mutex_op->component.mutex_op.ext_op_prefix->string.length;
	mutex_op->component.mutex_op.mutex_op_suffix = analyse_aml_mutex_op_suffix(mutex_op, aml);
	mutex_op->string.length += mutex_op->component.mutex_op.mutex_op_suffix->string.length;
	aml.initial += mutex_op->component.mutex_op.mutex_op_suffix->string.length;
	aml.length -= mutex_op->component.mutex_op.mutex_op_suffix->string.length;
	return mutex_op;
}

// <mutex_op_suffix> := AML_BYTE_MUTEX_OP
AMLSymbol *analyse_aml_mutex_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *mutex_op_suffix = malloc(sizeof(*mutex_op_suffix));
	mutex_op_suffix->parent = parent;
	mutex_op_suffix->string.initial = aml.initial;
	mutex_op_suffix->string.length = 1;
	mutex_op_suffix->type = aml_mutex_op_suffix;
	if(*aml.initial != AML_BYTE_MUTEX_OP)ERROR(); // Incorrect mutex_op_suffix
	return mutex_op_suffix;
}

// <name_char> := <digit_char> | <lead_name_char>
AMLSymbol *analyse_aml_name_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_char = malloc(sizeof(*name_char));
	name_char->parent = parent;
	name_char->string.initial = aml.initial;
	name_char->string.length = 0;
	name_char->type = aml_name_char;
	name_char->component.name_char.digit_char = NULL;
	name_char->component.name_char.lead_name_char = NULL;
	if('0' <= *aml.initial && *aml.initial <= '9')
	{
		name_char->component.name_char.digit_char = analyse_aml_digit_char(name_char, aml);
		name_char->string.length += name_char->component.name_char.digit_char->string.length;
	}
	else if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
	{
		name_char->component.name_char.lead_name_char = analyse_aml_lead_name_char(name_char, aml);
		name_char->string.length += name_char->component.name_char.lead_name_char->string.length;
	}
	else ERROR(); // Incorrect name char
	name_char->component.name_char.character = *aml.initial;
	return name_char;
}

// <name_op> := AML_BYTE_NAME_OP
AMLSymbol *analyse_aml_name_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_op = malloc(sizeof(*name_op));
	name_op->parent = parent;
	name_op->string.initial = aml.initial;
	name_op->string.length = 1;
	name_op->type = aml_name_op;
	if(*name_op->string.initial != AML_BYTE_NAME_OP)ERROR(); // Incorrect name op
	return name_op;
}

// <name_path> := <name_seg> | <dual_name_path> | <multi_name_path> | <null_name>
AMLSymbol *analyse_aml_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_path = malloc(sizeof(*name_path));
	name_path->parent = parent;
	name_path->string.initial = aml.initial;
	name_path->string.length = 0;
	name_path->type = aml_name_path;
	name_path->component.name_path.name_seg = NULL;
	name_path->component.name_path.dual_name_path = NULL;
	name_path->component.name_path.multi_name_path = NULL;
	name_path->component.name_path.null_name = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_DUAL_NAME_PREFIX:
		name_path->component.name_path.dual_name_path = analyse_aml_dual_name_path(name_path, aml);
		name_path->string.length += name_path->component.name_path.dual_name_path->string.length;
		name_path->component.name_path.string = name_path->component.name_path.dual_name_path->component.dual_name_path.string;
		break;
	case AML_BYTE_MULTI_NAME_PREFIX:
		name_path->component.name_path.multi_name_path = analyse_aml_multi_name_path(name_path, aml);
		name_path->string.length += name_path->component.name_path.multi_name_path->string.length;
		name_path->component.name_path.string = name_path->component.name_path.multi_name_path->component.multi_name_path.string;
		break;
	case AML_BYTE_NULL_NAME:
		name_path->component.name_path.null_name = analyse_aml_null_name(name_path, aml);
		name_path->string.length += name_path->component.name_path.null_name->string.length;
		name_path->component.name_path.string = "";
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			name_path->component.name_path.name_seg = analyse_aml_name_seg(name_path, aml);
			name_path->string.length += name_path->component.name_path.name_seg->string.length;
			name_path->component.name_path.string = name_path->component.name_path.name_seg->component.name_seg.string;
		}
		else
		{
			ERROR(); // Syntax error
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return name_path;
}

// <name_seg> := <lead_name_char> <name_char> <name_char> <name_char>
AMLSymbol *analyse_aml_name_seg(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_seg = malloc(sizeof(*name_seg));
	char *string_writer = name_seg->component.name_seg.string;
	name_seg->parent = parent;
	name_seg->string.initial = aml.initial;
	name_seg->string.length = 0;
	name_seg->type = aml_name_seg;
	name_seg->component.name_seg.lead_name_char = analyse_aml_lead_name_char(name_seg, aml);
	aml.initial += name_seg->component.name_seg.lead_name_char->string.length;
	aml.length -= name_seg->component.name_seg.lead_name_char->string.length;
	name_seg->string.length += name_seg->component.name_seg.lead_name_char->string.length;
	*string_writer++ = *name_seg->component.name_seg.lead_name_char->string.initial;
	for(AMLSymbol **name_char = name_seg->component.name_seg.name_char; name_char != name_seg->component.name_seg.name_char + _countof(name_seg->component.name_seg.name_char); name_char++)
	{
		*name_char = analyse_aml_name_char(name_seg, aml);
		name_seg->string.length += (*name_char)->string.length;
		aml.initial += (*name_char)->string.length;
		aml.length -= (*name_char)->string.length;
		*string_writer++ = (*name_char)->component.name_char.character;
	}
	*string_writer = '\0';
	return name_seg;
}

// <name_space_modifier_obj> := <def_alias> | <def_name> | <def_scope>
AMLSymbol *analyse_aml_name_space_modifier_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_space_modifier_obj = malloc(sizeof(*name_space_modifier_obj));
	name_space_modifier_obj->parent = parent;
	name_space_modifier_obj->string.initial = aml.initial;
	name_space_modifier_obj->string.length = 0;
	name_space_modifier_obj->type = aml_name_space_modifier_obj;
	name_space_modifier_obj->component.name_space_modifier_obj.def_alias = NULL;
	name_space_modifier_obj->component.name_space_modifier_obj.def_name = NULL;
	name_space_modifier_obj->component.name_space_modifier_obj.def_scope = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ALIAS_OP:
		name_space_modifier_obj->component.name_space_modifier_obj.def_alias = analyse_aml_def_alias(name_space_modifier_obj, aml);
		name_space_modifier_obj->string.length += name_space_modifier_obj->component.name_space_modifier_obj.def_alias->string.length;
		break;
	case AML_BYTE_NAME_OP:
		name_space_modifier_obj->component.name_space_modifier_obj.def_name = analyse_aml_def_name(name_space_modifier_obj, aml);
		name_space_modifier_obj->string.length += name_space_modifier_obj->component.name_space_modifier_obj.def_name->string.length;
		break;
	case AML_BYTE_SCOPE_OP:
		name_space_modifier_obj->component.name_space_modifier_obj.def_scope = analyse_aml_def_scope(name_space_modifier_obj, aml);
		name_space_modifier_obj->string.length += name_space_modifier_obj->component.name_space_modifier_obj.def_scope->string.length;
		break;
	default:
		ERROR(); // Syntax error
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return name_space_modifier_obj;
}

// <name_string> := <root_char> <name_path> | <prefix_path> <name_path>
AMLSymbol *analyse_aml_name_string(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *name_string = malloc(sizeof(*name_string));
	unsigned int string_length;
	char *string_writer;
	name_string->parent = parent;
	name_string->string.initial = aml.initial;
	name_string->string.length = 0;
	name_string->type = aml_name_string;
	name_string->component.name_string.root_char = NULL;
	name_string->component.name_string.prefix_path = NULL;
	name_string->component.name_string.name_path = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ROOT_CHAR:
		name_string->component.name_string.root_char = analyse_aml_root_char(name_string, aml);
		name_string->string.length += name_string->component.name_string.root_char->string.length;
		aml.initial += name_string->component.name_string.root_char->string.length;
		aml.length -= name_string->component.name_string.root_char->string.length;
		string_length = 1;
		break;
	default:
		name_string->component.name_string.prefix_path = analyse_aml_prefix_path(name_string, aml);
		name_string->string.length += name_string->component.name_string.prefix_path->string.length;
		aml.initial += name_string->component.name_string.prefix_path->string.length;
		aml.length -= name_string->component.name_string.prefix_path->string.length;
		string_length = strlen(name_string->component.name_string.prefix_path->component.prefix_path.string);
		break;
	}
	name_string->component.name_string.name_path = analyse_aml_name_path(name_string, aml);
	name_string->string.length += name_string->component.name_string.name_path->string.length;
	string_length += strlen(name_string->component.name_string.name_path->component.name_path.string);
	string_writer = name_string->component.name_string.string = malloc(string_length + 1);
	if(name_string->component.name_string.root_char)*string_writer++ = AML_BYTE_ROOT_CHAR;
	if(name_string->component.name_string.prefix_path)
	{
		strcpy(string_writer, name_string->component.name_string.prefix_path->component.prefix_path.string);
		string_writer += strlen(name_string->component.name_string.prefix_path->component.prefix_path.string);
	}
	strcpy(string_writer, name_string->component.name_string.name_path->component.name_path.string);
	return name_string;
}

// <named_field> := <name_seg> <pkg_length>
AMLSymbol *analyse_aml_named_field(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *named_field = malloc(sizeof(*named_field));
	named_field->parent = parent;
	named_field->string.initial = aml.initial;
	named_field->string.length = 0;
	named_field->type = aml_named_field;
	named_field->component.named_field.name_seg = analyse_aml_name_seg(named_field, aml);
	named_field->string.length += named_field->component.named_field.name_seg->string.length;
	aml.initial += named_field->component.named_field.name_seg->string.length;
	aml.length -= named_field->component.named_field.name_seg->string.length;
	named_field->component.named_field.pkg_length = analyse_aml_pkg_length(named_field, aml);
	named_field->string.length += named_field->component.named_field.pkg_length->string.length;
	aml.initial += named_field->component.named_field.pkg_length->string.length;
	aml.length = named_field->component.named_field.pkg_length->component.pkg_length.length - named_field->component.named_field.pkg_length->string.length;
	return named_field;
}

// <named_obj> := <def_bank_field> | <def_create_bit_field> | <def_create_byte_field> | <def_create_dword_field> | <def_create_field> | <def_create_qword_field> | <def_create_word_field> | <def_data_region> | <def_external> | <def_op_region> | <def_power_res> | <def_thermal_zone>
AMLSymbol *analyse_aml_named_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *named_obj = malloc(sizeof(*named_obj));
	named_obj->parent = parent;
	named_obj->string.initial = aml.initial;
	named_obj->string.length = 0;
	named_obj->type = aml_named_obj;
	named_obj->component.named_obj.def_bank_field = NULL;
	named_obj->component.named_obj.def_create_bit_field = NULL;
	named_obj->component.named_obj.def_create_byte_field = NULL;
	named_obj->component.named_obj.def_create_dword_field = NULL;
	named_obj->component.named_obj.def_create_field = NULL;
	named_obj->component.named_obj.def_create_qword_field = NULL;
	named_obj->component.named_obj.def_create_word_field = NULL;
	named_obj->component.named_obj.def_data_region = NULL;
	named_obj->component.named_obj.def_device = NULL;
	named_obj->component.named_obj.def_external = NULL;
	named_obj->component.named_obj.def_field = NULL;
	named_obj->component.named_obj.def_method = NULL;
	named_obj->component.named_obj.def_mutex = NULL;
	named_obj->component.named_obj.def_op_region = NULL;
	named_obj->component.named_obj.def_power_res = NULL;
	named_obj->component.named_obj.def_thermal_zone = NULL;
	switch(aml.initial[0])
	{
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_DEVICE_OP:
			named_obj->component.named_obj.def_device = analyse_aml_def_device(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_device->string.length;
			break;
		case AML_BYTE_FIELD_OP:
			named_obj->component.named_obj.def_field = analyse_aml_def_field(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_field->string.length;
			break;
		case AML_BYTE_MUTEX_OP:
			named_obj->component.named_obj.def_mutex = analyse_aml_def_mutex(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_mutex->string.length;
			break;
		case AML_BYTE_OP_REGION_OP:
			named_obj->component.named_obj.def_op_region = analyse_aml_def_op_region(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_op_region->string.length;
			break;
		default:
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("aml.initial[0] = %#04.2x\n", aml.initial[0]);
			printf_serial("aml.initial[1] = %#04.2x\n", aml.initial[1]);
			break;
		}
		break;
	case AML_BYTE_METHOD_OP:
		named_obj->component.named_obj.def_method = analyse_aml_def_method(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_method->string.length;
		break;
	default:
		ERROR(); // Syntax error or unimplemented pattern
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return named_obj;
}

// <null_char> := AML_BYTE_NULL_CHAR
AMLSymbol *analyse_aml_null_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *null_char = malloc(sizeof(*null_char));
	null_char->parent = parent;
	null_char->string.initial = aml.initial;
	null_char->string.length = 1;
	null_char->type = aml_null_char;
	if(*null_char->string.initial == AML_BYTE_NULL_CHAR)ERROR(); // Incorrect null char
	return null_char;
}

// <null_name> := AML_BYTE_NULL_NAME
AMLSymbol *analyse_aml_null_name(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *null_name = malloc(sizeof(*null_name));
	null_name->parent = parent;
	null_name->string.initial = aml.initial;
	null_name->string.length = 1;
	null_name->type = aml_null_name;
	if(*null_name->string.initial != AML_BYTE_NULL_NAME)ERROR(); // Incorrect null name
	return null_name;
}

// <num_elements> := <byte_data>
AMLSymbol *analyse_aml_num_elements(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *num_elements = malloc(sizeof(*num_elements));
	num_elements->parent = parent;
	num_elements->string.initial = aml.initial;
	num_elements->string.length = 0;
	num_elements->type = aml_num_elements;
	num_elements->component.num_elements.byte_data = analyse_aml_byte_data(num_elements, aml);
	num_elements->string.length += num_elements->component.num_elements.byte_data->string.length;
	aml.initial += num_elements->component.num_elements.byte_data->string.length;
	aml.length -= num_elements->component.num_elements.byte_data->string.length;
	return num_elements;
}

// <obj_reference> := <term_arg>
AMLSymbol *analyse_aml_obj_reference(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *obj_reference = malloc(sizeof(*obj_reference));
	obj_reference->parent = parent;
	obj_reference->string.initial = aml.initial;
	obj_reference->string.length = 0;
	obj_reference->type = aml_obj_reference;
	obj_reference->component.obj_reference.term_arg = analyse_aml_term_arg(obj_reference, aml);
	obj_reference->string.length += obj_reference->component.obj_reference.term_arg->string.length;
	aml.initial += obj_reference->component.obj_reference.term_arg->string.length;
	aml.length -= obj_reference->component.obj_reference.term_arg->string.length;
	return obj_reference;
}

// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *object = malloc(sizeof(*object));
	object->parent = parent;
	object->string.initial = aml.initial;
	object->string.length = 0;
	object->type = aml_object;
	object->component.object.name_space_modifier_obj = NULL;
	object->component.object.named_obj = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_NAME_OP:
	case AML_BYTE_SCOPE_OP:
		object->component.object.name_space_modifier_obj = analyse_aml_name_space_modifier_obj(object, aml);
		object->string.length += object->component.object.name_space_modifier_obj->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_METHOD_OP:
		object->component.object.named_obj = analyse_aml_named_obj(object, aml);
		object->string.length += object->component.object.named_obj->string.length;
		break;
	default:
		ERROR(); // Syntax error or unimplemented pattern
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return object;
}

// <one_op> := AML_BYTE_ONE_OP
AMLSymbol *analyse_aml_one_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *one_op = malloc(sizeof(*one_op));
	one_op->parent = parent;
	one_op->string.initial = aml.initial;
	one_op->string.length = 1;
	one_op->type = aml_one_op;
	if(*one_op->string.initial != AML_BYTE_ONE_OP)ERROR(); // Incorrect one op
	return one_op;
}

// <ones_op> := AML_BYTE_ONES_OP
AMLSymbol *analyse_aml_ones_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *ones_op = malloc(sizeof(*ones_op));
	ones_op->parent = parent;
	ones_op->string.initial = aml.initial;
	ones_op->string.length = 1;
	ones_op->type = aml_ones_op;
	if(*ones_op->string.initial != AML_BYTE_ONES_OP)ERROR(); // Incorrect ones op
	return ones_op;
}

// <op_region_op> := <ext_op_prefix> <op_region_op_suffix>
AMLSymbol *analyse_aml_op_region_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *op_region_op = malloc(sizeof(*op_region_op));
	op_region_op->parent = parent;
	op_region_op->string.initial = aml.initial;
	op_region_op->string.length = 0;
	op_region_op->type = aml_op_region_op;
	op_region_op->component.op_region_op.ext_op_prefix = analyse_aml_ext_op_prefix(op_region_op, aml);
	op_region_op->string.length += op_region_op->component.op_region_op.ext_op_prefix->string.length;
	aml.initial += op_region_op->component.op_region_op.ext_op_prefix->string.length;
	aml.length -= op_region_op->component.op_region_op.ext_op_prefix->string.length;
	op_region_op->component.op_region_op.op_region_op_suffix = analyse_aml_op_region_op_suffix(op_region_op, aml);
	op_region_op->string.length += op_region_op->component.op_region_op.op_region_op_suffix->string.length;
	aml.initial += op_region_op->component.op_region_op.op_region_op_suffix->string.length;
	aml.length -= op_region_op->component.op_region_op.op_region_op_suffix->string.length;
	return op_region_op;
}

// <op_region_op_suffix> := AML_BYTE_OP_REGION_OP
AMLSymbol *analyse_aml_op_region_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *op_region_op_suffix = malloc(sizeof(*op_region_op_suffix));
	op_region_op_suffix->parent = parent;
	op_region_op_suffix->string.initial = aml.initial;
	op_region_op_suffix->string.length = 1;
	op_region_op_suffix->type = aml_op_region_op_suffix;
	if(*aml.initial != AML_BYTE_OP_REGION_OP)ERROR();
	return op_region_op_suffix;
}

// <operand> := <term_arg>
AMLSymbol *analyse_aml_operand(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *operand = malloc(sizeof(*operand));
	operand->parent = parent;
	operand->string.initial = aml.initial;
	operand->string.length = 0;
	operand->type = aml_operand;
	operand->component.operand.term_arg = analyse_aml_term_arg(operand, aml);
	operand->string.length += operand->component.operand.term_arg->string.length;
	return operand;
}

// <package_element> := <data_ref_object> | <name_string>
AMLSymbol *analyse_aml_package_element(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *package_element = malloc(sizeof(*package_element));
	package_element->parent = parent;
	package_element->string.initial = aml.initial;
	package_element->string.length = 0;
	package_element->type = aml_package_element;
	package_element->component.package_element.data_ref_object = NULL;
	package_element->component.package_element.name_string = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
		package_element->component.package_element.data_ref_object = analyse_aml_data_ref_object(package_element, aml);
		package_element->string.length += package_element->component.package_element.data_ref_object->string.length;
		aml.initial += package_element->component.package_element.data_ref_object->string.length;
		aml.length -= package_element->component.package_element.data_ref_object->string.length;
		break;
	case AML_BYTE_DUAL_NAME_PREFIX:
	case AML_BYTE_MULTI_NAME_PREFIX:
	case AML_BYTE_NULL_NAME:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
		package_element->component.package_element.name_string = analyse_aml_name_string(package_element, aml);
		package_element->string.length += package_element->component.package_element.name_string->string.length;
		aml.initial += package_element->component.package_element.name_string->string.length;
		aml.length -= package_element->component.package_element.name_string->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			package_element->component.package_element.name_string = analyse_aml_name_string(package_element, aml);
			package_element->string.length += package_element->component.package_element.name_string->string.length;
			aml.initial += package_element->component.package_element.name_string->string.length;
			aml.length -= package_element->component.package_element.name_string->string.length;
		}
		else
		{
			ERROR(); // Syntax error
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return package_element;
}

// <package_element_list> := Nothing | <package_element> <package_element_list>
AMLSymbol *analyse_aml_package_element_list(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *package_element_list = malloc(sizeof(*package_element_list));
	package_element_list->parent = parent;
	package_element_list->string.initial = aml.initial;
	package_element_list->string.length = 0;
	package_element_list->type = aml_package_element_list;
	package_element_list->component.package_element_list.package_element = NULL;
	package_element_list->component.package_element_list.package_element_list = NULL;
	if(aml.length)switch(*aml.initial)
	{
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DUAL_NAME_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_MULTI_NAME_PREFIX:
	case AML_BYTE_NULL_NAME:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_ROOT_CHAR:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
		package_element_list->component.package_element_list.package_element = analyse_aml_package_element(package_element_list, aml);
		package_element_list->string.length += package_element_list->component.package_element_list.package_element->string.length;
		aml.initial += package_element_list->component.package_element_list.package_element->string.length;
		aml.length -= package_element_list->component.package_element_list.package_element->string.length;
		package_element_list->component.package_element_list.package_element_list = analyse_aml_package_element_list(package_element_list, aml);
		package_element_list->string.length += package_element_list->component.package_element_list.package_element_list->string.length;
		aml.initial += package_element_list->component.package_element_list.package_element_list->string.length;
		aml.length -= package_element_list->component.package_element_list.package_element_list->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			package_element_list->component.package_element_list.package_element = analyse_aml_package_element(package_element_list, aml);
			package_element_list->string.length += package_element_list->component.package_element_list.package_element->string.length;
			aml.initial += package_element_list->component.package_element_list.package_element->string.length;
			aml.length -= package_element_list->component.package_element_list.package_element->string.length;
			package_element_list->component.package_element_list.package_element_list = analyse_aml_package_element_list(package_element_list, aml);
			package_element_list->string.length += package_element_list->component.package_element_list.package_element_list->string.length;
			aml.initial += package_element_list->component.package_element_list.package_element_list->string.length;
			aml.length -= package_element_list->component.package_element_list.package_element_list->string.length;
		}
		else
		{
			ERROR(); // Syntax error
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return package_element_list;
}

// <package_op> := AML_BYTE_PACKAGE_OP
AMLSymbol *analyse_aml_package_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *package_op = malloc(sizeof(*package_op));
	package_op->parent = parent;
	package_op->string.initial = aml.initial;
	package_op->string.length = 1;
	package_op->type = aml_package_op;
	if(*aml.initial != AML_BYTE_PACKAGE_OP)ERROR(); // Incorrect package_op
	return package_op;
}

// <parent_prefix_char> := AML_BYTE_PARENT_PREFIX_CHAR
AMLSymbol *analyse_aml_parent_prefix_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *parent_prefix_char = malloc(sizeof(*parent_prefix_char));
	parent_prefix_char->parent = parent;
	parent_prefix_char->string.initial = aml.initial;
	parent_prefix_char->string.length = 1;
	parent_prefix_char->type = aml_parent_prefix_char;
	if(*parent_prefix_char->string.initial != AML_BYTE_PARENT_PREFIX_CHAR)ERROR(); // Incorrect parent prefix char
	return parent_prefix_char;
}

// <pkg_lead_byte>
AMLSymbol *analyse_aml_pkg_lead_byte(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *pkg_lead_byte = malloc(sizeof(*pkg_lead_byte));
	pkg_lead_byte->parent = parent;
	pkg_lead_byte->string.initial = aml.initial;
	pkg_lead_byte->string.length = 1;
	pkg_lead_byte->type = aml_pkg_lead_byte;
	return pkg_lead_byte;
}

// <pkg_length> := <pkg_lead_byte> | <pkg_lead_byte> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> <byte_data>
AMLSymbol *analyse_aml_pkg_length(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *pkg_length = malloc(sizeof(*pkg_length));
	pkg_length->parent = parent;
	pkg_length->string.initial = aml.initial;
	pkg_length->string.length = 0;
	pkg_length->type = aml_pkg_length;
	pkg_length->component.pkg_length.pkg_lead_byte = NULL;
	for(AMLSymbol **byte_data = pkg_length->component.pkg_length.byte_data; byte_data != pkg_length->component.pkg_length.byte_data + _countof(pkg_length->component.pkg_length.byte_data); byte_data++)*byte_data = NULL;
	pkg_length->component.pkg_length.pkg_lead_byte = analyse_aml_pkg_lead_byte(pkg_length, aml);
	pkg_length->string.length += pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	aml.initial += pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	aml.length -= pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	pkg_length->component.pkg_length.num_of_byte_data = *pkg_length->component.pkg_length.pkg_lead_byte->string.initial >> 6 & 3;
	for(AMLSymbol **byte_data = pkg_length->component.pkg_length.byte_data; byte_data != pkg_length->component.pkg_length.byte_data + pkg_length->component.pkg_length.num_of_byte_data; byte_data++)
	{
		*byte_data = analyse_aml_byte_data(pkg_length, aml);
		pkg_length->string.length += (*byte_data)->string.length;
		aml.initial += (*byte_data)->string.length;
		aml.length -= (*byte_data)->string.length;
	}
	if(pkg_length->component.pkg_length.num_of_byte_data)
	{
		pkg_length->component.pkg_length.length = 0;
		for(int i = pkg_length->component.pkg_length.num_of_byte_data - 1; 0 <= i; i--)
		{
			pkg_length->component.pkg_length.length <<= 8;
			pkg_length->component.pkg_length.length += *pkg_length->component.pkg_length.byte_data[i]->string.initial;
		}
		pkg_length->component.pkg_length.length <<= 4;
		pkg_length->component.pkg_length.length += *pkg_length->component.pkg_length.pkg_lead_byte->string.initial & 0x0f;
	}
	else pkg_length->component.pkg_length.length = *pkg_length->component.pkg_length.pkg_lead_byte->string.initial & 0x3f;
	return pkg_length;
}

// <predicate> := <term_arg>
AMLSymbol *analyse_aml_predicate(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *predicate = malloc(sizeof(*predicate));
	predicate->parent = parent;
	predicate->string.initial = aml.initial;
	predicate->string.length = 0;
	predicate->type = aml_predicate;
	predicate->component.predicate.term_arg = analyse_aml_term_arg(predicate, aml);
	predicate->string.length += predicate->component.predicate.term_arg->string.length;
	return predicate;
}

// <prefix_path> := Nothing | <parent_prefix_char> <prefix_path>
AMLSymbol *analyse_aml_prefix_path(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *prefix_path = malloc(sizeof(*prefix_path));
	char *string_writer;
	prefix_path->parent = parent;
	prefix_path->string.initial = aml.initial;
	prefix_path->string.length = 0;
	prefix_path->type = aml_prefix_path;
	prefix_path->component.prefix_path.parent_prefix_char = NULL;
	prefix_path->component.prefix_path.prefix_path = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_PARENT_PREFIX_CHAR:
		prefix_path->component.prefix_path.parent_prefix_char = analyse_aml_parent_prefix_char(prefix_path, aml);
		prefix_path->string.length += prefix_path->component.prefix_path.parent_prefix_char->string.length;
		aml.initial = aml.initial + prefix_path->component.prefix_path.parent_prefix_char->string.length;
		aml.length = aml.length - prefix_path->component.prefix_path.parent_prefix_char->string.length;
		prefix_path->component.prefix_path.prefix_path = analyse_aml_prefix_path(prefix_path, aml);
		prefix_path->string.length += prefix_path->component.prefix_path.prefix_path->string.length;
		prefix_path->component.prefix_path.string = malloc(strlen(prefix_path->component.prefix_path.prefix_path->component.prefix_path.string) + 2);
		string_writer = prefix_path->component.prefix_path.string;
		*string_writer++ = AML_BYTE_PARENT_PREFIX_CHAR;
		strcpy(string_writer, prefix_path->component.prefix_path.prefix_path->component.prefix_path.string);
		break;
	default:
		prefix_path->component.prefix_path.string = malloc(sizeof(*prefix_path->component.prefix_path.string));
		*prefix_path->component.prefix_path.string = '\0';
		break;
	}
	return prefix_path;
}

// <qword_const> := <qword_prefix> <qword_data>
AMLSymbol *analyse_aml_qword_const(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *qword_const = malloc(sizeof(*qword_const));
	qword_const->parent = parent;
	qword_const->string.initial = aml.initial;
	qword_const->string.length = 0;
	qword_const->type = aml_qword_const;
	qword_const->component.qword_const.qword_prefix = analyse_aml_qword_prefix(qword_const, aml);
	qword_const->string.length += qword_const->component.qword_const.qword_prefix->string.length;
	aml.initial += qword_const->component.qword_const.qword_prefix->string.length;
	aml.length -= qword_const->component.qword_const.qword_prefix->string.length;
	qword_const->component.qword_const.qword_data = analyse_aml_qword_data(qword_const, aml);
	qword_const->string.length += qword_const->component.qword_const.qword_data->string.length;
	aml.initial += qword_const->component.qword_const.qword_data->string.length;
	aml.length -= qword_const->component.qword_const.qword_data->string.length;
	qword_const->component.qword_const.value = qword_const->component.qword_const.qword_data->component.qword_data.value;
	return qword_const;
}

// <qword_data> := <dword_data> <dword_data>
AMLSymbol *analyse_aml_qword_data(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *qword_data = malloc(sizeof(*qword_data));
	qword_data->parent = parent;
	qword_data->string.initial = aml.initial;
	qword_data->string.length = 0;
	qword_data->type = aml_qword_data;
	for(AMLSymbol **dword_data = qword_data->component.qword_data.dword_data; dword_data != qword_data->component.qword_data.dword_data + _countof(qword_data->component.qword_data.dword_data); dword_data++)
	{
		*dword_data = analyse_aml_dword_data(qword_data, aml);
		qword_data->string.length += (*dword_data)->string.length;
		aml.initial += (*dword_data)->string.length;
		aml.length -= (*dword_data)->string.length;
	}
	qword_data->component.qword_data.value = qword_data->component.qword_data.value;
	return qword_data;
}

// <qword_prefix> := AML_BYTE_QWORD_PREFIX
AMLSymbol *analyse_aml_qword_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *qword_prefix = malloc(sizeof(*qword_prefix));
	qword_prefix->parent = parent;
	qword_prefix->string.initial = aml.initial;
	qword_prefix->string.length = 1;
	qword_prefix->type = aml_qword_prefix;
	if(*qword_prefix->string.initial != AML_BYTE_QWORD_PREFIX)ERROR(); // Incorrect qword prefix
	return qword_prefix;
}

// <region_len> := <term_arg>
AMLSymbol *analyse_aml_region_len(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *region_len = malloc(sizeof(*region_len));
	region_len->parent = parent;
	region_len->string.initial = aml.initial;
	region_len->string.length = 0;
	region_len->type = aml_region_len;
	region_len->component.region_len.term_arg = analyse_aml_term_arg(region_len, aml);
	region_len->string.length += region_len->component.region_len.term_arg->string.length;
	return region_len;
}

// <region_offset> := <term_arg>
AMLSymbol *analyse_aml_region_offset(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *region_offset = malloc(sizeof(*region_offset));
	region_offset->parent = parent;
	region_offset->string.initial = aml.initial;
	region_offset->string.length = 0;
	region_offset->type = aml_region_offset;
	region_offset->component.region_offset.term_arg = analyse_aml_term_arg(region_offset, aml);
	region_offset->string.length += region_offset->component.region_offset.term_arg->string.length;
	return region_offset;
}

// <region_space>
AMLSymbol *analyse_aml_region_space(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *region_space = malloc(sizeof(*region_space));
	region_space->parent = parent;
	region_space->string.initial = aml.initial;
	region_space->string.length = 1;
	region_space->type = aml_region_space;
	return region_space;
}

// <release_op> := <ext_op_prefix> <release_op_suffix>
AMLSymbol *analyse_aml_release_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *release_op = malloc(sizeof(*release_op));
	release_op->parent = parent;
	release_op->string.initial = aml.initial;
	release_op->string.length = 0;
	release_op->type = aml_release_op;
	release_op->component.release_op.ext_op_prefix = analyse_aml_ext_op_prefix(release_op, aml);
	release_op->string.length += release_op->component.release_op.ext_op_prefix->string.length;
	aml.initial += release_op->component.release_op.ext_op_prefix->string.length;
	aml.length -= release_op->component.release_op.ext_op_prefix->string.length;
	release_op->component.release_op.release_op_suffix = analyse_aml_release_op_suffix(release_op, aml);
	release_op->string.length += release_op->component.release_op.release_op_suffix->string.length;
	aml.initial += release_op->component.release_op.release_op_suffix->string.length;
	aml.length -= release_op->component.release_op.release_op_suffix->string.length;
	return release_op;
}

// <release_op_suffix> := AML_BYTE_RELEASE_OP
AMLSymbol *analyse_aml_release_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *release_op = malloc(sizeof(*release_op));
	release_op->parent = parent;
	release_op->string.initial = aml.initial;
	release_op->string.length = 1;
	release_op->type = aml_release_op;
	if(*aml.initial != AML_BYTE_RELEASE_OP)ERROR(); // Incorrect release_op
	return release_op;
}

// <return_op> := AML_BYTE_RETURN_OP
AMLSymbol *analyse_aml_return_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *return_op = malloc(sizeof(*return_op));
	return_op->parent = parent;
	return_op->string.initial = aml.initial;
	return_op->string.length = 1;
	return_op->type = aml_return_op;
	if(*aml.initial != AML_BYTE_RETURN_OP)ERROR(); // Incorrect return_op
	return return_op;
}

// <revision_op> := <ext_op_prefix> <revision_op_suffix>
AMLSymbol *analyse_aml_revision_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *revision_op = malloc(sizeof(*revision_op));
	revision_op->parent = parent;
	revision_op->string.initial = aml.initial;
	revision_op->string.length = 0;
	revision_op->type = aml_revision_op;
	revision_op->component.revision_op.ext_op_prefix = analyse_aml_ext_op_prefix(revision_op, aml);
	revision_op->string.length += revision_op->component.revision_op.ext_op_prefix->string.length;
	aml.initial += revision_op->component.revision_op.ext_op_prefix->string.length;
	aml.length -= revision_op->component.revision_op.ext_op_prefix->string.length;
	revision_op->component.revision_op.revision_op_suffix = analyse_aml_revision_op_suffix(revision_op, aml);
	revision_op->string.length += revision_op->component.revision_op.revision_op_suffix->string.length;
	aml.initial += revision_op->component.revision_op.revision_op_suffix->string.length;
	aml.length -= revision_op->component.revision_op.revision_op_suffix->string.length;
	return revision_op;
}

// <revision_op_suffix> := AML_BYTE_REVISION_OP
AMLSymbol *analyse_aml_revision_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *revision_op_suffix = malloc(sizeof(*revision_op_suffix));
	revision_op_suffix->parent = parent;
	revision_op_suffix->string.initial = aml.initial;
	revision_op_suffix->string.length = 1;
	revision_op_suffix->type = aml_revision_op_suffix;
	if(*revision_op_suffix->string.initial != AML_BYTE_REVISION_OP)ERROR(); // Incorrect revision op prefix
	return revision_op_suffix;
}

// <root_char> := AML_BYTE_ROOT_CHAR
AMLSymbol *analyse_aml_root_char(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *root_char = malloc(sizeof(*root_char));
	root_char->parent = parent;
	root_char->string.initial = aml.initial;
	root_char->string.length = 1;
	root_char->type = aml_root_char;
	if(*root_char->string.initial != AML_BYTE_ROOT_CHAR)ERROR(); // Incorrect root_char
	return root_char;
}

// <scope_op> := AML_BYTE_SCOPE_OP
AMLSymbol *analyse_aml_scope_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *scope_op = malloc(sizeof(*scope_op));
	scope_op->parent = parent;
	scope_op->string.initial = aml.initial;
	scope_op->string.length = 1;
	scope_op->type = aml_scope_op;
	if(*scope_op->string.initial != AML_BYTE_SCOPE_OP)ERROR(); // Incorrect scope op
	return scope_op;
}

// <seg_count> := 0x01 - 0xff
AMLSymbol *analyse_aml_seg_count(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *seg_count = malloc(sizeof(*seg_count));
	seg_count->parent = parent;
	seg_count->string.initial = aml.initial;
	seg_count->string.length = 1;
	seg_count->type = aml_seg_count;
	if(*seg_count->string.initial == 0x00)ERROR(); // SegCount can be from 1 to 255.
	return seg_count;
}

// <shift_count> := <term_arg>
AMLSymbol *analyse_aml_shift_count(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *shift_count = malloc(sizeof(*shift_count));
	shift_count->parent = parent;
	shift_count->string.initial = aml.initial;
	shift_count->string.length = 0;
	shift_count->type = aml_shift_count;
	shift_count->component.shift_count.term_arg = analyse_aml_term_arg(shift_count, aml);
	shift_count->string.length += shift_count->component.shift_count.term_arg->string.length;
	aml.initial += shift_count->component.shift_count.term_arg->string.length;
	aml.length -= shift_count->component.shift_count.term_arg->string.length;
	return shift_count;
}

// <shift_left_op> := AML_BYTE_SHIFT_LEFT
AMLSymbol *analyse_aml_shift_left_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *shift_left_op = malloc(sizeof(*shift_left_op));
	shift_left_op->parent = parent;
	shift_left_op->string.initial = aml.initial;
	shift_left_op->string.length = 1;
	shift_left_op->type = aml_shift_left_op;
	if(*shift_left_op->string.initial != AML_BYTE_SHIFT_LEFT_OP)ERROR(); // Incorrect shift_left_op
	return shift_left_op;
}

// <shift_right_op> := AML_BYTE_SHIFT_RIGHT
AMLSymbol *analyse_aml_shift_right_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *shift_right_op = malloc(sizeof(*shift_right_op));
	shift_right_op->parent = parent;
	shift_right_op->string.initial = aml.initial;
	shift_right_op->string.length = 1;
	shift_right_op->type = aml_shift_right_op;
	if(*shift_right_op->string.initial != AML_BYTE_SHIFT_RIGHT_OP)ERROR(); // Incorrect shift_right_op
	return shift_right_op;
}

// <simple_name> := <name_string> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_simple_name(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *simple_name = malloc(sizeof(*simple_name));
	simple_name->parent = parent;
	simple_name->string.initial = aml.initial;
	simple_name->string.length = 0;
	simple_name->type = aml_simple_name;
	simple_name->component.simple_name.name_string = NULL;
	simple_name->component.simple_name.arg_obj = NULL;
	simple_name->component.simple_name.local_obj = NULL;
	switch(*simple_name->string.initial)
	{
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
		simple_name->component.simple_name.arg_obj = analyse_aml_arg_obj(simple_name, aml);
		simple_name->string.length += simple_name->component.simple_name.arg_obj->string.length;
		aml.initial += simple_name->component.simple_name.arg_obj->string.length;
		aml.length -= simple_name->component.simple_name.arg_obj->string.length;
		break;
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
		simple_name->component.simple_name.local_obj = analyse_aml_local_obj(simple_name, aml);
		simple_name->string.length += simple_name->component.simple_name.local_obj->string.length;
		aml.initial += simple_name->component.simple_name.local_obj->string.length;
		aml.length -= simple_name->component.simple_name.local_obj->string.length;
		break;
	case AML_BYTE_NULL_NAME:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
		simple_name->component.simple_name.name_string = analyse_aml_name_string(simple_name, aml);
		simple_name->string.length += simple_name->component.simple_name.name_string->string.length;
		aml.initial += simple_name->component.simple_name.name_string->string.length;
		aml.length -= simple_name->component.simple_name.name_string->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			simple_name->component.simple_name.name_string = analyse_aml_name_string(simple_name, aml);
			simple_name->string.length += simple_name->component.simple_name.name_string->string.length;
			aml.initial += simple_name->component.simple_name.name_string->string.length;
			aml.length -= simple_name->component.simple_name.name_string->string.length;
		}
		else
		{
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return simple_name;
}

// <size_of_op> := AML_BYTE_SIZE_OF_OP
AMLSymbol *analyse_aml_size_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *size_of_op = malloc(sizeof(*size_of_op));
	size_of_op->parent = parent;
	size_of_op->string.initial = aml.initial;
	size_of_op->string.length = 1;
	size_of_op->type = aml_size_of_op;
	if(*size_of_op->string.initial != AML_BYTE_SIZE_OF_OP)ERROR(); // Incorrect size of op
	return size_of_op;
}

// <statement_opcode> := <def_break> | <def_breakpoint> | <def_continue> | <def_fatal> | <def_if_else> | <def_noop> | <def_notify> | <def_release> | <def_reset> | <def_return> | <def_signal> | <def_sleep> | <def_stall> | <def_while>
AMLSymbol *analyse_aml_statement_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *statement_opcode = malloc(sizeof(*statement_opcode));
	statement_opcode->parent = parent;
	statement_opcode->string.initial = aml.initial;
	statement_opcode->string.length = 0;
	statement_opcode->type = aml_statement_opcode;
	statement_opcode->component.statement_opcode.def_break = NULL;
	statement_opcode->component.statement_opcode.def_break_point = NULL;
	statement_opcode->component.statement_opcode.def_continue = NULL;
	statement_opcode->component.statement_opcode.def_fatal = NULL;
	statement_opcode->component.statement_opcode.def_if_else = NULL;
	statement_opcode->component.statement_opcode.def_noop = NULL;
	statement_opcode->component.statement_opcode.def_notify = NULL;
	statement_opcode->component.statement_opcode.def_release = NULL;
	statement_opcode->component.statement_opcode.def_reset = NULL;
	statement_opcode->component.statement_opcode.def_return = NULL;
	statement_opcode->component.statement_opcode.def_signal = NULL;
	statement_opcode->component.statement_opcode.def_sleep = NULL;
	statement_opcode->component.statement_opcode.def_stall = NULL;
	statement_opcode->component.statement_opcode.def_while = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_RELEASE_OP:
			statement_opcode->component.statement_opcode.def_release = analyse_aml_def_release(statement_opcode, aml);
			statement_opcode->string.length += statement_opcode->component.statement_opcode.def_release->string.length;
			break;
		default:
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("aml.initial[0] = %#04.2x\n", aml.initial[0]);
			printf_serial("aml.initial[1] = %#04.2x\n", aml.initial[1]);
			break;
		}
		break;
	case AML_BYTE_IF_OP:
		statement_opcode->component.statement_opcode.def_if_else = analyse_aml_def_if_else(statement_opcode, aml);
		statement_opcode->string.length += statement_opcode->component.statement_opcode.def_if_else->string.length;
		break;
	case AML_BYTE_RETURN_OP:
		statement_opcode->component.statement_opcode.def_return = analyse_aml_def_return(statement_opcode, aml);
		statement_opcode->string.length += statement_opcode->component.statement_opcode.def_return->string.length;
		break;
	case AML_BYTE_WHILE_OP:
		statement_opcode->component.statement_opcode.def_while = analyse_aml_def_while(statement_opcode, aml);
		statement_opcode->string.length += statement_opcode->component.statement_opcode.def_while->string.length;
		break;
	default:
		ERROR(); // Syntax error or unimplemented pattern
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return statement_opcode;
}

// <store_op> := AML_BYTE_STORE_OP
AMLSymbol *analyse_aml_store_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *store_op = malloc(sizeof(*store_op));
	store_op->parent = parent;
	store_op->string.initial = aml.initial;
	store_op->string.length = 1;
	store_op->type = aml_store_op;
	if(*store_op->string.initial != AML_BYTE_STORE_OP)ERROR(); // Incorrect store op
	return store_op;
}

// <string> := <string_prefix> <ascii_char_list> <null_char>
AMLSymbol *analyse_aml_string(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *string = malloc(sizeof(*string));
	string->parent = parent;
	string->string.initial = aml.initial;
	string->string.length = 0;
	string->type = aml_string;
	string->component.string.string_prefix = analyse_aml_string_prefix(string, aml);
	string->string.length += string->component.string.string_prefix->string.length;
	aml.initial += string->component.string.string_prefix->string.length;
	aml.length -= string->component.string.string_prefix->string.length;
	string->component.string.ascii_char_list = analyse_aml_ascii_char_list(string, aml);
	string->string.length += string->component.string.ascii_char_list->string.length;
	aml.initial += string->component.string.ascii_char_list->string.length;
	aml.length -= string->component.string.ascii_char_list->string.length;
	string->component.string.null_char = analyse_aml_null_char(string, aml);
	string->string.length += string->component.string.null_char->string.length;
	aml.initial += string->component.string.null_char->string.length;
	aml.length -= string->component.string.null_char->string.length;
	return string;
}

// <string_prefix> := AML_BYTE_STRING_PREFIX
AMLSymbol *analyse_aml_string_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *string_prefix = malloc(sizeof(*string_prefix));
	string_prefix->parent = parent;
	string_prefix->string.initial = aml.initial;
	string_prefix->string.length = 1;
	string_prefix->type = aml_string_prefix;
	if(*string_prefix->string.initial != AML_BYTE_STRING_PREFIX)ERROR(); // Incorrect string prefix
	return string_prefix;
}

// <subtract_op> := AML_BYTE_SUBTRACT_OP
AMLSymbol *analyse_aml_subtract_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *subtract_op = malloc(sizeof(*subtract_op));
	subtract_op->parent = parent;
	subtract_op->string.initial = aml.initial;
	subtract_op->string.length = 1;
	subtract_op->type = aml_subtract_op;
	if(*subtract_op->string.initial != AML_BYTE_SUBTRACT_OP)ERROR(); // Incorrect subtract op
	return subtract_op;
}

// <super_name> := <simple_name> | <debug_obj> | <reference_type_opcode>
AMLSymbol *analyse_aml_super_name(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *super_name = malloc(sizeof(*super_name));
	super_name->parent = parent;
	super_name->string.initial = aml.initial;
	super_name->string.length = 0;
	super_name->type = aml_super_name;
	super_name->component.super_name.debug_obj = NULL;
	super_name->component.super_name.reference_type_opcode = NULL;
	super_name->component.super_name.simple_name = NULL;
	switch(*super_name->string.initial)
	{
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
	case AML_BYTE_NULL_NAME:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
		super_name->component.super_name.simple_name = analyse_aml_simple_name(super_name, aml);
		super_name->string.length += super_name->component.super_name.simple_name->string.length;
		aml.initial += super_name->component.super_name.simple_name->string.length;
		aml.length -= super_name->component.super_name.simple_name->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			super_name->component.super_name.simple_name = analyse_aml_simple_name(super_name, aml);
			super_name->string.length += super_name->component.super_name.simple_name->string.length;
			aml.initial += super_name->component.super_name.simple_name->string.length;
			aml.length -= super_name->component.super_name.simple_name->string.length;
		}
		else
		{
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return super_name;
}

// <sync_flags>
AMLSymbol *analyse_aml_sync_flags(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *sync_flags = malloc(sizeof(*sync_flags));
	sync_flags->parent = parent;
	sync_flags->string.initial = aml.initial;
	sync_flags->string.length = 1;
	sync_flags->type = aml_sync_flags;
	if(0x10 <= *aml.initial)ERROR(); // Incorrect sync flags
	return sync_flags;
}

// <target> := <super_name> | <null_name>
AMLSymbol *analyse_aml_target(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *target = malloc(sizeof(*target));
	target->parent = parent;
	target->string.initial = aml.initial;
	target->string.length = 0;
	target->type = aml_target;
	target->component.target.super_name = NULL;
	target->component.target.null_name = NULL;
	switch(*target->string.initial)
	{
	case AML_BYTE_NULL_NAME:
		target->component.target.null_name = analyse_aml_null_name(target, aml);
		target->string.length += target->component.target.null_name->string.length;
		aml.initial += target->component.target.null_name->string.length;
		aml.length -= target->component.target.null_name->string.length;
		break;
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
		target->component.target.super_name = analyse_aml_super_name(target, aml);
		target->string.length += target->component.target.super_name->string.length;
		aml.initial += target->component.target.super_name->string.length;
		aml.length -= target->component.target.super_name->string.length;
		break;
	default:
		ERROR(); // Syntax error or unimplemented pattern
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return target;
}

// <term_arg> := <expression_opcode> | <data_object> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_term_arg(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *term_arg = malloc(sizeof(*term_arg));
	term_arg->parent = parent;
	term_arg->string.initial = aml.initial;
	term_arg->string.length = 0;
	term_arg->type = aml_term_arg;
	term_arg->component.term_arg.expression_opcode = NULL;
	term_arg->component.term_arg.data_object = NULL;
	term_arg->component.term_arg.arg_obj = NULL;
	term_arg->component.term_arg.local_obj = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
		term_arg->component.term_arg.arg_obj = analyse_aml_arg_obj(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.arg_obj->string.length;
		break;
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		term_arg->component.term_arg.data_object = analyse_aml_data_object(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.data_object->string.length;
		break;
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_INCREMENT_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
		term_arg->component.term_arg.expression_opcode = analyse_aml_expression_opcode(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.expression_opcode->string.length;
		break;
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
		term_arg->component.term_arg.local_obj = analyse_aml_local_obj(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.local_obj->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			term_arg->component.term_arg.expression_opcode = analyse_aml_expression_opcode(term_arg, aml);
			term_arg->string.length += term_arg->component.term_arg.expression_opcode->string.length;
		}
		else
		{
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return term_arg;
}

// <term_arg_list> := Nothing | <term_arg> <term_arg_list>
AMLSymbol *analyse_aml_term_arg_list(AMLSymbol *parent, AMLSubstring aml, unsigned int num_of_term_args)
{
	AMLSymbol *term_arg_list = malloc(sizeof(*term_arg_list));
	term_arg_list->parent = parent;
	term_arg_list->string.initial = aml.initial;
	term_arg_list->string.length = 0;
	term_arg_list->type = aml_term_arg_list;
	term_arg_list->component.term_arg_list.term_arg = NULL;
	term_arg_list->component.term_arg_list.term_arg_list = NULL;
	if(num_of_term_args)switch(*aml.initial)
	{
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_INCREMENT_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		term_arg_list->component.term_arg_list.term_arg = analyse_aml_term_arg(term_arg_list, aml);
		term_arg_list->string.length += term_arg_list->component.term_arg_list.term_arg->string.length;
		aml.initial += term_arg_list->component.term_arg_list.term_arg->string.length;
		aml.length -= term_arg_list->component.term_arg_list.term_arg->string.length;
		term_arg_list->component.term_arg_list.term_arg_list = analyse_aml_term_arg_list(term_arg_list, aml, num_of_term_args - 1);
		term_arg_list->string.length += term_arg_list->component.term_arg_list.term_arg_list->string.length;
		aml.initial += term_arg_list->component.term_arg_list.term_arg_list->string.length;
		aml.length -= term_arg_list->component.term_arg_list.term_arg_list->string.length;
		break;
	default:
		// term_arg_list can be nothing.
		break;
	}
	return term_arg_list;
}

// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *term_list = malloc(sizeof(*term_list));
	term_list->parent = parent;
	term_list->string.initial = aml.initial;
	term_list->string.length = 0;
	term_list->type = aml_term_list;
	term_list->component.term_list.term_list = NULL;
	term_list->component.term_list.term_obj = NULL;
	if(aml.length)
	{
		// <term_list> := <term_obj> <term_list>
		term_list->component.term_list.term_obj = analyse_aml_term_obj(term_list, aml);
		term_list->string.length += term_list->component.term_list.term_obj->string.length;
		aml.initial += term_list->component.term_list.term_obj->string.length;
		aml.length -= term_list->component.term_list.term_obj->string.length;
		if(aml.length && term_list->component.term_list.term_obj->string.length)switch(*aml.initial)
		{
		case AML_BYTE_ALIAS_OP:
		case AML_BYTE_DEREF_OF_OP:
		case AML_BYTE_EXT_OP_PREFIX:
		case AML_BYTE_IF_OP:
		case AML_BYTE_INCREMENT_OP:
		case AML_BYTE_INDEX_OP:
		case AML_BYTE_L_EQUAL_OP:
		case AML_BYTE_L_GREATER_OP:
		case AML_BYTE_L_LESS_OP:
		case AML_BYTE_L_OR_OP:
		case AML_BYTE_METHOD_OP:
		case AML_BYTE_NAME_OP:
		case AML_BYTE_PACKAGE_OP:
		case AML_BYTE_RETURN_OP:
		case AML_BYTE_SCOPE_OP:
		case AML_BYTE_SHIFT_LEFT_OP:
		case AML_BYTE_SHIFT_RIGHT_OP:
		case AML_BYTE_SIZE_OF_OP:
		case AML_BYTE_STORE_OP:
		case AML_BYTE_SUBTRACT_OP:
		case AML_BYTE_TO_BUFFER_OP:
		case AML_BYTE_TO_HEX_STRING_OP:
		case AML_BYTE_WHILE_OP:
			term_list->component.term_list.term_list = analyse_aml_term_list(term_list, aml);
			term_list->string.length += term_list->component.term_list.term_list->string.length;
			aml.initial += term_list->component.term_list.term_list->string.length;
			aml.length -= term_list->component.term_list.term_list->string.length;
			break;
		default:
			if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
			{
				term_list->component.term_list.term_list = analyse_aml_term_list(term_list, aml);
				term_list->string.length += term_list->component.term_list.term_list->string.length;
				aml.initial += term_list->component.term_list.term_list->string.length;
				aml.length -= term_list->component.term_list.term_list->string.length;
			}
			else
			{
				ERROR(); // Syntax error or unimplemented pattern
				printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
			}
			break;
		}
	}
	if(!parent)printf_serial("Root TermList = %p\n", term_list);
	return term_list;
}

// <term_obj> := <object> | <statement_opcode> | <expression_opcode>
AMLSymbol *analyse_aml_term_obj(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *term_obj = malloc(sizeof(*term_obj));
	term_obj->parent = parent;
	term_obj->string.initial = aml.initial;
	term_obj->string.length = 0;
	term_obj->type = aml_term_obj;
	term_obj->component.term_obj.object = NULL;
	term_obj->component.term_obj.statement_opcode = NULL;
	term_obj->component.term_obj.expression_opcode = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_INCREMENT_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
		term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
		break;
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_METHOD_OP:
	case AML_BYTE_NAME_OP:
	case AML_BYTE_SCOPE_OP:
		term_obj->component.term_obj.object = analyse_aml_object(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.object->string.length;
		break;
	case AML_BYTE_IF_OP:
	case AML_BYTE_RETURN_OP:
	case AML_BYTE_WHILE_OP:
		term_obj->component.term_obj.statement_opcode = analyse_aml_statement_opcode(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.statement_opcode->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_ACQUIRE_OP:
			term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
			break;
		case AML_BYTE_RELEASE_OP:
			term_obj->component.term_obj.statement_opcode = analyse_aml_statement_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.statement_opcode->string.length;
			break;
		default:
			term_obj->component.term_obj.object = analyse_aml_object(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.object->string.length;
			break;
		}
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
		}
		else 
		{
			ERROR(); // Syntax error or unimplemented pattern
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return term_obj;
}

// <time_out> := <word_data>
AMLSymbol *analyse_aml_time_out(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *time_out = malloc(sizeof(*time_out));
	time_out->parent = parent;
	time_out->string.initial = aml.initial;
	time_out->string.length = 0;
	time_out->type = aml_time_out;
	time_out->component.time_out.word_data = analyse_aml_word_data(time_out, aml);
	time_out->string.length += time_out->component.time_out.word_data->string.length;
	return time_out;
}

// <to_buffer_op> := AML_BYTE_TO_BUFFER_OP
AMLSymbol *analyse_aml_to_buffer_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *to_buffer_op = malloc(sizeof(*to_buffer_op));
	to_buffer_op->parent = parent;
	to_buffer_op->string.initial = aml.initial;
	to_buffer_op->string.length = 1;
	to_buffer_op->type = aml_to_buffer_op;
	if(*to_buffer_op->string.initial != AML_BYTE_TO_BUFFER_OP)ERROR();
	return to_buffer_op;
}

// <to_hex_string_op> := AML_BYTE_TO_HEX_STRING_OP
AMLSymbol *analyse_aml_to_hex_string_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *to_hex_string_op = malloc(sizeof(*to_hex_string_op));
	to_hex_string_op->parent = parent;
	to_hex_string_op->string.initial = aml.initial;
	to_hex_string_op->string.length = 1;
	to_hex_string_op->type = aml_to_hex_string_op;
	if(*to_hex_string_op->string.initial != AML_BYTE_TO_HEX_STRING_OP)ERROR();
	return to_hex_string_op;
}

// <while_op> := AML_BYTE_WHILE_OP
AMLSymbol *analyse_aml_while_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *while_op = malloc(sizeof(*while_op));
	while_op->parent = parent;
	while_op->string.initial = aml.initial;
	while_op->string.length = 1;
	while_op->type = aml_while_op;
	if(*while_op->string.initial != AML_BYTE_WHILE_OP)ERROR(); // Incorrect while op
	return while_op;
}

// <word_const> := <word_prefix> <word_data>
AMLSymbol *analyse_aml_word_const(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *word_const = malloc(sizeof(*word_const));
	word_const->parent = parent;
	word_const->string.initial = aml.initial;
	word_const->string.length = 0;
	word_const->type = aml_word_const;
	word_const->component.word_const.word_prefix = analyse_aml_word_prefix(word_const, aml);
	word_const->string.length += word_const->component.word_const.word_prefix->string.length;
	aml.initial += word_const->component.word_const.word_prefix->string.length;
	aml.length -= word_const->component.word_const.word_prefix->string.length;
	word_const->component.word_const.word_data = analyse_aml_word_data(word_const, aml);
	word_const->string.length += word_const->component.word_const.word_data->string.length;
	aml.initial += word_const->component.word_const.word_data->string.length;
	aml.length -= word_const->component.word_const.word_data->string.length;
	word_const->component.word_const.value = word_const->component.word_const.word_data->component.word_data.value;
	return word_const;
}

// <word_data> := <byte_data> <byte_data>
AMLSymbol *analyse_aml_word_data(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *word_data = malloc(sizeof(*word_data));
	word_data->parent = parent;
	word_data->string.initial = aml.initial;
	word_data->string.length = 0;
	word_data->type = aml_word_data;
	for(AMLSymbol **byte_data = word_data->component.word_data.byte_data; byte_data != word_data->component.word_data.byte_data + _countof(word_data->component.word_data.byte_data); byte_data++)
	{
		*byte_data = analyse_aml_byte_data(word_data, aml);
		word_data->string.length += (*byte_data)->string.length;
		aml.initial += (*byte_data)->string.length;
		aml.length -= (*byte_data)->string.length;
	}
	word_data->component.word_data.value = *(unsigned short const *)word_data->component.word_data.byte_data[0]->string.initial;
	return word_data;
}

// <word_prefix> := AML_BYTE_WORD_PREFIX
AMLSymbol *analyse_aml_word_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *word_prefix = malloc(sizeof(*word_prefix));
	word_prefix->parent = parent;
	word_prefix->string.initial = aml.initial;
	word_prefix->string.length = 1;
	word_prefix->type = aml_word_prefix;
	if(*aml.initial != AML_BYTE_WORD_PREFIX)ERROR(); // Incorrect word prefix
	return word_prefix;
}

// <zero_op> := AML_BYTE_ZERO_OP
AMLSymbol *analyse_aml_zero_op(AMLSymbol *parent, AMLSubstring aml)
{
	AMLSymbol *zero_op = malloc(sizeof(*zero_op));
	zero_op->parent = parent;
	zero_op->parent = parent;
	zero_op->string.initial = aml.initial;
	zero_op->string.length = 1;
	zero_op->type = aml_zero_op;
	if(*zero_op->string.initial != AML_BYTE_ZERO_OP)ERROR(); // Incorrect zero op
	return zero_op;
}

AMLSymbol *create_dsdt_aml_syntax_tree(void)
{
	AMLSubstring dsdt_aml = get_dsdt_aml();
	if(dsdt_aml.initial)return analyse_aml_term_list(NULL, dsdt_aml);
	else
	{
		ERROR(); // DSDT is not found.
		return NULL;
	}
}

void delete_aml_symbol(AMLSymbol *aml_symbol)
{
	switch(aml_symbol->type)
	{
	case aml_acquire_op:
		if(aml_symbol->component.acquire_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.acquire_op.ext_op_prefix);
		if(aml_symbol->component.acquire_op.acquire_op_suffix)delete_aml_symbol(aml_symbol->component.acquire_op.acquire_op_suffix);
		break;
	case aml_acquire_op_suffix:
		break;
	case aml_alias_op:
		break;
	case aml_arg_obj:
		if(aml_symbol->component.arg_obj.arg_op)delete_aml_symbol(aml_symbol->component.arg_obj.arg_op);
		break;
	case aml_arg_object:
		if(aml_symbol->component.arg_object.term_arg)delete_aml_symbol(aml_symbol->component.arg_object.term_arg);
		break;
	case aml_arg_op:
		break;
	case aml_ascii_char:
		break;
	case aml_ascii_char_list:
		if(aml_symbol->component.ascii_char_list.ascii_char)delete_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char);
		if(aml_symbol->component.ascii_char_list.ascii_char_list)delete_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char_list);
		break;
	case aml_buff_pkg_str_obj:
		if(aml_symbol->component.buff_pkg_str_obj.term_arg)delete_aml_symbol(aml_symbol->component.buff_pkg_str_obj.term_arg);
		break;
	case aml_buffer_op:
		break;
	case aml_buffer_size:
		if(aml_symbol->component.buffer_size.term_arg)delete_aml_symbol(aml_symbol->component.buffer_size.term_arg);
		break;
	case aml_byte_const:
		if(aml_symbol->component.byte_const.byte_prefix)delete_aml_symbol(aml_symbol->component.byte_const.byte_prefix);
		if(aml_symbol->component.byte_const.byte_data)delete_aml_symbol(aml_symbol->component.byte_const.byte_data);
		break;
	case aml_byte_data:
		break;
	case aml_byte_list:
		if(aml_symbol->component.byte_list.byte_data)delete_aml_symbol(aml_symbol->component.byte_list.byte_data);
		if(aml_symbol->component.byte_list.byte_list)delete_aml_symbol(aml_symbol->component.byte_list.byte_list);
		break;
	case aml_byte_prefix:
		break;
	case aml_computational_data:
		if(aml_symbol->component.computational_data.byte_const)delete_aml_symbol(aml_symbol->component.computational_data.byte_const);
		if(aml_symbol->component.computational_data.word_const)delete_aml_symbol(aml_symbol->component.computational_data.word_const);
		if(aml_symbol->component.computational_data.dword_const)delete_aml_symbol(aml_symbol->component.computational_data.dword_const);
		if(aml_symbol->component.computational_data.qword_const)delete_aml_symbol(aml_symbol->component.computational_data.qword_const);
		if(aml_symbol->component.computational_data.string)delete_aml_symbol(aml_symbol->component.computational_data.string);
		if(aml_symbol->component.computational_data.const_obj)delete_aml_symbol(aml_symbol->component.computational_data.const_obj);
		if(aml_symbol->component.computational_data.revision_op)delete_aml_symbol(aml_symbol->component.computational_data.revision_op);
		if(aml_symbol->component.computational_data.def_buffer)delete_aml_symbol(aml_symbol->component.computational_data.def_buffer);
		break;
	case aml_const_obj:
		if(aml_symbol->component.const_obj.zero_op)delete_aml_symbol(aml_symbol->component.const_obj.zero_op);
		if(aml_symbol->component.const_obj.one_op)delete_aml_symbol(aml_symbol->component.const_obj.one_op);
		if(aml_symbol->component.const_obj.ones_op)delete_aml_symbol(aml_symbol->component.const_obj.ones_op);
		break;
	case aml_data_object:
		if(aml_symbol->component.data_object.computational_data)delete_aml_symbol(aml_symbol->component.data_object.computational_data);
		if(aml_symbol->component.data_object.def_package)delete_aml_symbol(aml_symbol->component.data_object.def_package);
		if(aml_symbol->component.data_object.def_var_package)delete_aml_symbol(aml_symbol->component.data_object.def_var_package);
		break;
	case aml_data_ref_object:
		if(aml_symbol->component.data_ref_object.data_object)delete_aml_symbol(aml_symbol->component.data_ref_object.data_object);
		if(aml_symbol->component.data_ref_object.object_reference)delete_aml_symbol(aml_symbol->component.data_ref_object.object_reference);
		break;
	case aml_def_alias:
		if(aml_symbol->component.def_alias.alias_op)delete_aml_symbol(aml_symbol->component.def_alias.alias_op);
		for(AMLSymbol **name_string = aml_symbol->component.def_alias.name_string; name_string != aml_symbol->component.def_alias.name_string + _countof(aml_symbol->component.def_alias.name_string); name_string++)if(*name_string)delete_aml_symbol(*name_string);
		break;
	case aml_def_acquire:
		if(aml_symbol->component.def_acquire.acquire_op)delete_aml_symbol(aml_symbol->component.def_acquire.acquire_op);
		if(aml_symbol->component.def_acquire.mutex_object)delete_aml_symbol(aml_symbol->component.def_acquire.mutex_object);
		if(aml_symbol->component.def_acquire.time_out)delete_aml_symbol(aml_symbol->component.def_acquire.time_out);
		break;
	case aml_def_buffer:
		if(aml_symbol->component.def_buffer.buffer_op)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_op);
		if(aml_symbol->component.def_buffer.pkg_length)delete_aml_symbol(aml_symbol->component.def_buffer.pkg_length);
		if(aml_symbol->component.def_buffer.buffer_size)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_size);
		if(aml_symbol->component.def_buffer.byte_list)delete_aml_symbol(aml_symbol->component.def_buffer.byte_list);
		break;
	case aml_def_deref_of:
		if(aml_symbol->component.def_deref_of.deref_of_op)delete_aml_symbol(aml_symbol->component.def_deref_of.deref_of_op);
		if(aml_symbol->component.def_deref_of.obj_reference)delete_aml_symbol(aml_symbol->component.def_deref_of.obj_reference);
		break;
	case aml_def_device:
		if(aml_symbol->component.def_device.device_op)delete_aml_symbol(aml_symbol->component.def_device.device_op);
		if(aml_symbol->component.def_device.pkg_length)delete_aml_symbol(aml_symbol->component.def_device.pkg_length);
		if(aml_symbol->component.def_device.name_string)delete_aml_symbol(aml_symbol->component.def_device.name_string);
		if(aml_symbol->component.def_device.term_list)delete_aml_symbol(aml_symbol->component.def_device.term_list);
		break;
	case aml_def_field:
		if(aml_symbol->component.def_field.field_op)delete_aml_symbol(aml_symbol->component.def_field.field_op);
		if(aml_symbol->component.def_field.pkg_length)delete_aml_symbol(aml_symbol->component.def_field.pkg_length);
		if(aml_symbol->component.def_field.name_string)delete_aml_symbol(aml_symbol->component.def_field.name_string);
		if(aml_symbol->component.def_field.field_flags)delete_aml_symbol(aml_symbol->component.def_field.field_flags);
		if(aml_symbol->component.def_field.field_list)delete_aml_symbol(aml_symbol->component.def_field.field_list);
		break;
	case aml_def_if_else:
		if(aml_symbol->component.def_if_else.if_op)delete_aml_symbol(aml_symbol->component.def_if_else.if_op);
		if(aml_symbol->component.def_if_else.pkg_length)delete_aml_symbol(aml_symbol->component.def_if_else.pkg_length);
		if(aml_symbol->component.def_if_else.predicate)delete_aml_symbol(aml_symbol->component.def_if_else.predicate);
		if(aml_symbol->component.def_if_else.term_list)delete_aml_symbol(aml_symbol->component.def_if_else.term_list);
		if(aml_symbol->component.def_if_else.def_else)delete_aml_symbol(aml_symbol->component.def_if_else.def_else);
		break;
	case aml_def_increment:
		if(aml_symbol->component.def_increment.increment_op)delete_aml_symbol(aml_symbol->component.def_increment.increment_op);
		if(aml_symbol->component.def_increment.super_name)delete_aml_symbol(aml_symbol->component.def_increment.super_name);
		break;
	case aml_def_index:
		if(aml_symbol->component.def_index.index_op)delete_aml_symbol(aml_symbol->component.def_index.index_op);
		if(aml_symbol->component.def_index.buff_pkg_str_obj)delete_aml_symbol(aml_symbol->component.def_index.buff_pkg_str_obj);
		if(aml_symbol->component.def_index.index_value)delete_aml_symbol(aml_symbol->component.def_index.index_value);
		if(aml_symbol->component.def_index.target)delete_aml_symbol(aml_symbol->component.def_index.target);
		break;
	case aml_def_l_equal:
		if(aml_symbol->component.def_l_equal.l_equal_op)delete_aml_symbol(aml_symbol->component.def_l_equal.l_equal_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_equal.operand); i++)if(aml_symbol->component.def_l_equal.operand[i])delete_aml_symbol(aml_symbol->component.def_l_equal.operand[i]);
		break;
	case aml_def_l_greater:
		if(aml_symbol->component.def_l_greater.l_greater_op)delete_aml_symbol(aml_symbol->component.def_l_greater.l_greater_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_greater.operand); i++)if(aml_symbol->component.def_l_greater.operand[i])delete_aml_symbol(aml_symbol->component.def_l_greater.operand[i]);
		break;
	case aml_def_l_less:
		if(aml_symbol->component.def_l_less.l_less_op)delete_aml_symbol(aml_symbol->component.def_l_less.l_less_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_less.operand); i++)if(aml_symbol->component.def_l_less.operand[i])delete_aml_symbol(aml_symbol->component.def_l_less.operand[i]);
		break;
	case aml_def_l_or:
		if(aml_symbol->component.def_l_or.l_or_op)delete_aml_symbol(aml_symbol->component.def_l_or.l_or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)if(aml_symbol->component.def_l_or.operand[i])delete_aml_symbol(aml_symbol->component.def_l_or.operand[i]);
		break;
	case aml_def_method:
		if(aml_symbol->component.def_method.method_op)delete_aml_symbol(aml_symbol->component.def_method.method_op);
		if(aml_symbol->component.def_method.pkg_length)delete_aml_symbol(aml_symbol->component.def_method.pkg_length);
		if(aml_symbol->component.def_method.name_string)delete_aml_symbol(aml_symbol->component.def_method.name_string);
		if(aml_symbol->component.def_method.method_flags)delete_aml_symbol(aml_symbol->component.def_method.method_flags);
		if(aml_symbol->component.def_method.term_list)delete_aml_symbol(aml_symbol->component.def_method.term_list);
		break;
	case aml_def_mutex:
		if(aml_symbol->component.def_mutex.mutex_op)delete_aml_symbol(aml_symbol->component.def_mutex.mutex_op);
		if(aml_symbol->component.def_mutex.name_string)delete_aml_symbol(aml_symbol->component.def_mutex.name_string);
		if(aml_symbol->component.def_mutex.sync_flags)delete_aml_symbol(aml_symbol->component.def_mutex.sync_flags);
		break;
	case aml_def_name:
		if(aml_symbol->component.def_name.name_op)delete_aml_symbol(aml_symbol->component.def_name.name_op);
		if(aml_symbol->component.def_name.name_string)delete_aml_symbol(aml_symbol->component.def_name.name_string);
		if(aml_symbol->component.def_name.data_ref_object)delete_aml_symbol(aml_symbol->component.def_name.data_ref_object);
		break;
	case aml_def_op_region:
		if(aml_symbol->component.def_op_region.op_region_op)delete_aml_symbol(aml_symbol->component.def_op_region.op_region_op);
		if(aml_symbol->component.def_op_region.name_string)delete_aml_symbol(aml_symbol->component.def_op_region.name_string);
		if(aml_symbol->component.def_op_region.region_space)delete_aml_symbol(aml_symbol->component.def_op_region.region_space);
		if(aml_symbol->component.def_op_region.region_offset)delete_aml_symbol(aml_symbol->component.def_op_region.region_offset);
		if(aml_symbol->component.def_op_region.region_len)delete_aml_symbol(aml_symbol->component.def_op_region.region_len);
		break;
	case aml_def_package:
		if(aml_symbol->component.def_package.package_op)delete_aml_symbol(aml_symbol->component.def_package.package_op);
		if(aml_symbol->component.def_package.pkg_length)delete_aml_symbol(aml_symbol->component.def_package.pkg_length);
		if(aml_symbol->component.def_package.num_elements)delete_aml_symbol(aml_symbol->component.def_package.num_elements);
		if(aml_symbol->component.def_package.package_element_list)delete_aml_symbol(aml_symbol->component.def_package.package_element_list);
		break;
	case aml_def_release:
		if(aml_symbol->component.def_release.release_op)delete_aml_symbol(aml_symbol->component.def_release.release_op);
		if(aml_symbol->component.def_release.mutex_object)delete_aml_symbol(aml_symbol->component.def_release.mutex_object);
		break;
	case aml_def_return:
		if(aml_symbol->component.def_return.return_op)delete_aml_symbol(aml_symbol->component.def_return.return_op);
		if(aml_symbol->component.def_return.arg_object)delete_aml_symbol(aml_symbol->component.def_return.arg_object);
		break;
	case aml_def_scope:
		if(aml_symbol->component.def_scope.scope_op)delete_aml_symbol(aml_symbol->component.def_scope.scope_op);
		if(aml_symbol->component.def_scope.pkg_length)delete_aml_symbol(aml_symbol->component.def_scope.pkg_length);
		if(aml_symbol->component.def_scope.name_string)delete_aml_symbol(aml_symbol->component.def_scope.name_string);
		if(aml_symbol->component.def_scope.term_list)delete_aml_symbol(aml_symbol->component.def_scope.term_list);
		break;
	case aml_def_shift_left:
		if(aml_symbol->component.def_shift_left.shift_left_op)delete_aml_symbol(aml_symbol->component.def_shift_left.shift_left_op);
		if(aml_symbol->component.def_shift_left.operand)delete_aml_symbol(aml_symbol->component.def_shift_left.operand);
		if(aml_symbol->component.def_shift_left.shift_count)delete_aml_symbol(aml_symbol->component.def_shift_left.shift_count);
		if(aml_symbol->component.def_shift_left.target)delete_aml_symbol(aml_symbol->component.def_shift_left.target);
		break;
	case aml_def_shift_right:
		if(aml_symbol->component.def_shift_right.shift_right_op)delete_aml_symbol(aml_symbol->component.def_shift_right.shift_right_op);
		if(aml_symbol->component.def_shift_right.operand)delete_aml_symbol(aml_symbol->component.def_shift_right.operand);
		if(aml_symbol->component.def_shift_right.shift_count)delete_aml_symbol(aml_symbol->component.def_shift_right.shift_count);
		if(aml_symbol->component.def_shift_right.target)delete_aml_symbol(aml_symbol->component.def_shift_right.target);
		break;
	case aml_def_size_of:
		if(aml_symbol->component.def_size_of.size_of_op)delete_aml_symbol(aml_symbol->component.def_size_of.size_of_op);
		if(aml_symbol->component.def_size_of.super_name)delete_aml_symbol(aml_symbol->component.def_size_of.super_name);
		break;
	case aml_def_store:
		if(aml_symbol->component.def_store.store_op)delete_aml_symbol(aml_symbol->component.def_store.store_op);
		if(aml_symbol->component.def_store.term_arg)delete_aml_symbol(aml_symbol->component.def_store.term_arg);
		if(aml_symbol->component.def_store.super_name)delete_aml_symbol(aml_symbol->component.def_store.super_name);
		break;
	case aml_def_subtract:
		if(aml_symbol->component.def_subtract.subtract_op)delete_aml_symbol(aml_symbol->component.def_subtract.subtract_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_subtract.operand); i++)if(aml_symbol->component.def_subtract.operand[i])delete_aml_symbol(aml_symbol->component.def_subtract.operand[i]);
		if(aml_symbol->component.def_subtract.target)delete_aml_symbol(aml_symbol->component.def_subtract.target);
		break;
	case aml_def_to_buffer:
		if(aml_symbol->component.def_to_buffer.to_buffer_op)delete_aml_symbol(aml_symbol->component.def_to_buffer.to_buffer_op);
		if(aml_symbol->component.def_to_buffer.operand)delete_aml_symbol(aml_symbol->component.def_to_buffer.operand);
		if(aml_symbol->component.def_to_buffer.target)delete_aml_symbol(aml_symbol->component.def_to_buffer.target);
		break;
	case aml_def_to_hex_string:
		if(aml_symbol->component.def_to_hex_string.to_hex_string_op)delete_aml_symbol(aml_symbol->component.def_to_hex_string.to_hex_string_op);
		if(aml_symbol->component.def_to_hex_string.operand)delete_aml_symbol(aml_symbol->component.def_to_hex_string.operand);
		if(aml_symbol->component.def_to_hex_string.target)delete_aml_symbol(aml_symbol->component.def_to_hex_string.target);
		break;
	case aml_def_while:
		if(aml_symbol->component.def_while.while_op)delete_aml_symbol(aml_symbol->component.def_while.while_op);
		if(aml_symbol->component.def_while.pkg_length)delete_aml_symbol(aml_symbol->component.def_while.pkg_length);
		if(aml_symbol->component.def_while.predicate)delete_aml_symbol(aml_symbol->component.def_while.predicate);
		if(aml_symbol->component.def_while.term_list)delete_aml_symbol(aml_symbol->component.def_while.term_list);
		break;
	case aml_deref_of_op:
		break;
	case aml_device_op:
		if(aml_symbol->component.device_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.device_op.ext_op_prefix);
		if(aml_symbol->component.device_op.device_op_suffix)delete_aml_symbol(aml_symbol->component.device_op.device_op_suffix);
		break;
	case aml_device_op_suffix:
		break;
	case aml_digit_char:
		break;
	case aml_dual_name_path:
		if(aml_symbol->component.dual_name_path.dual_name_prefix)delete_aml_symbol(aml_symbol->component.dual_name_path.dual_name_prefix);
		for(AMLSymbol **name_seg = aml_symbol->component.dual_name_path.name_seg; name_seg != aml_symbol->component.dual_name_path.name_seg + _countof(aml_symbol->component.dual_name_path.name_seg); name_seg++)if(*name_seg)delete_aml_symbol(*name_seg);
		break;
	case aml_dual_name_prefix:
		break;
	case aml_dword_const:
		if(aml_symbol->component.dword_const.dword_prefix)delete_aml_symbol(aml_symbol->component.dword_const.dword_prefix);
		if(aml_symbol->component.dword_const.dword_data)delete_aml_symbol(aml_symbol->component.dword_const.dword_data);
		break;
	case aml_dword_data:
		for(AMLSymbol **word_data = aml_symbol->component.dword_data.word_data; word_data != aml_symbol->component.dword_data.word_data + _countof(aml_symbol->component.dword_data.word_data); word_data++)if(*word_data)delete_aml_symbol(*word_data);
		break;
	case aml_dword_prefix:
		break;
	case aml_expression_opcode:
		if(aml_symbol->component.expression_opcode.def_add)delete_aml_symbol(aml_symbol->component.expression_opcode.def_add);
		if(aml_symbol->component.expression_opcode.def_and)delete_aml_symbol(aml_symbol->component.expression_opcode.def_and);
		if(aml_symbol->component.expression_opcode.def_acquire)delete_aml_symbol(aml_symbol->component.expression_opcode.def_acquire);
		if(aml_symbol->component.expression_opcode.def_buffer)delete_aml_symbol(aml_symbol->component.expression_opcode.def_buffer);
		if(aml_symbol->component.expression_opcode.def_concat)delete_aml_symbol(aml_symbol->component.expression_opcode.def_concat);
		if(aml_symbol->component.expression_opcode.def_concat_res)delete_aml_symbol(aml_symbol->component.expression_opcode.def_concat_res);
		if(aml_symbol->component.expression_opcode.def_cond_ref_of)delete_aml_symbol(aml_symbol->component.expression_opcode.def_cond_ref_of);
		if(aml_symbol->component.expression_opcode.def_copy_object)delete_aml_symbol(aml_symbol->component.expression_opcode.def_copy_object);
		if(aml_symbol->component.expression_opcode.def_decrement)delete_aml_symbol(aml_symbol->component.expression_opcode.def_decrement);
		if(aml_symbol->component.expression_opcode.def_deref_of)delete_aml_symbol(aml_symbol->component.expression_opcode.def_deref_of);
		if(aml_symbol->component.expression_opcode.def_divide)delete_aml_symbol(aml_symbol->component.expression_opcode.def_divide);
		if(aml_symbol->component.expression_opcode.def_find_set_left_bit)delete_aml_symbol(aml_symbol->component.expression_opcode.def_find_set_left_bit);
		if(aml_symbol->component.expression_opcode.def_find_set_right_bit)delete_aml_symbol(aml_symbol->component.expression_opcode.def_find_set_right_bit);
		if(aml_symbol->component.expression_opcode.def_from_bcd)delete_aml_symbol(aml_symbol->component.expression_opcode.def_from_bcd);
		if(aml_symbol->component.expression_opcode.def_increment)delete_aml_symbol(aml_symbol->component.expression_opcode.def_increment);
		if(aml_symbol->component.expression_opcode.def_index)delete_aml_symbol(aml_symbol->component.expression_opcode.def_index);
		if(aml_symbol->component.expression_opcode.def_l_and)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_and);
		if(aml_symbol->component.expression_opcode.def_l_equal)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_equal);
		if(aml_symbol->component.expression_opcode.def_l_greater)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_greater);
		if(aml_symbol->component.expression_opcode.def_l_greater_equal)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_greater_equal);
		if(aml_symbol->component.expression_opcode.def_l_less)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_less);
		if(aml_symbol->component.expression_opcode.def_l_less_equal)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_less_equal);
		if(aml_symbol->component.expression_opcode.def_l_not)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_not);
		if(aml_symbol->component.expression_opcode.def_l_not_equal)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_not_equal);
		if(aml_symbol->component.expression_opcode.def_l_or)delete_aml_symbol(aml_symbol->component.expression_opcode.def_l_or);
		if(aml_symbol->component.expression_opcode.def_load_table)delete_aml_symbol(aml_symbol->component.expression_opcode.def_load_table);
		if(aml_symbol->component.expression_opcode.def_match)delete_aml_symbol(aml_symbol->component.expression_opcode.def_match);
		if(aml_symbol->component.expression_opcode.def_mid)delete_aml_symbol(aml_symbol->component.expression_opcode.def_mid);
		if(aml_symbol->component.expression_opcode.def_mod)delete_aml_symbol(aml_symbol->component.expression_opcode.def_mod);
		if(aml_symbol->component.expression_opcode.def_multiply)delete_aml_symbol(aml_symbol->component.expression_opcode.def_multiply);
		if(aml_symbol->component.expression_opcode.def_nand)delete_aml_symbol(aml_symbol->component.expression_opcode.def_nand);
		if(aml_symbol->component.expression_opcode.def_nor)delete_aml_symbol(aml_symbol->component.expression_opcode.def_nor);
		if(aml_symbol->component.expression_opcode.def_not)delete_aml_symbol(aml_symbol->component.expression_opcode.def_not);
		if(aml_symbol->component.expression_opcode.def_object_type)delete_aml_symbol(aml_symbol->component.expression_opcode.def_object_type);
		if(aml_symbol->component.expression_opcode.def_or)delete_aml_symbol(aml_symbol->component.expression_opcode.def_or);
		if(aml_symbol->component.expression_opcode.def_package)delete_aml_symbol(aml_symbol->component.expression_opcode.def_package);
		if(aml_symbol->component.expression_opcode.def_ref_of)delete_aml_symbol(aml_symbol->component.expression_opcode.def_ref_of);
		if(aml_symbol->component.expression_opcode.def_shift_left)delete_aml_symbol(aml_symbol->component.expression_opcode.def_shift_left);
		if(aml_symbol->component.expression_opcode.def_shift_right)delete_aml_symbol(aml_symbol->component.expression_opcode.def_shift_right);
		if(aml_symbol->component.expression_opcode.def_size_of)delete_aml_symbol(aml_symbol->component.expression_opcode.def_size_of);
		if(aml_symbol->component.expression_opcode.def_store)delete_aml_symbol(aml_symbol->component.expression_opcode.def_store);
		if(aml_symbol->component.expression_opcode.def_subtract)delete_aml_symbol(aml_symbol->component.expression_opcode.def_subtract);
		if(aml_symbol->component.expression_opcode.def_timer)delete_aml_symbol(aml_symbol->component.expression_opcode.def_timer);
		if(aml_symbol->component.expression_opcode.def_to_bcd)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_bcd);
		if(aml_symbol->component.expression_opcode.def_to_buffer)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_buffer);
		if(aml_symbol->component.expression_opcode.def_to_decimal_string)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_decimal_string);
		if(aml_symbol->component.expression_opcode.def_to_hex_string)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_hex_string);
		if(aml_symbol->component.expression_opcode.def_to_integer)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_integer);
		if(aml_symbol->component.expression_opcode.def_to_string)delete_aml_symbol(aml_symbol->component.expression_opcode.def_to_string);
		if(aml_symbol->component.expression_opcode.def_var_package)delete_aml_symbol(aml_symbol->component.expression_opcode.def_var_package);
		if(aml_symbol->component.expression_opcode.def_wait)delete_aml_symbol(aml_symbol->component.expression_opcode.def_wait);
		if(aml_symbol->component.expression_opcode.def_xor)delete_aml_symbol(aml_symbol->component.expression_opcode.def_xor);
		if(aml_symbol->component.expression_opcode.method_invocation)delete_aml_symbol(aml_symbol->component.expression_opcode.method_invocation);
		break;
	case aml_ext_op_prefix:
		break;
	case aml_field_element:
		if(aml_symbol->component.field_element.named_field)delete_aml_symbol(aml_symbol->component.field_element.named_field);
		if(aml_symbol->component.field_element.reserved_field)delete_aml_symbol(aml_symbol->component.field_element.reserved_field);
		if(aml_symbol->component.field_element.access_field)delete_aml_symbol(aml_symbol->component.field_element.access_field);
		if(aml_symbol->component.field_element.extended_access_field)delete_aml_symbol(aml_symbol->component.field_element.extended_access_field);
		if(aml_symbol->component.field_element.connect_field)delete_aml_symbol(aml_symbol->component.field_element.connect_field);
		break;
	case aml_field_flags:
		break;
	case aml_field_list:
		if(aml_symbol->component.field_list.field_element)delete_aml_symbol(aml_symbol->component.field_list.field_element);
		if(aml_symbol->component.field_list.field_list)delete_aml_symbol(aml_symbol->component.field_list.field_list);
		break;
	case aml_field_op:
		if(aml_symbol->component.field_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.field_op.ext_op_prefix);
		if(aml_symbol->component.field_op.field_op_suffix)delete_aml_symbol(aml_symbol->component.field_op.field_op_suffix);
		break;
	case aml_field_op_suffix:
		break;
	case aml_if_op:
		break;
	case aml_increment_op:
		break;
	case aml_index_op:
		break;
	case aml_index_value:
		if(aml_symbol->component.index_value.term_arg)delete_aml_symbol(aml_symbol->component.index_value.term_arg);
		break;
	case aml_lead_name_char:
		break;
	case aml_l_equal_op:
		break;
	case aml_l_greater_op:
		break;
	case aml_l_less_op:
		break;
	case aml_l_or_op:
		break;
	case aml_local_obj:
		if(aml_symbol->component.local_obj.local_op)delete_aml_symbol(aml_symbol->component.local_obj.local_op);
		break;
	case aml_local_op:
		break;
	case aml_method_flags:
		break;
	case aml_method_invocation:
		if(aml_symbol->component.method_invocation.name_string)delete_aml_symbol(aml_symbol->component.method_invocation.name_string);
		if(aml_symbol->component.method_invocation.term_arg_list)delete_aml_symbol(aml_symbol->component.method_invocation.term_arg_list);
		break;
	case aml_method_op:
		break;
	case aml_multi_name_path:
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])delete_aml_symbol(aml_symbol->component.multi_name_path.name_seg[i]);
		free(aml_symbol->component.multi_name_path.name_seg);
		if(aml_symbol->component.multi_name_path.multi_name_prefix)delete_aml_symbol(aml_symbol->component.multi_name_path.multi_name_prefix);
		if(aml_symbol->component.multi_name_path.seg_count)delete_aml_symbol(aml_symbol->component.multi_name_path.seg_count);
		free(aml_symbol->component.multi_name_path.string);
		break;
	case aml_multi_name_prefix:
		break;
	case aml_mutex_object:
		if(aml_symbol->component.mutex_object.super_name)delete_aml_symbol(aml_symbol->component.mutex_object.super_name);
		break;
	case aml_mutex_op:
		if(aml_symbol->component.mutex_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.mutex_op.ext_op_prefix);
		if(aml_symbol->component.mutex_op.mutex_op_suffix)delete_aml_symbol(aml_symbol->component.mutex_op.mutex_op_suffix);
		break;
	case aml_mutex_op_suffix:
		break;
	case aml_name_char:
		if(aml_symbol->component.name_char.digit_char)delete_aml_symbol(aml_symbol->component.name_char.digit_char);
		if(aml_symbol->component.name_char.lead_name_char)delete_aml_symbol(aml_symbol->component.name_char.lead_name_char);
		break;
	case aml_name_op:
		break;
	case aml_name_path:
		if(aml_symbol->component.name_path.name_seg)delete_aml_symbol(aml_symbol->component.name_path.name_seg);
		if(aml_symbol->component.name_path.dual_name_path)delete_aml_symbol(aml_symbol->component.name_path.dual_name_path);
		if(aml_symbol->component.name_path.multi_name_path)delete_aml_symbol(aml_symbol->component.name_path.multi_name_path);
		if(aml_symbol->component.name_path.null_name)delete_aml_symbol(aml_symbol->component.name_path.null_name);
		break;
	case aml_name_seg:
		if(aml_symbol->component.name_seg.lead_name_char)delete_aml_symbol(aml_symbol->component.name_seg.lead_name_char);
		for(AMLSymbol **name_char = aml_symbol->component.name_seg.name_char; name_char != aml_symbol->component.name_seg.name_char + _countof(aml_symbol->component.name_seg.name_char); name_char++)if(*name_char)delete_aml_symbol(*name_char);
		break;
	case aml_name_space_modifier_obj:
		if(aml_symbol->component.name_space_modifier_obj.def_alias)delete_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_alias);
		if(aml_symbol->component.name_space_modifier_obj.def_name)delete_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_name);
		if(aml_symbol->component.name_space_modifier_obj.def_scope)delete_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_scope);
		break;
	case aml_name_string:
		if(aml_symbol->component.name_string.name_path)delete_aml_symbol(aml_symbol->component.name_string.name_path);
		if(aml_symbol->component.name_string.prefix_path)delete_aml_symbol(aml_symbol->component.name_string.prefix_path);
		if(aml_symbol->component.name_string.root_char)delete_aml_symbol(aml_symbol->component.name_string.root_char);
		free(aml_symbol->component.name_string.string);
		break;
	case aml_named_field:
		if(aml_symbol->component.named_field.name_seg)delete_aml_symbol(aml_symbol->component.named_field.name_seg);
		if(aml_symbol->component.named_field.pkg_length)delete_aml_symbol(aml_symbol->component.named_field.pkg_length);
		break;
	case aml_named_obj:
		if(aml_symbol->component.named_obj.def_bank_field)delete_aml_symbol(aml_symbol->component.named_obj.def_bank_field);
		if(aml_symbol->component.named_obj.def_create_bit_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_bit_field);
		if(aml_symbol->component.named_obj.def_create_byte_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_byte_field);
		if(aml_symbol->component.named_obj.def_create_dword_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_dword_field);
		if(aml_symbol->component.named_obj.def_create_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_field);
		if(aml_symbol->component.named_obj.def_create_qword_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_qword_field);
		if(aml_symbol->component.named_obj.def_create_word_field)delete_aml_symbol(aml_symbol->component.named_obj.def_create_word_field);
		if(aml_symbol->component.named_obj.def_data_region)delete_aml_symbol(aml_symbol->component.named_obj.def_data_region);
		if(aml_symbol->component.named_obj.def_device)delete_aml_symbol(aml_symbol->component.named_obj.def_device);
		if(aml_symbol->component.named_obj.def_external)delete_aml_symbol(aml_symbol->component.named_obj.def_external);
		if(aml_symbol->component.named_obj.def_field)delete_aml_symbol(aml_symbol->component.named_obj.def_field);
		if(aml_symbol->component.named_obj.def_method)delete_aml_symbol(aml_symbol->component.named_obj.def_method);
		if(aml_symbol->component.named_obj.def_mutex)delete_aml_symbol(aml_symbol->component.named_obj.def_mutex);
		if(aml_symbol->component.named_obj.def_op_region)delete_aml_symbol(aml_symbol->component.named_obj.def_op_region);
		if(aml_symbol->component.named_obj.def_power_res)delete_aml_symbol(aml_symbol->component.named_obj.def_power_res);
		if(aml_symbol->component.named_obj.def_thermal_zone)delete_aml_symbol(aml_symbol->component.named_obj.def_thermal_zone);
		break;
	case aml_null_char:
		break;
	case aml_null_name:
		break;
	case aml_num_elements:
		if(aml_symbol->component.num_elements.byte_data)delete_aml_symbol(aml_symbol->component.num_elements.byte_data);
		break;
	case aml_obj_reference:
		if(aml_symbol->component.obj_reference.term_arg)delete_aml_symbol(aml_symbol->component.obj_reference.term_arg);
		break;
	case aml_object:
		if(aml_symbol->component.object.named_obj)delete_aml_symbol(aml_symbol->component.object.named_obj);
		if(aml_symbol->component.object.name_space_modifier_obj)delete_aml_symbol(aml_symbol->component.object.name_space_modifier_obj);
		break;
	case aml_one_op:
		break;
	case aml_ones_op:
		break;
	case aml_op_region_op:
		if(aml_symbol->component.op_region_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.op_region_op.ext_op_prefix);
		if(aml_symbol->component.op_region_op.op_region_op_suffix)delete_aml_symbol(aml_symbol->component.op_region_op.op_region_op_suffix);
		break;
	case aml_op_region_op_suffix:
		break;
	case aml_operand:
		if(aml_symbol->component.operand.term_arg)delete_aml_symbol(aml_symbol->component.operand.term_arg);
		break;
	case aml_package_element:
		if(aml_symbol->component.package_element.data_ref_object)delete_aml_symbol(aml_symbol->component.package_element.data_ref_object);
		if(aml_symbol->component.package_element.name_string)delete_aml_symbol(aml_symbol->component.package_element.name_string);
		break;
	case aml_package_element_list:
		if(aml_symbol->component.package_element_list.package_element)delete_aml_symbol(aml_symbol->component.package_element_list.package_element);
		if(aml_symbol->component.package_element_list.package_element_list)delete_aml_symbol(aml_symbol->component.package_element_list.package_element_list);
		break;
	case aml_package_op:
		break;
	case aml_parent_prefix_char:
		break;
	case aml_pkg_lead_byte:
		break;
	case aml_pkg_length:
		if(aml_symbol->component.pkg_length.pkg_lead_byte)delete_aml_symbol(aml_symbol->component.pkg_length.pkg_lead_byte);
		for(AMLSymbol **byte_data = aml_symbol->component.pkg_length.byte_data; byte_data != aml_symbol->component.pkg_length.byte_data + _countof(aml_symbol->component.pkg_length.byte_data); byte_data++)if(*byte_data)delete_aml_symbol(*byte_data);
		break;
	case aml_predicate:
		if(aml_symbol->component.predicate.term_arg)delete_aml_symbol(aml_symbol->component.predicate.term_arg);
		break;
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)delete_aml_symbol(aml_symbol->component.prefix_path.parent_prefix_char);
		if(aml_symbol->component.prefix_path.prefix_path)delete_aml_symbol(aml_symbol->component.prefix_path.prefix_path);
		free(aml_symbol->component.prefix_path.string);
		break;
	case aml_qword_const:
		if(aml_symbol->component.qword_const.qword_prefix)delete_aml_symbol(aml_symbol->component.qword_const.qword_prefix);
		if(aml_symbol->component.qword_const.qword_data)delete_aml_symbol(aml_symbol->component.qword_const.qword_data);
		break;
	case aml_qword_data:
		for(AMLSymbol **dword_data = aml_symbol->component.qword_data.dword_data; dword_data != aml_symbol->component.qword_data.dword_data + _countof(aml_symbol->component.qword_data.dword_data); dword_data++)if(*dword_data)delete_aml_symbol(*dword_data);
		break;
	case aml_qword_prefix:
		break;
	case aml_region_len:
		if(aml_symbol->component.region_len.term_arg)delete_aml_symbol(aml_symbol->component.region_len.term_arg);
		break;
	case aml_region_offset:
		if(aml_symbol->component.region_offset.term_arg)delete_aml_symbol(aml_symbol->component.region_offset.term_arg);
		break;
	case aml_region_space:
		break;
	case aml_release_op:
		if(aml_symbol->component.release_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.release_op.ext_op_prefix);
		if(aml_symbol->component.release_op.release_op_suffix)delete_aml_symbol(aml_symbol->component.release_op.release_op_suffix);
		break;
	case aml_release_op_suffix:
		break;
	case aml_return_op:
		break;
	case aml_revision_op:
		if(aml_symbol->component.revision_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.revision_op.ext_op_prefix);
		if(aml_symbol->component.revision_op.revision_op_suffix)delete_aml_symbol(aml_symbol->component.revision_op.revision_op_suffix);
		break;
	case aml_root_char:
		break;
	case aml_scope_op:
		break;
	case aml_seg_count:
		break;
	case aml_shift_count:
		if(aml_symbol->component.shift_count.term_arg)delete_aml_symbol(aml_symbol->component.shift_count.term_arg);
		break;
	case aml_shift_left_op:
		break;
	case aml_shift_right_op:
		break;
	case aml_simple_name:
		if(aml_symbol->component.simple_name.name_string)delete_aml_symbol(aml_symbol->component.simple_name.name_string);
		if(aml_symbol->component.simple_name.arg_obj)delete_aml_symbol(aml_symbol->component.simple_name.arg_obj);
		if(aml_symbol->component.simple_name.local_obj)delete_aml_symbol(aml_symbol->component.simple_name.local_obj);
		break;
	case aml_size_of_op:
		break;
	case aml_statement_opcode:
		if(aml_symbol->component.statement_opcode.def_break)delete_aml_symbol(aml_symbol->component.statement_opcode.def_break);
		if(aml_symbol->component.statement_opcode.def_break_point)delete_aml_symbol(aml_symbol->component.statement_opcode.def_break_point);
		if(aml_symbol->component.statement_opcode.def_continue)delete_aml_symbol(aml_symbol->component.statement_opcode.def_continue);
		if(aml_symbol->component.statement_opcode.def_fatal)delete_aml_symbol(aml_symbol->component.statement_opcode.def_fatal);
		if(aml_symbol->component.statement_opcode.def_if_else)delete_aml_symbol(aml_symbol->component.statement_opcode.def_if_else);
		if(aml_symbol->component.statement_opcode.def_noop)delete_aml_symbol(aml_symbol->component.statement_opcode.def_noop);
		if(aml_symbol->component.statement_opcode.def_notify)delete_aml_symbol(aml_symbol->component.statement_opcode.def_notify);
		if(aml_symbol->component.statement_opcode.def_release)delete_aml_symbol(aml_symbol->component.statement_opcode.def_release);
		if(aml_symbol->component.statement_opcode.def_reset)delete_aml_symbol(aml_symbol->component.statement_opcode.def_reset);
		if(aml_symbol->component.statement_opcode.def_return)delete_aml_symbol(aml_symbol->component.statement_opcode.def_return);
		if(aml_symbol->component.statement_opcode.def_signal)delete_aml_symbol(aml_symbol->component.statement_opcode.def_signal);
		if(aml_symbol->component.statement_opcode.def_sleep)delete_aml_symbol(aml_symbol->component.statement_opcode.def_sleep);
		if(aml_symbol->component.statement_opcode.def_stall)delete_aml_symbol(aml_symbol->component.statement_opcode.def_stall);
		if(aml_symbol->component.statement_opcode.def_while)delete_aml_symbol(aml_symbol->component.statement_opcode.def_while);
		break;
	case aml_string:
		if(aml_symbol->component.string.string_prefix)delete_aml_symbol(aml_symbol->component.string.string_prefix);
		if(aml_symbol->component.string.ascii_char_list)delete_aml_symbol(aml_symbol->component.string.ascii_char_list);
		if(aml_symbol->component.string.null_char)delete_aml_symbol(aml_symbol->component.string.null_char);
		break;
	case aml_string_prefix:
		break;
	case aml_store_op:
		break;
	case aml_subtract_op:
		break;
	case aml_super_name:
		if(aml_symbol->component.super_name.debug_obj)delete_aml_symbol(aml_symbol->component.super_name.debug_obj);
		if(aml_symbol->component.super_name.reference_type_opcode)delete_aml_symbol(aml_symbol->component.super_name.reference_type_opcode);
		if(aml_symbol->component.super_name.simple_name)delete_aml_symbol(aml_symbol->component.super_name.simple_name);
		break;
	case aml_sync_flags:
		break;
	case aml_target:
		if(aml_symbol->component.target.null_name)delete_aml_symbol(aml_symbol->component.target.null_name);
		if(aml_symbol->component.target.super_name)delete_aml_symbol(aml_symbol->component.target.super_name);
		break;
	case aml_term_arg:
		if(aml_symbol->component.term_arg.expression_opcode)delete_aml_symbol(aml_symbol->component.term_arg.expression_opcode);
		if(aml_symbol->component.term_arg.data_object)delete_aml_symbol(aml_symbol->component.term_arg.data_object);
		if(aml_symbol->component.term_arg.arg_obj)delete_aml_symbol(aml_symbol->component.term_arg.arg_obj);
		if(aml_symbol->component.term_arg.local_obj)delete_aml_symbol(aml_symbol->component.term_arg.local_obj);
		break;
	case aml_term_arg_list:
		if(aml_symbol->component.term_arg_list.term_arg)delete_aml_symbol(aml_symbol->component.term_arg_list.term_arg);
		if(aml_symbol->component.term_arg_list.term_arg_list)delete_aml_symbol(aml_symbol->component.term_arg_list.term_arg_list);
		break;
	case aml_term_list:
		if(aml_symbol->component.term_list.term_obj)delete_aml_symbol(aml_symbol->component.term_list.term_obj);
		if(aml_symbol->component.term_list.term_list)delete_aml_symbol(aml_symbol->component.term_list.term_list);
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object)delete_aml_symbol(aml_symbol->component.term_obj.object);
		if(aml_symbol->component.term_obj.statement_opcode)delete_aml_symbol(aml_symbol->component.term_obj.statement_opcode);
		if(aml_symbol->component.term_obj.expression_opcode)delete_aml_symbol(aml_symbol->component.term_obj.expression_opcode);
		break;
	case aml_time_out:
		if(aml_symbol->component.time_out.word_data)delete_aml_symbol(aml_symbol->component.time_out.word_data);
		break;
	case aml_to_buffer_op:
		break;
	case aml_to_hex_string_op:
		break;
	case aml_while_op:
		break;
	case aml_word_const:
		if(aml_symbol->component.word_const.word_prefix)delete_aml_symbol(aml_symbol->component.word_const.word_prefix);
		if(aml_symbol->component.word_const.word_data)delete_aml_symbol(aml_symbol->component.word_const.word_data);
		break;
	case aml_word_data:
		for(AMLSymbol **byte_data = aml_symbol->component.word_data.byte_data; byte_data != aml_symbol->component.word_data.byte_data + _countof(aml_symbol->component.word_data.byte_data); byte_data++)if(*byte_data)delete_aml_symbol(*byte_data);
		break;
	case aml_word_prefix:
		break;
	case aml_zero_op:
		break;
	default:
		ERROR(); // Invalid AML symbol type
		break;
	}
	free(aml_symbol);
}

MemoryRegionDescriptor get_acpi_memory_region_descriptor(void)
{
	MemoryRegionDescriptor acpi_memory_region_descriptor;
	unsigned int acpi_memory_region_descriptor_index;
	for(acpi_memory_region_descriptor = get_memory_region_descriptor(acpi_memory_region_descriptor_index = 0); acpi_memory_region_descriptor.base != 0 || acpi_memory_region_descriptor.length != 0 || acpi_memory_region_descriptor.type != 0 || acpi_memory_region_descriptor.attribute != 0; acpi_memory_region_descriptor = get_memory_region_descriptor(++acpi_memory_region_descriptor_index))if(acpi_memory_region_descriptor.type == MEMORY_REGION_ACPI)return acpi_memory_region_descriptor;
	// ACPI is not found.
	acpi_memory_region_descriptor.base = 0;
	acpi_memory_region_descriptor.length = 0;
	acpi_memory_region_descriptor.type = 0;
	acpi_memory_region_descriptor.attribute = 0;
	return acpi_memory_region_descriptor;
}

AMLSymbol const *get_aml_method(char const *method_name, AMLSymbol const *aml_symbol)
{
	AMLSymbol const *def_method = NULL;
	AMLSymbol const *def_method_in_def_device = NULL;
	AMLSymbol const *def_method_in_def_if_else = NULL;
	AMLSymbol const *def_method_in_def_while = NULL;
	AMLSymbol const *def_method_in_term_list = NULL;
	AMLSymbol const *def_method_in_term_obj = NULL;
	AMLSymbol const *def_method_in_object = NULL;
	AMLSymbol const *def_method_in_statement_opcode = NULL;
	if(!method_name)return NULL;
	if(!aml_symbol)return NULL;
	switch(aml_symbol->type)
	{
	case aml_def_device:
		def_method = get_aml_method(method_name, aml_symbol->component.def_device.term_list);
		break;
	case aml_def_if_else:
		def_method = get_aml_method(method_name, aml_symbol->component.def_if_else.term_list);
		break;
	case aml_def_method:
		if(!strcmp(method_name, aml_symbol->component.def_method.name_string->component.name_string.string))return aml_symbol;
		break;
	case aml_def_scope:
		def_method = get_aml_method(method_name, aml_symbol->component.def_scope.term_list);
		break;
	case aml_def_while:
		def_method = get_aml_method(method_name, aml_symbol->component.def_while.term_list);
		break;
	case aml_name_space_modifier_obj:
		def_method = get_aml_method(method_name, aml_symbol->component.name_space_modifier_obj.def_scope);
		break;
	case aml_named_obj:
		def_method = get_aml_method(method_name, aml_symbol->component.named_obj.def_method);
		def_method_in_def_device = get_aml_method(method_name, aml_symbol->component.named_obj.def_device);
		break;
	case aml_object:
		def_method = get_aml_method(method_name, aml_symbol->component.object.named_obj);
		break;
	case aml_statement_opcode:
		def_method_in_def_if_else = get_aml_method(method_name, aml_symbol->component.statement_opcode.def_if_else);
		def_method_in_def_while = get_aml_method(method_name, aml_symbol->component.statement_opcode.def_while);
		break;
	case aml_term_list:
		def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.term_list.term_list);
		def_method_in_term_obj = get_aml_method(method_name, aml_symbol->component.term_list.term_obj);
		break;
	case aml_term_obj:
		def_method_in_object = get_aml_method(method_name, aml_symbol->component.term_obj.object);
		def_method_in_statement_opcode = get_aml_method(method_name, aml_symbol->component.term_obj.statement_opcode);
		break;
	default:
		break;
	}
	if(def_method)return def_method;
	if(def_method_in_def_device)return def_method_in_def_device;
	if(def_method_in_def_if_else)return def_method_in_def_if_else;
	if(def_method_in_def_while)return def_method_in_def_while;
	if(def_method_in_term_list)return def_method_in_term_list;
	if(def_method_in_term_obj)return def_method_in_term_obj;
	if(def_method_in_object)return def_method_in_object;
	if(def_method_in_statement_opcode)return def_method_in_statement_opcode;
	return get_aml_method(method_name, aml_symbol->parent);
}

AMLSubstring get_dsdt_aml(void)
{
	AMLSubstring dsdt_aml;
	ACPITableHeader const *dsdt_header = get_dsdt_header();
	if(dsdt_header)
	{
		dsdt_aml.initial = (unsigned char const *)((unsigned int)dsdt_header + sizeof(*dsdt_header));
		dsdt_aml.length = dsdt_header->length - sizeof(*dsdt_header);
		return dsdt_aml;
	}
	else
	{
		ERROR(); // DSDT is not found.
		dsdt_aml.initial = NULL;
		dsdt_aml.length = 0;
		return dsdt_aml;
	}
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
	if(rsdt_header)return (rsdt_header->length - sizeof(*rsdt_header)) / sizeof(ACPITableHeader*);
	else
	{
		ERROR(); // RSDT is not found.
		return 0;
	}
}

RSDP const *get_rsdp(void)
{
	void const *extended_bios_data_area = get_extended_bios_data_area();
	for(char const *rsdp_candidate = extended_bios_data_area; (unsigned int)rsdp_candidate < (unsigned int)extended_bios_data_area + 0x00000400; rsdp_candidate += 0x10)if(rsdp_is_correct((RSDP const *)rsdp_candidate))return (RSDP const *)rsdp_candidate;
	for(char const *rsdp_candidate = (char const *)0x000e0000; (unsigned int)rsdp_candidate < 0x00100000; rsdp_candidate += 0x10)if(rsdp_is_correct((RSDP const *)rsdp_candidate))return (RSDP const *)rsdp_candidate;
	ERROR(); // RSDP is not found.
	return NULL;
}

ACPITableHeader const *get_rsdt_header(void)
{
	ACPITableHeader const *rsdt_header = (ACPITableHeader const *)(unsigned int)get_acpi_memory_region_descriptor().base;
	if(!rsdt_header)
	{
		RSDP const *rsdp = get_rsdp();
		if(!rsdp)
		{
			ERROR(); // RSDT is not found!
			return NULL;
		}
		rsdt_header = rsdp->rsdt;
	}
	if(acpi_table_is_correct(rsdt_header))return rsdt_header;
	else
	{
		ERROR(); // RSDT is incorrect!
		return NULL;
	}
}

AMLSymbol const *get_root_aml_symbol(AMLSymbol const *aml_symbol)
{
	return aml_symbol->parent ? get_root_aml_symbol(aml_symbol->parent) : aml_symbol;
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
	if(rsdt_header)return (ACPITableHeader const * const *)((unsigned int)rsdt_header + sizeof(*rsdt_header));
	else
	{
		ERROR(); // RSDT is not found.
		return NULL;
	}
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
	printf_shell(shell, "%s = %p\n", name, acpi_table_header);
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

void print_aml_symbol(AMLSymbol const *aml_symbol)
{
	char *aml_symbol_string = aml_symbol_to_string(aml_symbol);
	printf_shell(get_current_shell(), aml_symbol_string);
	free(aml_symbol_string);
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

void print_rsdp(RSDP const *rsdp, char const *name)
{
	Shell *shell = get_current_shell();
	printf_shell(shell, "%s->signature = %.*s\n", name, sizeof(rsdp->signature), rsdp->signature);
	printf_shell(shell, "%s->checksum = %#04.2x\n", name, rsdp->checksum);
	printf_shell(shell, "%s->oemid = %.*s\n", name, sizeof(rsdp->oemid), rsdp->oemid);
	printf_shell(shell, "%s->revision = %#04.2x\n", name, rsdp->revision);
	printf_shell(shell, "%s->rsdt = %p\n", name, rsdp->rsdt);
	printf_shell(shell, "%s->length = %#010.8x\n", name, rsdp->length);
	printf_shell(shell, "%s->xsdt_addr = %#018.16x\n", name, rsdp->xsdt_addr);
	printf_shell(shell, "%s->extended_checksum = %#04.2x\n", name, rsdp->extended_checksum);
}

void print_sdts(void)
{
	Shell *shell = get_current_shell();
	ACPITableHeader const * const *sdt_headers = get_sdt_headers();
	unsigned int num_of_sdt_headers = get_num_of_sdt_headers();
	for(ACPITableHeader const * const *sdt_header = sdt_headers; sdt_header != sdt_headers + num_of_sdt_headers; sdt_header++)
	{
		PRINT_ACPI_TABLE_HEADER_P(*sdt_header);
		for(unsigned int i = 0; i < (*sdt_header)->length; i++)printf_shell(shell, "%02.2x%c", *((unsigned char *)(*sdt_header) + sizeof(**sdt_header) + i), (i + 1) % 0x10 ? ' ' : '\n');
		printf_shell(shell, "\n");
	}
}

char const *region_space_type_name(unsigned char region_space_byte_data)
{
	switch(region_space_byte_data)
	{
	case AML_REGION_SPACE_SYSTEM_MEMORY:
		return "SystemMerory";
	case AML_REGION_SPACE_SYSTEM_IO:
		return "SystemIO";
	case AML_REGION_SPACE_PCI_CONFIG:
		return "PCI_Config";
	case AML_REGION_SPACE_EMBEDDED_CONTROL:
		return "EmbeddedControl";
	case AML_REGION_SPACE_SM_BUS:
		return "SMBus";
	case AML_REGION_SPACE_SYSTEM_CMOS:
		return "SystemCMOS";
	case AML_REGION_SPACE_PCI_BAR_TARGET:
		return "PciBarTarget";
	case AML_REGION_SPACE_IPMI:
		return "IPMI";
	case AML_REGION_SPACE_GENERAL_PURPOSE_IO:
		return "GeneralPurposeIO";
	case AML_REGION_SPACE_GENERIC_SERIAL_BUS:
		return "GenericSerialBus";
	case AML_REGION_SPACE_PCC:
		return "PCC";
	default:
		if(0x80 <= region_space_byte_data)return "OEM Defined";
		else
		{
			ERROR(); // Incorrect region_space_byte_data
			return "ERROR";
		}
	}
}

bool rsdp_is_correct(RSDP const *rsdp)
{
	static char const * const rsdp_signature = "RSD PTR ";
	 unsigned char sum = 0;
	if(strncmp(rsdp->signature, rsdp_signature, sizeof(rsdp->signature)))return false;
	for(unsigned char const *byte = (unsigned char const *)rsdp; byte != (unsigned char const *)&rsdp->length; byte++)sum += *byte;
	if(sum)return false;
	return true;
}

