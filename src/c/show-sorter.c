#include "show-sorter.h"

FormatTokenNode_t* create_format_token_node(FormatTokenType_t type, char* value, size_t length) {
	FormatTokenNode_t* node = (FormatTokenNode_t*)malloc(sizeof(FormatTokenNode_t));
	if (node == NULL) {
		return NULL;
	}

	node->token.type = type;
	node->token.value = (char*)malloc(length + 1);
	if (node->token.value == NULL) {
		free(node);
		return NULL;
	}

	strncpy(node->token.value, value, length);
	node->token.value[length] = '\0'; // Null-terminate the string
	node->token.length = length;

	node->next = NULL;

	return node;
}

FormatTokenList_t* create_format_token_list(void) {
	FormatTokenList_t* list = (FormatTokenList_t*)malloc(sizeof(FormatTokenList_t));
	if (list == NULL) {
		return NULL;
	}
	
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;

	return list;
}
void destroy_format_token_list(FormatTokenList_t* list) {
	if (list == NULL) {
		return;
	}

	FormatTokenNode_t* current = list->head;
	while (current != NULL) {
		FormatTokenNode_t* next = current->next;

		free(current->token.value); // Should we free values not malloced by this structure?
		free(current);

		current = next;
	}

	free(list);
}
void add_format_token(FormatTokenList_t* list, FormatTokenType_t type, char* value, size_t length) {
	if (list == NULL || value == NULL || length == 0) {
		return;
	}

	FormatTokenNode_t* new_node = create_format_token_node(type, value, length);
	if (new_node == NULL) {
		return;
	}

	new_node->next = NULL;
	if (list->tail != NULL) {
		list->tail->next = new_node;
	}
	list->tail = new_node;
	if (list->head == NULL) {
		list->head = new_node;
	}
	list->count++;
}
FormatToken_t* format_token_list_at(FormatTokenList_t* list, size_t index) {
	if (list == NULL || index >= list->count) {
		return NULL;
	}

	index %= list->count;
	if (index < 0) {
		index += list->count;
	}

	FormatTokenNode_t* current = list->head;
	for (size_t i = 0; i < index; i++) {
		current = current->next;
	}

	return &current->token;
}

VariableList_t* create_variable_list(void) {
	VariableList_t* list = (VariableList_t*)malloc(sizeof(VariableList_t));
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->count = 0;

	return list;
}
void destroy_variable_list(VariableList_t* list) {
	if (list == NULL) {
		return;
	}

	VariableNode_t* current = list->head;
	while (current != NULL) {
		VariableNode_t* next = current->next;
	
		free(current->variable.name);  // Should we free names not malloced by this structure?
		free(current->variable.value); // ^^^
		free(current);

		current = next;
	}
	free(list);
}
void add_variable(VariableList_t* list, char* name, size_t name_length, char* value, size_t value_length) {
	if (list == NULL ||
		name == NULL || name_length == 0 ||
		value == NULL || value_length == 0) {
		return;
	}

	VariableNode_t* new_node = (VariableNode_t*)malloc(sizeof(VariableNode_t));
	if (new_node == NULL) {
		return;
	}

	new_node->variable.name = (char*)malloc(name_length + 1);
	if (new_node->variable.name == NULL) {
		free(new_node);
		return;
	}
	strncpy(new_node->variable.name, name, name_length);
	new_node->variable.name[name_length] = '\0';
	new_node->variable.name_length = name_length;

	new_node->variable.value = (char*)malloc(value_length + 1);
	if (new_node->variable.value == NULL) {
		free(new_node->variable.name);
		free(new_node);
		return;
	}
	strncpy(new_node->variable.value, value, value_length);
	new_node->variable.value[value_length] = '\0';
	new_node->variable.value_length = value_length;

	new_node->next = NULL;
	if (list->tail != NULL) {
		list->tail->next = new_node;
	}
	list->tail = new_node;
	if (list->head == NULL) {
		list->head = new_node;
	}
	list->count++;
}
Variable_t* variable_list_at(VariableList_t* list, size_t index) {
	if (list == NULL || index >= list->count) {
		return NULL;
	}

	index %= list->count;
	if (index < 0) {
		index += list->count;
	}

	VariableNode_t* current = list->head;
	for (size_t i = 0; i < index; i++) {
		current = current->next;
	}

	return &current->variable;
}

