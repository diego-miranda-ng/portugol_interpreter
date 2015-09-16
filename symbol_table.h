#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short address_t;
typedef signed char value_t;

// Diferencia as classes dos elementos entre variáveis,
// consstantes, tipos, procedimentos e desconhecidos.
typedef enum _class
{
	class_unknown,
	class_var,
	class_const,
	class_type,
	class_function
} class_t;

// Diferencia os elementos entre tipos estruturados (Vetor e Registros).
typedef enum _form
{
	form_atomic,
	form_array,
	form_record
} form_t;

struct _entry;

typedef struct _type
{
	form_t form;
	value_t length;
	unsigned int size;
	struct _entry *fields;
	struct _type *base;
} type_t;

struct _symbol_table;

// Representa uma entrada da tabela de simbolos.
typedef struct _entry
{
	identifier_t id;
	position_t position;
	class_t class;
	value_t value;
	struct _type *type;
	struct _entry *next;
	struct _symbol_table *children;
} entry_t;

typedef struct _symbol_table
{
	entry_t *entries;
	struct _symbol_table *parent;
} symbol_table_t;

typedef enum _addressing
{
	addressing_unknown,
	addressing_direct,
	addressing_immediate,
	addressing_register,
	addressing_indirect,
	addressing_condition
} addressing_t;

address_t current_address;
entry_t *integer_type;
entry_t *boolean_type;
entry_t *float_type;
entry_t *string_type;

bool clear_table(symbol_table_t *table)
{
	entry_t *entry = table->entries, *entry_aux;
	while(entry != NULL)
	{
		entry_aux = entry;
		entry = entry->next;
		free(entry_aux);
	}
	table->entries = NULL;
}

entry_t *create_entry(identifier_t id, position_t position, class_t class)
{
	entry_t *new_entry = (entry_t *)malloc(sizeof(entry_t));
	if(!new_entry)
	{
		mark(error_fatal, "Não há memória suficiente.");
		return NULL;
	}
	strcpy(new_entry->id, id);
	new_entry->position = position;
	new_entry->class = class;
	new_entry->type = NULL;
	new_entry->value = 0;
	new_entry->next = NULL;
	new_entry->children = NULL;
	return new_entry;
}

type_t *create_type(form_t form, value_t length, unsigned int size, entry_t *fields, type_t *base)
{
	type_t *type = (type_t *)malloc(sizeof(type_t));
	if(!type)
	{
		mark(error_fatal, "Não há memória suficiente.");
		return NULL;
	}
	type->form = form;
	type->length = length;
	type->size = size;
	type->fields = fields;
	type->base = base;
	return type;
}

entry_t *create_elementary_type(identifier_t id)
{
	entry_t *entry = create_entry(id, position_zero, class_type);
	if(!entry)
	{
		mark(error_fatal, "Não há memória suficiente.");
		return NULL;
	}
	type_t *type = create_type(form_atomic, 0, sizeof(value_t), NULL, NULL);
	if(!type)
	{
		mark(error_fatal, "Não há memória suficiente.");
		free(entry);
		return NULL;
	}
	entry->type = type;
	return entry;
}

entry_t *find_entry(identifier_t id, symbol_table_t *table, bool search_parent)
{
	if(table == NULL)
		return NULL;
	entry_t *entry_aux;
	symbol_table_t *table_aux = table;
	if(table->entries != NULL)
		entry_aux = table->entries;
	else if(table->parent != NULL && search_parent == true)
	{
		table_aux = table->parent;
		entry_aux = table_aux->entries;
	}
	else
		return NULL;

	do
	{
		if(entry_aux->id != NULL && !strcmp(entry_aux->id, id))
			break;
		if((entry_aux == NULL || (entry_aux->next == NULL && table_aux->parent != NULL)) && search_parent == true)
		{
			table_aux = table_aux->parent;
			entry_aux = table_aux->entries;
		}
		else
		{
			entry_aux = entry_aux->next;
		}
	}
	while(entry_aux != NULL);
	return entry_aux;
}

entry_t *find_field(identifier_t id, entry_t *entry)
{
	if(entry == NULL)
		return NULL;

	entry_t *entry_aux = entry->type->fields;
	do
	{
		if(!strcmp(entry_aux->id, id))
			break;
		else
			entry_aux = entry_aux->next;
	}
	while(entry_aux != NULL);
	return entry_aux;
}

bool add_entry(entry_t *entry, symbol_table_t *table)
{
	if(entry == NULL || table == NULL)
		return false;

	if(table->entries == NULL)
	{
		table->entries = entry;
	}
	else
	{
		entry_t *entry_aux = table->entries;
		while(entry_aux->next != NULL)
			entry_aux = entry_aux->next;
		entry_aux->next = entry;
	}
	return true;
}

symbol_table_t *create_table(symbol_table_t *parent)
{
	symbol_table_t *table = malloc(sizeof(symbol_table_t));
	table->parent = parent;
	table->entries = NULL;
	return table;
}

symbol_table_t *initialize_table()
{
	symbol_table_t *table = create_table(NULL);
	integer_type = create_elementary_type("inteiro");
	float_type = create_elementary_type("decimal");
	boolean_type = create_elementary_type("logico");
	string_type = create_elementary_type("texto");
	if(!integer_type || !float_type || !boolean_type || !string_type)
		return NULL;

	add_entry(integer_type, table);
	add_entry(float_type, table);
	add_entry(boolean_type, table);
	add_entry(string_type, table);
	return table;
}
