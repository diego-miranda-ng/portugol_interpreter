#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

symbol_table_t *symbol_table;

bool scan()
{
	if(current_token.lexem.symbol == sym_eof)
		return false;
	read_token();
	return current_token.lexem.symbol == sym_eof;
}

bool verify(symbol_t sym, bool next)
{
	if(current_token.lexem.symbol == sym)
	{
		if(next)
			scan();
		return true;
	}
	else
		return false;
}

bool verify_report(symbol_t sym, bool next, error_t error)
{
	if(verify(sym, next))
		return true;
	if(sym == sym_identifier)
		mark(error, "Era esperado um identificador mas foi encontrado \"%s\".", current_token.lexem.id);
	else if(sym == sym_integer)
		mark(error, "Era esperado um inteiro mas foi encontrado \"%s\".", current_token.lexem.id);
	else if(sym == sym_new_line)
		mark(error, "Era esperado uma quebra de linha mas foi encontrado \"%s\".", current_token.lexem.id);
	else
		mark(error, "Era esperado \"%s\" mas foi encontrado \"%s\".", find_symbol(sym), current_token.lexem.id);
	return false;
}

bool try_consume(symbol_t sym)
{
	return verify(sym, true);
}

bool consume(symbol_t sym, error_t error)
{
	return verify_report(sym, true, error);
}

bool assert(symbol_t sym, error_t error)
{
	return verify_report(sym, false, error);
}

bool try_assert(symbol_t sym)
{
	return verify(sym, false);
}

void consume_new_line()
{
	while(try_consume(sym_new_line));
}

void block();

void expression();

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
			expression();
			consume(sym_close_bracket, error_log);
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
void var_declaration(bool search_parent)
{
	entry_t *table = NULL;
	entry_t *entry = NULL;
	do
	{
		assert(sym_identifier, error_fatal);
		if(find_entry(current_token.lexem.id, symbol_table, search_parent) == NULL)
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
			mark(error_fatal, "O identificador \"%s\" já foi declarado anteriormente.", current_token.lexem.id);
	}
	while(try_consume(sym_comma));
	consume(sym_colon, error_fatal);
	assert(sym_identifier, error_fatal);
	entry = find_entry(current_token.lexem.id, symbol_table, true);
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
			mark(error_fatal,"Este tipo de dados não é válido ou não foi declarado.");
		scan();
		add_entry(table, symbol_table);
	}
	else
		mark(error_fatal, "O tipo de dados \"%s\" não foi declarado anteriormente.", current_token.lexem.id);
}

// parâmetros = “(“ declaração_variável { “;” declaração_variável } “)”.
void parameters()
{
	consume(sym_open_paren, error_log);
	do
	{
		var_declaration(false);
	}
	while(try_consume(sym_semicolon));
	consume(sym_close_paren, error_log);
}

// argumentos = “(“ [ expressão { “,” expressão } ] “)”.
void arguments()
{
	consume(sym_open_paren, error_log);	
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
	if(!consume(sym_close_paren, error_log))
		mark(error_fatal, "O token \"%s\" é inválido.", current_token.lexem.id);
}

// seletor = { “.” identificador | “[“ expressão “]” }.
void selector(entry_t *entry)
{
	while(try_assert(sym_dot) || try_assert(sym_open_bracket))
	{
		if(try_consume(sym_dot))
		{
			if(assert(sym_identifier, error_fatal))
			{
				if(find_field(current_token.lexem.id, entry) != NULL)
					consume(sym_identifier, error_log);
				else
					mark(error_fatal, "O identificador \"%s\" não foi declarado como um campo de \"%s\"",current_token.lexem.id, entry->id);
			}
		}
		else if(try_consume(sym_open_bracket))
		{
			expression();
			consume(sym_close_bracket, error_log);
		}
	}
}

