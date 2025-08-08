#ifndef SHOW_SORTER_H
#define SHOW_SORTER_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef enum FormatTokenType {
	NONE,
	VARIABLE,
	DEFAULT
} FormatTokenType_t;

typedef struct FormatToken {
	FormatTokenType_t type;
	char* value;
	size_t length;
} FormatToken_t;

typedef struct FormatTokenNode {
	FormatToken_t token;
	struct FormatTokenNode* next;
} FormatTokenNode_t;

typedef struct FormatTokenList {
	FormatTokenNode_t* head;
	FormatTokenNode_t* tail;
	size_t count;
} FormatTokenList_t;

typedef struct Variable {
	char* name;
	size_t name_length;
	char* value;
	size_t value_length;
} Variable_t;

typedef struct VariableNode {
	Variable_t variable;
	struct VariableNode* next;
} VariableNode_t;

typedef struct VariableList {
	VariableNode_t* head;
	VariableNode_t* tail;
	size_t count;
} VariableList_t;

typedef struct StringDictionary {
	char** keys;
	char** values;
	size_t consumed;
	size_t max;
} StringDictionary_t;

typedef struct NameScheme {
	VariableList_t* variables;

	FormatTokenList_t* input_format;
	FormatTokenList_t* output_format;
} NameScheme_t;

FormatTokenNode_t* create_format_token_node(FormatTokenType_t type, char* value, size_t length);

FormatTokenList_t* create_format_token_list(void);
void destroy_format_token_list(FormatTokenList_t* list);
void add_format_token(FormatTokenList_t* list, FormatTokenType_t type, char* value, size_t length);
FormatToken_t* format_token_list_at(FormatTokenList_t* list, size_t index);

VariableList_t* create_variable_list(void);
void destroy_variable_list(VariableList_t* list);
void add_variable(VariableList_t* list, char* name, size_t name_length, char* value, size_t value_length);
Variable_t* variable_list_at(VariableList_t* list, size_t index);

StringDictionary_t* create_string_dictionary(size_t max);
StringDictionary_t* destroy_string_dictionary(StringDictionary_t* dict);
void add_string_to_dictionary(StringDictionary_t* dict, char* key, char* value);
char* get_value_from_dictionary(StringDictionary_t* dict, char* key);

NameScheme_t* create_name_scheme(void);
void destroy_name_scheme(NameScheme_t* scheme);

void load_format(NameScheme_t* scheme, char* input_format);
void parse_file(NameScheme_t* scheme, char* file);

#endif
