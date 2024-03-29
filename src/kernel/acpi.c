#include "acpi.h"
#include "bios.h"
#include "serial.h"
#include "shell.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#define AML_BYTE_NULL_CHAR		0x00
#define AML_BYTE_NULL_NAME		0x00
#define AML_BYTE_RESERVED_FIELD_OP	0x00
#define AML_BYTE_ZERO_OP		0x00
#define AML_BYTE_ACCESS_FIELD_OP	0x01
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
#define AML_BYTE_PROCESSOR_OP		0x83
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

char const *aml_symbol_type_name(AMLSymbolType aml_symbol_type)
{
	static char const * const aml_access_attrib_name = "AccessAttrib";
	static char const * const aml_access_field_name = "AccessField";
	static char const * const aml_access_field_op_name = "AccessFieldOp";
	static char const * const aml_access_type_name = "AccessType";
	static char const * const aml_acquire_op_name = "AcquireOp";
	static char const * const aml_acquire_op_suffix_name = "AcquireOpSuffix";
	static char const * const aml_alias_op_name = "AliasOp";
	static char const * const aml_add_op_name = "AddOp";
	static char const * const aml_and_op_name = "AndOp";
	static char const * const aml_arg_obj_name = "ArgObj";
	static char const * const aml_arg_object_name = "ArgObject";
	static char const * const aml_arg_op_name = "ArgOp";
	static char const * const aml_ascii_char_name = "AsciiChar";
	static char const * const aml_ascii_char_list_name = "AsciiCharList";
	static char const * const aml_bit_index_name = "BitIndex";
	static char const * const aml_break_op_name = "BreakOp";
	static char const * const aml_buf_data_name = "BufData";
	static char const * const aml_buff_pkg_str_obj_name = "BuffPkgStrObj";
	static char const * const aml_buffer_op_name = "BufferOp";
	static char const * const aml_buffer_size_name = "BufferSize";
	static char const * const aml_byte_const_name = "ByteConst";
	static char const * const aml_byte_data_name = "ByteData";
	static char const * const aml_byte_index_name = "ByteIndex";
	static char const * const aml_byte_list_name = "ByteList";
	static char const * const aml_byte_prefix_name = "BytePrefix";
	static char const * const aml_computational_data_name = "ComputationalData";
	static char const * const aml_concat_op_name = "ConcatOp";
	static char const * const aml_concat_res_op_name = "ConcatResOp";
	static char const * const aml_cond_ref_of_op_name = "CondRefOfOp";
	static char const * const aml_cond_ref_of_op_suffix_name = "CondRefOfOpSuffix";
	static char const * const aml_const_obj_name = "ConstObj";
	static char const * const aml_create_bit_field_op_name = "CreateBitFieldOp";
	static char const * const aml_create_byte_field_op_name = "CreateByteFieldOp";
	static char const * const aml_create_dword_field_op_name = "CreateDWordFieldOp";
	static char const * const aml_create_field_op_name = "CreateFieldOp";
	static char const * const aml_create_field_op_suffix_name = "CreateFieldOpSuffix";
	static char const * const aml_create_qword_field_op_name = "CreateQWordFieldOp";
	static char const * const aml_create_word_field_op_name = "CreateWordFieldOp";
	static char const * const aml_data_name = "Data";
	static char const * const aml_data_object_name = "DataObject";
	static char const * const aml_data_ref_object_name = "DataRefObject";
	static char const * const aml_debug_obj_name = "DebugObj";
	static char const * const aml_debug_op_name = "DebugOp";
	static char const * const aml_debug_op_suffix_name = "DebugOpSuffix";
	static char const * const aml_decrement_op_name = "DecrementOp";
	static char const * const aml_def_alias_name = "DefAlias";
	static char const * const aml_def_acquire_name = "DefAcquire";
	static char const * const aml_def_add_name = "DefAdd";
	static char const * const aml_def_and_name = "DefAnd";
	static char const * const aml_def_break_name = "DefBreak";
	static char const * const aml_def_buffer_name = "DefBuffer";
	static char const * const aml_def_concat_name = "DefConcat";
	static char const * const aml_def_concat_res_name = "DefConcatRes";
	static char const * const aml_def_cond_ref_of_name = "DefCondRefOf";
	static char const * const aml_def_create_bit_field_name = "DefCreateBitField";
	static char const * const aml_def_create_byte_field_name = "DefCreateByteField";
	static char const * const aml_def_create_dword_field_name = "DefCreateDWordField";
	static char const * const aml_def_create_field_name = "DefCreateField";
	static char const * const aml_def_create_qword_field_name = "DefCreateQWordField";
	static char const * const aml_def_create_word_field_name = "DefCreateWordField";
	static char const * const aml_def_decrement_name = "DefDecrement";
	static char const * const aml_def_deref_of_name = "DefDerefOf";
	static char const * const aml_def_device_name = "DefDevice";
	static char const * const aml_def_divide_name = "DefDivide";
	static char const * const aml_def_else_name = "DefElse";
	static char const * const aml_def_field_name = "DefField";
	static char const * const aml_def_find_set_left_bit_name = "DefFindSetLeftBit";
	static char const * const aml_def_find_set_right_bit_name = "DefFindSetRightBit";
	static char const * const aml_def_if_else_name = "DefIfElse";
	static char const * const aml_def_increment_name = "DefIncrement";
	static char const * const aml_def_index_name = "DefIndex";
	static char const * const aml_def_index_field_name = "DefIndexField";
	static char const * const aml_def_l_and_name = "DefLAnd";
	static char const * const aml_def_l_equal_name = "DefLEqual";
	static char const * const aml_def_l_greater_name = "DefLGreater";
	static char const * const aml_def_l_less_name = "DefLLess";
	static char const * const aml_def_l_not_name = "DefLNot";
	static char const * const aml_def_l_or_name = "DefLOr";
	static char const * const aml_def_match_name = "DefMatch";
	static char const * const aml_def_method_name = "DefMethod";
	static char const * const aml_def_mid_name = "DefMid";
	static char const * const aml_def_multiply_name = "DefMultiply";
	static char const * const aml_def_mutex_name = "DefMutex";
	static char const * const aml_def_name_name = "DefName";
	static char const * const aml_def_not_name = "DefNot";
	static char const * const aml_def_notify_name = "DefNotify";
	static char const * const aml_def_object_type_name = "DefObjectType";
	static char const * const aml_def_op_region_name = "DefOpRegion";
	static char const * const aml_def_or_name = "DefOr";
	static char const * const aml_def_package_name = "DefPackage";
	static char const * const aml_def_processor_name = "DefProcessor";
	static char const * const aml_def_ref_of_name = "DefRefOf";
	static char const * const aml_def_release_name = "DefRelease";
	static char const * const aml_def_return_name = "DefReturn";
	static char const * const aml_def_scope_name = "DefScope";
	static char const * const aml_def_shift_left_name = "DefShiftLeft";
	static char const * const aml_def_shift_right_name = "DefShiftRight";
	static char const * const aml_def_size_of_name = "DefSizeOf";
	static char const * const aml_def_sleep_name = "DefSleep";
	static char const * const aml_def_stall_name = "DefStall";
	static char const * const aml_def_store_name = "DefStore";
	static char const * const aml_def_subtract_name = "DefSubtract";
	static char const * const aml_def_thermal_zone_name = "DefThermalZone";
	static char const * const aml_def_to_buffer_name = "DefToBuffer";
	static char const * const aml_def_to_hex_string_name = "DefToHexString";
	static char const * const aml_def_to_integer_name = "DefToInteger";
	static char const * const aml_def_while_name = "DefWhile";
	static char const * const aml_def_xor_name = "DefXor";
	static char const * const aml_deref_of_op_name = "DerefOfOp";
	static char const * const aml_device_op_name = "DeviceOp";
	static char const * const aml_device_op_suffix_name = "DeviceOpSuffix";
	static char const * const aml_digit_char_name = "DigitChar";
	static char const * const aml_divide_op_name = "DivideOp";
	static char const * const aml_dividend_name = "Dividend";
	static char const * const aml_divisor_name = "Divisor";
	static char const * const aml_dual_name_path_name = "DualNamePath";
	static char const * const aml_dual_name_prefix_name = "DualNamePrefix";
	static char const * const aml_dword_const_name = "DWordConst";
	static char const * const aml_dword_data_name = "DWordData";
	static char const * const aml_dword_prefix_name = "DWordPrefix";
	static char const * const aml_else_op_name = "ElseOp";
	static char const * const aml_expression_opcode_name = "ExpressionOpcode";
	static char const * const aml_ext_op_prefix_name = "ExtOpPrefix";
	static char const * const aml_field_element_name = "FieldElement";
	static char const * const aml_field_flags_name = "FieldFlags";
	static char const * const aml_field_list_name = "FieldList";
	static char const * const aml_field_op_name = "FieldOp";
	static char const * const aml_field_op_suffix_name = "FieldOpSuffix";
	static char const * const aml_find_set_left_bit_op_name = "FindSetLeftBitOp";
	static char const * const aml_find_set_right_bit_op_name = "FindSetRightBitOp";
	static char const * const aml_if_op_name = "IfOp";
	static char const * const aml_increment_op_name = "IncrementOp";
	static char const * const aml_index_field_op_name = "IndexFieldOp";
	static char const * const aml_index_field_op_suffix_name = "IndexFieldOpSuffix";
	static char const * const aml_index_op_name = "IndexOp";
	static char const * const aml_index_value_name = "IndexValue";
	static char const * const aml_lead_name_char_name = "LeadNameChar";
	static char const * const aml_l_and_op_name = "LAndOp";
	static char const * const aml_l_equal_op_name = "LEqualOp";
	static char const * const aml_l_greater_op_name = "LGreaterOp";
	static char const * const aml_l_less_op_name = "LLessOp";
	static char const * const aml_l_not_op_name = "LNotOp";
	static char const * const aml_l_or_op_name = "LOrOp";
	static char const * const aml_local_obj_name = "LocalObj";
	static char const * const aml_local_op_name = "LocalOp";
	static char const * const aml_match_op_name = "MatchOp";
	static char const * const aml_match_opcode_name = "MatchOpcode";
	static char const * const aml_method_flags_name = "MethodFlags";
	static char const * const aml_method_invocation_name = "MethodInvocation";
	static char const * const aml_method_op_name = "MethodOp";
	static char const * const aml_mid_obj_name = "MidObj";
	static char const * const aml_mid_op_name = "MidOp";
	static char const * const aml_msec_time_name = "MsecTime";
	static char const * const aml_multi_name_path_name = "MultiNamePath";
	static char const * const aml_multi_name_prefix_name = "MultiNamePrefix";
	static char const * const aml_multiply_op_name = "MultiplyOp";
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
	static char const * const aml_not_op_name = "NotOp";
	static char const * const aml_notify_object_name = "NotifyObject";
	static char const * const aml_notify_value_name = "NotifyValue";
	static char const * const aml_notify_op_name = "NotifyOp";
	static char const * const aml_null_char_name = "NullChar";
	static char const * const aml_null_name_name = "NullName";
	static char const * const aml_num_bits_name = "NumBits";
	static char const * const aml_num_elements_name = "NumElements";
	static char const * const aml_obj_reference_name = "ObjReference";
	static char const * const aml_object_name = "Object";
	static char const * const aml_object_type_op_name = "ObjectTypeOp";
	static char const * const aml_one_op_name = "OneOp";
	static char const * const aml_ones_op_name = "OnesOp";
	static char const * const aml_op_region_op_name = "OpRegionOp";
	static char const * const aml_op_region_op_suffix_name = "OpRegionOpSuffix";
	static char const * const aml_operand_name = "Operand";
	static char const * const aml_or_op_name = "OrOp";
	static char const * const aml_package_element_name = "PackageElement";
	static char const * const aml_package_element_list_name = "PackageElementList";
	static char const * const aml_package_op_name = "PackageOp";
	static char const * const aml_parent_prefix_char_name = "ParentPrefixChar";
	static char const * const aml_pblk_addr_name = "PblkAddr";
	static char const * const aml_pblk_len_name = "PblkLen";
	static char const * const aml_pkg_lead_byte_name = "PkgLeadByte";
	static char const * const aml_pkg_length_name = "PkgLength";
	static char const * const aml_predicate_name = "Predicate";
	static char const * const aml_prefix_path_name = "PrefixPath";
	static char const * const aml_proc_id_name = "ProcID";
	static char const * const aml_processor_op_name = "ProcessorOp";
	static char const * const aml_processor_op_suffix_name = "ProcessorOpSuffix";
	static char const * const aml_quotient_name = "Quotient";
	static char const * const aml_qword_const_name = "QWordConst";
	static char const * const aml_qword_data_name = "QWordData";
	static char const * const aml_qword_prefix_name = "QWordPrefix";
	static char const * const aml_ref_of_op_name = "RefOfOp";
	static char const * const aml_reference_type_opcode_name = "ReferenceTypeOpcode";
	static char const * const aml_region_len_name = "RegionLen";
	static char const * const aml_region_offset_name = "RegionOffset";
	static char const * const aml_region_space_name = "RegionSpace";
	static char const * const aml_release_op_name = "ReleaseOp";
	static char const * const aml_release_op_suffix_name = "ReleaseOpSuffix";
	static char const * const aml_remainder_name = "Remainder";
	static char const * const aml_return_op_name = "ReturnOp";
	static char const * const aml_reserved_field_name = "ReservedField";
	static char const * const aml_reserved_field_op_name = "ReservedFieldOp";
	static char const * const aml_revision_op_name = "RevisionOp";
	static char const * const aml_revision_op_suffix_name = "RevisionOpSuffix";
	static char const * const aml_root_char_name = "RootChar";
	static char const * const aml_scope_op_name = "ScopeOp";
	static char const * const aml_search_pkg_name = "SearchPkg";
	static char const * const aml_seg_count_name = "SegCount";
	static char const * const aml_shift_count_name = "ShiftCount";
	static char const * const aml_shift_left_op_name = "ShiftLeftOp";
	static char const * const aml_shift_right_op_name = "ShiftRightOp";
	static char const * const aml_simple_name_name = "SimpleName";
	static char const * const aml_size_of_op_name = "SizeOfOp";
	static char const * const aml_sleep_op_name = "SleepOp";
	static char const * const aml_sleep_op_suffix_name = "SleepOpSuffix";
	static char const * const aml_source_buff_name = "SourceBuff";
	static char const * const aml_stall_op_name = "StallOp";
	static char const * const aml_stall_op_suffix_name = "StallOpSuffix";
	static char const * const aml_start_index_name = "StartIndex";
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
	static char const * const aml_thermal_zone_op_name = "ThermalZoneOp";
	static char const * const aml_thermal_zone_op_suffix_name = "ThermalZoneOpSuffix";
	static char const * const aml_time_out_name = "TimeOut";
	static char const * const aml_to_buffer_op_name = "ToBufferOp";
	static char const * const aml_to_hex_string_op_name = "ToHexStringOp";
	static char const * const aml_to_integer_op_name = "ToIntegerOp";
	static char const * const aml_usec_time_name = "UsecTime";
	static char const * const aml_user_term_obj_name = "UserTermObj";
	static char const * const aml_while_op_name = "WhileOp";
	static char const * const aml_word_const_name = "WordConst";
	static char const * const aml_word_data_name = "WordData";
	static char const * const aml_word_prefix_name = "WordPrefix";
	static char const * const aml_xor_op_name = "XorOp";
	static char const * const aml_zero_op_name = "ZeroOp";
	switch(aml_symbol_type)
	{
	case aml_access_attrib:
		return aml_access_attrib_name;
	case aml_access_field:
		return aml_access_field_name;
	case aml_access_field_op:
		return aml_access_field_op_name;
	case aml_access_type:
		return aml_access_type_name;
	case aml_acquire_op:
		return aml_acquire_op_name;
	case aml_acquire_op_suffix:
		return aml_acquire_op_suffix_name;
	case aml_alias_op:
		return aml_alias_op_name;
	case aml_add_op:
		return aml_add_op_name;
	case aml_and_op:
		return aml_and_op_name;
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
	case aml_bit_index:
		return aml_bit_index_name;
	case aml_break_op:
		return aml_break_op_name;
	case aml_buf_data:
		return aml_buf_data_name;
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
	case aml_byte_index:
		return aml_byte_index_name;
	case aml_byte_list:
		return aml_byte_list_name;
	case aml_byte_prefix:
		return aml_byte_prefix_name;
	case aml_computational_data:
		return aml_computational_data_name;
	case aml_concat_op:
		return aml_concat_op_name;
	case aml_concat_res_op:
		return aml_concat_res_op_name;
	case aml_cond_ref_of_op:
		return aml_cond_ref_of_op_name;
	case aml_cond_ref_of_op_suffix:
		return aml_cond_ref_of_op_suffix_name;
	case aml_const_obj:
		return aml_const_obj_name;
	case aml_create_bit_field_op:
		return aml_create_bit_field_op_name;
	case aml_create_byte_field_op:
		return aml_create_byte_field_op_name;
	case aml_create_dword_field_op:
		return aml_create_dword_field_op_name;
	case aml_create_field_op:
		return aml_create_field_op_name;
	case aml_create_field_op_suffix:
		return aml_create_field_op_suffix_name;
	case aml_create_qword_field_op:
		return aml_create_qword_field_op_name;
	case aml_create_word_field_op:
		return aml_create_word_field_op_name;
	case aml_data:
		return aml_data_name;
	case aml_data_object:
		return aml_data_object_name;
	case aml_data_ref_object:
		return aml_data_ref_object_name;
	case aml_debug_obj:
		return aml_debug_obj_name;
	case aml_debug_op:
		return aml_debug_op_name;
	case aml_debug_op_suffix:
		return aml_debug_op_suffix_name;
	case aml_decrement_op:
		return aml_decrement_op_name;
	case aml_def_alias:
		return aml_def_alias_name;
	case aml_def_acquire:
		return aml_def_acquire_name;
	case aml_def_add:
		return aml_def_add_name;
	case aml_def_and:
		return aml_def_and_name;
	case aml_def_break:
		return aml_def_break_name;
	case aml_def_buffer:
		return aml_def_buffer_name;
	case aml_def_concat:
		return aml_def_concat_name;
	case aml_def_concat_res:
		return aml_def_concat_res_name;
	case aml_def_cond_ref_of:
		return aml_def_cond_ref_of_name;
	case aml_def_create_bit_field:
		return aml_def_create_bit_field_name;
	case aml_def_create_byte_field:
		return aml_def_create_byte_field_name;
	case aml_def_create_dword_field:
		return aml_def_create_dword_field_name;
	case aml_def_create_field:
		return aml_def_create_field_name;
	case aml_def_create_qword_field:
		return aml_def_create_qword_field_name;
	case aml_def_create_word_field:
		return aml_def_create_word_field_name;
	case aml_def_decrement:
		return aml_def_decrement_name;
	case aml_def_deref_of:
		return aml_def_deref_of_name;
	case aml_def_device:
		return aml_def_device_name;
	case aml_def_divide:
		return aml_def_divide_name;
	case aml_def_else:
		return aml_def_else_name;
	case aml_def_field:
		return aml_def_field_name;
	case aml_def_find_set_left_bit:
		return aml_def_find_set_left_bit_name;
	case aml_def_find_set_right_bit:
		return aml_def_find_set_right_bit_name;
	case aml_def_if_else:
		return aml_def_if_else_name;
	case aml_def_increment:
		return aml_def_increment_name;
	case aml_def_index:
		return aml_def_index_name;
	case aml_def_index_field:
		return aml_def_index_field_name;
	case aml_def_l_and:
		return aml_def_l_and_name;
	case aml_def_l_equal:
		return aml_def_l_equal_name;
	case aml_def_l_greater:
		return aml_def_l_greater_name;
	case aml_def_l_less:
		return aml_def_l_less_name;
	case aml_def_l_not:
		return aml_def_l_not_name;
	case aml_def_l_or:
		return aml_def_l_or_name;
	case aml_def_match:
		return aml_def_match_name;
	case aml_def_method:
		return aml_def_method_name;
	case aml_def_mid:
		return aml_def_mid_name;
	case aml_def_multiply:
		return aml_def_multiply_name;
	case aml_def_mutex:
		return aml_def_mutex_name;
	case aml_def_name:
		return aml_def_name_name;
	case aml_def_not:
		return aml_def_not_name;
	case aml_def_notify:
		return aml_def_notify_name;
	case aml_def_object_type:
		return aml_def_object_type_name;
	case aml_def_op_region:
		return aml_def_op_region_name;
	case aml_def_or:
		return aml_def_or_name;
	case aml_def_package:
		return aml_def_package_name;
	case aml_def_processor:
		return aml_def_processor_name;
	case aml_def_ref_of:
		return aml_def_ref_of_name;
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
	case aml_def_sleep:
		return aml_def_sleep_name;
	case aml_def_stall:
		return aml_def_stall_name;
	case aml_def_store:
		return aml_def_store_name;
	case aml_def_subtract:
		return aml_def_subtract_name;
	case aml_def_thermal_zone:
		return aml_def_thermal_zone_name;
	case aml_def_to_buffer:
		return aml_def_to_buffer_name;
	case aml_def_to_hex_string:
		return aml_def_to_hex_string_name;
	case aml_def_to_integer:
		return aml_def_to_integer_name;
	case aml_def_while:
		return aml_def_while_name;
	case aml_def_xor:
		return aml_def_xor_name;
	case aml_deref_of_op:
		return aml_deref_of_op_name;
	case aml_device_op:
		return aml_device_op_name;
	case aml_device_op_suffix:
		return aml_device_op_suffix_name;
	case aml_digit_char:
		return aml_digit_char_name;
	case aml_divide_op:
		return aml_divide_op_name;
	case aml_dividend:
		return aml_dividend_name;
	case aml_divisor:
		return aml_divisor_name;
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
	case aml_else_op:
		return aml_else_op_name;
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
	case aml_find_set_left_bit_op:
		return aml_find_set_left_bit_op_name;
	case aml_find_set_right_bit_op:
		return aml_find_set_right_bit_op_name;
	case aml_if_op:
		return aml_if_op_name;
	case aml_increment_op:
		return aml_increment_op_name;
	case aml_index_field_op:
		return aml_index_field_op_name;
	case aml_index_field_op_suffix:
		return aml_index_field_op_suffix_name;
	case aml_index_op:
		return aml_index_op_name;
	case aml_index_value:
		return aml_index_value_name;
	case aml_lead_name_char:
		return aml_lead_name_char_name;
	case aml_l_and_op:
		return aml_l_and_op_name;
	case aml_l_equal_op:
		return aml_l_equal_op_name;
	case aml_l_greater_op:
		return aml_l_greater_op_name;
	case aml_l_less_op:
		return aml_l_less_op_name;
	case aml_l_not_op:
		return aml_l_not_op_name;
	case aml_l_or_op:
		return aml_l_or_op_name;
	case aml_local_obj:
		return aml_local_obj_name;
	case aml_local_op:
		return aml_local_op_name;
	case aml_match_op:
		return aml_match_op_name;
	case aml_match_opcode:
		return aml_match_opcode_name;
	case aml_method_flags:
		return aml_method_flags_name;
	case aml_method_invocation:
		return aml_method_invocation_name;
	case aml_method_op:
		return aml_method_op_name;
	case aml_mid_obj:
		return aml_mid_obj_name;
	case aml_mid_op:
		return aml_mid_op_name;
	case aml_msec_time:
		return aml_msec_time_name;
	case aml_multi_name_path:
		return aml_multi_name_path_name;
	case aml_multi_name_prefix:
		return aml_multi_name_prefix_name;
	case aml_multiply_op:
		return aml_multiply_op_name;
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
	case aml_not_op:
		return aml_not_op_name;
	case aml_notify_object:
		return aml_notify_object_name;
	case aml_notify_value:
		return aml_notify_value_name;
	case aml_notify_op:
		return aml_notify_op_name;
	case aml_null_char:
		return aml_null_char_name;
	case aml_null_name:
		return aml_null_name_name;
	case aml_num_bits:
		return aml_num_bits_name;
	case aml_num_elements:
		return aml_num_elements_name;
	case aml_obj_reference:
		return aml_obj_reference_name;
	case aml_object:
		return aml_object_name;
	case aml_object_type_op:
		return aml_object_type_op_name;
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
	case aml_or_op:
		return aml_or_op_name;
	case aml_package_element:
		return aml_package_element_name;
	case aml_package_element_list:
		return aml_package_element_list_name;
	case aml_package_op:
		return aml_package_op_name;
	case aml_parent_prefix_char:
		return aml_parent_prefix_char_name;
	case aml_pblk_addr:
		return aml_pblk_addr_name;
	case aml_pblk_len:
		return aml_pblk_len_name;
	case aml_pkg_lead_byte:
		return aml_pkg_lead_byte_name;
	case aml_pkg_length:
		return aml_pkg_length_name;
	case aml_predicate:
		return aml_predicate_name;
	case aml_prefix_path:
		return aml_prefix_path_name;
	case aml_proc_id:
		return aml_proc_id_name;
	case aml_processor_op:
		return aml_processor_op_name;
	case aml_processor_op_suffix:
		return aml_processor_op_suffix_name;
	case aml_quotient:
		return aml_quotient_name;
	case aml_qword_const:
		return aml_qword_const_name;
	case aml_qword_data:
		return aml_qword_data_name;
	case aml_qword_prefix:
		return aml_qword_prefix_name;
	case aml_ref_of_op:
		return aml_ref_of_op_name;
	case aml_reference_type_opcode:
		return aml_reference_type_opcode_name;
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
	case aml_remainder:
		return aml_remainder_name;
	case aml_return_op:
		return aml_return_op_name;
	case aml_reserved_field:
		return aml_reserved_field_name;
	case aml_reserved_field_op:
		return aml_reserved_field_op_name;
	case aml_revision_op:
		return aml_revision_op_name;
	case aml_revision_op_suffix:
		return aml_revision_op_suffix_name;
	case aml_root_char:
		return aml_root_char_name;
	case aml_scope_op:
		return aml_scope_op_name;
	case aml_search_pkg:
		return aml_search_pkg_name;
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
	case aml_sleep_op:
		return aml_sleep_op_name;
	case aml_sleep_op_suffix:
		return aml_sleep_op_suffix_name;
	case aml_source_buff:
		return aml_source_buff_name;
	case aml_stall_op:
		return aml_stall_op_name;
	case aml_stall_op_suffix:
		return aml_stall_op_suffix_name;
	case aml_start_index:
		return aml_start_index_name;
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
	case aml_thermal_zone_op:
		return aml_thermal_zone_op_name;
	case aml_thermal_zone_op_suffix:
		return aml_thermal_zone_op_suffix_name;
	case aml_time_out:
		return aml_time_out_name;
	case aml_to_buffer_op:
		return aml_to_buffer_op_name;
	case aml_to_hex_string_op:
		return aml_to_hex_string_op_name;
	case aml_to_integer_op:
		return aml_to_integer_op_name;
	case aml_user_term_obj:
		return aml_user_term_obj_name;
	case aml_usec_time:
		return aml_usec_time_name;
	case aml_while_op:
		return aml_while_op_name;
	case aml_word_const:
		return aml_word_const_name;
	case aml_word_data:
		return aml_word_data_name;
	case aml_word_prefix:
		return aml_word_prefix_name;
	case aml_xor_op:
		return aml_xor_op_name;
	case aml_zero_op:
		return aml_zero_op_name;
	}
	ERROR(); // Invalid aml_symbol->type
	return NULL;
}

// <access_attrib> := <byte_data>
AMLSymbol *analyse_aml_access_attrib(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("access_attrib aml.length = %#010.8x\n", aml.length);
	AMLSymbol *access_attrib = malloc(sizeof(*access_attrib));
	access_attrib->parent = parent;
	access_attrib->string.initial = aml.initial;
	access_attrib->string.length = 0;
	access_attrib->type = aml_access_attrib;
	access_attrib->flags = 0;
	access_attrib->component.access_attrib.byte_data = analyse_aml_byte_data(access_attrib, aml);
	access_attrib->string.length += access_attrib->component.access_attrib.byte_data->string.length;
	aml.initial += access_attrib->component.access_attrib.byte_data->string.length;
	aml.length -= access_attrib->component.access_attrib.byte_data->string.length;
	return access_attrib;
}

// <access_field> := <access_field_op> <access_type> <access_attrib>
AMLSymbol *analyse_aml_access_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("access_field aml.length = %#010.8x\n");
	AMLSymbol *access_field = malloc(sizeof(*access_field));
	access_field->parent = parent;
	access_field->string.initial = aml.initial;
	access_field->string.length = 0;
	access_field->type = aml_access_field;
	access_field->flags = 0;
	access_field->component.access_field.access_field_op = analyse_aml_access_field_op(access_field, aml);
	access_field->string.length += access_field->component.access_field.access_field_op->string.length;
	aml.initial += access_field->component.access_field.access_field_op->string.length;
	aml.length -= access_field->component.access_field.access_field_op->string.length;
	access_field->component.access_field.access_type = analyse_aml_access_type(access_field, aml);
	access_field->string.length += access_field->component.access_field.access_type->string.length;
	aml.initial += access_field->component.access_field.access_type->string.length;
	aml.length -= access_field->component.access_field.access_type->string.length;
	access_field->component.access_field.access_attrib = analyse_aml_access_attrib(access_field, aml);
	access_field->string.length += access_field->component.access_field.access_attrib->string.length;
	aml.initial += access_field->component.access_field.access_attrib->string.length;
	aml.length -= access_field->component.access_field.access_attrib->string.length;
	return access_field;
}

// <access_field_op> := AML_BYTE_ACCESS_FIELD_OP
AMLSymbol *analyse_aml_access_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("access_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *access_field_op = malloc(sizeof(*access_field_op));
	access_field_op->parent = parent;
	access_field_op->string.initial = aml.initial;
	access_field_op->string.length = 1;
	access_field_op->type = aml_access_field_op;
	access_field_op->flags = 0;
	if(!aml.length)
	{
		access_field_op->string.length = 0;
		access_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_ACCESS_FIELD_OP)
	{
		access_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect access_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return access_field_op;
}

// <access_type> := <byte_data>
AMLSymbol *analyse_aml_access_type(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("access_type aml.length = %#010.8x\n", aml.length);
	AMLSymbol *access_type = malloc(sizeof(*access_type));
	access_type->parent = parent;
	access_type->string.initial = aml.initial;
	access_type->string.length = 0;
	access_type->type = aml_access_type;
	access_type->flags = 0;
	access_type->component.access_type.byte_data = analyse_aml_byte_data(access_type, aml);
	access_type->string.length += access_type->component.access_type.byte_data->string.length;
	aml.initial += access_type->component.access_type.byte_data->string.length;
	aml.length -= access_type->component.access_type.byte_data->string.length;
	return access_type;
}

// <acquire_op> := <ext_op_prefix> <acquire_op_suffix>
AMLSymbol *analyse_aml_acquire_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("acquire_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *acquire_op = malloc(sizeof(*acquire_op));
	acquire_op->parent = parent;
	acquire_op->string.initial = aml.initial;
	acquire_op->string.length = 0;
	acquire_op->type = aml_acquire_op;
	acquire_op->flags = 0;
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
	printf_serial("acquire_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *acquire_op_suffix = malloc(sizeof(*acquire_op_suffix));
	acquire_op_suffix->parent = parent;
	acquire_op_suffix->string.initial = aml.initial;
	acquire_op_suffix->string.length = 1;
	acquire_op_suffix->type = aml_acquire_op_suffix;
	acquire_op_suffix->flags = 0;
	if(!aml.length)
	{
		acquire_op_suffix->string.length = 0;
		acquire_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_ACQUIRE_OP)
	{
		acquire_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect acquire_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return acquire_op_suffix;
}

// <alias_op> := AML_BYTE_ALIAS_OP
AMLSymbol *analyse_aml_alias_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("alias_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *alias_op = malloc(sizeof(*alias_op));
	alias_op->parent = parent;
	alias_op->string.initial = aml.initial;
	alias_op->string.length = 1;
	alias_op->type = aml_alias_op;
	alias_op->flags = 0;
	if(!aml.length)
	{
		alias_op->string.length = 0;
		alias_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*alias_op->string.initial != AML_BYTE_ALIAS_OP)
	{
		alias_op->flags |= AML_SYMBOL_ERROR; // Incorrect alias_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return alias_op;
}

// <add_op> := AML_BYTE_ADD_OP
AMLSymbol *analyse_aml_add_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("add_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *add_op = malloc(sizeof(*add_op));
	add_op->parent = parent;
	add_op->string.initial = aml.initial;
	add_op->string.length = 1;
	add_op->type = aml_add_op;
	add_op->flags = 0;
	if(!aml.length)
	{
		add_op->string.length = 0;
		add_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*add_op->string.initial != AML_BYTE_ADD_OP)
	{
		add_op->flags |= AML_SYMBOL_ERROR; // Incorrect add_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return add_op;
}

// <and_op> := AML_BYTE_AND_OP
AMLSymbol *analyse_aml_and_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("and_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *and_op = malloc(sizeof(*and_op));
	and_op->parent = parent;
	and_op->string.initial = aml.initial;
	and_op->string.length = 1;
	and_op->type = aml_and_op;
	and_op->flags = 0;
	if(!aml.length)
	{
		and_op->string.length = 0;
		and_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*and_op->string.initial != AML_BYTE_AND_OP)
	{
		and_op->flags |= AML_SYMBOL_ERROR; // Incorrect and_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return and_op;
}

// <arg_obj> := <arg_op>
AMLSymbol *analyse_aml_arg_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("arg_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *arg_obj = malloc(sizeof(*arg_obj));
	arg_obj->parent = parent;
	arg_obj->string.initial = aml.initial;
	arg_obj->string.length = 0;
	arg_obj->type = aml_arg_obj;
	arg_obj->flags = 0;
	arg_obj->component.arg_obj.arg_op = analyse_aml_arg_op(arg_obj, aml);
	arg_obj->string.length += arg_obj->component.arg_obj.arg_op->string.length;
	arg_obj->component.arg_obj.arg_op_number = *arg_obj->component.arg_obj.arg_op->string.initial - AML_BYTE_ARG_0_OP;
	return arg_obj;
}

// <arg_object> := <term_arg>
AMLSymbol *analyse_aml_arg_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("arg_object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *arg_object = malloc(sizeof(*arg_object));
	arg_object->parent = parent;
	arg_object->string.initial = aml.initial;
	arg_object->string.length = 0;
	arg_object->type = aml_arg_object;
	arg_object->flags = 0;
	arg_object->component.arg_object.term_arg = analyse_aml_term_arg(arg_object, aml);
	arg_object->string.length += arg_object->component.arg_object.term_arg->string.length;
	return arg_object;
}

// <arg_op> := 0x68 - 0x6e
AMLSymbol *analyse_aml_arg_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("arg_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *arg_op = malloc(sizeof(*arg_op));
	arg_op->parent = parent;
	arg_op->string.initial = aml.initial;
	arg_op->string.length = 1;
	arg_op->type = aml_arg_op;
	arg_op->flags = 0;
	if(!aml.length)
	{
		arg_op->string.length = 0;
		arg_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else switch(*arg_op->string.initial)
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
		arg_op->flags |= AML_SYMBOL_ERROR; // Incorrect arg op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return arg_op;
}

// <ascii_char> := 0x01 - 0x7f
AMLSymbol *analyse_aml_ascii_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("ascii_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *ascii_char = malloc(sizeof(*ascii_char));
	ascii_char->parent = parent;
	ascii_char->string.initial = aml.initial;
	ascii_char->string.length = 1;
	ascii_char->type = aml_ascii_char;
	ascii_char->flags = 0;
	if(!aml.length)
	{
		ascii_char->string.length = 0;
		ascii_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(!(0x01 <= *ascii_char->string.initial && *ascii_char->string.initial <= 0x7f))
	{
		ascii_char->flags |= AML_SYMBOL_ERROR; // Incorrect ascii char
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return ascii_char;
}

// <ascii_char_list> := Nothing | <ascii_char> <ascii_char_list>
AMLSymbol *analyse_aml_ascii_char_list(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("ascii_char_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *ascii_char_list = malloc(sizeof(*ascii_char_list));
	ascii_char_list->parent = parent;
	ascii_char_list->string.initial = aml.initial;
	ascii_char_list->string.length = 0;
	ascii_char_list->type = aml_ascii_char_list;
	ascii_char_list->flags = 0;
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

// <bit_index> := <term_arg>
AMLSymbol *analyse_aml_bit_index(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("bit_index aml.length = %#010.8x\n", aml.length);
	AMLSymbol *bit_index = malloc(sizeof(*bit_index));
	bit_index->parent = parent;
	bit_index->string.initial = aml.initial;
	bit_index->string.length = 0;
	bit_index->type = aml_bit_index;
	bit_index->flags = 0;
	bit_index->component.bit_index.term_arg = analyse_aml_term_arg(bit_index, aml);
	bit_index->string.length += bit_index->component.bit_index.term_arg->string.length;
	aml.initial += bit_index->component.bit_index.term_arg->string.length;
	aml.length -= bit_index->component.bit_index.term_arg->string.length;
	return bit_index;
}

// <break_op> := AML_BYTE_BREAK_OP
AMLSymbol *analyse_aml_break_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("break_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *break_op = malloc(sizeof(*break_op));
	break_op->parent = parent;
	break_op->string.initial = aml.initial;
	break_op->string.length = 1;
	break_op->type = aml_break_op;
	break_op->flags = 0;
	if(!aml.length)
	{
		break_op->string.length = 0;
		break_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*break_op->string.initial != AML_BYTE_BREAK_OP)
	{
		break_op->flags |= AML_SYMBOL_ERROR; // Incorrect break_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return break_op;
}

// <buf_data> := <term_arg>
AMLSymbol *analyse_aml_buf_data(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("buf_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *buf_data = malloc(sizeof(*buf_data));
	buf_data->parent = parent;
	buf_data->string.initial = aml.initial;
	buf_data->string.length = 0;
	buf_data->type = aml_buf_data;
	buf_data->flags = 0;
	buf_data->component.buf_data.term_arg = analyse_aml_term_arg(buf_data, aml);
	buf_data->string.length += buf_data->component.buf_data.term_arg->string.length;
	aml.initial += buf_data->component.buf_data.term_arg->string.length;
	aml.length -= buf_data->component.buf_data.term_arg->string.length;
	return buf_data;
}

// <buff_pkg_str_obj> := <term_arg>
AMLSymbol *analyse_aml_buff_pkg_str_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("buff_pkg_str_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *buff_pkg_str_obj = malloc(sizeof(*buff_pkg_str_obj));
	buff_pkg_str_obj->parent = parent;
	buff_pkg_str_obj->string.initial = aml.initial;
	buff_pkg_str_obj->string.length = 0;
	buff_pkg_str_obj->type = aml_buff_pkg_str_obj;
	buff_pkg_str_obj->flags = 0;
	buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg = analyse_aml_term_arg(buff_pkg_str_obj, aml);
	buff_pkg_str_obj->string.length += buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	aml.initial += buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	aml.length -= buff_pkg_str_obj->component.buff_pkg_str_obj.term_arg->string.length;
	return buff_pkg_str_obj;
}

// <buffer_op> := AML_BYTE_BUFFER_OP
AMLSymbol *analyse_aml_buffer_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("buffer_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *buffer_op = malloc(sizeof(*buffer_op));
	buffer_op->parent = parent;
	buffer_op->string.initial = aml.initial;
	buffer_op->string.length = 1;
	buffer_op->type = aml_buffer_op;
	buffer_op->flags = 0;
	if(!aml.length)
	{
		buffer_op->string.length = 0;
		buffer_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*buffer_op->string.initial != AML_BYTE_BUFFER_OP)
	{
		buffer_op->flags |= AML_SYMBOL_ERROR; // Incorrect buffer op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return buffer_op;
}

// <buffer_size> := <term_arg>
AMLSymbol *analyse_aml_buffer_size(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("buffer_size aml.length = %#010.8x\n", aml.length);
	AMLSymbol *buffer_size = malloc(sizeof(*buffer_size));
	buffer_size->parent = parent;
	buffer_size->string.initial = aml.initial;
	buffer_size->string.length = 0;
	buffer_size->type = aml_buffer_size;
	buffer_size->flags = 0;
	buffer_size->component.buffer_size.term_arg = analyse_aml_term_arg(buffer_size, aml);
	buffer_size->string.length += buffer_size->component.buffer_size.term_arg->string.length;
	return buffer_size;
}

// <byte_const> := <byte_prefix> <byte_data>
AMLSymbol *analyse_aml_byte_const(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("byte_const aml.length = %#010.8x\n", aml.length);
	AMLSymbol *byte_const = malloc(sizeof(*byte_const));
	byte_const->parent = parent;
	byte_const->string.initial = aml.initial;
	byte_const->string.length = 0;
	byte_const->type = aml_byte_const;
	byte_const->flags = 0;
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
	printf_serial("byte_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *byte_data = malloc(sizeof(*byte_data));
	byte_data->parent = parent;
	byte_data->string.initial = aml.initial;
	byte_data->string.length = 1;
	byte_data->type = aml_byte_data;
	byte_data->flags = 0;
	if(!aml.length)
	{
		byte_data->string.length = 0;
		byte_data->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	return byte_data;
}

// <byte_index> := <term_arg>
AMLSymbol *analyse_aml_byte_index(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("byte_index aml.length = %#010.8x\n", aml.length);
	AMLSymbol *byte_index = malloc(sizeof(*byte_index));
	byte_index->parent = parent;
	byte_index->string.initial = aml.initial;
	byte_index->string.length = 0;
	byte_index->type = aml_byte_index;
	byte_index->flags = 0;
	byte_index->component.byte_index.term_arg = analyse_aml_term_arg(byte_index, aml);
	byte_index->string.length += byte_index->component.byte_index.term_arg->string.length;
	aml.initial += byte_index->component.byte_index.term_arg->string.length;
	aml.length -= byte_index->component.byte_index.term_arg->string.length;
	return byte_index;
}

// <byte_list> := Nothing | <byte_data> <byte_list>
AMLSymbol *analyse_aml_byte_list(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("byte_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *byte_list = malloc(sizeof(*byte_list));
	byte_list->parent = parent;
	byte_list->string.initial = aml.initial;
	byte_list->string.length = 0;
	byte_list->type = aml_byte_list;
	byte_list->flags = 0;
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
	printf_serial("byte_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *byte_prefix = malloc(sizeof(*byte_prefix));
	byte_prefix->parent = parent;
	byte_prefix->string.initial = aml.initial;
	byte_prefix->string.length = 1;
	byte_prefix->type = aml_byte_prefix;
	byte_prefix->flags = 0;
	if(!aml.length)
	{
		byte_prefix->string.length = 0;
		byte_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	if(*byte_prefix->string.initial != AML_BYTE_BYTE_PREFIX)
	{
		byte_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect byte prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return byte_prefix;
}

// <computational_data> := <byte_const> | <word_const> | <dword_const> | <qword_const> | <string> | <const_obj> | <revision_op> | <def_buffer>
AMLSymbol *analyse_aml_computational_data(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("computational_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *computational_data = malloc(sizeof(*computational_data));
	computational_data->parent = parent;
	computational_data->string.initial = aml.initial;
	computational_data->string.length = 0;
	computational_data->type = aml_computational_data;
	computational_data->flags = 0;
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
		computational_data->flags |= AML_SYMBOL_ERROR; // Incorrect computational data
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return computational_data;
}

// <concat_op> := AML_BYTE_CONCAT_OP
AMLSymbol *analyse_aml_concat_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("concat_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *concat_op = malloc(sizeof(*concat_op));
	concat_op->parent = parent;
	concat_op->string.initial = aml.initial;
	concat_op->string.length = 1;
	concat_op->type = aml_concat_op;
	concat_op->flags = 0;
	if(!aml.length)
	{
		concat_op->string.length = 0;
		concat_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*concat_op->string.initial != AML_BYTE_CONCAT_OP)
	{
		concat_op->flags |= AML_SYMBOL_ERROR; // Incorrect concat_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return concat_op;
}

// <concat_res_op> := AML_BYTE_CONCAT_RES_OP
AMLSymbol *analyse_aml_concat_res_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("concat_res_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *concat_res_op = malloc(sizeof(*concat_res_op));
	concat_res_op->parent = parent;
	concat_res_op->string.initial = aml.initial;
	concat_res_op->string.length = 1;
	concat_res_op->type = aml_concat_res_op;
	concat_res_op->flags = 0;
	if(!aml.length)
	{
		concat_res_op->string.length = 0;
		concat_res_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*concat_res_op->string.initial != AML_BYTE_CONCAT_RES_OP)
	{
		concat_res_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return concat_res_op;
}

// <cond_ref_of_op> := <ext_op_prefix> <cond_ref_of_op_suffix>
AMLSymbol *analyse_aml_cond_ref_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("cond_ref_of_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *cond_ref_of_op = malloc(sizeof(*cond_ref_of_op));
	cond_ref_of_op->parent = parent;
	cond_ref_of_op->string.initial = aml.initial;
	cond_ref_of_op->string.length = 0;
	cond_ref_of_op->type = aml_cond_ref_of_op;
	cond_ref_of_op->flags = 0;
	cond_ref_of_op->component.cond_ref_of_op.ext_op_prefix = analyse_aml_ext_op_prefix(cond_ref_of_op, aml);
	cond_ref_of_op->string.length += cond_ref_of_op->component.cond_ref_of_op.ext_op_prefix->string.length;
	aml.initial += cond_ref_of_op->component.cond_ref_of_op.ext_op_prefix->string.length;
	aml.length -= cond_ref_of_op->component.cond_ref_of_op.ext_op_prefix->string.length;
	cond_ref_of_op->component.cond_ref_of_op.cond_ref_of_op_suffix = analyse_aml_cond_ref_of_op_suffix(cond_ref_of_op, aml);
	cond_ref_of_op->string.length += cond_ref_of_op->component.cond_ref_of_op.cond_ref_of_op_suffix->string.length;
	aml.initial += cond_ref_of_op->component.cond_ref_of_op.cond_ref_of_op_suffix->string.length;
	aml.length -= cond_ref_of_op->component.cond_ref_of_op.cond_ref_of_op_suffix->string.length;
	return cond_ref_of_op;
}

// <cond_ref_of_op_suffix> := AML_BYTE_COND_REF_OF_OP
AMLSymbol *analyse_aml_cond_ref_of_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("cond_ref_of_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *cond_ref_of_op_suffix = malloc(sizeof(*cond_ref_of_op_suffix));
	cond_ref_of_op_suffix->parent = parent;
	cond_ref_of_op_suffix->string.initial = aml.initial;
	cond_ref_of_op_suffix->string.length = 1;
	cond_ref_of_op_suffix->type = aml_cond_ref_of_op_suffix;
	cond_ref_of_op_suffix->flags = 0;
	if(!aml.length)
	{
		cond_ref_of_op_suffix->string.length = 0;
		cond_ref_of_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*cond_ref_of_op_suffix->string.initial != AML_BYTE_COND_REF_OF_OP)
	{
		cond_ref_of_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect cond_ref_of_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return cond_ref_of_op_suffix;
}

// <const_obj> := <zero_op> | <one_op> | <ones_op>
AMLSymbol *analyse_aml_const_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("const_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *const_obj = malloc(sizeof(*const_obj));
	const_obj->parent = parent;
	const_obj->string.initial = aml.initial;
	const_obj->string.length = 0;
	const_obj->type = aml_const_obj;
	const_obj->flags = 0;
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
		const_obj->flags |= AML_SYMBOL_ERROR; // Incorrect const obj
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return const_obj;
}

// <create_bit_field_op> := AML_BYTE_CREATE_BIT_FIELD_OP
AMLSymbol *analyse_aml_create_bit_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_bit_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_bit_field_op = malloc(sizeof(*create_bit_field_op));
	create_bit_field_op->parent = parent;
	create_bit_field_op->string.initial = aml.initial;
	create_bit_field_op->string.length = 1;
	create_bit_field_op->type = aml_create_bit_field_op;
	create_bit_field_op->flags = 0;
	if(!aml.length)
	{
		create_bit_field_op->string.length = 0;
		create_bit_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_bit_field_op->string.initial != AML_BYTE_CREATE_BIT_FIELD_OP)
	{
		create_bit_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect create_bit_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_bit_field_op;
}

// <create_byte_field_op> := AML_BYTE_CREATE_BYTE_FIELD_OP
AMLSymbol *analyse_aml_create_byte_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_byte_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_byte_field_op = malloc(sizeof(*create_byte_field_op));
	create_byte_field_op->parent = parent;
	create_byte_field_op->string.initial = aml.initial;
	create_byte_field_op->string.length = 1;
	create_byte_field_op->type = aml_create_byte_field_op;
	create_byte_field_op->flags = 0;
	if(!aml.length)
	{
		create_byte_field_op->string.length = 0;
		create_byte_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_byte_field_op->string.initial != AML_BYTE_CREATE_BYTE_FIELD_OP)
	{
		create_byte_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect create_byte_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_byte_field_op;
}

// <create_dword_field_op> := AML_BYTE_CREATE_DWORD_FIELD_OP
AMLSymbol *analyse_aml_create_dword_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_dword_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_dword_field_op = malloc(sizeof(*create_dword_field_op));
	create_dword_field_op->parent = parent;
	create_dword_field_op->string.initial = aml.initial;
	create_dword_field_op->string.length = 1;
	create_dword_field_op->type = aml_create_dword_field_op;
	create_dword_field_op->flags = 0;
	if(!aml.length)
	{
		create_dword_field_op->string.length = 0;
		create_dword_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_dword_field_op->string.initial != AML_BYTE_CREATE_DWORD_FIELD_OP)
	{
		create_dword_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect create_dword_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_dword_field_op;
}

// <create_field_op> := <ext_op_prefix> <create_field_op_suffix>
AMLSymbol *analyse_aml_create_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_field_op = malloc(sizeof(*create_field_op));
	create_field_op->parent = parent;
	create_field_op->string.initial = aml.initial;
	create_field_op->string.length = 0;
	create_field_op->type = aml_create_field_op;
	create_field_op->flags = 0;
	create_field_op->component.create_field_op.ext_op_prefix = analyse_aml_ext_op_prefix(create_field_op, aml);
	create_field_op->string.length += create_field_op->component.create_field_op.ext_op_prefix->string.length;
	aml.initial += create_field_op->component.create_field_op.ext_op_prefix->string.length;
	aml.length -= create_field_op->component.create_field_op.ext_op_prefix->string.length;
	create_field_op->component.create_field_op.create_field_op_suffix = analyse_aml_create_field_op_suffix(create_field_op, aml);
	create_field_op->string.length += create_field_op->component.create_field_op.create_field_op_suffix->string.length;
	aml.initial += create_field_op->component.create_field_op.create_field_op_suffix->string.length;
	aml.length -= create_field_op->component.create_field_op.create_field_op_suffix->string.length;
	return create_field_op;
}

// <create_field_op_suffix> := AML_BYTE_CREATE_FIELD_OP
AMLSymbol *analyse_aml_create_field_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_field_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_field_op_suffix = malloc(sizeof(*create_field_op_suffix));
	create_field_op_suffix->parent = parent;
	create_field_op_suffix->string.initial = aml.initial;
	create_field_op_suffix->string.length = 1;
	create_field_op_suffix->type = aml_create_field_op_suffix;
	create_field_op_suffix->flags = 0;
	if(!aml.length)
	{
		create_field_op_suffix->string.length = 0;
		create_field_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_field_op_suffix->string.initial != AML_BYTE_CREATE_FIELD_OP)
	{
		create_field_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect create_field_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_field_op_suffix;
}

// <create_qword_field_op> := AML_BYTE_CREATE_QWORD_FIELD_OP
AMLSymbol *analyse_aml_create_qword_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_qword_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_qword_field_op = malloc(sizeof(*create_qword_field_op));
	create_qword_field_op->parent = parent;
	create_qword_field_op->string.initial = aml.initial;
	create_qword_field_op->string.length = 1;
	create_qword_field_op->type = aml_create_qword_field_op;
	create_qword_field_op->flags = 0;
	if(!aml.length)
	{
		create_qword_field_op->string.length = 0;
		create_qword_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_qword_field_op->string.initial != AML_BYTE_CREATE_QWORD_FIELD_OP)
	{
		create_qword_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect create_qword_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_qword_field_op;
}

// <create_word_field_op> := AML_BYTE_CREATE_WORD_FIELD_OP
AMLSymbol *analyse_aml_create_word_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("create_word_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *create_word_field_op = malloc(sizeof(*create_word_field_op));
	create_word_field_op->parent = parent;
	create_word_field_op->string.initial = aml.initial;
	create_word_field_op->string.length = 1;
	create_word_field_op->type = aml_create_word_field_op;
	create_word_field_op->flags = 0;
	if(!aml.length)
	{
		create_word_field_op->string.length = 0;
		create_word_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*create_word_field_op->string.initial != AML_BYTE_CREATE_WORD_FIELD_OP)
	{
		create_word_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect create_word_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return create_word_field_op;
}

// <data> := <term_arg>
AMLSymbol *analyse_aml_data(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *data = malloc(sizeof(*data));
	data->parent = parent;
	data->string.initial = aml.initial;
	data->string.length = 0;
	data->type = aml_data;
	data->flags = 0;
	data->component.data.term_arg = analyse_aml_term_arg(data, aml);
	data->string.length += data->component.data.term_arg->string.length;
	aml.initial += data->component.data.term_arg->string.length;
	aml.length -= data->component.data.term_arg->string.length;
	return data;
}

// <data_object> := <computational_data> | <def_package> | <def_var_package>
AMLSymbol *analyse_aml_data_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("data_object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *data_object = malloc(sizeof(*data_object));
	data_object->parent = parent;
	data_object->string.initial = aml.initial;
	data_object->string.length = 0;
	data_object->type = aml_data_object;
	data_object->flags = 0;
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
		data_object->flags |= AML_SYMBOL_ERROR; // Incorrect data object
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return data_object;
}

// <data_ref_object> := <data_object> | <object_reference>
AMLSymbol *analyse_aml_data_ref_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("data_ref_object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *data_ref_object = malloc(sizeof(*data_ref_object));
	data_ref_object->parent = parent;
	data_ref_object->string.initial = aml.initial;
	data_ref_object->string.length = 0;
	data_ref_object->type = aml_data_ref_object;
	data_ref_object->flags = 0;
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
		data_ref_object->flags |= AML_SYMBOL_ERROR; // Incorrect data ref object
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return data_ref_object;
}

// <debug_obj> := <debug_op>
AMLSymbol *analyse_aml_debug_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("debug_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *debug_obj = malloc(sizeof(*debug_obj));
	debug_obj->parent = parent;
	debug_obj->string.initial = aml.initial;
	debug_obj->string.length = 0;
	debug_obj->type = aml_debug_obj;
	debug_obj->flags = 0;
	debug_obj->component.debug_obj.debug_op = analyse_aml_debug_op(debug_obj, aml);
	debug_obj->string.length += debug_obj->component.debug_obj.debug_op->string.length;
	aml.initial += debug_obj->component.debug_obj.debug_op->string.length;
	aml.length -= debug_obj->component.debug_obj.debug_op->string.length;
	return debug_obj;
}

// <debug_op> := <ext_op_prefix> <debug_op_suffix>
AMLSymbol *analyse_aml_debug_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("debug_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *debug_op = malloc(sizeof(*debug_op));
	debug_op->parent = parent;
	debug_op->string.initial = aml.initial;
	debug_op->string.length = 0;
	debug_op->type = aml_debug_op;
	debug_op->flags = 0;
	debug_op->component.debug_op.ext_op_prefix = analyse_aml_ext_op_prefix(debug_op, aml);
	debug_op->string.length += debug_op->component.debug_op.ext_op_prefix->string.length;
	aml.initial += debug_op->component.debug_op.ext_op_prefix->string.length;
	aml.length -= debug_op->component.debug_op.ext_op_prefix->string.length;
	debug_op->component.debug_op.debug_op_suffix = analyse_aml_debug_op_suffix(debug_op, aml);
	debug_op->string.length += debug_op->component.debug_op.debug_op_suffix->string.length;
	aml.initial += debug_op->component.debug_op.debug_op_suffix->string.length;
	aml.length -= debug_op->component.debug_op.debug_op_suffix->string.length;
	return debug_op;
}

// <debug_op_suffix> := AML_BYTE_DEBUG_OP
AMLSymbol *analyse_aml_debug_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("debug_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *debug_op_suffix = malloc(sizeof(*debug_op_suffix));
	debug_op_suffix->parent = parent;
	debug_op_suffix->string.initial = aml.initial;
	debug_op_suffix->string.length = 1;
	debug_op_suffix->type = aml_debug_op_suffix;
	debug_op_suffix->flags = 0;
	if(!aml.length)
	{
		debug_op_suffix->string.length = 0;
		debug_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*debug_op_suffix->string.initial != AML_BYTE_DEBUG_OP)
	{
		debug_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect debug_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return debug_op_suffix;
}

// <decrement_op> := AML_BYTE_DECREMENT_OP
AMLSymbol *analyse_aml_decrement_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("decrement_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *decrement_op = malloc(sizeof(*decrement_op));
	decrement_op->parent = parent;
	decrement_op->string.initial = aml.initial;
	decrement_op->string.length = 1;
	decrement_op->type = aml_decrement_op;
	decrement_op->flags = 0;
	if(!aml.length)
	{
		decrement_op->string.length = 0;
		decrement_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*decrement_op->string.initial != AML_BYTE_DECREMENT_OP)
	{
		decrement_op->flags |= AML_SYMBOL_ERROR; // Incorrect decrement_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return decrement_op;
}

// <def_alias> := <alias_op> <name_string> <name_string>
AMLSymbol *analyse_aml_def_alias(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_alias aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_alias = malloc(sizeof(*def_alias));
	def_alias->parent = parent;
	def_alias->string.initial = aml.initial;
	def_alias->string.length = 0;
	def_alias->type = aml_def_alias;
	def_alias->flags = 0;
	def_alias->component.def_alias.alias_op = analyse_aml_alias_op(def_alias, aml);
	def_alias->string.length += def_alias->component.def_alias.alias_op->string.length;
	aml.initial += def_alias->component.def_alias.alias_op->string.length;
	aml.length -= def_alias->component.def_alias.alias_op->string.length;
	for(AMLSymbol **name_string = def_alias->component.def_alias.name_string; name_string != def_alias->component.def_alias.name_string + _countof(def_alias->component.def_alias.name_string); name_string++)
	{
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
	printf_serial("def_acquire aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_acquire = malloc(sizeof(*def_acquire));
	def_acquire->parent = parent;
	def_acquire->string.initial = aml.initial;
	def_acquire->string.length = 0;
	def_acquire->type = aml_def_acquire;
	def_acquire->flags = 0;
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

// <def_add> := <add_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_add(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_add aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_add = malloc(sizeof(*def_add));
	def_add->parent = parent;
	def_add->string.initial = aml.initial;
	def_add->string.length = 0;
	def_add->type = aml_def_add;
	def_add->flags = 0;
	def_add->component.def_add.add_op = analyse_aml_add_op(def_add, aml);
	def_add->string.length += def_add->component.def_add.add_op->string.length;
	aml.initial += def_add->component.def_add.add_op->string.length;
	aml.length -= def_add->component.def_add.add_op->string.length;
	for(AMLSymbol **operand = def_add->component.def_add.operand; operand != def_add->component.def_add.operand + _countof(def_add->component.def_add.operand); operand++)
	{
		*operand = analyse_aml_operand(def_add, aml);
		def_add->string.length += (*operand)->string.length;
		aml.initial += (*operand)->string.length;
		aml.length -= (*operand)->string.length;
	}
	def_add->component.def_add.target = analyse_aml_target(def_add, aml);
	def_add->string.length += def_add->component.def_add.target->string.length;
	aml.initial += def_add->component.def_add.target->string.length;
	aml.length -= def_add->component.def_add.target->string.length;
	return def_add;
}

// <def_and> := <and_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_and(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_and aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_and = malloc(sizeof(*def_and));
	def_and->parent = parent;
	def_and->string.initial = aml.initial;
	def_and->string.length = 0;
	def_and->type = aml_def_and;
	def_and->flags = 0;
	def_and->component.def_and.and_op = analyse_aml_and_op(def_and, aml);
	def_and->string.length += def_and->component.def_and.and_op->string.length;
	aml.initial += def_and->component.def_and.and_op->string.length;
	aml.length -= def_and->component.def_and.and_op->string.length;
	for(AMLSymbol **operand = def_and->component.def_and.operand; operand != def_and->component.def_and.operand + _countof(def_and->component.def_and.operand); operand++)
	{
		*operand = analyse_aml_operand(def_and, aml);
		def_and->string.length += (*operand)->string.length;
		aml.initial += (*operand)->string.length;
		aml.length -= (*operand)->string.length;
	}
	def_and->component.def_and.target = analyse_aml_target(def_and, aml);
	def_and->string.length += def_and->component.def_and.target->string.length;
	aml.initial += def_and->component.def_and.target->string.length;
	aml.length -= def_and->component.def_and.target->string.length;
	return def_and;
}

// <def_break> := <break_op>
AMLSymbol *analyse_aml_def_break(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_break aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_break = malloc(sizeof(*def_break));
	def_break->parent = parent;
	def_break->string.initial = aml.initial;
	def_break->string.length = 0;
	def_break->type = aml_def_break;
	def_break->flags = 0;
	def_break->component.def_break.break_op = analyse_aml_break_op(def_break, aml);
	def_break->string.length += def_break->component.def_break.break_op->string.length;
	aml.initial += def_break->component.def_break.break_op->string.length;
	aml.length -= def_break->component.def_break.break_op->string.length;
	return def_break;
}

// <def_buffer> := <buffer_op> <pkg_length> <buffer_size> <byte_list>
AMLSymbol *analyse_aml_def_buffer(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_buffer aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_buffer = malloc(sizeof(*def_buffer));
	def_buffer->parent = parent;
	def_buffer->string.initial = aml.initial;
	def_buffer->string.length = 0;
	def_buffer->type = aml_def_buffer;
	def_buffer->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_buffer->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_buffer;
}

// <def_concat> := <concat_op> <data> <data> <target>
AMLSymbol *analyse_aml_def_concat(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_concat aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_concat = malloc(sizeof(*def_concat));
	def_concat->parent = parent;
	def_concat->string.initial = aml.initial;
	def_concat->string.length = 0;
	def_concat->type = aml_def_concat;
	def_concat->flags = 0;
	def_concat->component.def_concat.concat_op = analyse_aml_concat_op(def_concat, aml);
	def_concat->string.length += def_concat->component.def_concat.concat_op->string.length;
	aml.initial += def_concat->component.def_concat.concat_op->string.length;
	aml.length -= def_concat->component.def_concat.concat_op->string.length;
	for(unsigned int i = 0; i < _countof(def_concat->component.def_concat.data); i++)
	{
		def_concat->component.def_concat.data[i] = analyse_aml_data(def_concat, aml);
		def_concat->string.length += def_concat->component.def_concat.data[i]->string.length;
		aml.initial += def_concat->component.def_concat.data[i]->string.length;
		aml.length -= def_concat->component.def_concat.data[i]->string.length;
	}
	def_concat->component.def_concat.target = analyse_aml_target(def_concat, aml);
	def_concat->string.length += def_concat->component.def_concat.target->string.length;
	aml.initial += def_concat->component.def_concat.target->string.length;
	aml.length -= def_concat->component.def_concat.target->string.length;
	return def_concat;
}

// <def_concat_res> := <concat_res_op> <buf_data> <buf_data> <target>
AMLSymbol *analyse_aml_def_concat_res(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_concat_res aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_concat_res = malloc(sizeof(*def_concat_res));
	def_concat_res->parent = parent;
	def_concat_res->string.initial = aml.initial;
	def_concat_res->string.length = 0;
	def_concat_res->type = aml_def_concat_res;
	def_concat_res->flags = 0;
	def_concat_res->component.def_concat_res.concat_res_op = analyse_aml_concat_res_op(def_concat_res, aml);
	def_concat_res->string.length += def_concat_res->component.def_concat_res.concat_res_op->string.length;
	aml.initial += def_concat_res->component.def_concat_res.concat_res_op->string.length;
	aml.length -= def_concat_res->component.def_concat_res.concat_res_op->string.length;
	for(unsigned int i = 0; i < _countof(def_concat_res->component.def_concat_res.buf_data); i++)
	{
		def_concat_res->component.def_concat_res.buf_data[i] = analyse_aml_buf_data(def_concat_res, aml);
		def_concat_res->string.length += def_concat_res->component.def_concat_res.buf_data[i]->string.length;
		aml.initial += def_concat_res->component.def_concat_res.buf_data[i]->string.length;
		aml.length -= def_concat_res->component.def_concat_res.buf_data[i]->string.length;
	}
	def_concat_res->component.def_concat_res.target = analyse_aml_target(def_concat_res, aml);
	def_concat_res->string.length += def_concat_res->component.def_concat_res.target->string.length;
	aml.initial += def_concat_res->component.def_concat_res.target->string.length;
	aml.length -= def_concat_res->component.def_concat_res.target->string.length;
	return def_concat_res;
}

// <def_cond_ref_of> := <cond_ref_of_op> <super_name> <target>
AMLSymbol *analyse_aml_def_cond_ref_of(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_cond_ref_of aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_cond_ref_of = malloc(sizeof(*def_cond_ref_of));
	def_cond_ref_of->parent = parent;
	def_cond_ref_of->string.initial = aml.initial;
	def_cond_ref_of->string.length = 0;
	def_cond_ref_of->type = aml_def_cond_ref_of;
	def_cond_ref_of->flags = 0;
	def_cond_ref_of->component.def_cond_ref_of.cond_ref_of_op = analyse_aml_cond_ref_of_op(def_cond_ref_of, aml);
	def_cond_ref_of->string.length += def_cond_ref_of->component.def_cond_ref_of.cond_ref_of_op->string.length;
	aml.initial += def_cond_ref_of->component.def_cond_ref_of.cond_ref_of_op->string.length;
	aml.length -= def_cond_ref_of->component.def_cond_ref_of.cond_ref_of_op->string.length;
	def_cond_ref_of->component.def_cond_ref_of.super_name = analyse_aml_super_name(def_cond_ref_of, aml);
	def_cond_ref_of->string.length += def_cond_ref_of->component.def_cond_ref_of.super_name->string.length;
	aml.initial += def_cond_ref_of->component.def_cond_ref_of.super_name->string.length;
	aml.length -= def_cond_ref_of->component.def_cond_ref_of.super_name->string.length;
	def_cond_ref_of->component.def_cond_ref_of.target = analyse_aml_target(def_cond_ref_of, aml);
	def_cond_ref_of->string.length += def_cond_ref_of->component.def_cond_ref_of.target->string.length;
	aml.initial += def_cond_ref_of->component.def_cond_ref_of.target->string.length;
	aml.length -= def_cond_ref_of->component.def_cond_ref_of.target->string.length;
	return def_cond_ref_of;
}

// <def_create_bit_field> := <create_bit_field_op> <source_buff> <byte_index> <name_string>
AMLSymbol *analyse_aml_def_create_bit_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_bit_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_bit_field = malloc(sizeof(*def_create_bit_field));
	def_create_bit_field->parent = parent;
	def_create_bit_field->string.initial = aml.initial;
	def_create_bit_field->string.length = 0;
	def_create_bit_field->type = aml_def_create_bit_field;
	def_create_bit_field->flags = 0;
	def_create_bit_field->component.def_create_bit_field.create_bit_field_op = analyse_aml_create_bit_field_op(def_create_bit_field, aml);
	def_create_bit_field->string.length += def_create_bit_field->component.def_create_bit_field.create_bit_field_op->string.length;
	aml.initial += def_create_bit_field->component.def_create_bit_field.create_bit_field_op->string.length;
	aml.length -= def_create_bit_field->component.def_create_bit_field.create_bit_field_op->string.length;
	def_create_bit_field->component.def_create_bit_field.source_buff = analyse_aml_source_buff(def_create_bit_field, aml);
	def_create_bit_field->string.length += def_create_bit_field->component.def_create_bit_field.source_buff->string.length;
	aml.initial += def_create_bit_field->component.def_create_bit_field.source_buff->string.length;
	aml.length -= def_create_bit_field->component.def_create_bit_field.source_buff->string.length;
	def_create_bit_field->component.def_create_bit_field.byte_index = analyse_aml_byte_index(def_create_bit_field, aml);
	def_create_bit_field->string.length += def_create_bit_field->component.def_create_bit_field.byte_index->string.length;
	aml.initial += def_create_bit_field->component.def_create_bit_field.byte_index->string.length;
	aml.length -= def_create_bit_field->component.def_create_bit_field.byte_index->string.length;
	def_create_bit_field->component.def_create_bit_field.name_string = analyse_aml_name_string(def_create_bit_field, aml);
	def_create_bit_field->string.length += def_create_bit_field->component.def_create_bit_field.name_string->string.length;
	aml.initial += def_create_bit_field->component.def_create_bit_field.name_string->string.length;
	aml.length -= def_create_bit_field->component.def_create_bit_field.name_string->string.length;
	return def_create_bit_field;
}

// <def_create_byte_field> := <create_byte_field_op> <source_buff> <byte_index> <name_string>
AMLSymbol *analyse_aml_def_create_byte_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_byte_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_byte_field = malloc(sizeof(*def_create_byte_field));
	def_create_byte_field->parent = parent;
	def_create_byte_field->string.initial = aml.initial;
	def_create_byte_field->string.length = 0;
	def_create_byte_field->type = aml_def_create_byte_field;
	def_create_byte_field->flags = 0;
	def_create_byte_field->component.def_create_byte_field.create_byte_field_op = analyse_aml_create_byte_field_op(def_create_byte_field, aml);
	def_create_byte_field->string.length += def_create_byte_field->component.def_create_byte_field.create_byte_field_op->string.length;
	aml.initial += def_create_byte_field->component.def_create_byte_field.create_byte_field_op->string.length;
	aml.length -= def_create_byte_field->component.def_create_byte_field.create_byte_field_op->string.length;
	def_create_byte_field->component.def_create_byte_field.source_buff = analyse_aml_source_buff(def_create_byte_field, aml);
	def_create_byte_field->string.length += def_create_byte_field->component.def_create_byte_field.source_buff->string.length;
	aml.initial += def_create_byte_field->component.def_create_byte_field.source_buff->string.length;
	aml.length -= def_create_byte_field->component.def_create_byte_field.source_buff->string.length;
	def_create_byte_field->component.def_create_byte_field.byte_index = analyse_aml_byte_index(def_create_byte_field, aml);
	def_create_byte_field->string.length += def_create_byte_field->component.def_create_byte_field.byte_index->string.length;
	aml.initial += def_create_byte_field->component.def_create_byte_field.byte_index->string.length;
	aml.length -= def_create_byte_field->component.def_create_byte_field.byte_index->string.length;
	def_create_byte_field->component.def_create_byte_field.name_string = analyse_aml_name_string(def_create_byte_field, aml);
	def_create_byte_field->string.length += def_create_byte_field->component.def_create_byte_field.name_string->string.length;
	aml.initial += def_create_byte_field->component.def_create_byte_field.name_string->string.length;
	aml.length -= def_create_byte_field->component.def_create_byte_field.name_string->string.length;
	return def_create_byte_field;
}

// <def_create_dword_field> := <create_dword_field_op> <source_buff> <byte_index> <name_string>
AMLSymbol *analyse_aml_def_create_dword_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_dword_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_dword_field = malloc(sizeof(*def_create_dword_field));
	def_create_dword_field->parent = parent;
	def_create_dword_field->string.initial = aml.initial;
	def_create_dword_field->string.length = 0;
	def_create_dword_field->type = aml_def_create_dword_field;
	def_create_dword_field->flags = 0;
	def_create_dword_field->component.def_create_dword_field.create_dword_field_op = analyse_aml_create_dword_field_op(def_create_dword_field, aml);
	def_create_dword_field->string.length += def_create_dword_field->component.def_create_dword_field.create_dword_field_op->string.length;
	aml.initial += def_create_dword_field->component.def_create_dword_field.create_dword_field_op->string.length;
	aml.length -= def_create_dword_field->component.def_create_dword_field.create_dword_field_op->string.length;
	def_create_dword_field->component.def_create_dword_field.source_buff = analyse_aml_source_buff(def_create_dword_field, aml);
	def_create_dword_field->string.length += def_create_dword_field->component.def_create_dword_field.source_buff->string.length;
	aml.initial += def_create_dword_field->component.def_create_dword_field.source_buff->string.length;
	aml.length -= def_create_dword_field->component.def_create_dword_field.source_buff->string.length;
	def_create_dword_field->component.def_create_dword_field.byte_index = analyse_aml_byte_index(def_create_dword_field, aml);
	def_create_dword_field->string.length += def_create_dword_field->component.def_create_dword_field.byte_index->string.length;
	aml.initial += def_create_dword_field->component.def_create_dword_field.byte_index->string.length;
	aml.length -= def_create_dword_field->component.def_create_dword_field.byte_index->string.length;
	def_create_dword_field->component.def_create_dword_field.name_string = analyse_aml_name_string(def_create_dword_field, aml);
	def_create_dword_field->string.length += def_create_dword_field->component.def_create_dword_field.name_string->string.length;
	aml.initial += def_create_dword_field->component.def_create_dword_field.name_string->string.length;
	aml.length -= def_create_dword_field->component.def_create_dword_field.name_string->string.length;
	return def_create_dword_field;
}

// <def_create_field> := <create_field_op> <soutce_buff> <bit_index> <num_bits> <name_string>
AMLSymbol *analyse_aml_def_create_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_field = malloc(sizeof(*def_create_field));
	def_create_field->parent = parent;
	def_create_field->string.initial = aml.initial;
	def_create_field->string.length = 0;
	def_create_field->type = aml_def_create_field;
	def_create_field->flags = 0;
	def_create_field->component.def_create_field.create_field_op = analyse_aml_create_field_op(def_create_field, aml);
	def_create_field->string.length += def_create_field->component.def_create_field.create_field_op->string.length;
	aml.initial += def_create_field->component.def_create_field.create_field_op->string.length;
	aml.length -= def_create_field->component.def_create_field.create_field_op->string.length;
	def_create_field->component.def_create_field.source_buff = analyse_aml_source_buff(def_create_field, aml);
	def_create_field->string.length += def_create_field->component.def_create_field.source_buff->string.length;
	aml.initial += def_create_field->component.def_create_field.source_buff->string.length;
	aml.length -= def_create_field->component.def_create_field.source_buff->string.length;
	def_create_field->component.def_create_field.bit_index = analyse_aml_bit_index(def_create_field, aml);
	def_create_field->string.length += def_create_field->component.def_create_field.bit_index->string.length;
	aml.initial += def_create_field->component.def_create_field.bit_index->string.length;
	aml.length -= def_create_field->component.def_create_field.bit_index->string.length;
	def_create_field->component.def_create_field.num_bits = analyse_aml_num_bits(def_create_field, aml);
	def_create_field->string.length += def_create_field->component.def_create_field.num_bits->string.length;
	aml.initial += def_create_field->component.def_create_field.num_bits->string.length;
	aml.length -= def_create_field->component.def_create_field.num_bits->string.length;
	def_create_field->component.def_create_field.name_string = analyse_aml_name_string(def_create_field, aml);
	def_create_field->string.length += def_create_field->component.def_create_field.name_string->string.length;
	aml.initial += def_create_field->component.def_create_field.name_string->string.length;
	aml.length -= def_create_field->component.def_create_field.name_string->string.length;
	return def_create_field;
}

// <def_create_qword_field> := <create_qword_field_op> <source_buff> <byte_index> <name_string>
AMLSymbol *analyse_aml_def_create_qword_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_qword_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_qword_field = malloc(sizeof(*def_create_qword_field));
	def_create_qword_field->parent = parent;
	def_create_qword_field->string.initial = aml.initial;
	def_create_qword_field->string.length = 0;
	def_create_qword_field->type = aml_def_create_qword_field;
	def_create_qword_field->flags = 0;
	def_create_qword_field->component.def_create_qword_field.create_qword_field_op = analyse_aml_create_qword_field_op(def_create_qword_field, aml);
	def_create_qword_field->string.length += def_create_qword_field->component.def_create_qword_field.create_qword_field_op->string.length;
	aml.initial += def_create_qword_field->component.def_create_qword_field.create_qword_field_op->string.length;
	aml.length -= def_create_qword_field->component.def_create_qword_field.create_qword_field_op->string.length;
	def_create_qword_field->component.def_create_qword_field.source_buff = analyse_aml_source_buff(def_create_qword_field, aml);
	def_create_qword_field->string.length += def_create_qword_field->component.def_create_qword_field.source_buff->string.length;
	aml.initial += def_create_qword_field->component.def_create_qword_field.source_buff->string.length;
	aml.length -= def_create_qword_field->component.def_create_qword_field.source_buff->string.length;
	def_create_qword_field->component.def_create_qword_field.byte_index = analyse_aml_byte_index(def_create_qword_field, aml);
	def_create_qword_field->string.length += def_create_qword_field->component.def_create_qword_field.byte_index->string.length;
	aml.initial += def_create_qword_field->component.def_create_qword_field.byte_index->string.length;
	aml.length -= def_create_qword_field->component.def_create_qword_field.byte_index->string.length;
	def_create_qword_field->component.def_create_qword_field.name_string = analyse_aml_name_string(def_create_qword_field, aml);
	def_create_qword_field->string.length += def_create_qword_field->component.def_create_qword_field.name_string->string.length;
	aml.initial += def_create_qword_field->component.def_create_qword_field.name_string->string.length;
	aml.length -= def_create_qword_field->component.def_create_qword_field.name_string->string.length;
	return def_create_qword_field;
}

// <def_create_word_field> := <create_word_field_op> <source_buff> <byte_index> <name_string>
AMLSymbol *analyse_aml_def_create_word_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_create_word_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_create_word_field = malloc(sizeof(*def_create_word_field));
	def_create_word_field->parent = parent;
	def_create_word_field->string.initial = aml.initial;
	def_create_word_field->string.length = 0;
	def_create_word_field->type = aml_def_create_word_field;
	def_create_word_field->flags = 0;
	def_create_word_field->component.def_create_word_field.create_word_field_op = analyse_aml_create_word_field_op(def_create_word_field, aml);
	def_create_word_field->string.length += def_create_word_field->component.def_create_word_field.create_word_field_op->string.length;
	aml.initial += def_create_word_field->component.def_create_word_field.create_word_field_op->string.length;
	aml.length -= def_create_word_field->component.def_create_word_field.create_word_field_op->string.length;
	def_create_word_field->component.def_create_word_field.source_buff = analyse_aml_source_buff(def_create_word_field, aml);
	def_create_word_field->string.length += def_create_word_field->component.def_create_word_field.source_buff->string.length;
	aml.initial += def_create_word_field->component.def_create_word_field.source_buff->string.length;
	aml.length -= def_create_word_field->component.def_create_word_field.source_buff->string.length;
	def_create_word_field->component.def_create_word_field.byte_index = analyse_aml_byte_index(def_create_word_field, aml);
	def_create_word_field->string.length += def_create_word_field->component.def_create_word_field.byte_index->string.length;
	aml.initial += def_create_word_field->component.def_create_word_field.byte_index->string.length;
	aml.length -= def_create_word_field->component.def_create_word_field.byte_index->string.length;
	def_create_word_field->component.def_create_word_field.name_string = analyse_aml_name_string(def_create_word_field, aml);
	def_create_word_field->string.length += def_create_word_field->component.def_create_word_field.name_string->string.length;
	aml.initial += def_create_word_field->component.def_create_word_field.name_string->string.length;
	aml.length -= def_create_word_field->component.def_create_word_field.name_string->string.length;
	return def_create_word_field;
}

// <def_decrement> := <decrement_op> <super_name>
AMLSymbol *analyse_aml_def_decrement(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_decrement aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_decrement = malloc(sizeof(*def_decrement));
	def_decrement->parent = parent;
	def_decrement->string.initial = aml.initial;
	def_decrement->string.length = 0;
	def_decrement->type = aml_def_decrement;
	def_decrement->flags = 0;
	def_decrement->component.def_decrement.decrement_op = analyse_aml_decrement_op(def_decrement, aml);
	def_decrement->string.length += def_decrement->component.def_decrement.decrement_op->string.length;
	aml.initial += def_decrement->component.def_decrement.decrement_op->string.length;
	aml.length -= def_decrement->component.def_decrement.decrement_op->string.length;
	def_decrement->component.def_decrement.super_name = analyse_aml_super_name(def_decrement, aml);
	def_decrement->string.length += def_decrement->component.def_decrement.super_name->string.length;
	aml.initial += def_decrement->component.def_decrement.super_name->string.length;
	aml.length -= def_decrement->component.def_decrement.super_name->string.length;
	return def_decrement;
}

// <def_deref_of> := <deref_of_op> <obj_reference>
AMLSymbol *analyse_aml_def_deref_of(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_deref_of aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_deref_of = malloc(sizeof(*def_deref_of));
	def_deref_of->parent = parent;
	def_deref_of->string.initial = aml.initial;
	def_deref_of->string.length = 0;
	def_deref_of->type = aml_def_deref_of;
	def_deref_of->flags = 0;
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
	printf_serial("def_device aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_device = malloc(sizeof(*def_device));
	def_device->parent = parent;
	def_device->string.initial = aml.initial;
	def_device->string.length = 0;
	def_device->type = aml_def_device;
	def_device->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_device->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_device;
}

// <def_divide> := <divide_op> <dividend> <divisor> <remainder> <quotient>
AMLSymbol *analyse_aml_def_divide(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_divide aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_divide = malloc(sizeof(*def_divide));
	def_divide->parent = parent;
	def_divide->string.initial = aml.initial;
	def_divide->string.length = 0;
	def_divide->type = aml_def_divide;
	def_divide->flags = 0;
	def_divide->component.def_divide.divide_op = analyse_aml_divide_op(def_divide, aml);
	def_divide->string.length += def_divide->component.def_divide.divide_op->string.length;
	aml.initial += def_divide->component.def_divide.divide_op->string.length;
	aml.length -= def_divide->component.def_divide.divide_op->string.length;
	def_divide->component.def_divide.dividend = analyse_aml_dividend(def_divide, aml);
	def_divide->string.length += def_divide->component.def_divide.dividend->string.length;
	aml.initial += def_divide->component.def_divide.dividend->string.length;
	aml.length -= def_divide->component.def_divide.dividend->string.length;
	def_divide->component.def_divide.divisor = analyse_aml_divisor(def_divide, aml);
	def_divide->string.length += def_divide->component.def_divide.divisor->string.length;
	aml.initial += def_divide->component.def_divide.divisor->string.length;
	aml.length -= def_divide->component.def_divide.divisor->string.length;
	def_divide->component.def_divide.remainder = analyse_aml_remainder(def_divide, aml);
	def_divide->string.length += def_divide->component.def_divide.remainder->string.length;
	aml.initial += def_divide->component.def_divide.remainder->string.length;
	aml.length -= def_divide->component.def_divide.remainder->string.length;
	def_divide->component.def_divide.quotient = analyse_aml_quotient(def_divide, aml);
	def_divide->string.length += def_divide->component.def_divide.quotient->string.length;
	aml.initial += def_divide->component.def_divide.quotient->string.length;
	aml.length -= def_divide->component.def_divide.quotient->string.length;
	return def_divide;
}

// <def_else> := Nothing | <else_op> <pkg_length> <term_list>
AMLSymbol *analyse_aml_def_else(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_else aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_else = malloc(sizeof(*def_else));
	def_else->parent = parent;
	def_else->string.initial = aml.initial;
	def_else->string.length = 0;
	def_else->type = aml_def_else;
	def_else->flags = 0;
	def_else->component.def_else.else_op = NULL;
	def_else->component.def_else.pkg_length = NULL;
	def_else->component.def_else.term_list = NULL;
	if(aml.length && *aml.initial == AML_BYTE_ELSE_OP)
	{
		def_else->component.def_else.else_op = analyse_aml_else_op(def_else, aml);
		def_else->string.length += def_else->component.def_else.else_op->string.length;
		aml.initial += def_else->component.def_else.else_op->string.length;
		aml.length -= def_else->component.def_else.else_op->string.length;
		def_else->component.def_else.pkg_length = analyse_aml_pkg_length(def_else, aml);
		def_else->string.length += def_else->component.def_else.pkg_length->string.length;
		aml.initial += def_else->component.def_else.pkg_length->string.length;
		aml.length = def_else->component.def_else.pkg_length->component.pkg_length.length - def_else->component.def_else.pkg_length->string.length;
		def_else->component.def_else.term_list = analyse_aml_term_list(def_else, aml);
		def_else->string.length += def_else->component.def_else.term_list->string.length;
		aml.initial += def_else->component.def_else.term_list->string.length;
		aml.length -= def_else->component.def_else.term_list->string.length;
		if((int)aml.length < 0)
		{
			def_else->flags |= AML_SYMBOL_ERROR; // Length error
			ERROR();
		}
	}
	return def_else;
}

// <def_field> := <field_op> <pkg_length> <name_string> <field_flags> <field_list>
AMLSymbol *analyse_aml_def_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_field = malloc(sizeof(*def_field));
	def_field->parent = parent;
	def_field->string.initial = aml.initial;
	def_field->string.length = 0;
	def_field->type = aml_def_field;
	def_field->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_field->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_field;
}

// <def_find_set_left_bit> := <find_set_left_bitop> <operand> <target>
AMLSymbol *analyse_aml_def_find_set_left_bit(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_find_set_left_bit aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_find_set_left_bit = malloc(sizeof(*def_find_set_left_bit));
	def_find_set_left_bit->parent = parent;
	def_find_set_left_bit->string.initial = aml.initial;
	def_find_set_left_bit->string.length = 0;
	def_find_set_left_bit->flags = 0;
	def_find_set_left_bit->component.def_find_set_left_bit.find_set_left_bit_op = analyse_aml_find_set_left_bit_op(def_find_set_left_bit, aml);
	def_find_set_left_bit->string.length += def_find_set_left_bit->component.def_find_set_left_bit.find_set_left_bit_op->string.length;
	aml.initial += def_find_set_left_bit->component.def_find_set_left_bit.find_set_left_bit_op->string.length;
	aml.length -= def_find_set_left_bit->component.def_find_set_left_bit.find_set_left_bit_op->string.length;
	def_find_set_left_bit->component.def_find_set_left_bit.operand = analyse_aml_operand(def_find_set_left_bit, aml);
	def_find_set_left_bit->string.length += def_find_set_left_bit->component.def_find_set_left_bit.operand->string.length;
	aml.initial += def_find_set_left_bit->component.def_find_set_left_bit.operand->string.length;
	aml.length -= def_find_set_left_bit->component.def_find_set_left_bit.operand->string.length;
	def_find_set_left_bit->component.def_find_set_left_bit.target = analyse_aml_target(def_find_set_left_bit, aml);
	def_find_set_left_bit->string.length += def_find_set_left_bit->component.def_find_set_left_bit.target->string.length;
	aml.initial += def_find_set_left_bit->component.def_find_set_left_bit.target->string.length;
	aml.length -= def_find_set_left_bit->component.def_find_set_left_bit.target->string.length;
	return def_find_set_left_bit;
}

// <def_find_set_right_bit> := <find_set_right_bitop> <operand> <target>
AMLSymbol *analyse_aml_def_find_set_right_bit(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_find_set_right_bit aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_find_set_right_bit = malloc(sizeof(*def_find_set_right_bit));
	def_find_set_right_bit->parent = parent;
	def_find_set_right_bit->string.initial = aml.initial;
	def_find_set_right_bit->string.length = 0;
	def_find_set_right_bit->flags = 0;
	def_find_set_right_bit->component.def_find_set_right_bit.find_set_right_bit_op = analyse_aml_find_set_right_bit_op(def_find_set_right_bit, aml);
	def_find_set_right_bit->string.length += def_find_set_right_bit->component.def_find_set_right_bit.find_set_right_bit_op->string.length;
	aml.initial += def_find_set_right_bit->component.def_find_set_right_bit.find_set_right_bit_op->string.length;
	aml.length -= def_find_set_right_bit->component.def_find_set_right_bit.find_set_right_bit_op->string.length;
	def_find_set_right_bit->component.def_find_set_right_bit.operand = analyse_aml_operand(def_find_set_right_bit, aml);
	def_find_set_right_bit->string.length += def_find_set_right_bit->component.def_find_set_right_bit.operand->string.length;
	aml.initial += def_find_set_right_bit->component.def_find_set_right_bit.operand->string.length;
	aml.length -= def_find_set_right_bit->component.def_find_set_right_bit.operand->string.length;
	def_find_set_right_bit->component.def_find_set_right_bit.target = analyse_aml_target(def_find_set_right_bit, aml);
	def_find_set_right_bit->string.length += def_find_set_right_bit->component.def_find_set_right_bit.target->string.length;
	aml.initial += def_find_set_right_bit->component.def_find_set_right_bit.target->string.length;
	aml.length -= def_find_set_right_bit->component.def_find_set_right_bit.target->string.length;
	return def_find_set_right_bit;
}

// <def_if_else> := <if_op> <pkg_length> <predicate> <term_list> <def_else>
AMLSymbol *analyse_aml_def_if_else(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_if_else aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_if_else = malloc(sizeof(*def_if_else));
	AMLSubstring if_aml = aml;
	AMLSubstring else_aml = aml;
	def_if_else->parent = parent;
	def_if_else->string.initial = if_aml.initial;
	def_if_else->string.length = 0;
	def_if_else->type = aml_def_if_else;
	def_if_else->flags = 0;
	def_if_else->component.def_if_else.if_op = NULL;
	def_if_else->component.def_if_else.pkg_length = NULL;
	def_if_else->component.def_if_else.predicate = NULL;
	def_if_else->component.def_if_else.term_list = NULL;
	def_if_else->component.def_if_else.def_else = NULL;
	def_if_else->component.def_if_else.if_op = analyse_aml_if_op(def_if_else, if_aml);
	def_if_else->string.length += def_if_else->component.def_if_else.if_op->string.length;
	if_aml.initial += def_if_else->component.def_if_else.if_op->string.length;
	if_aml.length -= def_if_else->component.def_if_else.if_op->string.length;
	else_aml.initial += def_if_else->component.def_if_else.if_op->string.length;
	else_aml.length -= def_if_else->component.def_if_else.if_op->string.length;
	def_if_else->component.def_if_else.pkg_length = analyse_aml_pkg_length(def_if_else, if_aml);
	def_if_else->string.length += def_if_else->component.def_if_else.pkg_length->string.length;
	if_aml.initial += def_if_else->component.def_if_else.pkg_length->string.length;
	if_aml.length = def_if_else->component.def_if_else.pkg_length->component.pkg_length.length - def_if_else->component.def_if_else.pkg_length->string.length;
	else_aml.initial += def_if_else->component.def_if_else.pkg_length->string.length;
	else_aml.length -= def_if_else->component.def_if_else.pkg_length->string.length;
	def_if_else->component.def_if_else.predicate = analyse_aml_predicate(def_if_else, if_aml);
	def_if_else->string.length += def_if_else->component.def_if_else.predicate->string.length;
	if_aml.initial += def_if_else->component.def_if_else.predicate->string.length;
	if_aml.length -= def_if_else->component.def_if_else.predicate->string.length;
	else_aml.initial += def_if_else->component.def_if_else.predicate->string.length;
	else_aml.length -= def_if_else->component.def_if_else.predicate->string.length;
	def_if_else->component.def_if_else.term_list = analyse_aml_term_list(def_if_else, if_aml);
	def_if_else->string.length += def_if_else->component.def_if_else.term_list->string.length;
	if_aml.initial += def_if_else->component.def_if_else.term_list->string.length;
	if_aml.length -= def_if_else->component.def_if_else.term_list->string.length;
	else_aml.initial += def_if_else->component.def_if_else.term_list->string.length;
	else_aml.length -= def_if_else->component.def_if_else.term_list->string.length;
	def_if_else->component.def_if_else.def_else = analyse_aml_def_else(def_if_else, else_aml);
	def_if_else->string.length += def_if_else->component.def_if_else.def_else->string.length;
	else_aml.initial += def_if_else->component.def_if_else.def_else->string.length;
	else_aml.length -= def_if_else->component.def_if_else.def_else->string.length;
	if((int)if_aml.length < 0)
	{
		def_if_else->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	if((int)else_aml.length < 0)
	{
		def_if_else->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_if_else;
}

// <def_increment> := <increment_op> <super_name>
AMLSymbol *analyse_aml_def_increment(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_increment aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_increment = malloc(sizeof(*def_increment));
	def_increment->parent = parent;
	def_increment->string.initial = aml.initial;
	def_increment->string.length = 0;
	def_increment->type = aml_def_increment;
	def_increment->flags = 0;
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
	printf_serial("def_index aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_index = malloc(sizeof(*def_index));
	def_index->parent = parent;
	def_index->string.initial = aml.initial;
	def_index->string.length = 0;
	def_index->type = aml_def_index;
	def_index->flags = 0;
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

// <def_index_field> := <index_field_op> <pkg_length> <name_string> <name_string> <field_flags> <field_list>
AMLSymbol *analyse_aml_def_index_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_index_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_index_field = malloc(sizeof(*def_index_field));
	def_index_field->parent = parent;
	def_index_field->string.initial = aml.initial;
	def_index_field->string.length = 0;
	def_index_field->type = aml_def_index_field;
	def_index_field->flags = 0;
	def_index_field->component.def_index_field.index_field_op = analyse_aml_index_field_op(def_index_field, aml);
	def_index_field->string.length += def_index_field->component.def_index_field.index_field_op->string.length;
	aml.initial += def_index_field->component.def_index_field.index_field_op->string.length;
	aml.length += def_index_field->component.def_index_field.index_field_op->string.length;
	def_index_field->component.def_index_field.pkg_length = analyse_aml_pkg_length(def_index_field, aml);
	def_index_field->string.length += def_index_field->component.def_index_field.pkg_length->string.length;
	aml.initial += def_index_field->component.def_index_field.pkg_length->string.length;
	aml.length += def_index_field->component.def_index_field.pkg_length->string.length;
	for(unsigned int i = 0; i < _countof(def_index_field->component.def_index_field.name_string); i++)
	{
		def_index_field->component.def_index_field.name_string[i] = analyse_aml_name_string(def_index_field, aml);
		def_index_field->string.length += def_index_field->component.def_index_field.name_string[i]->string.length;
		aml.initial += def_index_field->component.def_index_field.name_string[i]->string.length;
		aml.length += def_index_field->component.def_index_field.name_string[i]->string.length;
	}
	def_index_field->component.def_index_field.field_flags = analyse_aml_field_flags(def_index_field, aml);
	def_index_field->string.length += def_index_field->component.def_index_field.field_flags->string.length;
	aml.initial += def_index_field->component.def_index_field.field_flags->string.length;
	aml.length += def_index_field->component.def_index_field.field_flags->string.length;
	def_index_field->component.def_index_field.field_list = analyse_aml_field_list(def_index_field, aml);
	def_index_field->string.length += def_index_field->component.def_index_field.field_list->string.length;
	aml.initial += def_index_field->component.def_index_field.field_list->string.length;
	aml.length += def_index_field->component.def_index_field.field_list->string.length;
	return def_index_field;
}

// <def_l_and> := <l_and_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_and(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_l_and aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_and = malloc(sizeof(*def_l_and));
	def_l_and->parent = parent;
	def_l_and->string.initial = aml.initial;
	def_l_and->string.length = 0;
	def_l_and->type = aml_def_l_and;
	def_l_and->flags = 0;
	def_l_and->component.def_l_and.l_and_op = analyse_aml_l_and_op(def_l_and, aml);
	def_l_and->string.length += def_l_and->component.def_l_and.l_and_op->string.length;
	aml.initial += def_l_and->component.def_l_and.l_and_op->string.length;
	aml.length -= def_l_and->component.def_l_and.l_and_op->string.length;
	for(unsigned int i = 0; i < _countof(def_l_and->component.def_l_and.operand); i++)
	{
		def_l_and->component.def_l_and.operand[i] = analyse_aml_operand(def_l_and, aml);
		def_l_and->string.length += def_l_and->component.def_l_and.operand[i]->string.length;
		aml.initial += def_l_and->component.def_l_and.operand[i]->string.length;
		aml.length -= def_l_and->component.def_l_and.operand[i]->string.length;
	}
	return def_l_and;
}

// <def_l_equal> := <l_equal_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_equal(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_l_equal aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_equal = malloc(sizeof(*def_l_equal));
	def_l_equal->parent = parent;
	def_l_equal->string.initial = aml.initial;
	def_l_equal->string.length = 0;
	def_l_equal->type = aml_def_l_equal;
	def_l_equal->flags = 0;
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
	printf_serial("def_l_greater aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_greater = malloc(sizeof(*def_l_greater));
	def_l_greater->parent = parent;
	def_l_greater->string.initial = aml.initial;
	def_l_greater->string.length = 0;
	def_l_greater->type = aml_def_l_greater;
	def_l_greater->flags = 0;
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
	printf_serial("def_l_less aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_less = malloc(sizeof(*def_l_less));
	def_l_less->parent = parent;
	def_l_less->string.initial = aml.initial;
	def_l_less->string.length = 0;
	def_l_less->type = aml_def_l_less;
	def_l_less->flags = 0;
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

// <def_l_not> := <l_not_op> <operand>
AMLSymbol *analyse_aml_def_l_not(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_l_not aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_not = malloc(sizeof(*def_l_not));
	def_l_not->parent = parent;
	def_l_not->string.initial = aml.initial;
	def_l_not->string.length = 0;
	def_l_not->type = aml_def_l_not;
	def_l_not->type = 0;
	def_l_not->component.def_l_not.l_not_op = analyse_aml_l_not_op(def_l_not, aml);
	def_l_not->string.length += def_l_not->component.def_l_not.l_not_op->string.length;
	aml.initial += def_l_not->component.def_l_not.l_not_op->string.length;
	aml.length -= def_l_not->component.def_l_not.l_not_op->string.length;
	def_l_not->component.def_l_not.operand = analyse_aml_operand(def_l_not, aml);
	def_l_not->string.length += def_l_not->component.def_l_not.operand->string.length;
	aml.initial += def_l_not->component.def_l_not.operand->string.length;
	aml.length -= def_l_not->component.def_l_not.operand->string.length;
	return def_l_not;
}

// <def_l_or> := <l_or_op> <operand> <operand>
AMLSymbol *analyse_aml_def_l_or(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_l_or aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_l_or = malloc(sizeof(*def_l_or));
	def_l_or->parent = parent;
	def_l_or->string.initial = aml.initial;
	def_l_or->string.length = 0;
	def_l_or->type = aml_def_l_or;
	def_l_or->flags = 0;
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

// <def_match> := <match_op> <serach_pkg> <match_opcode> <operand> <match_opcode> <operand> <start_index>
AMLSymbol *analyse_aml_def_match(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_match aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_match = malloc(sizeof(*def_match));
	def_match->parent = parent;
	def_match->string.initial = aml.initial;
	def_match->string.length = 0;
	def_match->type = aml_def_match;
	def_match->component.def_match.match_op = analyse_aml_match_op(def_match, aml);
	def_match->string.length += def_match->component.def_match.match_op->string.length;
	aml.initial += def_match->component.def_match.match_op->string.length;
	aml.length -= def_match->component.def_match.match_op->string.length;
	def_match->component.def_match.search_pkg = analyse_aml_search_pkg(def_match, aml);
	def_match->string.length += def_match->component.def_match.search_pkg->string.length;
	aml.initial += def_match->component.def_match.search_pkg->string.length;
	aml.length -= def_match->component.def_match.search_pkg->string.length;
	def_match->component.def_match.match_opcode[0] = analyse_aml_match_opcode(def_match, aml);
	def_match->string.length += def_match->component.def_match.match_opcode[0]->string.length;
	aml.initial += def_match->component.def_match.match_opcode[0]->string.length;
	aml.length -= def_match->component.def_match.match_opcode[0]->string.length;
	def_match->component.def_match.operand[0] = analyse_aml_operand(def_match, aml);
	def_match->string.length += def_match->component.def_match.operand[0]->string.length;
	aml.initial += def_match->component.def_match.operand[0]->string.length;
	aml.length -= def_match->component.def_match.operand[0]->string.length;
	def_match->component.def_match.match_opcode[1] = analyse_aml_match_opcode(def_match, aml);
	def_match->string.length += def_match->component.def_match.match_opcode[1]->string.length;
	aml.initial += def_match->component.def_match.match_opcode[1]->string.length;
	aml.length -= def_match->component.def_match.match_opcode[1]->string.length;
	def_match->component.def_match.operand[1] = analyse_aml_operand(def_match, aml);
	def_match->string.length += def_match->component.def_match.operand[1]->string.length;
	aml.initial += def_match->component.def_match.operand[1]->string.length;
	aml.length -= def_match->component.def_match.operand[1]->string.length;
	def_match->component.def_match.start_index = analyse_aml_start_index(def_match, aml);
	def_match->string.length += def_match->component.def_match.start_index->string.length;
	aml.initial += def_match->component.def_match.start_index->string.length;
	aml.length -= def_match->component.def_match.start_index->string.length;
	return def_match;
}

// <def_method> := <method_op> <pkg_length> <name_string> <method_flags> <term_list>
AMLSymbol *analyse_aml_def_method(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_method aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_method = malloc(sizeof(*def_method));
	def_method->parent = parent;
	def_method->string.initial = aml.initial;
	def_method->string.length = 0;
	def_method->type = aml_def_method;
	def_method->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_method->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	printf_serial("Method \"%s\" is defined.\n", def_method->component.def_method.name_string->component.name_string.string);
	return def_method;
}

// <def_mid> := <mid_op> <mid_obj> <term_arg> <term_arg> <target>
AMLSymbol *analyse_aml_def_mid(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_mid aml.length %#010.8x\n", aml.length);
	AMLSymbol *def_mid = malloc(sizeof(*def_mid));
	def_mid->parent = parent;
	def_mid->string.initial = aml.initial;
	def_mid->string.length = 0;
	def_mid->type = aml_def_mid;
	def_mid->flags = 0;
	def_mid->component.def_mid.mid_op = analyse_aml_mid_op(def_mid, aml);
	def_mid->string.length += def_mid->component.def_mid.mid_op->string.length;
	aml.initial += def_mid->component.def_mid.mid_op->string.length;
	aml.length -= def_mid->component.def_mid.mid_op->string.length;
	def_mid->component.def_mid.mid_obj = analyse_aml_mid_obj(def_mid, aml);
	def_mid->string.length += def_mid->component.def_mid.mid_obj->string.length;
	aml.initial += def_mid->component.def_mid.mid_obj->string.length;
	aml.length -= def_mid->component.def_mid.mid_obj->string.length;
	for(unsigned int i = 0; i < _countof(def_mid->component.def_mid.term_arg); i++)
	{
		def_mid->component.def_mid.term_arg[i] = analyse_aml_term_arg(def_mid, aml);
		def_mid->string.length += def_mid->component.def_mid.term_arg[i]->string.length;
		aml.initial += def_mid->component.def_mid.term_arg[i]->string.length;
		aml.length -= def_mid->component.def_mid.term_arg[i]->string.length;
	}
	def_mid->component.def_mid.target = analyse_aml_target(def_mid, aml);
	def_mid->string.length += def_mid->component.def_mid.target->string.length;
	aml.initial += def_mid->component.def_mid.target->string.length;
	aml.length -= def_mid->component.def_mid.target->string.length;
	return def_mid;
}

// <def_multiply> := <multiply_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_multiply(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_multiply aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_multiply = malloc(sizeof(*def_multiply));
	def_multiply->parent = parent;
	def_multiply->string.initial = aml.initial;
	def_multiply->string.length = 0;
	def_multiply->type = aml_def_multiply;
	def_multiply->flags = 0;
	def_multiply->component.def_multiply.multiply_op = analyse_aml_multiply_op(def_multiply, aml);
	def_multiply->string.length += def_multiply->component.def_multiply.multiply_op->string.length;
	aml.initial += def_multiply->component.def_multiply.multiply_op->string.length;
	aml.length -= def_multiply->component.def_multiply.multiply_op->string.length;
	for(unsigned int i = 0; i < _countof(def_multiply->component.def_multiply.operand); i++)
	{
		def_multiply->component.def_multiply.operand[i] = analyse_aml_operand(def_multiply, aml);
		def_multiply->string.length += def_multiply->component.def_multiply.operand[i]->string.length;
		aml.initial += def_multiply->component.def_multiply.operand[i]->string.length;
		aml.length -= def_multiply->component.def_multiply.operand[i]->string.length;
	}
	def_multiply->component.def_multiply.target = analyse_aml_target(def_multiply, aml);
	def_multiply->string.length += def_multiply->component.def_multiply.target->string.length;
	aml.initial += def_multiply->component.def_multiply.target->string.length;
	aml.length -= def_multiply->component.def_multiply.target->string.length;
	return def_multiply;
}

// <def_mutex> := <mutex_op> <name_string> <sync_flags>
AMLSymbol *analyse_aml_def_mutex(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_mutex aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_mutex = malloc(sizeof(*def_mutex));
	def_mutex->parent = parent;
	def_mutex->string.initial = aml.initial;
	def_mutex->string.length = 0;
	def_mutex->type = aml_def_mutex;
	def_mutex->flags = 0;
	def_mutex->component.def_mutex.mutex_op = analyse_aml_mutex_op(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.mutex_op->string.length;
	aml.initial += def_mutex->component.def_mutex.mutex_op->string.length;
	aml.length -= def_mutex->component.def_mutex.mutex_op->string.length;
	def_mutex->component.def_mutex.name_string = analyse_aml_name_string(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.name_string->string.length;
	aml.initial += def_mutex->component.def_mutex.name_string->string.length;
	aml.length -= def_mutex->component.def_mutex.name_string->string.length;
	def_mutex->component.def_mutex.sync_flags = analyse_aml_sync_flags(def_mutex, aml);
	def_mutex->string.length += def_mutex->component.def_mutex.sync_flags->string.length;
	aml.initial += def_mutex->component.def_mutex.sync_flags->string.length;
	aml.length -= def_mutex->component.def_mutex.sync_flags->string.length;
	return def_mutex;
}

// <def_name> := <name_op> <name_string> <data_ref_object>
AMLSymbol *analyse_aml_def_name(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_name aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_name = malloc(sizeof(*def_name));
	def_name->parent = parent;
	def_name->string.initial = aml.initial;
	def_name->string.length = 0;
	def_name->type = aml_def_name;
	def_name->flags = 0;
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

// <def_not> := <not_op> <operand> <target>
AMLSymbol *analyse_aml_def_not(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_not aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_not = malloc(sizeof(*def_not));
	def_not->parent = parent;
	def_not->string.initial = aml.initial;
	def_not->string.length = 0;
	def_not->type = aml_def_not;
	def_not->flags = 0;
	def_not->component.def_not.not_op = analyse_aml_not_op(def_not, aml);
	def_not->string.length += def_not->component.def_not.not_op->string.length;
	aml.initial += def_not->component.def_not.not_op->string.length;
	aml.length -= def_not->component.def_not.not_op->string.length;
	def_not->component.def_not.operand = analyse_aml_operand(def_not, aml);
	def_not->string.length += def_not->component.def_not.operand->string.length;
	aml.initial += def_not->component.def_not.operand->string.length;
	aml.length -= def_not->component.def_not.operand->string.length;
	def_not->component.def_not.target = analyse_aml_target(def_not, aml);
	def_not->string.length += def_not->component.def_not.target->string.length;
	aml.initial += def_not->component.def_not.target->string.length;
	aml.length -= def_not->component.def_not.target->string.length;
	return def_not;
}

// <def_notify> := <notify_op> <notify_object> <notify_value>
AMLSymbol *analyse_aml_def_notify(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_notify aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_notify = malloc(sizeof(*def_notify));
	def_notify->parent = parent;
	def_notify->string.initial = aml.initial;
	def_notify->string.length = 0;
	def_notify->type = aml_def_notify;
	def_notify->flags = 0;
	def_notify->component.def_notify.notify_op = analyse_aml_notify_op(def_notify, aml);
	def_notify->string.length += def_notify->component.def_notify.notify_op->string.length;
	aml.initial += def_notify->component.def_notify.notify_op->string.length;
	aml.length -= def_notify->component.def_notify.notify_op->string.length;
	def_notify->component.def_notify.notify_object = analyse_aml_notify_object(def_notify, aml);
	def_notify->string.length += def_notify->component.def_notify.notify_object->string.length;
	aml.initial += def_notify->component.def_notify.notify_object->string.length;
	aml.length -= def_notify->component.def_notify.notify_object->string.length;
	def_notify->component.def_notify.notify_value = analyse_aml_notify_value(def_notify, aml);
	def_notify->string.length += def_notify->component.def_notify.notify_value->string.length;
	aml.initial += def_notify->component.def_notify.notify_value->string.length;
	aml.length -= def_notify->component.def_notify.notify_value->string.length;
	return def_notify;
}

// <def_object_type> := <object_type_op> (<simple_name> | <debug_obj> | <def_ref_of> | <def_deref_of> | <def_index>)
AMLSymbol *analyse_aml_def_object_type(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_object_type aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_object_type = malloc(sizeof(*def_object_type));
	def_object_type->parent = parent;
	def_object_type->string.initial = aml.initial;
	def_object_type->string.length = 0;
	def_object_type->type = aml_def_object_type;
	def_object_type->flags = 0;
	def_object_type->component.def_object_type.object_type_op = analyse_aml_object_type_op(def_object_type, aml);
	def_object_type->string.length += def_object_type->component.def_object_type.object_type_op->string.length;
	aml.initial += def_object_type->component.def_object_type.object_type_op->string.length;
	aml.length -= def_object_type->component.def_object_type.object_type_op->string.length;
	def_object_type->component.def_object_type.simple_name = NULL;
	def_object_type->component.def_object_type.debug_obj = NULL;
	def_object_type->component.def_object_type.def_ref_of = NULL;
	def_object_type->component.def_object_type.def_deref_of = NULL;
	def_object_type->component.def_object_type.def_index = NULL;
	if(aml.length)switch(*aml.initial)
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
		def_object_type->component.def_object_type.simple_name = analyse_aml_simple_name(def_object_type, aml);
		def_object_type->string.length += def_object_type->component.def_object_type.simple_name->string.length;
		aml.initial += def_object_type->component.def_object_type.simple_name->string.length;
		aml.length -= def_object_type->component.def_object_type.simple_name->string.length;
		break;
	case AML_BYTE_DEBUG_OP:
		def_object_type->component.def_object_type.debug_obj = analyse_aml_debug_obj(def_object_type, aml);
		def_object_type->string.length += def_object_type->component.def_object_type.debug_obj->string.length;
		aml.initial += def_object_type->component.def_object_type.debug_obj->string.length;
		aml.length -= def_object_type->component.def_object_type.debug_obj->string.length;
		break;
	case AML_BYTE_DEREF_OF_OP:
		def_object_type->component.def_object_type.def_deref_of = analyse_aml_def_deref_of(def_object_type, aml);
		def_object_type->string.length += def_object_type->component.def_object_type.def_deref_of->string.length;
		aml.initial += def_object_type->component.def_object_type.def_deref_of->string.length;
		aml.length -= def_object_type->component.def_object_type.def_deref_of->string.length;
		break;
	case AML_BYTE_INDEX_OP:
		def_object_type->component.def_object_type.def_index = analyse_aml_def_index(def_object_type, aml);
		def_object_type->string.length += def_object_type->component.def_object_type.def_index->string.length;
		aml.initial += def_object_type->component.def_object_type.def_index->string.length;
		aml.length -= def_object_type->component.def_object_type.def_index->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			def_object_type->component.def_object_type.simple_name = analyse_aml_simple_name(def_object_type, aml);
			def_object_type->string.length += def_object_type->component.def_object_type.simple_name->string.length;
			aml.initial += def_object_type->component.def_object_type.simple_name->string.length;
			aml.length -= def_object_type->component.def_object_type.simple_name->string.length;
		}
		else
		{
			def_object_type->flags |= AML_SYMBOL_ERROR;
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	else
	{
		def_object_type->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return def_object_type;
}

// <def_op_region> := <op_region_op> <name_string> <region_space> <region_offset> <region_len>
AMLSymbol *analyse_aml_def_op_region(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_op_region aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_op_region = malloc(sizeof(*def_op_region));
	def_op_region->parent = parent;
	def_op_region->string.initial = aml.initial;
	def_op_region->string.length = 0;
	def_op_region->type = aml_def_op_region;
	def_op_region->flags = 0;
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

// <def_or> := <or_op> <operand> <operand> <target>
AMLSymbol *analyse_aml_def_or(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_or aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_or = malloc(sizeof(*def_or));
	def_or->parent = parent;
	def_or->string.initial = aml.initial;
	def_or->string.length = 0;
	def_or->type = aml_def_or;
	def_or->flags = 0;
	def_or->component.def_or.or_op = analyse_aml_or_op(def_or, aml);
	def_or->string.length += def_or->component.def_or.or_op->string.length;
	aml.initial += def_or->component.def_or.or_op->string.length;
	aml.length -= def_or->component.def_or.or_op->string.length;
	for(AMLSymbol **operand = def_or->component.def_or.operand; operand != def_or->component.def_or.operand + _countof(def_or->component.def_or.operand); operand++)
	{
		*operand = analyse_aml_operand(def_or, aml);
		def_or->string.length += (*operand)->string.length;
		aml.initial += (*operand)->string.length;
		aml.length -= (*operand)->string.length;
	}
	def_or->component.def_or.target = analyse_aml_target(def_or, aml);
	def_or->string.length += def_or->component.def_or.target->string.length;
	aml.initial += def_or->component.def_or.target->string.length;
	aml.length -= def_or->component.def_or.target->string.length;
	return def_or;
}

// <def_package> := <package_op> <pkg_length> <num_elements> <package_element_list>
AMLSymbol *analyse_aml_def_package(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_package aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_package = malloc(sizeof(*def_package));
	def_package->parent = parent;
	def_package->string.initial = aml.initial;
	def_package->string.length = 0;
	def_package->type = aml_def_package;
	def_package->flags = 0;
	def_package->component.def_package.package_op = analyse_aml_package_op(def_package, aml);
	def_package->string.length += def_package->component.def_package.package_op->string.length;
	aml.initial += def_package->component.def_package.package_op->string.length;
	aml.length -= def_package->component.def_package.package_op->string.length;
	def_package->component.def_package.pkg_length = analyse_aml_pkg_length(def_package, aml);
	def_package->string.length += def_package->component.def_package.pkg_length->string.length;
	aml.initial += def_package->component.def_package.pkg_length->string.length;
	aml.length = def_package->component.def_package.pkg_length->component.pkg_length.length - def_package->component.def_package.pkg_length->string.length;
	def_package->component.def_package.num_elements = analyse_aml_num_elements(def_package, aml);
	def_package->string.length += def_package->component.def_package.num_elements->string.length;
	aml.initial += def_package->component.def_package.num_elements->string.length;
	aml.length -= def_package->component.def_package.num_elements->string.length;
	def_package->component.def_package.package_element_list = analyse_aml_package_element_list(def_package, aml);
	def_package->string.length += def_package->component.def_package.package_element_list->string.length;
	aml.initial += def_package->component.def_package.package_element_list->string.length;
	aml.length -= def_package->component.def_package.package_element_list->string.length;
	if((int)aml.length < 0)
	{
		def_package->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_package;
}

// <def_processor> := <processor_op> <pkg_length> <name_string> <proc_id> <pblk_addr> <pblk_len> <term_list>
AMLSymbol *analyse_aml_def_processor(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_processor aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_processor = malloc(sizeof(*def_processor));
	def_processor->parent = parent;
	def_processor->string.initial = aml.initial;
	def_processor->string.length = 0;
	def_processor->type = aml_def_processor;
	def_processor->flags = 0;
	def_processor->component.def_processor.processor_op = analyse_aml_processor_op(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.processor_op->string.length;
	aml.initial += def_processor->component.def_processor.processor_op->string.length;
	aml.length -= def_processor->component.def_processor.processor_op->string.length;
	def_processor->component.def_processor.pkg_length = analyse_aml_pkg_length(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.pkg_length->string.length;
	aml.initial += def_processor->component.def_processor.pkg_length->string.length;
	aml.length = def_processor->component.def_processor.pkg_length->component.pkg_length.length - def_processor->component.def_processor.pkg_length->string.length;
	def_processor->component.def_processor.name_string = analyse_aml_name_string(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.name_string->string.length;
	aml.initial += def_processor->component.def_processor.name_string->string.length;
	aml.length -= def_processor->component.def_processor.name_string->string.length;
	def_processor->component.def_processor.proc_id = analyse_aml_proc_id(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.proc_id->string.length;
	aml.initial += def_processor->component.def_processor.proc_id->string.length;
	aml.length -= def_processor->component.def_processor.proc_id->string.length;
	def_processor->component.def_processor.pblk_addr = analyse_aml_pblk_addr(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.pblk_addr->string.length;
	aml.initial += def_processor->component.def_processor.pblk_addr->string.length;
	aml.length -= def_processor->component.def_processor.pblk_addr->string.length;
	def_processor->component.def_processor.pblk_len = analyse_aml_pblk_len(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.pblk_len->string.length;
	aml.initial += def_processor->component.def_processor.pblk_len->string.length;
	aml.length -= def_processor->component.def_processor.pblk_len->string.length;
	def_processor->component.def_processor.term_list = analyse_aml_term_list(def_processor, aml);
	def_processor->string.length += def_processor->component.def_processor.term_list->string.length;
	aml.initial += def_processor->component.def_processor.term_list->string.length;
	aml.length -= def_processor->component.def_processor.term_list->string.length;
	return def_processor;
}

// <def_ref_of> := <ref_of_op> <super_name>
AMLSymbol *analyse_aml_def_ref_of(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_ref_of aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_ref_of = malloc(sizeof(*def_ref_of));
	def_ref_of->parent = parent;
	def_ref_of->string.initial = aml.initial;
	def_ref_of->string.length = 0;
	def_ref_of->type = aml_def_ref_of;
	def_ref_of->flags = 0;
	def_ref_of->component.def_ref_of.ref_of_op = analyse_aml_ref_of_op(def_ref_of, aml);
	def_ref_of->string.length += def_ref_of->component.def_ref_of.ref_of_op->string.length;
	aml.initial += def_ref_of->component.def_ref_of.ref_of_op->string.length;
	aml.length -= def_ref_of->component.def_ref_of.ref_of_op->string.length;
	def_ref_of->component.def_ref_of.super_name = analyse_aml_super_name(def_ref_of, aml);
	def_ref_of->string.length += def_ref_of->component.def_ref_of.super_name->string.length;
	aml.initial += def_ref_of->component.def_ref_of.super_name->string.length;
	aml.length -= def_ref_of->component.def_ref_of.super_name->string.length;
	return def_ref_of;
}

// <def_release> := <release_op> <mutex_object>
AMLSymbol *analyse_aml_def_release(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_release aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_release = malloc(sizeof(*def_release));
	def_release->parent = parent;
	def_release->string.initial = aml.initial;
	def_release->string.length = 0;
	def_release->type = aml_def_release;
	def_release->flags = 0;
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
	printf_serial("def_return aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_return = malloc(sizeof(*def_return));
	def_return->parent = parent;
	def_return->string.initial = aml.initial;
	def_return->string.length = 0;
	def_return->type = aml_def_return;
	def_return->flags = 0;
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
	printf_serial("def_scope aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_scope = malloc(sizeof(*def_scope));
	def_scope->parent = parent;
	def_scope->string.initial = aml.initial;
	def_scope->string.length = 0;
	def_scope->type = aml_def_scope;
	def_scope->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_scope->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_scope;
}

// <def_shift_left> := <shift_left_op> <operand> <shift_count> <target>
AMLSymbol *analyse_aml_def_shift_left(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_shift_left aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_shift_left = malloc(sizeof(*def_shift_left));
	def_shift_left->parent = parent;
	def_shift_left->string.initial = aml.initial;
	def_shift_left->string.length = 0;
	def_shift_left->type = aml_def_shift_left;
	def_shift_left->flags = 0;
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
	printf_serial("def_shift_right aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_shift_right = malloc(sizeof(*def_shift_right));
	def_shift_right->parent = parent;
	def_shift_right->string.initial = aml.initial;
	def_shift_right->string.length = 0;
	def_shift_right->type = aml_def_shift_right;
	def_shift_right->flags = 0;
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
	printf_serial("def_size_of aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_size_of = malloc(sizeof(*def_size_of));
	def_size_of->parent = parent;
	def_size_of->string.initial = aml.initial;
	def_size_of->string.length = 0;
	def_size_of->type = aml_def_size_of;
	def_size_of->flags = 0;
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

// <def_sleep> := <sleep_op> <msec_time>
AMLSymbol *analyse_aml_def_sleep(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_sleep aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_sleep = malloc(sizeof(*def_sleep));
	def_sleep->parent = parent;
	def_sleep->string.initial = aml.initial;
	def_sleep->string.length = 0;
	def_sleep->type = aml_def_sleep;
	def_sleep->flags = 0;
	def_sleep->component.def_sleep.sleep_op = analyse_aml_sleep_op(def_sleep, aml);
	def_sleep->string.length += def_sleep->component.def_sleep.sleep_op->string.length;
	aml.initial += def_sleep->component.def_sleep.sleep_op->string.length;
	aml.length -= def_sleep->component.def_sleep.sleep_op->string.length;
	def_sleep->component.def_sleep.msec_time = analyse_aml_msec_time(def_sleep, aml);
	def_sleep->string.length += def_sleep->component.def_sleep.msec_time->string.length;
	aml.initial += def_sleep->component.def_sleep.msec_time->string.length;
	aml.length -= def_sleep->component.def_sleep.msec_time->string.length;
	return def_sleep;
}

// <def_stall> := <stall_op> <usec_time>
AMLSymbol *analyse_aml_def_stall(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_stall aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_stall = malloc(sizeof(*def_stall));
	def_stall->parent = parent;
	def_stall->string.initial = aml.initial;
	def_stall->string.length = 0;
	def_stall->type = aml_def_stall;
	def_stall->flags = 0;
	def_stall->component.def_stall.stall_op = analyse_aml_stall_op(def_stall, aml);
	def_stall->string.length += def_stall->component.def_stall.stall_op->string.length;
	aml.initial += def_stall->component.def_stall.stall_op->string.length;
	aml.length -= def_stall->component.def_stall.stall_op->string.length;
	def_stall->component.def_stall.usec_time = analyse_aml_usec_time(def_stall, aml);
	def_stall->string.length += def_stall->component.def_stall.usec_time->string.length;
	aml.initial += def_stall->component.def_stall.usec_time->string.length;
	aml.length -= def_stall->component.def_stall.usec_time->string.length;
	return def_stall;
}

// <def_store> := <store_op> <term_arg> <super_name>
AMLSymbol *analyse_aml_def_store(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_store aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_store = malloc(sizeof(*def_store));
	def_store->parent = parent;
	def_store->string.initial = aml.initial;
	def_store->string.length = 0;
	def_store->type = aml_def_store;
	def_store->flags = 0;
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
	printf_serial("def_subtract aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_subtract = malloc(sizeof(*def_subtract));
	def_subtract->parent = parent;
	def_subtract->string.initial = aml.initial;
	def_subtract->string.length = 0;
	def_subtract->type = aml_def_subtract;
	def_subtract->flags = 0;
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

// <def_thermal_zone> := <thermal_zone_op> <pkg_length> <name_string> <term_list>
AMLSymbol *analyse_aml_def_thermal_zone(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_thermal_zone aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_thermal_zone = malloc(sizeof(*def_thermal_zone));
	def_thermal_zone->parent = parent;
	def_thermal_zone->string.initial = aml.initial;
	def_thermal_zone->string.length = 0;
	def_thermal_zone->type = aml_def_thermal_zone;
	def_thermal_zone->flags = 0;
	def_thermal_zone->component.def_thermal_zone.thermal_zone_op = analyse_aml_thermal_zone_op(def_thermal_zone, aml);
	def_thermal_zone->string.length += def_thermal_zone->component.def_thermal_zone.thermal_zone_op->string.length;
	aml.initial += def_thermal_zone->component.def_thermal_zone.thermal_zone_op->string.length;
	aml.length -= def_thermal_zone->component.def_thermal_zone.thermal_zone_op->string.length;
	def_thermal_zone->component.def_thermal_zone.pkg_length = analyse_aml_pkg_length(def_thermal_zone, aml);
	def_thermal_zone->string.length += def_thermal_zone->component.def_thermal_zone.pkg_length->string.length;
	aml.initial += def_thermal_zone->component.def_thermal_zone.pkg_length->string.length;
	aml.length = def_thermal_zone->component.def_thermal_zone.pkg_length->component.pkg_length.length - def_thermal_zone->component.def_thermal_zone.pkg_length->string.length;
	def_thermal_zone->component.def_thermal_zone.name_string = analyse_aml_name_string(def_thermal_zone, aml);
	def_thermal_zone->string.length += def_thermal_zone->component.def_thermal_zone.name_string->string.length;
	aml.initial += def_thermal_zone->component.def_thermal_zone.name_string->string.length;
	aml.length -= def_thermal_zone->component.def_thermal_zone.name_string->string.length;
	def_thermal_zone->component.def_thermal_zone.term_list = analyse_aml_term_list(def_thermal_zone, aml);
	def_thermal_zone->string.length += def_thermal_zone->component.def_thermal_zone.term_list->string.length;
	aml.initial += def_thermal_zone->component.def_thermal_zone.term_list->string.length;
	aml.length -= def_thermal_zone->component.def_thermal_zone.term_list->string.length;
	return def_thermal_zone;
}

// <def_to_buffer> := <to_buffer_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_buffer(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_to_buffer aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_to_buffer = malloc(sizeof(*def_to_buffer));
	def_to_buffer->parent = parent;
	def_to_buffer->string.initial = aml.initial;
	def_to_buffer->string.length = 0;
	def_to_buffer->type = aml_def_to_buffer;
	def_to_buffer->flags = 0;
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
	printf_serial("def_to_hex_string aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_to_hex_string = malloc(sizeof(*def_to_hex_string));
	def_to_hex_string->parent = parent;
	def_to_hex_string->string.initial = aml.initial;
	def_to_hex_string->string.length = 0;
	def_to_hex_string->type = aml_def_to_hex_string;
	def_to_hex_string->flags = 0;
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

// <def_to_integer> := <to_integer_op> <operand> <target>
AMLSymbol *analyse_aml_def_to_integer(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_to_integer aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_to_integer = malloc(sizeof(*def_to_integer));
	def_to_integer->parent = parent;
	def_to_integer->string.initial = aml.initial;
	def_to_integer->string.length = 0;
	def_to_integer->type = aml_def_to_integer;
	def_to_integer->flags = 0;
	def_to_integer->component.def_to_integer.to_integer_op = analyse_aml_to_integer_op(def_to_integer, aml);
	def_to_integer->string.length += def_to_integer->component.def_to_integer.to_integer_op->string.length;
	aml.initial += def_to_integer->component.def_to_integer.to_integer_op->string.length;
	aml.length -= def_to_integer->component.def_to_integer.to_integer_op->string.length;
	def_to_integer->component.def_to_integer.operand = analyse_aml_operand(def_to_integer, aml);
	def_to_integer->string.length += def_to_integer->component.def_to_integer.operand->string.length;
	aml.initial += def_to_integer->component.def_to_integer.operand->string.length;
	aml.length -= def_to_integer->component.def_to_integer.operand->string.length;
	def_to_integer->component.def_to_integer.target = analyse_aml_target(def_to_integer, aml);
	def_to_integer->string.length += def_to_integer->component.def_to_integer.target->string.length;
	aml.initial += def_to_integer->component.def_to_integer.target->string.length;
	aml.length -= def_to_integer->component.def_to_integer.target->string.length;
	return def_to_integer;
}

// <def_while> := <while_op> <pkg_length> <predicate> <term_list>
AMLSymbol *analyse_aml_def_while(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_while aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_while = malloc(sizeof(*def_while));
	def_while->parent = parent;
	def_while->string.initial = aml.initial;
	def_while->string.length = 0;
	def_while->type = aml_def_while;
	def_while->flags = 0;
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
	if((int)aml.length < 0)
	{
		def_while->flags |= AML_SYMBOL_ERROR; // Length error
		ERROR();
	}
	return def_while;
}

// <def_xor> := <xor_op> <operxor> <operxor> <target>
AMLSymbol *analyse_aml_def_xor(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("def_xor aml.length = %#010.8x\n", aml.length);
	AMLSymbol *def_xor = malloc(sizeof(*def_xor));
	def_xor->parent = parent;
	def_xor->string.initial = aml.initial;
	def_xor->string.length = 0;
	def_xor->type = aml_def_xor;
	def_xor->flags = 0;
	def_xor->component.def_xor.xor_op = analyse_aml_xor_op(def_xor, aml);
	def_xor->string.length += def_xor->component.def_xor.xor_op->string.length;
	aml.initial += def_xor->component.def_xor.xor_op->string.length;
	aml.length -= def_xor->component.def_xor.xor_op->string.length;
	for(AMLSymbol **operand = def_xor->component.def_xor.operand; operand != def_xor->component.def_xor.operand + _countof(def_xor->component.def_xor.operand); operand++)
	{
		*operand = analyse_aml_operand(def_xor, aml);
		def_xor->string.length += (*operand)->string.length;
		aml.initial += (*operand)->string.length;
		aml.length -= (*operand)->string.length;
	}
	def_xor->component.def_xor.target = analyse_aml_target(def_xor, aml);
	def_xor->string.length += def_xor->component.def_xor.target->string.length;
	aml.initial += def_xor->component.def_xor.target->string.length;
	aml.length -= def_xor->component.def_xor.target->string.length;
	return def_xor;
}

// <deref_of_op> := AML_BYTE_DEREF_OF_OP
AMLSymbol *analyse_aml_deref_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("deref_of_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *deref_of_op = malloc(sizeof(*deref_of_op));
	deref_of_op->parent = parent;
	deref_of_op->string.initial = aml.initial;
	deref_of_op->string.length = 1;
	deref_of_op->type = aml_deref_of_op;
	deref_of_op->flags = 0;
	if(!aml.length)
	{
		deref_of_op->string.length = 0;
		deref_of_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*deref_of_op->string.initial != AML_BYTE_DEREF_OF_OP)
	{
		deref_of_op->flags |= AML_SYMBOL_ERROR; // Incorrect deref of op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return deref_of_op;
}

// <device_op> := <ext_op_prefix> <device_op_suffix>
AMLSymbol *analyse_aml_device_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("device_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *device_op = malloc(sizeof(*device_op));
	device_op->parent = parent;
	device_op->string.initial = aml.initial;
	device_op->string.length = 0;
	device_op->type = aml_device_op;
	device_op->flags = 0;
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
	printf_serial("device_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *device_op_suffix = malloc(sizeof(*device_op_suffix));
	device_op_suffix->parent = parent;
	device_op_suffix->string.initial = aml.initial;
	device_op_suffix->string.length = 1;
	device_op_suffix->type = aml_device_op_suffix;
	device_op_suffix->flags = 0;
	if(!aml.length)
	{
		device_op_suffix->string.length = 0;
		device_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_DEVICE_OP)
	{
		device_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect device op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return device_op_suffix;
}

// <digit_char> := '0' - '9'
AMLSymbol *analyse_aml_digit_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("digit_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *digit_char = malloc(sizeof(*digit_char));
	digit_char->parent = parent;
	digit_char->string.initial = aml.initial;
	digit_char->string.length = 1;
	digit_char->type = aml_digit_char;
	digit_char->flags = 0;
	if(!aml.length)
	{
		digit_char->string.length = 0;
		digit_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(!('0' <= *digit_char->string.initial && *digit_char->string.initial <= '9'))
	{
		digit_char->flags |= AML_SYMBOL_ERROR; // Incorrect digit char
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return digit_char;
}

// <divide_op> := AML_BYTE_DIVIDE_OP
AMLSymbol *analyse_aml_divide_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("divide_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *divide_op = malloc(sizeof(*divide_op));
	divide_op->parent = parent;
	divide_op->string.initial = aml.initial;
	divide_op->string.length = 1;
	divide_op->type = aml_divide_op;
	divide_op->flags = 0;
	if(!aml.length)
	{
		divide_op->string.length = 0;
		divide_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*divide_op->string.initial != AML_BYTE_DIVIDE_OP)
	{
		divide_op->flags |= AML_SYMBOL_ERROR; // Incorrect dual name prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return divide_op;
}

// <dividend> := <term_arg>
AMLSymbol *analyse_aml_dividend(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("dividend aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dividend = malloc(sizeof(*dividend));
	dividend->parent = parent;
	dividend->string.initial = aml.initial;
	dividend->string.length = 0;
	dividend->type = aml_dividend;
	dividend->flags = 0;
	dividend->component.dividend.term_arg = analyse_aml_term_arg(dividend, aml);
	dividend->string.length += dividend->component.dividend.term_arg->string.length;
	return dividend;
}

// <divisor> := <term_arg>
AMLSymbol *analyse_aml_divisor(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("divisor aml.length = %#010.8x\n", aml.length);
	AMLSymbol *divisor = malloc(sizeof(*divisor));
	divisor->parent = parent;
	divisor->string.initial = aml.initial;
	divisor->string.length = 0;
	divisor->type = aml_divisor;
	divisor->flags = 0;
	divisor->component.divisor.term_arg = analyse_aml_term_arg(divisor, aml);
	divisor->string.length += divisor->component.divisor.term_arg->string.length;
	return divisor;
}

// <dual_name_path> := <dual_name_prefix> <name_seg> <name_seg>
AMLSymbol *analyse_aml_dual_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("dual_name_path aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dual_name_path = malloc(sizeof(*dual_name_path));
	char *string_writer = dual_name_path->component.dual_name_path.string;
	dual_name_path->parent = parent;
	dual_name_path->string.initial = aml.initial;
	dual_name_path->string.length = 0;
	dual_name_path->type = aml_dual_name_path;
	dual_name_path->flags = 0;
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
	printf_serial("dual_name_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dual_name_prefix = malloc(sizeof(*dual_name_prefix));
	dual_name_prefix->parent = parent;
	dual_name_prefix->string.initial = aml.initial;
	dual_name_prefix->string.length = 1;
	dual_name_prefix->type = aml_dual_name_prefix;
	dual_name_prefix->flags = 0;
	if(!aml.length)
	{
		dual_name_prefix->string.length = 0;
		dual_name_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*dual_name_prefix->string.initial != AML_BYTE_DUAL_NAME_PREFIX)
	{
		dual_name_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect dual name prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return dual_name_prefix;
}

// <dword_const> := <dword_prefix> <dword_data>
AMLSymbol *analyse_aml_dword_const(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("dword_const aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dword_const = malloc(sizeof(*dword_const));
	dword_const->parent = parent;
	dword_const->string.initial = aml.initial;
	dword_const->string.length = 0;
	dword_const->type = aml_dword_const;
	dword_const->flags = 0;
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
	printf_serial("dword_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dword_data = malloc(sizeof(*dword_data));
	dword_data->parent = parent;
	dword_data->string.initial = aml.initial;
	dword_data->string.length = 0;
	dword_data->type = aml_dword_data;
	dword_data->flags = 0;
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
	printf_serial("dword_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *dword_prefix = malloc(sizeof(*dword_prefix));
	dword_prefix->parent = parent;
	dword_prefix->string.initial = aml.initial;
	dword_prefix->string.length = 1;
	dword_prefix->type = aml_dword_prefix;
	dword_prefix->flags = 0;
	if(!aml.length)
	{
		dword_prefix->string.length = 0;
		dword_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*dword_prefix->string.initial != AML_BYTE_DWORD_PREFIX)
	{
		dword_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect dword prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return dword_prefix;
}

// <else_op> := AML_BYTE_ELSE_OP
AMLSymbol *analyse_aml_else_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("else_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *else_op = malloc(sizeof(*else_op));
	else_op->parent = parent;
	else_op->string.initial = aml.initial;
	else_op->string.length = 1;
	else_op->type = aml_else_op;
	else_op->flags = 0;
	if(!aml.length)
	{
		else_op->string.length = 0;
		else_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*else_op->string.initial != AML_BYTE_ELSE_OP)
	{
		else_op->flags |= AML_SYMBOL_ERROR; // Incorrect else_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return else_op;
}

// <expression_opcode> := <def_acquire> | <def_add> | <def_and> | <def_buffer> | <def_concat> | <def_concat_res> | <def_cond_ref_of> | <def_copy_object> | <def_decrement> | <def_deref_of> | <def_divide> | <def_find_set_left_bit> | <def_find_set_right_bit> | <def_from_bcd> | <def_increment> | <def_index> | <def_l_and> | <def_l_equal> | <def_l_greater> | <def_l_greater_equal> | <def_l_less> | <def_l_less_equal> | <def_mid> | <def_l_not> | <def_l_not_equal> | <def_load_table> | <def_l_or> | <def_match> | <def_mod> | <def_multiply> | <def_nand> | <def_nor> | <def_not> | <def_object_type> | <def_or> | <def_package> | <def_var_package> | <def_ref_of> | <def_shift_left> | <def_shift_right> | <def_size_of> | <def_store> | <def_subtract> | <def_timer> | <def_to_bcd> | <def_to_buffer> | <def_to_decimal_string> | <def_to_hex_string> | <def_to_integer> | <def_to_string> | <def_wait> | <def_xor> | <method_invocation>
AMLSymbol *analyse_aml_expression_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("expression_opcode aml.length = %#010.8x\n", aml.length);
	AMLSymbol *expression_opcode = malloc(sizeof(*expression_opcode));
	expression_opcode->parent = parent;
	expression_opcode->string.initial = aml.initial;
	expression_opcode->string.length = 0;
	expression_opcode->type = aml_expression_opcode;
	expression_opcode->flags = 0;
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
	case AML_BYTE_ADD_OP:
		expression_opcode->component.expression_opcode.def_add = analyse_aml_def_add(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_add->string.length;
		break;
	case AML_BYTE_AND_OP:
		expression_opcode->component.expression_opcode.def_and = analyse_aml_def_and(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_and->string.length;
		break;
	case AML_BYTE_CONCAT_OP:
		expression_opcode->component.expression_opcode.def_concat = analyse_aml_def_concat(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_concat->string.length;
		break;
	case AML_BYTE_CONCAT_RES_OP:
		expression_opcode->component.expression_opcode.def_concat_res = analyse_aml_def_concat_res(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_concat_res->string.length;
		break;
	case AML_BYTE_DECREMENT_OP:
		expression_opcode->component.expression_opcode.def_decrement = analyse_aml_def_decrement(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_decrement->string.length;
		break;
	case AML_BYTE_DEREF_OF_OP:
		expression_opcode->component.expression_opcode.def_deref_of = analyse_aml_def_deref_of(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_deref_of->string.length;
		break;
	case AML_BYTE_DIVIDE_OP:
		expression_opcode->component.expression_opcode.def_divide = analyse_aml_def_divide(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_divide->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_ACQUIRE_OP:
			expression_opcode->component.expression_opcode.def_acquire = analyse_aml_def_acquire(expression_opcode, aml);
			expression_opcode->string.length += expression_opcode->component.expression_opcode.def_acquire->string.length;
			break;
		case AML_BYTE_COND_REF_OF_OP:
			expression_opcode->component.expression_opcode.def_cond_ref_of = analyse_aml_def_cond_ref_of(expression_opcode, aml);
			expression_opcode->string.length += expression_opcode->component.expression_opcode.def_cond_ref_of->string.length;
			break;
		default:
			ERROR(); // Syntax error
			printf_serial("aml.initial[0] = %#04.2x\n", aml.initial[0]);
			printf_serial("aml.initial[1] = %#04.2x\n", aml.initial[1]);
			break;
		}
		break;
	case AML_BYTE_FIND_SET_LEFT_BIT_OP:
		expression_opcode->component.expression_opcode.def_find_set_left_bit = analyse_aml_def_find_set_left_bit(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_find_set_left_bit->string.length;
		break;
	case AML_BYTE_FIND_SET_RIGHT_BIT_OP:
		expression_opcode->component.expression_opcode.def_find_set_right_bit = analyse_aml_def_find_set_right_bit(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_find_set_right_bit->string.length;
		break;
	case AML_BYTE_INCREMENT_OP:
		expression_opcode->component.expression_opcode.def_increment = analyse_aml_def_increment(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_increment->string.length;
		break;
	case AML_BYTE_INDEX_OP:
		expression_opcode->component.expression_opcode.def_index = analyse_aml_def_index(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_index->string.length;
		break;
	case AML_BYTE_L_AND_OP:
		expression_opcode->component.expression_opcode.def_l_and = analyse_aml_def_l_and(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_and->string.length;
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
	case AML_BYTE_L_NOT_OP:
		expression_opcode->component.expression_opcode.def_l_not = analyse_aml_def_l_not(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_not->string.length;
		break;
	case AML_BYTE_L_OR_OP:
		expression_opcode->component.expression_opcode.def_l_or = analyse_aml_def_l_or(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_l_or->string.length;
		break;
	case AML_BYTE_MATCH_OP:
		expression_opcode->component.expression_opcode.def_match = analyse_aml_def_match(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_match->string.length;
		break;
	case AML_BYTE_MID_OP:
		expression_opcode->component.expression_opcode.def_mid = analyse_aml_def_mid(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_mid->string.length;
		break;
	case AML_BYTE_MULTIPLY_OP:
		expression_opcode->component.expression_opcode.def_multiply = analyse_aml_def_multiply(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_multiply->string.length;
		break;
	case AML_BYTE_NOT_OP:
		expression_opcode->component.expression_opcode.def_not = analyse_aml_def_not(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_not->string.length;
		break;
	case AML_BYTE_OBJECT_TYPE_OP:
		expression_opcode->component.expression_opcode.def_object_type = analyse_aml_def_object_type(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_object_type->string.length;
		break;
	case AML_BYTE_OR_OP:
		expression_opcode->component.expression_opcode.def_or = analyse_aml_def_or(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_or->string.length;
		break;
	case AML_BYTE_PACKAGE_OP:
		expression_opcode->component.expression_opcode.def_package = analyse_aml_def_package(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_package->string.length;
		break;
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
		expression_opcode->component.expression_opcode.method_invocation = analyse_aml_method_invocation(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.method_invocation->string.length;
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
	case AML_BYTE_TO_INTEGER_OP:
		expression_opcode->component.expression_opcode.def_to_integer = analyse_aml_def_to_integer(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_to_integer->string.length;
		break;
	case AML_BYTE_XOR_OP:
		expression_opcode->component.expression_opcode.def_xor = analyse_aml_def_xor(expression_opcode, aml);
		expression_opcode->string.length += expression_opcode->component.expression_opcode.def_xor->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			expression_opcode->component.expression_opcode.method_invocation = analyse_aml_method_invocation(expression_opcode, aml);
			expression_opcode->string.length += expression_opcode->component.expression_opcode.method_invocation->string.length;
		}
		else
		{
			expression_opcode->flags |= AML_SYMBOL_ERROR; // Syntax error
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return expression_opcode;
}

// <ext_op_prefix> := AML_BYTE_EXT_OP_PREFIX
AMLSymbol *analyse_aml_ext_op_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("ext_op_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *ext_op_prefix = malloc(sizeof(*ext_op_prefix));
	ext_op_prefix->parent = parent;
	ext_op_prefix->string.initial = aml.initial;
	ext_op_prefix->string.length = 1;
	ext_op_prefix->type = aml_ext_op_prefix;
	ext_op_prefix->flags = 0;
	if(!aml.length)
	{
		ext_op_prefix->string.length = 0;
		ext_op_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*ext_op_prefix->string.initial != AML_BYTE_EXT_OP_PREFIX)
	{
		ext_op_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect ext op prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return ext_op_prefix;
}

// <field_element> := <named_field> | <reserved_field> | <access_field> | <extended_access_field> | <connect_field>
AMLSymbol *analyse_aml_field_element(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("field_element aml.length = %#010.8x\n", aml.length);
	AMLSymbol *field_element = malloc(sizeof(*field_element));
	field_element->parent = parent;
	field_element->string.initial = aml.initial;
	field_element->string.length = 0;
	field_element->type = aml_field_element;
	field_element->flags = 0;
	field_element->component.field_element.named_field = NULL;
	field_element->component.field_element.reserved_field = NULL;
	field_element->component.field_element.access_field = NULL;
	field_element->component.field_element.extended_access_field = NULL;
	field_element->component.field_element.connect_field = NULL;
	switch(*aml.initial)
	{
	case AML_BYTE_ACCESS_FIELD_OP:
		field_element->component.field_element.access_field = analyse_aml_access_field(field_element, aml);
		field_element->string.length += field_element->component.field_element.access_field->string.length;
		aml.initial += field_element->component.field_element.access_field->string.length;
		aml.length -= field_element->component.field_element.access_field->string.length;
		break;
	case AML_BYTE_RESERVED_FIELD_OP:
		field_element->component.field_element.reserved_field = analyse_aml_reserved_field(field_element, aml);
		field_element->string.length += field_element->component.field_element.reserved_field->string.length;
		aml.initial += field_element->component.field_element.reserved_field->string.length;
		aml.length -= field_element->component.field_element.reserved_field->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			field_element->component.field_element.named_field = analyse_aml_named_field(field_element, aml);
			field_element->string.length += field_element->component.field_element.named_field->string.length;
			aml.initial += field_element->component.field_element.named_field->string.length;
			aml.length -= field_element->component.field_element.named_field->string.length;
		}
		else
		{
			field_element->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return field_element;
}

// <field_flags>
AMLSymbol *analyse_aml_field_flags(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("field_flags aml.length = %#010.8x\n", aml.length);
	AMLSymbol *field_flags = malloc(sizeof(*field_flags));
	field_flags->parent = parent;
	field_flags->string.initial = aml.initial;
	field_flags->string.length = 1;
	field_flags->type = aml_field_flags;
	field_flags->flags = 0;
	if(!aml.length)
	{
		field_flags->string.length = 0;
		field_flags->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(0x80 <= *field_flags->string.initial)
	{
		field_flags->flags |= AML_SYMBOL_ERROR; // Incorrect field flags
		ERROR();
	}
	return field_flags;
}

// <field_list> := Nothing | <field_element> <field_list>
AMLSymbol *analyse_aml_field_list(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("field_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *field_list = malloc(sizeof(*field_list));
	field_list->parent = parent;
	field_list->string.initial = aml.initial;
	field_list->string.length = 0;
	field_list->type = aml_field_list;
	field_list->flags = 0;
	field_list->component.field_list.field_element = NULL;
	field_list->component.field_list.field_list = NULL;
	if(aml.length)switch(*aml.initial)
	{
	case AML_BYTE_ACCESS_FIELD_OP:
	case AML_BYTE_RESERVED_FIELD_OP:
		field_list->component.field_list.field_element = analyse_aml_field_element(field_list, aml);
		field_list->string.length += field_list->component.field_list.field_element->string.length;
		aml.initial += field_list->component.field_list.field_element->string.length;
		aml.length -= field_list->component.field_list.field_element->string.length;
		field_list->component.field_list.field_list = analyse_aml_field_list(field_list, aml);
		field_list->string.length += field_list->component.field_list.field_list->string.length;
		aml.initial += field_list->component.field_list.field_list->string.length;
		aml.length -= field_list->component.field_list.field_list->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
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
		break;
	}
	return field_list;
}

// <field_op> := <ext_op_prefix> <field_op_suffix>
AMLSymbol *analyse_aml_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *field_op = malloc(sizeof(*field_op));
	field_op->parent = parent;
	field_op->string.initial = aml.initial;
	field_op->string.length = 0;
	field_op->type = aml_field_op;
	field_op->flags = 0;
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
	printf_serial("field_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *field_op_suffix = malloc(sizeof(*field_op_suffix));
	field_op_suffix->parent = parent;
	field_op_suffix->string.initial = aml.initial;
	field_op_suffix->string.length = 1;
	field_op_suffix->type = aml_field_op_suffix;
	field_op_suffix->flags = 0;
	if(!aml.length)
	{
		field_op_suffix->string.length = 0;
		field_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_FIELD_OP)
	{
		field_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect field op prefix
		ERROR();
	}
	return field_op_suffix;
}

// <find_set_left_bit_op> := AML_BYTE_FIND_SET_LEFT_BIT
AMLSymbol *analyse_aml_find_set_left_bit_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("find_set_left_bit_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *find_set_left_bit_op = malloc(sizeof(*find_set_left_bit_op));
	find_set_left_bit_op->parent = parent;
	find_set_left_bit_op->string.initial = aml.initial;
	find_set_left_bit_op->string.length = 1;
	find_set_left_bit_op->type = aml_find_set_left_bit_op;
	find_set_left_bit_op->flags = 0;
	if(!aml.length)
	{
		find_set_left_bit_op->string.length = 0;
		find_set_left_bit_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_FIND_SET_LEFT_BIT_OP)
	{
		find_set_left_bit_op->flags |= AML_SYMBOL_ERROR; // Incorrect find_set_left_bit_op
		ERROR();
	}
	return find_set_left_bit_op;
}

// <find_set_right_bit_op> := AML_BYTE_FIND_SET_RIGHT_BIT
AMLSymbol *analyse_aml_find_set_right_bit_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("find_set_right_bit_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *find_set_right_bit_op = malloc(sizeof(*find_set_right_bit_op));
	find_set_right_bit_op->parent = parent;
	find_set_right_bit_op->string.initial = aml.initial;
	find_set_right_bit_op->string.length = 1;
	find_set_right_bit_op->type = aml_find_set_right_bit_op;
	find_set_right_bit_op->flags = 0;
	if(!aml.length)
	{
		find_set_right_bit_op->string.length = 0;
		find_set_right_bit_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_FIND_SET_RIGHT_BIT_OP)
	{
		find_set_right_bit_op->flags |= AML_SYMBOL_ERROR; // Incorrect find_set_right_bit_op
		ERROR();
	}
	return find_set_right_bit_op;
}

// <if_op> := AML_BYTE_IF_OP
AMLSymbol *analyse_aml_if_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("if_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *if_op = malloc(sizeof(*if_op));
	if_op->parent = parent;
	if_op->string.initial = aml.initial;
	if_op->string.length = 1;
	if_op->type = aml_if_op;
	if_op->flags = 0;
	if(!aml.length)
	{
		if_op->string.length = 0;
		if_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_IF_OP)
	{
		if_op->flags |= AML_SYMBOL_ERROR; // Incorrect if_op
		ERROR();
	}
	return if_op;
}

// <increment_op> := AML_BYTE_INCREMENT_OP
AMLSymbol *analyse_aml_increment_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("increment_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *increment_op = malloc(sizeof(*increment_op));
	increment_op->parent = parent;
	increment_op->string.initial = aml.initial;
	increment_op->string.length = 1;
	increment_op->type = aml_increment_op;
	increment_op->flags = 0;
	if(!aml.length)
	{
		increment_op->string.length = 0;
		increment_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*increment_op->string.initial != AML_BYTE_INCREMENT_OP)
	{
		increment_op->flags |= AML_SYMBOL_ERROR; // Incorrect increment op
		ERROR();
	}
	return increment_op;
}

// <index_field_op> := <ext_op_prefix> <index_field_op_suffix>
AMLSymbol *analyse_aml_index_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("index_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *index_field_op = malloc(sizeof(*index_field_op));
	index_field_op->parent = parent;
	index_field_op->string.initial = aml.initial;
	index_field_op->string.length = 0;
	index_field_op->type = aml_index_field_op;
	index_field_op->flags = 0;
	index_field_op->component.index_field_op.ext_op_prefix = analyse_aml_ext_op_prefix(index_field_op, aml);
	index_field_op->string.length += index_field_op->component.index_field_op.ext_op_prefix->string.length;
	aml.initial += index_field_op->component.index_field_op.ext_op_prefix->string.length;
	aml.length -= index_field_op->component.index_field_op.ext_op_prefix->string.length;
	index_field_op->component.index_field_op.index_field_op_suffix = analyse_aml_index_field_op_suffix(index_field_op, aml);
	index_field_op->string.length += index_field_op->component.index_field_op.index_field_op_suffix->string.length;
	aml.initial += index_field_op->component.index_field_op.index_field_op_suffix->string.length;
	aml.length -= index_field_op->component.index_field_op.index_field_op_suffix->string.length;
	return index_field_op;
}

// <index_field_op_suffix> := AML_BYTE_INDEX_FIELD_OP
AMLSymbol *analyse_aml_index_field_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("index_field_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *index_field_op_suffix = malloc(sizeof(*index_field_op_suffix));
	index_field_op_suffix->parent = parent;
	index_field_op_suffix->string.initial = aml.initial;
	index_field_op_suffix->string.length = 1;
	index_field_op_suffix->type = aml_index_field_op_suffix;
	index_field_op_suffix->flags = 0;
	if(!aml.length)
	{
		index_field_op_suffix->string.length = 0;
		index_field_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*index_field_op_suffix->string.initial != AML_BYTE_INDEX_FIELD_OP)
	{
		index_field_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	return index_field_op_suffix;
}

// <index_op> := AML_BYTE_INDEX_OP
AMLSymbol *analyse_aml_index_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("index_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *index_op = malloc(sizeof(*index_op));
	index_op->parent = parent;
	index_op->string.initial = aml.initial;
	index_op->string.length = 1;
	index_op->type = aml_index_op;
	index_op->flags = 0;
	if(!aml.length)
	{
		index_op->string.length = 0;
		index_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*index_op->string.initial != AML_BYTE_INDEX_OP)
	{
		index_op->flags |= AML_SYMBOL_ERROR; // Incorrect index op
		ERROR();
	}
	return index_op;
}

// <index_value> := <term_arg>
AMLSymbol *analyse_aml_index_value(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("index_value aml.length = %#010.8x\n", aml.length);
	AMLSymbol *index_value = malloc(sizeof(*index_value));
	index_value->parent = parent;
	index_value->string.initial = aml.initial;
	index_value->string.length = 0;
	index_value->type = aml_index_value;
	index_value->flags = 0;
	index_value->component.index_value.term_arg = analyse_aml_term_arg(index_value, aml);
	index_value->string.length += index_value->component.index_value.term_arg->string.length;
	return index_value;
}

// <lead_name_char> := 'A' - 'Z' | '_'
AMLSymbol *analyse_aml_lead_name_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("lead_name_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *lead_name_char = malloc(sizeof(*lead_name_char));
	lead_name_char->parent = parent;
	lead_name_char->string.initial = aml.initial;
	lead_name_char->string.length = 1;
	lead_name_char->type = aml_lead_name_char;
	lead_name_char->flags = 0;
	if(!aml.length)
	{
		lead_name_char->string.length = 0;
		lead_name_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(!(('A' <= *lead_name_char->string.initial && *lead_name_char->string.initial <= 'Z') || *lead_name_char->string.initial == '_'))
	{
		lead_name_char->flags |= AML_SYMBOL_ERROR; // Incorrect lead name char
		ERROR();
	}
	return lead_name_char;
}

// <l_and_op> := AML_BYTE_L_AND_OP
AMLSymbol *analyse_aml_l_and_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_and_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_and_op = malloc(sizeof(*l_and_op));
	l_and_op->parent = parent;
	l_and_op->string.initial = aml.initial;
	l_and_op->string.length = 1;
	l_and_op->type = aml_l_and_op;
	l_and_op->flags = 0;
	if(!aml.length)
	{
		l_and_op->string.length = 0;
		l_and_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_and_op->string.initial != AML_BYTE_L_AND_OP)
	{
		l_and_op->flags |= AML_SYMBOL_ERROR; // Incandrect l_and_op
		ERROR();
	}
	return l_and_op;
}

// <l_equal_op> := AML_BYTE_L_EQUAL_OP
AMLSymbol *analyse_aml_l_equal_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_equal_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_equal_op = malloc(sizeof(*l_equal_op));
	l_equal_op->parent = parent;
	l_equal_op->string.initial = aml.initial;
	l_equal_op->string.length = 1;
	l_equal_op->type = aml_l_equal_op;
	l_equal_op->flags = 0;
	if(!aml.length)
	{
		l_equal_op->string.length = 0;
		l_equal_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_equal_op->string.initial != AML_BYTE_L_EQUAL_OP)
	{
		l_equal_op->flags |= AML_SYMBOL_ERROR; // Incorrect l_equal_op
		ERROR();
	}
	return l_equal_op;
}

// <l_greater_op> := AML_BYTE_L_EQUAL_OP
AMLSymbol *analyse_aml_l_greater_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_greater_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_greater_op = malloc(sizeof(*l_greater_op));
	l_greater_op->parent = parent;
	l_greater_op->string.initial = aml.initial;
	l_greater_op->string.length = 1;
	l_greater_op->type = aml_l_greater_op;
	l_greater_op->flags = 0;
	if(!aml.length)
	{
		l_greater_op->string.length = 0;
		l_greater_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_greater_op->string.initial != AML_BYTE_L_GREATER_OP)
	{
		l_greater_op->flags |= AML_SYMBOL_ERROR; // Incorrect l_greater_op
		ERROR();
	}
	return l_greater_op;
}

// <l_less_op> := AML_BYTE_L_LESS_OP
AMLSymbol *analyse_aml_l_less_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_less_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_less_op = malloc(sizeof(*l_less_op));
	l_less_op->parent = parent;
	l_less_op->string.initial = aml.initial;
	l_less_op->string.length = 1;
	l_less_op->type = aml_l_less_op;
	l_less_op->flags = 0;
	if(!aml.length)
	{
		l_less_op->string.length = 0;
		l_less_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_less_op->string.initial != AML_BYTE_L_LESS_OP)
	{
		l_less_op->flags |= AML_SYMBOL_ERROR; // Incorrect l_less_op
		ERROR();
	}
	return l_less_op;
}

// <l_not_op> := AML_BYTE_L_NOT_OP
AMLSymbol *analyse_aml_l_not_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_not_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_not_op = malloc(sizeof(*l_not_op));
	l_not_op->parent = parent;
	l_not_op->string.initial = aml.initial;
	l_not_op->string.length = 1;
	l_not_op->type = aml_l_not_op;
	l_not_op->flags = 0;
	if(!aml.length)
	{
		l_not_op->string.length = 0;
		l_not_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_not_op->string.initial != AML_BYTE_L_NOT_OP)
	{
		l_not_op->flags |= AML_SYMBOL_ERROR; // Incorrect l_not_op
		ERROR();
	}
	return l_not_op;
}

// <l_or_op> := AML_BYTE_L_OR_OP
AMLSymbol *analyse_aml_l_or_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("l_or_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *l_or_op = malloc(sizeof(*l_or_op));
	l_or_op->parent = parent;
	l_or_op->string.initial = aml.initial;
	l_or_op->string.length = 1;
	l_or_op->type = aml_l_or_op;
	l_or_op->flags = 0;
	if(!aml.length)
	{
		l_or_op->string.length = 0;
		l_or_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*l_or_op->string.initial != AML_BYTE_L_OR_OP)
	{
		l_or_op->flags |= AML_SYMBOL_ERROR; // Incorrect l_or_op
		ERROR();
	}
	return l_or_op;
}

// <local_obj> := <local_op>
AMLSymbol *analyse_aml_local_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("local_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *local_obj = malloc(sizeof(*local_obj));
	local_obj->parent = parent;
	local_obj->string.initial = aml.initial;
	local_obj->string.length = 0;
	local_obj->type = aml_local_obj;
	local_obj->flags = 0;
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
	printf_serial("local_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *local_op = malloc(sizeof(*local_op));
	local_op->parent = parent;
	local_op->string.initial = aml.initial;
	local_op->string.length = 1;
	local_op->type = aml_local_op;
	local_op->flags = 0;
	if(!aml.length)
	{
		local_op->string.length = 0;
		local_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else switch(*local_op->string.initial)
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
		local_op->flags |= AML_SYMBOL_ERROR; // Incorrect local op
		ERROR();
		break;
	}
	return local_op;
}

// <match_op> := AML_BYTE_MATCH_OP
AMLSymbol *analyse_aml_match_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("match_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *match_op = malloc(sizeof(*match_op));
	match_op->parent = parent;
	match_op->string.initial = aml.initial;
	match_op->string.length = 1;
	match_op->type = aml_match_op;
	match_op->flags = 0;
	if(!aml.length)
	{
		match_op->string.length = 0;
		match_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*match_op->string.initial != AML_BYTE_MATCH_OP)
	{
		match_op->flags |= AML_SYMBOL_ERROR; // Incorrect match op
		ERROR();
	}
	return match_op;
}

// <match_opcode> := <byte_data>
AMLSymbol *analyse_aml_match_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("match_opcode aml.length = %#010.8x\n", aml.length);
	AMLSymbol *match_opcode = malloc(sizeof(*match_opcode));
	match_opcode->parent = parent;
	match_opcode->string.initial = aml.initial;
	match_opcode->string.length = 1;
	match_opcode->type = aml_match_opcode;
	match_opcode->flags = 0;
	return match_opcode;
}

// <method_flags>
AMLSymbol *analyse_aml_method_flags(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("method_flags aml.length = %#010.8x\n", aml.length);
	AMLSymbol *method_flags = malloc(sizeof(*method_flags));
	method_flags->parent = parent;
	method_flags->string.initial = aml.initial;
	method_flags->string.length = 1;
	method_flags->type = aml_method_flags;
	method_flags->flags = 0;
	if(!aml.length)
	{
		method_flags->string.length = 0;
		method_flags->flags |= AML_SYMBOL_ERROR;
		ERROR();
		method_flags->component.method_flags.arg_count = 0;
		method_flags->component.method_flags.serialize_flag = 0;
		method_flags->component.method_flags.sync_level = 0;
	}
	else
	{
		method_flags->component.method_flags.arg_count = *aml.initial & 0x07;
		method_flags->component.method_flags.serialize_flag = *aml.initial & 0x08 ? true : false;
		method_flags->component.method_flags.sync_level = *aml.initial >> 4;
	}
	return method_flags;
}

// <method_invocation> := <name_string> <term_arg_list>
AMLSymbol *analyse_aml_method_invocation(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("method_invocation aml.length = %#010.8x\n", aml.length);
	AMLSymbol *method_invocation = malloc(sizeof(*method_invocation));
	AMLSymbol const *def_method;
	int num_of_args;
	method_invocation->parent = parent;
	method_invocation->string.initial = aml.initial;
	method_invocation->string.length = 0;
	method_invocation->type = aml_method_invocation;
	method_invocation->flags = 0;
	method_invocation->component.method_invocation.name_string = analyse_aml_name_string(method_invocation, aml);
	method_invocation->string.length += method_invocation->component.method_invocation.name_string->string.length;
	aml.initial += method_invocation->component.method_invocation.name_string->string.length;
	aml.length -= method_invocation->component.method_invocation.name_string->string.length;
	def_method = get_aml_method(method_invocation->component.method_invocation.name_string->component.name_string.string, method_invocation, NULL);
	if(def_method)num_of_args = def_method->component.def_method.method_flags->component.method_flags.arg_count;
	else
	{
		WARNING();
		printf_serial("Undefined method \"%s\" is called.\n", method_invocation->component.method_invocation.name_string->component.name_string.string);
		num_of_args = 0;
	}
	method_invocation->component.method_invocation.term_arg_list = analyse_aml_term_arg_list(method_invocation, aml, num_of_args);
	method_invocation->string.length += method_invocation->component.method_invocation.term_arg_list->string.length;
	aml.initial += method_invocation->component.method_invocation.term_arg_list->string.length;
	aml.length -= method_invocation->component.method_invocation.term_arg_list->string.length;
	return method_invocation;
}

// <method_op> := AML_BYTE_METHOD_OP
AMLSymbol *analyse_aml_method_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("method_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *method_op = malloc(sizeof(*method_op));
	method_op->parent = parent;
	method_op->string.initial = aml.initial;
	method_op->string.length = 1;
	method_op->type = aml_method_op;
	method_op->flags = 0;
	if(!aml.length)
	{
		method_op->string.length = 0;
		method_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*method_op->string.initial != AML_BYTE_METHOD_OP)
	{
		method_op->flags |= AML_SYMBOL_ERROR; // Incorrect method op
		ERROR();
	}
	return method_op;
}

// <mid_obj> := <term_arg>
AMLSymbol *analyse_aml_mid_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("mid_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *mid_obj = malloc(sizeof(*mid_obj));
	mid_obj->parent = parent;
	mid_obj->string.initial = aml.initial;
	mid_obj->string.length = 0;
	mid_obj->type = aml_mid_obj;
	mid_obj->flags = 0;
	mid_obj->component.mid_obj.term_arg = analyse_aml_term_arg(mid_obj, aml);
	mid_obj->string.length += mid_obj->component.mid_obj.term_arg->string.length;
	return mid_obj;
}

// <mid_op> := AML_BYTE_MID_OP
AMLSymbol *analyse_aml_mid_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("mid_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *mid_op = malloc(sizeof(*mid_op));
	mid_op->parent = parent;
	mid_op->string.initial = aml.initial;
	mid_op->string.length = 1;
	mid_op->type = aml_mid_op;
	mid_op->flags = 0;
	if(!aml.length)
	{
		mid_op->string.length = 0;
		mid_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*mid_op->string.initial != AML_BYTE_MID_OP)
	{
		mid_op->flags |= AML_SYMBOL_ERROR; // Incorrect munti name prefix
		ERROR();
	}
	return mid_op;
}

// <msec_time> := <term_arg>
AMLSymbol *analyse_aml_msec_time(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("msec_time aml.length = %#010.8x\n", aml.length);
	AMLSymbol *msec_time = malloc(sizeof(*msec_time));
	msec_time->parent = parent;
	msec_time->string.initial = aml.initial;
	msec_time->string.length = 0;
	msec_time->type = aml_msec_time;
	msec_time->flags = 0;
	msec_time->component.msec_time.term_arg = analyse_aml_term_arg(msec_time, aml);
	msec_time->string.length += msec_time->component.msec_time.term_arg->string.length;
	return msec_time;
}

// <multi_name_path> := <multi_name_prefix> <seg_count> <name_seg>*
AMLSymbol *analyse_aml_multi_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("multi_name_path aml.length = %#010.8x\n", aml.length);
	AMLSymbol *multi_name_path = malloc(sizeof(*multi_name_path));
	char *string_writer;
	multi_name_path->parent = parent;
	multi_name_path->string.initial = aml.initial;
	multi_name_path->string.length = 0;
	multi_name_path->type = aml_multi_name_path;
	multi_name_path->flags = 0;
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
	printf_serial("multi_name_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *multi_name_prefix = malloc(sizeof(*multi_name_prefix));
	multi_name_prefix->parent = parent;
	multi_name_prefix->string.initial = aml.initial;
	multi_name_prefix->string.length = 1;
	multi_name_prefix->type = aml_multi_name_prefix;
	multi_name_prefix->flags = 0;
	if(!aml.length)
	{
		multi_name_prefix->string.length = 0;
		multi_name_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*multi_name_prefix->string.initial != AML_BYTE_MULTI_NAME_PREFIX)
	{
		multi_name_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect munti name prefix
		ERROR();
	}
	return multi_name_prefix;
}

// <multiply_op> := AML_BYTE_MULTIPLY_OP
AMLSymbol *analyse_aml_multiply_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("multiply_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *multiply_op = malloc(sizeof(*multiply_op));
	multiply_op->parent = parent;
	multiply_op->string.initial = aml.initial;
	multiply_op->string.length = 1;
	multiply_op->type = aml_multiply_op;
	multiply_op->flags = 0;
	if(!aml.length)
	{
		multiply_op->string.length = 0;
		multiply_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*multiply_op->string.initial != AML_BYTE_MULTIPLY_OP)
	{
		multiply_op->flags |= AML_SYMBOL_ERROR; // Incorrect multiply_op
		ERROR();
	}
	return multiply_op;
}

// <mutex_object> := <super_name>
AMLSymbol *analyse_aml_mutex_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("mutex_object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *mutex_object = malloc(sizeof(*mutex_object));
	mutex_object->parent = parent;
	mutex_object->string.initial = aml.initial;
	mutex_object->string.length = 0;
	mutex_object->type = aml_mutex_object;
	mutex_object->flags = 0;
	mutex_object->component.mutex_object.super_name = analyse_aml_super_name(mutex_object, aml);
	mutex_object->string.length += mutex_object->component.mutex_object.super_name->string.length;
	return mutex_object;
}

// <mutex_op> := <ext_op_prefix> <mutex_op_suffix>
AMLSymbol *analyse_aml_mutex_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("mutex_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *mutex_op = malloc(sizeof(*mutex_op));
	mutex_op->parent = parent;
	mutex_op->string.initial = aml.initial;
	mutex_op->string.length = 0;
	mutex_op->type = aml_mutex_op;
	mutex_op->flags = 0;
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
	printf_serial("mutex_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *mutex_op_suffix = malloc(sizeof(*mutex_op_suffix));
	mutex_op_suffix->parent = parent;
	mutex_op_suffix->string.initial = aml.initial;
	mutex_op_suffix->string.length = 1;
	mutex_op_suffix->type = aml_mutex_op_suffix;
	mutex_op_suffix->flags = 0;
	if(!aml.length)
	{
		mutex_op_suffix->string.length = 0;
		mutex_op_suffix->flags = 0;
	}
	else if(*aml.initial != AML_BYTE_MUTEX_OP)
	{
		mutex_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect mutex_op_suffix
		ERROR();
	}
	return mutex_op_suffix;
}

// <name_char> := <digit_char> | <lead_name_char>
AMLSymbol *analyse_aml_name_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("name_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_char = malloc(sizeof(*name_char));
	name_char->parent = parent;
	name_char->string.initial = aml.initial;
	name_char->string.length = 0;
	name_char->type = aml_name_char;
	name_char->flags = 0;
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
	else
	{
		name_char->flags |= AML_SYMBOL_ERROR; // Incorrect name char
		ERROR();
	}
	name_char->component.name_char.character = *aml.initial;
	return name_char;
}

// <name_op> := AML_BYTE_NAME_OP
AMLSymbol *analyse_aml_name_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("name_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_op = malloc(sizeof(*name_op));
	name_op->parent = parent;
	name_op->string.initial = aml.initial;
	name_op->string.length = 1;
	name_op->type = aml_name_op;
	name_op->flags = 0;
	if(!aml.length)
	{
		name_op->string.length = 0;
		name_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*name_op->string.initial != AML_BYTE_NAME_OP)
	{
		name_op->flags |= AML_SYMBOL_ERROR; // Incorrect name op
		ERROR();
	}
	return name_op;
}

// <name_path> := <name_seg> | <dual_name_path> | <multi_name_path> | <null_name>
AMLSymbol *analyse_aml_name_path(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("name_path aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_path = malloc(sizeof(*name_path));
	name_path->parent = parent;
	name_path->string.initial = aml.initial;
	name_path->string.length = 0;
	name_path->type = aml_name_path;
	name_path->flags = 0;
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
			name_path->flags |= AML_SYMBOL_ERROR; // Syntax error
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return name_path;
}

// <name_seg> := <lead_name_char> <name_char> <name_char> <name_char>
AMLSymbol *analyse_aml_name_seg(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("name_seg aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_seg = malloc(sizeof(*name_seg));
	char *string_writer = name_seg->component.name_seg.string;
	name_seg->parent = parent;
	name_seg->string.initial = aml.initial;
	name_seg->string.length = 0;
	name_seg->type = aml_name_seg;
	name_seg->flags = 0;
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
	printf_serial("name_space_modifier_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_space_modifier_obj = malloc(sizeof(*name_space_modifier_obj));
	name_space_modifier_obj->parent = parent;
	name_space_modifier_obj->string.initial = aml.initial;
	name_space_modifier_obj->string.length = 0;
	name_space_modifier_obj->type = aml_name_space_modifier_obj;
	name_space_modifier_obj->flags = 0;
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
		name_space_modifier_obj->flags |= AML_SYMBOL_ERROR; // Syntax error
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return name_space_modifier_obj;
}

// <name_string> := <root_char> <name_path> | <prefix_path> <name_path>
AMLSymbol *analyse_aml_name_string(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("name_string aml.length = %#010.8x\n", aml.length);
	AMLSymbol *name_string = malloc(sizeof(*name_string));
	unsigned int string_length;
	char *string_writer;
	name_string->parent = parent;
	name_string->string.initial = aml.initial;
	name_string->string.length = 0;
	name_string->type = aml_name_string;
	name_string->flags = 0;
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
	printf_serial("named_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *named_field = malloc(sizeof(*named_field));
	named_field->parent = parent;
	named_field->string.initial = aml.initial;
	named_field->string.length = 0;
	named_field->type = aml_named_field;
	named_field->flags = 0;
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
	printf_serial("named_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *named_obj = malloc(sizeof(*named_obj));
	named_obj->parent = parent;
	named_obj->string.initial = aml.initial;
	named_obj->string.length = 0;
	named_obj->type = aml_named_obj;
	named_obj->flags = 0;
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
	named_obj->component.named_obj.def_index_field = NULL;
	named_obj->component.named_obj.def_method = NULL;
	named_obj->component.named_obj.def_mutex = NULL;
	named_obj->component.named_obj.def_op_region = NULL;
	named_obj->component.named_obj.def_power_res = NULL;
	named_obj->component.named_obj.def_processor = NULL;
	named_obj->component.named_obj.def_thermal_zone = NULL;
	switch(aml.initial[0])
	{
	case AML_BYTE_CREATE_BIT_FIELD_OP:
		named_obj->component.named_obj.def_create_bit_field = analyse_aml_def_create_bit_field(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_create_bit_field->string.length;
		break;
	case AML_BYTE_CREATE_BYTE_FIELD_OP:
		named_obj->component.named_obj.def_create_byte_field = analyse_aml_def_create_byte_field(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_create_byte_field->string.length;
		break;
	case AML_BYTE_CREATE_DWORD_FIELD_OP:
		named_obj->component.named_obj.def_create_dword_field = analyse_aml_def_create_dword_field(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_create_dword_field->string.length;
		break;
	case AML_BYTE_CREATE_QWORD_FIELD_OP:
		named_obj->component.named_obj.def_create_qword_field = analyse_aml_def_create_qword_field(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_create_qword_field->string.length;
		break;
	case AML_BYTE_CREATE_WORD_FIELD_OP:
		named_obj->component.named_obj.def_create_word_field = analyse_aml_def_create_word_field(named_obj, aml);
		named_obj->string.length += named_obj->component.named_obj.def_create_word_field->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_CREATE_FIELD_OP:
			named_obj->component.named_obj.def_create_field = analyse_aml_def_create_field(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_create_field->string.length;
			break;
		case AML_BYTE_DEVICE_OP:
			named_obj->component.named_obj.def_device = analyse_aml_def_device(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_device->string.length;
			break;
		case AML_BYTE_FIELD_OP:
			named_obj->component.named_obj.def_field = analyse_aml_def_field(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_field->string.length;
			break;
		case AML_BYTE_INDEX_FIELD_OP:
			named_obj->component.named_obj.def_index_field = analyse_aml_def_index_field(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_index_field->string.length;
			break;
		case AML_BYTE_MUTEX_OP:
			named_obj->component.named_obj.def_mutex = analyse_aml_def_mutex(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_mutex->string.length;
			break;
		case AML_BYTE_PROCESSOR_OP:
			named_obj->component.named_obj.def_processor = analyse_aml_def_processor(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_processor->string.length;
			break;
		case AML_BYTE_OP_REGION_OP:
			named_obj->component.named_obj.def_op_region = analyse_aml_def_op_region(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_op_region->string.length;
			break;
		case AML_BYTE_THERMAL_ZONE_OP:
			named_obj->component.named_obj.def_thermal_zone = analyse_aml_def_thermal_zone(named_obj, aml);
			named_obj->string.length += named_obj->component.named_obj.def_thermal_zone->string.length;
			break;
		default:
			named_obj->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
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
		named_obj->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return named_obj;
}

// <not_op> := AML_BYTE_NOT_OP
AMLSymbol *analyse_aml_not_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("not_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *not_op = malloc(sizeof(*not_op));
	not_op->parent = parent;
	not_op->string.initial = aml.initial;
	not_op->string.length = 1;
	not_op->type = aml_not_op;
	not_op->flags = 0;
	if(!aml.length)
	{
		not_op->string.length = 0;
		not_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*not_op->string.initial != AML_BYTE_NOT_OP)
	{
		not_op->flags |= AML_SYMBOL_ERROR; // Incorrect not_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return not_op;
}

// <notify_object> := <super_name>
AMLSymbol *analyse_aml_notify_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("notify_object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *notify_object = malloc(sizeof(*notify_object));
	notify_object->parent = parent;
	notify_object->string.initial = aml.initial;
	notify_object->string.length = 0;
	notify_object->type = aml_notify_object;
	notify_object->flags = 0;
	notify_object->component.notify_object.super_name = analyse_aml_super_name(notify_object, aml);
	notify_object->string.length += notify_object->component.notify_object.super_name->string.length;
	return notify_object;
}

// <notify_value> := <term_arg>
AMLSymbol *analyse_aml_notify_value(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("notify_value aml.length = %#010.8x\n", aml.length);
	AMLSymbol *notify_value = malloc(sizeof(*notify_value));
	notify_value->parent = parent;
	notify_value->string.initial = aml.initial;
	notify_value->string.length = 0;
	notify_value->type = aml_notify_value;
	notify_value->flags = 0;
	notify_value->component.notify_value.term_arg = analyse_aml_term_arg(notify_value, aml);
	notify_value->string.length += notify_value->component.notify_value.term_arg->string.length;
	return notify_value;
}

// <notify_op> := AML_BYTE_NOTIFY_OP
AMLSymbol *analyse_aml_notify_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("notify_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *notify_op = malloc(sizeof(*notify_op));
	notify_op->parent = parent;
	notify_op->string.initial = aml.initial;
	notify_op->string.length = 1;
	notify_op->type = aml_notify_op;
	notify_op->flags = 0;
	if(!aml.length)
	{
		notify_op->string.length = 0;
		notify_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*notify_op->string.initial != AML_BYTE_NOTIFY_OP)
	{
		notify_op->flags |= AML_SYMBOL_ERROR; // Incorrect notify_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return notify_op;
}

// <null_char> := AML_BYTE_NULL_CHAR
AMLSymbol *analyse_aml_null_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("null_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *null_char = malloc(sizeof(*null_char));
	null_char->parent = parent;
	null_char->string.initial = aml.initial;
	null_char->string.length = 1;
	null_char->type = aml_null_char;
	null_char->flags = 0;
	if(!aml.length)
	{
		null_char->string.length = 0;
		null_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*null_char->string.initial != AML_BYTE_NULL_CHAR)
	{
		null_char->flags |= AML_SYMBOL_ERROR; // Incorrect null char
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return null_char;
}

// <null_name> := AML_BYTE_NULL_NAME
AMLSymbol *analyse_aml_null_name(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("null_name aml.length = %#010.8x\n", aml.length);
	AMLSymbol *null_name = malloc(sizeof(*null_name));
	null_name->parent = parent;
	null_name->string.initial = aml.initial;
	null_name->string.length = 1;
	null_name->type = aml_null_name;
	null_name->flags = 0;
	if(!aml.length)
	{
		null_name->string.length = 0;
		null_name->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*null_name->string.initial != AML_BYTE_NULL_NAME)
	{
		null_name->flags |= AML_SYMBOL_ERROR; // Incorrect null name
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return null_name;
}

// <num_bits> := <term_arg>
AMLSymbol *analyse_aml_num_bits(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("num_bits aml.length = %#010.8x\n", aml.length);
	AMLSymbol *num_bits = malloc(sizeof(*num_bits));
	num_bits->parent = parent;
	num_bits->string.initial = aml.initial;
	num_bits->string.length = 0;
	num_bits->type = aml_num_bits;
	num_bits->flags = 0;
	num_bits->component.num_bits.term_arg = analyse_aml_term_arg(num_bits, aml);
	num_bits->string.length += num_bits->component.num_bits.term_arg->string.length;
	aml.initial += num_bits->component.num_bits.term_arg->string.length;
	aml.length -= num_bits->component.num_bits.term_arg->string.length;
	return num_bits;
}

// <num_elements> := <byte_data>
AMLSymbol *analyse_aml_num_elements(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("num_elements aml.length = %#010.8x\n", aml.length);
	AMLSymbol *num_elements = malloc(sizeof(*num_elements));
	num_elements->parent = parent;
	num_elements->string.initial = aml.initial;
	num_elements->string.length = 0;
	num_elements->type = aml_num_elements;
	num_elements->flags = 0;
	num_elements->component.num_elements.byte_data = analyse_aml_byte_data(num_elements, aml);
	num_elements->component.num_elements.num_of_elements = *num_elements->component.num_elements.byte_data->string.initial;
	num_elements->string.length += num_elements->component.num_elements.byte_data->string.length;
	aml.initial += num_elements->component.num_elements.byte_data->string.length;
	aml.length -= num_elements->component.num_elements.byte_data->string.length;
	return num_elements;
}

// <obj_reference> := <term_arg>
AMLSymbol *analyse_aml_obj_reference(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("obj_reference aml.length = %#010.8x\n", aml.length);
	AMLSymbol *obj_reference = malloc(sizeof(*obj_reference));
	obj_reference->parent = parent;
	obj_reference->string.initial = aml.initial;
	obj_reference->string.length = 0;
	obj_reference->type = aml_obj_reference;
	obj_reference->flags = 0;
	obj_reference->component.obj_reference.term_arg = analyse_aml_term_arg(obj_reference, aml);
	obj_reference->string.length += obj_reference->component.obj_reference.term_arg->string.length;
	aml.initial += obj_reference->component.obj_reference.term_arg->string.length;
	aml.length -= obj_reference->component.obj_reference.term_arg->string.length;
	return obj_reference;
}

// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("object aml.length = %#010.8x\n", aml.length);
	AMLSymbol *object = malloc(sizeof(*object));
	object->parent = parent;
	object->string.initial = aml.initial;
	object->string.length = 0;
	object->type = aml_object;
	object->flags = 0;
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
	case AML_BYTE_CREATE_BIT_FIELD_OP:
	case AML_BYTE_CREATE_BYTE_FIELD_OP:
	case AML_BYTE_CREATE_DWORD_FIELD_OP:
	case AML_BYTE_CREATE_QWORD_FIELD_OP:
	case AML_BYTE_CREATE_WORD_FIELD_OP:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_METHOD_OP:
		object->component.object.named_obj = analyse_aml_named_obj(object, aml);
		object->string.length += object->component.object.named_obj->string.length;
		break;
	default:
		object->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return object;
}

// <object_type_op> := AML_BYTE_OBJECT_TYPE_OP
AMLSymbol *analyse_aml_object_type_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("object_type_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *object_type_op = malloc(sizeof(*object_type_op));
	object_type_op->parent = parent;
	object_type_op->string.initial = aml.initial;
	object_type_op->string.length = 1;
	object_type_op->type = aml_object_type_op;
	object_type_op->flags = 0;
	if(!aml.length)
	{
		object_type_op->string.length = 0;
		object_type_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*object_type_op->string.initial != AML_BYTE_OBJECT_TYPE_OP)
	{
		object_type_op->flags |= AML_SYMBOL_ERROR; // Incorrect object_type op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return object_type_op;
}

// <one_op> := AML_BYTE_ONE_OP
AMLSymbol *analyse_aml_one_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("one_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *one_op = malloc(sizeof(*one_op));
	one_op->parent = parent;
	one_op->string.initial = aml.initial;
	one_op->string.length = 1;
	one_op->type = aml_one_op;
	one_op->flags = 0;
	if(!aml.length)
	{
		one_op->string.length = 0;
		one_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*one_op->string.initial != AML_BYTE_ONE_OP)
	{
		one_op->flags |= AML_SYMBOL_ERROR; // Incorrect one op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return one_op;
}

// <ones_op> := AML_BYTE_ONES_OP
AMLSymbol *analyse_aml_ones_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("ones_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *ones_op = malloc(sizeof(*ones_op));
	ones_op->parent = parent;
	ones_op->string.initial = aml.initial;
	ones_op->string.length = 1;
	ones_op->type = aml_ones_op;
	ones_op->flags = 0;
	if(!aml.length)
	{
		ones_op->string.length = 0;
		ones_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*ones_op->string.initial != AML_BYTE_ONES_OP)
	{
		ones_op->flags |= AML_SYMBOL_ERROR; // Incorrect ones op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return ones_op;
}

// <op_region_op> := <ext_op_prefix> <op_region_op_suffix>
AMLSymbol *analyse_aml_op_region_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("op_region_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *op_region_op = malloc(sizeof(*op_region_op));
	op_region_op->parent = parent;
	op_region_op->string.initial = aml.initial;
	op_region_op->string.length = 0;
	op_region_op->type = aml_op_region_op;
	op_region_op->flags = 0;
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
	printf_serial("op_region_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *op_region_op_suffix = malloc(sizeof(*op_region_op_suffix));
	op_region_op_suffix->parent = parent;
	op_region_op_suffix->string.initial = aml.initial;
	op_region_op_suffix->string.length = 1;
	op_region_op_suffix->type = aml_op_region_op_suffix;
	op_region_op_suffix->flags = 0;
	if(!aml.length)
	{
		op_region_op_suffix->string.length = 0;
		op_region_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_OP_REGION_OP)
	{
		op_region_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return op_region_op_suffix;
}

// <operand> := <term_arg>
AMLSymbol *analyse_aml_operand(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("operand aml.length = %#010.8x\n", aml.length);
	AMLSymbol *operand = malloc(sizeof(*operand));
	operand->parent = parent;
	operand->string.initial = aml.initial;
	operand->string.length = 0;
	operand->type = aml_operand;
	operand->flags = 0;
	operand->component.operand.term_arg = analyse_aml_term_arg(operand, aml);
	operand->string.length += operand->component.operand.term_arg->string.length;
	return operand;
}

// <or_op> := AML_BYTE_OR_OP
AMLSymbol *analyse_aml_or_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("or_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *or_op = malloc(sizeof(*or_op));
	or_op->parent = parent;
	or_op->string.initial = aml.initial;
	or_op->string.length = 1;
	or_op->type = aml_or_op;
	or_op->flags = 0;
	if(!aml.length)
	{
		or_op->string.length = 0;
		or_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*or_op->string.initial != AML_BYTE_OR_OP)
	{
		or_op->flags |= AML_SYMBOL_ERROR; // Incorrect or_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return or_op;
}

// <package_element> := <data_ref_object> | <name_string>
AMLSymbol *analyse_aml_package_element(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("package_element aml.length = %#010.8x\n", aml.length);
	AMLSymbol *package_element = malloc(sizeof(*package_element));
	package_element->parent = parent;
	package_element->string.initial = aml.initial;
	package_element->string.length = 0;
	package_element->type = aml_package_element;
	package_element->flags = 0;
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
	case AML_BYTE_ZERO_OP:
		package_element->component.package_element.data_ref_object = analyse_aml_data_ref_object(package_element, aml);
		package_element->string.length += package_element->component.package_element.data_ref_object->string.length;
		aml.initial += package_element->component.package_element.data_ref_object->string.length;
		aml.length -= package_element->component.package_element.data_ref_object->string.length;
		break;
	case AML_BYTE_DUAL_NAME_PREFIX:
	case AML_BYTE_MULTI_NAME_PREFIX:
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
			package_element->flags |= AML_SYMBOL_ERROR; // Syntax error
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return package_element;
}

// <package_element_list> := Nothing | <package_element> <package_element_list>
AMLSymbol *analyse_aml_package_element_list(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("package_element_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *package_element_list = malloc(sizeof(*package_element_list));
	package_element_list->parent = parent;
	package_element_list->string.initial = aml.initial;
	package_element_list->string.length = 0;
	package_element_list->type = aml_package_element_list;
	package_element_list->flags = 0;
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
			package_element_list->flags |= AML_SYMBOL_ERROR; // Syntax error
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return package_element_list;
}

// <package_op> := AML_BYTE_PACKAGE_OP
AMLSymbol *analyse_aml_package_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("package_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *package_op = malloc(sizeof(*package_op));
	package_op->parent = parent;
	package_op->string.initial = aml.initial;
	package_op->string.length = 1;
	package_op->type = aml_package_op;
	package_op->flags = 0;
	if(!aml.length)
	{
		package_op->string.length = 0;
		package_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_PACKAGE_OP)
	{
		package_op->flags |= AML_SYMBOL_ERROR; // Incorrect package_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return package_op;
}

// <parent_prefix_char> := AML_BYTE_PARENT_PREFIX_CHAR
AMLSymbol *analyse_aml_parent_prefix_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("parent_prefix_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *parent_prefix_char = malloc(sizeof(*parent_prefix_char));
	parent_prefix_char->parent = parent;
	parent_prefix_char->string.initial = aml.initial;
	parent_prefix_char->string.length = 1;
	parent_prefix_char->type = aml_parent_prefix_char;
	parent_prefix_char->flags = 0;
	if(!aml.length)
	{
		parent_prefix_char->string.length = 0;
		parent_prefix_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*parent_prefix_char->string.initial != AML_BYTE_PARENT_PREFIX_CHAR)
	{
		parent_prefix_char->flags |= AML_SYMBOL_ERROR; // Incorrect parent prefix char
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return parent_prefix_char;
}

// <pblk_addr> := <dword_data>
AMLSymbol *analyse_aml_pblk_addr(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("pblk_addr aml.length = %#010.8x\n", aml.length);
	AMLSymbol *pblk_addr = malloc(sizeof(*pblk_addr));
	pblk_addr->parent = parent;
	pblk_addr->string.initial = aml.initial;
	pblk_addr->string.length = 0;
	pblk_addr->type = aml_pblk_addr;
	pblk_addr->flags = 0;
	pblk_addr->component.pblk_addr.dword_data = analyse_aml_dword_data(pblk_addr, aml);
	pblk_addr->string.length += pblk_addr->component.pblk_addr.dword_data->string.length;
	aml.initial += pblk_addr->component.pblk_addr.dword_data->string.length;
	aml.length -= pblk_addr->component.pblk_addr.dword_data->string.length;
	return pblk_addr;
}

// <pblk_len> := <byte_data>
AMLSymbol *analyse_aml_pblk_len(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("pblk_len aml.length = %#010.8x\n", aml.length);
	AMLSymbol *pblk_len = malloc(sizeof(*pblk_len));
	pblk_len->parent = parent;
	pblk_len->string.initial = aml.initial;
	pblk_len->string.length = 0;
	pblk_len->type = aml_pblk_len;
	pblk_len->flags = 0;
	pblk_len->component.pblk_len.byte_data = analyse_aml_byte_data(pblk_len, aml);
	pblk_len->string.length += pblk_len->component.pblk_len.byte_data->string.length;
	aml.initial += pblk_len->component.pblk_len.byte_data->string.length;
	aml.length -= pblk_len->component.pblk_len.byte_data->string.length;
	return pblk_len;
}

// <pkg_lead_byte>
AMLSymbol *analyse_aml_pkg_lead_byte(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("pkg_lead_byte aml.length = %#010.8x\n", aml.length);
	AMLSymbol *pkg_lead_byte = malloc(sizeof(*pkg_lead_byte));
	pkg_lead_byte->parent = parent;
	pkg_lead_byte->string.initial = aml.initial;
	pkg_lead_byte->string.length = 1;
	pkg_lead_byte->type = aml_pkg_lead_byte;
	pkg_lead_byte->flags = 0;
	return pkg_lead_byte;
}

// <pkg_length> := <pkg_lead_byte> | <pkg_lead_byte> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> | <pkg_lead_byte> <byte_data> <byte_data> <byte_data>
AMLSymbol *analyse_aml_pkg_length(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("pkg_length aml.length = %#010.8x\n", aml.length);
	AMLSymbol *pkg_length = malloc(sizeof(*pkg_length));
	pkg_length->parent = parent;
	pkg_length->string.initial = aml.initial;
	pkg_length->string.length = 0;
	pkg_length->type = aml_pkg_length;
	pkg_length->flags = 0;
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
	printf_serial("predicate aml.length = %#010.8x\n", aml.length);
	AMLSymbol *predicate = malloc(sizeof(*predicate));
	predicate->parent = parent;
	predicate->string.initial = aml.initial;
	predicate->string.length = 0;
	predicate->type = aml_predicate;
	predicate->flags = 0;
	predicate->component.predicate.term_arg = analyse_aml_term_arg(predicate, aml);
	predicate->string.length += predicate->component.predicate.term_arg->string.length;
	return predicate;
}

// <prefix_path> := Nothing | <parent_prefix_char> <prefix_path>
AMLSymbol *analyse_aml_prefix_path(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("prefix_path aml.length = %#010.8x\n", aml.length);
	AMLSymbol *prefix_path = malloc(sizeof(*prefix_path));
	char *string_writer;
	prefix_path->parent = parent;
	prefix_path->string.initial = aml.initial;
	prefix_path->string.length = 0;
	prefix_path->type = aml_prefix_path;
	prefix_path->flags = 0;
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

// <proc_id> := <byte_data>
AMLSymbol *analyse_aml_proc_id(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("proc_id aml.length = %#010.8x\n", aml.length);
	AMLSymbol *proc_id = malloc(sizeof(*proc_id));
	proc_id->parent = parent;
	proc_id->string.initial = aml.initial;
	proc_id->string.length = 0;
	proc_id->type = aml_proc_id;
	proc_id->flags = 0;
	proc_id->component.proc_id.byte_data = analyse_aml_byte_data(proc_id, aml);
	proc_id->string.length += proc_id->component.proc_id.byte_data->string.length;
	aml.initial += proc_id->component.proc_id.byte_data->string.length;
	aml.length -= proc_id->component.proc_id.byte_data->string.length;
	return proc_id;
}

// <processor_op> := <ext_op_prefix> <processor_op_suffix>
AMLSymbol *analyse_aml_processor_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("processor_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *processor_op = malloc(sizeof(*processor_op));
	processor_op->parent = parent;
	processor_op->string.initial = aml.initial;
	processor_op->string.length = 0;
	processor_op->type = aml_processor_op;
	processor_op->flags = 0;
	processor_op->component.processor_op.ext_op_prefix = analyse_aml_ext_op_prefix(processor_op, aml);
	processor_op->string.length += processor_op->component.processor_op.ext_op_prefix->string.length;
	aml.initial += processor_op->component.processor_op.ext_op_prefix->string.length;
	aml.length -= processor_op->component.processor_op.ext_op_prefix->string.length;
	processor_op->component.processor_op.processor_op_suffix = analyse_aml_processor_op_suffix(processor_op, aml);
	processor_op->string.length += processor_op->component.processor_op.processor_op_suffix->string.length;
	aml.initial += processor_op->component.processor_op.processor_op_suffix->string.length;
	aml.length -= processor_op->component.processor_op.processor_op_suffix->string.length;
	return processor_op;
}

// <processor_op_suffix> := AML_BYTE_PROCESSOR_OP
AMLSymbol *analyse_aml_processor_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("processor_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *processor_op_suffix = malloc(sizeof(*processor_op_suffix));
	processor_op_suffix->parent = parent;
	processor_op_suffix->string.initial = aml.initial;
	processor_op_suffix->string.length = 1;
	processor_op_suffix->type = aml_processor_op_suffix;
	processor_op_suffix->flags = 0;
	if(!aml.length)
	{
		processor_op_suffix->string.length = 0;
		processor_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*processor_op_suffix->string.initial != AML_BYTE_PROCESSOR_OP)
	{
		processor_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect processor_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return processor_op_suffix;
}

// <quotient> := <term_arg>
AMLSymbol *analyse_aml_quotient(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("quotient aml.length = %#010.8x\n", aml.length);
	AMLSymbol *quotient = malloc(sizeof(*quotient));
	quotient->parent = parent;
	quotient->string.initial = aml.initial;
	quotient->string.length = 0;
	quotient->type = aml_quotient;
	quotient->flags = 0;
	quotient->component.quotient.term_arg = analyse_aml_term_arg(quotient, aml);
	quotient->string.length += quotient->component.quotient.term_arg->string.length;
	return quotient;
}

// <qword_const> := <qword_prefix> <qword_data>
AMLSymbol *analyse_aml_qword_const(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("qword_const aml.length = %#010.8x\n", aml.length);
	AMLSymbol *qword_const = malloc(sizeof(*qword_const));
	qword_const->parent = parent;
	qword_const->string.initial = aml.initial;
	qword_const->string.length = 0;
	qword_const->type = aml_qword_const;
	qword_const->flags = 0;
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
	printf_serial("qword_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *qword_data = malloc(sizeof(*qword_data));
	qword_data->parent = parent;
	qword_data->string.initial = aml.initial;
	qword_data->string.length = 0;
	qword_data->type = aml_qword_data;
	qword_data->flags = 0;
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

// <ref_of_op> := AML_BYTE_REF_OF_OP
AMLSymbol *analyse_aml_ref_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("ref_of_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *ref_of_op = malloc(sizeof(*ref_of_op));
	ref_of_op->parent = parent;
	ref_of_op->string.initial = aml.initial;
	ref_of_op->string.length = 1;
	ref_of_op->type = aml_ref_of_op;
	ref_of_op->flags = 0;
	if(!aml.length)
	{
		ref_of_op->string.length = 0;
		ref_of_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*ref_of_op->string.initial != AML_BYTE_REF_OF_OP)
	{
		ref_of_op->flags |= AML_SYMBOL_ERROR; // Incorrect qword prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return ref_of_op;
}

// <qword_prefix> := AML_BYTE_QWORD_PREFIX
AMLSymbol *analyse_aml_qword_prefix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("qword_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *qword_prefix = malloc(sizeof(*qword_prefix));
	qword_prefix->parent = parent;
	qword_prefix->string.initial = aml.initial;
	qword_prefix->string.length = 1;
	qword_prefix->type = aml_qword_prefix;
	qword_prefix->flags = 0;
	if(!aml.length)
	{
		qword_prefix->string.length = 0;
		qword_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*qword_prefix->string.initial != AML_BYTE_QWORD_PREFIX)
	{
		qword_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect qword prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return qword_prefix;
}

// <reference_type_opcode> := <DefRefOf> <DefDerefOf> <DefIndex> <UserTermObj>
AMLSymbol *analyse_aml_reference_type_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("reference_type_opcode aml.length = %#010.8x\n", aml.length);
	AMLSymbol *reference_type_opcode = malloc(sizeof(*reference_type_opcode));
	reference_type_opcode->parent = parent;
	reference_type_opcode->string.initial = aml.initial;
	reference_type_opcode->string.length = 0;
	reference_type_opcode->type = aml_reference_type_opcode;
	reference_type_opcode->flags = 0;
	reference_type_opcode->component.reference_type_opcode.def_ref_of = NULL;
	reference_type_opcode->component.reference_type_opcode.def_deref_of = NULL;
	reference_type_opcode->component.reference_type_opcode.def_index = NULL;
	reference_type_opcode->component.reference_type_opcode.user_term_obj = NULL;
	switch(*reference_type_opcode->string.initial)
	{
	case AML_BYTE_DEREF_OF_OP:
		reference_type_opcode->component.reference_type_opcode.def_deref_of = analyse_aml_def_deref_of(reference_type_opcode, aml);
		reference_type_opcode->string.length += reference_type_opcode->component.reference_type_opcode.def_deref_of->string.length;
		aml.initial += reference_type_opcode->component.reference_type_opcode.def_deref_of->string.length;
		aml.length -= reference_type_opcode->component.reference_type_opcode.def_deref_of->string.length;
		break;
	case AML_BYTE_INDEX_OP:
		reference_type_opcode->component.reference_type_opcode.def_index = analyse_aml_def_index(reference_type_opcode, aml);
		reference_type_opcode->string.length += reference_type_opcode->component.reference_type_opcode.def_index->string.length;
		aml.initial += reference_type_opcode->component.reference_type_opcode.def_index->string.length;
		aml.length -= reference_type_opcode->component.reference_type_opcode.def_index->string.length;
		break;
	case AML_BYTE_REF_OF_OP:
		reference_type_opcode->component.reference_type_opcode.def_ref_of = analyse_aml_def_ref_of(reference_type_opcode, aml);
		reference_type_opcode->string.length += reference_type_opcode->component.reference_type_opcode.def_ref_of->string.length;
		aml.initial += reference_type_opcode->component.reference_type_opcode.def_ref_of->string.length;
		aml.length -= reference_type_opcode->component.reference_type_opcode.def_ref_of->string.length;
		break;
	default:
		reference_type_opcode->component.reference_type_opcode.user_term_obj = analyse_aml_user_term_obj(reference_type_opcode, aml);
		reference_type_opcode->string.length += reference_type_opcode->component.reference_type_opcode.user_term_obj->string.length;
		aml.initial += reference_type_opcode->component.reference_type_opcode.user_term_obj->string.length;
		aml.length -= reference_type_opcode->component.reference_type_opcode.user_term_obj->string.length;
		break;
	}
	return reference_type_opcode;
}

// <region_len> := <term_arg>
AMLSymbol *analyse_aml_region_len(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("region_len aml.length = %#010.8x\n", aml.length);
	AMLSymbol *region_len = malloc(sizeof(*region_len));
	region_len->parent = parent;
	region_len->string.initial = aml.initial;
	region_len->string.length = 0;
	region_len->type = aml_region_len;
	region_len->flags = 0;
	region_len->component.region_len.term_arg = analyse_aml_term_arg(region_len, aml);
	region_len->string.length += region_len->component.region_len.term_arg->string.length;
	return region_len;
}

// <region_offset> := <term_arg>
AMLSymbol *analyse_aml_region_offset(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("region_offset aml.length = %#010.8x\n", aml.length);
	AMLSymbol *region_offset = malloc(sizeof(*region_offset));
	region_offset->parent = parent;
	region_offset->string.initial = aml.initial;
	region_offset->string.length = 0;
	region_offset->type = aml_region_offset;
	region_offset->flags = 0;
	region_offset->component.region_offset.term_arg = analyse_aml_term_arg(region_offset, aml);
	region_offset->string.length += region_offset->component.region_offset.term_arg->string.length;
	return region_offset;
}

// <region_space>
AMLSymbol *analyse_aml_region_space(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("region_space aml.length = %#010.8x\n", aml.length);
	AMLSymbol *region_space = malloc(sizeof(*region_space));
	region_space->parent = parent;
	region_space->string.initial = aml.initial;
	region_space->string.length = 1;
	region_space->type = aml_region_space;
	region_space->flags = 0;
	if(!aml.length)
	{
		region_space->string.length = 0;
		region_space->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	return region_space;
}

// <release_op> := <ext_op_prefix> <release_op_suffix>
AMLSymbol *analyse_aml_release_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("release_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *release_op = malloc(sizeof(*release_op));
	release_op->parent = parent;
	release_op->string.initial = aml.initial;
	release_op->string.length = 0;
	release_op->type = aml_release_op;
	release_op->flags = 0;
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
	printf_serial("release_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *release_op_suffix = malloc(sizeof(*release_op_suffix));
	release_op_suffix->parent = parent;
	release_op_suffix->string.initial = aml.initial;
	release_op_suffix->string.length = 1;
	release_op_suffix->type = aml_release_op_suffix;
	release_op_suffix->flags = 0;
	if(!aml.length)
	{
		release_op_suffix->string.length = 0;
		release_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_RELEASE_OP)
	{
		release_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect release_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return release_op_suffix;
}

// <remainder> := <term_arg>
AMLSymbol *analyse_aml_remainder(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("remainder aml.length = %#010.8x\n", aml.length);
	AMLSymbol *remainder = malloc(sizeof(*remainder));
	remainder->parent = parent;
	remainder->string.initial = aml.initial;
	remainder->string.length = 0;
	remainder->type = aml_remainder;
	remainder->flags = 0;
	remainder->component.remainder.term_arg = analyse_aml_term_arg(remainder, aml);
	remainder->string.length += remainder->component.remainder.term_arg->string.length;
	return remainder;
}

// <return_op> := AML_BYTE_RETURN_OP
AMLSymbol *analyse_aml_return_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("return_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *return_op = malloc(sizeof(*return_op));
	return_op->parent = parent;
	return_op->string.initial = aml.initial;
	return_op->string.length = 1;
	return_op->type = aml_return_op;
	return_op->flags = 0;
	if(!aml.length)
	{
		return_op->string.length = 0;
		return_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_RETURN_OP)
	{
		return_op->flags |= AML_SYMBOL_ERROR; // Incorrect return_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return return_op;
}

// <reserved_field> := <reserved_field_op> <pkg_length>
AMLSymbol *analyse_aml_reserved_field(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("reserved_field aml.length = %#010.8x\n", aml.length);
	AMLSymbol *reserved_field = malloc(sizeof(*reserved_field));
	reserved_field->parent = parent;
	reserved_field->string.initial = aml.initial;
	reserved_field->string.length = 0;
	reserved_field->type = aml_reserved_field;
	reserved_field->flags = 0;
	reserved_field->component.reserved_field.reserved_field_op = analyse_aml_reserved_field_op(reserved_field, aml);
	reserved_field->string.length += reserved_field->component.reserved_field.reserved_field_op->string.length;
	aml.initial += reserved_field->component.reserved_field.reserved_field_op->string.length;
	aml.length -= reserved_field->component.reserved_field.reserved_field_op->string.length;
	reserved_field->component.reserved_field.pkg_length = analyse_aml_pkg_length(reserved_field, aml);
	reserved_field->string.length += reserved_field->component.reserved_field.pkg_length->string.length;
	aml.initial += reserved_field->component.reserved_field.pkg_length->string.length;
	aml.length -= reserved_field->component.reserved_field.pkg_length->string.length;
	return reserved_field;
}

// <reserved_field_op> := AML_BYTE_RESERVED_FIELD_OP
AMLSymbol *analyse_aml_reserved_field_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("reserved_field_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *reserved_field_op = malloc(sizeof(*reserved_field_op));
	reserved_field_op->parent = parent;
	reserved_field_op->string.initial = aml.initial;
	reserved_field_op->string.length = 1;
	reserved_field_op->type = aml_reserved_field_op;
	reserved_field_op->flags = 0;
	if(!aml.length)
	{
		reserved_field_op->string.length = 0;
		reserved_field_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*reserved_field_op->string.initial != AML_BYTE_RESERVED_FIELD_OP)
	{
		reserved_field_op->flags |= AML_SYMBOL_ERROR; // Incorrect reserved_field_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return reserved_field_op;
}

// <revision_op> := <ext_op_prefix> <revision_op_suffix>
AMLSymbol *analyse_aml_revision_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("revision_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *revision_op = malloc(sizeof(*revision_op));
	revision_op->parent = parent;
	revision_op->string.initial = aml.initial;
	revision_op->string.length = 0;
	revision_op->type = aml_revision_op;
	revision_op->flags = 0;
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
	printf_serial("revision_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *revision_op_suffix = malloc(sizeof(*revision_op_suffix));
	revision_op_suffix->parent = parent;
	revision_op_suffix->string.initial = aml.initial;
	revision_op_suffix->string.length = 1;
	revision_op_suffix->type = aml_revision_op_suffix;
	revision_op_suffix->flags = 0;
	if(!aml.length)
	{
		revision_op_suffix->string.length = 0;
		revision_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*revision_op_suffix->string.initial != AML_BYTE_REVISION_OP)
	{
		revision_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect revision op prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return revision_op_suffix;
}

// <root_char> := AML_BYTE_ROOT_CHAR
AMLSymbol *analyse_aml_root_char(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("root_char aml.length = %#010.8x\n", aml.length);
	AMLSymbol *root_char = malloc(sizeof(*root_char));
	root_char->parent = parent;
	root_char->string.initial = aml.initial;
	root_char->string.length = 1;
	root_char->type = aml_root_char;
	root_char->flags = 0;
	if(!aml.length)
	{
		root_char->string.length = 0;
		root_char->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*root_char->string.initial != AML_BYTE_ROOT_CHAR)
	{
		root_char->flags |= AML_SYMBOL_ERROR; // Incorrect root_char
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return root_char;
}

// <scope_op> := AML_BYTE_SCOPE_OP
AMLSymbol *analyse_aml_scope_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("scope_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *scope_op = malloc(sizeof(*scope_op));
	scope_op->parent = parent;
	scope_op->string.initial = aml.initial;
	scope_op->string.length = 1;
	scope_op->type = aml_scope_op;
	scope_op->flags = 0;
	if(!aml.length)
	{
		scope_op->string.length = 0;
		scope_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*scope_op->string.initial != AML_BYTE_SCOPE_OP)
	{
		scope_op->flags |= AML_SYMBOL_ERROR; // Incorrect scope op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return scope_op;
}

// <search_pkg> := <term_arg>
AMLSymbol *analyse_aml_search_pkg(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("search_pkg aml.length = %#010.8x\n", aml.length);
	AMLSymbol *search_pkg = malloc(sizeof(*search_pkg));
	search_pkg->parent = parent;
	search_pkg->string.initial = aml.initial;
	search_pkg->string.length = 0;
	search_pkg->type = aml_search_pkg;
	search_pkg->flags = 0;
	search_pkg->component.search_pkg.term_arg = analyse_aml_term_arg(search_pkg, aml);
	search_pkg->string.length += search_pkg->component.search_pkg.term_arg->string.length;
	aml.initial += search_pkg->component.search_pkg.term_arg->string.length;
	aml.length -= search_pkg->component.search_pkg.term_arg->string.length;
	return search_pkg;
}

// <seg_count> := 0x01 - 0xff
AMLSymbol *analyse_aml_seg_count(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("seg_count aml.length = %#010.8x\n", aml.length);
	AMLSymbol *seg_count = malloc(sizeof(*seg_count));
	seg_count->parent = parent;
	seg_count->string.initial = aml.initial;
	seg_count->string.length = 1;
	seg_count->type = aml_seg_count;
	seg_count->flags = 0;
	if(!aml.length)
	{
		seg_count->string.length = 0;
		seg_count->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*seg_count->string.initial == 0x00)
	{
		seg_count->flags |= AML_SYMBOL_ERROR; // SegCount can be from 1 to 255.
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return seg_count;
}

// <shift_count> := <term_arg>
AMLSymbol *analyse_aml_shift_count(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("shift_count aml.length = %#010.8x\n", aml.length);
	AMLSymbol *shift_count = malloc(sizeof(*shift_count));
	shift_count->parent = parent;
	shift_count->string.initial = aml.initial;
	shift_count->string.length = 0;
	shift_count->type = aml_shift_count;
	shift_count->flags = 0;
	shift_count->component.shift_count.term_arg = analyse_aml_term_arg(shift_count, aml);
	shift_count->string.length += shift_count->component.shift_count.term_arg->string.length;
	aml.initial += shift_count->component.shift_count.term_arg->string.length;
	aml.length -= shift_count->component.shift_count.term_arg->string.length;
	return shift_count;
}

// <shift_left_op> := AML_BYTE_SHIFT_LEFT
AMLSymbol *analyse_aml_shift_left_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("shift_left_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *shift_left_op = malloc(sizeof(*shift_left_op));
	shift_left_op->parent = parent;
	shift_left_op->string.initial = aml.initial;
	shift_left_op->string.length = 1;
	shift_left_op->type = aml_shift_left_op;
	shift_left_op->flags = 0;
	if(!aml.length)
	{
		shift_left_op->string.length = 0;
		shift_left_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*shift_left_op->string.initial != AML_BYTE_SHIFT_LEFT_OP)
	{
		shift_left_op->flags |= AML_SYMBOL_ERROR; // Incorrect shift_left_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return shift_left_op;
}

// <shift_right_op> := AML_BYTE_SHIFT_RIGHT
AMLSymbol *analyse_aml_shift_right_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("shift_right_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *shift_right_op = malloc(sizeof(*shift_right_op));
	shift_right_op->parent = parent;
	shift_right_op->string.initial = aml.initial;
	shift_right_op->string.length = 1;
	shift_right_op->type = aml_shift_right_op;
	shift_right_op->flags = 0;
	if(!aml.length)
	{
		shift_right_op->string.length = 0;
		shift_right_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*shift_right_op->string.initial != AML_BYTE_SHIFT_RIGHT_OP)
	{
		shift_right_op->flags |= AML_SYMBOL_ERROR; // Incorrect shift_right_op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return shift_right_op;
}

// <simple_name> := <name_string> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_simple_name(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("simple_name aml.length = %#010.8x\n", aml.length);
	AMLSymbol *simple_name = malloc(sizeof(*simple_name));
	simple_name->parent = parent;
	simple_name->string.initial = aml.initial;
	simple_name->string.length = 0;
	simple_name->type = aml_simple_name;
	simple_name->flags = 0;
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
			simple_name->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return simple_name;
}

// <size_of_op> := AML_BYTE_SIZE_OF_OP
AMLSymbol *analyse_aml_size_of_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("size_of_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *size_of_op = malloc(sizeof(*size_of_op));
	size_of_op->parent = parent;
	size_of_op->string.initial = aml.initial;
	size_of_op->string.length = 1;
	size_of_op->type = aml_size_of_op;
	size_of_op->flags = 0;
	if(!aml.length)
	{
		size_of_op->string.length = 0;
		size_of_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*size_of_op->string.initial != AML_BYTE_SIZE_OF_OP)
	{
		size_of_op->flags |= AML_SYMBOL_ERROR; // Incorrect size of op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return size_of_op;
}

// <sleep_op> := <ext_op_prefix> <sleep_op_suffix>
AMLSymbol *analyse_aml_sleep_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("sleep_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *sleep_op = malloc(sizeof(*sleep_op));
	sleep_op->parent = parent;
	sleep_op->string.initial = aml.initial;
	sleep_op->string.length = 0;
	sleep_op->type = aml_sleep_op;
	sleep_op->flags = 0;
	sleep_op->component.sleep_op.ext_op_prefix = analyse_aml_ext_op_prefix(sleep_op, aml);
	sleep_op->string.length += sleep_op->component.sleep_op.ext_op_prefix->string.length;
	aml.initial += sleep_op->component.sleep_op.ext_op_prefix->string.length;
	aml.length -= sleep_op->component.sleep_op.ext_op_prefix->string.length;
	sleep_op->component.sleep_op.sleep_op_suffix = analyse_aml_sleep_op_suffix(sleep_op, aml);
	sleep_op->string.length += sleep_op->component.sleep_op.sleep_op_suffix->string.length;
	aml.initial += sleep_op->component.sleep_op.sleep_op_suffix->string.length;
	aml.length -= sleep_op->component.sleep_op.sleep_op_suffix->string.length;
	return sleep_op;
}

// <sleep_op_suffix> := AML_BYTE_SLEEP_OP
AMLSymbol *analyse_aml_sleep_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("sleep_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *sleep_op_suffix = malloc(sizeof(*sleep_op_suffix));
	sleep_op_suffix->parent = parent;
	sleep_op_suffix->string.initial = aml.initial;
	sleep_op_suffix->string.length = 1;
	sleep_op_suffix->type = aml_sleep_op_suffix;
	sleep_op_suffix->flags = 0;
	if(!aml.length)
	{
		sleep_op_suffix->string.length = 0;
		sleep_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_SLEEP_OP)
	{
		sleep_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect sleep_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return sleep_op_suffix;
}

// <source_buff> := <term_arg>
AMLSymbol *analyse_aml_source_buff(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("source_buff aml.length = %#010.8x\n", aml.length);
	AMLSymbol *source_buff = malloc(sizeof(*source_buff));
	source_buff->parent = parent;
	source_buff->string.initial = aml.initial;
	source_buff->string.length = 0;
	source_buff->type = aml_source_buff;
	source_buff->flags = 0;
	source_buff->component.source_buff.term_arg = analyse_aml_term_arg(source_buff, aml);
	source_buff->string.length += source_buff->component.source_buff.term_arg->string.length;
	aml.initial += source_buff->component.source_buff.term_arg->string.length;
	aml.length -= source_buff->component.source_buff.term_arg->string.length;
	return source_buff;
}

// <stall_op> := <ext_op_prefix> <stall_op_suffix>
AMLSymbol *analyse_aml_stall_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("stall_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *stall_op = malloc(sizeof(*stall_op));
	stall_op->parent = parent;
	stall_op->string.initial = aml.initial;
	stall_op->string.length = 0;
	stall_op->type = aml_stall_op;
	stall_op->flags = 0;
	stall_op->component.stall_op.ext_op_prefix = analyse_aml_ext_op_prefix(stall_op, aml);
	stall_op->string.length += stall_op->component.stall_op.ext_op_prefix->string.length;
	aml.initial += stall_op->component.stall_op.ext_op_prefix->string.length;
	aml.length -= stall_op->component.stall_op.ext_op_prefix->string.length;
	stall_op->component.stall_op.stall_op_suffix = analyse_aml_stall_op_suffix(stall_op, aml);
	stall_op->string.length += stall_op->component.stall_op.stall_op_suffix->string.length;
	aml.initial += stall_op->component.stall_op.stall_op_suffix->string.length;
	aml.length -= stall_op->component.stall_op.stall_op_suffix->string.length;
	return stall_op;
}

// <stall_op_suffix> := AML_BYTE_STALL_OP
AMLSymbol *analyse_aml_stall_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("stall_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *stall_op_suffix = malloc(sizeof(*stall_op_suffix));
	stall_op_suffix->parent = parent;
	stall_op_suffix->string.initial = aml.initial;
	stall_op_suffix->string.length = 1;
	stall_op_suffix->type = aml_stall_op_suffix;
	stall_op_suffix->flags = 0;
	if(!aml.length)
	{
		stall_op_suffix->string.length = 0;
		stall_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_STALL_OP)
	{
		stall_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect stall_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return stall_op_suffix;
}

// <start_index> := <term_arg>
AMLSymbol *analyse_aml_start_index(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("start_index aml.length = %#010.8x\n", aml.length);
	AMLSymbol *start_index = malloc(sizeof(*start_index));
	start_index->parent = parent;
	start_index->string.initial = aml.initial;
	start_index->string.length = 0;
	start_index->type = aml_start_index;
	start_index->flags = 0;
	start_index->component.start_index.term_arg = analyse_aml_term_arg(start_index, aml);
	start_index->string.length += start_index->component.start_index.term_arg->string.length;
	return start_index;
}

// <statement_opcode> := <def_break> | <def_breakpoint> | <def_continue> | <def_fatal> | <def_if_else> | <def_noop> | <def_notify> | <def_release> | <def_reset> | <def_return> | <def_signal> | <def_sleep> | <def_stall> | <def_while>
AMLSymbol *analyse_aml_statement_opcode(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("statement_opcode aml.length = %#010.8x\n", aml.length);
	AMLSymbol *statement_opcode = malloc(sizeof(*statement_opcode));
	statement_opcode->parent = parent;
	statement_opcode->string.initial = aml.initial;
	statement_opcode->string.length = 0;
	statement_opcode->type = aml_statement_opcode;
	statement_opcode->flags = 0;
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
	case AML_BYTE_BREAK_OP:
		statement_opcode->component.statement_opcode.def_break = analyse_aml_def_break(statement_opcode, aml);
		statement_opcode->string.length += statement_opcode->component.statement_opcode.def_break->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_RELEASE_OP:
			statement_opcode->component.statement_opcode.def_release = analyse_aml_def_release(statement_opcode, aml);
			statement_opcode->string.length += statement_opcode->component.statement_opcode.def_release->string.length;
			break;
		case AML_BYTE_SLEEP_OP:
			statement_opcode->component.statement_opcode.def_sleep = analyse_aml_def_sleep(statement_opcode, aml);
			statement_opcode->string.length += statement_opcode->component.statement_opcode.def_sleep->string.length;
			break;
		case AML_BYTE_STALL_OP:
			statement_opcode->component.statement_opcode.def_stall = analyse_aml_def_stall(statement_opcode, aml);
			statement_opcode->string.length += statement_opcode->component.statement_opcode.def_stall->string.length;
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
	case AML_BYTE_NOTIFY_OP:
		statement_opcode->component.statement_opcode.def_notify = analyse_aml_def_notify(statement_opcode, aml);
		statement_opcode->string.length += statement_opcode->component.statement_opcode.def_notify->string.length;
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
		statement_opcode->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		break;
	}
	return statement_opcode;
}

// <store_op> := AML_BYTE_STORE_OP
AMLSymbol *analyse_aml_store_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("store_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *store_op = malloc(sizeof(*store_op));
	store_op->parent = parent;
	store_op->string.initial = aml.initial;
	store_op->string.length = 1;
	store_op->type = aml_store_op;
	store_op->flags = 0;
	if(!aml.length)
	{
		store_op->string.length = 0;
		store_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*store_op->string.initial != AML_BYTE_STORE_OP)
	{
		store_op->flags |= AML_SYMBOL_ERROR; // Incorrect store op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return store_op;
}

// <string> := <string_prefix> <ascii_char_list> <null_char>
AMLSymbol *analyse_aml_string(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("string aml.length = %#010.8x\n", aml.length);
	AMLSymbol *string = malloc(sizeof(*string));
	string->parent = parent;
	string->string.initial = aml.initial;
	string->string.length = 0;
	string->type = aml_string;
	string->flags = 0;
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
	printf_serial("string_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *string_prefix = malloc(sizeof(*string_prefix));
	string_prefix->parent = parent;
	string_prefix->string.initial = aml.initial;
	string_prefix->string.length = 1;
	string_prefix->type = aml_string_prefix;
	string_prefix->flags = 0;
	if(!aml.length)
	{
		string_prefix->string.length = 0;
		string_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*string_prefix->string.initial != AML_BYTE_STRING_PREFIX)
	{
		string_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect string prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return string_prefix;
}

// <subtract_op> := AML_BYTE_SUBTRACT_OP
AMLSymbol *analyse_aml_subtract_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("subtract_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *subtract_op = malloc(sizeof(*subtract_op));
	subtract_op->parent = parent;
	subtract_op->string.initial = aml.initial;
	subtract_op->string.length = 1;
	subtract_op->type = aml_subtract_op;
	subtract_op->flags = 0;
	if(!aml.length)
	{
		subtract_op->string.length = 0;
		subtract_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*subtract_op->string.initial != AML_BYTE_SUBTRACT_OP)
	{
		subtract_op->flags |= AML_SYMBOL_ERROR; // Incorrect subtract op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return subtract_op;
}

// <super_name> := <simple_name> | <debug_obj> | <reference_type_opcode>
AMLSymbol *analyse_aml_super_name(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("super_name aml.length = %#010.8x\n", aml.length);
	AMLSymbol *super_name = malloc(sizeof(*super_name));
	super_name->parent = parent;
	super_name->string.initial = aml.initial;
	super_name->string.length = 0;
	super_name->type = aml_super_name;
	super_name->flags = 0;
	super_name->component.super_name.debug_obj = NULL;
	super_name->component.super_name.reference_type_opcode = NULL;
	super_name->component.super_name.simple_name = NULL;
	switch(*super_name->string.initial)
	{
	case AML_BYTE_EXT_OP_PREFIX:
		switch(super_name->string.initial[1])
		{
		case AML_BYTE_DEBUG_OP:
			super_name->component.super_name.debug_obj = analyse_aml_debug_obj(super_name, aml);
			super_name->string.length += super_name->component.super_name.debug_obj->string.length;
			aml.initial += super_name->component.super_name.debug_obj->string.length;
			aml.length -= super_name->component.super_name.debug_obj->string.length;
			break;
		default:
			super_name->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
			break;
		}
		break;
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_REF_OF_OP:
		super_name->component.super_name.reference_type_opcode = analyse_aml_reference_type_opcode(super_name, aml);
		super_name->string.length += super_name->component.super_name.reference_type_opcode->string.length;
		aml.initial += super_name->component.super_name.reference_type_opcode->string.length;
		aml.length -= super_name->component.super_name.reference_type_opcode->string.length;
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
			// UserTermObj
			super_name->component.super_name.reference_type_opcode = analyse_aml_reference_type_opcode(super_name, aml);
			super_name->string.length += super_name->component.super_name.reference_type_opcode->string.length;
			aml.initial += super_name->component.super_name.reference_type_opcode->string.length;
			aml.length -= super_name->component.super_name.reference_type_opcode->string.length;
		}
		break;
	}
	return super_name;
}

// <sync_flags>
AMLSymbol *analyse_aml_sync_flags(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("sync_flags aml.length = %#010.8x\n", aml.length);
	AMLSymbol *sync_flags = malloc(sizeof(*sync_flags));
	sync_flags->parent = parent;
	sync_flags->string.initial = aml.initial;
	sync_flags->string.length = 1;
	sync_flags->type = aml_sync_flags;
	sync_flags->flags = 0;
	if(!aml.length)
	{
		sync_flags->string.length = 0;
		sync_flags->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(0x10 <= *aml.initial)
	{
		sync_flags->flags |= AML_SYMBOL_ERROR; // Incorrect sync flags
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return sync_flags;
}

// <target> := <super_name> | <null_name>
AMLSymbol *analyse_aml_target(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("target aml.length = %#010.8x\n", aml.length);
	AMLSymbol *target = malloc(sizeof(*target));
	target->parent = parent;
	target->string.initial = aml.initial;
	target->string.length = 0;
	target->type = aml_target;
	target->flags = 0;
	target->component.target.super_name = NULL;
	target->component.target.null_name = NULL;
	if(aml.length)switch(*target->string.initial)
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
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_REF_OF_OP:
	case AML_BYTE_ROOT_CHAR:
		target->component.target.super_name = analyse_aml_super_name(target, aml);
		target->string.length += target->component.target.super_name->string.length;
		aml.initial += target->component.target.super_name->string.length;
		aml.length -= target->component.target.super_name->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			target->component.target.super_name = analyse_aml_super_name(target, aml);
			target->string.length += target->component.target.super_name->string.length;
			aml.initial += target->component.target.super_name->string.length;
			aml.length -= target->component.target.super_name->string.length;
		}
		else
		{
			// UserTermObj
			target->component.target.super_name = analyse_aml_super_name(target, aml);
			target->string.length += target->component.target.super_name->string.length;
			aml.initial += target->component.target.super_name->string.length;
			aml.length -= target->component.target.super_name->string.length;
		}
		break;
	}
	else
	{
		target->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	return target;
}

// <term_arg> := <expression_opcode> | <data_object> | <arg_obj> | <local_obj>
AMLSymbol *analyse_aml_term_arg(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("term_arg aml.length = %#010.8x\n", aml.length);
	AMLSymbol *term_arg = malloc(sizeof(*term_arg));
	term_arg->parent = parent;
	term_arg->string.initial = aml.initial;
	term_arg->string.length = 0;
	term_arg->type = aml_term_arg;
	term_arg->flags = 0;
	term_arg->component.term_arg.expression_opcode = NULL;
	term_arg->component.term_arg.data_object = NULL;
	term_arg->component.term_arg.arg_obj = NULL;
	term_arg->component.term_arg.local_obj = NULL;
	if(aml.length)switch(*aml.initial)
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
	case AML_BYTE_ADD_OP:
	case AML_BYTE_AND_OP:
	case AML_BYTE_CONCAT_OP:
	case AML_BYTE_CONCAT_RES_OP:
	case AML_BYTE_DECREMENT_OP:
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_DIVIDE_OP:
	case AML_BYTE_FIND_SET_LEFT_BIT_OP:
	case AML_BYTE_FIND_SET_RIGHT_BIT_OP:
	case AML_BYTE_INCREMENT_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_L_AND_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_NOT_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_MATCH_OP:
	case AML_BYTE_MID_OP:
	case AML_BYTE_MULTIPLY_OP:
	case AML_BYTE_NOT_OP:
	case AML_BYTE_OBJECT_TYPE_OP:
	case AML_BYTE_OR_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
	case AML_BYTE_TO_INTEGER_OP:
	case AML_BYTE_XOR_OP:
		term_arg->component.term_arg.expression_opcode = analyse_aml_expression_opcode(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.expression_opcode->string.length;
		break;
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		term_arg->component.term_arg.data_object = analyse_aml_data_object(term_arg, aml);
		term_arg->string.length += term_arg->component.term_arg.data_object->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_COND_REF_OF_OP:
			term_arg->component.term_arg.expression_opcode = analyse_aml_expression_opcode(term_arg, aml);
			term_arg->string.length += term_arg->component.term_arg.expression_opcode->string.length;
			break;
		case AML_BYTE_REVISION_OP:
			term_arg->component.term_arg.data_object = analyse_aml_data_object(term_arg, aml);
			term_arg->string.length += term_arg->component.term_arg.data_object->string.length;
			break;
		default:
			term_arg->flags |= AML_SYMBOL_ERROR;
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
			break;
		}
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
			term_arg->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	else
	{
		term_arg->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	return term_arg;
}

// <term_arg_list> := Nothing | <term_arg> <term_arg_list>
AMLSymbol *analyse_aml_term_arg_list(AMLSymbol *parent, AMLSubstring aml, int num_of_term_args)
{
	printf_serial("term_arg_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *term_arg_list = malloc(sizeof(*term_arg_list));
	term_arg_list->parent = parent;
	term_arg_list->string.initial = aml.initial;
	term_arg_list->string.length = 0;
	term_arg_list->type = aml_term_arg_list;
	term_arg_list->flags = 0;
	term_arg_list->component.term_arg_list.term_arg = NULL;
	term_arg_list->component.term_arg_list.term_arg_list = NULL;
	if(num_of_term_args)switch(*aml.initial)
	{
	case AML_BYTE_ADD_OP:
	case AML_BYTE_AND_OP:
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_CONCAT_OP:
	case AML_BYTE_CONCAT_RES_OP:
	case AML_BYTE_DECREMENT_OP:
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_DIVIDE_OP:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_FIND_SET_LEFT_BIT_OP:
	case AML_BYTE_FIND_SET_RIGHT_BIT_OP:
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
	case AML_BYTE_L_AND_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_NOT_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_MATCH_OP:
	case AML_BYTE_MID_OP:
	case AML_BYTE_MULTIPLY_OP:
	case AML_BYTE_NOT_OP:
	case AML_BYTE_OBJECT_TYPE_OP:
	case AML_BYTE_OR_OP:
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
	case AML_BYTE_TO_INTEGER_OP:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_XOR_OP:
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
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			term_arg_list->component.term_arg_list.term_arg = analyse_aml_term_arg(term_arg_list, aml);
			term_arg_list->string.length += term_arg_list->component.term_arg_list.term_arg->string.length;
			aml.initial += term_arg_list->component.term_arg_list.term_arg->string.length;
			aml.length -= term_arg_list->component.term_arg_list.term_arg->string.length;
			term_arg_list->component.term_arg_list.term_arg_list = analyse_aml_term_arg_list(term_arg_list, aml, num_of_term_args - 1);
			term_arg_list->string.length += term_arg_list->component.term_arg_list.term_arg_list->string.length;
			aml.initial += term_arg_list->component.term_arg_list.term_arg_list->string.length;
			aml.length -= term_arg_list->component.term_arg_list.term_arg_list->string.length;
		}
		// term_arg_list can be nothing.
		break;
	}
	return term_arg_list;
}

// <term_list> := Nothing | <term_obj> <term_list>
AMLSymbol *analyse_aml_term_list(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("term_list aml.length = %#010.8x\n", aml.length);
	AMLSymbol *term_list = malloc(sizeof(*term_list));
	term_list->parent = parent;
	term_list->string.initial = aml.initial;
	term_list->string.length = 0;
	term_list->type = aml_term_list;
	term_list->flags = 0;
	term_list->component.term_list.term_list = NULL;
	term_list->component.term_list.term_obj = NULL;
	if(!aml.length)return term_list;
	// <term_list> := <term_obj> <term_list>
	term_list->component.term_list.term_obj = analyse_aml_term_obj(term_list, aml);
	term_list->string.length += term_list->component.term_list.term_obj->string.length;
	aml.initial += term_list->component.term_list.term_obj->string.length;
	aml.length -= term_list->component.term_list.term_obj->string.length;
	if(!aml.length || !term_list->component.term_list.term_obj->string.length)return term_list;
	// Analyse next term_list
	switch(*aml.initial)
	{
	case AML_BYTE_ADD_OP:
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_AND_OP:
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_BREAK_OP:
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_CONCAT_OP:
	case AML_BYTE_CONCAT_RES_OP:
	case AML_BYTE_CREATE_BIT_FIELD_OP:
	case AML_BYTE_CREATE_BYTE_FIELD_OP:
	case AML_BYTE_CREATE_DWORD_FIELD_OP:
	case AML_BYTE_CREATE_QWORD_FIELD_OP:
	case AML_BYTE_CREATE_WORD_FIELD_OP:
	case AML_BYTE_DECREMENT_OP:
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_DIVIDE_OP:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_EXT_OP_PREFIX:
	case AML_BYTE_FIND_SET_LEFT_BIT_OP:
	case AML_BYTE_FIND_SET_RIGHT_BIT_OP:
	case AML_BYTE_IF_OP:
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
	case AML_BYTE_L_AND_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_NOT_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_MATCH_OP:
	case AML_BYTE_METHOD_OP:
	case AML_BYTE_MID_OP:
	case AML_BYTE_MULTIPLY_OP:
	case AML_BYTE_NAME_OP:
	case AML_BYTE_NOTIFY_OP:
	case AML_BYTE_NOT_OP:
	case AML_BYTE_OBJECT_TYPE_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_OR_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_RETURN_OP:
	case AML_BYTE_ROOT_CHAR:
	case AML_BYTE_SCOPE_OP:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
	case AML_BYTE_TO_INTEGER_OP:
	case AML_BYTE_WHILE_OP:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_XOR_OP:
	case AML_BYTE_ZERO_OP:
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
			term_list->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return term_list;
}

// <term_obj> := <object> | <statement_opcode> | <expression_opcode>
AMLSymbol *analyse_aml_term_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("term_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *term_obj = malloc(sizeof(*term_obj));
	term_obj->parent = parent;
	term_obj->string.initial = aml.initial;
	term_obj->string.length = 0;
	term_obj->type = aml_term_obj;
	term_obj->flags = 0;
	term_obj->component.term_obj.object = NULL;
	term_obj->component.term_obj.statement_opcode = NULL;
	term_obj->component.term_obj.expression_opcode = NULL;
	term_obj->component.term_obj.wrong_term_arg = NULL;
	// Analyse term_obj
	switch(*aml.initial)
	{
	case AML_BYTE_ADD_OP:
	case AML_BYTE_AND_OP:
	case AML_BYTE_CONCAT_OP:
	case AML_BYTE_CONCAT_RES_OP:
	case AML_BYTE_DECREMENT_OP:
	case AML_BYTE_DEREF_OF_OP:
	case AML_BYTE_DIVIDE_OP:
	case AML_BYTE_FIND_SET_LEFT_BIT_OP:
	case AML_BYTE_FIND_SET_RIGHT_BIT_OP:
	case AML_BYTE_INCREMENT_OP:
	case AML_BYTE_INDEX_OP:
	case AML_BYTE_L_AND_OP:
	case AML_BYTE_L_EQUAL_OP:
	case AML_BYTE_L_GREATER_OP:
	case AML_BYTE_L_LESS_OP:
	case AML_BYTE_L_NOT_OP:
	case AML_BYTE_L_OR_OP:
	case AML_BYTE_MATCH_OP:
	case AML_BYTE_MID_OP:
	case AML_BYTE_MULTIPLY_OP:
	case AML_BYTE_NOT_OP:
	case AML_BYTE_OBJECT_TYPE_OP:
	case AML_BYTE_OR_OP:
	case AML_BYTE_PACKAGE_OP:
	case AML_BYTE_PARENT_PREFIX_CHAR:
	case AML_BYTE_ROOT_CHAR:
	case AML_BYTE_SHIFT_LEFT_OP:
	case AML_BYTE_SHIFT_RIGHT_OP:
	case AML_BYTE_SIZE_OF_OP:
	case AML_BYTE_STORE_OP:
	case AML_BYTE_SUBTRACT_OP:
	case AML_BYTE_TO_BUFFER_OP:
	case AML_BYTE_TO_HEX_STRING_OP:
	case AML_BYTE_TO_INTEGER_OP:
	case AML_BYTE_XOR_OP:
		term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
		break;
	case AML_BYTE_ALIAS_OP:
	case AML_BYTE_CREATE_BIT_FIELD_OP:
	case AML_BYTE_CREATE_BYTE_FIELD_OP:
	case AML_BYTE_CREATE_DWORD_FIELD_OP:
	case AML_BYTE_CREATE_QWORD_FIELD_OP:
	case AML_BYTE_CREATE_WORD_FIELD_OP:
	case AML_BYTE_METHOD_OP:
	case AML_BYTE_NAME_OP:
	case AML_BYTE_SCOPE_OP:
		term_obj->component.term_obj.object = analyse_aml_object(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.object->string.length;
		break;
	case AML_BYTE_BREAK_OP:
	case AML_BYTE_IF_OP:
	case AML_BYTE_NOTIFY_OP:
	case AML_BYTE_RETURN_OP:
	case AML_BYTE_WHILE_OP:
		term_obj->component.term_obj.statement_opcode = analyse_aml_statement_opcode(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.statement_opcode->string.length;
		break;
	case AML_BYTE_EXT_OP_PREFIX:
		switch(aml.initial[1])
		{
		case AML_BYTE_ACQUIRE_OP:
		case AML_BYTE_COND_REF_OF_OP:
			term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
			break;
		case AML_BYTE_RELEASE_OP:
		case AML_BYTE_SLEEP_OP:
		case AML_BYTE_STALL_OP:
			term_obj->component.term_obj.statement_opcode = analyse_aml_statement_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.statement_opcode->string.length;
			break;
		default:
			term_obj->component.term_obj.object = analyse_aml_object(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.object->string.length;
			break;
		}
		break;
	case AML_BYTE_ARG_0_OP:
	case AML_BYTE_ARG_1_OP:
	case AML_BYTE_ARG_2_OP:
	case AML_BYTE_ARG_3_OP:
	case AML_BYTE_ARG_4_OP:
	case AML_BYTE_ARG_5_OP:
	case AML_BYTE_ARG_6_OP:
	case AML_BYTE_BUFFER_OP:
	case AML_BYTE_BYTE_PREFIX:
	case AML_BYTE_DWORD_PREFIX:
	case AML_BYTE_LOCAL_0_OP:
	case AML_BYTE_LOCAL_1_OP:
	case AML_BYTE_LOCAL_2_OP:
	case AML_BYTE_LOCAL_3_OP:
	case AML_BYTE_LOCAL_4_OP:
	case AML_BYTE_LOCAL_5_OP:
	case AML_BYTE_LOCAL_6_OP:
	case AML_BYTE_LOCAL_7_OP:
	case AML_BYTE_ONES_OP:
	case AML_BYTE_ONE_OP:
	case AML_BYTE_QWORD_PREFIX:
	case AML_BYTE_STRING_PREFIX:
	case AML_BYTE_WORD_PREFIX:
	case AML_BYTE_ZERO_OP:
		term_obj->component.term_obj.wrong_term_arg = analyse_aml_term_arg(term_obj, aml);
		term_obj->string.length += term_obj->component.term_obj.wrong_term_arg->string.length;
		break;
	default:
		if(('A' <= *aml.initial && *aml.initial <= 'Z') || *aml.initial == '_')
		{
			term_obj->component.term_obj.expression_opcode = analyse_aml_expression_opcode(term_obj, aml);
			term_obj->string.length += term_obj->component.term_obj.expression_opcode->string.length;
		}
		else
		{
			term_obj->flags |= AML_SYMBOL_ERROR; // Syntax error or unimplemented pattern
			ERROR();
			printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
		}
		break;
	}
	return term_obj;
}

// <thermal_zone_op> := <ext_op_prefix> <thermal_zone_op_suffix>
AMLSymbol *analyse_aml_thermal_zone_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("thermal_zone_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *thermal_zone_op = malloc(sizeof(*thermal_zone_op));
	thermal_zone_op->parent = parent;
	thermal_zone_op->string.initial = aml.initial;
	thermal_zone_op->string.length = 0;
	thermal_zone_op->type = aml_thermal_zone_op;
	thermal_zone_op->flags = 0;
	thermal_zone_op->component.thermal_zone_op.ext_op_prefix = analyse_aml_ext_op_prefix(thermal_zone_op, aml);
	thermal_zone_op->string.length += thermal_zone_op->component.thermal_zone_op.ext_op_prefix->string.length;
	aml.initial += thermal_zone_op->component.thermal_zone_op.ext_op_prefix->string.length;
	aml.length -= thermal_zone_op->component.thermal_zone_op.ext_op_prefix->string.length;
	thermal_zone_op->component.thermal_zone_op.thermal_zone_op_suffix = analyse_aml_thermal_zone_op_suffix(thermal_zone_op, aml);
	thermal_zone_op->string.length += thermal_zone_op->component.thermal_zone_op.thermal_zone_op_suffix->string.length;
	aml.initial += thermal_zone_op->component.thermal_zone_op.thermal_zone_op_suffix->string.length;
	aml.length -= thermal_zone_op->component.thermal_zone_op.thermal_zone_op_suffix->string.length;
	return thermal_zone_op;
}

// <thermal_zone_op_suffix> := AML_BYTE_THERMAL_ZONE_OP
AMLSymbol *analyse_aml_thermal_zone_op_suffix(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("thermal_zone_op_suffix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *thermal_zone_op_suffix = malloc(sizeof(*thermal_zone_op_suffix));
	thermal_zone_op_suffix->parent = parent;
	thermal_zone_op_suffix->string.initial = aml.initial;
	thermal_zone_op_suffix->string.length = 1;
	thermal_zone_op_suffix->type = aml_thermal_zone_op_suffix;
	thermal_zone_op_suffix->flags = 0;
	if(!aml.length)
	{
		thermal_zone_op_suffix->string.length = 0;
		thermal_zone_op_suffix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_THERMAL_ZONE_OP)
	{
		thermal_zone_op_suffix->flags |= AML_SYMBOL_ERROR; // Incorrect thermal_zone_op_suffix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return thermal_zone_op_suffix;
}

// <time_out> := <word_data>
AMLSymbol *analyse_aml_time_out(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("time_out aml.length = %#010.8x\n", aml.length);
	AMLSymbol *time_out = malloc(sizeof(*time_out));
	time_out->parent = parent;
	time_out->string.initial = aml.initial;
	time_out->string.length = 0;
	time_out->type = aml_time_out;
	time_out->flags = 0;
	time_out->component.time_out.word_data = analyse_aml_word_data(time_out, aml);
	time_out->string.length += time_out->component.time_out.word_data->string.length;
	return time_out;
}

// <to_buffer_op> := AML_BYTE_TO_BUFFER_OP
AMLSymbol *analyse_aml_to_buffer_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("to_buffer_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *to_buffer_op = malloc(sizeof(*to_buffer_op));
	to_buffer_op->parent = parent;
	to_buffer_op->string.initial = aml.initial;
	to_buffer_op->string.length = 1;
	to_buffer_op->type = aml_to_buffer_op;
	to_buffer_op->flags = 0;
	if(!aml.length)
	{
		to_buffer_op->string.length = 0;
		to_buffer_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*to_buffer_op->string.initial != AML_BYTE_TO_BUFFER_OP)
	{
		to_buffer_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return to_buffer_op;
}

// <to_hex_string_op> := AML_BYTE_TO_HEX_STRING_OP
AMLSymbol *analyse_aml_to_hex_string_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("to_hex_string_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *to_hex_string_op = malloc(sizeof(*to_hex_string_op));
	to_hex_string_op->parent = parent;
	to_hex_string_op->string.initial = aml.initial;
	to_hex_string_op->string.length = 1;
	to_hex_string_op->type = aml_to_hex_string_op;
	to_hex_string_op->flags = 0;
	if(!aml.length)
	{
		to_hex_string_op->string.length = 0;
		to_hex_string_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*to_hex_string_op->string.initial != AML_BYTE_TO_HEX_STRING_OP)
	{
		to_hex_string_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return to_hex_string_op;
}

// <to_integer_op> := AML_BYTE_TO_INTEGER_OP
AMLSymbol *analyse_aml_to_integer_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("to_integer_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *to_integer_op = malloc(sizeof(*to_integer_op));
	to_integer_op->parent = parent;
	to_integer_op->string.initial = aml.initial;
	to_integer_op->string.length = 1;
	to_integer_op->type = aml_to_integer_op;
	to_integer_op->flags = 0;
	if(!aml.length)
	{
		to_integer_op->string.length = 0;
		to_integer_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*to_integer_op->string.initial != AML_BYTE_TO_INTEGER_OP)
	{
		to_integer_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return to_integer_op;
}

// <usec_time> := <term_arg>
AMLSymbol *analyse_aml_usec_time(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("usec_time aml.length = %#010.8x\n", aml.length);
	AMLSymbol *usec_time = malloc(sizeof(*usec_time));
	usec_time->parent = parent;
	usec_time->string.initial = aml.initial;
	usec_time->string.length = 0;
	usec_time->type = aml_usec_time;
	usec_time->flags = 0;
	usec_time->component.usec_time.term_arg = analyse_aml_term_arg(usec_time, aml);
	usec_time->string.length += usec_time->component.usec_time.term_arg->string.length;
	return usec_time;
}

// <user_term_obj> := <byte_data>
AMLSymbol *analyse_aml_user_term_obj(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("user_term_obj aml.length = %#010.8x\n", aml.length);
	AMLSymbol *user_term_obj = malloc(sizeof(*user_term_obj));
	user_term_obj->parent = parent;
	user_term_obj->string.initial = aml.initial;
	user_term_obj->string.length = 0;
	user_term_obj->type = aml_user_term_obj;
	user_term_obj->flags = 0;
	return user_term_obj;
}

// <while_op> := AML_BYTE_WHILE_OP
AMLSymbol *analyse_aml_while_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("while_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *while_op = malloc(sizeof(*while_op));
	while_op->parent = parent;
	while_op->string.initial = aml.initial;
	while_op->string.length = 1;
	while_op->type = aml_while_op;
	while_op->flags = 0;
	if(!aml.length)
	{
		while_op->string.length = 0;
		while_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*while_op->string.initial != AML_BYTE_WHILE_OP)
	{
		while_op->flags |= AML_SYMBOL_ERROR; // Incorrect while op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return while_op;
}

// <word_const> := <word_prefix> <word_data>
AMLSymbol *analyse_aml_word_const(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("word_const aml.length = %#010.8x\n", aml.length);
	AMLSymbol *word_const = malloc(sizeof(*word_const));
	word_const->parent = parent;
	word_const->string.initial = aml.initial;
	word_const->string.length = 0;
	word_const->type = aml_word_const;
	word_const->flags = 0;
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
	printf_serial("word_data aml.length = %#010.8x\n", aml.length);
	AMLSymbol *word_data = malloc(sizeof(*word_data));
	word_data->parent = parent;
	word_data->string.initial = aml.initial;
	word_data->string.length = 0;
	word_data->type = aml_word_data;
	word_data->flags = 0;
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
	printf_serial("word_prefix aml.length = %#010.8x\n", aml.length);
	AMLSymbol *word_prefix = malloc(sizeof(*word_prefix));
	word_prefix->parent = parent;
	word_prefix->string.initial = aml.initial;
	word_prefix->string.length = 1;
	word_prefix->type = aml_word_prefix;
	word_prefix->flags = 0;
	if(!aml.length)
	{
		word_prefix->string.length = 0;
		word_prefix->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*aml.initial != AML_BYTE_WORD_PREFIX)
	{
		word_prefix->flags |= AML_SYMBOL_ERROR; // Incorrect word prefix
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return word_prefix;
}

// <xorl_op> := AML_BYTE_XOR_OP
AMLSymbol *analyse_aml_xor_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("xor_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *xor_op = malloc(sizeof(*xor_op));
	xor_op->parent = parent;
	xor_op->parent = parent;
	xor_op->string.initial = aml.initial;
	xor_op->string.length = 1;
	xor_op->type = aml_xor_op;
	xor_op->flags = 0;
	if(!aml.length)
	{
		xor_op->string.length = 0;
		xor_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*xor_op->string.initial != AML_BYTE_XOR_OP)
	{
		xor_op->flags |= AML_SYMBOL_ERROR; // Incorrect xor op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
	return xor_op;
}

// <zero_op> := AML_BYTE_ZERO_OP
AMLSymbol *analyse_aml_zero_op(AMLSymbol *parent, AMLSubstring aml)
{
	printf_serial("zero_op aml.length = %#010.8x\n", aml.length);
	AMLSymbol *zero_op = malloc(sizeof(*zero_op));
	zero_op->parent = parent;
	zero_op->parent = parent;
	zero_op->string.initial = aml.initial;
	zero_op->string.length = 1;
	zero_op->type = aml_zero_op;
	zero_op->flags = 0;
	if(!aml.length)
	{
		zero_op->string.length = 0;
		zero_op->flags |= AML_SYMBOL_ERROR;
		ERROR();
	}
	else if(*zero_op->string.initial != AML_BYTE_ZERO_OP)
	{
		zero_op->flags |= AML_SYMBOL_ERROR; // Incorrect zero op
		ERROR();
		printf_serial("*aml.initial = %#04.2x\n", *aml.initial);
	}
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
	case aml_access_attrib:
		if(aml_symbol->component.access_attrib.byte_data)delete_aml_symbol(aml_symbol->component.access_attrib.byte_data);
		break;
	case aml_access_field:
		if(aml_symbol->component.access_field.access_field_op)delete_aml_symbol(aml_symbol->component.access_field.access_field_op);
		if(aml_symbol->component.access_field.access_type)delete_aml_symbol(aml_symbol->component.access_field.access_type);
		if(aml_symbol->component.access_field.access_attrib)delete_aml_symbol(aml_symbol->component.access_field.access_attrib);
		break;
	case aml_access_field_op:
		break;
	case aml_access_type:
		if(aml_symbol->component.access_type.byte_data)delete_aml_symbol(aml_symbol->component.access_type.byte_data);
		break;
	case aml_acquire_op:
		if(aml_symbol->component.acquire_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.acquire_op.ext_op_prefix);
		if(aml_symbol->component.acquire_op.acquire_op_suffix)delete_aml_symbol(aml_symbol->component.acquire_op.acquire_op_suffix);
		break;
	case aml_acquire_op_suffix:
		break;
	case aml_alias_op:
		break;
	case aml_add_op:
		break;
	case aml_and_op:
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
	case aml_bit_index:
		if(aml_symbol->component.bit_index.term_arg)delete_aml_symbol(aml_symbol->component.bit_index.term_arg);
		break;
	case aml_break_op:
		break;
	case aml_buf_data:
		if(aml_symbol->component.buf_data.term_arg)delete_aml_symbol(aml_symbol->component.buf_data.term_arg);
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
	case aml_byte_index:
		if(aml_symbol->component.byte_index.term_arg)delete_aml_symbol(aml_symbol->component.byte_index.term_arg);
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
	case aml_concat_op:
		break;
	case aml_concat_res_op:
		break;
	case aml_cond_ref_of_op:
		if(aml_symbol->component.cond_ref_of_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.cond_ref_of_op.ext_op_prefix);
		if(aml_symbol->component.cond_ref_of_op.cond_ref_of_op_suffix)delete_aml_symbol(aml_symbol->component.cond_ref_of_op.cond_ref_of_op_suffix);
		break;
	case aml_cond_ref_of_op_suffix:
		break;
	case aml_const_obj:
		if(aml_symbol->component.const_obj.zero_op)delete_aml_symbol(aml_symbol->component.const_obj.zero_op);
		if(aml_symbol->component.const_obj.one_op)delete_aml_symbol(aml_symbol->component.const_obj.one_op);
		if(aml_symbol->component.const_obj.ones_op)delete_aml_symbol(aml_symbol->component.const_obj.ones_op);
		break;
	case aml_create_bit_field_op:
		break;
	case aml_create_byte_field_op:
		break;
	case aml_create_dword_field_op:
		break;
	case aml_create_field_op:
		if(aml_symbol->component.create_field_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.create_field_op.ext_op_prefix);
		if(aml_symbol->component.create_field_op.create_field_op_suffix)delete_aml_symbol(aml_symbol->component.create_field_op.create_field_op_suffix);
		break;
	case aml_create_field_op_suffix:
		break;
	case aml_create_qword_field_op:
		break;
	case aml_create_word_field_op:
		break;
	case aml_data:
		if(aml_symbol->component.data.term_arg)delete_aml_symbol(aml_symbol->component.data.term_arg);
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
	case aml_debug_obj:
		if(aml_symbol->component.debug_obj.debug_op)delete_aml_symbol(aml_symbol->component.debug_obj.debug_op);
		break;
	case aml_debug_op:
		if(aml_symbol->component.debug_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.debug_op.ext_op_prefix);
		if(aml_symbol->component.debug_op.debug_op_suffix)delete_aml_symbol(aml_symbol->component.debug_op.debug_op_suffix);
		break;
	case aml_debug_op_suffix:
		break;
	case aml_decrement_op:
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
	case aml_def_add:
		if(aml_symbol->component.def_or.or_op)delete_aml_symbol(aml_symbol->component.def_or.or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_or.operand); i++)if(aml_symbol->component.def_or.operand[i])delete_aml_symbol(aml_symbol->component.def_or.operand[i]);
		if(aml_symbol->component.def_or.target)delete_aml_symbol(aml_symbol->component.def_or.target);
		break;
	case aml_def_and:
		if(aml_symbol->component.def_and.and_op)delete_aml_symbol(aml_symbol->component.def_and.and_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_and.operand); i++)if(aml_symbol->component.def_and.operand[i])delete_aml_symbol(aml_symbol->component.def_and.operand[i]);
		if(aml_symbol->component.def_and.target)delete_aml_symbol(aml_symbol->component.def_and.target);
		break;
	case aml_def_break:
		if(aml_symbol->component.def_break.break_op)delete_aml_symbol(aml_symbol->component.def_break.break_op);
		break;
	case aml_def_buffer:
		if(aml_symbol->component.def_buffer.buffer_op)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_op);
		if(aml_symbol->component.def_buffer.pkg_length)delete_aml_symbol(aml_symbol->component.def_buffer.pkg_length);
		if(aml_symbol->component.def_buffer.buffer_size)delete_aml_symbol(aml_symbol->component.def_buffer.buffer_size);
		if(aml_symbol->component.def_buffer.byte_list)delete_aml_symbol(aml_symbol->component.def_buffer.byte_list);
		break;
	case aml_def_concat:
		if(aml_symbol->component.def_concat.concat_op)delete_aml_symbol(aml_symbol->component.def_concat.concat_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_concat.data); i++)if(aml_symbol->component.def_concat.data[i])delete_aml_symbol(aml_symbol->component.def_concat.data[i]);
		if(aml_symbol->component.def_concat.target)delete_aml_symbol(aml_symbol->component.def_concat.target);
		break;
	case aml_def_concat_res:
		if(aml_symbol->component.def_concat_res.concat_res_op)delete_aml_symbol(aml_symbol->component.def_concat_res.concat_res_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_concat_res.buf_data); i++)if(aml_symbol->component.def_concat_res.buf_data[i])delete_aml_symbol(aml_symbol->component.def_concat_res.buf_data[i]);
		if(aml_symbol->component.def_concat_res.target)delete_aml_symbol(aml_symbol->component.def_concat_res.target);
		break;
	case aml_def_cond_ref_of:
		if(aml_symbol->component.def_cond_ref_of.cond_ref_of_op)delete_aml_symbol(aml_symbol->component.def_cond_ref_of.cond_ref_of_op);
		if(aml_symbol->component.def_cond_ref_of.super_name)delete_aml_symbol(aml_symbol->component.def_cond_ref_of.super_name);
		if(aml_symbol->component.def_cond_ref_of.target)delete_aml_symbol(aml_symbol->component.def_cond_ref_of.target);
		break;
	case aml_def_create_bit_field:
		if(aml_symbol->component.def_create_bit_field.create_bit_field_op)delete_aml_symbol(aml_symbol->component.def_create_bit_field.create_bit_field_op);
		if(aml_symbol->component.def_create_bit_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_bit_field.source_buff);
		if(aml_symbol->component.def_create_bit_field.byte_index)delete_aml_symbol(aml_symbol->component.def_create_bit_field.byte_index);
		if(aml_symbol->component.def_create_bit_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_bit_field.name_string);
		break;
	case aml_def_create_byte_field:
		if(aml_symbol->component.def_create_byte_field.create_byte_field_op)delete_aml_symbol(aml_symbol->component.def_create_byte_field.create_byte_field_op);
		if(aml_symbol->component.def_create_byte_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_byte_field.source_buff);
		if(aml_symbol->component.def_create_byte_field.byte_index)delete_aml_symbol(aml_symbol->component.def_create_byte_field.byte_index);
		if(aml_symbol->component.def_create_byte_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_byte_field.name_string);
		break;
	case aml_def_create_dword_field:
		if(aml_symbol->component.def_create_dword_field.create_dword_field_op)delete_aml_symbol(aml_symbol->component.def_create_dword_field.create_dword_field_op);
		if(aml_symbol->component.def_create_dword_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_dword_field.source_buff);
		if(aml_symbol->component.def_create_dword_field.byte_index)delete_aml_symbol(aml_symbol->component.def_create_dword_field.byte_index);
		if(aml_symbol->component.def_create_dword_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_dword_field.name_string);
		break;
	case aml_def_create_field:
		if(aml_symbol->component.def_create_field.create_field_op)delete_aml_symbol(aml_symbol->component.def_create_field.create_field_op);
		if(aml_symbol->component.def_create_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_field.source_buff);
		if(aml_symbol->component.def_create_field.bit_index)delete_aml_symbol(aml_symbol->component.def_create_field.bit_index);
		if(aml_symbol->component.def_create_field.num_bits)delete_aml_symbol(aml_symbol->component.def_create_field.num_bits);
		if(aml_symbol->component.def_create_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_field.name_string);
		break;
	case aml_def_create_qword_field:
		if(aml_symbol->component.def_create_qword_field.create_qword_field_op)delete_aml_symbol(aml_symbol->component.def_create_qword_field.create_qword_field_op);
		if(aml_symbol->component.def_create_qword_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_qword_field.source_buff);
		if(aml_symbol->component.def_create_qword_field.byte_index)delete_aml_symbol(aml_symbol->component.def_create_qword_field.byte_index);
		if(aml_symbol->component.def_create_qword_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_qword_field.name_string);
		break;
	case aml_def_create_word_field:
		if(aml_symbol->component.def_create_word_field.create_word_field_op)delete_aml_symbol(aml_symbol->component.def_create_word_field.create_word_field_op);
		if(aml_symbol->component.def_create_word_field.source_buff)delete_aml_symbol(aml_symbol->component.def_create_word_field.source_buff);
		if(aml_symbol->component.def_create_word_field.byte_index)delete_aml_symbol(aml_symbol->component.def_create_word_field.byte_index);
		if(aml_symbol->component.def_create_word_field.name_string)delete_aml_symbol(aml_symbol->component.def_create_word_field.name_string);
		break;
	case aml_def_decrement:
		if(aml_symbol->component.def_decrement.decrement_op)delete_aml_symbol(aml_symbol->component.def_decrement.decrement_op);
		if(aml_symbol->component.def_decrement.super_name)delete_aml_symbol(aml_symbol->component.def_decrement.super_name);
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
	case aml_def_divide:
		if(aml_symbol->component.def_divide.divide_op)delete_aml_symbol(aml_symbol->component.def_divide.divide_op);
		if(aml_symbol->component.def_divide.dividend)delete_aml_symbol(aml_symbol->component.def_divide.dividend);
		if(aml_symbol->component.def_divide.divisor)delete_aml_symbol(aml_symbol->component.def_divide.divisor);
		if(aml_symbol->component.def_divide.remainder)delete_aml_symbol(aml_symbol->component.def_divide.remainder);
		if(aml_symbol->component.def_divide.quotient)delete_aml_symbol(aml_symbol->component.def_divide.quotient);
		break;
	case aml_def_else:
		if(aml_symbol->component.def_else.else_op)delete_aml_symbol(aml_symbol->component.def_else.else_op);
		if(aml_symbol->component.def_else.pkg_length)delete_aml_symbol(aml_symbol->component.def_else.pkg_length);
		if(aml_symbol->component.def_else.term_list)delete_aml_symbol(aml_symbol->component.def_else.term_list);
		break;
	case aml_def_field:
		if(aml_symbol->component.def_field.field_op)delete_aml_symbol(aml_symbol->component.def_field.field_op);
		if(aml_symbol->component.def_field.pkg_length)delete_aml_symbol(aml_symbol->component.def_field.pkg_length);
		if(aml_symbol->component.def_field.name_string)delete_aml_symbol(aml_symbol->component.def_field.name_string);
		if(aml_symbol->component.def_field.field_flags)delete_aml_symbol(aml_symbol->component.def_field.field_flags);
		if(aml_symbol->component.def_field.field_list)delete_aml_symbol(aml_symbol->component.def_field.field_list);
		break;
	case aml_def_find_set_left_bit:
		if(aml_symbol->component.def_find_set_left_bit.find_set_left_bit_op)delete_aml_symbol(aml_symbol->component.def_find_set_left_bit.find_set_left_bit_op);
		if(aml_symbol->component.def_find_set_left_bit.operand)delete_aml_symbol(aml_symbol->component.def_find_set_left_bit.operand);
		if(aml_symbol->component.def_find_set_left_bit.target)delete_aml_symbol(aml_symbol->component.def_find_set_left_bit.target);
		break;
	case aml_def_find_set_right_bit:
		if(aml_symbol->component.def_find_set_right_bit.find_set_right_bit_op)delete_aml_symbol(aml_symbol->component.def_find_set_right_bit.find_set_right_bit_op);
		if(aml_symbol->component.def_find_set_right_bit.operand)delete_aml_symbol(aml_symbol->component.def_find_set_right_bit.operand);
		if(aml_symbol->component.def_find_set_right_bit.target)delete_aml_symbol(aml_symbol->component.def_find_set_right_bit.target);
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
	case aml_def_index_field:
		if(aml_symbol->component.def_index_field.index_field_op)delete_aml_symbol(aml_symbol->component.def_index_field.index_field_op);
		if(aml_symbol->component.def_index_field.pkg_length)delete_aml_symbol(aml_symbol->component.def_index_field.pkg_length);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_index_field.name_string); i++)if(aml_symbol->component.def_index_field.name_string[i])delete_aml_symbol(aml_symbol->component.def_index_field.name_string[i]);
		if(aml_symbol->component.def_index_field.field_flags)delete_aml_symbol(aml_symbol->component.def_index_field.field_flags);
		if(aml_symbol->component.def_index_field.field_list)delete_aml_symbol(aml_symbol->component.def_index_field.field_list);
		break;
	case aml_def_l_and:
		if(aml_symbol->component.def_l_and.l_and_op)delete_aml_symbol(aml_symbol->component.def_l_and.l_and_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_and.operand); i++)if(aml_symbol->component.def_l_and.operand[i])delete_aml_symbol(aml_symbol->component.def_l_and.operand[i]);
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
	case aml_def_l_not:
		if(aml_symbol->component.def_l_not.l_not_op)delete_aml_symbol(aml_symbol->component.def_l_not.l_not_op);
		if(aml_symbol->component.def_l_not.operand)delete_aml_symbol(aml_symbol->component.def_l_not.operand);
		break;
	case aml_def_l_or:
		if(aml_symbol->component.def_l_or.l_or_op)delete_aml_symbol(aml_symbol->component.def_l_or.l_or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)if(aml_symbol->component.def_l_or.operand[i])delete_aml_symbol(aml_symbol->component.def_l_or.operand[i]);
		break;
	case aml_def_match:
		if(aml_symbol->component.def_match.match_op)delete_aml_symbol(aml_symbol->component.def_match.match_op);
		if(aml_symbol->component.def_match.search_pkg)delete_aml_symbol(aml_symbol->component.def_match.search_pkg);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_match.match_opcode); i++)if(aml_symbol->component.def_match.match_opcode)delete_aml_symbol(aml_symbol->component.def_match.match_opcode[i]);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_match.operand); i++)if(aml_symbol->component.def_match.operand)delete_aml_symbol(aml_symbol->component.def_match.operand[i]);
		if(aml_symbol->component.def_match.start_index)delete_aml_symbol(aml_symbol->component.def_match.start_index);
		break;
	case aml_def_method:
		if(aml_symbol->component.def_method.method_op)delete_aml_symbol(aml_symbol->component.def_method.method_op);
		if(aml_symbol->component.def_method.pkg_length)delete_aml_symbol(aml_symbol->component.def_method.pkg_length);
		if(aml_symbol->component.def_method.name_string)delete_aml_symbol(aml_symbol->component.def_method.name_string);
		if(aml_symbol->component.def_method.method_flags)delete_aml_symbol(aml_symbol->component.def_method.method_flags);
		if(aml_symbol->component.def_method.term_list)delete_aml_symbol(aml_symbol->component.def_method.term_list);
		break;
	case aml_def_mid:
		if(aml_symbol->component.def_mid.mid_op)delete_aml_symbol(aml_symbol->component.def_mid.mid_op);
		if(aml_symbol->component.def_mid.mid_obj)delete_aml_symbol(aml_symbol->component.def_mid.mid_obj);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_mid.term_arg); i++)if(aml_symbol->component.def_mid.term_arg[i])delete_aml_symbol(aml_symbol->component.def_mid.term_arg[i]);
		if(aml_symbol->component.def_mid.target)delete_aml_symbol(aml_symbol->component.def_mid.target);
		break;
	case aml_def_multiply:
		if(aml_symbol->component.def_multiply.multiply_op)delete_aml_symbol(aml_symbol->component.def_multiply.multiply_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_multiply.operand); i++)if(aml_symbol->component.def_multiply.operand[i])delete_aml_symbol(aml_symbol->component.def_multiply.operand[i]);
		if(aml_symbol->component.def_multiply.target)delete_aml_symbol(aml_symbol->component.def_multiply.target);
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
	case aml_def_not:
		if(aml_symbol->component.def_not.not_op)delete_aml_symbol(aml_symbol->component.def_not.not_op);
		if(aml_symbol->component.def_not.operand)delete_aml_symbol(aml_symbol->component.def_not.operand);
		if(aml_symbol->component.def_not.target)delete_aml_symbol(aml_symbol->component.def_not.target);
		break;
	case aml_def_notify:
		if(aml_symbol->component.def_notify.notify_op)delete_aml_symbol(aml_symbol->component.def_notify.notify_op);
		if(aml_symbol->component.def_notify.notify_object)delete_aml_symbol(aml_symbol->component.def_notify.notify_object);
		if(aml_symbol->component.def_notify.notify_value)delete_aml_symbol(aml_symbol->component.def_notify.notify_value);
		break;
	case aml_def_object_type:
		if(aml_symbol->component.def_object_type.object_type_op)delete_aml_symbol(aml_symbol->component.def_object_type.object_type_op);
		if(aml_symbol->component.def_object_type.simple_name)delete_aml_symbol(aml_symbol->component.def_object_type.simple_name);
		if(aml_symbol->component.def_object_type.debug_obj)delete_aml_symbol(aml_symbol->component.def_object_type.debug_obj);
		if(aml_symbol->component.def_object_type.def_ref_of)delete_aml_symbol(aml_symbol->component.def_object_type.def_ref_of);
		if(aml_symbol->component.def_object_type.def_deref_of)delete_aml_symbol(aml_symbol->component.def_object_type.def_deref_of);
		if(aml_symbol->component.def_object_type.def_index)delete_aml_symbol(aml_symbol->component.def_object_type.def_index);
		break;
	case aml_def_op_region:
		if(aml_symbol->component.def_op_region.op_region_op)delete_aml_symbol(aml_symbol->component.def_op_region.op_region_op);
		if(aml_symbol->component.def_op_region.name_string)delete_aml_symbol(aml_symbol->component.def_op_region.name_string);
		if(aml_symbol->component.def_op_region.region_space)delete_aml_symbol(aml_symbol->component.def_op_region.region_space);
		if(aml_symbol->component.def_op_region.region_offset)delete_aml_symbol(aml_symbol->component.def_op_region.region_offset);
		if(aml_symbol->component.def_op_region.region_len)delete_aml_symbol(aml_symbol->component.def_op_region.region_len);
		break;
	case aml_def_or:
		if(aml_symbol->component.def_or.or_op)delete_aml_symbol(aml_symbol->component.def_or.or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_or.operand); i++)if(aml_symbol->component.def_or.operand[i])delete_aml_symbol(aml_symbol->component.def_or.operand[i]);
		if(aml_symbol->component.def_or.target)delete_aml_symbol(aml_symbol->component.def_or.target);
		break;
	case aml_def_package:
		if(aml_symbol->component.def_package.package_op)delete_aml_symbol(aml_symbol->component.def_package.package_op);
		if(aml_symbol->component.def_package.pkg_length)delete_aml_symbol(aml_symbol->component.def_package.pkg_length);
		if(aml_symbol->component.def_package.num_elements)delete_aml_symbol(aml_symbol->component.def_package.num_elements);
		if(aml_symbol->component.def_package.package_element_list)delete_aml_symbol(aml_symbol->component.def_package.package_element_list);
		break;
	case aml_def_processor:
		if(aml_symbol->component.def_processor.processor_op)delete_aml_symbol(aml_symbol->component.def_processor.processor_op);
		if(aml_symbol->component.def_processor.pkg_length)delete_aml_symbol(aml_symbol->component.def_processor.pkg_length);
		if(aml_symbol->component.def_processor.name_string)delete_aml_symbol(aml_symbol->component.def_processor.name_string);
		if(aml_symbol->component.def_processor.proc_id)delete_aml_symbol(aml_symbol->component.def_processor.proc_id);
		if(aml_symbol->component.def_processor.pblk_addr)delete_aml_symbol(aml_symbol->component.def_processor.pblk_addr);
		if(aml_symbol->component.def_processor.pblk_len)delete_aml_symbol(aml_symbol->component.def_processor.pblk_len);
		if(aml_symbol->component.def_processor.term_list)delete_aml_symbol(aml_symbol->component.def_processor.term_list);
		break;
	case aml_def_ref_of:
		if(aml_symbol->component.def_ref_of.ref_of_op)delete_aml_symbol(aml_symbol->component.def_ref_of.ref_of_op);
		if(aml_symbol->component.def_ref_of.super_name)delete_aml_symbol(aml_symbol->component.def_ref_of.super_name);
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
	case aml_def_sleep:
		if(aml_symbol->component.def_sleep.sleep_op)delete_aml_symbol(aml_symbol->component.def_sleep.sleep_op);
		if(aml_symbol->component.def_sleep.msec_time)delete_aml_symbol(aml_symbol->component.def_sleep.msec_time);
		break;
	case aml_def_stall:
		if(aml_symbol->component.def_stall.stall_op)delete_aml_symbol(aml_symbol->component.def_stall.stall_op);
		if(aml_symbol->component.def_stall.usec_time)delete_aml_symbol(aml_symbol->component.def_stall.usec_time);
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
	case aml_def_thermal_zone:
		if(aml_symbol->component.def_thermal_zone.thermal_zone_op)delete_aml_symbol(aml_symbol->component.def_thermal_zone.thermal_zone_op);
		if(aml_symbol->component.def_thermal_zone.pkg_length)delete_aml_symbol(aml_symbol->component.def_thermal_zone.pkg_length);
		if(aml_symbol->component.def_thermal_zone.name_string)delete_aml_symbol(aml_symbol->component.def_thermal_zone.name_string);
		if(aml_symbol->component.def_thermal_zone.term_list)delete_aml_symbol(aml_symbol->component.def_thermal_zone.term_list);
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
	case aml_def_to_integer:
		if(aml_symbol->component.def_to_integer.to_integer_op)delete_aml_symbol(aml_symbol->component.def_to_integer.to_integer_op);
		if(aml_symbol->component.def_to_integer.operand)delete_aml_symbol(aml_symbol->component.def_to_integer.operand);
		if(aml_symbol->component.def_to_integer.target)delete_aml_symbol(aml_symbol->component.def_to_integer.target);
		break;
	case aml_def_while:
		if(aml_symbol->component.def_while.while_op)delete_aml_symbol(aml_symbol->component.def_while.while_op);
		if(aml_symbol->component.def_while.pkg_length)delete_aml_symbol(aml_symbol->component.def_while.pkg_length);
		if(aml_symbol->component.def_while.predicate)delete_aml_symbol(aml_symbol->component.def_while.predicate);
		if(aml_symbol->component.def_while.term_list)delete_aml_symbol(aml_symbol->component.def_while.term_list);
		break;
	case aml_def_xor:
		if(aml_symbol->component.def_xor.xor_op)delete_aml_symbol(aml_symbol->component.def_xor.xor_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_xor.operand); i++)if(aml_symbol->component.def_xor.operand[i])delete_aml_symbol(aml_symbol->component.def_xor.operand[i]);
		if(aml_symbol->component.def_xor.target)delete_aml_symbol(aml_symbol->component.def_xor.target);
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
	case aml_divide_op:
		break;
	case aml_dividend:
		if(aml_symbol->component.dividend.term_arg)delete_aml_symbol(aml_symbol->component.dividend.term_arg);
		break;
	case aml_divisor:
		if(aml_symbol->component.divisor.term_arg)delete_aml_symbol(aml_symbol->component.divisor.term_arg);
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
	case aml_else_op:
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
	case aml_find_set_left_bit_op:
		break;
	case aml_find_set_right_bit_op:
		break;
	case aml_if_op:
		break;
	case aml_increment_op:
		break;
	case aml_index_field_op:
		if(aml_symbol->component.index_field_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.index_field_op.ext_op_prefix);
		if(aml_symbol->component.index_field_op.index_field_op_suffix)delete_aml_symbol(aml_symbol->component.index_field_op.index_field_op_suffix);
		break;
	case aml_index_field_op_suffix:
		break;
	case aml_index_op:
		break;
	case aml_index_value:
		if(aml_symbol->component.index_value.term_arg)delete_aml_symbol(aml_symbol->component.index_value.term_arg);
		break;
	case aml_lead_name_char:
		break;
	case aml_l_and_op:
		break;
	case aml_l_equal_op:
		break;
	case aml_l_greater_op:
		break;
	case aml_l_less_op:
		break;
	case aml_l_not_op:
		break;
	case aml_l_or_op:
		break;
	case aml_local_obj:
		if(aml_symbol->component.local_obj.local_op)delete_aml_symbol(aml_symbol->component.local_obj.local_op);
		break;
	case aml_local_op:
		break;
	case aml_match_op:
		break;
	case aml_match_opcode:
		break;
	case aml_method_flags:
		break;
	case aml_method_invocation:
		if(aml_symbol->component.method_invocation.name_string)delete_aml_symbol(aml_symbol->component.method_invocation.name_string);
		if(aml_symbol->component.method_invocation.term_arg_list)delete_aml_symbol(aml_symbol->component.method_invocation.term_arg_list);
		break;
	case aml_method_op:
		break;
	case aml_mid_obj:
		if(aml_symbol->component.mid_obj.term_arg)delete_aml_symbol(aml_symbol->component.mid_obj.term_arg);
		break;
	case aml_mid_op:
		break;
	case aml_msec_time:
		if(aml_symbol->component.msec_time.term_arg)delete_aml_symbol(aml_symbol->component.msec_time.term_arg);
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
	case aml_multiply_op:
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
		if(aml_symbol->component.named_obj.def_index_field)delete_aml_symbol(aml_symbol->component.named_obj.def_index_field);
		if(aml_symbol->component.named_obj.def_method)delete_aml_symbol(aml_symbol->component.named_obj.def_method);
		if(aml_symbol->component.named_obj.def_mutex)delete_aml_symbol(aml_symbol->component.named_obj.def_mutex);
		if(aml_symbol->component.named_obj.def_op_region)delete_aml_symbol(aml_symbol->component.named_obj.def_op_region);
		if(aml_symbol->component.named_obj.def_power_res)delete_aml_symbol(aml_symbol->component.named_obj.def_power_res);
		if(aml_symbol->component.named_obj.def_processor)delete_aml_symbol(aml_symbol->component.named_obj.def_processor);
		if(aml_symbol->component.named_obj.def_thermal_zone)delete_aml_symbol(aml_symbol->component.named_obj.def_thermal_zone);
		break;
	case aml_not_op:
		break;
	case aml_notify_object:
		if(aml_symbol->component.notify_object.super_name)delete_aml_symbol(aml_symbol->component.notify_object.super_name);
		break;
	case aml_notify_value:
		if(aml_symbol->component.notify_value.term_arg)delete_aml_symbol(aml_symbol->component.notify_value.term_arg);
		break;
	case aml_notify_op:
		break;
	case aml_null_char:
		break;
	case aml_null_name:
		break;
	case aml_num_bits:
		if(aml_symbol->component.num_bits.term_arg)delete_aml_symbol(aml_symbol->component.num_bits.term_arg);
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
	case aml_object_type_op:
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
	case aml_or_op:
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
	case aml_pblk_addr:
		if(aml_symbol->component.pblk_addr.dword_data)delete_aml_symbol(aml_symbol->component.pblk_addr.dword_data);
		break;
	case aml_pblk_len:
		if(aml_symbol->component.pblk_len.byte_data)delete_aml_symbol(aml_symbol->component.pblk_len.byte_data);
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
	case aml_proc_id:
		if(aml_symbol->component.proc_id.byte_data)delete_aml_symbol(aml_symbol->component.proc_id.byte_data);
		break;
	case aml_processor_op:
		if(aml_symbol->component.processor_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.processor_op.ext_op_prefix);
		if(aml_symbol->component.processor_op.processor_op_suffix)delete_aml_symbol(aml_symbol->component.processor_op.processor_op_suffix);
		break;
	case aml_processor_op_suffix:
		break;
	case aml_quotient:
		if(aml_symbol->component.quotient.term_arg)delete_aml_symbol(aml_symbol->component.quotient.term_arg);
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
	case aml_ref_of_op:
		break;
	case aml_reference_type_opcode:
		if(aml_symbol->component.reference_type_opcode.def_ref_of)delete_aml_symbol(aml_symbol->component.reference_type_opcode.def_ref_of);
		if(aml_symbol->component.reference_type_opcode.def_deref_of)delete_aml_symbol(aml_symbol->component.reference_type_opcode.def_deref_of);
		if(aml_symbol->component.reference_type_opcode.def_index)delete_aml_symbol(aml_symbol->component.reference_type_opcode.def_index);
		if(aml_symbol->component.reference_type_opcode.user_term_obj)delete_aml_symbol(aml_symbol->component.reference_type_opcode.user_term_obj);
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
	case aml_remainder:
		if(aml_symbol->component.remainder.term_arg)delete_aml_symbol(aml_symbol->component.remainder.term_arg);
		break;
	case aml_return_op:
		break;
	case aml_reserved_field:
		if(aml_symbol->component.reserved_field.reserved_field_op)delete_aml_symbol(aml_symbol->component.reserved_field.reserved_field_op);
		if(aml_symbol->component.reserved_field.pkg_length)delete_aml_symbol(aml_symbol->component.reserved_field.pkg_length);
		break;
	case aml_reserved_field_op:
		break;
	case aml_revision_op:
		if(aml_symbol->component.revision_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.revision_op.ext_op_prefix);
		if(aml_symbol->component.revision_op.revision_op_suffix)delete_aml_symbol(aml_symbol->component.revision_op.revision_op_suffix);
		break;
	case aml_revision_op_suffix:
		break;
	case aml_root_char:
		break;
	case aml_scope_op:
		break;
	case aml_search_pkg:
		if(aml_symbol->component.search_pkg.term_arg)delete_aml_symbol(aml_symbol->component.search_pkg.term_arg);
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
	case aml_sleep_op:
		if(aml_symbol->component.sleep_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.sleep_op.ext_op_prefix);
		if(aml_symbol->component.sleep_op.sleep_op_suffix)delete_aml_symbol(aml_symbol->component.sleep_op.sleep_op_suffix);
		break;
	case aml_sleep_op_suffix:
		break;
	case aml_source_buff:
		if(aml_symbol->component.source_buff.term_arg)delete_aml_symbol(aml_symbol->component.source_buff.term_arg);
		break;
	case aml_stall_op:
		if(aml_symbol->component.stall_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.stall_op.ext_op_prefix);
		if(aml_symbol->component.stall_op.stall_op_suffix)delete_aml_symbol(aml_symbol->component.stall_op.stall_op_suffix);
		break;
	case aml_stall_op_suffix:
		break;
	case aml_start_index:
		if(aml_symbol->component.start_index.term_arg)delete_aml_symbol(aml_symbol->component.start_index.term_arg);
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
		if(aml_symbol->component.term_obj.wrong_term_arg)delete_aml_symbol(aml_symbol->component.term_obj.wrong_term_arg);
		break;
	case aml_thermal_zone_op:
		if(aml_symbol->component.thermal_zone_op.ext_op_prefix)delete_aml_symbol(aml_symbol->component.thermal_zone_op.ext_op_prefix);
		if(aml_symbol->component.thermal_zone_op.thermal_zone_op_suffix)delete_aml_symbol(aml_symbol->component.thermal_zone_op.thermal_zone_op_suffix);
		break;
	case aml_thermal_zone_op_suffix:
		break;
	case aml_time_out:
		if(aml_symbol->component.time_out.word_data)delete_aml_symbol(aml_symbol->component.time_out.word_data);
		break;
	case aml_to_buffer_op:
		break;
	case aml_to_hex_string_op:
		break;
	case aml_to_integer_op:
		break;
	case aml_user_term_obj:
		break;
	case aml_usec_time:
		if(aml_symbol->component.usec_time.term_arg)delete_aml_symbol(aml_symbol->component.usec_time.term_arg);
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
	case aml_xor_op:
		break;
	case aml_zero_op:
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

AMLSymbol const *get_aml_def_name(char const *name, AMLSubstring aml)
{
	for(unsigned char const *name_string_initial = aml.initial; name_string_initial != aml.initial + aml.length; name_string_initial++)if(!strncmp(name, (char const *)name_string_initial, strlen(name)))
	{
		unsigned char const *def_name_initial = name_string_initial - 1;
		if(*def_name_initial != AML_BYTE_NAME_OP)continue;
		AMLSubstring def_name_aml;
		def_name_aml.initial = def_name_initial;
		def_name_aml.length = (unsigned int)aml.initial + aml.length - (unsigned int)def_name_initial;
		return analyse_aml_def_name(NULL, def_name_aml);
	}
	return NULL;
}

AMLSymbol const *get_aml_method(char const *method_name, AMLSymbol const *aml_symbol, AMLSymbol const *searched)
{
	AMLSymbol const *def_method = NULL;
	AMLSymbol const *def_method_in_def_device = NULL;
	AMLSymbol const *def_method_in_def_else = NULL;
	AMLSymbol const *def_method_in_def_if_else = NULL;
	AMLSymbol const *def_method_in_def_scope = NULL;
	AMLSymbol const *def_method_in_def_while = NULL;
	AMLSymbol const *def_method_in_name_space_modifier_obj = NULL;
	AMLSymbol const *def_method_in_named_obj = NULL;
	AMLSymbol const *def_method_in_term_list = NULL;
	AMLSymbol const *def_method_in_term_obj = NULL;
	AMLSymbol const *def_method_in_object = NULL;
	AMLSymbol const *def_method_in_statement_opcode = NULL;
	if(!method_name || !aml_symbol)return NULL;
	switch(aml_symbol->type)
	{
	case aml_def_device:
		if(aml_symbol->component.def_device.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_device.term_list, aml_symbol);
		break;
	case aml_def_else:
		if(aml_symbol->component.def_else.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_else.term_list, aml_symbol);
		break;
	case aml_def_if_else:
		if(aml_symbol->component.def_if_else.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_if_else.term_list, aml_symbol);
		if(aml_symbol->component.def_if_else.def_else != searched)def_method_in_def_else = get_aml_method(method_name, aml_symbol->component.def_if_else.def_else, aml_symbol);
		break;
	case aml_def_method:
		if(!strcmp(method_name, aml_symbol->component.def_method.name_string->component.name_string.string))return aml_symbol;
		if(aml_symbol->component.def_method.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_method.term_list, aml_symbol);
		break;
	case aml_def_scope:
		if(aml_symbol->component.def_scope.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_scope.term_list, aml_symbol);
		break;
	case aml_def_while:
		if(aml_symbol->component.def_while.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.def_while.term_list, aml_symbol);
		break;
	case aml_name_space_modifier_obj:
		if(aml_symbol->component.name_space_modifier_obj.def_scope != searched)def_method_in_def_scope = get_aml_method(method_name, aml_symbol->component.name_space_modifier_obj.def_scope, aml_symbol);
		break;
	case aml_named_obj:
		if(aml_symbol->component.named_obj.def_device != searched)def_method_in_def_device = get_aml_method(method_name, aml_symbol->component.named_obj.def_device, aml_symbol);
		if(aml_symbol->component.named_obj.def_method != searched)def_method = get_aml_method(method_name, aml_symbol->component.named_obj.def_method, aml_symbol);
		break;
	case aml_object:
		if(aml_symbol->component.object.name_space_modifier_obj != searched)def_method_in_name_space_modifier_obj = get_aml_method(method_name, aml_symbol->component.object.name_space_modifier_obj, aml_symbol);
		if(aml_symbol->component.object.named_obj != searched)def_method_in_named_obj = get_aml_method(method_name, aml_symbol->component.object.named_obj, aml_symbol);
		break;
	case aml_statement_opcode:
		if(aml_symbol->component.statement_opcode.def_if_else != searched)def_method_in_def_if_else = get_aml_method(method_name, aml_symbol->component.statement_opcode.def_if_else, aml_symbol);
		if(aml_symbol->component.statement_opcode.def_while != searched)def_method_in_def_while = get_aml_method(method_name, aml_symbol->component.statement_opcode.def_while, aml_symbol);
		break;
	case aml_term_list:
		if(aml_symbol->component.term_list.term_list != searched)def_method_in_term_list = get_aml_method(method_name, aml_symbol->component.term_list.term_list, aml_symbol);
		if(aml_symbol->component.term_list.term_obj != searched)def_method_in_term_obj = get_aml_method(method_name, aml_symbol->component.term_list.term_obj, aml_symbol);
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object != searched)def_method_in_object = get_aml_method(method_name, aml_symbol->component.term_obj.object, aml_symbol);
		if(aml_symbol->component.term_obj.statement_opcode != searched)def_method_in_statement_opcode = get_aml_method(method_name, aml_symbol->component.term_obj.statement_opcode, aml_symbol);
		break;
	default:
		break;
	}
	if(def_method)return def_method;
	else if(def_method_in_def_device)return def_method_in_def_device;
	else if(def_method_in_def_else)return def_method_in_def_else;
	else if(def_method_in_def_if_else)return def_method_in_def_if_else;
	else if(def_method_in_def_scope)return def_method_in_def_scope;
	else if(def_method_in_def_while)return def_method_in_def_while;
	else if(def_method_in_name_space_modifier_obj)return def_method_in_name_space_modifier_obj;
	else if(def_method_in_named_obj)return def_method_in_named_obj;
	else if(def_method_in_term_list)return def_method_in_term_list;
	else if(def_method_in_term_obj)return def_method_in_term_obj;
	else if(def_method_in_object)return def_method_in_object;
	else if(def_method_in_statement_opcode)return def_method_in_statement_opcode;
	else if(aml_symbol->parent != searched)return get_aml_method(method_name, aml_symbol->parent, aml_symbol);
	else return NULL;
}

AMLSymbol const *get_aml_s5_package(AMLSubstring aml)
{
	AMLSymbol const *s5_def_name = get_aml_def_name("_S5_", aml);
	AMLSymbol const *s5_data_ref_object = s5_def_name->component.def_name.data_ref_object;
	AMLSymbol const *s5_data_object;
	if(!s5_data_ref_object)return NULL;
	s5_data_object = s5_data_ref_object->component.data_ref_object.data_object;
	if(!s5_data_object)return NULL;
	return s5_data_object->component.data_object.def_package;
}

unsigned short get_aml_s5_pm1_cnt_slp_typ(AMLSubstring aml)
{
	unsigned char pm1a_cnt_slp_typ = 0;
	unsigned char pm1b_cnt_slp_typ = 0;
	AMLSymbol const *s5_package = get_aml_s5_package(aml);
	AMLSymbol const *s5_package_element_list = s5_package->component.def_package.package_element_list;
	AMLSymbol const *s5_pm1a_cnt_slp_typ_package_element = s5_package_element_list->component.package_element_list.package_element;
	AMLSymbol const *s5_pm1a_cnt_slp_typ_data_ref_object = s5_pm1a_cnt_slp_typ_package_element->component.package_element.data_ref_object;
	AMLSymbol const *s5_pm1a_cnt_slp_typ_data_object = s5_pm1a_cnt_slp_typ_data_ref_object->component.data_ref_object.data_object;
	AMLSymbol const *s5_pm1a_cnt_slp_typ_computational_data = s5_pm1a_cnt_slp_typ_data_object->component.data_object.computational_data;
	if(!s5_package->component.def_package.num_elements->component.num_elements.num_of_elements)
	{
		ERROR();
		return 0;
	}
	if(!s5_package_element_list)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1a_cnt_slp_typ_package_element)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1a_cnt_slp_typ_data_ref_object)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1a_cnt_slp_typ_data_object)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1a_cnt_slp_typ_computational_data)
	{
		ERROR();
		return 0;
	}
	if(s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.byte_const)
	{
		pm1a_cnt_slp_typ = s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.byte_const->component.byte_const.value;
	}
	else if(s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.word_const)
	{
		return s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.word_const->component.word_const.value;
	}
	else if(s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.dword_const)
	{
		return s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.dword_const->component.dword_const.value;
	}
	else if(s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.qword_const)
	{
		return s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.qword_const->component.qword_const.value;
	}
	else if(s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.const_obj)
	{
		AMLSymbol const *s5_pm1a_cnt_slp_typ_const_obj = s5_pm1a_cnt_slp_typ_computational_data->component.computational_data.const_obj;
		if(s5_pm1a_cnt_slp_typ_const_obj->component.const_obj.zero_op)pm1a_cnt_slp_typ = 0x00;
		if(s5_pm1a_cnt_slp_typ_const_obj->component.const_obj.one_op)pm1a_cnt_slp_typ = 0x01;
		if(s5_pm1a_cnt_slp_typ_const_obj->component.const_obj.ones_op)pm1a_cnt_slp_typ = 0xff;
	}
	else
	{
		ERROR(); // Unimplemented pattern
		return 0;
	}
	AMLSymbol const *s5_pm1b_cnt_slp_typ_package_element = s5_package_element_list->component.package_element_list.package_element_list->component.package_element_list.package_element;
	AMLSymbol const *s5_pm1b_cnt_slp_typ_data_ref_object = s5_pm1b_cnt_slp_typ_package_element->component.package_element.data_ref_object;
	AMLSymbol const *s5_pm1b_cnt_slp_typ_data_object = s5_pm1b_cnt_slp_typ_data_ref_object->component.data_ref_object.data_object;
	AMLSymbol const *s5_pm1b_cnt_slp_typ_computational_data = s5_pm1b_cnt_slp_typ_data_object->component.data_object.computational_data;
	if(!s5_package->component.def_package.num_elements->component.num_elements.num_of_elements)
	{
		ERROR();
		return 0;
	}
	if(!s5_package_element_list)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1b_cnt_slp_typ_package_element)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1b_cnt_slp_typ_data_ref_object)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1b_cnt_slp_typ_data_object)
	{
		ERROR();
		return 0;
	}
	if(!s5_pm1b_cnt_slp_typ_computational_data)
	{
		ERROR();
		return 0;
	}
	if(s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.byte_const)
	{
		pm1b_cnt_slp_typ = s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.byte_const->component.byte_const.value;
	}
	else if(s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.word_const)
	{
		pm1b_cnt_slp_typ = s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.word_const->component.word_const.value;
	}
	else if(s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.dword_const)
	{
		pm1b_cnt_slp_typ = s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.dword_const->component.dword_const.value;
	}
	else if(s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.qword_const)
	{
		pm1b_cnt_slp_typ = s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.qword_const->component.qword_const.value;
	}
	else if(s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.const_obj)
	{
		AMLSymbol const *s5_pm1b_cnt_slp_typ_const_obj = s5_pm1b_cnt_slp_typ_computational_data->component.computational_data.const_obj;
		if(s5_pm1b_cnt_slp_typ_const_obj->component.const_obj.zero_op)pm1b_cnt_slp_typ = 0x00;
		if(s5_pm1b_cnt_slp_typ_const_obj->component.const_obj.one_op)pm1b_cnt_slp_typ = 0x01;
		if(s5_pm1b_cnt_slp_typ_const_obj->component.const_obj.ones_op)pm1b_cnt_slp_typ = 0xff;
	}
	else
	{
		ERROR(); // Unimplemented pattern
		return 0;
	}
	return (unsigned short)pm1b_cnt_slp_typ << 8 | (unsigned short)pm1a_cnt_slp_typ;
}

unsigned int get_aml_symbol_depth(AMLSymbol const *aml_symbol)
{
	if(aml_symbol->parent)return get_aml_symbol_depth(aml_symbol->parent) + 1;
	else return 0;
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
	if(!rsdt_header || strncmp(rsdt_header->signature, "RSDT", 4))
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
	printf_serial("%s.signature = %.*s\n", name, sizeof(acpi_table_header.signature), acpi_table_header.signature);
	printf_serial("%s.length = %#010.8x\n", name, acpi_table_header.length);
	printf_serial("%s.revision = %#04.2x\n", name, acpi_table_header.revision);
	printf_serial("%s.checksum = %#04.2x\n", name, acpi_table_header.checksum);
	printf_serial("%s.oem_id = %.*s\n", name, sizeof(acpi_table_header.oem_id), acpi_table_header.oem_id);
	printf_serial("%s.oem_table_id = %.*s\n", name, sizeof(acpi_table_header.oem_table_id), acpi_table_header.oem_table_id);
	printf_serial("%s.oem_revision = %#010.8x\n", name, acpi_table_header.oem_revision);
	printf_serial("%s.creater_id = %#010.8x\n", name, acpi_table_header.creater_id);
	printf_serial("%s.creater_revision = %#010.8x\n", name, acpi_table_header.creater_revision);
}

void print_acpi_table_header_p(ACPITableHeader const *acpi_table_header, char const *name)
{
	printf_serial("%s = %p\n", name, acpi_table_header);
	printf_serial("%s->signature = %.*s\n", name, sizeof(acpi_table_header->signature), acpi_table_header->signature);
	printf_serial("%s->length = %#010.8x\n", name, acpi_table_header->length);
	printf_serial("%s->revision = %#04.2x\n", name, acpi_table_header->revision);
	printf_serial("%s->checksum = %#04.2x\n", name, acpi_table_header->checksum);
	printf_serial("%s->oem_id = %.*s\n", name, sizeof(acpi_table_header->oem_id), acpi_table_header->oem_id);
	printf_serial("%s->oem_table_id = %.*s\n", name, sizeof(acpi_table_header->oem_table_id), acpi_table_header->oem_table_id);
	printf_serial("%s->oem_revision = %#010.8x\n", name, acpi_table_header->oem_revision);
	printf_serial("%s->creater_id = %#010.8x\n", name, acpi_table_header->creater_id);
	printf_serial("%s->creater_revision = %#010.8x\n", name, acpi_table_header->creater_revision);
}

void print_aml_symbol(AMLSymbol const *aml_symbol)
{
	for(unsigned int i = 0; i < get_aml_symbol_depth(aml_symbol); i++)printf_serial(" ");
	printf_serial("%s", aml_symbol_type_name(aml_symbol->type));
	printf_serial(" initial = %p", aml_symbol->string.initial);
	printf_serial(" length = %#010.8x", aml_symbol->string.length);
	switch(aml_symbol->type)
	{
	case aml_access_attrib:
		break;
	case aml_access_field:
		break;
	case aml_access_field_op:
		break;
	case aml_access_type:
		break;
	case aml_acquire_op:
		break;
	case aml_acquire_op_suffix:
		break;
	case aml_alias_op:
		break;
	case aml_add_op:
		break;
	case aml_and_op:
		break;
	case aml_arg_obj:
		printf_serial(" Arg%dOp", aml_symbol->component.arg_obj.arg_op_number);
		break;
	case aml_arg_object:
		break;
	case aml_arg_op:
		break;
	case aml_ascii_char:
		printf_serial(" '%c'", *aml_symbol->string.initial);
		break;
	case aml_ascii_char_list:
		printf_serial(" \"%.*s\"", aml_symbol->string.length, aml_symbol->string.initial);
		break;
	case aml_bit_index:
		break;
	case aml_break_op:
		break;
	case aml_buf_data:
		break;
	case aml_buff_pkg_str_obj:
		break;
	case aml_buffer_op:
		break;
	case aml_buffer_size:
		break;
	case aml_byte_const:
		printf_serial(" value = %#04.2x", aml_symbol->component.byte_const.value);
		break;
	case aml_byte_data:
		printf_serial(" value = %#04.2x", *aml_symbol->string.initial);
		break;
	case aml_byte_index:
		break;
	case aml_byte_list:
		break;
	case aml_byte_prefix:
		break;
	case aml_computational_data:
		break;
	case aml_concat_op:
		break;
	case aml_concat_res_op:
		break;
	case aml_cond_ref_of_op:
		break;
	case aml_cond_ref_of_op_suffix:
		break;
	case aml_const_obj:
		break;
	case aml_create_bit_field_op:
		break;
	case aml_create_byte_field_op:
		break;
	case aml_create_dword_field_op:
		break;
	case aml_create_field_op:
		break;
	case aml_create_field_op_suffix:
		break;
	case aml_create_qword_field_op:
		break;
	case aml_create_word_field_op:
		break;
	case aml_data:
		break;
	case aml_data_object:
		break;
	case aml_data_ref_object:
		break;
	case aml_debug_obj:
		break;
	case aml_debug_op:
		break;
	case aml_debug_op_suffix:
		break;
	case aml_decrement_op:
		break;
	case aml_def_alias:
		break;
	case aml_def_acquire:
		break;
	case aml_def_add:
		break;
	case aml_def_and:
		break;
	case aml_def_break:
		break;
	case aml_def_buffer:
		break;
	case aml_def_concat:
		break;
	case aml_def_concat_res:
		break;
	case aml_def_cond_ref_of:
		break;
	case aml_def_create_bit_field:
		break;
	case aml_def_create_byte_field:
		break;
	case aml_def_create_dword_field:
		break;
	case aml_def_create_field:
		break;
	case aml_def_create_qword_field:
		break;
	case aml_def_create_word_field:
		break;
	case aml_def_decrement:
		break;
	case aml_def_deref_of:
		break;
	case aml_def_device:
		break;
	case aml_def_divide:
		break;
	case aml_def_else:
		break;
	case aml_def_field:
		break;
	case aml_def_find_set_left_bit:
		break;
	case aml_def_find_set_right_bit:
		break;
	case aml_def_if_else:
		break;
	case aml_def_increment:
		break;
	case aml_def_index:
		break;
	case aml_def_index_field:
		break;
	case aml_def_l_and:
		break;
	case aml_def_l_equal:
		break;
	case aml_def_l_greater:
		break;
	case aml_def_l_less:
		break;
	case aml_def_l_not:
		break;
	case aml_def_l_or:
		break;
	case aml_def_match:
		break;
	case aml_def_method:
		break;
	case aml_def_mid:
		break;
	case aml_def_multiply:
		break;
	case aml_def_mutex:
		break;
	case aml_def_name:
		break;
	case aml_def_not:
		break;
	case aml_def_notify:
		break;
	case aml_def_object_type:
		break;
	case aml_def_op_region:
		break;
	case aml_def_or:
		break;
	case aml_def_package:
		break;
	case aml_def_processor:
		break;
	case aml_def_ref_of:
		break;
	case aml_def_release:
		break;
	case aml_def_return:
		break;
	case aml_def_scope:
		break;
	case aml_def_shift_left:
		break;
	case aml_def_shift_right:
		break;
	case aml_def_size_of:
		break;
	case aml_def_sleep:
		break;
	case aml_def_stall:
		break;
	case aml_def_store:
		break;
	case aml_def_subtract:
		break;
	case aml_def_thermal_zone:
		break;
	case aml_def_to_buffer:
		break;
	case aml_def_to_hex_string:
		break;
	case aml_def_to_integer:
		break;
	case aml_def_while:
		break;
	case aml_def_xor:
		break;
	case aml_deref_of_op:
		break;
	case aml_device_op:
		break;
	case aml_device_op_suffix:
		break;
	case aml_digit_char:
		break;
	case aml_divide_op:
		break;
	case aml_dividend:
		break;
	case aml_divisor:
		break;
	case aml_dual_name_path:
		printf_serial(" \"%s\"", aml_symbol->component.dual_name_path.string);
		break;
	case aml_dual_name_prefix:
		break;
	case aml_dword_const:
		printf_serial(" value = %#010.8x", aml_symbol->component.dword_const.value);
		break;
	case aml_dword_data:
		printf_serial(" value = %#010.8x", aml_symbol->component.dword_data.value);
		break;
	case aml_dword_prefix:
		break;
	case aml_else_op:
		break;
	case aml_expression_opcode:
		break;
	case aml_ext_op_prefix:
		break;
	case aml_field_element:
		break;
	case aml_field_flags:
		break;
	case aml_field_list:
		break;
	case aml_field_op:
		break;
	case aml_field_op_suffix:
		break;
	case aml_find_set_left_bit_op:
		break;
	case aml_find_set_right_bit_op:
		break;
	case aml_if_op:
		break;
	case aml_increment_op:
		break;
	case aml_index_field_op:
		break;
	case aml_index_field_op_suffix:
		break;
	case aml_index_op:
		break;
	case aml_index_value:
		break;
	case aml_lead_name_char:
		printf_serial(" '%c'", *aml_symbol->string.initial);
		break;
	case aml_l_and_op:
		break;
	case aml_l_equal_op:
		break;
	case aml_l_greater_op:
		break;
	case aml_l_less_op:
		break;
	case aml_l_not_op:
		break;
	case aml_l_or_op:
		break;
	case aml_local_obj:
		printf_serial(" Local%dOp", aml_symbol->component.local_obj.local_op_number);
		break;
	case aml_local_op:
		break;
	case aml_match_op:
		break;
	case aml_match_opcode:
		break;
	case aml_method_flags:
		printf_serial(" arg_count = %d, %s, sync_level = %d", aml_symbol->component.method_flags.arg_count, aml_symbol->component.method_flags.serialize_flag ? "Serialized" : "NotSerialized", aml_symbol->component.method_flags.sync_level);
		break;
	case aml_method_invocation:
		break;
	case aml_method_op:
		break;
	case aml_mid_obj:
		break;
	case aml_mid_op:
		break;
	case aml_msec_time:
		break;
	case aml_multi_name_path:
		printf_serial(" \"%s\"", aml_symbol->component.multi_name_path.string);
		break;
	case aml_multi_name_prefix:
		break;
	case aml_multiply_op:
		break;
	case aml_mutex_object:
		break;
	case aml_mutex_op:
		break;
	case aml_mutex_op_suffix:
		break;
	case aml_name_char:
		printf_serial(" '%c'", aml_symbol->component.name_char.character);
		break;
	case aml_name_op:
		break;
	case aml_name_path:
		printf_serial(" \"%s\"", aml_symbol->component.name_path.string);
		break;
	case aml_name_seg:
		printf_serial(" \"%s\"", aml_symbol->component.name_seg.string);
		break;
	case aml_name_space_modifier_obj:
		break;
	case aml_name_string:
		printf_serial(" \"%s\"", aml_symbol->component.name_string.string);
		break;
	case aml_named_field:
		break;
	case aml_named_obj:
		break;
	case aml_not_op:
		break;
	case aml_notify_object:
		break;
	case aml_notify_value:
		break;
	case aml_notify_op:
		break;
	case aml_null_char:
		break;
	case aml_null_name:
		break;
	case aml_num_bits:
		break;
	case aml_num_elements:
		printf_serial(" NumOfElements = %#04.2x", aml_symbol->component.num_elements.num_of_elements);
		break;
	case aml_obj_reference:
		break;
	case aml_object:
		break;
	case aml_object_type_op:
		break;
	case aml_one_op:
		break;
	case aml_ones_op:
		break;
	case aml_op_region_op:
		break;
	case aml_op_region_op_suffix:
		break;
	case aml_operand:
		break;
	case aml_or_op:
		break;
	case aml_package_element:
		break;
	case aml_package_element_list:
		break;
	case aml_package_op:
		break;
	case aml_parent_prefix_char:
		break;
	case aml_pblk_addr:
		break;
	case aml_pblk_len:
		break;
	case aml_pkg_lead_byte:
		break;
	case aml_pkg_length:
		printf_serial(" pkg_length = %#010.8x", aml_symbol->component.pkg_length.length);
		break;
	case aml_predicate:
		break;
	case aml_prefix_path:
		break;
	case aml_proc_id:
		break;
	case aml_processor_op:
		break;
	case aml_processor_op_suffix:
		break;
	case aml_quotient:
		break;
	case aml_qword_const:
		printf_serial(" value = %#018.16x", aml_symbol->component.qword_const.value);
		break;
	case aml_qword_data:
		printf_serial(" value = %#018.16x", aml_symbol->component.qword_data.value);
		break;
	case aml_qword_prefix:
		break;
	case aml_ref_of_op:
		break;
	case aml_reference_type_opcode:
		break;
	case aml_region_len:
		break;
	case aml_region_offset:
		break;
	case aml_region_space:
		break;
	case aml_release_op:
		break;
	case aml_release_op_suffix:
		break;
	case aml_reserved_field:
		break;
	case aml_reserved_field_op:
		break;
	case aml_remainder:
		break;
	case aml_return_op:
		break;
	case aml_revision_op:
		break;
	case aml_revision_op_suffix:
		break;
	case aml_root_char:
		break;
	case aml_scope_op:
		break;
	case aml_search_pkg:
		break;
	case aml_seg_count:
		break;
	case aml_shift_count:
		break;
	case aml_shift_left_op:
		break;
	case aml_shift_right_op:
		break;
	case aml_simple_name:
		break;
	case aml_size_of_op:
		break;
	case aml_sleep_op:
		break;
	case aml_sleep_op_suffix:
		break;
	case aml_source_buff:
		break;
	case aml_stall_op:
		break;
	case aml_stall_op_suffix:
		break;
	case aml_start_index:
		break;
	case aml_statement_opcode:
		break;
	case aml_store_op:
		break;
	case aml_string:
		break;
	case aml_string_prefix:
		break;
	case aml_subtract_op:
		break;
	case aml_super_name:
		break;
	case aml_sync_flags:
		break;
	case aml_target:
		break;
	case aml_term_arg:
		break;
	case aml_term_arg_list:
		break;
	case aml_term_list:
		break;
	case aml_term_obj:
		break;
	case aml_thermal_zone_op:
		break;
	case aml_thermal_zone_op_suffix:
		break;
	case aml_time_out:
		break;
	case aml_to_buffer_op:
		break;
	case aml_to_hex_string_op:
		break;
	case aml_to_integer_op:
		break;
	case aml_user_term_obj:
		break;
	case aml_usec_time:
		break;
	case aml_while_op:
		break;
	case aml_word_const:
		printf_serial(" value = %#06.4x", aml_symbol->component.word_const.value);
		break;
	case aml_word_data:
		printf_serial(" value = %#06.4x", aml_symbol->component.word_data.value);
		break;
	case aml_word_prefix:
		break;
	case aml_xor_op:
		break;
	case aml_zero_op:
		break;
	}
	if(aml_symbol->flags & AML_SYMBOL_ERROR)printf_serial(" ERROR!!!");
	printf_serial("\n");
	switch(aml_symbol->type)
	{
	case aml_access_attrib:
		if(aml_symbol->component.access_attrib.byte_data)print_aml_symbol(aml_symbol->component.access_attrib.byte_data);
		break;
	case aml_access_field:
		if(aml_symbol->component.access_field.access_field_op)print_aml_symbol(aml_symbol->component.access_field.access_field_op);
		if(aml_symbol->component.access_field.access_type)print_aml_symbol(aml_symbol->component.access_field.access_type);
		if(aml_symbol->component.access_field.access_attrib)print_aml_symbol(aml_symbol->component.access_field.access_attrib);
		break;
	case aml_access_field_op:
		break;
	case aml_access_type:
		if(aml_symbol->component.access_type.byte_data)print_aml_symbol(aml_symbol->component.access_type.byte_data);
		break;
	case aml_acquire_op:
		if(aml_symbol->component.acquire_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.acquire_op.ext_op_prefix);
		if(aml_symbol->component.acquire_op.acquire_op_suffix)print_aml_symbol(aml_symbol->component.acquire_op.acquire_op_suffix);
		break;
	case aml_acquire_op_suffix:
		break;
	case aml_alias_op:
		break;
	case aml_add_op:
		break;
	case aml_and_op:
		break;
	case aml_arg_obj:
		if(aml_symbol->component.arg_obj.arg_op)print_aml_symbol(aml_symbol->component.arg_obj.arg_op);
		break;
	case aml_arg_object:
		if(aml_symbol->component.arg_object.term_arg)print_aml_symbol(aml_symbol->component.arg_object.term_arg);
		break;
	case aml_arg_op:
		break;
	case aml_ascii_char:
		break;
	case aml_ascii_char_list:
		if(aml_symbol->component.ascii_char_list.ascii_char)print_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char);
		if(aml_symbol->component.ascii_char_list.ascii_char_list)print_aml_symbol(aml_symbol->component.ascii_char_list.ascii_char_list);
		break;
	case aml_bit_index:
		if(aml_symbol->component.bit_index.term_arg)print_aml_symbol(aml_symbol->component.bit_index.term_arg);
		break;
	case aml_break_op:
		break;
	case aml_buf_data:
		if(aml_symbol->component.buf_data.term_arg)print_aml_symbol(aml_symbol->component.buf_data.term_arg);
		break;
	case aml_buff_pkg_str_obj:
		if(aml_symbol->component.buff_pkg_str_obj.term_arg)print_aml_symbol(aml_symbol->component.buff_pkg_str_obj.term_arg);
		break;
	case aml_buffer_op:
		break;
	case aml_buffer_size:
		if(aml_symbol->component.buffer_size.term_arg)print_aml_symbol(aml_symbol->component.buffer_size.term_arg);
		break;
	case aml_byte_const:
		if(aml_symbol->component.byte_const.byte_prefix)print_aml_symbol(aml_symbol->component.byte_const.byte_prefix);
		if(aml_symbol->component.byte_const.byte_data)print_aml_symbol(aml_symbol->component.byte_const.byte_data);
		break;
	case aml_byte_data:
		break;
	case aml_byte_index:
		if(aml_symbol->component.byte_index.term_arg)print_aml_symbol(aml_symbol->component.byte_index.term_arg);
		break;
	case aml_byte_list:
		if(aml_symbol->component.byte_list.byte_data)print_aml_symbol(aml_symbol->component.byte_list.byte_data);
		if(aml_symbol->component.byte_list.byte_list)print_aml_symbol(aml_symbol->component.byte_list.byte_list);
		break;
	case aml_byte_prefix:
		break;
	case aml_computational_data:
		if(aml_symbol->component.computational_data.byte_const)print_aml_symbol(aml_symbol->component.computational_data.byte_const);
		if(aml_symbol->component.computational_data.word_const)print_aml_symbol(aml_symbol->component.computational_data.word_const);
		if(aml_symbol->component.computational_data.dword_const)print_aml_symbol(aml_symbol->component.computational_data.dword_const);
		if(aml_symbol->component.computational_data.qword_const)print_aml_symbol(aml_symbol->component.computational_data.qword_const);
		if(aml_symbol->component.computational_data.string)print_aml_symbol(aml_symbol->component.computational_data.string);
		if(aml_symbol->component.computational_data.const_obj)print_aml_symbol(aml_symbol->component.computational_data.const_obj);
		if(aml_symbol->component.computational_data.revision_op)print_aml_symbol(aml_symbol->component.computational_data.revision_op);
		if(aml_symbol->component.computational_data.def_buffer)print_aml_symbol(aml_symbol->component.computational_data.def_buffer);
		break;
	case aml_concat_op:
		break;
	case aml_concat_res_op:
		break;
	case aml_cond_ref_of_op:
		if(aml_symbol->component.cond_ref_of_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.cond_ref_of_op.ext_op_prefix);
		if(aml_symbol->component.cond_ref_of_op.cond_ref_of_op_suffix)print_aml_symbol(aml_symbol->component.cond_ref_of_op.cond_ref_of_op_suffix);
		break;
	case aml_cond_ref_of_op_suffix:
		break;
	case aml_const_obj:
		if(aml_symbol->component.const_obj.zero_op)print_aml_symbol(aml_symbol->component.const_obj.zero_op);
		if(aml_symbol->component.const_obj.one_op)print_aml_symbol(aml_symbol->component.const_obj.one_op);
		if(aml_symbol->component.const_obj.ones_op)print_aml_symbol(aml_symbol->component.const_obj.ones_op);
		break;
	case aml_create_bit_field_op:
		break;
	case aml_create_byte_field_op:
		break;
	case aml_create_dword_field_op:
		break;
	case aml_create_field_op:
		if(aml_symbol->component.create_field_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.create_field_op.ext_op_prefix);
		if(aml_symbol->component.create_field_op.create_field_op_suffix)print_aml_symbol(aml_symbol->component.create_field_op.create_field_op_suffix);
		break;
	case aml_create_field_op_suffix:
		break;
	case aml_create_qword_field_op:
		break;
	case aml_create_word_field_op:
		break;
	case aml_data:
		if(aml_symbol->component.data.term_arg)print_aml_symbol(aml_symbol->component.data.term_arg);
		break;
	case aml_data_object:
		if(aml_symbol->component.data_object.computational_data)print_aml_symbol(aml_symbol->component.data_object.computational_data);
		if(aml_symbol->component.data_object.def_package)print_aml_symbol(aml_symbol->component.data_object.def_package);
		if(aml_symbol->component.data_object.def_var_package)print_aml_symbol(aml_symbol->component.data_object.def_var_package);
		break;
	case aml_data_ref_object:
		if(aml_symbol->component.data_ref_object.data_object)print_aml_symbol(aml_symbol->component.data_ref_object.data_object);
		if(aml_symbol->component.data_ref_object.object_reference)print_aml_symbol(aml_symbol->component.data_ref_object.object_reference);
		break;
	case aml_debug_obj:
		if(aml_symbol->component.debug_obj.debug_op)print_aml_symbol(aml_symbol->component.debug_obj.debug_op);
		break;
	case aml_debug_op:
		if(aml_symbol->component.debug_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.debug_op.ext_op_prefix);
		if(aml_symbol->component.debug_op.debug_op_suffix)print_aml_symbol(aml_symbol->component.debug_op.debug_op_suffix);
		break;
	case aml_debug_op_suffix:
		break;
	case aml_decrement_op:
		break;
	case aml_def_alias:
		if(aml_symbol->component.def_alias.alias_op)print_aml_symbol(aml_symbol->component.def_alias.alias_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_alias.name_string); i++)if(aml_symbol->component.def_alias.name_string[i])print_aml_symbol(aml_symbol->component.def_alias.name_string[i]);
		break;
	case aml_def_acquire:
		if(aml_symbol->component.def_acquire.acquire_op)print_aml_symbol(aml_symbol->component.def_acquire.acquire_op);
		if(aml_symbol->component.def_acquire.mutex_object)print_aml_symbol(aml_symbol->component.def_acquire.mutex_object);
		if(aml_symbol->component.def_acquire.time_out)print_aml_symbol(aml_symbol->component.def_acquire.time_out);
		break;
	case aml_def_add:
		if(aml_symbol->component.def_add.add_op)print_aml_symbol(aml_symbol->component.def_add.add_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_add.operand); i++)if(aml_symbol->component.def_add.operand[i])print_aml_symbol(aml_symbol->component.def_add.operand[i]);
		if(aml_symbol->component.def_add.target)print_aml_symbol(aml_symbol->component.def_add.target);
		break;
	case aml_def_and:
		if(aml_symbol->component.def_and.and_op)print_aml_symbol(aml_symbol->component.def_and.and_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_and.operand); i++)if(aml_symbol->component.def_and.operand[i])print_aml_symbol(aml_symbol->component.def_and.operand[i]);
		if(aml_symbol->component.def_and.target)print_aml_symbol(aml_symbol->component.def_and.target);
		break;
	case aml_def_break:
		if(aml_symbol->component.def_break.break_op)print_aml_symbol(aml_symbol->component.def_break.break_op);
		break;
	case aml_def_buffer:
		if(aml_symbol->component.def_buffer.buffer_op)print_aml_symbol(aml_symbol->component.def_buffer.buffer_op);
		if(aml_symbol->component.def_buffer.pkg_length)print_aml_symbol(aml_symbol->component.def_buffer.pkg_length);
		if(aml_symbol->component.def_buffer.buffer_size)print_aml_symbol(aml_symbol->component.def_buffer.buffer_size);
		if(aml_symbol->component.def_buffer.byte_list)print_aml_symbol(aml_symbol->component.def_buffer.byte_list);
		break;
	case aml_def_concat:
		if(aml_symbol->component.def_concat.concat_op)print_aml_symbol(aml_symbol->component.def_concat.concat_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_concat.data); i++)if(aml_symbol->component.def_concat.data[i])print_aml_symbol(aml_symbol->component.def_concat.data[i]);
		if(aml_symbol->component.def_concat.target)print_aml_symbol(aml_symbol->component.def_concat.target);
		break;
	case aml_def_concat_res:
		if(aml_symbol->component.def_concat_res.concat_res_op)print_aml_symbol(aml_symbol->component.def_concat_res.concat_res_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_concat_res.buf_data); i++)if(aml_symbol->component.def_concat_res.buf_data[i])print_aml_symbol(aml_symbol->component.def_concat_res.buf_data[i]);
		if(aml_symbol->component.def_concat_res.target)print_aml_symbol(aml_symbol->component.def_concat_res.target);
		break;
	case aml_def_cond_ref_of:
		if(aml_symbol->component.def_cond_ref_of.cond_ref_of_op)print_aml_symbol(aml_symbol->component.def_cond_ref_of.cond_ref_of_op);
		if(aml_symbol->component.def_cond_ref_of.super_name)print_aml_symbol(aml_symbol->component.def_cond_ref_of.super_name);
		if(aml_symbol->component.def_cond_ref_of.target)print_aml_symbol(aml_symbol->component.def_cond_ref_of.target);
		break;
	case aml_def_create_bit_field:
		if(aml_symbol->component.def_create_bit_field.create_bit_field_op)print_aml_symbol(aml_symbol->component.def_create_bit_field.create_bit_field_op);
		if(aml_symbol->component.def_create_bit_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_bit_field.source_buff);
		if(aml_symbol->component.def_create_bit_field.byte_index)print_aml_symbol(aml_symbol->component.def_create_bit_field.byte_index);
		if(aml_symbol->component.def_create_bit_field.name_string)print_aml_symbol(aml_symbol->component.def_create_bit_field.name_string);
		break;
	case aml_def_create_byte_field:
		if(aml_symbol->component.def_create_byte_field.create_byte_field_op)print_aml_symbol(aml_symbol->component.def_create_byte_field.create_byte_field_op);
		if(aml_symbol->component.def_create_byte_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_byte_field.source_buff);
		if(aml_symbol->component.def_create_byte_field.byte_index)print_aml_symbol(aml_symbol->component.def_create_byte_field.byte_index);
		if(aml_symbol->component.def_create_byte_field.name_string)print_aml_symbol(aml_symbol->component.def_create_byte_field.name_string);
		break;
	case aml_def_create_dword_field:
		if(aml_symbol->component.def_create_dword_field.create_dword_field_op)print_aml_symbol(aml_symbol->component.def_create_dword_field.create_dword_field_op);
		if(aml_symbol->component.def_create_dword_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_dword_field.source_buff);
		if(aml_symbol->component.def_create_dword_field.byte_index)print_aml_symbol(aml_symbol->component.def_create_dword_field.byte_index);
		if(aml_symbol->component.def_create_dword_field.name_string)print_aml_symbol(aml_symbol->component.def_create_dword_field.name_string);
		break;
	case aml_def_create_field:
		if(aml_symbol->component.def_create_field.create_field_op)print_aml_symbol(aml_symbol->component.def_create_field.create_field_op);
		if(aml_symbol->component.def_create_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_field.source_buff);
		if(aml_symbol->component.def_create_field.bit_index)print_aml_symbol(aml_symbol->component.def_create_field.bit_index);
		if(aml_symbol->component.def_create_field.num_bits)print_aml_symbol(aml_symbol->component.def_create_field.num_bits);
		if(aml_symbol->component.def_create_field.name_string)print_aml_symbol(aml_symbol->component.def_create_field.name_string);
		break;
	case aml_def_create_qword_field:
		if(aml_symbol->component.def_create_qword_field.create_qword_field_op)print_aml_symbol(aml_symbol->component.def_create_qword_field.create_qword_field_op);
		if(aml_symbol->component.def_create_qword_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_qword_field.source_buff);
		if(aml_symbol->component.def_create_qword_field.byte_index)print_aml_symbol(aml_symbol->component.def_create_qword_field.byte_index);
		if(aml_symbol->component.def_create_qword_field.name_string)print_aml_symbol(aml_symbol->component.def_create_qword_field.name_string);
		break;
	case aml_def_create_word_field:
		if(aml_symbol->component.def_create_word_field.create_word_field_op)print_aml_symbol(aml_symbol->component.def_create_word_field.create_word_field_op);
		if(aml_symbol->component.def_create_word_field.source_buff)print_aml_symbol(aml_symbol->component.def_create_word_field.source_buff);
		if(aml_symbol->component.def_create_word_field.byte_index)print_aml_symbol(aml_symbol->component.def_create_word_field.byte_index);
		if(aml_symbol->component.def_create_word_field.name_string)print_aml_symbol(aml_symbol->component.def_create_word_field.name_string);
		break;
	case aml_def_decrement:
		if(aml_symbol->component.def_decrement.decrement_op)print_aml_symbol(aml_symbol->component.def_decrement.decrement_op);
		if(aml_symbol->component.def_decrement.super_name)print_aml_symbol(aml_symbol->component.def_decrement.super_name);
		break;
	case aml_def_deref_of:
		if(aml_symbol->component.def_deref_of.deref_of_op)print_aml_symbol(aml_symbol->component.def_deref_of.deref_of_op);
		if(aml_symbol->component.def_deref_of.obj_reference)print_aml_symbol(aml_symbol->component.def_deref_of.obj_reference);
		break;
	case aml_def_device:
		if(aml_symbol->component.def_device.device_op)print_aml_symbol(aml_symbol->component.def_device.device_op);
		if(aml_symbol->component.def_device.pkg_length)print_aml_symbol(aml_symbol->component.def_device.pkg_length);
		if(aml_symbol->component.def_device.name_string)print_aml_symbol(aml_symbol->component.def_device.name_string);
		if(aml_symbol->component.def_device.term_list)print_aml_symbol(aml_symbol->component.def_device.term_list);
		break;
	case aml_def_divide:
		if(aml_symbol->component.def_divide.divide_op)print_aml_symbol(aml_symbol->component.def_divide.divide_op);
		if(aml_symbol->component.def_divide.dividend)print_aml_symbol(aml_symbol->component.def_divide.dividend);
		if(aml_symbol->component.def_divide.divisor)print_aml_symbol(aml_symbol->component.def_divide.divisor);
		if(aml_symbol->component.def_divide.remainder)print_aml_symbol(aml_symbol->component.def_divide.remainder);
		if(aml_symbol->component.def_divide.quotient)print_aml_symbol(aml_symbol->component.def_divide.quotient);
		break;
	case aml_def_else:
		if(aml_symbol->component.def_else.else_op)print_aml_symbol(aml_symbol->component.def_else.else_op);
		if(aml_symbol->component.def_else.pkg_length)print_aml_symbol(aml_symbol->component.def_else.pkg_length);
		if(aml_symbol->component.def_else.term_list)print_aml_symbol(aml_symbol->component.def_else.term_list);
		break;
	case aml_def_field:
		if(aml_symbol->component.def_field.field_op)print_aml_symbol(aml_symbol->component.def_field.field_op);
		if(aml_symbol->component.def_field.pkg_length)print_aml_symbol(aml_symbol->component.def_field.pkg_length);
		if(aml_symbol->component.def_field.name_string)print_aml_symbol(aml_symbol->component.def_field.name_string);
		if(aml_symbol->component.def_field.field_flags)print_aml_symbol(aml_symbol->component.def_field.field_flags);
		if(aml_symbol->component.def_field.field_list)print_aml_symbol(aml_symbol->component.def_field.field_list);
		break;
	case aml_def_find_set_left_bit:
		if(aml_symbol->component.def_find_set_left_bit.find_set_left_bit_op)print_aml_symbol(aml_symbol->component.def_find_set_left_bit.find_set_left_bit_op);
		if(aml_symbol->component.def_find_set_left_bit.operand)print_aml_symbol(aml_symbol->component.def_find_set_left_bit.operand);
		if(aml_symbol->component.def_find_set_left_bit.target)print_aml_symbol(aml_symbol->component.def_find_set_left_bit.target);
		break;
	case aml_def_find_set_right_bit:
		if(aml_symbol->component.def_find_set_right_bit.find_set_right_bit_op)print_aml_symbol(aml_symbol->component.def_find_set_right_bit.find_set_right_bit_op);
		if(aml_symbol->component.def_find_set_right_bit.operand)print_aml_symbol(aml_symbol->component.def_find_set_right_bit.operand);
		if(aml_symbol->component.def_find_set_right_bit.target)print_aml_symbol(aml_symbol->component.def_find_set_right_bit.target);
		break;
	case aml_def_if_else:
		if(aml_symbol->component.def_if_else.if_op)print_aml_symbol(aml_symbol->component.def_if_else.if_op);
		if(aml_symbol->component.def_if_else.pkg_length)print_aml_symbol(aml_symbol->component.def_if_else.pkg_length);
		if(aml_symbol->component.def_if_else.predicate)print_aml_symbol(aml_symbol->component.def_if_else.predicate);
		if(aml_symbol->component.def_if_else.term_list)print_aml_symbol(aml_symbol->component.def_if_else.term_list);
		if(aml_symbol->component.def_if_else.def_else)print_aml_symbol(aml_symbol->component.def_if_else.def_else);
		break;
	case aml_def_increment:
		if(aml_symbol->component.def_increment.increment_op)print_aml_symbol(aml_symbol->component.def_increment.increment_op);
		if(aml_symbol->component.def_increment.super_name)print_aml_symbol(aml_symbol->component.def_increment.super_name);
		break;
	case aml_def_index:
		if(aml_symbol->component.def_index.index_op)print_aml_symbol(aml_symbol->component.def_index.index_op);
		if(aml_symbol->component.def_index.buff_pkg_str_obj)print_aml_symbol(aml_symbol->component.def_index.buff_pkg_str_obj);
		if(aml_symbol->component.def_index.index_value)print_aml_symbol(aml_symbol->component.def_index.index_value);
		if(aml_symbol->component.def_index.target)print_aml_symbol(aml_symbol->component.def_index.target);
		break;
	case aml_def_index_field:
		if(aml_symbol->component.def_index_field.index_field_op)print_aml_symbol(aml_symbol->component.def_index_field.index_field_op);
		if(aml_symbol->component.def_index_field.pkg_length)print_aml_symbol(aml_symbol->component.def_index_field.pkg_length);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_index_field.name_string); i++)if(aml_symbol->component.def_index_field.name_string[i])print_aml_symbol(aml_symbol->component.def_index_field.name_string[i]);
		if(aml_symbol->component.def_index_field.field_flags)print_aml_symbol(aml_symbol->component.def_index_field.field_flags);
		if(aml_symbol->component.def_index_field.field_list)print_aml_symbol(aml_symbol->component.def_index_field.field_list);
		break;
	case aml_def_l_and:
		if(aml_symbol->component.def_l_and.l_and_op)print_aml_symbol(aml_symbol->component.def_l_and.l_and_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_and.operand); i++)if(aml_symbol->component.def_l_and.operand[i])print_aml_symbol(aml_symbol->component.def_l_and.operand[i]);
		break;
	case aml_def_l_equal:
		if(aml_symbol->component.def_l_equal.l_equal_op)print_aml_symbol(aml_symbol->component.def_l_equal.l_equal_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_equal.operand); i++)if(aml_symbol->component.def_l_equal.operand[i])print_aml_symbol(aml_symbol->component.def_l_equal.operand[i]);
		break;
	case aml_def_l_greater:
		if(aml_symbol->component.def_l_greater.l_greater_op)print_aml_symbol(aml_symbol->component.def_l_greater.l_greater_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_greater.operand); i++)if(aml_symbol->component.def_l_greater.operand[i])print_aml_symbol(aml_symbol->component.def_l_greater.operand[i]);
		break;
	case aml_def_l_less:
		if(aml_symbol->component.def_l_less.l_less_op)print_aml_symbol(aml_symbol->component.def_l_less.l_less_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_less.operand); i++)if(aml_symbol->component.def_l_less.operand[i])print_aml_symbol(aml_symbol->component.def_l_less.operand[i]);
		break;
	case aml_def_l_not:
		if(aml_symbol->component.def_l_not.l_not_op)print_aml_symbol(aml_symbol->component.def_l_not.l_not_op);
		if(aml_symbol->component.def_l_not.operand)print_aml_symbol(aml_symbol->component.def_l_not.operand);
		break;
	case aml_def_l_or:
		if(aml_symbol->component.def_l_or.l_or_op)print_aml_symbol(aml_symbol->component.def_l_or.l_or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_l_or.operand); i++)if(aml_symbol->component.def_l_or.operand[i])print_aml_symbol(aml_symbol->component.def_l_or.operand[i]);
		break;
	case aml_def_match:
		if(aml_symbol->component.def_match.match_op)print_aml_symbol(aml_symbol->component.def_match.match_op);
		if(aml_symbol->component.def_match.search_pkg)print_aml_symbol(aml_symbol->component.def_match.search_pkg);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_match.match_opcode); i++)if(aml_symbol->component.def_match.match_opcode)print_aml_symbol(aml_symbol->component.def_match.match_opcode[i]);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_match.operand); i++)if(aml_symbol->component.def_match.operand)print_aml_symbol(aml_symbol->component.def_match.operand[i]);
		if(aml_symbol->component.def_match.start_index)print_aml_symbol(aml_symbol->component.def_match.start_index);
		break;
	case aml_def_method:
		if(aml_symbol->component.def_method.method_op)print_aml_symbol(aml_symbol->component.def_method.method_op);
		if(aml_symbol->component.def_method.pkg_length)print_aml_symbol(aml_symbol->component.def_method.pkg_length);
		if(aml_symbol->component.def_method.name_string)print_aml_symbol(aml_symbol->component.def_method.name_string);
		if(aml_symbol->component.def_method.method_flags)print_aml_symbol(aml_symbol->component.def_method.method_flags);
		if(aml_symbol->component.def_method.term_list)print_aml_symbol(aml_symbol->component.def_method.term_list);
		break;
	case aml_def_mid:
		if(aml_symbol->component.def_mid.mid_op)print_aml_symbol(aml_symbol->component.def_mid.mid_op);
		if(aml_symbol->component.def_mid.mid_obj)print_aml_symbol(aml_symbol->component.def_mid.mid_obj);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_mid.term_arg); i++)if(aml_symbol->component.def_mid.term_arg[i])print_aml_symbol(aml_symbol->component.def_mid.term_arg[i]);
		if(aml_symbol->component.def_mid.target)print_aml_symbol(aml_symbol->component.def_mid.target);
		break;
	case aml_def_multiply:
		if(aml_symbol->component.def_multiply.multiply_op)print_aml_symbol(aml_symbol->component.def_multiply.multiply_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_multiply.operand); i++)if(aml_symbol->component.def_multiply.operand[i])print_aml_symbol(aml_symbol->component.def_multiply.operand[i]);
		if(aml_symbol->component.def_multiply.target)print_aml_symbol(aml_symbol->component.def_multiply.target);
		break;
	case aml_def_mutex:
		if(aml_symbol->component.def_mutex.mutex_op)print_aml_symbol(aml_symbol->component.def_mutex.mutex_op);
		if(aml_symbol->component.def_mutex.name_string)print_aml_symbol(aml_symbol->component.def_mutex.name_string);
		if(aml_symbol->component.def_mutex.sync_flags)print_aml_symbol(aml_symbol->component.def_mutex.sync_flags);
		break;
	case aml_def_name:
		if(aml_symbol->component.def_name.name_op)print_aml_symbol(aml_symbol->component.def_name.name_op);
		if(aml_symbol->component.def_name.name_string)print_aml_symbol(aml_symbol->component.def_name.name_string);
		if(aml_symbol->component.def_name.data_ref_object)print_aml_symbol(aml_symbol->component.def_name.data_ref_object);
		break;
	case aml_def_not:
		if(aml_symbol->component.def_not.not_op)print_aml_symbol(aml_symbol->component.def_not.not_op);
		if(aml_symbol->component.def_not.operand)print_aml_symbol(aml_symbol->component.def_not.operand);
		if(aml_symbol->component.def_not.target)print_aml_symbol(aml_symbol->component.def_not.target);
		break;
	case aml_def_notify:
		if(aml_symbol->component.def_notify.notify_op)print_aml_symbol(aml_symbol->component.def_notify.notify_op);
		if(aml_symbol->component.def_notify.notify_object)print_aml_symbol(aml_symbol->component.def_notify.notify_object);
		if(aml_symbol->component.def_notify.notify_value)print_aml_symbol(aml_symbol->component.def_notify.notify_value);
		break;
	case aml_def_object_type:
		if(aml_symbol->component.def_object_type.object_type_op)print_aml_symbol(aml_symbol->component.def_object_type.object_type_op);
		if(aml_symbol->component.def_object_type.simple_name)print_aml_symbol(aml_symbol->component.def_object_type.simple_name);
		if(aml_symbol->component.def_object_type.debug_obj)print_aml_symbol(aml_symbol->component.def_object_type.debug_obj);
		if(aml_symbol->component.def_object_type.def_ref_of)print_aml_symbol(aml_symbol->component.def_object_type.def_ref_of);
		if(aml_symbol->component.def_object_type.def_deref_of)print_aml_symbol(aml_symbol->component.def_object_type.def_deref_of);
		if(aml_symbol->component.def_object_type.def_index)print_aml_symbol(aml_symbol->component.def_object_type.def_index);
		break;
	case aml_def_op_region:
		if(aml_symbol->component.def_op_region.op_region_op)print_aml_symbol(aml_symbol->component.def_op_region.op_region_op);
		if(aml_symbol->component.def_op_region.name_string)print_aml_symbol(aml_symbol->component.def_op_region.name_string);
		if(aml_symbol->component.def_op_region.region_space)print_aml_symbol(aml_symbol->component.def_op_region.region_space);
		if(aml_symbol->component.def_op_region.region_offset)print_aml_symbol(aml_symbol->component.def_op_region.region_offset);
		if(aml_symbol->component.def_op_region.region_len)print_aml_symbol(aml_symbol->component.def_op_region.region_len);
		break;
	case aml_def_or:
		if(aml_symbol->component.def_or.or_op)print_aml_symbol(aml_symbol->component.def_or.or_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_or.operand); i++)if(aml_symbol->component.def_or.operand[i])print_aml_symbol(aml_symbol->component.def_or.operand[i]);
		if(aml_symbol->component.def_or.target)print_aml_symbol(aml_symbol->component.def_or.target);
		break;
	case aml_def_package:
		if(aml_symbol->component.def_package.package_op)print_aml_symbol(aml_symbol->component.def_package.package_op);
		if(aml_symbol->component.def_package.pkg_length)print_aml_symbol(aml_symbol->component.def_package.pkg_length);
		if(aml_symbol->component.def_package.num_elements)print_aml_symbol(aml_symbol->component.def_package.num_elements);
		if(aml_symbol->component.def_package.package_element_list)print_aml_symbol(aml_symbol->component.def_package.package_element_list);
		break;
	case aml_def_processor:
		if(aml_symbol->component.def_processor.processor_op)print_aml_symbol(aml_symbol->component.def_processor.processor_op);
		if(aml_symbol->component.def_processor.pkg_length)print_aml_symbol(aml_symbol->component.def_processor.pkg_length);
		if(aml_symbol->component.def_processor.name_string)print_aml_symbol(aml_symbol->component.def_processor.name_string);
		if(aml_symbol->component.def_processor.proc_id)print_aml_symbol(aml_symbol->component.def_processor.proc_id);
		if(aml_symbol->component.def_processor.pblk_addr)print_aml_symbol(aml_symbol->component.def_processor.pblk_addr);
		if(aml_symbol->component.def_processor.pblk_len)print_aml_symbol(aml_symbol->component.def_processor.pblk_len);
		if(aml_symbol->component.def_processor.term_list)print_aml_symbol(aml_symbol->component.def_processor.term_list);
		break;
	case aml_def_ref_of:
		if(aml_symbol->component.def_ref_of.ref_of_op)print_aml_symbol(aml_symbol->component.def_ref_of.ref_of_op);
		if(aml_symbol->component.def_ref_of.super_name)print_aml_symbol(aml_symbol->component.def_ref_of.super_name);
		break;
	case aml_def_release:
		if(aml_symbol->component.def_release.release_op)print_aml_symbol(aml_symbol->component.def_release.release_op);
		if(aml_symbol->component.def_release.mutex_object)print_aml_symbol(aml_symbol->component.def_release.mutex_object);
		break;
	case aml_def_return:
		if(aml_symbol->component.def_return.return_op)print_aml_symbol(aml_symbol->component.def_return.return_op);
		if(aml_symbol->component.def_return.arg_object)print_aml_symbol(aml_symbol->component.def_return.arg_object);
		break;
	case aml_def_scope:
		if(aml_symbol->component.def_scope.scope_op)print_aml_symbol(aml_symbol->component.def_scope.scope_op);
		if(aml_symbol->component.def_scope.pkg_length)print_aml_symbol(aml_symbol->component.def_scope.pkg_length);
		if(aml_symbol->component.def_scope.name_string)print_aml_symbol(aml_symbol->component.def_scope.name_string);
		if(aml_symbol->component.def_scope.term_list)print_aml_symbol(aml_symbol->component.def_scope.term_list);
		break;
	case aml_def_shift_left:
		if(aml_symbol->component.def_shift_left.shift_left_op)print_aml_symbol(aml_symbol->component.def_shift_left.shift_left_op);
		if(aml_symbol->component.def_shift_left.operand)print_aml_symbol(aml_symbol->component.def_shift_left.operand);
		if(aml_symbol->component.def_shift_left.shift_count)print_aml_symbol(aml_symbol->component.def_shift_left.shift_count);
		if(aml_symbol->component.def_shift_left.target)print_aml_symbol(aml_symbol->component.def_shift_left.target);
		break;
	case aml_def_shift_right:
		if(aml_symbol->component.def_shift_right.shift_right_op)print_aml_symbol(aml_symbol->component.def_shift_right.shift_right_op);
		if(aml_symbol->component.def_shift_right.operand)print_aml_symbol(aml_symbol->component.def_shift_right.operand);
		if(aml_symbol->component.def_shift_right.shift_count)print_aml_symbol(aml_symbol->component.def_shift_right.shift_count);
		if(aml_symbol->component.def_shift_right.target)print_aml_symbol(aml_symbol->component.def_shift_right.target);
		break;
	case aml_def_size_of:
		if(aml_symbol->component.def_size_of.size_of_op)print_aml_symbol(aml_symbol->component.def_size_of.size_of_op);
		if(aml_symbol->component.def_size_of.super_name)print_aml_symbol(aml_symbol->component.def_size_of.super_name);
		break;
	case aml_def_sleep:
		if(aml_symbol->component.def_sleep.sleep_op)print_aml_symbol(aml_symbol->component.def_sleep.sleep_op);
		if(aml_symbol->component.def_sleep.msec_time)print_aml_symbol(aml_symbol->component.def_sleep.msec_time);
		break;
	case aml_def_stall:
		if(aml_symbol->component.def_stall.stall_op)print_aml_symbol(aml_symbol->component.def_stall.stall_op);
		if(aml_symbol->component.def_stall.usec_time)print_aml_symbol(aml_symbol->component.def_stall.usec_time);
		break;
	case aml_def_store:
		if(aml_symbol->component.def_store.store_op)print_aml_symbol(aml_symbol->component.def_store.store_op);
		if(aml_symbol->component.def_store.term_arg)print_aml_symbol(aml_symbol->component.def_store.term_arg);
		if(aml_symbol->component.def_store.super_name)print_aml_symbol(aml_symbol->component.def_store.super_name);
		break;
	case aml_def_subtract:
		if(aml_symbol->component.def_subtract.subtract_op)print_aml_symbol(aml_symbol->component.def_subtract.subtract_op);
		for(unsigned i = 0; i < _countof(aml_symbol->component.def_subtract.operand); i++)if(aml_symbol->component.def_subtract.operand[i])print_aml_symbol(aml_symbol->component.def_subtract.operand[i]);
		if(aml_symbol->component.def_subtract.target)print_aml_symbol(aml_symbol->component.def_subtract.target);
		break;
	case aml_def_thermal_zone:
		if(aml_symbol->component.def_thermal_zone.thermal_zone_op)print_aml_symbol(aml_symbol->component.def_thermal_zone.thermal_zone_op);
		if(aml_symbol->component.def_thermal_zone.pkg_length)print_aml_symbol(aml_symbol->component.def_thermal_zone.pkg_length);
		if(aml_symbol->component.def_thermal_zone.name_string)print_aml_symbol(aml_symbol->component.def_thermal_zone.name_string);
		if(aml_symbol->component.def_thermal_zone.term_list)print_aml_symbol(aml_symbol->component.def_thermal_zone.term_list);
		break;
	case aml_def_to_buffer:
		if(aml_symbol->component.def_to_buffer.to_buffer_op)print_aml_symbol(aml_symbol->component.def_to_buffer.to_buffer_op);
		if(aml_symbol->component.def_to_buffer.operand)print_aml_symbol(aml_symbol->component.def_to_buffer.operand);
		if(aml_symbol->component.def_to_buffer.target)print_aml_symbol(aml_symbol->component.def_to_buffer.target);
		break;
	case aml_def_to_hex_string:
		if(aml_symbol->component.def_to_hex_string.to_hex_string_op)print_aml_symbol(aml_symbol->component.def_to_hex_string.to_hex_string_op);
		if(aml_symbol->component.def_to_hex_string.operand)print_aml_symbol(aml_symbol->component.def_to_hex_string.operand);
		if(aml_symbol->component.def_to_hex_string.target)print_aml_symbol(aml_symbol->component.def_to_hex_string.target);
		break;
	case aml_def_to_integer:
		if(aml_symbol->component.def_to_integer.to_integer_op)print_aml_symbol(aml_symbol->component.def_to_integer.to_integer_op);
		if(aml_symbol->component.def_to_integer.operand)print_aml_symbol(aml_symbol->component.def_to_integer.operand);
		if(aml_symbol->component.def_to_integer.target)print_aml_symbol(aml_symbol->component.def_to_integer.target);
		break;
	case aml_def_xor:
		if(aml_symbol->component.def_xor.xor_op)print_aml_symbol(aml_symbol->component.def_xor.xor_op);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.def_xor.operand); i++)if(aml_symbol->component.def_xor.operand[i])print_aml_symbol(aml_symbol->component.def_xor.operand[i]);
		if(aml_symbol->component.def_xor.target)print_aml_symbol(aml_symbol->component.def_xor.target);
		break;
	case aml_def_while:
		if(aml_symbol->component.def_while.while_op)print_aml_symbol(aml_symbol->component.def_while.while_op);
		if(aml_symbol->component.def_while.pkg_length)print_aml_symbol(aml_symbol->component.def_while.pkg_length);
		if(aml_symbol->component.def_while.predicate)print_aml_symbol(aml_symbol->component.def_while.predicate);
		if(aml_symbol->component.def_while.term_list)print_aml_symbol(aml_symbol->component.def_while.term_list);
		break;
	case aml_deref_of_op:
		break;
	case aml_device_op:
		if(aml_symbol->component.device_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.device_op.ext_op_prefix);
		if(aml_symbol->component.device_op.device_op_suffix)print_aml_symbol(aml_symbol->component.device_op.device_op_suffix);
		break;
	case aml_device_op_suffix:
		break;
	case aml_digit_char:
		break;
	case aml_divide_op:
		break;
	case aml_dividend:
		if(aml_symbol->component.dividend.term_arg)print_aml_symbol(aml_symbol->component.dividend.term_arg);
		break;
	case aml_divisor:
		if(aml_symbol->component.divisor.term_arg)print_aml_symbol(aml_symbol->component.divisor.term_arg);
		break;
	case aml_dual_name_path:
		if(aml_symbol->component.dual_name_path.dual_name_prefix)print_aml_symbol(aml_symbol->component.dual_name_path.dual_name_prefix);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dual_name_path.name_seg); i++)if(aml_symbol->component.dual_name_path.name_seg[i])print_aml_symbol(aml_symbol->component.dual_name_path.name_seg[i]);
		break;
	case aml_dual_name_prefix:
		break;
	case aml_dword_const:
		if(aml_symbol->component.dword_const.dword_prefix)print_aml_symbol(aml_symbol->component.dword_const.dword_prefix);
		if(aml_symbol->component.dword_const.dword_data)print_aml_symbol(aml_symbol->component.dword_const.dword_data);
		break;
	case aml_dword_data:
		for(unsigned int i = 0; i < _countof(aml_symbol->component.dword_data.word_data); i++)if(aml_symbol->component.dword_data.word_data[i])print_aml_symbol(aml_symbol->component.dword_data.word_data[i]);
		break;
	case aml_dword_prefix:
		break;
	case aml_else_op:
		break;
	case aml_expression_opcode:
		if(aml_symbol->component.expression_opcode.def_add)print_aml_symbol(aml_symbol->component.expression_opcode.def_add);
		if(aml_symbol->component.expression_opcode.def_and)print_aml_symbol(aml_symbol->component.expression_opcode.def_and);
		if(aml_symbol->component.expression_opcode.def_acquire)print_aml_symbol(aml_symbol->component.expression_opcode.def_acquire);
		if(aml_symbol->component.expression_opcode.def_buffer)print_aml_symbol(aml_symbol->component.expression_opcode.def_buffer);
		if(aml_symbol->component.expression_opcode.def_concat)print_aml_symbol(aml_symbol->component.expression_opcode.def_concat);
		if(aml_symbol->component.expression_opcode.def_concat_res)print_aml_symbol(aml_symbol->component.expression_opcode.def_concat_res);
		if(aml_symbol->component.expression_opcode.def_cond_ref_of)print_aml_symbol(aml_symbol->component.expression_opcode.def_cond_ref_of);
		if(aml_symbol->component.expression_opcode.def_copy_object)print_aml_symbol(aml_symbol->component.expression_opcode.def_copy_object);
		if(aml_symbol->component.expression_opcode.def_decrement)print_aml_symbol(aml_symbol->component.expression_opcode.def_decrement);
		if(aml_symbol->component.expression_opcode.def_deref_of)print_aml_symbol(aml_symbol->component.expression_opcode.def_deref_of);
		if(aml_symbol->component.expression_opcode.def_divide)print_aml_symbol(aml_symbol->component.expression_opcode.def_divide);
		if(aml_symbol->component.expression_opcode.def_find_set_left_bit)print_aml_symbol(aml_symbol->component.expression_opcode.def_find_set_left_bit);
		if(aml_symbol->component.expression_opcode.def_find_set_right_bit)print_aml_symbol(aml_symbol->component.expression_opcode.def_find_set_right_bit);
		if(aml_symbol->component.expression_opcode.def_from_bcd)print_aml_symbol(aml_symbol->component.expression_opcode.def_from_bcd);
		if(aml_symbol->component.expression_opcode.def_increment)print_aml_symbol(aml_symbol->component.expression_opcode.def_increment);
		if(aml_symbol->component.expression_opcode.def_index)print_aml_symbol(aml_symbol->component.expression_opcode.def_index);
		if(aml_symbol->component.expression_opcode.def_l_and)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_and);
		if(aml_symbol->component.expression_opcode.def_l_equal)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_equal);
		if(aml_symbol->component.expression_opcode.def_l_greater)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_greater);
		if(aml_symbol->component.expression_opcode.def_l_greater_equal)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_greater_equal);
		if(aml_symbol->component.expression_opcode.def_l_less)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_less);
		if(aml_symbol->component.expression_opcode.def_l_less_equal)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_less_equal);
		if(aml_symbol->component.expression_opcode.def_l_not)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_not);
		if(aml_symbol->component.expression_opcode.def_l_not_equal)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_not_equal);
		if(aml_symbol->component.expression_opcode.def_l_or)print_aml_symbol(aml_symbol->component.expression_opcode.def_l_or);
		if(aml_symbol->component.expression_opcode.def_load_table)print_aml_symbol(aml_symbol->component.expression_opcode.def_load_table);
		if(aml_symbol->component.expression_opcode.def_match)print_aml_symbol(aml_symbol->component.expression_opcode.def_match);
		if(aml_symbol->component.expression_opcode.def_mid)print_aml_symbol(aml_symbol->component.expression_opcode.def_mid);
		if(aml_symbol->component.expression_opcode.def_mod)print_aml_symbol(aml_symbol->component.expression_opcode.def_mod);
		if(aml_symbol->component.expression_opcode.def_multiply)print_aml_symbol(aml_symbol->component.expression_opcode.def_multiply);
		if(aml_symbol->component.expression_opcode.def_nand)print_aml_symbol(aml_symbol->component.expression_opcode.def_nand);
		if(aml_symbol->component.expression_opcode.def_nor)print_aml_symbol(aml_symbol->component.expression_opcode.def_nor);
		if(aml_symbol->component.expression_opcode.def_not)print_aml_symbol(aml_symbol->component.expression_opcode.def_not);
		if(aml_symbol->component.expression_opcode.def_object_type)print_aml_symbol(aml_symbol->component.expression_opcode.def_object_type);
		if(aml_symbol->component.expression_opcode.def_or)print_aml_symbol(aml_symbol->component.expression_opcode.def_or);
		if(aml_symbol->component.expression_opcode.def_package)print_aml_symbol(aml_symbol->component.expression_opcode.def_package);
		if(aml_symbol->component.expression_opcode.def_ref_of)print_aml_symbol(aml_symbol->component.expression_opcode.def_ref_of);
		if(aml_symbol->component.expression_opcode.def_shift_left)print_aml_symbol(aml_symbol->component.expression_opcode.def_shift_left);
		if(aml_symbol->component.expression_opcode.def_shift_right)print_aml_symbol(aml_symbol->component.expression_opcode.def_shift_right);
		if(aml_symbol->component.expression_opcode.def_size_of)print_aml_symbol(aml_symbol->component.expression_opcode.def_size_of);
		if(aml_symbol->component.expression_opcode.def_store)print_aml_symbol(aml_symbol->component.expression_opcode.def_store);
		if(aml_symbol->component.expression_opcode.def_subtract)print_aml_symbol(aml_symbol->component.expression_opcode.def_subtract);
		if(aml_symbol->component.expression_opcode.def_timer)print_aml_symbol(aml_symbol->component.expression_opcode.def_timer);
		if(aml_symbol->component.expression_opcode.def_to_bcd)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_bcd);
		if(aml_symbol->component.expression_opcode.def_to_buffer)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_buffer);
		if(aml_symbol->component.expression_opcode.def_to_decimal_string)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_decimal_string);
		if(aml_symbol->component.expression_opcode.def_to_hex_string)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_hex_string);
		if(aml_symbol->component.expression_opcode.def_to_integer)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_integer);
		if(aml_symbol->component.expression_opcode.def_to_string)print_aml_symbol(aml_symbol->component.expression_opcode.def_to_string);
		if(aml_symbol->component.expression_opcode.def_var_package)print_aml_symbol(aml_symbol->component.expression_opcode.def_var_package);
		if(aml_symbol->component.expression_opcode.def_wait)print_aml_symbol(aml_symbol->component.expression_opcode.def_wait);
		if(aml_symbol->component.expression_opcode.def_xor)print_aml_symbol(aml_symbol->component.expression_opcode.def_xor);
		if(aml_symbol->component.expression_opcode.method_invocation)print_aml_symbol(aml_symbol->component.expression_opcode.method_invocation);
		break;
	case aml_ext_op_prefix:
		break;
	case aml_field_element:
		if(aml_symbol->component.field_element.named_field)print_aml_symbol(aml_symbol->component.field_element.named_field);
		if(aml_symbol->component.field_element.reserved_field)print_aml_symbol(aml_symbol->component.field_element.reserved_field);
		if(aml_symbol->component.field_element.access_field)print_aml_symbol(aml_symbol->component.field_element.access_field);
		if(aml_symbol->component.field_element.extended_access_field)print_aml_symbol(aml_symbol->component.field_element.extended_access_field);
		if(aml_symbol->component.field_element.connect_field)print_aml_symbol(aml_symbol->component.field_element.connect_field);
		break;
	case aml_field_flags:
		break;
	case aml_field_list:
		if(aml_symbol->component.field_list.field_element)print_aml_symbol(aml_symbol->component.field_list.field_element);
		if(aml_symbol->component.field_list.field_list)print_aml_symbol(aml_symbol->component.field_list.field_list);
		break;
	case aml_field_op:
		if(aml_symbol->component.field_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.field_op.ext_op_prefix);
		if(aml_symbol->component.field_op.field_op_suffix)print_aml_symbol(aml_symbol->component.field_op.field_op_suffix);
		break;
	case aml_field_op_suffix:
		break;
	case aml_find_set_left_bit_op:
		break;
	case aml_find_set_right_bit_op:
		break;
	case aml_if_op:
		break;
	case aml_increment_op:
		break;
	case aml_index_field_op:
		if(aml_symbol->component.index_field_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.index_field_op.ext_op_prefix);
		if(aml_symbol->component.index_field_op.index_field_op_suffix)print_aml_symbol(aml_symbol->component.index_field_op.index_field_op_suffix);
		break;
	case aml_index_field_op_suffix:
	case aml_index_op:
		break;
	case aml_index_value:
		if(aml_symbol->component.index_value.term_arg)print_aml_symbol(aml_symbol->component.index_value.term_arg);
		break;
	case aml_lead_name_char:
		break;
	case aml_l_and_op:
		break;
	case aml_l_equal_op:
		break;
	case aml_l_greater_op:
		break;
	case aml_l_less_op:
		break;
	case aml_l_not_op:
		break;
	case aml_l_or_op:
		break;
	case aml_local_obj:
		if(aml_symbol->component.local_obj.local_op)print_aml_symbol(aml_symbol->component.local_obj.local_op);
		break;
	case aml_local_op:
		break;
	case aml_match_op:
		break;
	case aml_match_opcode:
		break;
	case aml_method_flags:
		break;
	case aml_method_invocation:
		if(aml_symbol->component.method_invocation.name_string)print_aml_symbol(aml_symbol->component.method_invocation.name_string);
		if(aml_symbol->component.method_invocation.term_arg_list)print_aml_symbol(aml_symbol->component.method_invocation.term_arg_list);
		break;
	case aml_method_op:
		break;
	case aml_mid_obj:
		if(aml_symbol->component.mid_obj.term_arg)print_aml_symbol(aml_symbol->component.mid_obj.term_arg);
		break;
	case aml_mid_op:
		break;
	case aml_msec_time:
		if(aml_symbol->component.msec_time.term_arg)print_aml_symbol(aml_symbol->component.msec_time.term_arg);
		break;
	case aml_multi_name_path:
		if(aml_symbol->component.multi_name_path.multi_name_prefix)print_aml_symbol(aml_symbol->component.multi_name_path.multi_name_prefix);
		if(aml_symbol->component.multi_name_path.seg_count)print_aml_symbol(aml_symbol->component.multi_name_path.seg_count);
		for(unsigned int i = 0; i < *aml_symbol->component.multi_name_path.seg_count->string.initial; i++)if(aml_symbol->component.multi_name_path.name_seg[i])print_aml_symbol(aml_symbol->component.multi_name_path.name_seg[i]);
		break;
	case aml_multi_name_prefix:
		break;
	case aml_multiply_op:
		break;
	case aml_mutex_object:
		if(aml_symbol->component.mutex_object.super_name)print_aml_symbol(aml_symbol->component.mutex_object.super_name);
		break;
	case aml_mutex_op:
		if(aml_symbol->component.mutex_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.mutex_op.ext_op_prefix);
		if(aml_symbol->component.mutex_op.mutex_op_suffix)print_aml_symbol(aml_symbol->component.mutex_op.mutex_op_suffix);
		break;
	case aml_mutex_op_suffix:
		break;
	case aml_name_char:
		if(aml_symbol->component.name_char.digit_char)print_aml_symbol(aml_symbol->component.name_char.digit_char);
		if(aml_symbol->component.name_char.lead_name_char)print_aml_symbol(aml_symbol->component.name_char.lead_name_char);
		break;
	case aml_name_op:
		break;
	case aml_name_path:
		if(aml_symbol->component.name_path.name_seg)print_aml_symbol(aml_symbol->component.name_path.name_seg);
		if(aml_symbol->component.name_path.dual_name_path)print_aml_symbol(aml_symbol->component.name_path.dual_name_path);
		if(aml_symbol->component.name_path.multi_name_path)print_aml_symbol(aml_symbol->component.name_path.multi_name_path);
		if(aml_symbol->component.name_path.null_name)print_aml_symbol(aml_symbol->component.name_path.null_name);
		break;
	case aml_name_seg:
		if(aml_symbol->component.name_seg.lead_name_char)print_aml_symbol(aml_symbol->component.name_seg.lead_name_char);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.name_seg.name_char); i++)if(aml_symbol->component.name_seg.name_char[i])print_aml_symbol(aml_symbol->component.name_seg.name_char[i]);
		break;
	case aml_name_space_modifier_obj:
		if(aml_symbol->component.name_space_modifier_obj.def_alias)print_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_alias);
		if(aml_symbol->component.name_space_modifier_obj.def_name)print_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_name);
		if(aml_symbol->component.name_space_modifier_obj.def_scope)print_aml_symbol(aml_symbol->component.name_space_modifier_obj.def_scope);
		break;
	case aml_name_string:
		if(aml_symbol->component.name_string.prefix_path)print_aml_symbol(aml_symbol->component.name_string.prefix_path);
		if(aml_symbol->component.name_string.root_char)print_aml_symbol(aml_symbol->component.name_string.root_char);
		if(aml_symbol->component.name_string.name_path)print_aml_symbol(aml_symbol->component.name_string.name_path);
		break;
	case aml_named_field:
		if(aml_symbol->component.named_field.name_seg)print_aml_symbol(aml_symbol->component.named_field.name_seg);
		if(aml_symbol->component.named_field.pkg_length)print_aml_symbol(aml_symbol->component.named_field.pkg_length);
		break;
	case aml_named_obj:
		if(aml_symbol->component.named_obj.def_bank_field)print_aml_symbol(aml_symbol->component.named_obj.def_bank_field);
		if(aml_symbol->component.named_obj.def_create_bit_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_bit_field);
		if(aml_symbol->component.named_obj.def_create_byte_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_byte_field);
		if(aml_symbol->component.named_obj.def_create_dword_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_dword_field);
		if(aml_symbol->component.named_obj.def_create_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_field);
		if(aml_symbol->component.named_obj.def_create_qword_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_qword_field);
		if(aml_symbol->component.named_obj.def_create_word_field)print_aml_symbol(aml_symbol->component.named_obj.def_create_word_field);
		if(aml_symbol->component.named_obj.def_data_region)print_aml_symbol(aml_symbol->component.named_obj.def_data_region);
		if(aml_symbol->component.named_obj.def_device)print_aml_symbol(aml_symbol->component.named_obj.def_device);
		if(aml_symbol->component.named_obj.def_external)print_aml_symbol(aml_symbol->component.named_obj.def_external);
		if(aml_symbol->component.named_obj.def_field)print_aml_symbol(aml_symbol->component.named_obj.def_field);
		if(aml_symbol->component.named_obj.def_index_field)print_aml_symbol(aml_symbol->component.named_obj.def_index_field);
		if(aml_symbol->component.named_obj.def_method)print_aml_symbol(aml_symbol->component.named_obj.def_method);
		if(aml_symbol->component.named_obj.def_mutex)print_aml_symbol(aml_symbol->component.named_obj.def_mutex);
		if(aml_symbol->component.named_obj.def_op_region)print_aml_symbol(aml_symbol->component.named_obj.def_op_region);
		if(aml_symbol->component.named_obj.def_power_res)print_aml_symbol(aml_symbol->component.named_obj.def_power_res);
		if(aml_symbol->component.named_obj.def_processor)print_aml_symbol(aml_symbol->component.named_obj.def_processor);
		if(aml_symbol->component.named_obj.def_thermal_zone)print_aml_symbol(aml_symbol->component.named_obj.def_thermal_zone);
		break;
	case aml_not_op:
		break;
	case aml_notify_object:
		if(aml_symbol->component.notify_object.super_name)print_aml_symbol(aml_symbol->component.notify_object.super_name);
		break;
	case aml_notify_value:
		if(aml_symbol->component.notify_value.term_arg)print_aml_symbol(aml_symbol->component.notify_value.term_arg);
		break;
	case aml_notify_op:
		break;
	case aml_null_char:
		break;
	case aml_null_name:
		break;
	case aml_num_bits:
		if(aml_symbol->component.num_bits.term_arg)print_aml_symbol(aml_symbol->component.num_bits.term_arg);
		break;
	case aml_num_elements:
		if(aml_symbol->component.num_elements.byte_data)print_aml_symbol(aml_symbol->component.num_elements.byte_data);
		break;
	case aml_obj_reference:
		if(aml_symbol->component.obj_reference.term_arg)print_aml_symbol(aml_symbol->component.obj_reference.term_arg);
		break;
	case aml_object:
		if(aml_symbol->component.object.named_obj)print_aml_symbol(aml_symbol->component.object.named_obj);
		if(aml_symbol->component.object.name_space_modifier_obj)print_aml_symbol(aml_symbol->component.object.name_space_modifier_obj);
		break;
	case aml_object_type_op:
		break;
	case aml_one_op:
		break;
	case aml_ones_op:
		break;
	case aml_op_region_op:
		if(aml_symbol->component.op_region_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.op_region_op.ext_op_prefix);
		if(aml_symbol->component.op_region_op.op_region_op_suffix)print_aml_symbol(aml_symbol->component.op_region_op.op_region_op_suffix);
		break;
	case aml_op_region_op_suffix:
		break;
	case aml_operand:
		if(aml_symbol->component.operand.term_arg)print_aml_symbol(aml_symbol->component.operand.term_arg);
		break;
	case aml_or_op:
		break;
	case aml_package_element:
		if(aml_symbol->component.package_element.data_ref_object)print_aml_symbol(aml_symbol->component.package_element.data_ref_object);
		if(aml_symbol->component.package_element.name_string)print_aml_symbol(aml_symbol->component.package_element.name_string);
		break;
	case aml_package_element_list:
		if(aml_symbol->component.package_element_list.package_element)print_aml_symbol(aml_symbol->component.package_element_list.package_element);
		if(aml_symbol->component.package_element_list.package_element_list)print_aml_symbol(aml_symbol->component.package_element_list.package_element_list);
		break;
	case aml_package_op:
		break;
	case aml_parent_prefix_char:
		break;
	case aml_pblk_addr:
		if(aml_symbol->component.pblk_addr.dword_data)print_aml_symbol(aml_symbol->component.pblk_addr.dword_data);
		break;
	case aml_pblk_len:
		if(aml_symbol->component.pblk_len.byte_data)print_aml_symbol(aml_symbol->component.pblk_len.byte_data);
		break;
	case aml_pkg_lead_byte:
		break;
	case aml_pkg_length:
		if(aml_symbol->component.pkg_length.pkg_lead_byte)print_aml_symbol(aml_symbol->component.pkg_length.pkg_lead_byte);
		for(unsigned int i = 0; i < _countof(aml_symbol->component.pkg_length.byte_data); i++)if(aml_symbol->component.pkg_length.byte_data[i])print_aml_symbol(aml_symbol->component.pkg_length.byte_data[i]);
		break;
	case aml_predicate:
		if(aml_symbol->component.predicate.term_arg)print_aml_symbol(aml_symbol->component.predicate.term_arg);
		break;
	case aml_prefix_path:
		if(aml_symbol->component.prefix_path.parent_prefix_char)print_aml_symbol(aml_symbol->component.prefix_path.parent_prefix_char);
		if(aml_symbol->component.prefix_path.prefix_path)print_aml_symbol(aml_symbol->component.prefix_path.prefix_path);
		break;
	case aml_proc_id:
		if(aml_symbol->component.proc_id.byte_data)print_aml_symbol(aml_symbol->component.proc_id.byte_data);
		break;
	case aml_processor_op:
		if(aml_symbol->component.processor_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.processor_op.ext_op_prefix);
		if(aml_symbol->component.processor_op.processor_op_suffix)print_aml_symbol(aml_symbol->component.processor_op.processor_op_suffix);
		break;
	case aml_processor_op_suffix:
		break;
	case aml_quotient:
		if(aml_symbol->component.quotient.term_arg)print_aml_symbol(aml_symbol->component.quotient.term_arg);
		break;
	case aml_qword_const:
		if(aml_symbol->component.qword_const.qword_prefix)print_aml_symbol(aml_symbol->component.qword_const.qword_prefix);
		if(aml_symbol->component.qword_const.qword_data)print_aml_symbol(aml_symbol->component.qword_const.qword_data);
		break;
	case aml_qword_data:
		for(unsigned int i = 0; i < _countof(aml_symbol->component.qword_data.dword_data); i++)if(aml_symbol->component.qword_data.dword_data[i])print_aml_symbol(aml_symbol->component.qword_data.dword_data[i]);
		break;
	case aml_qword_prefix:
		break;
	case aml_ref_of_op:
		break;
	case aml_reference_type_opcode:
		if(aml_symbol->component.reference_type_opcode.def_ref_of)print_aml_symbol(aml_symbol->component.reference_type_opcode.def_ref_of);
		if(aml_symbol->component.reference_type_opcode.def_deref_of)print_aml_symbol(aml_symbol->component.reference_type_opcode.def_deref_of);
		if(aml_symbol->component.reference_type_opcode.def_index)print_aml_symbol(aml_symbol->component.reference_type_opcode.def_index);
		if(aml_symbol->component.reference_type_opcode.user_term_obj)print_aml_symbol(aml_symbol->component.reference_type_opcode.user_term_obj);
		break;
	case aml_region_len:
		if(aml_symbol->component.region_len.term_arg)print_aml_symbol(aml_symbol->component.region_len.term_arg);
		break;
	case aml_region_offset:
		if(aml_symbol->component.region_offset.term_arg)print_aml_symbol(aml_symbol->component.region_offset.term_arg);
		break;
	case aml_region_space:
		break;
	case aml_release_op:
		if(aml_symbol->component.release_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.release_op.ext_op_prefix);
		if(aml_symbol->component.release_op.release_op_suffix)print_aml_symbol(aml_symbol->component.release_op.release_op_suffix);
		break;
	case aml_release_op_suffix:
		break;
	case aml_reserved_field:
		if(aml_symbol->component.reserved_field.reserved_field_op)print_aml_symbol(aml_symbol->component.reserved_field.reserved_field_op);
		if(aml_symbol->component.reserved_field.pkg_length)print_aml_symbol(aml_symbol->component.reserved_field.pkg_length);
		break;
	case aml_reserved_field_op:
		break;
	case aml_remainder:
		if(aml_symbol->component.remainder.term_arg)print_aml_symbol(aml_symbol->component.remainder.term_arg);
		break;
	case aml_return_op:
		break;
	case aml_revision_op:
		if(aml_symbol->component.revision_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.revision_op.ext_op_prefix);
		if(aml_symbol->component.revision_op.revision_op_suffix)print_aml_symbol(aml_symbol->component.revision_op.revision_op_suffix);
		break;
	case aml_revision_op_suffix:
		break;
	case aml_root_char:
		break;
	case aml_scope_op:
		break;
	case aml_search_pkg:
		if(aml_symbol->component.search_pkg.term_arg)print_aml_symbol(aml_symbol->component.search_pkg.term_arg);
		break;
	case aml_seg_count:
		break;
	case aml_shift_count:
		if(aml_symbol->component.shift_count.term_arg)print_aml_symbol(aml_symbol->component.shift_count.term_arg);
		break;
	case aml_shift_left_op:
		break;
	case aml_shift_right_op:
		break;
	case aml_simple_name:
		if(aml_symbol->component.simple_name.name_string)print_aml_symbol(aml_symbol->component.simple_name.name_string);
		if(aml_symbol->component.simple_name.arg_obj)print_aml_symbol(aml_symbol->component.simple_name.arg_obj);
		if(aml_symbol->component.simple_name.local_obj)print_aml_symbol(aml_symbol->component.simple_name.local_obj);
		break;
	case aml_size_of_op:
		break;
	case aml_sleep_op:
		if(aml_symbol->component.sleep_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.sleep_op.ext_op_prefix);
		if(aml_symbol->component.sleep_op.sleep_op_suffix)print_aml_symbol(aml_symbol->component.sleep_op.sleep_op_suffix);
		break;
	case aml_sleep_op_suffix:
		break;
	case aml_source_buff:
		if(aml_symbol->component.source_buff.term_arg)print_aml_symbol(aml_symbol->component.source_buff.term_arg);
		break;
	case aml_stall_op:
		if(aml_symbol->component.stall_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.stall_op.ext_op_prefix);
		if(aml_symbol->component.stall_op.stall_op_suffix)print_aml_symbol(aml_symbol->component.stall_op.stall_op_suffix);
		break;
	case aml_stall_op_suffix:
		break;
	case aml_start_index:
		if(aml_symbol->component.start_index.term_arg)print_aml_symbol(aml_symbol->component.start_index.term_arg);
		break;
	case aml_statement_opcode:
		if(aml_symbol->component.statement_opcode.def_break)print_aml_symbol(aml_symbol->component.statement_opcode.def_break);
		if(aml_symbol->component.statement_opcode.def_break_point)print_aml_symbol(aml_symbol->component.statement_opcode.def_break_point);
		if(aml_symbol->component.statement_opcode.def_continue)print_aml_symbol(aml_symbol->component.statement_opcode.def_continue);
		if(aml_symbol->component.statement_opcode.def_fatal)print_aml_symbol(aml_symbol->component.statement_opcode.def_fatal);
		if(aml_symbol->component.statement_opcode.def_if_else)print_aml_symbol(aml_symbol->component.statement_opcode.def_if_else);
		if(aml_symbol->component.statement_opcode.def_noop)print_aml_symbol(aml_symbol->component.statement_opcode.def_noop);
		if(aml_symbol->component.statement_opcode.def_notify)print_aml_symbol(aml_symbol->component.statement_opcode.def_notify);
		if(aml_symbol->component.statement_opcode.def_release)print_aml_symbol(aml_symbol->component.statement_opcode.def_release);
		if(aml_symbol->component.statement_opcode.def_reset)print_aml_symbol(aml_symbol->component.statement_opcode.def_reset);
		if(aml_symbol->component.statement_opcode.def_return)print_aml_symbol(aml_symbol->component.statement_opcode.def_return);
		if(aml_symbol->component.statement_opcode.def_signal)print_aml_symbol(aml_symbol->component.statement_opcode.def_signal);
		if(aml_symbol->component.statement_opcode.def_sleep)print_aml_symbol(aml_symbol->component.statement_opcode.def_sleep);
		if(aml_symbol->component.statement_opcode.def_stall)print_aml_symbol(aml_symbol->component.statement_opcode.def_stall);
		if(aml_symbol->component.statement_opcode.def_while)print_aml_symbol(aml_symbol->component.statement_opcode.def_while);
		break;
	case aml_store_op:
		break;
	case aml_string:
		if(aml_symbol->component.string.string_prefix)print_aml_symbol(aml_symbol->component.string.string_prefix);
		if(aml_symbol->component.string.ascii_char_list)print_aml_symbol(aml_symbol->component.string.ascii_char_list);
		if(aml_symbol->component.string.null_char)print_aml_symbol(aml_symbol->component.string.null_char);
		break;
	case aml_string_prefix:
		break;
	case aml_subtract_op:
		break;
	case aml_super_name:
		if(aml_symbol->component.super_name.debug_obj)print_aml_symbol(aml_symbol->component.super_name.debug_obj);
		if(aml_symbol->component.super_name.reference_type_opcode)print_aml_symbol(aml_symbol->component.super_name.reference_type_opcode);
		if(aml_symbol->component.super_name.simple_name)print_aml_symbol(aml_symbol->component.super_name.simple_name);
		break;
	case aml_sync_flags:
		break;
	case aml_target:
		if(aml_symbol->component.target.super_name)print_aml_symbol(aml_symbol->component.target.super_name);
		if(aml_symbol->component.target.null_name)print_aml_symbol(aml_symbol->component.target.null_name);
		break;
	case aml_term_arg:
		if(aml_symbol->component.term_arg.expression_opcode)print_aml_symbol(aml_symbol->component.term_arg.expression_opcode);
		if(aml_symbol->component.term_arg.data_object)print_aml_symbol(aml_symbol->component.term_arg.data_object);
		if(aml_symbol->component.term_arg.arg_obj)print_aml_symbol(aml_symbol->component.term_arg.arg_obj);
		if(aml_symbol->component.term_arg.local_obj)print_aml_symbol(aml_symbol->component.term_arg.local_obj);
		break;
	case aml_term_arg_list:
		if(aml_symbol->component.term_arg_list.term_arg)print_aml_symbol(aml_symbol->component.term_arg_list.term_arg);
		if(aml_symbol->component.term_arg_list.term_arg_list)print_aml_symbol(aml_symbol->component.term_arg_list.term_arg_list);
		break;
	case aml_term_list:
		if(aml_symbol->component.term_list.term_obj)print_aml_symbol(aml_symbol->component.term_list.term_obj);
		if(aml_symbol->component.term_list.term_list)print_aml_symbol(aml_symbol->component.term_list.term_list);
		break;
	case aml_term_obj:
		if(aml_symbol->component.term_obj.object)print_aml_symbol(aml_symbol->component.term_obj.object);
		if(aml_symbol->component.term_obj.statement_opcode)print_aml_symbol(aml_symbol->component.term_obj.statement_opcode);
		if(aml_symbol->component.term_obj.expression_opcode)print_aml_symbol(aml_symbol->component.term_obj.expression_opcode);
		if(aml_symbol->component.term_obj.wrong_term_arg)print_aml_symbol(aml_symbol->component.term_obj.wrong_term_arg);
		break;
	case aml_thermal_zone_op:
		if(aml_symbol->component.thermal_zone_op.ext_op_prefix)print_aml_symbol(aml_symbol->component.thermal_zone_op.ext_op_prefix);
		if(aml_symbol->component.thermal_zone_op.thermal_zone_op_suffix)print_aml_symbol(aml_symbol->component.thermal_zone_op.thermal_zone_op_suffix);
		break;
	case aml_thermal_zone_op_suffix:
		break;
	case aml_time_out:
		if(aml_symbol->component.time_out.word_data)print_aml_symbol(aml_symbol->component.time_out.word_data);
		break;
	case aml_to_buffer_op:
		break;
	case aml_to_hex_string_op:
		break;
	case aml_to_integer_op:
		break;
	case aml_user_term_obj:
		break;
	case aml_usec_time:
		if(aml_symbol->component.usec_time.term_arg)print_aml_symbol(aml_symbol->component.usec_time.term_arg);
		break;
	case aml_while_op:
		break;
	case aml_word_const:
		if(aml_symbol->component.word_const.word_prefix)print_aml_symbol(aml_symbol->component.word_const.word_prefix);
		if(aml_symbol->component.word_const.word_data)print_aml_symbol(aml_symbol->component.word_const.word_data);
		break;
	case aml_word_data:
		for(unsigned int i = 0; i < _countof(aml_symbol->component.word_data.byte_data); i++)if(aml_symbol->component.word_data.byte_data[i])print_aml_symbol(aml_symbol->component.word_data.byte_data[i]);
		break;
	case aml_word_prefix:
		break;
	case aml_xor_op:
		break;
	case aml_zero_op:
		break;
	}
}

void print_generic_address_structure(GenericAddressStructure generic_address_structure, char const *name)
{
	printf_serial("%s.address_space = %#04.2x\n", name, generic_address_structure.address_space);
	printf_serial("%s.bit_width = %#04.2x\n", name, generic_address_structure.bit_width);
	printf_serial("%s.bit_offset = %#04.2x\n", name, generic_address_structure.bit_offset);
	printf_serial("%s.access_size = %#04.2x\n", name, generic_address_structure.access_size);
	printf_serial("%s.address = %#018.16x\n", name, generic_address_structure.address);
}

void print_rsdp(RSDP const *rsdp, char const *name)
{
	printf_serial("%s->signature = %.*s\n", name, sizeof(rsdp->signature), rsdp->signature);
	printf_serial("%s->checksum = %#04.2x\n", name, rsdp->checksum);
	printf_serial("%s->oemid = %.*s\n", name, sizeof(rsdp->oemid), rsdp->oemid);
	printf_serial("%s->revision = %#04.2x\n", name, rsdp->revision);
	printf_serial("%s->rsdt = %p\n", name, rsdp->rsdt);
	printf_serial("%s->length = %#010.8x\n", name, rsdp->length);
	printf_serial("%s->xsdt_addr = %#018.16x\n", name, rsdp->xsdt_addr);
	printf_serial("%s->extended_checksum = %#04.2x\n", name, rsdp->extended_checksum);
}

void print_sdts(void)
{
	ACPITableHeader const * const *sdt_headers = get_sdt_headers();
	unsigned int num_of_sdt_headers = get_num_of_sdt_headers();
	for(ACPITableHeader const * const *sdt_header = sdt_headers; sdt_header != sdt_headers + num_of_sdt_headers; sdt_header++)
	{
		PRINT_ACPI_TABLE_HEADER_P(*sdt_header);
		for(unsigned int i = 0; i < (*sdt_header)->length; i++)printf_serial("%02.2x%c", *((unsigned char *)(*sdt_header) + sizeof(**sdt_header) + i), (i + 1) % 0x10 ? ' ' : '\n');
		printf_serial("\n");
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