// fator = texto | inteiro | decimal | identificador ( seletor | argumentos ) | “(“ expressão “)” | “nao” fator.
void factor()
{
	if(try_assert(sym_identifier))
	{
		entry_t *entry = find_entry(current_token.lexem.id, symbol_table, true);
		if(entry != NULL)
		{
			consume(sym_identifier, error_log);
			if(try_assert(sym_open_paren))
				arguments();
			else if(try_assert(sym_dot) || try_assert(sym_open_bracket))
				selector(entry);
		}
		else
			mark(error_parser, "O identificador \"%s\" não foi declarado anteriormente.", current_token.lexem.id);
	}
	else if(try_consume(sym_open_paren))
	{
		expression();
		consume(sym_close_paren, error_fatal);
	}
	else if(try_consume(sym_not))
	{
		factor();
		return;
	}
	else if(try_consume(sym_string) || try_consume(sym_integer) || try_consume(sym_float))
		return;
	else
		mark(error_fatal, "O identificador \"%s\" é inválido na posição atual.", current_token.lexem.id);
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
	consume(sym_type, error_log);
	if(assert(sym_identifier, error_log))
	{
		if(find_entry(current_token.lexem.id, symbol_table, true) == NULL)
		{
			entry_t *entry = NULL;
			entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_unknown);
			scan();
			consume(sym_allocation, error_log);
			if(try_assert(sym_identifier))
			{
				entry = find_entry(current_token.lexem.id, symbol_table, true);
				new_entry->class = class_type;
				if(entry != NULL)
					new_entry->type = entry->type;
				else
					mark(error_parser, "O identificador \"%s\" não foi declarado.", current_token.lexem.id);
				scan();
			}
			else if(try_assert(sym_registry))
			{
				scan();
				consume(sym_new_line, error_log);
				unsigned int size = 0;
				new_entry->class = class_var;
				if(try_assert(sym_identifier))
				{
					symbol_table = create_table(symbol_table);
					do
					{
						var_declaration(false);
						consume(sym_new_line, error_log);
					}
					while(try_assert(sym_identifier));
					entry = symbol_table->entries;
					if(entry != NULL)
					{
						entry_t *e = entry;
						while (e != NULL && e->type != NULL)
						{
							size += e->type->size;
							e = e->next;
						}
					}
					new_entry->children = symbol_table;
					symbol_table = symbol_table->parent;
					free(new_entry->children);
					new_entry->children = NULL;
				}
				if(!try_consume(sym_end_registry))
					mark(error_fatal, "Era esperado um identificador ou \"fim_registro\" mas foi encontrado \"%s\".", current_token.lexem.id);
				new_entry->type = create_type(form_record, 0, size, entry, NULL);
			}
			else
				mark(error_fatal, "Era esperado um identificador ou \"registro\" mas foi encontrado \"%s\".", current_token.lexem.id);
			add_entry(new_entry, symbol_table);
			consume(sym_new_line, error_log);
		}
		else
			mark(error_parser, "O identificador \"%s\" não foi declarado.", current_token.lexem.id);
	}
	else
		mark(error_fatal, "O token \"%s\" é inválido como identificador de uma função.", current_token.lexem.id);
}

// declaração_constante = “constante” identificador “=” expressão.
void const_declaration()
{
	consume_new_line();
	consume(sym_const, error_log);
	if(assert(sym_identifier, error_log))
	{
		if(find_entry(current_token.lexem.id, symbol_table, true) == NULL)
		{
			entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_const);
			add_entry(new_entry, symbol_table);
			consume(sym_identifier, error_log);
			consume(sym_allocation, error_log);
			expression();
		}
		else
			mark(error_warning, "O identificador \"%s\" já foi declarado anteriormente.", current_token.lexem.id);
	}
	else
		mark(error_fatal, "O token \"%s\" é inválido. Era esperando um identificador.", current_token.lexem.id);
}

