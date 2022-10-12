#include "acpi.h"
#include "serial.h"
#include "shell.h"
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

ChainString *aml_symbol_to_chain_string(AMLSymbol const *aml_symbol)
{
	ChainString *output;
	ChainString *def_break_chain_string;
	ChainString *def_break_point_chain_string;
	ChainString *def_continue_chain_string;
	ChainString *def_fatal_chain_string;
	ChainString *def_if_else_chain_string;
	ChainString *def_noop_chain_string;
	ChainString *def_notify_chain_string;
	ChainString *def_release_chain_string;
	ChainString *def_reset_chain_string;
	ChainString *def_return_chain_string;
	ChainString *def_signal_chain_string;
	ChainString *def_sleep_chain_string;
	ChainString *def_stall_chain_string;
	ChainString *def_while_chain_string;
	ChainString *expression_opcode_chain_string;
	ChainString *named_obj_chain_string;
	ChainString *name_space_modifier_obj_chain_string;
	ChainString *object_chain_string;
	ChainString *statement_opcode_chain_string;
	ChainString *term_list_chain_string;
	ChainString *term_obj_chain_string;
	char *def_break_char_array;
	char *def_break_point_char_array;
	char *def_continue_char_array;
	char *def_fatal_char_array;
	char *def_if_else_char_array;
	char *def_noop_char_array;
	char *def_notify_char_array;
	char *def_release_char_array;
	char *def_reset_char_array;
	char *def_return_char_array;
	char *def_signal_char_array;
	char *def_sleep_char_array;
	char *def_stall_char_array;
	char *def_while_char_array;
	char *expression_opcode_char_array;
	char *named_obj_char_array;
	char *name_space_modifier_obj_char_array;
	char *object_char_array;
	char *statement_opcode_char_array;
	char *term_list_char_array;
	char *term_obj_char_array;
	switch(aml_symbol->type)
	{
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
	static char const * const aml_object_name = "Object";
	static char const * const aml_statement_opcode_name = "StatementOpcode";
	static char const * const aml_term_list_name = "TermList";
	static char const * const aml_term_obj_name = "TermObj";
	switch(aml_symbol_type)
	{
	case aml_object:
		return aml_object_name;
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

// <object> := <name_space_modifier_obj> | <named_obj>
AMLSymbol *analyse_aml_object(AMLSubstring aml)
{
	AMLSymbol *object = malloc(sizeof(*object));
	object->string.initial = aml.initial;
	object->string.length = 0;
	object->type = aml_object;
	object->component.object.name_space_modifier_obj = NULL;
	object->component.object.named_obj = NULL;
	return object;
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
	case aml_object:
		if(aml_symbol->component.object.named_obj)delete_aml_symbol(aml_symbol->component.object.named_obj);
		if(aml_symbol->component.object.name_space_modifier_obj)delete_aml_symbol(aml_symbol->component.object.name_space_modifier_obj);
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