StringDictionary_t* create_string_dictionary(size_t max) {
	StringDictionary_t* dict = (StringDictionary_t*)malloc(sizeof(StringDictionary_t));
	if (dict == NULL) {
		return NULL;
	}

	dict->keys = (char**)malloc(sizeof(char*) * max);
	dict->values = (char**)malloc(sizeof(char*) * max);
	if (dict->keys == NULL || dict->values == NULL) {
		free(dict->keys);
		free(dict->values);
		free(dict);
		return NULL;
	}

	dict->consumed = 0;
	dict->max = max;

	return dict;
}
StringDictionary_t* destroy_string_dictionary(StringDictionary_t* dict) {
	if (dict == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < dict->consumed; i++) {
		free(dict->keys[i]);
		free(dict->values[i]);
	}
	free(dict->keys);   // Should we free keys and values not malloced by this structure?
	free(dict->values); // ^^^
	free(dict);

	return NULL;
}
void resize_dictionary(StringDictionary_t* dict, size_t new_max) {
	if (dict == NULL || new_max <= dict->max) {
		return;
	}

	char** new_keys = (char**)realloc(dict->keys, sizeof(char*) * new_max);
	char** new_values = (char**)realloc(dict->values, sizeof(char*) * new_max);
	if (new_keys == NULL || new_values == NULL) {
		return;
	}

	dict->keys = new_keys;
	dict->values = new_values;
	dict->max = new_max;
}
void add_string_to_dictionary(StringDictionary_t* dict, char* key, char* value) {
	if (dict == NULL || key == NULL || value == NULL || dict->consumed >= dict->max) {
		return;
	}

	if (dict->consumed == dict->max) {
		resize_dictionary(dict, dict->max * 2);
		if (dict->keys == NULL || dict->values == NULL) {
			return;
		}
	}

	dict->keys[dict->consumed] = malloc(strlen(key) + 1);
	if (dict->keys[dict->consumed] == NULL) {
		return;
	}
	strcpy(dict->keys[dict->consumed], key);

	dict->values[dict->consumed] = malloc(strlen(value) + 1);
	if (dict->values[dict->consumed] == NULL) {
		free(dict->keys[dict->consumed]);
		return;
	}
	strcpy(dict->values[dict->consumed], value);

	dict->consumed++;
}
char* get_value_from_dictionary(StringDictionary_t* dict, char* key) {
	if (dict == NULL || key == NULL) {
		return NULL;
	}

	for (size_t i = 0; i < dict->consumed; i++) {
		if (strcmp(dict->keys[i], key) == 0) {
			return dict->values[i];
		}
	}

	return NULL;
}

NameScheme_t* create_name_scheme(void) {
	NameScheme_t* scheme = (NameScheme_t*)malloc(sizeof(NameScheme_t));
	if (scheme == NULL) {
		return NULL;
	}

	scheme->variables     = NULL;
	scheme->input_format  = NULL;
	scheme->output_format = NULL;

	return scheme;
}
void destroy_name_scheme(NameScheme_t* scheme) {
	if (scheme == NULL) {
		return;
	}

	destroy_variable_list(scheme->variables);
	destroy_format_token_list(scheme->input_format);
	destroy_format_token_list(scheme->output_format);

	free(scheme);
}

void handle_buffer_window_resizing(char** buffer, size_t* buffer_window, size_t buffer_size) {
	if (buffer_size >= *buffer_window) {
		*buffer_window *= 2;
		char* new_buffer = (char*)realloc(*buffer, *buffer_window);
		if (new_buffer == NULL) {
			free(*buffer);
			*buffer = NULL;
			return;
		}
		*buffer = new_buffer;
	}
}