// declarações =  declaração_tipo | declaração_constante | declaração_variável.
void declarations()
{
	consume_new_line();
	if(try_consume(sym_var))
	{
		if(assert(sym_identifier, error_log))
			var_declaration(false);
	}
	else if(try_assert(sym_const))
		const_declaration();
	else if(try_assert(sym_type))
		type_declaration();
}

// se = “se” expressão “entao” “⏎” bloco { “senao_se” expressão “entao” “⏎” bloco } [ “senao” “⏎” bloco ] “fim_se”.
void stmt_if()
{
	consume_new_line();
	consume(sym_if, error_log);
	expression();
	consume(sym_then, error_log);
	consume(sym_new_line, error_log);
	entry_t *new_entry = create_entry("se", current_token.position, class_unknown);
	symbol_table = create_table(symbol_table);
	new_entry->children = symbol_table;
	block();
	symbol_table = symbol_table->parent;
	add_entry(new_entry, symbol_table);
	while(try_consume(sym_else_if))
	{
		expression();
		consume(sym_then, error_log);
		consume(sym_new_line, error_log);
		new_entry = create_entry("se", current_token.position, class_unknown);
		symbol_table = create_table(symbol_table);
		new_entry->children = symbol_table;
		block();
		symbol_table = symbol_table->parent;
		add_entry(new_entry, symbol_table);
	}
	if(try_consume(sym_else))
	{
		consume(sym_new_line, error_log);
		new_entry = create_entry("se", current_token.position, class_unknown);
		symbol_table = create_table(symbol_table);
		new_entry->children = symbol_table;
		block();
		symbol_table = symbol_table->parent;
		add_entry(new_entry, symbol_table);
	}
	consume(sym_end_if, error_log);
}

// caso =  “caso” expressão “⏎” “seja” expressão “:” “⏎” bloco { “seja” expressão “:” “⏎” bloco } [ “senao” “:” “⏎” bloco ] “fim_caso”.
void stmt_case()
{
	consume_new_line();
	entry_t *new_entry;
	consume(sym_case, error_fatal);
	expression();
	consume(sym_new_line, error_log);
	consume(sym_be, error_fatal);
	do
	{
		expression();
		if(!consume(sym_colon, error_log))
			scan();
		consume(sym_new_line, error_log);
		new_entry = create_entry("caso", current_token.position, class_unknown);
		symbol_table = create_table(symbol_table);
		new_entry->children = symbol_table;
		block();
		symbol_table = symbol_table->parent;
		add_entry(new_entry, symbol_table);
	}
	while(try_consume(sym_be));
	if(try_consume(sym_else))
	{
		if(!consume(sym_colon, error_log))
			scan();
		consume(sym_new_line, error_log);
		new_entry = create_entry("caso", current_token.position, class_unknown);
		symbol_table = create_table(symbol_table);
		new_entry->children = symbol_table;
		block();
		symbol_table = symbol_table->parent;
		add_entry(new_entry, symbol_table);
	}
	if(!try_consume(sym_end_case))
		mark(error_fatal, "Era esperado \"seja\", \"senao\" ou \"fim_caso\" mas foi encontrado \"%s\".", current_token.lexem.id);
}

// para = “para” identificador seletor “de” expressão “ate” expressão [ “passo” expressão ] “faca” “⏎” bloco “fim_para”.
void stmt_for()
{
	consume(sym_for, error_fatal);
	assert(sym_identifier, error_fatal);
	entry_t *entry = find_entry(current_token.lexem.id, symbol_table, true);
	if(entry != NULL)
	{
		scan();
		selector(entry);
		consume(sym_of, error_fatal);
		expression();
		consume(sym_until, error_fatal);
		expression();
		bool step = false;
		if(try_consume(sym_step))
		{
			expression();
			step = true;
		}
		if(!try_consume(sym_do))
		{
			if(step)
				mark(error_fatal, "Era esperado \"faca\" mas foi encontrado \"%s\".", current_token.lexem.id);
			else
				mark(error_fatal, "Era esperado \"passo\", \"faca\" mas foi encontrado \"%s\".", current_token.lexem.id);
		}
		consume(sym_new_line, error_log);
		entry_t *new_entry = create_entry("para", current_token.position, class_unknown);
		symbol_table = create_table(symbol_table);
		new_entry->children = symbol_table;
		block();
		symbol_table = symbol_table->parent;
		add_entry(new_entry, symbol_table);
		consume(sym_end_for, error_fatal);
	}
	else
		mark(error_fatal, "O identificador \"%s\" não foi declarado anteriormente.", current_token.lexem.id);
}

