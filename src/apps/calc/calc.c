#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct _Substring
{
	char const *initial;
	size_t length;
} Substring;

typedef struct _Token
{
	Substring string;
	struct _Token *previous;
	struct _Token *next;
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
		token->string.initial = reader;
		token->string.length = 1;
		token->previous = tokens.last_token;
		token->next = NULL;
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
	for(Token *token = tokens.first_token; token; token = token->next)printf("Token \"%0.*s\"\n", token->string.length, token->string.initial);
}

