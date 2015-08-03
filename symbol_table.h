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
	class_proc
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

// Representa uma entrada da tabela de simbolos.
typedef struct _entry
{
	identifier_t id;
	position_t position;
	address_t address;
	class_t class;
	value_t value;
	struct _type *type;
	struct _entry *next;
} entry_t;

typedef enum _addressing
{
	addressing_unknown,
	addressing_direct,
	addressing_immediate,
	addressing_register,
	addressing_indirect,
	addressing_condition
} addressing_t;

entry_t *symble_table = NULL;

address_t current_address;
entry_t *integer_type;
entry_t *boolean_type;
entry_t *float_type;
entry_t *string_type;

bool clear_table(entry_t *table)
{
}

entry_t *create_entry(identifier_t id, position_t position, class_t class)
{
	entry_t *new_entry = (entry_t *)malloc(sizeof(entry_t));
	if(!new_entry)
	{
		printf("\n\nERRO!!\n\n"); // A memória não foi alocada.
		return NULL;
	}
	strcpy(new_entry->id, id);
	new_entry->position = position;
	new_entry->address = 0;
	new_entry->class = class;
	new_entry->type = NULL;
	new_entry->value = 0;
	new_entry->next = NULL;
	return new_entry;
}

type_t *create_type(form_t form, value_t length, unsigned int size, entry_t *fields, type_t *base)
{
	type_t *type = (type_t *)malloc(sizeof(type_t));
	if(!type)
	{
		printf("\n\nERRO!!\n\n"); // A memória não foi alocada.
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
		printf("\n\nERRO!!\n\n"); // Entry não foi criado.
		return NULL;
	}
	type_t *type = create_type(form_atomic, 0, sizeof(value_t), NULL, NULL);
	if(!type)
	{
		printf("\n\nERRO!!\n\n"); // Type não foi criado.
		free(entry);
		return NULL;
	}
	entry->type = type;
	return entry;
}

entry_t *find_entry(identifier_t id, entry_t *table)
{
	if(table == NULL)
		return NULL;
	entry_t *table_aux = table;
	while(table_aux != NULL)
	{
		if(!strcmp(table_aux->id, id))
			break;
		table_aux = table_aux->next;
	}
	return table_aux;
}

bool add_entry(entry_t *entry, entry_t *table)
{
	if(entry == NULL)
		return false;
		
	if(find_entry(entry->id, table))
	{
		mark_at(error_parser, entry->position, "O identificador \"%s\" foi anteriormente declarado.", entry->id);
		return false;
	}

	entry_t *table_aux = table;
	while(table_aux->next != NULL)
		table_aux = table_aux->next;
	table_aux->next = entry;
	
	return true;
}


entry_t *initialize_table(address_t base_address)
{
	entry_t *table;
	current_address = base_address;
	integer_type = create_elementary_type("inteiro");
	float_type = create_elementary_type("decimal");
	boolean_type = create_elementary_type("logico");
	string_type = create_elementary_type("texto");
	if(!integer_type || !float_type || !boolean_type || !string_type)
		return NULL;
	// add_entry(integer_type, table);
	table = integer_type;
	add_entry(float_type, table);
	add_entry(boolean_type, table);
	add_entry(string_type, table);
	return table;
}

void show_content(entry_t *table)
{
	if(table == NULL)
		return;

	printf("\n%s\n", table->id);

	entry_t *table_aux = table;
	while(table_aux != NULL)
	{
		printf("ID: %s", table_aux->id);
		printf("\n----------------------------------------------------\n\n");
		table_aux = table_aux->next;
	}
}