// faça = “faca” “⏎” bloco “sempre_que” expressão.
void stmt_do()
{
	consume(sym_do, error_fatal);
	consume(sym_new_line, error_log);
	entry_t *new_entry = create_entry("faca", current_token.position, class_unknown);
	symbol_table = create_table(symbol_table);
	new_entry->children = symbol_table;
	block();
	symbol_table = symbol_table->parent;
	add_entry(new_entry, symbol_table);
	consume(sym_whenever, error_fatal);
	expression();
}

// enquanto = “enquanto” expressão “faca” “⏎” bloco “fim_enquanto”.
void stmt_while()
{
	consume(sym_while, error_fatal);
	expression();
	consume(sym_do, error_fatal);
	consume(sym_new_line, error_log);
	entry_t *new_entry = create_entry("enquanto", current_token.position, class_unknown);
	symbol_table = create_table(symbol_table);
	new_entry->children = symbol_table;
	block();
	symbol_table = symbol_table->parent;
	add_entry(new_entry, symbol_table);
	consume(sym_end_while, error_fatal);
}

// repita = “repita” “⏎” bloco “ate_que” expressão.
void stmt_repeat()
{
	consume(sym_repeat, error_fatal);
	consume(sym_new_line, error_log);
	entry_t *new_entry = create_entry("repita", current_token.position, class_unknown);
	symbol_table = create_table(symbol_table);
	new_entry->children = symbol_table;
	block();
	symbol_table = symbol_table->parent;
	add_entry(new_entry, symbol_table);
	consume(sym_until2, error_fatal);
	expression();
}

// até = “ate” expressão “repita” “⏎” bloco “fim_ate”.
void stmt_until()
{
	consume(sym_until, error_fatal);
	expression();
	consume(sym_repeat, error_fatal);
	consume(sym_new_line, error_log);
	entry_t *new_entry = create_entry("ate", current_token.position, class_unknown);
	symbol_table = create_table(symbol_table);
	new_entry->children = symbol_table;
	block();
	symbol_table = symbol_table->parent;
	add_entry(new_entry, symbol_table);
	consume(sym_end_until, error_fatal);
}

// retorno = “retorne” expressão.
void stmt_return()
{
	consume(sym_return, error_log);
	expression();
}

// decisão = identificador ( argumentos | seletor “=” expressão ).
void decision()
{
	entry_t *entry = find_entry(current_token.lexem.id, symbol_table, true);
	if(entry != NULL)
	{
		consume(sym_identifier, error_log);
		if(try_assert(sym_open_paren))
			arguments();
		else if(try_assert(sym_dot) || try_assert(sym_open_bracket) || try_assert(sym_allocation))
		{
			selector(entry);
			if(!try_consume(sym_allocation))
				mark(error_fatal, "Era esperado \"=\" mas foi encontrado \"%s\".", current_token.lexem.id);
			if(try_assert(sym_string) 
			|| try_assert(sym_integer) 
			|| try_assert(sym_float) 
			|| try_assert(sym_identifier)
			|| try_assert(sym_open_paren)
			|| try_assert(sym_not))
				expression();
			else
				mark(error_fatal, "O token \"%s\" é inválido.", current_token.lexem.id);
		}
		else
			mark(error_fatal, "O token \"%s\" é inválido. Era esperado \".\", \"[\" ou \"=\".", current_token.lexem.id);
	}
	else
		mark(error_fatal, "O identificador \"%s\" não foi declarado anteriormente.", current_token.lexem.id);
}

