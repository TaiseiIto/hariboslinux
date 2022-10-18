#include "acpi.h"
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
#define AML_BYTE_DWORD_PREFIX		0xc
#define AML_BYTE_STRING_PREFIX		0xd
#define AML_BYTE_QWORD_PREFIX		0xe
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
#define AML_BYTE_LAND_OP		0x90
#define AML_BYTE_LOR_OP			0x91
#define AML_BYTE_LNOT_OP		0x92
#define AML_BYTE_LNOT_EQUAL_OP		0x93
#define AML_BYTE_LLESS_EQUAL_OP		0x94
#define AML_BYTE_LGREATER_EQUAL_OP	0x95
#define AML_BYTE_LEQUAL_OP		0x93
#define AML_BYTE_LGREATER_OP		0x94
#define AML_BYTE_LLESS_OP		0x95
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

ChainString *aml_symbol_to_chain_string(AMLSymbol const *aml_symbol)
{
	ChainString **bytes_data_chain_string;
	ChainString **dwords_data_chain_string;
	ChainString **name_chars_chain_string;
	ChainString **name_segs_chain_string;
	ChainString **name_strings_chain_string;
	ChainString **words_data_chain_string;
	ChainString *alias_op_chain_string;
	ChainString *arg_obj_chain_string;
	ChainString *ascii_char_chain_string;
	ChainString *ascii_char_list_chain_string;
	ChainString *buffer_op_chain_string;
	ChainString *buffer_size_chain_string;
	ChainString *byte_const_chain_string;
	ChainString *byte_data_chain_string;
	ChainString *byte_list_chain_string;
	ChainString *byte_prefix_chain_string;
	ChainString *computational_data_chain_string;
	ChainString *const_obj_chain_string;
	ChainString *data_object_chain_string;
	ChainString *data_ref_object_chain_string;
	ChainString *def_add_chain_string;
	ChainString *def_alias_chain_string;
	ChainString *def_and_chain_string;
	ChainString *def_aquire_chain_string;
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
	ChainString *def_divide_chain_string;
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
	ChainString *def_mid_chain_string;
	ChainString *def_mod_chain_string;
	ChainString *def_multiply_chain_string;
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
	ChainString *digit_char_chain_string;
	ChainString *dual_name_path_chain_string;
	ChainString *dual_name_prefix_chain_string;
	ChainString *dword_const_chain_string;
	ChainString *dword_data_chain_string;
	ChainString *dword_prefix_chain_string;
	ChainString *expression_opcode_chain_string;
	ChainString *ext_op_prefix_chain_string;
	ChainString *field_element_chain_string;
	ChainString *field_flags_chain_string;
	ChainString *field_list_chain_string;
	ChainString *field_op_chain_string;
	ChainString *field_op_prefix_chain_string;
	ChainString *lead_name_char_chain_string;
	ChainString *local_obj_chain_string;
	ChainString *method_invocation_chain_string;
	ChainString *multi_name_path_chain_string;
	ChainString *multi_name_prefix_chain_string;
	ChainString *name_op_chain_string;
	ChainString *name_path_chain_string;
	ChainString *name_seg_chain_string;
	ChainString *name_space_modifier_obj_chain_string;
	ChainString *name_string_chain_string;
	ChainString *named_obj_chain_string;
	ChainString *null_char_chain_string;
	ChainString *null_name_chain_string;
	ChainString *object_chain_string;
	ChainString *object_reference_chain_string;
	ChainString *one_op_chain_string;
	ChainString *ones_op_chain_string;
	ChainString *op_region_op_chain_string;
	ChainString *op_region_op_prefix_chain_string;
	ChainString *output;
	ChainString *parent_prefix_char_chain_string;
	ChainString *pkg_lead_byte_chain_string;
	ChainString *pkg_length_chain_string;
	ChainString *prefix_path_chain_string;
	ChainString *qword_const_chain_string;
	ChainString *qword_data_chain_string;
	ChainString *qword_prefix_chain_string;
	ChainString *region_len_chain_string;
	ChainString *region_offset_chain_string;
	ChainString *region_space_chain_string;
	ChainString *revision_op_chain_string;
	ChainString *revision_op_prefix_chain_string;
	ChainString *root_char_chain_string;
	ChainString *seg_count_chain_string;
	ChainString *statement_opcode_chain_string;
	ChainString *string_chain_string;
	ChainString *string_prefix_chain_string;
	ChainString *term_arg_chain_string;
	ChainString *term_list_chain_string;
	ChainString *term_obj_chain_string;
	ChainString *word_const_chain_string;
	ChainString *word_data_chain_string;
	ChainString *word_prefix_chain_string;
	ChainString *zero_op_chain_string;
	char **bytes_data_char_array;
	char **dwords_data_char_array;
	char **name_chars_char_array;
	char **name_segs_char_array;
	char **name_strings_char_array;
	char **words_data_char_array;
	char *alias_op_char_array;
	char *arg_obj_char_array;
	char *ascii_char_char_array;
	char *ascii_char_list_char_array;
	char *buffer_op_char_array;
	char *buffer_size_char_array;
	char *byte_const_char_array;
	char *byte_data_char_array;
	char *byte_list_char_array;
	char *byte_prefix_char_array;
	char *computational_data_char_array;
	char *const_obj_char_array;
	char *data_object_char_array;
	char *data_ref_object_char_array;
	char *def_add_char_array;
	char *def_alias_char_array;
	char *def_and_char_array;
	char *def_aquire_char_array;
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
	char *def_divide_char_array;
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
	char *def_mid_char_array;
	char *def_mod_char_array;
	char *def_multiply_char_array;
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
	char *digit_char_char_array;
	char *dual_name_path_char_array;
	char *dual_name_prefix_char_array;
	char *dword_const_char_array;
	char *dword_data_char_array;
	char *dword_prefix_char_array;
	char *expression_opcode_char_array;
	char *ext_op_prefix_char_array;
	char *field_element_char_array;
	char *field_flags_char_array;
	char *field_list_char_array;
	char *field_op_char_array;
	char *field_op_prefix_char_array;
	char *lead_name_char_char_array;
	char *local_obj_char_array;
	char *method_invocation_char_array;
	char *multi_name_path_char_array;
	char *multi_name_prefix_char_array;
	char *name_op_char_array;
	char *name_path_char_array;
	char *name_seg_char_array;
	char *name_space_modifier_obj_char_array;
	char *name_string_char_array;
	char *named_obj_char_array;
	char *null_char_char_array;
	char *null_name_char_array;
	char *object_char_array;
	char *object_reference_char_array;
	char *one_op_char_array;
	char *ones_op_char_array;
	char *op_region_op_char_array;
	char *op_region_op_prefix_char_array;
	char *parent_prefix_char_char_array;
	char *pkg_lead_byte_char_array;
	char *pkg_length_char_array;
	char *prefix_path_char_array;
	char *qword_const_char_array;
	char *qword_data_char_array;
	char *qword_prefix_char_array;
	char *region_len_char_array;
	char *region_offset_char_array;
	char *region_space_char_array;
	char *revision_op_char_array;
	char *revision_op_prefix_char_array;
	char *root_char_char_array;
	char *seg_count_char_array;
	char *statement_opcode_char_array;
	char *string_char_array;
	char *string_prefix_char_array;
	char *term_arg_char_array;
	char *term_list_char_array;
	char *term_obj_char_array;
	char *word_const_char_array;
	char *word_data_char_array;
	char *word_prefix_char_array;
	char *zero_op_char_array;
	switch(aml_symbol->type)
	{
	case aml_alias_op:
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), byte_prefix_char_array, byte_data_char_array);
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
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
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
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), buffer_op_char_array, pkg_length_char_array, buffer_size_char_array, byte_list_char_array);
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
		output = create_format_chain_string("%s\n%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), field_op_char_array, pkg_length_char_array, name_string_char_array, field_flags_char_array, field_list_char_array);
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
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), dual_name_prefix_char_array);
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), dword_prefix_char_array, dword_data_char_array);
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
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
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
		if(aml_symbol->component.expression_opcode.def_aquire)
		{
			def_aquire_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_aquire);
			insert_char_front(def_aquire_chain_string, def_aquire_chain_string->first_character, ' ');
			replace_chain_string(def_aquire_chain_string, "\n", "\n ");
			def_aquire_char_array = create_char_array_from_chain_string(def_aquire_chain_string);
		}
		else def_aquire_char_array = "";
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
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), def_add_char_array, def_and_char_array, def_aquire_char_array, def_buffer_char_array, def_concat_char_array, def_concat_res_char_array, def_cond_ref_of_char_array, def_copy_object_char_array, def_decrement_char_array, def_deref_of_char_array, def_divide_char_array, def_find_set_left_bit_char_array, def_find_set_right_bit_char_array, def_from_bcd_char_array, def_increment_char_array, def_index_char_array, def_l_and_char_array, def_l_equal_char_array, def_l_greater_char_array, def_l_greater_equal_char_array, def_l_less_char_array, def_l_less_equal_char_array, def_l_not_char_array, def_l_not_equal_char_array, def_l_or_char_array, def_load_table_char_array, def_match_char_array, def_mid_char_array, def_mod_char_array, def_multiply_char_array, def_nand_char_array, def_nor_char_array, def_not_char_array, def_object_type_char_array, def_or_char_array, def_package_char_array, def_ref_of_char_array, def_shift_left_char_array, def_shift_right_char_array, def_size_of_char_array, def_store_char_array, def_subtract_char_array, def_timer_char_array, def_to_bcd_char_array, def_to_buffer_char_array, def_to_decimal_string_char_array, def_to_hex_string_char_array, def_to_integer_char_array, def_to_string_char_array, def_var_package_char_array, def_wait_char_array, def_xor_char_array, method_invocation_char_array);
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
		if(aml_symbol->component.expression_opcode.def_aquire)
		{
			delete_chain_string(def_aquire_chain_string);
			free(def_aquire_char_array);
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
			delete_chain_string(field_list_chain_string);
			free(field_list_char_array);
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
		if(aml_symbol->component.field_op.field_op_prefix)
		{
			field_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.field_op.field_op_prefix);
			insert_char_front(field_op_prefix_chain_string, field_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(field_op_prefix_chain_string, "\n", "\n ");
			field_op_prefix_char_array = create_char_array_from_chain_string(field_op_prefix_chain_string);
		}
		else field_op_prefix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, field_op_prefix_char_array);
		if(aml_symbol->component.field_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.field_op.field_op_prefix)
		{
			delete_chain_string(field_op_prefix_chain_string);
			free(field_op_prefix_char_array);
		}
		break;
	case aml_field_op_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_lead_name_char:
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), digit_char_char_array, lead_name_char_char_array);
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
		output = create_format_chain_string("%s\n%s%s%s%s", aml_symbol_type_name(aml_symbol->type), name_seg_char_array, dual_name_path_char_array, multi_name_path_char_array, null_name_char_array);
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
		output = create_format_chain_string("%s\n%s", aml_symbol_type_name(aml_symbol->type), lead_name_char_char_array);
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
		output = create_format_chain_string("%s \"%.*s\"\n%s%s%s", aml_symbol_type_name(aml_symbol->type), aml_symbol->string.length, aml_symbol->string.initial, root_char_char_array, prefix_path_char_array, name_path_char_array);
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
		output = create_format_chain_string("%s\n%s%s%s%s%s%s%s%s%s%s%s%s%s", aml_symbol_type_name(aml_symbol->type), def_bank_field_char_array ,def_create_bit_field_char_array ,def_create_byte_field_char_array ,def_create_dword_field_char_array ,def_create_field_char_array ,def_create_qword_field_char_array ,def_create_word_field_char_array ,def_data_region_char_array ,def_external_char_array , def_field_char_array,def_op_region_char_array ,def_power_res_char_array ,def_thermal_zone_char_array);
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
		if(aml_symbol->component.op_region_op.op_region_op_prefix)
		{
			op_region_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.op_region_op.op_region_op_prefix);
			insert_char_front(op_region_op_prefix_chain_string, op_region_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(op_region_op_prefix_chain_string, "\n", "\n ");
			op_region_op_prefix_char_array = create_char_array_from_chain_string(op_region_op_prefix_chain_string);
		}
		else op_region_op_prefix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, op_region_op_prefix_char_array);
		if(aml_symbol->component.op_region_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.op_region_op.op_region_op_prefix)
		{
			delete_chain_string(op_region_op_prefix_chain_string);
			free(op_region_op_prefix_char_array);
		}
		break;
	case aml_op_region_op_prefix:
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), parent_prefix_char_char_array, prefix_path_char_array);
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), qword_prefix_char_array, qword_data_char_array);
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
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
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
		if(aml_symbol->component.revision_op.revision_op_prefix)
		{
			revision_op_prefix_chain_string = aml_symbol_to_chain_string(aml_symbol->component.revision_op.revision_op_prefix);
			insert_char_front(revision_op_prefix_chain_string, revision_op_prefix_chain_string->first_character, ' ');
			replace_chain_string(revision_op_prefix_chain_string, "\n", "\n ");
			revision_op_prefix_char_array = create_char_array_from_chain_string(revision_op_prefix_chain_string);
		}
		else revision_op_prefix_char_array = "";
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), ext_op_prefix_char_array, revision_op_prefix_char_array);
		if(aml_symbol->component.revision_op.ext_op_prefix)
		{
			delete_chain_string(ext_op_prefix_chain_string);
			free(ext_op_prefix_char_array);
		}
		if(aml_symbol->component.revision_op.revision_op_prefix)
		{
			delete_chain_string(revision_op_prefix_chain_string);
			free(revision_op_prefix_char_array);
		}
		break;
	case aml_revision_op_prefix:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_root_char:
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_seg_count:
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), term_obj_char_array, term_list_char_array);
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
		output = create_format_chain_string("%s\n%s%s", aml_symbol_type_name(aml_symbol->type), word_prefix_char_array, word_data_char_array);
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
		output = create_format_chain_string("%s\n", aml_symbol_type_name(aml_symbol->type));
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
	static char const * const aml_alias_op_name = "AliasOp";
	static char const * const aml_ascii_char_name = "AsciiChar";
	static char const * const aml_ascii_char_list_name = "AsciiCharList";
	static char const * const aml_buffer_op_name = "BufferOp";
	static char const * const aml_buffer_size_name = "BufferSize";
	static char const * const aml_byte_const_name = "ByteConst";
	static char const * const aml_byte_data_name = "ByteData";
	static char const * const aml_byte_prefix_name = "BytePrefix";
	static char const * const aml_computational_data_name = "ComputationalData";
	static char const * const aml_const_obj_name = "ConstObj";
	static char const * const aml_data_object_name = "DataObject";
	static char const * const aml_data_ref_object_name = "DataRefObject";
	static char const * const aml_def_alias_name = "DefAlias";
	static char const * const aml_def_buffer_name = "DefBuffer";
	static char const * const aml_def_field_name = "DefField";
	static char const * const aml_def_name_name = "DefName";
	static char const * const aml_def_op_region_name = "DefOpRegion";
	static char const * const aml_digit_char_name = "DigitChar";
	static char const * const aml_dual_name_path_name = "DualNamePath";
	static char const * const aml_dual_name_prefix_name = "DualNamePrefix";
	static char const * const aml_dword_const_name = "DWordConst";
	static char const * const aml_dword_data_name = "DWordData";
	static char const * const aml_dword_prefix_name = "DWordPrefix";
	static char const * const aml_expression_opcode_name = "ExpressionOpcode";
	static char const * const aml_ext_op_prefix_name = "ExtOpPrefix";
	static char const * const aml_field_flags_name = "FieldFlags";
	static char const * const aml_field_list_name = "FieldList";
	static char const * const aml_field_op_name = "FieldOp";
	static char const * const aml_field_op_prefix_name = "FieldOpPrefix";
	static char const * const aml_lead_name_char_name = "LeadNameChar";
	static char const * const aml_multi_name_path_name = "MultiNamePath";
	static char const * const aml_multi_name_prefix_name = "MultiNamePrefix";
	static char const * const aml_name_char_name = "NameChar";
	static char const * const aml_name_op_name = "NameOp";
	static char const * const aml_name_path_name = "NamePath";
	static char const * const aml_name_seg_name = "NameSeg";
	static char const * const aml_name_space_modifier_obj_name = "NameSpaceModifierObj";
	static char const * const aml_name_string_name = "NameString";
	static char const * const aml_named_obj_name = "NamedObj";
	static char const * const aml_null_char_name = "NullChar";
	static char const * const aml_null_name_name = "NullName";
	static char const * const aml_object_name = "Object";
	static char const * const aml_one_op_name = "OneOp";
	static char const * const aml_ones_op_name = "OnesOp";
	static char const * const aml_op_region_op_name = "OpRegionOp";
	static char const * const aml_op_region_op_prefix_name = "OpRegionOpPrefix";
	static char const * const aml_parent_prefix_char_name = "ParentPrefixChar";
	static char const * const aml_pkg_lead_byte_name = "PkgLeadByte";
	static char const * const aml_pkg_length_name = "PkgLength";
	static char const * const aml_prefix_path_name = "PrefixPath";
	static char const * const aml_qword_const_name = "QWordConst";
	static char const * const aml_qword_data_name = "QWordData";
	static char const * const aml_qword_prefix_name = "QWordPrefix";
	static char const * const aml_region_len_name = "RegionLen";
	static char const * const aml_region_offset_name = "RegionOffset";
	static char const * const aml_region_space_name = "RegionSpace";
	static char const * const aml_revision_op_name = "RevisionOp";
	static char const * const aml_revision_op_prefix_name = "RevisionOpPrefix";
	static char const * const aml_root_char_name = "RootChar";
	static char const * const aml_seg_count_name = "SegCount";
	static char const * const aml_statement_opcode_name = "StatementOpcode";
	static char const * const aml_string_name = "String";
	static char const * const aml_string_prefix_name = "StringPrefix";
	static char const * const aml_term_arg_name = "TermArg";
	static char const * const aml_term_list_name = "TermList";
	static char const * const aml_term_obj_name = "TermObj";
	static char const * const aml_word_const_name = "WordConst";
	static char const * const aml_word_data_name = "WordData";
	static char const * const aml_word_prefix_name = "WordPrefix";
	static char const * const aml_zero_op_name = "ZeroOp";
	switch(aml_symbol_type)
	{
	case aml_alias_op:
		return aml_alias_op_name;
	case aml_ascii_char:
		return aml_ascii_char_name;
	case aml_ascii_char_list:
		return aml_ascii_char_list_name;
	case aml_buffer_op:
		return aml_buffer_op_name;
	case aml_buffer_size:
		return aml_buffer_size_name;
	case aml_byte_const:
		return aml_byte_const_name;
	case aml_byte_data:
		return aml_byte_data_name;
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
	case aml_def_buffer:
		return aml_def_buffer_name;
	case aml_def_field:
		return aml_def_field_name;
	case aml_def_name:
		return aml_def_name_name;
	case aml_def_op_region:
		return aml_def_op_region_name;
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
	case aml_field_flags:
		return aml_field_flags_name;
	case aml_field_list:
		return aml_field_list_name;
	case aml_field_op:
		return aml_field_op_name;
	case aml_field_op_prefix:
		return aml_field_op_prefix_name;
	case aml_lead_name_char:
		return aml_lead_name_char_name;
	case aml_multi_name_path:
		return aml_multi_name_path_name;
	case aml_multi_name_prefix:
		return aml_multi_name_prefix_name;
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
	case aml_named_obj:
		return aml_named_obj_name;
	case aml_null_char:
		return aml_null_char_name;
	case aml_null_name:
		return aml_null_name_name;
	case aml_object:
		return aml_object_name;
	case aml_one_op:
		return aml_one_op_name;
	case aml_ones_op:
		return aml_ones_op_name;
	case aml_op_region_op:
		return aml_op_region_op_name;
	case aml_op_region_op_prefix:
		return aml_op_region_op_prefix_name;
	case aml_parent_prefix_char:
		return aml_parent_prefix_char_name;
	case aml_pkg_lead_byte:
		return aml_pkg_lead_byte_name;
	case aml_pkg_length:
		return aml_pkg_length_name;
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
	case aml_revision_op:
		return aml_revision_op_name;
	case aml_revision_op_prefix:
		return aml_revision_op_prefix_name;
	case aml_root_char:
		return aml_root_char_name;
	case aml_seg_count:
		return aml_seg_count_name;
	case aml_statement_opcode:
		return aml_statement_opcode_name;
	case aml_string:
		return aml_string_name;
	case aml_string_prefix:
		return aml_string_prefix_name;
	case aml_term_arg:
		return aml_term_arg_name;
	case aml_term_list:
		return aml_term_list_name;
	case aml_term_obj:
		return aml_term_obj_name;
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
		return NULL;
	}
}

