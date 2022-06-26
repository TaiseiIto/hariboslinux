#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef enum _NonterminalSymbol
{
	asterisk,
	dot,
	left_parenthesis,
	minus,
	number,
	plus,
	right_parenthesis,
	slash,
	invalid_symbol
} NonterminalSymbol;

typedef struct _Substring
{
	char const *initial;
	size_t length;
} Substring;

typedef struct _Symbol
{
	struct _Symbol *previous;
	struct _Symbol *next;
	Substring string;
	NonterminalSymbol symbol;
} Symbol;

typedef struct _Symbols
{
	Symbol *first_symbol;
	Symbol *last_symbol;
} Symbols;

char *combine_argv(int argc, char const * const * const argv);
void delete_symbols(Symbols symbols);
Symbols lexical_analysis(char const * const input_string);
void print_symbols(Symbols symbols);
NonterminalSymbol substring2symbol(Substring substring);
char const *symbol_name(NonterminalSymbol symbol);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	Symbols symbols = lexical_analysis(input_string);
	print_symbols(symbols);
	delete_symbols(symbols);
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

void delete_symbols(Symbols symbols)
{
	Symbol *next_symbol;
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = next_symbol)
	{
		next_symbol = symbol->next;
		free(symbol);
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
		symbol->symbol = substring2symbol(symbol->string);
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

void print_symbols(Symbols symbols)
{
	for(Symbol *symbol = symbols.first_symbol; symbol; symbol = symbol->next)printf("%s \"%0.*s\"\n", symbol_name(symbol->symbol), symbol->string.length, symbol->string.initial);
}

NonterminalSymbol substring2symbol(Substring substring)
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
			return invalid_symbol;
		}
		break;
	default:
		ERROR(); // Invalid substring length
		exit(-1);
		return invalid_symbol;
	}
}

char const *symbol_name(NonterminalSymbol symbol)
{
	static char const * const asterisk_name = "asterisk";
	static char const * const dot_name = "dot";
	static char const * const left_parenthesis_name = "left parenthesis";
	static char const * const minus_name = "minus";
	static char const * const number_name = "number";
	static char const * const plus_name = "plus";
	static char const * const right_parenthesis_name = "right parenthesis";
	static char const * const slash_name = "slash";
	switch(symbol)
	{
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

