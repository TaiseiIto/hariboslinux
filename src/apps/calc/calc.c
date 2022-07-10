// Backus Naur Form
//
// <formula>           ::= <term>
// <term>              ::= <factor> | <plus> <factor> | <minus> <factor> | <term> <plus> <factor> | <term> <minus> <factor>
// <factor>            ::= <power> | <factor> <asterisk> <power> | <factor> <slash> <power>
// <power>             ::= <operand> | <power> <circumflex> <operand>
// <operand>           ::= <absolute> | <left_parenthesis> <formula> <right_parenthesis> | <e> | <pi>
// <e>                 ::= <alphabets "e">
// <pi>                ::= <alphabets "pi">
// <absolute>          ::= <numbers> | <numbers> <dot> <numbers>
// <alphabets>         ::= <alphabet> | <alphabets> <alphabet>
// <numbers>           ::= <number> | <numbers> <number>
// <alphabet>          ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
// <number>            ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
// <dot>               ::= '.'
// <plus>              ::= '+'
// <minux>             ::= '-'
// <asterisk>          ::= '*'
// <slash>             ::= '/'
// <circumflex>        ::= '^'
// <left_parenthesis>  ::= '('
// <right_parenthesis> ::= ')'

#include "chain_string.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define DEBUG

struct _Symbol;

