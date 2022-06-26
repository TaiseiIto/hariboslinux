#ifndef _CHAIN_STRING_H_
#define _CHAIN_STRING_H_

typedef struct _ChainCharacter
{
	struct _ChainCharacter *previous;
	struct _ChainCharacter *next;
	char character;
} ChainCharacter;

typedef struct _ChainString
{
	ChainCharacter *first_character;
	ChainCharacter *last_character;
	unsigned int length;
} ChainString;

ChainString *create_chain_string(char const *string);
ChainString *create_chain_substring(ChainCharacter *first_character, ChainCharacter *last_character);
char *create_char_array_from_chain_string(ChainString const *string);
ChainString *create_caller_format_chain_string(unsigned int format_arg_pos);
ChainString *create_format_chain_string(char const *format, ...);
char *create_format_char_array(char const *format, ...);
void delete_chain_string(ChainString *string);
void delete_char(ChainString *string, ChainCharacter *position);
void delete_chars(ChainString *string, ChainCharacter *position, unsigned int length);
void insert_char_front(ChainString *string, ChainCharacter *position, char wedge);
void insert_char_array_front(ChainString *string, ChainCharacter *position, char const *wedge);
void insert_chain_string_front(ChainString *string, ChainCharacter *position, ChainString const *wedge);
void insert_char_back(ChainString *string, ChainCharacter *position, char wedge);
void insert_char_array_back(ChainString *string, ChainCharacter *position, char const *wedge);
void insert_chain_string_back(ChainString *string, ChainCharacter *position, ChainString const *wedge);

#endif