// bloco =  { ( chamada_função | atribuição | declarações | retorno | se | enquanto | para | faça | caso | repita | até ) “⏎” }.
void block()
{
	consume_new_line();
	while(try_assert(sym_identifier)
		|| try_assert(sym_var)
		|| try_assert(sym_type)
		|| try_assert(sym_const)
		|| try_assert(sym_if)
		|| try_assert(sym_case)
		|| try_assert(sym_for)
		|| try_assert(sym_do)
		|| try_assert(sym_while)
		|| try_assert(sym_repeat)
		|| try_assert(sym_until)
		|| try_assert(sym_return))
	{
		if(try_assert(sym_var) || try_assert(sym_type) || try_assert(sym_const))
			declarations();
		else if(try_assert(sym_if))
			stmt_if();
		else if(try_assert(sym_case))
			stmt_case();
		else if(try_assert(sym_for))
			stmt_for();
		else if(try_assert(sym_do))
			stmt_do();
		else if(try_assert(sym_while))
			stmt_while();
		else if(try_assert(sym_repeat))
			stmt_repeat();
		else if(try_assert(sym_until))
			stmt_until();
		else if(try_assert(sym_identifier))
			decision();
		else if(try_assert(sym_return))
			stmt_return();
		else
			mark(error_fatal, "O token \"%s\" é desconhecido.", current_token.lexem.id);
		consume(sym_new_line, error_log);
		consume_new_line();
	}
}

// declaração_função = “funcao” identificador [ parâmetros ] [ “:” identificador ] “⏎” bloco “fim_funcao” “⏎”.
void function_declaration()
{
	consume_new_line();
	consume(sym_function, error_log);
	if(assert(sym_identifier, error_log))
	{
		if(find_entry(current_token.lexem.id, symbol_table, true) == NULL)
		{
			entry_t *new_entry = create_entry(current_token.lexem.id, current_token.position, class_function);
			add_entry(new_entry, symbol_table);
			scan();
			symbol_table = create_table(symbol_table);
			new_entry->children = symbol_table;
			if(try_assert(sym_open_paren))
				parameters();
			if(try_consume(sym_colon))
			{
				if(!assert(sym_identifier, error_log))
					mark(error_fatal, "Era esperado um tipo de dados mas foi encontrado \"%s\"", current_token.lexem.id);
				entry_t *entry = find_entry(current_token.lexem.id, symbol_table, true);
				type_t *type = entry->type;
				if(entry != NULL && (type->form == form_atomic || type->form == form_record))
				{
					new_entry->type = type;
					scan();
				}
				else
					mark(error_fatal, "\"%s\" não é válido ou não foi declarado.", current_token.lexem.id);
			}
			consume(sym_new_line, error_log);
			block();
			symbol_table = symbol_table->parent;
		}
		else
			mark(error_fatal, "O identificador \"%s\" já foi declarada anteriormente.", current_token.lexem.id);
	}
	else
		mark(error_fatal, "\"%s\" é inválido como identificador de uma função.", current_token.lexem.id);
	consume(sym_end_function, error_log);
}

// programa = “algoritmo” identificador “⏎” { declaração_função } bloco “fim_algoritmo”.
void program()
{
	consume_new_line();
	consume(sym_algorith, error_log);
	consume(sym_identifier, error_log);
	consume(sym_new_line, error_log);
	consume_new_line();
	while(try_assert(sym_var) || try_assert(sym_function) || try_assert(sym_const) || try_assert(sym_type))
	{
		if(try_assert(sym_var) || try_assert(sym_const) || try_assert(sym_type))
			declarations();
		else if(try_assert(sym_function))
			function_declaration();
		consume(sym_new_line, error_log);
		consume_new_line();
	}
	block();
	consume_new_line();
	consume(sym_end_algorith, error_log);
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
