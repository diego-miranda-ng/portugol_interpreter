#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

entry_t *symbol_table;

bool is_first(char non_terminal[], symbol_t sym)
{
	if(strcmp(non_terminal, "funcao"))
		return sym == sym_function;
	if(strcmp(non_terminal, "funcao"))
		return sym == sym_function;
}

bool scan()
{
	if(current_token.lexem.symbol == sym_eof)
		return false;
	read_token();
	return current_token.lexem.symbol == sym_eof;
}

bool verify(symbol_t sym, bool mark_error, bool next)
{
	if(current_token.lexem.symbol == sym)
	{
		if(next)
			scan();
		return true;
	}
	if(mark_error)
		mark(error_log, "\"%s\" encontrado.", current_token.lexem.id);
	return false;
}

bool try_consume(symbol_t sym)
{
	return verify(sym, false, true);
}

void consume_new_line()
{
	while(try_consume(sym_new_line));
}

bool consume(symbol_t sym)
{
	return verify(sym, true, true);
}

bool assert(symbol_t sym)
{
	return verify(sym, true, false);
}

bool try_assert(symbol_t sym)
{
	return verify(sym, false, false);
}

// variável = identificador { “[“ inteiro “]” }.
entry_t *var(entry_t *parent)
{
	entry_t *table;
	entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_var, parent);
	scan();
	type_t *type = NULL;
	type_t *aux_type = NULL;
	if(try_consume(sym_open_bracket))
	{
		value_t length = 0;
		unsigned int size = 0;
		do
		{
			if(assert(sym_integer))
				length = atoi(current_token.lexem.id);
			scan();
			consume(sym_close_bracket);
			if(type == NULL)
			{
				type = create_type(form_array, length, size, NULL, NULL);			
				aux_type = type;
			}
			else
			{
				aux_type->base = create_type(form_array, length, size, NULL, NULL);
				aux_type = aux_type->base;
			}
		}
		while(try_consume(sym_open_bracket));
	}
	new_entry->type = type;
	return new_entry;
}

// declaração_variável = variável { “,” variável } “:” identificador.
entry_t *var_declaration(entry_t *parent)
{
	entry_t *table = NULL;
	entry_t *entry = NULL;
	do
	{
		if(!find_entry(current_token.lexem.id, symbol_table))
		{
			if(table == NULL)
			{
				table = var(parent);
				entry = table;
			}
			else
				add_entry(var(parent), entry);
		}
		else
			mark(error_warning, "Esta variável já foi declarada anteriormente.");
	}
	while(try_consume(sym_comma));
	consume(sym_two_points);
	if(assert(sym_identifier))
	{
		entry = find_entry(current_token.lexem.id, symbol_table);
		type_t *type = entry->type;
		if(entry != NULL && (type->form == form_atomic || type->form == form_record))
		{
			entry_t *entry_aux = table;
			type_t *type_aux;
			while(entry_aux != NULL)
			{
				type_aux = entry_aux->type;
				if(type_aux != NULL)
				{
					while(type_aux->base != NULL)
						type_aux = type_aux->base;
					type_aux->base = type;
				}
				else
					entry_aux->type = type;
				entry_aux = entry_aux->next;
			}
		}
		else
			mark(error_fatal,"Este tipo de variável não é válido");
	}
	scan();
	// add_entry(table, symbol_table);
	return table;
}

// parâmetros = “(“ declaração_variável { “;” declaração_variável } “)”.
entry_t *parameters(entry_t *parent)
{
	consume(sym_open_paren);
	entry_t *table = NULL;
	entry_t *entry = NULL;
	do
	{
		if(table == NULL)
		{
			table = var_declaration(parent);
			entry = table;
		}
		else
			add_entry(var_declaration(parent), entry);
	}
	while(try_consume(sym_semicolon));
	consume(sym_close_paren);
	return table;
}

void declarations(entry_t *parent)
{
	consume_new_line();
	add_entry(var_declaration(parent), symbol_table);
}

// bloco =  { ( chamada_função | atribuição | declarações | retorno | se | enquanto | para | faça | caso | repita | até ) “⏎” }.
void block()
{
	declarations(symbol_table->parent);
}

// declaração_função = “funcao” identificador [ parâmetros ] [ “:” identificador ] “⏎” bloco “fim_funcao” “⏎”.
void function_declaration()
{
	consume_new_line();
	consume(sym_function);
	if(assert(sym_identifier))
	{
		entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_function, symbol_table->parent);
		add_entry(new_entry, symbol_table);
		scan();
		if(try_assert(sym_open_paren))
			symbol_table = parameters(symbol_table);
		if(try_consume(sym_two_points))
		{
			if(!assert(sym_identifier))
				return;
			if(find_entry(current_token.lexem.id, symbol_table))
			{
				scan();
				if(!consume(sym_new_line))
					return;
				block();
				consume(sym_end_function);
			}
			else
				mark(error_warning, "O tipo \"%s\" é inválido para a declaração de funções.", current_token.lexem.id);
		}
		symbol_table = symbol_table->next->parent;
	}
}

// programa = “algoritmo” identificador “⏎” { declaração_função } bloco “fim_algoritmo”.
void program()
{
	consume_new_line();
	consume(sym_algorith);
	consume(sym_identifier);
	consume(sym_new_line);
	while(try_assert(sym_identifier) || try_assert(sym_function))
	{
		if(try_assert(sym_identifier))
			declarations(NULL);
		else if(try_assert(sym_function))
			function_declaration();
		consume(sym_new_line);
	}
}

bool initialize_parser(FILE *file)
{
	symbol_table = initialize_table(0);
	if(symbol_table == NULL)
		return false;

	initialize_scanner(file);
	read_token();
	return current_token.lexem.symbol != sym_eof;
}

bool parse()
{
	program();
	return true;
}