// <alias_op> := AML_BYTE_ALIAS_OP
AMLSymbol *analyse_aml_alias_op(AMLSubstring aml)
{
	AMLSymbol *alias_op = malloc(sizeof(*alias_op));
	alias_op->string.initial = aml.initial;
	alias_op->string.length = 1;
	alias_op->type = aml_alias_op;
	if(*alias_op->string.initial != AML_BYTE_ALIAS_OP)ERROR(); // Incorrect alias_op
	return alias_op;
}

// <ascii_char> := 0x01 - 0x7f
AMLSymbol *analyse_aml_ascii_char(AMLSubstring aml)
{
	AMLSymbol *ascii_char = malloc(sizeof(*ascii_char));
	ascii_char->string.initial = aml.initial;
	ascii_char->string.length = 1;
	ascii_char->type = aml_ascii_char;
	if(!(0x01 <= *ascii_char->string.initial && *ascii_char->string.initial <= 0x7f))ERROR(); // Incorrect ascii char
	return ascii_char;
}

// <ascii_char_list> := Nothing | <ascii_char> <ascii_char_list>
AMLSymbol *analyse_aml_ascii_char_list(AMLSubstring aml)
{
	AMLSymbol *ascii_char_list = malloc(sizeof(*ascii_char_list));
	ascii_char_list->string.initial = aml.initial;
	ascii_char_list->string.length = 0;
	ascii_char_list->type = aml_ascii_char_list;
	if(0x01 <= *ascii_char_list->string.initial && *ascii_char_list->string.initial <= 0x7f)
	{
		ascii_char_list->component.ascii_char_list.ascii_char = analyse_aml_ascii_char(aml);
		ascii_char_list->string.length += ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		aml.initial += ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		aml.length -= ascii_char_list->component.ascii_char_list.ascii_char->string.length;
		ascii_char_list->component.ascii_char_list.ascii_char_list = analyse_aml_ascii_char_list(aml);
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

// <buffer_op> := AML_BYTE_BUFFER_OP
AMLSymbol *analyse_aml_buffer_op(AMLSubstring aml)
{
	AMLSymbol *buffer_op = malloc(sizeof(*buffer_op));
	buffer_op->string.initial = aml.initial;
	buffer_op->string.length = 1;
	buffer_op->type = aml_buffer_op;
	if(*buffer_op->string.initial != AML_BYTE_BUFFER_OP)ERROR(); // Incorrect buffer op
	return buffer_op;
}

// <buffer_size> := <term_arg>
AMLSymbol *analyse_aml_buffer_size(AMLSubstring aml)
{
	AMLSymbol *buffer_size = malloc(sizeof(*buffer_size));
	buffer_size->string.initial = aml.initial;
	buffer_size->string.length = 0;
	buffer_size->type = aml_buffer_size;
	buffer_size->component.buffer_size.term_arg = analyse_aml_term_arg(aml);
	buffer_size->string.length += buffer_size->component.buffer_size.term_arg->string.length;
	return buffer_size;
}

// <byte_const> := <byte_prefix> <byte_data>
AMLSymbol *analyse_aml_byte_const(AMLSubstring aml)
{
	AMLSymbol *byte_const = malloc(sizeof(*byte_const));
	byte_const->string.initial = aml.initial;
	byte_const->string.length = 0;
	byte_const->type = aml_byte_const;
	byte_const->component.byte_const.byte_prefix = analyse_aml_byte_prefix(aml);
	byte_const->string.length += byte_const->component.byte_const.byte_prefix->string.length;
	aml.initial += byte_const->component.byte_const.byte_prefix->string.length;
	aml.length -= byte_const->component.byte_const.byte_prefix->string.length;
	byte_const->component.byte_const.byte_data = analyse_aml_byte_data(aml);
	byte_const->string.length += byte_const->component.byte_const.byte_data->string.length;
	aml.initial += byte_const->component.byte_const.byte_data->string.length;
	aml.length -= byte_const->component.byte_const.byte_data->string.length;
	return byte_const;
}

// <byte_data> := 0x00 - 0xff
AMLSymbol *analyse_aml_byte_data(AMLSubstring aml)
{
	AMLSymbol *byte_data = malloc(sizeof(*byte_data));
	byte_data->string.initial = aml.initial;
	byte_data->string.length = 1;
	byte_data->type = aml_byte_data;
	return byte_data;
}

// <byte_prefix> := AML_BYTE_BYTE_PREFIX
AMLSymbol *analyse_aml_byte_prefix(AMLSubstring aml)
{
	AMLSymbol *byte_prefix = malloc(sizeof(*byte_prefix));
	byte_prefix->string.initial = aml.initial;
	byte_prefix->string.length = 1;
	byte_prefix->type = aml_byte_prefix;
	if(*byte_prefix->string.initial != AML_BYTE_BYTE_PREFIX)ERROR(); // Incorrect byte prefix
	return byte_prefix;
}

// <computational_data> := <byte_const> | <word_const> | <dword_const> | <qword_const> | <string> | <const_obj> | <revision_op> | <def_buffer>
AMLSymbol *analyse_aml_computational_data(AMLSubstring aml)
{
	AMLSymbol *computational_data = malloc(sizeof(*computational_data));
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
		computational_data->component.computational_data.def_buffer = analyse_aml_def_buffer(aml);
		computational_data->string.length += computational_data->component.computational_data.def_buffer->string.length;
		break;
	case AML_BYTE_BYTE_PREFIX:
		computational_data->component.computational_data.byte_const = analyse_aml_byte_const(aml);
		computational_data->string.length += computational_data->component.computational_data.byte_const->string.length;
		break;
	case AML_BYTE_DWORD_PREFIX:
		computational_data->component.computational_data.dword_const = analyse_aml_dword_const(aml);
		computational_data->string.length += computational_data->component.computational_data.dword_const->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		computational_data->component.computational_data.revision_op = analyse_aml_revision_op(aml);
		computational_data->string.length += computational_data->component.computational_data.revision_op->string.length;
		break;
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ZERO_OP:
		computational_data->component.computational_data.const_obj = analyse_aml_const_obj(aml);
		computational_data->string.length += computational_data->component.computational_data.const_obj->string.length;
		break;
	case AML_BYTE_QWORD_PREFIX:
		computational_data->component.computational_data.qword_const = analyse_aml_qword_const(aml);
		computational_data->string.length += computational_data->component.computational_data.qword_const->string.length;
		break;
	case AML_BYTE_STRING_PREFIX:
		computational_data->component.computational_data.string = analyse_aml_string(aml);
		computational_data->string.length += computational_data->component.computational_data.string->string.length;
		break;
	case AML_BYTE_WORD_PREFIX:
		computational_data->component.computational_data.word_const = analyse_aml_word_const(aml);
		computational_data->string.length += computational_data->component.computational_data.word_const->string.length;
		break;
	}
	return computational_data;
}

// <const_obj> := <zero_op> | <one_op> | <ones_op>
AMLSymbol *analyse_aml_const_obj(AMLSubstring aml)
{
	AMLSymbol *const_obj = malloc(sizeof(*const_obj));
	const_obj->string.initial = aml.initial;
	const_obj->string.length = 0;
	const_obj->type = aml_const_obj;
	const_obj->component.const_obj.zero_op = NULL;
	const_obj->component.const_obj.one_op = NULL;
	const_obj->component.const_obj.ones_op = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ONE_OP:
		const_obj->component.const_obj.one_op = analyse_aml_one_op(aml);
		const_obj->string.length += const_obj->component.const_obj.one_op->string.length;
		aml.initial += const_obj->component.const_obj.one_op->string.length;
		aml.length -= const_obj->component.const_obj.one_op->string.length;
		break;
	case AML_BYTE_ONES_OP:
		const_obj->component.const_obj.ones_op = analyse_aml_ones_op(aml);
		const_obj->string.length += const_obj->component.const_obj.ones_op->string.length;
		aml.initial += const_obj->component.const_obj.ones_op->string.length;
		aml.length -= const_obj->component.const_obj.ones_op->string.length;
		break;
	case AML_BYTE_ZERO_OP:
		const_obj->component.const_obj.zero_op = analyse_aml_zero_op(aml);
		const_obj->string.length += const_obj->component.const_obj.zero_op->string.length;
		aml.initial += const_obj->component.const_obj.zero_op->string.length;
		aml.length -= const_obj->component.const_obj.zero_op->string.length;
		break;
	}
	return const_obj;
}

