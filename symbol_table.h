
#include <stdbool.h>
#include "scanner.h"

typedef unsigned short address_t;
typedef signed char value_t;

// Diferencia as classes dos elementos entre variáveis, consstantes,
// tipos, procedimentos e desconhecidos.
/*-----------------------------------------------------------------------------------------*/
typedef enum _class
{
	class_unknown,
	class_var,
	class_const,
	class_type,
	class_function
} class_t;

// Diferencia os elementos entre tipos estruturados (Vetor e Registros).
/*-----------------------------------------------------------------------------------------*/
typedef enum _form
{
	form_atomic,
	form_array,
	form_record
} form_t;

struct _entry;

// Armazena um tipo de dados de um identificador.
/*-----------------------------------------------------------------------------------------*/
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
/*-----------------------------------------------------------------------------------------*/
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

// Representa uma tabela de símbolos.
/*-----------------------------------------------------------------------------------------*/
typedef struct _symbol_table
{
	entry_t *entries;
	struct _symbol_table *parent;
} symbol_table_t;

extern address_t current_address;
extern entry_t *integer_type;
extern entry_t *boolean_type;
extern entry_t *float_type;
extern entry_t *string_type;

// Limpa uma tabela de símbolos recebendo como parâmetro a tabela de símbolos.
/*-----------------------------------------------------------------------------------------*/
bool clear_table(symbol_table_t *table);

// cria uma entrada para uma tabela de símbolos.
// identifier_t id é o nome do identificador.
// position_t position recebe a posição do identificador no código-fonte.
// class_t class recebe a classe do identificador.
// Retorna a entrada criada ou NULL se ela não for criada.
/*-----------------------------------------------------------------------------------------*/
entry_t *create_entry(identifier_t id, position_t position, class_t class);

// Esta função cria um tipo de dados.
// form_t form é a forma do tipo de dados.
// value_t length é o comprimento do tipo de dados.
// int size é o tamanho do tipo de dados.
// entry_t *fields é utilizado caso serja um registro para assim armazenar seus campos.
// type_t *base armana o tipo de dados base para o que será criado.
// Retorna o tipo de dados criado ou NULL se não for criada.
/*-----------------------------------------------------------------------------------------*/
type_t *create_type(form_t form, value_t length, unsigned int size, entry_t *fields, type_t *base);

// Encontra uma entrada em na tabela de símbolos.
// identifier_t id é o nome do identificador a ser encontrado.
// symbol_table_t *table é a tabela de símbolos onde será procurado.
// bool search_parent é para indicar com true se é para procurar nos escopos acima ou false.
// Retorna a entrada se encontrada ou NULLcaso contrário.
/*-----------------------------------------------------------------------------------------*/
entry_t *find_entry(identifier_t id, symbol_table_t *table, bool search_parent);

// Esta função possui a função de encontrar um campo em uma lista de entradas.
// identifier_t id é o nome do identificador a ser encontrado.
// entry_t *entry é a lista de entradas.
/*-----------------------------------------------------------------------------------------*/
entry_t *find_field(identifier_t id, entry_t *entry);

// Esta função adiciona uma entrada a uma tabela de símbolos.
// entry_t *entry é a entrada a ser adicionada.
// symbo_tale_t *table é a tabela de símbolos onde será adicionada a entrada.
/*-----------------------------------------------------------------------------------------*/
bool add_entry(entry_t *entry, symbol_table_t *table);

// Cria uma tabela de símbolos passando por parâmentro a tabela de símbolos que ela pertence
/*-----------------------------------------------------------------------------------------*/
symbol_table_t *create_table(symbol_table_t *parent);

// Esta função inicia a tabela de símbolos.
/*-----------------------------------------------------------------------------------------*/
symbol_table_t *initialize_table();
