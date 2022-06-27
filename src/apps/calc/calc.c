// Backus Naur Form
//
// <formula>           ::= <term> | <formula> <plus> <term> | <formula> <minus> <term>
// <term>              ::= <factor> | <term> <asterisk> <factor> | <term> <slash> <factor>
// <factor>            ::= <real> | <left_parenthesis> <formula> <right_parenthesis>
// <real>              ::= <absolute> | <sign> <absolute>
// <sign>              ::= <plus> | <minus>
// <absolute>          ::= <numbers> | <numbers> <dot> <numbers>
// <numbers>           ::= <number> | <numbers> <number>
// <number>            ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
// <dot>               ::= "."
// <plus>              ::= "+"
// <minux>             ::= "-"
// <asterisk>          ::= "*"
// <slash>             ::= "/"
// <left_parenthesis>  ::= "("
// <right_parenthesis> ::= ")"

#include "chain_string.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

struct _Symbol;

typedef enum _SymbolType
{
	absolute,
	asterisk,
	dot,
	left_parenthesis,
	minus,
	number,
	numbers,
	plus,
	right_parenthesis,
	slash,
	invalid_symbol_type
} SymbolType;

typedef struct _Substring
{
	char const *initial;
	size_t length;
} Substring;

typedef struct _Absolute
{
	struct _Symbol *integer;
	struct _Symbol *dot;
	struct _Symbol *decimal;
} Absolute;

typedef struct _Numbers
{
	struct _Symbol *numbers;
	struct _Symbol *number;
} Numbers;

typedef union _Component
{
	Absolute absolute;
	Numbers numbers;
} Component;

typedef struct _Symbol
{
	Component component;
	Substring string;
	SymbolType type;
	struct _Symbol *previous;
	struct _Symbol *next;
} Symbol;

typedef struct _Symbols
{
	Symbol *first_symbol;
	Symbol *last_symbol;
} Symbols;

char *combine_argv(int argc, char const * const * const argv);
void delete_symbol(Symbol *symbol);
void delete_symbols(Symbols symbols);
Symbols lexical_analysis(char const * const input_string);
void print_symbols(Symbols const symbols);
SymbolType substring2symbol_type(Substring substring);
char *symbol_to_string(Symbol const *symbol);
char const *symbol_type_name(SymbolType symbol);
Symbols syntactic_analysis(Symbols symbols);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	Symbols symbols = syntactic_analysis(lexical_analysis(input_string));
	print_symbols(symbols);
	delete_symbols(symbols);
	free(input_string);
	return 0;
}

char *combine_argv(int argc, char const * const * const argv)
{
	char *combined_string;
	char *writer;
	unsigned int sum_length = 0;
	for(int argi = 0; argi < argc; argi++)sum_length += strlen(argv[argi]);
	combined_string = malloc(sum_length + 1);
	writer = combined_string;
	for(int argi = 0; argi < argc; argi++)
	{
		strcpy(writer, argv[argi]);
		writer += strlen(argv[argi]);
	}
	return combined_string;
}

void delete_symbol(Symbol *symbol)
{
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)delete_symbol(symbol->component.absolute.integer);
		if(symbol->component.absolute.dot)delete_symbol(symbol->component.absolute.dot);
		if(symbol->component.absolute.decimal)delete_symbol(symbol->component.absolute.decimal);
		break;
	case asterisk:
		break;
	case dot:
		break;
	case left_parenthesis:
		break;
	case minus:
		break;
	case number:
		break;
	case numbers:
		if(symbol->component.numbers.numbers)delete_symbol(symbol->component.numbers.numbers);
		if(symbol->component.numbers.number)delete_symbol(symbol->component.numbers.number);
		break;
	case plus:
		break;
	case right_parenthesis:
		break;
	case slash:
		break;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		break;
	}
	free(symbol);
}

void delete_symbols(Symbols symbols)
{
	Symbol *next_symbol;
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		delete_symbol(symbol);
	}
}

Symbols lexical_analysis(char const * const input_string)
{
	Symbols symbols;
	symbols.first_symbol = NULL;
	symbols.last_symbol = NULL;
	for(char const *reader = input_string; *reader; reader++)
	{
		Symbol *symbol = malloc(sizeof(*symbol));
		symbol->previous = symbols.last_symbol;
		symbol->next = NULL;
		symbol->string.initial = reader;
		symbol->string.length = 1;
		symbol->type = substring2symbol_type(symbol->string);
		if(!symbols.first_symbol)symbols.first_symbol = symbol;
		if(symbols.last_symbol)
		{
			symbols.last_symbol->next = symbol;
			symbols.last_symbol = symbol;
		}
		else symbols.last_symbol = symbol;
	}
	return symbols;
}

