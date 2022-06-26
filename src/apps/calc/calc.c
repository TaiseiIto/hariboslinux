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

typedef struct _Token
{
	struct _Token *previous;
	struct _Token *next;
	Substring string;
	NonterminalSymbol symbol;
} Token;

typedef struct _Tokens
{
	Token *first_token;
	Token *last_token;
} Tokens;

char *combine_argv(int argc, char const * const * const argv);
void delete_tokens(Tokens tokens);
Tokens lexical_analysis(char const * const input_string);
void print_tokens(Tokens tokens);
NonterminalSymbol substring2symbol(Substring substring);
char const *symbol_name(NonterminalSymbol symbol);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	Tokens tokens = lexical_analysis(input_string);
	print_tokens(tokens);
	delete_tokens(tokens);
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

void delete_tokens(Tokens tokens)
{
	Token *next_token;
	for(Token *token = tokens.first_token; token; token = next_token)
	{
		next_token = token->next;
		free(token);
	}
}

Tokens lexical_analysis(char const * const input_string)
{
	Tokens tokens;
	tokens.first_token = NULL;
	tokens.last_token = NULL;
	for(char const *reader = input_string; *reader; reader++)
	{
		Token *token = malloc(sizeof(*token));
		token->previous = tokens.last_token;
		token->next = NULL;
		token->string.initial = reader;
		token->string.length = 1;
		token->symbol = substring2symbol(token->string);
		if(!tokens.first_token)tokens.first_token = token;
		if(tokens.last_token)
		{
			tokens.last_token->next = token;
			tokens.last_token = token;
		}
		else tokens.last_token = token;
	}
	return tokens;
}

void print_tokens(Tokens tokens)
{
	for(Token *token = tokens.first_token; token; token = token->next)printf("%s \"%0.*s\"\n", symbol_name(token->symbol), token->string.length, token->string.initial);
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