// <data_object> := <computational_data> | <def_package> | <def_var_package>
AMLSymbol *analyse_aml_data_object(AMLSubstring aml)
{
	AMLSymbol *data_object = malloc(sizeof(*data_object));
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
		data_object->component.data_object.computational_data = analyse_aml_computational_data(aml);
		data_object->string.length += data_object->component.data_object.computational_data->string.length;
		break;
	}
	return data_object;
}

// <data_ref_object> := <data_object> | <object_reference>
AMLSymbol *analyse_aml_data_ref_object(AMLSubstring aml)
{
	AMLSymbol *data_ref_object = malloc(sizeof(*data_ref_object));
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
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		data_ref_object->component.data_ref_object.data_object = analyse_aml_data_object(aml);
		data_ref_object->string.length += data_ref_object->component.data_ref_object.data_object->string.length;
		break;
	}
	return data_ref_object;
}

// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSubstring aml)
{
	AMLSymbol *def_alias = malloc(sizeof(*def_alias));
	def_alias->string.initial = aml.initial;
	def_alias->string.length = 0;
	def_alias->type = aml_def_alias;
	def_alias->component.def_alias.alias_op = analyse_aml_alias_op(aml);
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
			break;
		}
		*name_string = analyse_aml_name_string(aml);
		def_alias->string.length += (*name_string)->string.length;
		aml.initial += (*name_string)->string.length;
		aml.length -= (*name_string)->string.length;
	}
	return def_alias;
}

// <def_buffer> := <buffer_op> <pkg_length> <buffer_size> <byte_list>
AMLSymbol *analyse_aml_def_buffer(AMLSubstring aml)
{
	AMLSymbol *def_buffer = malloc(sizeof(*def_buffer));
	def_buffer->string.initial = aml.initial;
	def_buffer->string.length = 0;
	def_buffer->type = aml_def_buffer;
	def_buffer->component.def_buffer.buffer_op = analyse_aml_buffer_op(aml);
	def_buffer->string.length += def_buffer->component.def_buffer.buffer_op->string.length;
	aml.initial += def_buffer->component.def_buffer.buffer_op->string.length;
	aml.length -= def_buffer->component.def_buffer.buffer_op->string.length;
	def_buffer->component.def_buffer.pkg_length = analyse_aml_pkg_length(aml);
	def_buffer->string.length += def_buffer->component.def_buffer.pkg_length->string.length;
	aml.initial += def_buffer->component.def_buffer.pkg_length->string.length;
	aml.length -= def_buffer->component.def_buffer.pkg_length->string.length;
	def_buffer->component.def_buffer.buffer_size = analyse_aml_buffer_size(aml);
	def_buffer->string.length += def_buffer->component.def_buffer.buffer_size->string.length;
	aml.initial += def_buffer->component.def_buffer.buffer_size->string.length;
	aml.length -= def_buffer->component.def_buffer.buffer_size->string.length;
	def_buffer->component.def_buffer.byte_list = NULL;
	return def_buffer;
}