void print_symbols(Symbols const symbols)
{
	for(Symbol const *symbol = symbols.first_symbol; symbol; symbol = symbol->next)
	{
		char *symbol_string = symbol_to_string(symbol);
		printf("%s", symbol_string);
		free(symbol_string);
	}
}

SymbolType substring2symbol_type(Substring substring)
{
	switch(substring.length)
	{
	case 1:
		switch(*substring.initial)
		{
		case '(':
			return left_parenthesis;
		case ')':
			return right_parenthesis;
		case '+':
			return plus;
		case '-':
			return minus;
		case '*':
			return asterisk;
		case '/':
			return slash;
		case '.':
			return dot;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return number;
		default:
			ERROR(); // Invalid substring initial
			exit(-1);
			return invalid_symbol_type;
		}
		break;
	default:
		ERROR(); // Invalid substring length
		exit(-1);
		return invalid_symbol_type;
	}
}

ChainString *symbol_to_chain_string(Symbol const *symbol)
{
	ChainString *output;
	ChainString *decimal_chain_string;
	ChainString *dot_chain_string;
	ChainString *integer_chain_string;
	ChainString *number_chain_string;
	ChainString *numbers_chain_string;
	char *decimal_char_array;
	char *dot_char_array;
	char *integer_char_array;
	char *number_char_array;
	char *numbers_char_array;
	if(!symbol)return create_chain_string("");
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)
		{
			integer_chain_string = symbol_to_chain_string(symbol->component.absolute.integer);
			insert_char_front(integer_chain_string, integer_chain_string->first_character, ' ');
			replace_chain_string(integer_chain_string, "\n", "\n ");
			integer_char_array = create_char_array_from_chain_string(integer_chain_string);
		}
		else integer_char_array = "";
		if(symbol->component.absolute.dot)
		{
			dot_chain_string = symbol_to_chain_string(symbol->component.absolute.dot);
			insert_char_front(dot_chain_string, dot_chain_string->first_character, ' ');
			replace_chain_string(dot_chain_string, "\n", "\n ");
			dot_char_array = create_char_array_from_chain_string(dot_chain_string);
		}
		else dot_char_array = "";
		if(symbol->component.absolute.decimal)
		{
			decimal_chain_string = symbol_to_chain_string(symbol->component.absolute.decimal);
			insert_char_front(decimal_chain_string, decimal_chain_string->first_character, ' ');
			replace_chain_string(decimal_chain_string, "\n", "\n ");
			decimal_char_array = create_char_array_from_chain_string(decimal_chain_string);
		}
		else decimal_char_array = "";
		output = create_format_chain_string("%s\n%s%s%s", symbol_type_name(symbol->type), integer_char_array, dot_char_array, decimal_char_array);
		if(symbol->component.absolute.integer)
		{
			delete_chain_string(integer_chain_string);
			free(integer_char_array);
		}
		if(symbol->component.absolute.dot)
		{
			delete_chain_string(dot_chain_string);
			free(dot_char_array);
		}
		if(symbol->component.absolute.decimal)
		{
			delete_chain_string(decimal_chain_string);
			free(decimal_char_array);
		}
		return output;
	case asterisk:
	case dot:
	case left_parenthesis:
	case minus:
	case number:
	case plus:
	case right_parenthesis:
	case slash:
		return create_format_chain_string("%s \"%0.*s\"\n", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial);
	case numbers:
		if(symbol->component.numbers.numbers)
		{
			numbers_chain_string = symbol_to_chain_string(symbol->component.numbers.numbers);
			insert_char_front(numbers_chain_string, numbers_chain_string->first_character, ' ');
			replace_chain_string(numbers_chain_string, "\n", "\n ");
			numbers_char_array = create_char_array_from_chain_string(numbers_chain_string);
		}
		else numbers_char_array = "";
		if(symbol->component.numbers.number)
		{
			number_chain_string = symbol_to_chain_string(symbol->component.numbers.number);
			insert_char_front(number_chain_string, number_chain_string->first_character, ' ');
			replace_chain_string(number_chain_string, "\n", "\n ");
			number_char_array = create_char_array_from_chain_string(number_chain_string);
		}
		else number_char_array = "";

		output = create_format_chain_string("%s\n%s%s", symbol_type_name(symbol->type), numbers_char_array, number_char_array);
		if(symbol->component.numbers.numbers)
		{
			delete_chain_string(numbers_chain_string);
			free(numbers_char_array);
		}
		if(symbol->component.numbers.number)
		{
			delete_chain_string(number_chain_string);
			free(number_char_array);
		}
		return output;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		return NULL;
	}
}

