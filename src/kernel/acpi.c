#include "acpi.h"
#include "serial.h"
#include "shell.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

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
#define AML_BYTE_OP_REFION_OP		0x80
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
	ChainString *alias_op_chain_string;
	ChainString *def_add_chain_string;
	ChainString *def_alias_chain_string;
	ChainString *def_and_chain_string;
	ChainString *def_aquire_chain_string;
	ChainString *def_break_chain_string;
	ChainString *def_break_point_chain_string;
	ChainString *def_buffer_chain_string;
	ChainString *def_concat_chain_string;
	ChainString *def_concat_res_chain_string;
	ChainString *def_cond_ref_of_chain_string;
	ChainString *def_continue_chain_string;
	ChainString *def_copy_object_chain_string;
	ChainString *def_decrement_chain_string;
	ChainString *def_deref_of_chain_string;
	ChainString *def_divide_chain_string;
	ChainString *def_fatal_chain_string;
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
	ChainString *def_or_chain_string;
	ChainString *def_package_chain_string;
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
	ChainString *dual_name_path_chain_string;
	ChainString *expression_opcode_chain_string;
	ChainString *lead_name_char_chain_string;
	ChainString *method_invocation_chain_string;
	ChainString *multi_name_path_chain_string;
	ChainString *name_chars_chain_string[3];
	ChainString *name_path_chain_string;
	ChainString *name_seg_chain_string;
	ChainString *name_space_modifier_obj_chain_string;
	ChainString *name_strings_chain_string[2];
	ChainString *named_obj_chain_string;
	ChainString *null_name_chain_string;
	ChainString *object_chain_string;
	ChainString *output;
	ChainString *parent_prefix_char_chain_string;
	ChainString *prefix_path_chain_string;
	ChainString *root_char_chain_string;
	ChainString *statement_opcode_chain_string;
	ChainString *term_list_chain_string;
	ChainString *term_obj_chain_string;
	char *alias_op_char_array;
	char *def_add_char_array;
	char *def_alias_char_array;
	char *def_and_char_array;
	char *def_aquire_char_array;
	char *def_break_char_array;
	char *def_break_point_char_array;
	char *def_buffer_char_array;
	char *def_concat_char_array;
	char *def_concat_res_char_array;
	char *def_cond_ref_of_char_array;
	char *def_continue_char_array;
	char *def_copy_object_char_array;
	char *def_decrement_char_array;
	char *def_deref_of_char_array;
	char *def_divide_char_array;
	char *def_fatal_char_array;
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
	char *def_or_char_array;
	char *def_package_char_array;
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
	char *dual_name_path_char_array;
	char *expression_opcode_char_array;
	char *lead_name_char_char_array;
	char *method_invocation_char_array;
	char *multi_name_path_char_array;
	char *name_chars_char_array[3];
	char *name_path_char_array;
	char *name_seg_char_array;
	char *name_space_modifier_obj_char_array;
	char *name_strings_char_array[2];
	char *named_obj_char_array;
	char *null_name_char_array;
	char *object_char_array;
	char *parent_prefix_char_char_array;
	char *prefix_path_char_array;
	char *root_char_char_array;
	char *statement_opcode_char_array;
	char *term_list_char_array;
	char *term_obj_char_array;
	switch(aml_symbol->type)
	{
	case aml_alias_op:
		output = create_chain_string(aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_def_alias:
		if(aml_symbol->component.def_alias.alias_op)
		{
			alias_op_chain_string = aml_symbol_to_chain_string(aml_symbol->component.def_alias.alias_op);
			insert_char_front(alias_op_chain_string, alias_op_chain_string->first_character, ' ');
			replace_chain_string(alias_op_chain_string, "\n", " \n");
			alias_op_char_array = create_char_array_from_chain_string(alias_op_chain_string);
		}
		else alias_op_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_alias.name_string); i++)if(aml_symbol->component.def_alias.name_string[i])
		{
			name_strings_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.def_alias.name_string[i]);
			insert_char_front(name_strings_chain_string[i], name_strings_chain_string[i]->first_character, ' ');
			replace_chain_string(name_strings_chain_string[i], "\n", " \n");
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
		break;
	case aml_expression_opcode:
		if(aml_symbol->component.expression_opcode.def_add)
		{
			def_add_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_add);
			insert_char_front(def_add_chain_string, def_add_chain_string->first_character, ' ');
			replace_chain_string(def_add_chain_string, "\n", " \n");
			def_add_char_array = create_char_array_from_chain_string(def_add_chain_string);
		}
		else def_add_char_array = "";
		if(aml_symbol->component.expression_opcode.def_and)
		{
			def_and_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_and);
			insert_char_front(def_and_chain_string, def_and_chain_string->first_character, ' ');
			replace_chain_string(def_and_chain_string, "\n", " \n");
			def_and_char_array = create_char_array_from_chain_string(def_and_chain_string);
		}
		else def_and_char_array = "";
		if(aml_symbol->component.expression_opcode.def_aquire)
		{
			def_aquire_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_aquire);
			insert_char_front(def_aquire_chain_string, def_aquire_chain_string->first_character, ' ');
			replace_chain_string(def_aquire_chain_string, "\n", " \n");
			def_aquire_char_array = create_char_array_from_chain_string(def_aquire_chain_string);
		}
		else def_aquire_char_array = "";
		if(aml_symbol->component.expression_opcode.def_buffer)
		{
			def_buffer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_buffer);
			insert_char_front(def_buffer_chain_string, def_buffer_chain_string->first_character, ' ');
			replace_chain_string(def_buffer_chain_string, "\n", " \n");
			def_buffer_char_array = create_char_array_from_chain_string(def_buffer_chain_string);
		}
		else def_buffer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_concat)
		{
			def_concat_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_concat);
			insert_char_front(def_concat_chain_string, def_concat_chain_string->first_character, ' ');
			replace_chain_string(def_concat_chain_string, "\n", " \n");
			def_concat_char_array = create_char_array_from_chain_string(def_concat_chain_string);
		}
		else def_concat_char_array = "";
		if(aml_symbol->component.expression_opcode.def_concat_res)
		{
			def_concat_res_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_concat_res);
			insert_char_front(def_concat_res_chain_string, def_concat_res_chain_string->first_character, ' ');
			replace_chain_string(def_concat_res_chain_string, "\n", " \n");
			def_concat_res_char_array = create_char_array_from_chain_string(def_concat_res_chain_string);
		}
		else def_concat_res_char_array = "";
		if(aml_symbol->component.expression_opcode.def_cond_ref_of)
		{
			def_cond_ref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_cond_ref_of);
			insert_char_front(def_cond_ref_of_chain_string, def_cond_ref_of_chain_string->first_character, ' ');
			replace_chain_string(def_cond_ref_of_chain_string, "\n", " \n");
			def_cond_ref_of_char_array = create_char_array_from_chain_string(def_cond_ref_of_chain_string);
		}
		else def_cond_ref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_copy_object)
		{
			def_copy_object_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_copy_object);
			insert_char_front(def_copy_object_chain_string, def_copy_object_chain_string->first_character, ' ');
			replace_chain_string(def_copy_object_chain_string, "\n", " \n");
			def_copy_object_char_array = create_char_array_from_chain_string(def_copy_object_chain_string);
		}
		else def_copy_object_char_array = "";
		if(aml_symbol->component.expression_opcode.def_decrement)
		{
			def_decrement_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_decrement);
			insert_char_front(def_decrement_chain_string, def_decrement_chain_string->first_character, ' ');
			replace_chain_string(def_decrement_chain_string, "\n", " \n");
			def_decrement_char_array = create_char_array_from_chain_string(def_decrement_chain_string);
		}
		else def_decrement_char_array = "";
		if(aml_symbol->component.expression_opcode.def_deref_of)
		{
			def_deref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_deref_of);
			insert_char_front(def_deref_of_chain_string, def_deref_of_chain_string->first_character, ' ');
			replace_chain_string(def_deref_of_chain_string, "\n", " \n");
			def_deref_of_char_array = create_char_array_from_chain_string(def_deref_of_chain_string);
		}
		else def_deref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_divide)
		{
			def_divide_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_divide);
			insert_char_front(def_divide_chain_string, def_divide_chain_string->first_character, ' ');
			replace_chain_string(def_divide_chain_string, "\n", " \n");
			def_divide_char_array = create_char_array_from_chain_string(def_divide_chain_string);
		}
		else def_divide_char_array = "";
		if(aml_symbol->component.expression_opcode.def_find_set_left_bit)
		{
			def_find_set_left_bit_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_find_set_left_bit);
			insert_char_front(def_find_set_left_bit_chain_string, def_find_set_left_bit_chain_string->first_character, ' ');
			replace_chain_string(def_find_set_left_bit_chain_string, "\n", " \n");
			def_find_set_left_bit_char_array = create_char_array_from_chain_string(def_find_set_left_bit_chain_string);
		}
		else def_find_set_left_bit_char_array = "";
		if(aml_symbol->component.expression_opcode.def_find_set_right_bit)
		{
			def_find_set_right_bit_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_find_set_right_bit);
			insert_char_front(def_find_set_right_bit_chain_string, def_find_set_right_bit_chain_string->first_character, ' ');
			replace_chain_string(def_find_set_right_bit_chain_string, "\n", " \n");
			def_find_set_right_bit_char_array = create_char_array_from_chain_string(def_find_set_right_bit_chain_string);
		}
		else def_find_set_right_bit_char_array = "";
		if(aml_symbol->component.expression_opcode.def_from_bcd)
		{
			def_from_bcd_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_from_bcd);
			insert_char_front(def_from_bcd_chain_string, def_from_bcd_chain_string->first_character, ' ');
			replace_chain_string(def_from_bcd_chain_string, "\n", " \n");
			def_from_bcd_char_array = create_char_array_from_chain_string(def_from_bcd_chain_string);
		}
		else def_from_bcd_char_array = "";
		if(aml_symbol->component.expression_opcode.def_increment)
		{
			def_increment_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_increment);
			insert_char_front(def_increment_chain_string, def_increment_chain_string->first_character, ' ');
			replace_chain_string(def_increment_chain_string, "\n", " \n");
			def_increment_char_array = create_char_array_from_chain_string(def_increment_chain_string);
		}
		else def_increment_char_array = "";
		if(aml_symbol->component.expression_opcode.def_index)
		{
			def_index_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_index);
			insert_char_front(def_index_chain_string, def_index_chain_string->first_character, ' ');
			replace_chain_string(def_index_chain_string, "\n", " \n");
			def_index_char_array = create_char_array_from_chain_string(def_index_chain_string);
		}
		else def_index_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_and)
		{
			def_l_and_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_and);
			insert_char_front(def_l_and_chain_string, def_l_and_chain_string->first_character, ' ');
			replace_chain_string(def_l_and_chain_string, "\n", " \n");
			def_l_and_char_array = create_char_array_from_chain_string(def_l_and_chain_string);
		}
		else def_l_and_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_equal)
		{
			def_l_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_equal);
			insert_char_front(def_l_equal_chain_string, def_l_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_equal_chain_string, "\n", " \n");
			def_l_equal_char_array = create_char_array_from_chain_string(def_l_equal_chain_string);
		}
		else def_l_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_greater)
		{
			def_l_greater_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_greater);
			insert_char_front(def_l_greater_chain_string, def_l_greater_chain_string->first_character, ' ');
			replace_chain_string(def_l_greater_chain_string, "\n", " \n");
			def_l_greater_char_array = create_char_array_from_chain_string(def_l_greater_chain_string);
		}
		else def_l_greater_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_greater_equal)
		{
			def_l_greater_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_greater_equal);
			insert_char_front(def_l_greater_equal_chain_string, def_l_greater_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_greater_equal_chain_string, "\n", " \n");
			def_l_greater_equal_char_array = create_char_array_from_chain_string(def_l_greater_equal_chain_string);
		}
		else def_l_greater_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_less)
		{
			def_l_less_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_less);
			insert_char_front(def_l_less_chain_string, def_l_less_chain_string->first_character, ' ');
			replace_chain_string(def_l_less_chain_string, "\n", " \n");
			def_l_less_char_array = create_char_array_from_chain_string(def_l_less_chain_string);
		}
		else def_l_less_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_less_equal)
		{
			def_l_less_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_less_equal);
			insert_char_front(def_l_less_equal_chain_string, def_l_less_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_less_equal_chain_string, "\n", " \n");
			def_l_less_equal_char_array = create_char_array_from_chain_string(def_l_less_equal_chain_string);
		}
		else def_l_less_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_not)
		{
			def_l_not_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_not);
			insert_char_front(def_l_not_chain_string, def_l_not_chain_string->first_character, ' ');
			replace_chain_string(def_l_not_chain_string, "\n", " \n");
			def_l_not_char_array = create_char_array_from_chain_string(def_l_not_chain_string);
		}
		else def_l_not_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_not_equal)
		{
			def_l_not_equal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_not_equal);
			insert_char_front(def_l_not_equal_chain_string, def_l_not_equal_chain_string->first_character, ' ');
			replace_chain_string(def_l_not_equal_chain_string, "\n", " \n");
			def_l_not_equal_char_array = create_char_array_from_chain_string(def_l_not_equal_chain_string);
		}
		else def_l_not_equal_char_array = "";
		if(aml_symbol->component.expression_opcode.def_l_or)
		{
			def_l_or_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_l_or);
			insert_char_front(def_l_or_chain_string, def_l_or_chain_string->first_character, ' ');
			replace_chain_string(def_l_or_chain_string, "\n", " \n");
			def_l_or_char_array = create_char_array_from_chain_string(def_l_or_chain_string);
		}
		else def_l_or_char_array = "";
		if(aml_symbol->component.expression_opcode.def_load_table)
		{
			def_load_table_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_load_table);
			insert_char_front(def_load_table_chain_string, def_load_table_chain_string->first_character, ' ');
			replace_chain_string(def_load_table_chain_string, "\n", " \n");
			def_load_table_char_array = create_char_array_from_chain_string(def_load_table_chain_string);
		}
		else def_load_table_char_array = "";
		if(aml_symbol->component.expression_opcode.def_match)
		{
			def_match_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_match);
			insert_char_front(def_match_chain_string, def_match_chain_string->first_character, ' ');
			replace_chain_string(def_match_chain_string, "\n", " \n");
			def_match_char_array = create_char_array_from_chain_string(def_match_chain_string);
		}
		else def_match_char_array = "";
		if(aml_symbol->component.expression_opcode.def_mid)
		{
			def_mid_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_mid);
			insert_char_front(def_mid_chain_string, def_mid_chain_string->first_character, ' ');
			replace_chain_string(def_mid_chain_string, "\n", " \n");
			def_mid_char_array = create_char_array_from_chain_string(def_mid_chain_string);
		}
		else def_mid_char_array = "";
		if(aml_symbol->component.expression_opcode.def_mod)
		{
			def_mod_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_mod);
			insert_char_front(def_mod_chain_string, def_mod_chain_string->first_character, ' ');
			replace_chain_string(def_mod_chain_string, "\n", " \n");
			def_mod_char_array = create_char_array_from_chain_string(def_mod_chain_string);
		}
		else def_mod_char_array = "";
		if(aml_symbol->component.expression_opcode.def_multiply)
		{
			def_multiply_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_multiply);
			insert_char_front(def_multiply_chain_string, def_multiply_chain_string->first_character, ' ');
			replace_chain_string(def_multiply_chain_string, "\n", " \n");
			def_multiply_char_array = create_char_array_from_chain_string(def_multiply_chain_string);
		}
		else def_multiply_char_array = "";
		if(aml_symbol->component.expression_opcode.def_nand)
		{
			def_nand_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_nand);
			insert_char_front(def_nand_chain_string, def_nand_chain_string->first_character, ' ');
			replace_chain_string(def_nand_chain_string, "\n", " \n");
			def_nand_char_array = create_char_array_from_chain_string(def_nand_chain_string);
		}
		else def_nand_char_array = "";
		if(aml_symbol->component.expression_opcode.def_nor)
		{
			def_nor_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_nor);
			insert_char_front(def_nor_chain_string, def_nor_chain_string->first_character, ' ');
			replace_chain_string(def_nor_chain_string, "\n", " \n");
			def_nor_char_array = create_char_array_from_chain_string(def_nor_chain_string);
		}
		else def_nor_char_array = "";
		if(aml_symbol->component.expression_opcode.def_not)
		{
			def_not_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_not);
			insert_char_front(def_not_chain_string, def_not_chain_string->first_character, ' ');
			replace_chain_string(def_not_chain_string, "\n", " \n");
			def_not_char_array = create_char_array_from_chain_string(def_not_chain_string);
		}
		else def_not_char_array = "";
		if(aml_symbol->component.expression_opcode.def_object_type)
		{
			def_object_type_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_object_type);
			insert_char_front(def_object_type_chain_string, def_object_type_chain_string->first_character, ' ');
			replace_chain_string(def_object_type_chain_string, "\n", " \n");
			def_object_type_char_array = create_char_array_from_chain_string(def_object_type_chain_string);
		}
		else def_object_type_char_array = "";
		if(aml_symbol->component.expression_opcode.def_or)
		{
			def_or_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_or);
			insert_char_front(def_or_chain_string, def_or_chain_string->first_character, ' ');
			replace_chain_string(def_or_chain_string, "\n", " \n");
			def_or_char_array = create_char_array_from_chain_string(def_or_chain_string);
		}
		else def_or_char_array = "";
		if(aml_symbol->component.expression_opcode.def_package)
		{
			def_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_package);
			insert_char_front(def_package_chain_string, def_package_chain_string->first_character, ' ');
			replace_chain_string(def_package_chain_string, "\n", " \n");
			def_package_char_array = create_char_array_from_chain_string(def_package_chain_string);
		}
		else def_package_char_array = "";
		if(aml_symbol->component.expression_opcode.def_ref_of)
		{
			def_ref_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_ref_of);
			insert_char_front(def_ref_of_chain_string, def_ref_of_chain_string->first_character, ' ');
			replace_chain_string(def_ref_of_chain_string, "\n", " \n");
			def_ref_of_char_array = create_char_array_from_chain_string(def_ref_of_chain_string);
		}
		else def_ref_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_shift_left)
		{
			def_shift_left_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_shift_left);
			insert_char_front(def_shift_left_chain_string, def_shift_left_chain_string->first_character, ' ');
			replace_chain_string(def_shift_left_chain_string, "\n", " \n");
			def_shift_left_char_array = create_char_array_from_chain_string(def_shift_left_chain_string);
		}
		else def_shift_left_char_array = "";
		if(aml_symbol->component.expression_opcode.def_shift_right)
		{
			def_shift_right_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_shift_right);
			insert_char_front(def_shift_right_chain_string, def_shift_right_chain_string->first_character, ' ');
			replace_chain_string(def_shift_right_chain_string, "\n", " \n");
			def_shift_right_char_array = create_char_array_from_chain_string(def_shift_right_chain_string);
		}
		else def_shift_right_char_array = "";
		if(aml_symbol->component.expression_opcode.def_size_of)
		{
			def_size_of_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_size_of);
			insert_char_front(def_size_of_chain_string, def_size_of_chain_string->first_character, ' ');
			replace_chain_string(def_size_of_chain_string, "\n", " \n");
			def_size_of_char_array = create_char_array_from_chain_string(def_size_of_chain_string);
		}
		else def_size_of_char_array = "";
		if(aml_symbol->component.expression_opcode.def_store)
		{
			def_store_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_store);
			insert_char_front(def_store_chain_string, def_store_chain_string->first_character, ' ');
			replace_chain_string(def_store_chain_string, "\n", " \n");
			def_store_char_array = create_char_array_from_chain_string(def_store_chain_string);
		}
		else def_store_char_array = "";
		if(aml_symbol->component.expression_opcode.def_subtract)
		{
			def_subtract_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_subtract);
			insert_char_front(def_subtract_chain_string, def_subtract_chain_string->first_character, ' ');
			replace_chain_string(def_subtract_chain_string, "\n", " \n");
			def_subtract_char_array = create_char_array_from_chain_string(def_subtract_chain_string);
		}
		else def_subtract_char_array = "";
		if(aml_symbol->component.expression_opcode.def_timer)
		{
			def_timer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_timer);
			insert_char_front(def_timer_chain_string, def_timer_chain_string->first_character, ' ');
			replace_chain_string(def_timer_chain_string, "\n", " \n");
			def_timer_char_array = create_char_array_from_chain_string(def_timer_chain_string);
		}
		else def_timer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_bcd)
		{
			def_to_bcd_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_bcd);
			insert_char_front(def_to_bcd_chain_string, def_to_bcd_chain_string->first_character, ' ');
			replace_chain_string(def_to_bcd_chain_string, "\n", " \n");
			def_to_bcd_char_array = create_char_array_from_chain_string(def_to_bcd_chain_string);
		}
		else def_to_bcd_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_buffer)
		{
			def_to_buffer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_buffer);
			insert_char_front(def_to_buffer_chain_string, def_to_buffer_chain_string->first_character, ' ');
			replace_chain_string(def_to_buffer_chain_string, "\n", " \n");
			def_to_buffer_char_array = create_char_array_from_chain_string(def_to_buffer_chain_string);
		}
		else def_to_buffer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_decimal_string)
		{
			def_to_decimal_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_decimal_string);
			insert_char_front(def_to_decimal_string_chain_string, def_to_decimal_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_decimal_string_chain_string, "\n", " \n");
			def_to_decimal_string_char_array = create_char_array_from_chain_string(def_to_decimal_string_chain_string);
		}
		else def_to_decimal_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_hex_string)
		{
			def_to_hex_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_hex_string);
			insert_char_front(def_to_hex_string_chain_string, def_to_hex_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_hex_string_chain_string, "\n", " \n");
			def_to_hex_string_char_array = create_char_array_from_chain_string(def_to_hex_string_chain_string);
		}
		else def_to_hex_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_integer)
		{
			def_to_integer_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_integer);
			insert_char_front(def_to_integer_chain_string, def_to_integer_chain_string->first_character, ' ');
			replace_chain_string(def_to_integer_chain_string, "\n", " \n");
			def_to_integer_char_array = create_char_array_from_chain_string(def_to_integer_chain_string);
		}
		else def_to_integer_char_array = "";
		if(aml_symbol->component.expression_opcode.def_to_string)
		{
			def_to_string_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_to_string);
			insert_char_front(def_to_string_chain_string, def_to_string_chain_string->first_character, ' ');
			replace_chain_string(def_to_string_chain_string, "\n", " \n");
			def_to_string_char_array = create_char_array_from_chain_string(def_to_string_chain_string);
		}
		else def_to_string_char_array = "";
		if(aml_symbol->component.expression_opcode.def_var_package)
		{
			def_var_package_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_var_package);
			insert_char_front(def_var_package_chain_string, def_var_package_chain_string->first_character, ' ');
			replace_chain_string(def_var_package_chain_string, "\n", " \n");
			def_var_package_char_array = create_char_array_from_chain_string(def_var_package_chain_string);
		}
		else def_var_package_char_array = "";
		if(aml_symbol->component.expression_opcode.def_wait)
		{
			def_wait_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_wait);
			insert_char_front(def_wait_chain_string, def_wait_chain_string->first_character, ' ');
			replace_chain_string(def_wait_chain_string, "\n", " \n");
			def_wait_char_array = create_char_array_from_chain_string(def_wait_chain_string);
		}
		else def_wait_char_array = "";
		if(aml_symbol->component.expression_opcode.def_xor)
		{
			def_xor_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.def_xor);
			insert_char_front(def_xor_chain_string, def_xor_chain_string->first_character, ' ');
			replace_chain_string(def_xor_chain_string, "\n", " \n");
			def_xor_char_array = create_char_array_from_chain_string(def_xor_chain_string);
		}
		else def_xor_char_array = "";
		if(aml_symbol->component.expression_opcode.method_invocation)
		{
			method_invocation_chain_string = aml_symbol_to_chain_string(aml_symbol->component.expression_opcode.method_invocation);
			insert_char_front(method_invocation_chain_string, method_invocation_chain_string->first_character, ' ');
			replace_chain_string(method_invocation_chain_string, "\n", " \n");
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
	case aml_name_path:
		if(aml_symbol->component.name_path.name_seg)
		{
			name_seg_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.name_seg);
			insert_char_front(name_seg_chain_string, name_seg_chain_string->first_character, ' ');
			replace_chain_string(name_seg_chain_string, "\n", " \n");
			name_seg_char_array = create_char_array_from_chain_string(name_seg_chain_string);
		}
		else name_seg_char_array = "";
		if(aml_symbol->component.name_path.dual_name_path)
		{
			dual_name_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.dual_name_path);
			insert_char_front(dual_name_path_chain_string, dual_name_path_chain_string->first_character, ' ');
			replace_chain_string(dual_name_path_chain_string, "\n", " \n");
			dual_name_path_char_array = create_char_array_from_chain_string(dual_name_path_chain_string);
		}
		else dual_name_path_char_array = "";
		if(aml_symbol->component.name_path.multi_name_path)
		{
			multi_name_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.multi_name_path);
			insert_char_front(multi_name_path_chain_string, multi_name_path_chain_string->first_character, ' ');
			replace_chain_string(multi_name_path_chain_string, "\n", " \n");
			multi_name_path_char_array = create_char_array_from_chain_string(multi_name_path_chain_string);
		}
		else multi_name_path_char_array = "";
		if(aml_symbol->component.name_path.null_name)
		{
			null_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_path.null_name);
			insert_char_front(null_name_chain_string, null_name_chain_string->first_character, ' ');
			replace_chain_string(null_name_chain_string, "\n", " \n");
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
		if(aml_symbol->component.name_seg.lead_name_char)
		{
			lead_name_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_seg.lead_name_char);
			insert_char_front(lead_name_char_chain_string, lead_name_char_chain_string->first_character, ' ');
			replace_chain_string(lead_name_char_chain_string, "\n", " \n");
			lead_name_char_char_array = create_char_array_from_chain_string(lead_name_char_chain_string);
		}
		else lead_name_char_char_array = "";
		for(unsigned int i = 0; i < _countof(aml_symbol->component.name_seg.name_char); i++)if(aml_symbol->component.name_seg.name_char[i])
		{
			name_chars_chain_string[i] = aml_symbol_to_chain_string(aml_symbol->component.name_seg.name_char[i]);
			insert_char_front(name_chars_chain_string[i], name_chars_chain_string[i]->first_character, ' ');
			replace_chain_string(name_chars_chain_string[i], "\n", " \n");
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
		break;
	case aml_name_space_modifier_obj:
		if(aml_symbol->component.name_space_modifier_obj.def_alias)
		{
			def_alias_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_alias);
			insert_char_front(def_alias_chain_string, def_alias_chain_string->first_character, ' ');
			replace_chain_string(def_alias_chain_string, "\n", " \n");
			def_alias_char_array = create_char_array_from_chain_string(def_alias_chain_string);
		}
		else def_alias_char_array = "";
		if(aml_symbol->component.name_space_modifier_obj.def_name)
		{
			def_name_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_name);
			insert_char_front(def_name_chain_string, def_name_chain_string->first_character, ' ');
			replace_chain_string(def_name_chain_string, "\n", " \n");
			def_name_char_array = create_char_array_from_chain_string(def_name_chain_string);
		}
		else def_name_char_array = "";
		if(aml_symbol->component.name_space_modifier_obj.def_scope)
		{
			def_scope_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_space_modifier_obj.def_scope);
			insert_char_front(def_scope_chain_string, def_scope_chain_string->first_character, ' ');
			replace_chain_string(def_scope_chain_string, "\n", " \n");
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
			replace_chain_string(name_path_chain_string, "\n", " \n");
			name_path_char_array = create_char_array_from_chain_string(name_path_chain_string);
		}
		else name_path_char_array = "";
		if(aml_symbol->component.name_string.prefix_path)
		{
			prefix_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_string.prefix_path);
			insert_char_front(prefix_path_chain_string, prefix_path_chain_string->first_character, ' ');
			replace_chain_string(prefix_path_chain_string, "\n", " \n");
			prefix_path_char_array = create_char_array_from_chain_string(prefix_path_chain_string);
		}
		else prefix_path_char_array = "";
		if(aml_symbol->component.name_string.root_char)
		{
			root_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.name_string.root_char);
			insert_char_front(root_char_chain_string, root_char_chain_string->first_character, ' ');
			replace_chain_string(root_char_chain_string, "\n", " \n");
			root_char_char_array = create_char_array_from_chain_string(root_char_chain_string);
		}
		else root_char_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", root_char_char_array, prefix_path_char_array, name_path_char_array);
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
	case aml_object:
		if(aml_symbol->component.object.named_obj)
		{
			named_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.object.named_obj);
			insert_char_front(named_obj_chain_string, named_obj_chain_string->first_character, ' ');
			replace_chain_string(named_obj_chain_string, "\n", " \n");
			named_obj_char_array = create_char_array_from_chain_string(named_obj_chain_string);
		}
		else named_obj_char_array = "";
		if(aml_symbol->component.object.name_space_modifier_obj)
		{
			name_space_modifier_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.object.name_space_modifier_obj);
			insert_char_front(name_space_modifier_obj_chain_string, name_space_modifier_obj_chain_string->first_character, ' ');
			replace_chain_string(name_space_modifier_obj_chain_string, "\n", " \n");
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
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)
		{
			parent_prefix_char_chain_string = aml_symbol_to_chain_string(aml_symbol->component.prefix_path.parent_prefix_char);
			insert_char_front(parent_prefix_char_chain_string, parent_prefix_char_chain_string->first_character, ' ');
			replace_chain_string(parent_prefix_char_chain_string, "\n", " \n");
			parent_prefix_char_char_array = create_char_array_from_chain_string(parent_prefix_char_chain_string);
		}
		else parent_prefix_char_char_array = "";
		if(aml_symbol->component.prefix_path.prefix_path)
		{
			prefix_path_chain_string = aml_symbol_to_chain_string(aml_symbol->component.prefix_path.prefix_path);
			insert_char_front(prefix_path_chain_string, prefix_path_chain_string->first_character, ' ');
			replace_chain_string(prefix_path_chain_string, "\n", " \n");
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
	case aml_root_char:
		output = create_chain_string(aml_symbol_type_name(aml_symbol->type));
		break;
	case aml_statement_opcode:
		if(aml_symbol->component.statement_opcode.def_break)
		{
			def_break_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_break);
			insert_char_front(def_break_chain_string, def_break_chain_string->first_character, ' ');
			replace_chain_string(def_break_chain_string, "\n", " \n");
			def_break_char_array = create_char_array_from_chain_string(def_break_chain_string);
		}
		else def_break_char_array = "";
		if(aml_symbol->component.statement_opcode.def_break_point)
		{
			def_break_point_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_break_point);
			insert_char_front(def_break_point_chain_string, def_break_point_chain_string->first_character, ' ');
			replace_chain_string(def_break_point_chain_string, "\n", " \n");
			def_break_point_char_array = create_char_array_from_chain_string(def_break_point_chain_string);
		}
		else def_break_point_char_array = "";
		if(aml_symbol->component.statement_opcode.def_continue)
		{
			def_continue_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_continue);
			insert_char_front(def_continue_chain_string, def_continue_chain_string->first_character, ' ');
			replace_chain_string(def_continue_chain_string, "\n", " \n");
			def_continue_char_array = create_char_array_from_chain_string(def_continue_chain_string);
		}
		else def_continue_char_array = "";
		if(aml_symbol->component.statement_opcode.def_fatal)
		{
			def_fatal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_fatal);
			insert_char_front(def_fatal_chain_string, def_fatal_chain_string->first_character, ' ');
			replace_chain_string(def_fatal_chain_string, "\n", " \n");
			def_fatal_char_array = create_char_array_from_chain_string(def_fatal_chain_string);
		}
		else def_fatal_char_array = "";
		if(aml_symbol->component.statement_opcode.def_if_else)
		{
			def_if_else_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_if_else);
			insert_char_front(def_if_else_chain_string, def_if_else_chain_string->first_character, ' ');
			replace_chain_string(def_if_else_chain_string, "\n", " \n");
			def_if_else_char_array = create_char_array_from_chain_string(def_if_else_chain_string);
		}
		else def_if_else_char_array = "";
		if(aml_symbol->component.statement_opcode.def_noop)
		{
			def_noop_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_noop);
			insert_char_front(def_noop_chain_string, def_noop_chain_string->first_character, ' ');
			replace_chain_string(def_noop_chain_string, "\n", " \n");
			def_noop_char_array = create_char_array_from_chain_string(def_noop_chain_string);
		}
		else def_noop_char_array = "";
		if(aml_symbol->component.statement_opcode.def_notify)
		{
			def_notify_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_notify);
			insert_char_front(def_notify_chain_string, def_notify_chain_string->first_character, ' ');
			replace_chain_string(def_notify_chain_string, "\n", " \n");
			def_notify_char_array = create_char_array_from_chain_string(def_notify_chain_string);
		}
		else def_notify_char_array = "";
		if(aml_symbol->component.statement_opcode.def_release)
		{
			def_release_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_release);
			insert_char_front(def_release_chain_string, def_release_chain_string->first_character, ' ');
			replace_chain_string(def_release_chain_string, "\n", " \n");
			def_release_char_array = create_char_array_from_chain_string(def_release_chain_string);
		}
		else def_release_char_array = "";
		if(aml_symbol->component.statement_opcode.def_reset)
		{
			def_reset_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_reset);
			insert_char_front(def_reset_chain_string, def_reset_chain_string->first_character, ' ');
			replace_chain_string(def_reset_chain_string, "\n", " \n");
			def_reset_char_array = create_char_array_from_chain_string(def_reset_chain_string);
		}
		else def_reset_char_array = "";
		if(aml_symbol->component.statement_opcode.def_return)
		{
			def_return_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_return);
			insert_char_front(def_return_chain_string, def_return_chain_string->first_character, ' ');
			replace_chain_string(def_return_chain_string, "\n", " \n");
			def_return_char_array = create_char_array_from_chain_string(def_return_chain_string);
		}
		else def_return_char_array = "";
		if(aml_symbol->component.statement_opcode.def_signal)
		{
			def_signal_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_signal);
			insert_char_front(def_signal_chain_string, def_signal_chain_string->first_character, ' ');
			replace_chain_string(def_signal_chain_string, "\n", " \n");
			def_signal_char_array = create_char_array_from_chain_string(def_signal_chain_string);
		}
		else def_signal_char_array = "";
		if(aml_symbol->component.statement_opcode.def_sleep)
		{
			def_sleep_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_sleep);
			insert_char_front(def_sleep_chain_string, def_sleep_chain_string->first_character, ' ');
			replace_chain_string(def_sleep_chain_string, "\n", " \n");
			def_sleep_char_array = create_char_array_from_chain_string(def_sleep_chain_string);
		}
		else def_sleep_char_array = "";
		if(aml_symbol->component.statement_opcode.def_stall)
		{
			def_stall_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_stall);
			insert_char_front(def_stall_chain_string, def_stall_chain_string->first_character, ' ');
			replace_chain_string(def_stall_chain_string, "\n", " \n");
			def_stall_char_array = create_char_array_from_chain_string(def_stall_chain_string);
		}
		else def_stall_char_array = "";
		if(aml_symbol->component.statement_opcode.def_while)
		{
			def_while_chain_string = aml_symbol_to_chain_string(aml_symbol->component.statement_opcode.def_while);
			insert_char_front(def_while_chain_string, def_while_chain_string->first_character, ' ');
			replace_chain_string(def_while_chain_string, "\n", " \n");
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
	case aml_term_list:
		if(aml_symbol->component.term_list.term_obj)
		{
			term_obj_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_list.term_obj);
			insert_char_front(term_obj_chain_string, term_obj_chain_string->first_character, ' ');
			replace_chain_string(term_obj_chain_string, "\n", " \n");
			term_obj_char_array = create_char_array_from_chain_string(term_obj_chain_string);
		}
		else term_obj_char_array = "";
		if(aml_symbol->component.term_list.term_list)
		{
			term_list_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_list.term_list);
			insert_char_front(term_list_chain_string, term_list_chain_string->first_character, ' ');
			replace_chain_string(term_list_chain_string, "\n", " \n");
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
			replace_chain_string(object_chain_string, "\n", " \n");
			object_char_array = create_char_array_from_chain_string(object_chain_string);
		}
		else object_char_array = "";
		if(aml_symbol->component.term_obj.statement_opcode)
		{
			statement_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_obj.statement_opcode);
			insert_char_front(statement_opcode_chain_string, statement_opcode_chain_string->first_character, ' ');
			replace_chain_string(statement_opcode_chain_string, "\n", " \n");
			statement_opcode_char_array = create_char_array_from_chain_string(statement_opcode_chain_string);
		}
		else statement_opcode_char_array = "";
		if(aml_symbol->component.term_obj.expression_opcode)
		{
			expression_opcode_chain_string = aml_symbol_to_chain_string(aml_symbol->component.term_obj.expression_opcode);
			insert_char_front(expression_opcode_chain_string, expression_opcode_chain_string->first_character, ' ');
			replace_chain_string(expression_opcode_chain_string, "\n", " \n");
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
	static char const * const aml_def_alias_name = "DefAlias";
	static char const * const aml_expression_opcode_name = "ExpressionOpcode";
	static char const * const aml_name_path_name = "NamePath";
	static char const * const aml_name_seg_name = "NameSeg";
	static char const * const aml_name_space_modifier_obj_name = "NameSpaceModifierObj";
	static char const * const aml_object_name = "Object";
	static char const * const aml_parent_prefix_char_name = "ParentPrefixChar";
	static char const * const aml_prefix_path_name = "PrefixPath";
	static char const * const aml_root_char_name = "RootChar";
	static char const * const aml_statement_opcode_name = "StatementOpcode";
	static char const * const aml_term_list_name = "TermList";
	static char const * const aml_term_obj_name = "TermObj";
	switch(aml_symbol_type)
	{
	case aml_alias_op:
		return aml_alias_op_name;
	case aml_def_alias:
		return aml_def_alias_name;
	case aml_expression_opcode:
		return aml_expression_opcode_name;
	case aml_name_path:
		return aml_name_path_name;
	case aml_name_seg:
		return aml_name_seg_name;
	case aml_name_space_modifier_obj:
		return aml_name_space_modifier_obj_name;
	case aml_object:
		return aml_object_name;
	case aml_parent_prefix_char:
		return aml_parent_prefix_char_name;
	case aml_prefix_path:
		return aml_prefix_path_name;
	case aml_root_char:
		return aml_root_char_name;
	case aml_statement_opcode:
		return aml_statement_opcode_name;
	case aml_term_list:
		return aml_term_list_name;
	case aml_term_obj:
		return aml_term_obj_name;
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

// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSubstring aml)
{
	AMLSymbol *def_alias = malloc(sizeof(*def_alias));

	AMLSubstring name_string_aml;
	def_alias->string.initial = aml.initial;
	def_alias->string.length = 0;
	def_alias->type = aml_def_alias;
	def_alias->component.def_alias.alias_op = analyse_aml_alias_op(aml);
	name_string_aml.initial = def_alias->component.def_alias.alias_op->string.initial + def_alias->component.def_alias.alias_op->string.length;
	name_string_aml.length = aml.length - def_alias->component.def_alias.alias_op->string.length;
	def_alias->string.length += def_alias->component.def_alias.alias_op->string.length;
	for(AMLSymbol **name_string = def_alias->component.def_alias.name_string; name_string != def_alias->component.def_alias.name_string + _countof(def_alias->component.def_alias.name_string); name_string++)
	{
		switch(*name_string_aml.initial)
		{
		case AML_BYTE_PARENT_PREFIX_CHAR:
		case AML_BYTE_ROOT_CHAR:
			break;
		default:
			ERROR(); // Syntax error
			break;
		}
		*name_string = analyse_aml_name_string(name_string_aml);
		name_string_aml.initial += (*name_string)->string.length;
		name_string_aml.length -= (*name_string)->string.length;
		def_alias->string.length += (*name_string)->string.length;
	}
	return def_alias;
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
	return name_path;
}

// <name_seg> := <lead_name_char> <name_char> <name_char> <name_char>
AMLSymbol *analyse_aml_name_seg(AMLSubstring aml)
{
	AMLSymbol *name_seg = malloc(sizeof(*name_seg));
	name_seg->string.initial = aml.initial;
	name_seg->string.length = 0;
	name_seg->type = aml_name_seg;
	name_seg->component.name_seg.lead_name_char = NULL;
	for(AMLSymbol **name_char = name_seg->component.name_seg.name_char; name_char != name_seg->component.name_seg.name_char + _countof(name_seg->component.name_seg.name_char); name_char++)
	{
		*name_char = NULL;
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
	case AML_BYTE_PARENT_PREFIX_CHAR:
		name_string->component.name_string.prefix_path = analyse_aml_prefix_path(aml);
		name_string->string.length += name_string->component.name_string.prefix_path->string.length;
		break;
	case AML_BYTE_ROOT_CHAR:
		name_string->component.name_string.root_char = analyse_aml_root_char(aml);
		name_string->string.length += name_string->component.name_string.root_char->string.length;
		break;
	}
	return name_string;
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
		object->component.object.name_space_modifier_obj = analyse_aml_name_space_modifier_obj(aml);
		object->string.length += object->component.object.name_space_modifier_obj->string.length;
		break;
	}
	return object;
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

// <prefix_path> := Nothing | <parent_prefix_char> <prefix_path>
AMLSymbol *analyse_aml_prefix_path(AMLSubstring aml)
{
	AMLSymbol *prefix_path = malloc(sizeof(*prefix_path));
	AMLSubstring next_prefix_path_aml;
	prefix_path->string.initial = aml.initial;
	prefix_path->string.length = 0;
	prefix_path->type = aml_prefix_path;
	prefix_path->component.prefix_path.parent_prefix_char = NULL;
	prefix_path->component.prefix_path.prefix_path = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_PARENT_PREFIX_CHAR:
		prefix_path->component.prefix_path.parent_prefix_char = analyse_aml_parent_prefix_char(aml);
		next_prefix_path_aml.initial = aml.initial + prefix_path->component.prefix_path.parent_prefix_char->string.length;
		next_prefix_path_aml.length = aml.length - prefix_path->component.prefix_path.parent_prefix_char->string.length;
		prefix_path->string.length += prefix_path->component.prefix_path.parent_prefix_char->string.length;
		prefix_path->component.prefix_path.prefix_path = analyse_aml_prefix_path(next_prefix_path_aml);
		prefix_path->string.length += prefix_path->component.prefix_path.prefix_path->string.length;
		break;
	}
	return prefix_path;
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

// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSubstring aml)
{
	AMLSymbol *term_list = malloc(sizeof(*term_list));
	term_list->string.initial = aml.initial;
	term_list->string.length = 0;
	term_list->type = aml_term_list;
	if(aml.length)
	{
		// <term_list> := <term_obj> <term_list>
		term_list->component.term_list.term_obj = analyse_aml_term_obj(aml);
		term_list->string.length += term_list->component.term_list.term_obj->string.length;
		term_list->component.term_list.term_list = NULL;
	}
	else
	{
		// <term_list> := Nothing
		term_list->component.term_list.term_list = NULL;
		term_list->component.term_list.term_obj = NULL;
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
		term_obj->component.term_obj.object = analyse_aml_object(aml);
		term_obj->string.length += term_obj->component.term_obj.object->string.length;
		break;
	}
	return term_obj;
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
	case aml_def_alias:
		if(aml_symbol->component.def_alias.alias_op)delete_aml_symbol(aml_symbol->component.def_alias.alias_op);
		for(AMLSymbol **name_string = aml_symbol->component.def_alias.name_string; name_string != aml_symbol->component.def_alias.name_string + _countof(aml_symbol->component.def_alias.name_string); name_string++)if(*name_string)delete_aml_symbol(*name_string);
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
	case aml_object:
		if(aml_symbol->component.object.named_obj)delete_aml_symbol(aml_symbol->component.object.named_obj);
		if(aml_symbol->component.object.name_space_modifier_obj)delete_aml_symbol(aml_symbol->component.object.name_space_modifier_obj);
		break;
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)delete_aml_symbol(aml_symbol->component.prefix_path.parent_prefix_char);
		if(aml_symbol->component.prefix_path.prefix_path)delete_aml_symbol(aml_symbol->component.prefix_path.prefix_path);
	case aml_root_char:
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
	case aml_term_list:
		if(aml_symbol->component.term_list.term_list)delete_aml_symbol(aml_symbol->component.term_list.term_list);
		if(aml_symbol->component.term_list.term_obj)delete_aml_symbol(aml_symbol->component.term_list.term_obj);
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object)delete_aml_symbol(aml_symbol->component.term_obj.object);
		if(aml_symbol->component.term_obj.statement_opcode)delete_aml_symbol(aml_symbol->component.term_obj.statement_opcode);
		if(aml_symbol->component.term_obj.expression_opcode)delete_aml_symbol(aml_symbol->component.term_obj.expression_opcode);
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