// <def_field> := <field_op> <pkg_length> <name_string> <field_flags> <field_list>
AMLSymbol *analyse_aml_def_field(AMLSubstring aml)
{
	AMLSymbol *def_field = malloc(sizeof(*def_field));
	def_field->string.initial = aml.initial;
	def_field->string.length = 0;
	def_field->type = aml_def_field;
	def_field->component.def_field.field_op = analyse_aml_field_op(aml);
	def_field->string.length += def_field->component.def_field.field_op->string.length;
	aml.initial += def_field->component.def_field.field_op->string.length;
	aml.length -= def_field->component.def_field.field_op->string.length;
	def_field->component.def_field.pkg_length = analyse_aml_pkg_length(aml);
	def_field->string.length += def_field->component.def_field.pkg_length->string.length;
	aml.initial += def_field->component.def_field.pkg_length->string.length;
	aml.length -= def_field->component.def_field.pkg_length->string.length;
	def_field->component.def_field.name_string = analyse_aml_name_string(aml);
	def_field->string.length += def_field->component.def_field.name_string->string.length;
	aml.initial += def_field->component.def_field.name_string->string.length;
	aml.length -= def_field->component.def_field.name_string->string.length;
	def_field->component.def_field.field_flags = analyse_aml_field_flags(aml);
	def_field->string.length += def_field->component.def_field.field_flags->string.length;
	aml.initial += def_field->component.def_field.field_flags->string.length;
	aml.length -= def_field->component.def_field.field_flags->string.length;
	def_field->component.def_field.field_list = analyse_aml_field_list(aml);
	def_field->string.length += def_field->component.def_field.field_list->string.length;
	aml.initial += def_field->component.def_field.field_list->string.length;
	aml.length -= def_field->component.def_field.field_list->string.length;
	return def_field;
}

// <def_name> := <name_op> <name_string> <data_ref_object>
AMLSymbol *analyse_aml_def_name(AMLSubstring aml)
{
	AMLSymbol *def_name = malloc(sizeof(*def_name));
	def_name->string.initial = aml.initial;
	def_name->string.length = 0;
	def_name->type = aml_def_name;
	def_name->component.def_name.name_op = analyse_aml_name_op(aml);
	def_name->string.length += def_name->component.def_name.name_op->string.length;
	aml.initial += def_name->component.def_name.name_op->string.length;
	aml.length -= def_name->component.def_name.name_op->string.length;
	def_name->component.def_name.name_string = analyse_aml_name_string(aml);;
	def_name->string.length += def_name->component.def_name.name_string->string.length;
	aml.initial += def_name->component.def_name.name_string->string.length;
	aml.length -= def_name->component.def_name.name_string->string.length;
	def_name->component.def_name.data_ref_object = analyse_aml_data_ref_object(aml);
	return def_name;
}