void load_format(NameScheme_t* scheme, char* file) {
	if (scheme == NULL || file == NULL) {
		return;
	}

	destroy_variable_list(scheme->variables);
	scheme->variables = create_variable_list();

	destroy_format_token_list(scheme->input_format);
	scheme->input_format = create_format_token_list();

	destroy_format_token_list(scheme->output_format);
	scheme->output_format = create_format_token_list();

	// read the file
	FILE* fd = fopen(file, "r");
	if (fd == NULL) {
		return;
	}

	size_t file_size;
	fseek(fd, 0, SEEK_END);
	file_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	char* buffer = (char*)malloc(file_size + 1);
	if (buffer == NULL) {
		fclose(fd);
		return;
	}

	fread(buffer, sizeof(char), sizeof(char) * file_size, fd);
	buffer[file_size] = '\0';

	fclose(fd);
	
	// parse the file content
	char* input_buffer = strtok(buffer, "\n");
	char* output_buffer = strtok(NULL, "\n");

	FormatTokenType_t type;
	char* token = malloc(sizeof(char) * 100);
	size_t token_length = 0;
	size_t current_token_window = 100;

	for (size_t i = 0; i < file_size; i++) {
		if (type == NONE) {
			if (input_buffer[i] == '<' &&
				(i == 0 || input_buffer[i - 1] != '\\')) {
				type = VARIABLE;
				token_length = 0;
			} else {
				type = DEFAULT;
				handle_buffer_window_resizing(&token, &current_token_window, &token_length);
				token[token_length] = input_buffer[i];
				token_length++;
			}
		} else if (type == VARIABLE) {
			if (input_buffer[i] == '>') {
				type = NONE;
				token[token_length] = '\0';
				token_length++;
				add_variable(scheme->variables, token, token_length, NULL, 0);
				token_length = 0;
			} else {
				handle_buffer_window_resizing(&token, &current_token_window, token_length + 1);
				token[token_length] = input_buffer[i];
				token_length++;
			}
		} else if (type == DEFAULT) {
			if (input_buffer[i] == '<' &&
				(i == 0 || input_buffer[i - 1] != '\\')) {
				type = VARIABLE;
				add_format_token(scheme->input_format, DEFAULT, token, token_length);
				token_length = 0;
			} else {
				handle_buffer_window_resizing(&token, &current_token_window, token_length + 1);
				token[token_length] = input_buffer[i];
				token_length++;
			}
		}
	}

	if (token_length > 0) {
		if (type == VARIABLE) {
			// error: variable not closed
		} else if (type == DEFAULT) {
			token[token_length] = '\0';
			add_format_token(scheme->input_format, DEFAULT, token, token_length);
		}
	}

	type = NONE;
	token_length = 0;

	for (size_t i = 0; i < file_size; i++) {
		if (type == NONE) {
			if (output_buffer[i] == '<' &&
			(i == 0 || output_buffer[i - 1] != '\\')) {
				type = VARIABLE;
				token_length = 0;
			} else {
				type = DEFAULT;
				handle_buffer_window_resizing(&token, &current_token_window, &token_length);
				token[token_length] = output_buffer[i];
				token_length++;
			}
		} else if (type == VARIABLE) {
			if (output_buffer[i] == '>') {
				type = NONE;
				token[token_length] = '\0';
				token_length++;
				bool variable_exists = false;
				for (size_t j = 0; j < scheme->variables->count; j++) {
					VariableNode_t* var_node = scheme->variables->head;
					while (var_node != NULL) {
						if (strncmp(var_node->variable.name, token, token_length) == 0) {
							variable_exists = true;
							break;
						}
						var_node = var_node->next;
					}
				}
				if (!variable_exists) {
					// error: variable not defined
				}
				add_format_token(scheme->output_format, VARIABLE, token, token_length);
				token_length = 0;
			} else {
				handle_buffer_window_resizing(&token, &current_token_window, token_length + 1);
				token[token_length] = output_buffer[i];
				token_length++;
			}
		} else if (type == DEFAULT) {
			if (output_buffer[i] == '<' &&
			(i == 0 || output_buffer[i - 1] != '\\')) {
				type = VARIABLE;
				token[token_length] = '\0';
				token_length++;
				add_format_token(scheme->output_format, DEFAULT, token, token_length);
				token_length = 0;
			} else {
				handle_buffer_window_resizing(&token, &current_token_window, token_length + 1);
				token[token_length] = output_buffer[i];
				token_length++;
			}
		}
	}

	free(token);
	free(buffer);
}

char* parse_file(NameScheme_t* scheme, char* file) {
	if (scheme == NULL || file == NULL) {
		return NULL;
	}

	FormatTokenType_t extracting_type = NONE;

	char* extracting = (char*)malloc(sizeof(char) * 100);
	if (extracting == NULL) {
		return NULL;
	}

	size_t extracting_length = 0;
	size_t extracting_window = 100;

	char* extension = strrchr(file, '.');

	size_t index = 0;
	for (size_t i = 0; i < scheme->input_format->count; i++) {
		if (extracting_type == VARIABLE) {
			if (format_token_list_at(scheme->input_format, i)->type == VARIABLE) {
				extracting_type = VARIABLE;
				extracting_length = 0;
				if (i == scheme->input_format->count - 1) {
					// ...
				}
			}
		}
	}
}
