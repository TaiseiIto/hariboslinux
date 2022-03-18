#include "chain_string.h"
#include "memory.h"
#include "serial.h"

ChainString *create_chain_string(char const *string)
{
	ChainString *chain_string = malloc(sizeof(*chain_string));
	chain_string->first_character = NULL;
	chain_string->last_character = NULL;
	chain_string->length = 0;
	if(string)insert_char_array_front(chain_string, NULL, string);
	return chain_string;
}

ChainString *create_chain_substring(ChainCharacter *first_character, ChainCharacter *last_character)
{
	ChainString *chain_string = malloc(sizeof(*chain_string));
	chain_string->first_character = NULL;
	chain_string->last_character = NULL;
	chain_string->length = 0;
	if(!first_character && !last_character)return chain_string;
	else if(first_character && last_character)for(ChainCharacter *character = first_character; character != last_character->next; character = character->next)insert_char_front(chain_string, NULL, character->character);
	else ERROR(); // The arguments first_character and last_character are inconsistent.
	return chain_string;
}

char *create_char_array_from_chain_string(ChainString const *string)
{
	char *char_array;
	char *writer;
	if(!string)ERROR(); // The string doesn't exist.
	if(!string->length)return NULL;
	char_array = malloc(string->length + 1);
	writer = char_array;
	for(ChainCharacter *position = string->first_character; position; position = position->next)*writer++ = position->character;
	*writer = '\0';
	return char_array;
}

void delete_chain_string(ChainString *string)
{
	if(!string)ERROR(); // The string doesn't exist.
	delete_chars(string, string->first_character, string->length);
	free(string);
}

void delete_char(ChainString *string, ChainCharacter *position)
{
	if(!string || !position)ERROR(); // The string or the position doesn't exist.
	if(!string->length)ERROR(); // There is no char in the string.
	if(position == string->first_character)string->first_character = position->next;
	if(position == string->last_character)string->last_character = position->previous;
	if(position->next)position->next->previous = position->previous;
	if(position->previous)position->previous->next = position->next;
	string->length--;
	free(position);
}

void delete_chars(ChainString *string, ChainCharacter *position, unsigned int length)
{
	ChainCharacter *next_position;
	if(!string)ERROR(); // The string doesn't exitst.
	for(int i = 0; i < length; i++)
	{
		next_position = position->next;
		if(!position)ERROR(); // Deletion length overflows.
		delete_char(string, position);
		position = next_position;
	}
}

void insert_char_front(ChainString *string, ChainCharacter *position, char wedge)
{
	ChainCharacter *new_chain_character;
	if(!string)ERROR(); // The string doesn't exist.
	new_chain_character = malloc(sizeof(*new_chain_character));
	new_chain_character->character = wedge;
	if(!string->first_character && !string->last_character)
	{
		new_chain_character->previous = NULL;
		new_chain_character->next = NULL;
		string->first_character = new_chain_character;
		string->last_character = new_chain_character;
	}
	else if(string->first_character && string->last_character)
	{
		if(!position) // Insert the wedge after the last character.
		{
			new_chain_character->previous = string->last_character;
			new_chain_character->next = NULL;
			string->last_character->next = new_chain_character;
			string->last_character = new_chain_character;
		}
		else
		{
			new_chain_character->previous = position->previous;
			new_chain_character->next = position;
			if(position->previous)position->previous->next = new_chain_character;
			position->previous = new_chain_character;
			if(position == string->first_character)string->first_character = new_chain_character;
		}
	}
	else ERROR(); // The string is broken.
	string->length++;
}

void insert_char_array_front(ChainString *string, ChainCharacter *position, char const *wedge)
{
	for(; *wedge; wedge++)insert_char_front(string, position, *wedge);
}

void insert_chain_string_front(ChainString *string, ChainCharacter *position, ChainString const *wedge)
{
	for(ChainCharacter const *wedge_character = wedge->first_character; wedge_character; wedge_character = wedge_character->next)insert_char_front(string, position, wedge_character->character);
}

void insert_char_back(ChainString *string, ChainCharacter *position, char wedge)
{
	ChainCharacter *new_chain_character;
	if(!string)ERROR(); // The string doesn't exist.
	new_chain_character = malloc(sizeof(*new_chain_character));
	new_chain_character->character = wedge;
	if(!string->first_character && !string->last_character)
	{
		new_chain_character->previous = NULL;
		new_chain_character->next = NULL;
		string->first_character = new_chain_character;
		string->last_character = new_chain_character;
	}
	else if(string->first_character && string->last_character)
	{
		if(!position) // Insert the wedge before the first character
		{
			new_chain_character->previous = NULL;
			new_chain_character->next = string->first_character;
			string->first_character->previous = new_chain_character;
			string->first_character = new_chain_character;
		}
		else
		{
			new_chain_character->previous = position;
			new_chain_character->next = position->next;
			if(position->next)position->next->previous = new_chain_character;
			position->next = new_chain_character;
			if(position == string->last_character)string->last_character = new_chain_character;
		}
	}
	else ERROR(); // The string is broken.
	string->length++;
}

void insert_char_array_back(ChainString *string, ChainCharacter *position, char const *wedge)
{
	for(; *wedge; wedge++)
	{
		insert_char_back(string, position, *wedge);
		if(position)position = position->next;
		else position = string->first_character;
	}
}

void insert_chain_string_back(ChainString *string, ChainCharacter *position, ChainString const *wedge)
{
	for(ChainCharacter const *wedge_character = wedge->first_character; wedge_character; wedge_character = wedge_character->next)
	{
		insert_char_back(string, position, wedge_character->character);
		if(position)position = position->next;
		else position = string->first_character;
	}
}