// <def_op_region> := <op_region_op> <name_string> <region_space> <region_offset> <region_len>
AMLSymbol *analyse_aml_def_op_region(AMLSubstring aml)
{
	AMLSymbol *def_op_region = malloc(sizeof(*def_op_region));
	def_op_region->string.initial = aml.initial;
	def_op_region->string.length = 0;
	def_op_region->type = aml_def_op_region;
	def_op_region->component.def_op_region.op_region_op = analyse_aml_op_region_op(aml);
	def_op_region->string.length += def_op_region->component.def_op_region.op_region_op->string.length;
	aml.initial += def_op_region->component.def_op_region.op_region_op->string.length;
	aml.length -= def_op_region->component.def_op_region.op_region_op->string.length;
	def_op_region->component.def_op_region.name_string = analyse_aml_name_string(aml);
	def_op_region->string.length += def_op_region->component.def_op_region.name_string->string.length;
	aml.initial += def_op_region->component.def_op_region.name_string->string.length;
	aml.length -= def_op_region->component.def_op_region.name_string->string.length;
	def_op_region->component.def_op_region.region_space = analyse_aml_region_space(aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_space->string.length;
	aml.initial += def_op_region->component.def_op_region.region_space->string.length;
	aml.length -= def_op_region->component.def_op_region.region_space->string.length;
	def_op_region->component.def_op_region.region_offset = analyse_aml_region_offset(aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_offset->string.length;
	aml.initial += def_op_region->component.def_op_region.region_offset->string.length;
	aml.length -= def_op_region->component.def_op_region.region_offset->string.length;
	def_op_region->component.def_op_region.region_len = analyse_aml_region_len(aml);
	def_op_region->string.length += def_op_region->component.def_op_region.region_len->string.length;
	aml.initial += def_op_region->component.def_op_region.region_len->string.length;
	aml.length -= def_op_region->component.def_op_region.region_len->string.length;
	return def_op_region;
}

// <digit_char> := '0' - '9'
AMLSymbol *analyse_aml_digit_char(AMLSubstring aml)
{
	AMLSymbol *digit_char = malloc(sizeof(*digit_char));
	digit_char->string.initial = aml.initial;
	digit_char->string.length = 1;
	digit_char->type = aml_digit_char;
	if(!('0' <= *digit_char->string.initial && *digit_char->string.initial <= '9'))ERROR(); // Incorrect digit char
	return digit_char;
}

// <dual_name_path> := <dual_name_prefix> <name_seg> <name_seg>
AMLSymbol *analyse_aml_dual_name_path(AMLSubstring aml)
{
	AMLSymbol *dual_name_path = malloc(sizeof(*dual_name_path));
	dual_name_path->string.initial = aml.initial;
	dual_name_path->string.length = 0;
	dual_name_path->type = aml_dual_name_path;
	dual_name_path->component.dual_name_path.dual_name_prefix = analyse_aml_dual_name_prefix(aml);
	dual_name_path->string.length += dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	aml.initial += dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	aml.length -= dual_name_path->component.dual_name_path.dual_name_prefix->string.length;
	for(AMLSymbol **name_seg = dual_name_path->component.dual_name_path.name_seg; name_seg != dual_name_path->component.dual_name_path.name_seg + _countof(dual_name_path->component.dual_name_path.name_seg); name_seg++)
	{
		*name_seg = analyse_aml_name_seg(aml);
		dual_name_path->string.length += (*name_seg)->string.length;
		aml.initial += (*name_seg)->string.length;
		aml.length -= (*name_seg)->string.length;
	}
	return dual_name_path;
}

// <dual_name_prefix> := AML_BYTE_DUAL_NAME_PREFIX
AMLSymbol *analyse_aml_dual_name_prefix(AMLSubstring aml)
{
	AMLSymbol *dual_name_prefix = malloc(sizeof(*dual_name_prefix));
	dual_name_prefix->string.initial = aml.initial;
	dual_name_prefix->string.length = 1;
	dual_name_prefix->type = aml_dual_name_prefix;
	if(*dual_name_prefix->string.initial != AML_BYTE_DUAL_NAME_PREFIX)ERROR(); // Incorrect dual name prefix
	return dual_name_prefix;
}

// <dword_const> := <dword_prefix> <dword_data>
AMLSymbol *analyse_aml_dword_const(AMLSubstring aml)
{
	AMLSymbol *dword_const = malloc(sizeof(*dword_const));
	dword_const->string.initial = aml.initial;
	dword_const->string.length = 0;
	dword_const->type = aml_dword_const;
	dword_const->component.dword_const.dword_prefix = analyse_aml_dword_prefix(aml);
	dword_const->string.length += dword_const->component.dword_const.dword_prefix->string.length;
	aml.initial += dword_const->component.dword_const.dword_prefix->string.length;
	aml.length -= dword_const->component.dword_const.dword_prefix->string.length;
	dword_const->component.dword_const.dword_data = analyse_aml_dword_data(aml);
	dword_const->string.length += dword_const->component.dword_const.dword_data->string.length;
	aml.initial += dword_const->component.dword_const.dword_data->string.length;
	aml.length -= dword_const->component.dword_const.dword_data->string.length;
	return dword_const;
}

// <dword_data> := <word_data> <word_data>
AMLSymbol *analyse_aml_dword_data(AMLSubstring aml)
{
	AMLSymbol *dword_data = malloc(sizeof(*dword_data));
	dword_data->string.initial = aml.initial;
	dword_data->string.length = 0;
	dword_data->type = aml_dword_data;
	for(AMLSymbol **word_data = dword_data->component.dword_data.word_data; word_data != dword_data->component.dword_data.word_data + _countof(dword_data->component.dword_data.word_data); word_data++)
	{
		*word_data = analyse_aml_word_data(aml);
		dword_data->string.length += (*word_data)->string.length;
		aml.initial += (*word_data)->string.length;
		aml.length -= (*word_data)->string.length;
	}
	return dword_data;
}

// <dword_prefix> := AML_BYTE_DWORD_PREFIX
AMLSymbol *analyse_aml_dword_prefix(AMLSubstring aml)
{
	AMLSymbol *dword_prefix = malloc(sizeof(*dword_prefix));
	dword_prefix->string.initial = aml.initial;
	dword_prefix->string.length = 1;
	dword_prefix->type = aml_dword_prefix;
	if(*dword_prefix->string.initial != AML_BYTE_DWORD_PREFIX)ERROR(); // Incorrect dword prefix
	return dword_prefix;
}

// <expression_opcode> := <def_aquire> | <def_add> | <def_and> | <def_buffer> | <def_concat> | <def_concat_res> | <def_cond_ref_of> | <def_copy_object> | <def_decrement> | <def_deref_of> | <def_divide> | <def_find_set_left_bit> | <def_find_set_right_bit> | <def_from_bcd> | <def_increment> | <def_index> | <def_l_and> | <def_l_equal> | <def_l_greater> | <def_l_greater_equal> | <def_l_less> | <def_l_less_equal> | <def_mid> | <def_l_not> | <def_l_not_equal> | <def_load_table> | <def_l_or> | <def_match> | <def_mod> | <def_multiply> | <def_nand> | <def_nor> | <def_not> | <def_object_type> | <def_or> | <def_package> | <def_var_package> | <def_ref_of> | <def_shift_left> | <def_shift_right> | <def_size_of> | <def_store> | <def_subtract> | <def_timer> | <def_to_bcd> | <def_to_buffer> | <def_to_decimal_string> | <def_to_hex_string> | <def_to_integer> | <def_to_string> | <def_wait> | <def_xor> | <method_invocation>
AMLSymbol *analyse_aml_expression_opcode(AMLSubstring aml)
{
	AMLSymbol *expression_opcode = malloc(sizeof(*expression_opcode));
	expression_opcode->string.initial = aml.initial;
	expression_opcode->string.length = 0;
	expression_opcode->type = aml_expression_opcode;
	expression_opcode->component.expression_opcode.def_add = NULL;
	expression_opcode->component.expression_opcode.def_and = NULL;
	expression_opcode->component.expression_opcode.def_aquire = NULL;
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
	return expression_opcode;
}

// <ext_op_prefix> := AML_BYTE_EXT_OP_PREFIX
AMLSymbol *analyse_aml_ext_op_prefix(AMLSubstring aml)
{
	AMLSymbol *ext_op_prefix = malloc(sizeof(*ext_op_prefix));
	ext_op_prefix->string.initial = aml.initial;
	ext_op_prefix->string.length = 1;
	ext_op_prefix->type = aml_ext_op_prefix;
	if(*ext_op_prefix->string.initial != AML_BYTE_EXT_OP_PREFIX)ERROR(); // Incorrect ext op prefix
	return ext_op_prefix;
}

// <field_flags>
AMLSymbol *analyse_aml_field_flags(AMLSubstring aml)
{
	AMLSymbol *field_flags = malloc(sizeof(*field_flags));
	field_flags->string.initial = aml.initial;
	field_flags->string.length = 1;
	field_flags->type = aml_field_flags;
	if(0x80 <= *field_flags->string.initial)ERROR(); // Incorrect field flags
	return field_flags;
}

// <field_list> := Nothing | <field_element> <field_list>
AMLSymbol *analyse_aml_field_list(AMLSubstring aml)
{
	AMLSymbol *field_list = malloc(sizeof(*field_list));
	field_list->string.initial = aml.initial;
	field_list->string.length = 0;
	field_list->type = aml_field_list;
	field_list->component.field_list.field_element = NULL;
	field_list->component.field_list.field_list = NULL;
	return field_list;
}

// <field_op> := <ext_op_prefix> <field_op_prefix>
AMLSymbol *analyse_aml_field_op(AMLSubstring aml)
{
	AMLSymbol *field_op = malloc(sizeof(*field_op));
	field_op->string.initial = aml.initial;
	field_op->string.length = 0;
	field_op->type = aml_field_op;
	field_op->component.field_op.ext_op_prefix = analyse_aml_ext_op_prefix(aml);
	field_op->string.length += field_op->component.field_op.ext_op_prefix->string.length;
	aml.initial += field_op->component.field_op.ext_op_prefix->string.length;
	aml.length -= field_op->component.field_op.ext_op_prefix->string.length;
	field_op->component.field_op.field_op_prefix = analyse_aml_field_op_prefix(aml);
	field_op->string.length += field_op->component.field_op.field_op_prefix->string.length;
	aml.initial += field_op->component.field_op.field_op_prefix->string.length;
	aml.length -= field_op->component.field_op.field_op_prefix->string.length;
	return field_op;
}

// <field_op_prefix> := AML_BYTE_FIELD_OP_PREFIX
AMLSymbol *analyse_aml_field_op_prefix(AMLSubstring aml)
{
	AMLSymbol *field_op_prefix = malloc(sizeof(*field_op_prefix));
	field_op_prefix->string.initial = aml.initial;
	field_op_prefix->string.length = 1;
	field_op_prefix->type = aml_field_op_prefix;
	if(*aml.initial != AML_BYTE_FIELD_OP)ERROR(); // Incorrect field op prefix
	return field_op_prefix;
}

// <lead_name_char> := 'A' - 'Z' | '_'
AMLSymbol *analyse_aml_lead_name_char(AMLSubstring aml)
{
	AMLSymbol *lead_name_char = malloc(sizeof(*lead_name_char));
	lead_name_char->string.initial = aml.initial;
	lead_name_char->string.length = 1;
	lead_name_char->type = aml_lead_name_char;
	if(!(('A' <= *lead_name_char->string.initial && *lead_name_char->string.initial <= 'Z') || *lead_name_char->string.initial == '_'))ERROR(); // Incorrect lead name char
	return lead_name_char;
}

// <multi_name_path> := <multi_name_prefix> <seg_count> <name_seg>*
AMLSymbol *analyse_aml_multi_name_path(AMLSubstring aml)
{
	AMLSymbol *multi_name_path = malloc(sizeof(*multi_name_path));
	multi_name_path->string.initial = aml.initial;
	multi_name_path->string.length = 0;
	multi_name_path->type = aml_multi_name_path;
	multi_name_path->component.multi_name_path.multi_name_prefix = analyse_aml_multi_name_prefix(aml);
	multi_name_path->string.length += multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	aml.initial += multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	aml.length -= multi_name_path->component.multi_name_path.multi_name_prefix->string.length;
	multi_name_path->component.multi_name_path.seg_count = analyse_aml_seg_count(aml);
	multi_name_path->string.length += multi_name_path->component.multi_name_path.seg_count->string.length;
	aml.initial += multi_name_path->component.multi_name_path.seg_count->string.length;
	aml.length -= multi_name_path->component.multi_name_path.seg_count->string.length;
	multi_name_path->component.multi_name_path.name_seg = malloc(*multi_name_path->component.multi_name_path.seg_count->string.initial * sizeof(*multi_name_path->component.multi_name_path.name_seg));
	for(unsigned int i = 0; i < *multi_name_path->component.multi_name_path.seg_count->string.initial; i++)
	{
		multi_name_path->component.multi_name_path.name_seg[i] = analyse_aml_name_seg(aml);
		multi_name_path->string.length += multi_name_path->component.multi_name_path.name_seg[i]->string.length;
		aml.initial += multi_name_path->component.multi_name_path.name_seg[i]->string.length;
		aml.length -= multi_name_path->component.multi_name_path.name_seg[i]->string.length;
	}
	return multi_name_path;
}

// <mult_name_prefix> := AML_BYTE_MULTI_NAME_PREFIX
AMLSymbol *analyse_aml_multi_name_prefix(AMLSubstring aml)
{
	AMLSymbol *multi_name_prefix = malloc(sizeof(*multi_name_prefix));
	multi_name_prefix->string.initial = aml.initial;
	multi_name_prefix->string.length = 1;
	multi_name_prefix->type = aml_multi_name_prefix;
	if(*multi_name_prefix->string.initial != AML_BYTE_MULTI_NAME_PREFIX)ERROR(); // Incorrect munti name prefix
	return multi_name_prefix;
}

// <name_char> := <digit_char> | <lead_name_char>
AMLSymbol *analyse_aml_name_char(AMLSubstring aml)
{
	AMLSymbol *name_char = malloc(sizeof(*name_char));
	name_char->string.initial = aml.initial;
	name_char->string.length = 0;
	name_char->type = aml_name_char;
	name_char->component.name_char.digit_char = NULL;
	name_char->component.name_char.lead_name_char = NULL;
	if('0' <= *aml.initial && *aml.initial <= '9')
	{
		name_char->component.name_char.digit_char = analyse_aml_digit_char(aml);
		name_char->string.length += name_char->component.name_char.digit_char->string.length;
	}
	else if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
	{
		name_char->component.name_char.lead_name_char = analyse_aml_lead_name_char(aml);
		name_char->string.length += name_char->component.name_char.lead_name_char->string.length;
	}
	else ERROR(); // Incorrect name char
	return name_char;
}

// <name_op> := AML_BYTE_NAME_OP
AMLSymbol *analyse_aml_name_op(AMLSubstring aml)
{
	AMLSymbol *name_op = malloc(sizeof(*name_op));
	name_op->string.initial = aml.initial;
	name_op->string.length = 1;
	name_op->type = aml_name_op;
	if(*name_op->string.initial != AML_BYTE_NAME_OP)ERROR(); // Incorrect name op
	return name_op;
}

// <name_path> := <name_seg> | <dual_name_path> | <multi_name_path> | <null_name>
AMLSymbol *analyse_aml_name_path(AMLSubstring aml)
{
	AMLSymbol *name_path = malloc(sizeof(*name_path));
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
		name_path->component.name_path.dual_name_path = analyse_aml_dual_name_path(aml);
		name_path->string.length += name_path->component.name_path.dual_name_path->string.length;
		break;
	case AML_BYTE_MULTI_NAME_PREFIX:
		name_path->component.name_path.multi_name_path = analyse_aml_multi_name_path(aml);
		name_path->string.length += name_path->component.name_path.multi_name_path->string.length;
		break;
	case AML_BYTE_NULL_NAME:
		name_path->component.name_path.null_name = analyse_aml_null_name(aml);
		name_path->string.length += name_path->component.name_path.null_name->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			name_path->component.name_path.name_seg = analyse_aml_name_seg(aml);
			name_path->string.length += name_path->component.name_path.name_seg->string.length;
		}
		break;
	}
	return name_path;
}

// <name_seg> := <lead_name_char> <name_char> <name_char> <name_char>
AMLSymbol *analyse_aml_name_seg(AMLSubstring aml)
{
	AMLSymbol *name_seg = malloc(sizeof(*name_seg));
	name_seg->string.initial = aml.initial;
	name_seg->string.length = 0;
	name_seg->type = aml_name_seg;
	name_seg->component.name_seg.lead_name_char = analyse_aml_lead_name_char(aml);
	aml.initial += name_seg->component.name_seg.lead_name_char->string.length;
	aml.length -= name_seg->component.name_seg.lead_name_char->string.length;
	name_seg->string.length += name_seg->component.name_seg.lead_name_char->string.length;
	for(AMLSymbol **name_char = name_seg->component.name_seg.name_char; name_char != name_seg->component.name_seg.name_char + _countof(name_seg->component.name_seg.name_char); name_char++)
	{
		*name_char = analyse_aml_name_char(aml);
		name_seg->string.length += (*name_char)->string.length;
		aml.initial += (*name_char)->string.length;
		aml.length -= (*name_char)->string.length;
	}
	return name_seg;
}

