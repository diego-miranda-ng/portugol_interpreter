#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

symbol_table_t *symbol_table;

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
	{
		if(sym == sym_identifier)
			mark(error_log, "Era esperado um identificador mas foi encontrado \"%s\".", current_token.lexem.id);
		else if(sym == sym_integer)
			mark(error_log, "Era esperado um inteiro mas foi encontrado \"%s\".", current_token.lexem.id);
		else if(sym == sym_new_line)
			mark(error_log, "Era esperado uma quebra de linha mas foi encontrado \"%s\".", current_token.lexem.id);
		else
			mark(error_log, "Era esperado \"%s\" mas foi encontrado \"%s\".", find_symbol(sym), current_token.lexem.id);
	}
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
entry_t *var()
{
	entry_t *table;
	entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_var);
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
void var_declaration()
{
	consume_new_line();
	entry_t *table = NULL;
	entry_t *entry = NULL;
	do
	{
		if(!find_entry(current_token.lexem.id, symbol_table))
		{
			if(table == NULL)
			{
				table = var();
				entry = table;
			}
			else
			{
				entry->next = var();
				entry = entry->next;
			}
		}
		else
			mark(error_warning, "O identificador \"%s\" já foi declarada anteriormente.", current_token.lexem.id);
	}
	while(try_consume(sym_comma));
	consume(sym_two_points);
	if(assert(sym_identifier))
	{
		entry = find_entry(current_token.lexem.id, symbol_table);
		if(entry != NULL)
		{
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
			scan();
			add_entry(table, symbol_table);
		}
		else
		{
			mark(error_fatal, "Era esperado um tipo de variável mas foi encontrado \"%s\".", current_token.lexem.id);
		}
	}
}

// parâmetros = “(“ declaração_variável { “;” declaração_variável } “)”.
void parameters()
{
	consume(sym_open_paren);
	do
	{
		var_declaration();
	}
	while(try_consume(sym_semicolon));
	consume(sym_close_paren);
}

void expression();

// argumentos = “(“ [ expressão { “,” expressão } ] “)”.
void arguments()
{
	consume(sym_open_paren);	
	if(try_assert(sym_identifier)
	|| try_assert(sym_float)
	|| try_assert(sym_string)
	|| try_assert(sym_integer)
	|| try_assert(sym_open_paren)
	|| try_assert(sym_not))
	{
		do
		{
			expression();
		}
		while(try_consume(sym_comma));
	}
	consume(sym_close_paren);
}

// chamada_função = identificador argumentos.
void function_call()
{
	consume(sym_identifier);
	arguments();
}

// seletor = { “.” identificador | “[“ expressão “]” }.
void selector()
{
	if(try_consume(sym_dot))
		consume(sym_identifier);
	else if(try_consume(sym_open_bracket))
	{
		expression();
		consume(sym_close_bracket);
	}
}

// fator = texto | inteiro | decimal | chamada_função | identificador seletor | “(“ expressão “)” | “nao” fator.
void factor()
{
	if(try_assert(sym_identifier))
	{
		entry_t *entry = find_entry(current_token.lexem.id, symbol_table);
		if(entry != NULL)
		{
			if(entry->class == class_var || entry->class == class_const || entry->class == class_function)
			{
				if(entry->class == class_function)
				{
					function_call();
				}
				else
				{
					scan();
					selector();
				}
			}
			else
			{
				mark(error_parser, "\"%s\" não é um fator válido válido.", current_token.lexem.id);
				scan();
			}
		}
		else
		{
			mark(error_parser, "\"%s\" não foi declarado anteriormente.", current_token.lexem.id);
			scan();
		}
	}
	else if(try_consume(sym_open_paren))
	{
		expression();
		consume(sym_close_paren);
	}
	else if(try_consume(sym_not))
	{
		factor();
		return;
	}
	else if(try_consume(sym_string) || try_consume(sym_integer) || try_consume(sym_float))
		return;
	else
		mark(error_parser, "Fator perdido.");
}

// termo = fator { ( “*” | “/” | “e” ) fator }.
void term()
{
	factor();
	while(try_consume(sym_times) || try_consume(sym_division) || try_consume(sym_and))
		factor();
}

// expressão_simples = [ “+” | “-” ] termo { ( “+” | “-” | “ou” ) termo }.
void simple_expression()
{
	if(strcmp(current_token.lexem.id, "+"))
		try_consume(sym_plus);
	else if(strcmp(current_token.lexem.id, "+"))
		try_consume(sym_minus);
	term();
	while(try_consume(sym_plus) || try_consume(sym_minus))
		term();
}

// expressão = expressão_simples { ( “<” | “>” | “<=” | “>=” | “==” ) expressão_simples}.
void expression()
{
	simple_expression();
	while(try_consume(sym_less) || try_consume(sym_greater) || try_consume(sym_less_equal) || try_consume(sym_greater_equal) ||	try_consume(sym_equal))
		simple_expression();
}