typedef enum _SymbolType
{
	absolute,
	alphabet,
	alphabets,
	asterisk,
	circumflex,
	dot,
	e,
	factor,
	formula,
	left_parenthesis,
	minus,
	number,
	numbers,
	operand,
	pi,
	plus,
	power,
	right_parenthesis,
	slash,
	term,
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

typedef struct _Alphabets
{
	struct _Symbol *alphabets;
	struct _Symbol *alphabet;
} Alphabets;

typedef struct _E
{
	struct _Symbol *alphabets;
} E;

typedef struct _Factor
{
	struct _Symbol *factor;
	struct _Symbol *operator;
	struct _Symbol *power;
} Factor;

typedef struct _Formula
{
	struct _Symbol *term;
} Formula;

typedef struct _Numbers
{
	struct _Symbol *numbers;
	struct _Symbol *number;
	unsigned int level;
} Numbers;

typedef struct _Operand
{
	struct _Symbol *left_parenthesis;
	struct _Symbol *right_parenthesis;
	struct _Symbol *value;
} Operand;

typedef struct _Pi
{
	struct _Symbol *alphabets;
} Pi;

typedef struct _Power
{
	struct _Symbol *power;
	struct _Symbol *circumflex;
	struct _Symbol *operand;
} Power;

typedef struct _Term
{
	struct _Symbol *term;
	struct _Symbol *operator;
	struct _Symbol *factor;
} Term;

typedef union _Component
{
	Absolute absolute;
	Alphabets alphabets;
	E e;
	Factor factor;
	Formula formula;
	Numbers numbers;
	Operand operand;
	Pi pi;
	Power power;
	Term term;
} Component;

typedef struct _Symbol
{
	Component component;
	Substring string;
	SymbolType type;
	double value;
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
void semantic_analysis(Symbol* symbol);
Symbols syntactic_analysis(Symbols symbols);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	Symbols symbols = lexical_analysis(input_string);
	symbols = syntactic_analysis(symbols);
	if(!symbols.first_symbol)
	{
		ERROR(); // no symbol error
		exit(-1);
	}
	if(symbols.first_symbol != symbols.last_symbol)
	{
		ERROR(); // syntactic analysis error
		print_symbols(symbols);
		exit(-1);
	}
	semantic_analysis(symbols.first_symbol);
	#ifdef DEBUG
	printf("\n");
	print_symbols(symbols);
	printf("\n");
	#endif
	printf("%.6llf\n", symbols.first_symbol->value);
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
	#ifdef DEBUG
	char *symbol_string = symbol_to_string(symbol);
	free(symbol_string);
	#endif
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)delete_symbol(symbol->component.absolute.integer);
		if(symbol->component.absolute.dot)delete_symbol(symbol->component.absolute.dot);
		if(symbol->component.absolute.decimal)delete_symbol(symbol->component.absolute.decimal);
		break;
	case alphabet:
		break;
	case alphabets:
		if(symbol->component.alphabets.alphabets)delete_symbol(symbol->component.alphabets.alphabets);
		if(symbol->component.alphabets.alphabet)delete_symbol(symbol->component.alphabets.alphabet);
		break;
	case asterisk:
		break;
	case circumflex:
		break;
	case dot:
		break;
	case e:
		if(symbol->component.e.alphabets)delete_symbol(symbol->component.e.alphabets);
		break;
	case factor:
		if(symbol->component.factor.factor)delete_symbol(symbol->component.factor.factor);
		if(symbol->component.factor.operator)delete_symbol(symbol->component.factor.operator);
		if(symbol->component.factor.power)delete_symbol(symbol->component.factor.power);
		break;
	case formula:
		if(symbol->component.formula.term)delete_symbol(symbol->component.formula.term);
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
	case operand:
		if(symbol->component.operand.left_parenthesis)delete_symbol(symbol->component.operand.left_parenthesis);
		if(symbol->component.operand.right_parenthesis)delete_symbol(symbol->component.operand.right_parenthesis);
		if(symbol->component.operand.value)delete_symbol(symbol->component.operand.value);
		break;
	case pi:
		if(symbol->component.pi.alphabets)delete_symbol(symbol->component.pi.alphabets);
		break;
	case plus:
		break;
	case power:
		if(symbol->component.power.power)delete_symbol(symbol->component.power.power);
		if(symbol->component.power.circumflex)delete_symbol(symbol->component.power.circumflex);
		if(symbol->component.power.operand)delete_symbol(symbol->component.power.operand);
		break;
	case right_parenthesis:
		break;
	case slash:
		break;
	case term:
		if(symbol->component.term.term)delete_symbol(symbol->component.term.term);
		if(symbol->component.term.operator)delete_symbol(symbol->component.term.operator);
		if(symbol->component.term.factor)delete_symbol(symbol->component.term.factor);
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
		case '^':
			return circumflex;
		case '.':
			return dot;
		default:
			if('0' <= *substring.initial && *substring.initial <= '9')return number;
			else if(('a' <= *substring.initial && *substring.initial <= 'z') || ('A' <= *substring.initial && *substring.initial <= 'Z'))return alphabet;
			else
			{
				ERROR(); // Invalid substring initial
				exit(-1);
				return invalid_symbol_type;
			}
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
	ChainString *alphabet_chain_string;
	ChainString *alphabets_chain_string;
	ChainString *circumflex_chain_string;
	ChainString *decimal_chain_string;
	ChainString *dot_chain_string;
	ChainString *factor_chain_string;
	ChainString *integer_chain_string;
	ChainString *left_parenthesis_chain_string;
	ChainString *number_chain_string;
	ChainString *numbers_chain_string;
	ChainString *operand_chain_string;
	ChainString *operator_chain_string;
	ChainString *power_chain_string;
	ChainString *right_parenthesis_chain_string;
	ChainString *term_chain_string;
	ChainString *value_chain_string;
	char *alphabet_char_array;
	char *alphabets_char_array;
	char *circumflex_char_array;
	char *decimal_char_array;
	char *dot_char_array;
	char *factor_char_array;
	char *integer_char_array;
	char *left_parenthesis_char_array;
	char *number_char_array;
	char *numbers_char_array;
	char *operand_char_array;
	char *operator_char_array;
	char *power_char_array;
	char *right_parenthesis_char_array;
	char *term_char_array;
	char *value_char_array;
	if(!symbol)return create_chain_string("");
	switch(symbol->type)
	{
	case alphabet:
	case asterisk:
	case circumflex:
	case dot:
	case left_parenthesis:
	case minus:
	case number:
	case plus:
	case right_parenthesis:
	case slash:
		return create_format_chain_string("%s \"%0.*s\"\n", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial);
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
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, integer_char_array, dot_char_array, decimal_char_array);
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
	case alphabets:
		if(symbol->component.alphabets.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.alphabets.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		if(symbol->component.alphabets.alphabet)
		{
			alphabet_chain_string = symbol_to_chain_string(symbol->component.alphabets.alphabet);
			insert_char_front(alphabet_chain_string, alphabet_chain_string->first_character, ' ');
			replace_chain_string(alphabet_chain_string, "\n", "\n ");
			alphabet_char_array = create_char_array_from_chain_string(alphabet_chain_string);
		}
		else alphabet_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, alphabets_char_array, alphabet_char_array);
		if(symbol->component.alphabets.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		if(symbol->component.alphabets.alphabet)
		{
			delete_chain_string(alphabet_chain_string);
			free(alphabet_char_array);
		}
		return output;
	case e:
		if(symbol->component.e.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.e.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, alphabets_char_array);
		if(symbol->component.e.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case factor:
		if(symbol->component.factor.factor)
		{
			factor_chain_string = symbol_to_chain_string(symbol->component.factor.factor);
			insert_char_front(factor_chain_string, factor_chain_string->first_character, ' ');
			replace_chain_string(factor_chain_string, "\n", "\n ");
			factor_char_array = create_char_array_from_chain_string(factor_chain_string);
		}
		else factor_char_array = "";
		if(symbol->component.factor.operator)
		{
			operator_chain_string = symbol_to_chain_string(symbol->component.factor.operator);
			insert_char_front(operator_chain_string, operator_chain_string->first_character, ' ');
			replace_chain_string(operator_chain_string, "\n", "\n ");
			operator_char_array = create_char_array_from_chain_string(operator_chain_string);
		}
		else operator_char_array = "";
		if(symbol->component.factor.power)
		{
			power_chain_string = symbol_to_chain_string(symbol->component.factor.power);
			insert_char_front(power_chain_string, power_chain_string->first_character, ' ');
			replace_chain_string(power_chain_string, "\n", "\n ");
			power_char_array = create_char_array_from_chain_string(power_chain_string);
		}
		else power_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, factor_char_array, operator_char_array, power_char_array);
		if(symbol->component.factor.factor)
		{
			delete_chain_string(factor_chain_string);
			free(factor_char_array);
		}
		if(symbol->component.factor.operator)
		{
			delete_chain_string(operator_chain_string);
			free(operator_char_array);
		}
		if(symbol->component.factor.power)
		{
			delete_chain_string(power_chain_string);
			free(power_char_array);
		}
		return output;
	case formula:
		if(symbol->component.formula.term)
		{
			term_chain_string = symbol_to_chain_string(symbol->component.formula.term);
			insert_char_front(term_chain_string, term_chain_string->first_character, ' ');
			replace_chain_string(term_chain_string, "\n", "\n ");
			term_char_array = create_char_array_from_chain_string(term_chain_string);
		}
		else term_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, term_char_array);
		if(symbol->component.formula.term)
		{
			delete_chain_string(term_chain_string);
			free(term_char_array);
		}
		return output;
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
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, numbers_char_array, number_char_array);
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
	case operand:
		if(symbol->component.operand.left_parenthesis)
		{
			left_parenthesis_chain_string = symbol_to_chain_string(symbol->component.operand.left_parenthesis);
			insert_char_front(left_parenthesis_chain_string, left_parenthesis_chain_string->first_character, ' ');
			replace_chain_string(left_parenthesis_chain_string, "\n", "\n ");
			left_parenthesis_char_array = create_char_array_from_chain_string(left_parenthesis_chain_string);
		}
		else left_parenthesis_char_array = "";
		if(symbol->component.operand.right_parenthesis)
		{
			right_parenthesis_chain_string = symbol_to_chain_string(symbol->component.operand.right_parenthesis);
			insert_char_front(right_parenthesis_chain_string, right_parenthesis_chain_string->first_character, ' ');
			replace_chain_string(right_parenthesis_chain_string, "\n", "\n ");
			right_parenthesis_char_array = create_char_array_from_chain_string(right_parenthesis_chain_string);
		}
		else right_parenthesis_char_array = "";
		if(symbol->component.operand.value)
		{
			value_chain_string = symbol_to_chain_string(symbol->component.operand.value);
			insert_char_front(value_chain_string, value_chain_string->first_character, ' ');
			replace_chain_string(value_chain_string, "\n", "\n ");
			value_char_array = create_char_array_from_chain_string(value_chain_string);
		}
		else value_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, left_parenthesis_char_array, value_char_array, right_parenthesis_char_array);
		if(symbol->component.operand.left_parenthesis)
		{
			delete_chain_string(left_parenthesis_chain_string);
			free(left_parenthesis_char_array);
		}
		if(symbol->component.operand.right_parenthesis)
		{
			delete_chain_string(right_parenthesis_chain_string);
			free(right_parenthesis_char_array);
		}
		if(symbol->component.operand.value)
		{
			delete_chain_string(value_chain_string);
			free(value_char_array);
		}
		return output;
	case pi:
		if(symbol->component.pi.alphabets)
		{
			alphabets_chain_string = symbol_to_chain_string(symbol->component.pi.alphabets);
			insert_char_front(alphabets_chain_string, alphabets_chain_string->first_character, ' ');
			replace_chain_string(alphabets_chain_string, "\n", "\n ");
			alphabets_char_array = create_char_array_from_chain_string(alphabets_chain_string);
		}
		else alphabets_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, alphabets_char_array);
		if(symbol->component.pi.alphabets)
		{
			delete_chain_string(alphabets_chain_string);
			free(alphabets_char_array);
		}
		return output;
	case power:
		if(symbol->component.power.power)
		{
			power_chain_string = symbol_to_chain_string(symbol->component.power.power);
			insert_char_front(power_chain_string, power_chain_string->first_character, ' ');
			replace_chain_string(power_chain_string, "\n", "\n ");
			power_char_array = create_char_array_from_chain_string(power_chain_string);
		}
		else power_char_array = "";
		if(symbol->component.power.circumflex)
		{
			circumflex_chain_string = symbol_to_chain_string(symbol->component.power.circumflex);
			insert_char_front(circumflex_chain_string, circumflex_chain_string->first_character, ' ');
			replace_chain_string(circumflex_chain_string, "\n", "\n ");
			circumflex_char_array = create_char_array_from_chain_string(circumflex_chain_string);
		}
		else circumflex_char_array = "";
		if(symbol->component.power.operand)
		{
			operand_chain_string = symbol_to_chain_string(symbol->component.power.operand);
			insert_char_front(operand_chain_string, operand_chain_string->first_character, ' ');
			replace_chain_string(operand_chain_string, "\n", "\n ");
			operand_char_array = create_char_array_from_chain_string(operand_chain_string);
		}
		else operand_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, power_char_array, circumflex_char_array, operand_char_array);
		if(symbol->component.power.power)
		{
			delete_chain_string(power_chain_string);
			free(power_char_array);
		}
		if(symbol->component.power.circumflex)
		{
			delete_chain_string(circumflex_chain_string);
			free(circumflex_char_array);
		}
		if(symbol->component.power.operand)
		{
			delete_chain_string(operand_chain_string);
			free(operand_char_array);
		}
		return output;
	case term:
		if(symbol->component.term.term)
		{
			term_chain_string = symbol_to_chain_string(symbol->component.term.term);
			insert_char_front(term_chain_string, term_chain_string->first_character, ' ');
			replace_chain_string(term_chain_string, "\n", "\n ");
			term_char_array = create_char_array_from_chain_string(term_chain_string);
		}
		else term_char_array = "";
		if(symbol->component.term.operator)
		{
			operator_chain_string = symbol_to_chain_string(symbol->component.term.operator);
			insert_char_front(operator_chain_string, operator_chain_string->first_character, ' ');
			replace_chain_string(operator_chain_string, "\n", "\n ");
			operator_char_array = create_char_array_from_chain_string(operator_chain_string);
		}
		else operator_char_array = "";
		if(symbol->component.term.factor)
		{
			factor_chain_string = symbol_to_chain_string(symbol->component.term.factor);
			insert_char_front(factor_chain_string, factor_chain_string->first_character, ' ');
			replace_chain_string(factor_chain_string, "\n", "\n ");
			factor_char_array = create_char_array_from_chain_string(factor_chain_string);
		}
		else factor_char_array = "";
		output = create_format_chain_string("%s \"%0.*s\" = %.6llf\n%s%s%s", symbol_type_name(symbol->type), symbol->string.length, symbol->string.initial, symbol->value, term_char_array, operator_char_array, factor_char_array);
		if(symbol->component.term.term)
		{
			delete_chain_string(term_chain_string);
			free(term_char_array);
		}
		if(symbol->component.term.operator)
		{
			delete_chain_string(operator_chain_string);
			free(operator_char_array);
		}
		if(symbol->component.term.factor)
		{
			delete_chain_string(factor_chain_string);
			free(factor_char_array);
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
	static char const * const alphabet_name = "alphabet";
	static char const * const alphabets_name = "alphabets";
	static char const * const asterisk_name = "asterisk";
	static char const * const circumflex_name = "circumflex";
	static char const * const dot_name = "dot";
	static char const * const e_name = "e";
	static char const * const factor_name = "factor";
	static char const * const formula_name = "formula";
	static char const * const left_parenthesis_name = "left parenthesis";
	static char const * const minus_name = "minus";
	static char const * const number_name = "number";
	static char const * const numbers_name = "numbers";
	static char const * const operand_name = "operand";
	static char const * const pi_name = "pi";
	static char const * const plus_name = "plus";
	static char const * const power_name = "power";
	static char const * const right_parenthesis_name = "right parenthesis";
	static char const * const slash_name = "slash";
	static char const * const term_name = "term";
	switch(symbol_type)
	{
	case absolute:
		return absolute_name;
	case alphabet:
		return alphabet_name;
	case alphabets:
		return alphabets_name;
	case asterisk:
		return asterisk_name;
	case circumflex:
		return circumflex_name;
	case dot:
		return dot_name;
	case e:
		return e_name;
	case factor:
		return factor_name;
	case formula:
		return formula_name;
	case left_parenthesis:
		return left_parenthesis_name;
	case minus:
		return minus_name;
	case number:
		return number_name;
	case numbers:
		return numbers_name;
	case operand:
		return operand_name;
	case pi:
		return pi_name;
	case plus:
		return plus_name;
	case power:
		return power_name;
	case right_parenthesis:
		return right_parenthesis_name;
	case slash:
		return slash_name;
	case term:
		return term_name;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		return NULL;
	}
}