// <name_space_modifier_obj> := <def_alias> | <def_name> | <def_scope>
AMLSymbol *analyse_aml_name_space_modifier_obj(AMLSubstring aml)
{
	AMLSymbol *name_space_modifier_obj = malloc(sizeof(*name_space_modifier_obj));
	name_space_modifier_obj->string.initial = aml.initial;
	name_space_modifier_obj->string.length = 0;
	name_space_modifier_obj->type = aml_name_space_modifier_obj;
	name_space_modifier_obj->component.name_space_modifier_obj.def_alias = NULL;
	name_space_modifier_obj->component.name_space_modifier_obj.def_name = NULL;
	name_space_modifier_obj->component.name_space_modifier_obj.def_scope = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ALIAS_OP:
		name_space_modifier_obj->component.name_space_modifier_obj.def_alias = analyse_aml_def_alias(aml);
		name_space_modifier_obj->string.length += name_space_modifier_obj->component.name_space_modifier_obj.def_alias->string.length;
		break;
	case AML_BYTE_NAME_OP:
		name_space_modifier_obj->component.name_space_modifier_obj.def_name = analyse_aml_def_name(aml);
		name_space_modifier_obj->string.length += name_space_modifier_obj->component.name_space_modifier_obj.def_name->string.length;
		break;
	}
	return name_space_modifier_obj;
}

// <name_string> := <root_char> <name_path> | <prefix_path> <name_path>
AMLSymbol *analyse_aml_name_string(AMLSubstring aml)
{
	AMLSymbol *name_string = malloc(sizeof(*name_string));
	name_string->string.initial = aml.initial;
	name_string->string.length = 0;
	name_string->type = aml_name_string;
	name_string->component.name_string.root_char = NULL;
	name_string->component.name_string.prefix_path = NULL;
	name_string->component.name_string.name_path = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ROOT_CHAR:
		name_string->component.name_string.root_char = analyse_aml_root_char(aml);
		name_string->string.length += name_string->component.name_string.root_char->string.length;
		aml.initial += name_string->component.name_string.root_char->string.length;
		aml.length -= name_string->component.name_string.root_char->string.length;
		break;
	default:
		name_string->component.name_string.prefix_path = analyse_aml_prefix_path(aml);
		name_string->string.length += name_string->component.name_string.prefix_path->string.length;
		aml.initial += name_string->component.name_string.prefix_path->string.length;
		aml.length -= name_string->component.name_string.prefix_path->string.length;
		break;
	}
	name_string->component.name_string.name_path = analyse_aml_name_path(aml);
	name_string->string.length += name_string->component.name_string.name_path->string.length;
	return name_string;
}

// <named_obj> := <def_bank_field> | <def_create_bit_field> | <def_create_byte_field> | <def_create_dword_field> | <def_create_field> | <def_create_qword_field> | <def_create_word_field> | <def_data_region> | <def_external> | <def_op_region> | <def_power_res> | <def_thermal_zone>
AMLSymbol *analyse_aml_named_obj(AMLSubstring aml)
{
	AMLSymbol *named_obj = malloc(sizeof(*named_obj));
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
	named_obj->component.named_obj.def_external = NULL;
	named_obj->component.named_obj.def_op_region = NULL;
	named_obj->component.named_obj.def_power_res = NULL;
	named_obj->component.named_obj.def_thermal_zone = NULL;
	switch(aml.initial[0])
	{
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_OP_REGION_OP:
			named_obj->component.named_obj.def_op_region = analyse_aml_def_op_region(aml);
			named_obj->string.length += named_obj->component.named_obj.def_op_region->string.length;
			break;
		case AML_BYTE_FIELD_OP:
			named_obj->component.named_obj.def_field = analyse_aml_def_field(aml);
			named_obj->string.length += named_obj->component.named_obj.def_field->string.length;
		}
		break;
	}
	return named_obj;
}

// <null_char> := AML_BYTE_NULL_CHAR
AMLSymbol *analyse_aml_null_char(AMLSubstring aml)
{
	AMLSymbol *null_char = malloc(sizeof(*null_char));
	null_char->string.initial = aml.initial;
	null_char->string.length = 1;
	null_char->type = aml_null_char;
	if(*null_char->string.initial == AML_BYTE_NULL_CHAR)ERROR(); // Incorrect null char
	return null_char;
}

// <null_name> := AML_BYTE_NULL_NAME
AMLSymbol *analyse_aml_null_name(AMLSubstring aml)
{
	AMLSymbol *null_name = malloc(sizeof(*null_name));
	null_name->string.initial = aml.initial;
	null_name->string.length = 1;
	null_name->type = aml_null_name;
	if(*null_name->string.initial != AML_BYTE_NULL_NAME)ERROR(); // Incorrect null name
	return null_name;
}

// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSubstring aml)
{
	AMLSymbol *object = malloc(sizeof(*object));
	object->string.initial = aml.initial;
	object->string.length = 0;
	object->type = aml_object;
	object->component.object.name_space_modifier_obj = NULL;
	object->component.object.named_obj = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_NAME_OP:
		object->component.object.name_space_modifier_obj = analyse_aml_name_space_modifier_obj(aml);
		object->string.length += object->component.object.name_space_modifier_obj->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		object->component.object.named_obj = analyse_aml_named_obj(aml);
		object->string.length += object->component.object.named_obj->string.length;
		break;
	}
	return object;
}

// <one_op> := AML_BYTE_ONE_OP
AMLSymbol *analyse_aml_one_op(AMLSubstring aml)
{
	AMLSymbol *one_op = malloc(sizeof(*one_op));
	one_op->string.initial = aml.initial;
	one_op->string.length = 1;
	one_op->type = aml_one_op;
	if(*one_op->string.initial != AML_BYTE_ONE_OP)ERROR(); // Incorrect one op
	return one_op;
}

// <ones_op> := AML_BYTE_ONES_OP
AMLSymbol *analyse_aml_ones_op(AMLSubstring aml)
{
	AMLSymbol *ones_op = malloc(sizeof(*ones_op));
	ones_op->string.initial = aml.initial;
	ones_op->string.length = 1;
	ones_op->type = aml_ones_op;
	if(*ones_op->string.initial != AML_BYTE_ONES_OP)ERROR(); // Incorrect ones op
	return ones_op;
}

// <op_region_op> := <ext_op_prefix> <op_region_op_prefix>
AMLSymbol *analyse_aml_op_region_op(AMLSubstring aml)
{
	AMLSymbol *op_region_op = malloc(sizeof(*op_region_op));
	op_region_op->string.initial = aml.initial;
	op_region_op->string.length = 0;
	op_region_op->type = aml_op_region_op;
	op_region_op->component.op_region_op.ext_op_prefix = analyse_aml_ext_op_prefix(aml);
	op_region_op->string.length += op_region_op->component.op_region_op.ext_op_prefix->string.length;
	aml.initial += op_region_op->component.op_region_op.ext_op_prefix->string.length;
	aml.length -= op_region_op->component.op_region_op.ext_op_prefix->string.length;
	op_region_op->component.op_region_op.op_region_op_prefix = analyse_aml_op_region_op_prefix(aml);
	op_region_op->string.length += op_region_op->component.op_region_op.op_region_op_prefix->string.length;
	aml.initial += op_region_op->component.op_region_op.op_region_op_prefix->string.length;
	aml.length -= op_region_op->component.op_region_op.op_region_op_prefix->string.length;
	return op_region_op;
}

// <op_region_op_prefix> := AML_BYTE_OP_REGION_OP
AMLSymbol *analyse_aml_op_region_op_prefix(AMLSubstring aml)
{
	AMLSymbol *op_region_op_prefix = malloc(sizeof(*op_region_op_prefix));
	op_region_op_prefix->string.initial = aml.initial;
	op_region_op_prefix->string.length = 1;
	op_region_op_prefix->type = aml_op_region_op_prefix;
	if(*aml.initial != AML_BYTE_OP_REGION_OP)ERROR();
	return op_region_op_prefix;
}

// <parent_prefix_char> := AML_BYTE_PARENT_PREFIX_CHAR
AMLSymbol *analyse_aml_parent_prefix_char(AMLSubstring aml)
{
	AMLSymbol *parent_prefix_char = malloc(sizeof(*parent_prefix_char));
	parent_prefix_char->string.initial = aml.initial;
	parent_prefix_char->string.length = 1;
	parent_prefix_char->type = aml_parent_prefix_char;
	if(*parent_prefix_char->string.initial != AML_BYTE_PARENT_PREFIX_CHAR)ERROR(); // Incorrect parent prefix char
	return parent_prefix_char;
}

// <pkg_lead_byte>
AMLSymbol *analyse_aml_pkg_lead_byte(AMLSubstring aml)
{
	AMLSymbol *pkg_lead_byte = malloc(sizeof(*pkg_lead_byte));
	pkg_lead_byte->string.initial = aml.initial;
	pkg_lead_byte->string.length = 1;
	pkg_lead_byte->type = aml_pkg_lead_byte;
	return pkg_lead_byte;
}