// declaração_tipo = “tipo” identificador “=” ( identificador | “registro” “⏎” { declaração_variável “⏎” } “fim_registro” ).
void type_declaration()
{
	consume_new_line();
	consume(sym_type);
	if(assert(sym_identifier))
	{
		entry_t *entry = NULL;
		entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_unknown);
		scan();
		consume(sym_allocation);
		if(try_assert(sym_identifier))
		{
			entry = find_entry(current_token.lexem.id, symbol_table);
			if(entry != NULL)
				new_entry->type = entry->type;
			else
				mark(error_parser, "O identificador \"%s\" não foi declarado.", current_token.lexem.id);
			scan();
		}
		else if(try_assert(sym_registry))
		{
			scan();
			consume(sym_new_line);
			unsigned int size = 0;
			if(try_assert(sym_identifier))
			{
				symbol_table = create_table(symbol_table);
				do
				{
					var_declaration();
					consume(sym_new_line);
				}
				while(try_assert(sym_identifier));
				entry = symbol_table->entry;
				if(entry != NULL)
				{
					entry_t *e = entry;
					while (e != NULL && e->type != NULL)
					{
						size += e->type->size;
						e = e->next;
					}
				}
				symbol_table->parent->childrens = symbol_table;
				symbol_table = symbol_table->parent;
			}
			consume(sym_end_registry);
			new_entry->type = create_type(form_record, 0, size, entry, NULL);
		}
		else
			mark(error_fatal, "Durante a adeclaração de um tipo era esperado um identificador ou a palavra-chave registro mas foi encontrado \"%s\".", current_token.lexem.id);
		consume(sym_new_line);
	}
	else
		mark(error_fatal, "\"%s\" é inválido como identificador de uma função.", current_token.lexem.id);
}

// declaração_constante = “constante” identificador “=” expressão.
void const_declaration()
{
	consume_new_line();
	consume(sym_const);
	if(assert(sym_identifier))
	{
		if(find_entry(current_token.lexem.id, symbol_table) == NULL)
		{
			entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_const);
			add_entry(new_entry, symbol_table);
			consume(sym_identifier);
			consume(sym_allocation);
			expression();
		}
		else
			mark(error_warning, "O identificador \"%s\" já foi declarado anteriormente.", current_token.lexem.id);
	}
	else
		mark(error_fatal, "O identificador \"%s\" é inválido.", current_token.lexem.id);
}

// declarações =  declaração_tipo | declaração_constante | declaração_variável.
void declarations()
{
	consume_new_line();
	if(try_assert(sym_identifier))
		var_declaration();
	else if(try_assert(sym_const))
		const_declaration();
	else if(try_assert(sym_type))
		type_declaration();
}

// bloco =  { ( chamada_função | atribuição | declarações | retorno | se | enquanto | para | faça | caso | repita | até ) “⏎” }.
void block()
{
	consume_new_line();
	if(current_token.lexem.symbol == sym_identifier)
	{
		declarations();
		consume(sym_new_line);
	}
}

// declaração_função = “funcao” identificador [ parâmetros ] [ “:” identificador ] “⏎” bloco “fim_funcao” “⏎”.
void function_declaration()
{
	consume_new_line();
	consume(sym_function);
	if(assert(sym_identifier))
	{
		if(find_entry(current_token.lexem.id, symbol_table) == NULL)
		{
			entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_function);
			add_entry(new_entry, symbol_table);
			scan();
			symbol_table = create_table(symbol_table);
			if(try_assert(sym_open_paren))
				parameters();
			if(try_consume(sym_two_points))
			{
				if(!assert(sym_identifier))
					mark(error_fatal, "Era esperado um tipo de dados mas foi encontrado \"%s\"", current_token.lexem.id);
				entry_t *entry = find_entry(current_token.lexem.id, symbol_table);
				if(entry != NULL)
				{
					new_entry->type = entry->type;
					scan();
				}
				else
					mark(error_fatal, "\"%s\" não é um tipo válido.", current_token.lexem.id);
			}
			consume(sym_new_line);
			block();
			symbol_table->parent->childrens = symbol_table;
			symbol_table = symbol_table->parent;
		}
		else
			mark(error_fatal, "O identificador \"%s\" já foi declarada anteriormente.", current_token.lexem.id);
	}
	else
		mark(error_fatal, "\"%s\" é inválido como identificador de uma função.", current_token.lexem.id);
	consume(sym_end_function);
}

// programa = “algoritmo” identificador “⏎” { declaração_função } bloco “fim_algoritmo”.
void program()
{
	consume_new_line();
	consume(sym_algorith);
	consume(sym_identifier);
	consume(sym_new_line);
	consume_new_line();
	while(try_assert(sym_identifier) || try_assert(sym_function) || try_assert(sym_const) || try_assert(sym_type))
	{
		if(try_assert(sym_identifier) || try_assert(sym_const) || try_assert(sym_type))
			declarations();
		else if(try_assert(sym_function))
			function_declaration();
		consume(sym_new_line);
		consume_new_line();
	}
	consume(sym_end_algorith);
}

bool initialize_parser(FILE *file)
{
	symbol_table = initialize_table();
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
