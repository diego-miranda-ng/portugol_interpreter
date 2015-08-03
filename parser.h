#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
		// printf("\n\nERRO!!\n\n"); // O simbolo atual não é igual e o simbolo passado por parâmetro não é null.
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

void type()
{}

void var()
{
	consume(sym_identifier);
	while(try_consume(sym_open_bracket))
	{
		consume(sym_integer);
		consume(sym_close_bracket);
	}
}

void var_declaration()
{
	do
	{
		var();
	}
	while(try_consume(sym_comma));
	consume(sym_two_points);
	consume(sym_identifier);
}

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

void function_declaration()
{
	consume_new_line();
	while(try_consume(sym_function))
	{
		consume(sym_identifier);
		if(try_assert(sym_open_paren))
			parameters();
		if(try_assert(sym_two_points))
		{
			scan();
			// assert(sym_identifier);
			// find_entry(current_token.lexem.id);
		}
		consume(sym_new_line);
	}
}

void program()
{
	consume_new_line();
	consume(sym_algorith);
	consume(sym_identifier);
	consume(sym_new_line);
	function_declaration();
}

bool initialize_parser(FILE *file, entry_t *symbol_table)
{
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