// <pkg_length> := <pkg_lead_byte> | <pkg_lead_byte> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> <byte_data>
AMLSymbol *analyse_aml_pkg_length(AMLSubstring aml)
{
	AMLSymbol *pkg_length = malloc(sizeof(*pkg_length));
	pkg_length->string.initial = aml.initial;
	pkg_length->string.length = 0;
	pkg_length->type = aml_pkg_length;
	pkg_length->component.pkg_length.pkg_lead_byte = NULL;
	for(AMLSymbol **byte_data = pkg_length->component.pkg_length.byte_data; byte_data != pkg_length->component.pkg_length.byte_data + _countof(pkg_length->component.pkg_length.byte_data); byte_data++)*byte_data = NULL;
	pkg_length->component.pkg_length.pkg_lead_byte = analyse_aml_pkg_lead_byte(aml);
	pkg_length->string.length += pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	aml.initial += pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	aml.length -= pkg_length->component.pkg_length.pkg_lead_byte->string.length;
	pkg_length->component.pkg_length.num_of_byte_data = *pkg_length->component.pkg_length.pkg_lead_byte->string.initial >> 6 & 3;
	for(AMLSymbol **byte_data = pkg_length->component.pkg_length.byte_data; byte_data != pkg_length->component.pkg_length.byte_data + pkg_length->component.pkg_length.num_of_byte_data; byte_data++)
	{
		*byte_data = analyse_aml_byte_data(aml);
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

// <prefix_path> := Nothing | <parent_prefix_char> <prefix_path>
AMLSymbol *analyse_aml_prefix_path(AMLSubstring aml)
{
	AMLSymbol *prefix_path = malloc(sizeof(*prefix_path));
	prefix_path->string.initial = aml.initial;
	prefix_path->string.length = 0;
	prefix_path->type = aml_prefix_path;
	prefix_path->component.prefix_path.parent_prefix_char = NULL;
	prefix_path->component.prefix_path.prefix_path = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_PARENT_PREFIX_CHAR:
		prefix_path->component.prefix_path.parent_prefix_char = analyse_aml_parent_prefix_char(aml);
		prefix_path->string.length += prefix_path->component.prefix_path.parent_prefix_char->string.length;
		aml.initial = aml.initial + prefix_path->component.prefix_path.parent_prefix_char->string.length;
		aml.length = aml.length - prefix_path->component.prefix_path.parent_prefix_char->string.length;
		prefix_path->component.prefix_path.prefix_path = analyse_aml_prefix_path(aml);
		prefix_path->string.length += prefix_path->component.prefix_path.prefix_path->string.length;
		break;
	}
	return prefix_path;
}

// <qword_const> := <qword_prefix> <qword_data>
AMLSymbol *analyse_aml_qword_const(AMLSubstring aml)
{
	AMLSymbol *qword_const = malloc(sizeof(*qword_const));
	qword_const->string.initial = aml.initial;
	qword_const->string.length = 0;
	qword_const->type = aml_qword_const;
	qword_const->component.qword_const.qword_prefix = analyse_aml_qword_prefix(aml);
	qword_const->string.length += qword_const->component.qword_const.qword_prefix->string.length;
	aml.initial += qword_const->component.qword_const.qword_prefix->string.length;
	aml.length -= qword_const->component.qword_const.qword_prefix->string.length;
	qword_const->component.qword_const.qword_data = analyse_aml_qword_data(aml);
	qword_const->string.length += qword_const->component.qword_const.qword_data->string.length;
	aml.initial += qword_const->component.qword_const.qword_data->string.length;
	aml.length -= qword_const->component.qword_const.qword_data->string.length;
	return qword_const;
}

// <qword_data> := <dword_data> <dword_data>
AMLSymbol *analyse_aml_qword_data(AMLSubstring aml)
{
	AMLSymbol *qword_data = malloc(sizeof(*qword_data));
	qword_data->string.initial = aml.initial;
	qword_data->string.length = 0;
	qword_data->type = aml_qword_data;
	for(AMLSymbol **dword_data = qword_data->component.qword_data.dword_data; dword_data != qword_data->component.qword_data.dword_data + _countof(qword_data->component.qword_data.dword_data); dword_data++)
	{
		*dword_data = analyse_aml_dword_data(aml);
		qword_data->string.length += (*dword_data)->string.length;
		aml.initial += (*dword_data)->string.length;
		aml.length -= (*dword_data)->string.length;
	}
	return qword_data;
}

// <qword_prefix> := AML_BYTE_QWORD_PREFIX
AMLSymbol *analyse_aml_qword_prefix(AMLSubstring aml)
{
	AMLSymbol *qword_prefix = malloc(sizeof(*qword_prefix));
	qword_prefix->string.initial = aml.initial;
	qword_prefix->string.length = 1;
	qword_prefix->type = aml_qword_prefix;
	if(*qword_prefix->string.initial != AML_BYTE_QWORD_PREFIX)ERROR(); // Incorrect qword prefix
	return qword_prefix;
}

// <region_len> := <term_arg>
AMLSymbol *analyse_aml_region_len(AMLSubstring aml)
{
	AMLSymbol *region_len = malloc(sizeof(*region_len));
	region_len->string.initial = aml.initial;
	region_len->string.length = 0;
	region_len->type = aml_region_len;
	region_len->component.region_len.term_arg = analyse_aml_term_arg(aml);
	region_len->string.length += region_len->component.region_len.term_arg->string.length;
	return region_len;
}

// <region_offset> := <term_arg>
AMLSymbol *analyse_aml_region_offset(AMLSubstring aml)
{
	AMLSymbol *region_offset = malloc(sizeof(*region_offset));
	region_offset->string.initial = aml.initial;
	region_offset->string.length = 0;
	region_offset->type = aml_region_offset;
	region_offset->component.region_offset.term_arg = analyse_aml_term_arg(aml);
	region_offset->string.length += region_offset->component.region_offset.term_arg->string.length;
	return region_offset;
}

// <region_space>
AMLSymbol *analyse_aml_region_space(AMLSubstring aml)
{
	AMLSymbol *region_space = malloc(sizeof(*region_space));
	region_space->string.initial = aml.initial;
	region_space->string.length = 1;
	region_space->type = aml_region_space;
	return region_space;
}

// <revision_op> := <ext_op_prefix> <revision_op_prefix>
AMLSymbol *analyse_aml_revision_op(AMLSubstring aml)
{
	AMLSymbol *revision_op = malloc(sizeof(*revision_op));
	revision_op->string.initial = aml.initial;
	revision_op->string.length = 0;
	revision_op->type = aml_revision_op;
	revision_op->component.revision_op.ext_op_prefix = analyse_aml_ext_op_prefix(aml);
	revision_op->string.length += revision_op->component.revision_op.ext_op_prefix->string.length;
	aml.initial += revision_op->component.revision_op.ext_op_prefix->string.length;
	aml.length -= revision_op->component.revision_op.ext_op_prefix->string.length;
	revision_op->component.revision_op.revision_op_prefix = analyse_aml_revision_op_prefix(aml);
	revision_op->string.length += revision_op->component.revision_op.revision_op_prefix->string.length;
	aml.initial += revision_op->component.revision_op.revision_op_prefix->string.length;
	aml.length -= revision_op->component.revision_op.revision_op_prefix->string.length;
	return revision_op;
}

// <revision_op_prefix> := AML_BYTE_REVISION_OP
AMLSymbol *analyse_aml_revision_op_prefix(AMLSubstring aml)
{
	AMLSymbol *revision_op_prefix = malloc(sizeof(*revision_op_prefix));
	revision_op_prefix->string.initial = aml.initial;
	revision_op_prefix->string.length = 1;
	revision_op_prefix->type = aml_revision_op_prefix;
	if(*revision_op_prefix->string.initial != AML_BYTE_REVISION_OP)ERROR(); // Incorrect revision op prefix
	return revision_op_prefix;
}

// <root_char> := AML_BYTE_ROOT_CHAR
AMLSymbol *analyse_aml_root_char(AMLSubstring aml)
{
	AMLSymbol *root_char = malloc(sizeof(*root_char));
	root_char->string.initial = aml.initial;
	root_char->string.length = 1;
	root_char->type = aml_root_char;
	if(*root_char->string.initial != AML_BYTE_ROOT_CHAR)ERROR(); // Incorrect root_char
	return root_char;
}

// <seg_count> := 0x01 - 0xff
AMLSymbol *analyse_aml_seg_count(AMLSubstring aml)
{
	AMLSymbol *seg_count = malloc(sizeof(*seg_count));
	seg_count->string.initial = aml.initial;
	seg_count->string.length = 1;
	seg_count->type = aml_seg_count;
	if(*seg_count->string.initial == 0x00)ERROR(); // SegCount can be from 1 to 255.
	return seg_count;
}

// <statement_opcode> := <def_break> | <def_breakpoint> | <def_continue> | <def_fatal> | <def_if_else> | <def_noop> | <def_notify> | <def_release> | <def_reset> | <def_return> | <def_signal> | <def_sleep> | <def_stall> | <def_while>
AMLSymbol *analyse_aml_statement_opcode(AMLSubstring aml)
{
	AMLSymbol *statement_opcode = malloc(sizeof(*statement_opcode));
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
	return statement_opcode;
}

// <string> := <string_prefix> <ascii_char_list> <null_char>
AMLSymbol *analyse_aml_string(AMLSubstring aml)
{
	AMLSymbol *string = malloc(sizeof(*string));
	string->string.initial = aml.initial;
	string->string.length = 0;
	string->type = aml_string;
	string->component.string.string_prefix = analyse_aml_string_prefix(aml);
	string->string.length += string->component.string.string_prefix->string.length;
	aml.initial += string->component.string.string_prefix->string.length;
	aml.length -= string->component.string.string_prefix->string.length;
	string->component.string.ascii_char_list = analyse_aml_ascii_char_list(aml);
	string->string.length += string->component.string.ascii_char_list->string.length;
	aml.initial += string->component.string.ascii_char_list->string.length;
	aml.length -= string->component.string.ascii_char_list->string.length;
	string->component.string.null_char = analyse_aml_null_char(aml);
	string->string.length += string->component.string.null_char->string.length;
	aml.initial += string->component.string.null_char->string.length;
	aml.length -= string->component.string.null_char->string.length;
	return string;
}

// <string_prefix> := AML_BYTE_STRING_PREFIX
AMLSymbol *analyse_aml_string_prefix(AMLSubstring aml)
{
	AMLSymbol *string_prefix = malloc(sizeof(*string_prefix));
	string_prefix->string.initial = aml.initial;
	string_prefix->string.length = 1;
	string_prefix->type = aml_string_prefix;
	if(*string_prefix->string.initial != AML_BYTE_STRING_PREFIX)ERROR(); // Incorrect string prefix
	return string_prefix;
}

// <term_arg> := <expression_opcode> | <data_object> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_term_arg(AMLSubstring aml)
{
	AMLSymbol *term_arg = malloc(sizeof(*term_arg));
	term_arg->string.initial = aml.initial;
	term_arg->string.length = 0;
	term_arg->type = aml_term_arg;
	term_arg->component.term_arg.expression_opcode = NULL;
	term_arg->component.term_arg.data_object = NULL;
	term_arg->component.term_arg.arg_obj = NULL;
	term_arg->component.term_arg.local_obj = NULL;
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
		term_arg->component.term_arg.data_object = analyse_aml_data_object(aml);
		term_arg->string.length += term_arg->component.term_arg.data_object->string.length;
		break;
	}
	return term_arg;
}

// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSubstring aml)
{
	AMLSymbol *term_list = malloc(sizeof(*term_list));
	term_list->string.initial = aml.initial;
	term_list->string.length = 0;
	term_list->type = aml_term_list;
	term_list->component.term_list.term_list = NULL;
	term_list->component.term_list.term_obj = NULL;
	if(aml.length)
	{
		// <term_list> := <term_obj> <term_list>
		term_list->component.term_list.term_obj = analyse_aml_term_obj(aml);
		term_list->string.length += term_list->component.term_list.term_obj->string.length;
		aml.initial += term_list->component.term_list.term_obj->string.length;
		aml.length -= term_list->component.term_list.term_obj->string.length;
		if(aml.length && term_list->component.term_list.term_obj->string.length)switch(*aml.initial)
		{
		case AML_BYTE_ALIAS_OP:
		case AML_BYTE_EXT_OP_PREFIX:
		case AML_BYTE_NAME_OP:
			term_list->component.term_list.term_list = analyse_aml_term_list(aml);
			term_list->string.length += term_list->component.term_list.term_list->string.length;
			aml.initial += term_list->component.term_list.term_list->string.length;
			aml.length -= term_list->component.term_list.term_list->string.length;
			break;
		}
	}
	return term_list;
}