char *symbol_to_string(Symbol const *symbol)
{
	ChainString *chain_string = symbol_to_chain_string(symbol);
	char *string = create_char_array_from_chain_string(chain_string);
	delete_chain_string(chain_string);
	return string;
}

char const *symbol_type_name(SymbolType symbol_type)
{
	static char const * const absolute_name = "absolute";
	static char const * const asterisk_name = "asterisk";
	static char const * const dot_name = "dot";
	static char const * const left_parenthesis_name = "left parenthesis";
	static char const * const minus_name = "minus";
	static char const * const number_name = "number";
	static char const * const numbers_name = "numbers";
	static char const * const plus_name = "plus";
	static char const * const right_parenthesis_name = "right parenthesis";
	static char const * const slash_name = "slash";
	switch(symbol_type)
	{
	case absolute:
		return absolute_name;
	case asterisk:
		return asterisk_name;
	case dot:
		return dot_name;
	case left_parenthesis:
		return left_parenthesis_name;
	case minus:
		return minus_name;
	case number:
		return number_name;
	case numbers:
		return numbers_name;
	case plus:
		return plus_name;
	case right_parenthesis:
		return right_parenthesis_name;
	case slash:
		return slash_name;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		return NULL;
	}
}

Symbols syntactic_analysis(Symbols symbols)
{
	Symbol *new_symbol;
	Symbol *next_symbol;
	unsigned char flags = 0;
	#define SYNTACTIC_ANALYSIS_FLAG_CHANGED	0x01
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		switch(symbol->type)
		{
		case absolute:
			break;
		case asterisk:
			break;
		case dot:
			if(symbol->previous && symbol->previous->type == numbers && symbol->next && symbol->next->type == numbers)
			{
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = absolute;
				new_symbol->component.absolute.integer = symbol->previous;
				new_symbol->component.absolute.dot = symbol;
				new_symbol->component.absolute.decimal = symbol->next;
				new_symbol->string.initial = NULL;
				new_symbol->string.length = 0;
				new_symbol->previous = symbol->previous->previous;
				new_symbol->next = symbol->next->next;
				if(symbols.first_symbol == symbol->previous)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				new_symbol->component.absolute.integer->previous = NULL;
				new_symbol->component.absolute.integer->next = NULL;
				new_symbol->component.absolute.dot->previous = NULL;
				new_symbol->component.absolute.dot->next = NULL;
				new_symbol->component.absolute.decimal->previous = NULL;
				new_symbol->component.absolute.decimal->next = NULL;
				next_symbol = new_symbol->next;
			}
			break;
		case left_parenthesis:
			break;
		case minus:
			break;
		case number:
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = numbers;
			new_symbol->component.numbers.number = symbol;
			new_symbol->string.initial = NULL;
			new_symbol->string.length = 0;
			new_symbol->next = symbol->next;
			if(symbol->previous && symbol->previous->type == numbers)
			{
				new_symbol->component.numbers.numbers = symbol->previous;
				new_symbol->previous = symbol->previous->previous;
				if(symbols.first_symbol == symbol->previous)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			}
			else
			{
				new_symbol->component.numbers.numbers = NULL;
				new_symbol->previous = symbol->previous;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			}
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			new_symbol->component.numbers.number->previous = NULL;
			new_symbol->component.numbers.number->next = NULL;
			if(new_symbol->component.numbers.numbers)
			{
				new_symbol->component.numbers.numbers->previous = NULL;
				new_symbol->component.numbers.numbers->next = NULL;
			}
			break;
		case numbers:
			break;
		case plus:
			break;
		case right_parenthesis:
			break;
		case slash:
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
	}
	if(flags & SYNTACTIC_ANALYSIS_FLAG_CHANGED)return syntactic_analysis(symbols);
	return symbols;
}