void semantic_analysis(Symbol* symbol)
{
	if(!symbol)
	{
		ERROR(); // no symbol error
		exit(-1);
	}
	switch(symbol->type)
	{
	case absolute:
		if(symbol->component.absolute.integer)semantic_analysis(symbol->component.absolute.integer);
		if(symbol->component.absolute.dot)semantic_analysis(symbol->component.absolute.dot);
		if(symbol->component.absolute.decimal)semantic_analysis(symbol->component.absolute.decimal);
		symbol->value = 0.0;
		if(symbol->component.absolute.decimal)
		{
			double divisor = 1.0;
			unsigned int divisor_level;
			for(divisor_level = 0; divisor_level <= symbol->component.absolute.decimal->component.numbers.level; divisor_level++)divisor *= 10.0;
			symbol->value = symbol->component.absolute.decimal->value / divisor;
		}
		if(symbol->component.absolute.integer)symbol->value += symbol->component.absolute.integer->value;
		break;
	case alphabet:
		symbol->value = 0.0;
		break;
	case alphabets:
		symbol->value = 0.0;
		break;
	case asterisk:
		symbol->value = 0.0;
		break;
	case circumflex:
		symbol->value = 0.0;
		break;
	case dot:
		symbol->value = 0.0;
		break;
	case e:
		symbol->value = M_E;
		break;
	case factor:
		if(symbol->component.factor.factor)semantic_analysis(symbol->component.factor.factor);
		if(symbol->component.factor.operator)semantic_analysis(symbol->component.factor.operator);
		if(symbol->component.factor.power)semantic_analysis(symbol->component.factor.power);
		
		if(symbol->component.factor.operator && symbol->component.factor.factor)switch(symbol->component.factor.operator->type)
		{
		case asterisk:
			symbol->value = symbol->component.factor.factor->value * symbol->component.factor.power->value;
			break;
		case slash:
			symbol->value = symbol->component.factor.factor->value / symbol->component.factor.power->value;
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else symbol->value = symbol->component.factor.power->value;
		break;
	case formula:
		if(symbol->component.formula.term)semantic_analysis(symbol->component.formula.term);
		symbol->value = symbol->component.formula.term->value;
		break;
	case left_parenthesis:
		symbol->value = 0.0;
		break;
	case minus:
		symbol->value = 0.0;
		break;
	case number:
		symbol->value = (double)((int)(*symbol->string.initial - '0'));
		break;
	case numbers:
		if(symbol->component.numbers.numbers)semantic_analysis(symbol->component.numbers.numbers);
		if(symbol->component.numbers.number)semantic_analysis(symbol->component.numbers.number);
		symbol->value = symbol->component.numbers.numbers ? 10.0 * symbol->component.numbers.numbers->value : 0.0;
		symbol->value += symbol->component.numbers.number->value;
		break;
	case operand:
		if(symbol->component.operand.value)semantic_analysis(symbol->component.operand.value);
		if(symbol->component.operand.left_parenthesis)semantic_analysis(symbol->component.operand.left_parenthesis);
		if(symbol->component.operand.right_parenthesis)semantic_analysis(symbol->component.operand.right_parenthesis);
		if(symbol->component.operand.value)symbol->value = symbol->component.operand.value->value;
		break;
	case pi:
		symbol->value = M_PI;
		break;
	case plus:
		symbol->value = 0.0;
		break;
	case power:
		if(symbol->component.power.power)semantic_analysis(symbol->component.power.power);
		if(symbol->component.power.circumflex)semantic_analysis(symbol->component.power.circumflex);
		if(symbol->component.power.operand)semantic_analysis(symbol->component.power.operand);
		if(symbol->component.power.operand)
		{
			if(symbol->component.power.power && symbol->component.power.circumflex)symbol->value = pow(symbol->component.power.power->value, symbol->component.power.operand->value);
			else symbol->value = symbol->component.power.operand->value;
		}
		else
		{
			ERROR(); // Operand is not found.
			exit(-1);
			break;
		}
		break;
	case right_parenthesis:
		symbol->value = 0.0;
		break;
	case slash:
		symbol->value = 0.0;
		break;
	case term:
		if(symbol->component.term.term)semantic_analysis(symbol->component.term.term);
		if(symbol->component.term.operator)semantic_analysis(symbol->component.term.operator);
		if(symbol->component.term.factor)semantic_analysis(symbol->component.term.factor);
		if(symbol->component.term.term && symbol->component.term.operator)switch(symbol->component.term.operator->type)
		{
		case plus:
			symbol->value = symbol->component.term.term->value + symbol->component.term.factor->value;
			break;
		case minus:
			symbol->value = symbol->component.term.term->value - symbol->component.term.factor->value;
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else if(symbol->component.term.operator)switch(symbol->component.term.operator->type)
		{
		case plus:
			symbol->value = symbol->component.term.factor->value;
			break;
		case minus:
			symbol->value = 0.0 - symbol->component.term.factor->value;
			break;
		default:
			ERROR(); // Invalid symbol
			exit(-1);
			break;
		}
		else symbol->value = symbol->component.term.factor->value;
		break;
	case invalid_symbol_type:
		symbol->value = 0.0;
		break;
	default:
		ERROR(); // Invalid symbol
		exit(-1);
		break;
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
			// <operand> ::= <absolute>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <absolute>\n");
			print_symbols(symbols);
			#endif
			break;
		case alphabet:
			// <alphabets> ::= <alphabet>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = alphabets;
			new_symbol->component.alphabets.alphabets = NULL;
			new_symbol->component.alphabets.alphabet = symbol;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<alphabets> ::= <alphabet>\n");
			print_symbols(symbols);
			#endif
			break;
		case alphabets:
			if(symbol->next && symbol->next->type == alphabet)
			{
				// <alphabets> ::= <alphabets> <alphabet>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = alphabets;
				new_symbol->component.alphabets.alphabets = symbol;
				new_symbol->component.alphabets.alphabet = symbol->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<alphabets> ::= <alphabets> <alphabet>\n");
				print_symbols(symbols);
				#endif
			}
			else
			{
				char *word = create_format_char_array("%0.*s", symbol->string.length, symbol->string.initial);
				if(!strcmp(word, "e"))
				{
					// <e> ::= <alphabets "e">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = e;
					new_symbol->component.e.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<e> ::= <alphabets \"e\">\n");
					print_symbols(symbols);
					#endif
				}
				else if(!strcmp(word, "pi"))
				{
					// <pi> ::= <alphabets "pi">
					new_symbol = malloc(sizeof(*new_symbol));
					new_symbol->type = pi;
					new_symbol->component.pi.alphabets = symbol;
					new_symbol->string.initial = symbol->string.initial;
					new_symbol->string.length = symbol->string.length;
					new_symbol->previous = symbol->previous;
					new_symbol->next = symbol->next;
					if(new_symbol->previous)new_symbol->previous->next = new_symbol;
					if(new_symbol->next)new_symbol->next->previous = new_symbol;
					if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
					if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
					symbol->previous = NULL;
					symbol->next = NULL;
					next_symbol = new_symbol;
					flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
					#ifdef DEBUG
					printf("\n<pi> ::= <alphabets \"pi\">\n");
					print_symbols(symbols);
					#endif
				}
				free(word);
			}
			break;
		case asterisk:
			break;
		case circumflex:
			break;
		case dot:
			break;
		case e:
			// <operand> ::= <e>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <e>\n");
			print_symbols(symbols);
			#endif
			break;
		case factor:
			if(symbol->next && (symbol->next->type == asterisk || symbol->next->type == slash) && symbol->next->next && symbol->next->next->type == power)
			{
				if(symbol->next->next->next && symbol->next->next->next->type == circumflex)break;
				// <factor> ::= <factor> <asterisk> <power> | <factor> <slash> <power>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = factor;
				new_symbol->component.factor.factor = symbol;
				new_symbol->component.factor.operator = symbol->next;
				new_symbol->component.factor.power = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<factor> ::= <factor> <asterisk> <power> | <factor> <slash> <power>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == asterisk)break;
			else if(symbol->next && symbol->next->type == slash)break;
			else if(symbol->previous && symbol->previous->type == plus)break;
			else if(symbol->previous && symbol->previous->type == minus)break;
			else
			{
				// <term> ::= <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = NULL;
				new_symbol->component.term.operator = NULL;
				new_symbol->component.term.factor = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <factor>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case formula:
			break;
		case left_parenthesis:
			if(symbol->next && symbol->next->type == formula && symbol->next->next && symbol->next->next->type == right_parenthesis)
			{
				// <operand> ::= <left_parenthesis> <formula> <right_parenthesis>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = operand;
				new_symbol->component.operand.value = symbol->next;
				new_symbol->component.operand.left_parenthesis = symbol;
				new_symbol->component.operand.right_parenthesis = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<operand> ::= <left_parenthesis> <formula> <right_parenthesis>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case minus:
		case plus:
			if(symbol->next && symbol->next->type == factor)
			{
				if(symbol->previous && symbol->previous->type != left_parenthesis)break;
				if(symbol->next->next && symbol->next->next->type == asterisk)break;
				if(symbol->next->next && symbol->next->next->type == slash)break;
				// <term> ::= <minus> <factor> | <plus> <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = NULL;
				new_symbol->component.term.operator = symbol;
				new_symbol->component.term.factor = symbol->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <minus> <factor> | <plus> <factor>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case number:
			// <numbers> ::= <number>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = numbers;
			new_symbol->component.numbers.numbers = NULL;
			new_symbol->component.numbers.number = symbol;
			new_symbol->component.numbers.level = 0;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<numbers> ::= <number>\n");
			print_symbols(symbols);
			#endif
			break;
		case numbers:
			if(symbol->next && symbol->next->type == number)
			{
				// <numbers> ::= <numbers> <number>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = numbers;
				new_symbol->component.numbers.numbers = symbol;
				new_symbol->component.numbers.number = symbol->next;
				new_symbol->component.numbers.level = symbol->component.numbers.level + 1;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next)symbols.last_symbol = new_symbol;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<numbers> ::= <numbers> <number>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == dot && symbol->next->next && symbol->next->next->type == numbers)
			{
				// <absolute> ::= <numbers> <dot> <numbers>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = absolute;
				new_symbol->component.absolute.integer = symbol;
				new_symbol->component.absolute.dot = symbol->next;
				new_symbol->component.absolute.decimal = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<absolute> ::= <numbers> <dot> <numbers>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->previous && symbol->previous->type == dot)break;
			else if(symbol->previous && symbol->previous->type == number)break;
			else if(symbol->next && symbol->next->type == dot)break;
			else if(symbol->next && symbol->next->type == number)break;
			else
			{
				// <absolute> ::= <numbers>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = absolute;
				new_symbol->component.absolute.integer = symbol;
				new_symbol->component.absolute.dot = NULL;
				new_symbol->component.absolute.decimal = NULL;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<absolute> ::= <numbers>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case operand:
			if(symbol->previous && symbol->previous->type == circumflex)break;
			else
			{
				// <power> ::= <operand>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = power;
				new_symbol->component.power.power = NULL;
				new_symbol->component.power.circumflex = NULL;
				new_symbol->component.power.operand = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<power> ::= <operand>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case pi:
			// <operand> ::= <pi>
			new_symbol = malloc(sizeof(*new_symbol));
			new_symbol->type = operand;
			new_symbol->component.operand.value = symbol;
			new_symbol->component.operand.left_parenthesis = NULL;
			new_symbol->component.operand.right_parenthesis = NULL;
			new_symbol->string.initial = symbol->string.initial;
			new_symbol->string.length = symbol->string.length;
			new_symbol->previous = symbol->previous;
			new_symbol->next = symbol->next;
			if(new_symbol->previous)new_symbol->previous->next = new_symbol;
			if(new_symbol->next)new_symbol->next->previous = new_symbol;
			if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
			if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
			symbol->previous = NULL;
			symbol->next = NULL;
			next_symbol = new_symbol;
			flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
			#ifdef DEBUG
			printf("\n<operand> ::= <pi>\n");
			print_symbols(symbols);
			#endif
			break;
		case power:
			if(symbol->previous && symbol->previous->type == asterisk)break;
			if(symbol->previous && symbol->previous->type == slash)break;
			if(symbol->next && symbol->next->type == circumflex && symbol->next->next && symbol->next->next->type == operand)
			{
				// <power> ::= <power> <circumflex> <operand>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = power;
				new_symbol->component.power.power = symbol;
				new_symbol->component.power.circumflex = symbol->next;
				new_symbol->component.power.operand = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<power> ::= <power> <circumflex> <operand>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == circumflex)break;
			else
			{
				// <factor> ::= <power>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = factor;
				new_symbol->component.factor.factor = NULL;
				new_symbol->component.factor.operator = NULL;
				new_symbol->component.factor.power = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<factor> ::= <power>\n");
				print_symbols(symbols);
				#endif
			}
			break;
		case right_parenthesis:
			break;
		case slash:
			break;
		case term:
			if(symbol->next && (symbol->next->type == plus || symbol->next->type == minus) && symbol->next->next && symbol->next->next->type == factor)
			{
				if(symbol->next->next->next && symbol->next->next->next->type == asterisk)break;
				if(symbol->next->next->next && symbol->next->next->next->type == slash)break;
				// <term> ::= <term> <plus> <factor> | <term> <minus> <factor>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = term;
				new_symbol->component.term.term = symbol;
				new_symbol->component.term.operator = symbol->next;
				new_symbol->component.term.factor = symbol->next->next;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length + symbol->next->string.length + symbol->next->next->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next->next->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol->next->next)symbols.last_symbol = new_symbol;
				symbol->next->next->previous = NULL;
				symbol->next->next->next = NULL;
				symbol->next->previous = NULL;
				symbol->next->next = NULL;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<term> ::= <term> <plus> <factor> | <term> <minus> <factor>\n");
				print_symbols(symbols);
				#endif
			}
			else if(symbol->next && symbol->next->type == plus)break;
			else if(symbol->next && symbol->next->type == minus)break;
			else
			{
				// <formula> ::= <term>
				new_symbol = malloc(sizeof(*new_symbol));
				new_symbol->type = formula;
				new_symbol->component.formula.term = symbol;
				new_symbol->string.initial = symbol->string.initial;
				new_symbol->string.length = symbol->string.length;
				new_symbol->previous = symbol->previous;
				new_symbol->next = symbol->next;
				if(new_symbol->previous)new_symbol->previous->next = new_symbol;
				if(new_symbol->next)new_symbol->next->previous = new_symbol;
				if(symbols.first_symbol == symbol)symbols.first_symbol = new_symbol;
				if(symbols.last_symbol == symbol)symbols.last_symbol = new_symbol;
				symbol->previous = NULL;
				symbol->next = NULL;
				next_symbol = new_symbol;
				flags |= SYNTACTIC_ANALYSIS_FLAG_CHANGED;
				#ifdef DEBUG
				printf("\n<formula> ::= <term>\n");
				print_symbols(symbols);
				#endif
			}
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