// <term_obj> := <object> | <statement_opcode> | <expression_opcode>
AMLSymbol *analyse_aml_term_obj(AMLSubstring aml)
{
	AMLSymbol *term_obj = malloc(sizeof(*term_obj));
	term_obj->string.initial = aml.initial;
	term_obj->string.length = 0;
	term_obj->type = aml_term_obj;
	term_obj->component.term_obj.object = NULL;
	term_obj->component.term_obj.statement_opcode = NULL;
	term_obj->component.term_obj.expression_opcode = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_NAME_OP:
		term_obj->component.term_obj.object = analyse_aml_object(aml);
		term_obj->string.length += term_obj->component.term_obj.object->string.length;
		break;
	}
	return term_obj;
}

// <word_const> := <word_prefix> <word_data>
AMLSymbol *analyse_aml_word_const(AMLSubstring aml)
{
	AMLSymbol *word_const = malloc(sizeof(*word_const));
	word_const->string.initial = aml.initial;
	word_const->string.length = 0;
	word_const->type = aml_word_const;
	word_const->component.word_const.word_prefix = analyse_aml_word_prefix(aml);
	word_const->string.length += word_const->component.word_const.word_prefix->string.length;
	aml.initial += word_const->component.word_const.word_prefix->string.length;
	aml.length -= word_const->component.word_const.word_prefix->string.length;
	word_const->component.word_const.word_data = analyse_aml_word_data(aml);
	word_const->string.length += word_const->component.word_const.word_data->string.length;
	aml.initial += word_const->component.word_const.word_data->string.length;
	aml.length -= word_const->component.word_const.word_data->string.length;
	return word_const;
}

// <word_data> := <byte_data> <byte_data>
AMLSymbol *analyse_aml_word_data(AMLSubstring aml)
{
	AMLSymbol *word_data = malloc(sizeof(*word_data));
	word_data->string.initial = aml.initial;
	word_data->string.length = 0;
	word_data->type = aml_word_data;
	for(AMLSymbol **byte_data = word_data->component.word_data.byte_data; byte_data != word_data->component.word_data.byte_data + _countof(word_data->component.word_data.byte_data); byte_data++)
	{
		*byte_data = analyse_aml_byte_data(aml);
		word_data->string.length += (*byte_data)->string.length;
		aml.initial += (*byte_data)->string.length;
		aml.length -= (*byte_data)->string.length;
	}
	return word_data;
}

// <word_prefix> := AML_BYTE_WORD_PREFIX
AMLSymbol *analyse_aml_word_prefix(AMLSubstring aml)
{
	AMLSymbol *word_prefix = malloc(sizeof(*word_prefix));
	word_prefix->string.initial = aml.initial;
	word_prefix->string.length = 1;
	word_prefix->type = aml_word_prefix;
	if(*aml.initial != AML_BYTE_WORD_PREFIX)ERROR(); // Incorrect word prefix
	return word_prefix;
}

// <zero_op> := AML_BYTE_ZERO_OP
AMLSymbol *analyse_aml_zero_op(AMLSubstring aml)
{
	AMLSymbol *zero_op = malloc(sizeof(*zero_op));
	zero_op->string.initial = aml.initial;
	zero_op->string.length = 1;
	zero_op->type = aml_zero_op;
	if(*zero_op->string.initial != AML_BYTE_ZERO_OP)ERROR(); // Incorrect zero op
	return zero_op;
}

AMLSymbol *create_dsdt_aml_syntax_tree(void)
{
	return analyse_aml_term_list(get_dsdt_aml());
}

void delete_aml_symbol(AMLSymbol *aml_symbol)
{
	switch(aml_symbol->type)
	{
	case aml_alias_op:
		break;
	case aml_ascii_char:
		break;
	case aml_ascii_char_list:
		if(aml_symbol->component.ascii_char_list.ascii_char)delete_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char);
		if(aml_symbol->component.ascii_char_list.ascii_char_list)delete_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char_list);
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
	case aml_def_buffer:
		if(aml_symbol->component.def_buffer.buffer_op)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_op);
		if(aml_symbol->component.def_buffer.pkg_length)delete_aml_symbol(aml_symbol->component.def_buffer.pkg_length);
		if(aml_symbol->component.def_buffer.buffer_size)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_size);
		if(aml_symbol->component.def_buffer.byte_list)delete_aml_symbol(aml_symbol->component.def_buffer.byte_list);
		break;
	case aml_def_field:
		if(aml_symbol->component.def_field.field_op)delete_aml_symbol(aml_symbol->component.def_field.field_op);
		if(aml_symbol->component.def_field.pkg_length)delete_aml_symbol(aml_symbol->component.def_field.pkg_length);
		if(aml_symbol->component.def_field.name_string)delete_aml_symbol(aml_symbol->component.def_field.name_string);
		if(aml_symbol->component.def_field.field_flags)delete_aml_symbol(aml_symbol->component.def_field.field_flags);
		if(aml_symbol->component.def_field.field_list)delete_aml_symbol(aml_symbol->component.def_field.field_list);
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
		if(aml_symbol->component.expression_opcode.def_aquire)delete_aml_symbol(aml_symbol->component.expression_opcode.def_aquire);
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
	case aml_field_flags:
		break;
	case aml_field_list:
		if(aml_symbol->component.field_list.field_element)delete_aml_symbol(aml_symbol->component.field_list.field_element);
		if(aml_symbol->component.field_list.field_list)delete_aml_symbol(aml_symbol->component.field_list.field_list);
		break;
	case aml_field_op:
		if(aml_symbol->component.field_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.field_op.ext_op_prefix);
		if(aml_symbol->component.field_op.field_op_prefix)delete_aml_symbol(aml_symbol->component.field_op.field_op_prefix);
		break;
	case aml_field_op_prefix:
		break;
	case aml_lead_name_char:
		break;
	case aml_multi_name_path:
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])delete_aml_symbol(aml_symbol->component.multi_name_path.name_seg[i]);
		free(aml_symbol->component.multi_name_path.name_seg);
		if(aml_symbol->component.multi_name_path.multi_name_prefix)delete_aml_symbol(aml_symbol->component.multi_name_path.multi_name_prefix);
		if(aml_symbol->component.multi_name_path.seg_count)delete_aml_symbol(aml_symbol->component.multi_name_path.seg_count);
		break;
	case aml_multi_name_prefix:
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
		if(aml_symbol->component.named_obj.def_external)delete_aml_symbol(aml_symbol->component.named_obj.def_external);
		if(aml_symbol->component.named_obj.def_field)delete_aml_symbol(aml_symbol->component.named_obj.def_field);
		if(aml_symbol->component.named_obj.def_op_region)delete_aml_symbol(aml_symbol->component.named_obj.def_op_region);
		if(aml_symbol->component.named_obj.def_power_res)delete_aml_symbol(aml_symbol->component.named_obj.def_power_res);
		if(aml_symbol->component.named_obj.def_thermal_zone)delete_aml_symbol(aml_symbol->component.named_obj.def_thermal_zone);
		break;
	case aml_null_char:
		break;
	case aml_null_name:
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
		if(aml_symbol->component.op_region_op.op_region_op_prefix)delete_aml_symbol(aml_symbol->component.op_region_op.op_region_op_prefix);
		break;
	case aml_op_region_op_prefix:
		break;
	case aml_parent_prefix_char:
		break;
	case aml_pkg_lead_byte:
		break;
	case aml_pkg_length:
		if(aml_symbol->component.pkg_length.pkg_lead_byte)delete_aml_symbol(aml_symbol->component.pkg_length.pkg_lead_byte);
		for(AMLSymbol **byte_data = aml_symbol->component.pkg_length.byte_data; byte_data != aml_symbol->component.pkg_length.byte_data + _countof(aml_symbol->component.pkg_length.byte_data); byte_data++)delete_aml_symbol(*byte_data);
		break;
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)delete_aml_symbol(aml_symbol->component.prefix_path.parent_prefix_char);
		if(aml_symbol->component.prefix_path.prefix_path)delete_aml_symbol(aml_symbol->component.prefix_path.prefix_path);
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
	case aml_revision_op:
		if(aml_symbol->component.revision_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.revision_op.ext_op_prefix);
		if(aml_symbol->component.revision_op.revision_op_prefix)delete_aml_symbol(aml_symbol->component.revision_op.revision_op_prefix);
		break;
	case aml_root_char:
		break;
	case aml_seg_count:
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
	case aml_term_arg:
		if(aml_symbol->component.term_arg.expression_opcode)delete_aml_symbol(aml_symbol->component.term_arg.expression_opcode);
		if(aml_symbol->component.term_arg.data_object)delete_aml_symbol(aml_symbol->component.term_arg.data_object);
		if(aml_symbol->component.term_arg.arg_obj)delete_aml_symbol(aml_symbol->component.term_arg.arg_obj);
		if(aml_symbol->component.term_arg.local_obj)delete_aml_symbol(aml_symbol->component.term_arg.local_obj);
		break;
	case aml_term_list:
		if(aml_symbol->component.term_list.term_list)delete_aml_symbol(aml_symbol->component.term_list.term_list);
		if(aml_symbol->component.term_list.term_obj)delete_aml_symbol(aml_symbol->component.term_list.term_obj);
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object)delete_aml_symbol(aml_symbol->component.term_obj.object);
		if(aml_symbol->component.term_obj.statement_opcode)delete_aml_symbol(aml_symbol->component.term_obj.statement_opcode);
		if(aml_symbol->component.term_obj.expression_opcode)delete_aml_symbol(aml_symbol->component.term_obj.expression_opcode);
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
	ERROR(); // ACPI is not found.
	acpi_memory_region_descriptor.base = 0;
	acpi_memory_region_descriptor.length = 0;
	acpi_memory_region_descriptor.type = 0;
	acpi_memory_region_descriptor.attribute = 0;
	return acpi_memory_region_descriptor;
}

AMLSubstring get_dsdt_aml(void)
{
	AMLSubstring dsdt_aml;
	ACPITableHeader const *dsdt_header = get_dsdt_header();
	dsdt_aml.initial = (unsigned char const *)((unsigned int)dsdt_header + sizeof(*dsdt_header));
	dsdt_aml.length = dsdt_header->length - sizeof(*dsdt_header);
	return dsdt_aml;
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

