#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "scanner_declarations.h"

// Esta função verifica se o atual caracter que está sendo lido é uma quebra de linha.
// "char current_c" é o atual caractere que está sendo lido.
// "last_c" é o caractere que foi lido antes do atual.
// A Função retorará "1" se for uma quebra de linha e "0" caso contrário
/*-----------------------------------------------------------------------------------------*/
int is_newline(char current_c, char last_c)
{
	if((current_c == '\n' && last_c != '\r') || current_c == '\r')
		return 1;
	else
		return 0;
}

// Esta função lê um único caractere do arquivo que foi passado como parâmetro na
// função inicializer_scanner().
// Ela retorna "1" se o caractere foi lido e "0" caso contrário.
/*-----------------------------------------------------------------------------------------*/
int read_char()
{
	last_char = current_char;
	if(fread(&current_char, sizeof(char), 1, input_file))
	{
		if(is_newline(current_char, last_char))
		{
			current_position.line++;
			current_position.column = 0;
		}
		else
			current_position.column++;
		return 1;
	}
	else
		return 0;
}

// Esta função vereifica se o "id (identificador)" que foi lido pertence ao conjunto
// de palavras chave do analisador.
// "char id[]" é o identificador que será verificado no conjunto de palavras chaves.
// "symbol_t *symbol" irá receber o endereço de memória de "symbol"
// Esta função irá retornar "1" se for uma palavra chave e "0" caso contrário.
/*-----------------------------------------------------------------------------------------*/
int is_keyword(char id[], symbol_t *symbol)
{
	int i = 0;
	for(i = 0; i < keywords_count; i++)
	{
		if(strcmp(id, keywords[i].id) == 0)
		{
			*symbol = keywords[i].symbol;
			return 1;
		}
	}
	return 0;
}

// Esta função verifica se a sequencia de caracteres que está sendo lida pelo arquivo
// passado como parâmetro na função inicializer_scanner().
/*-----------------------------------------------------------------------------------------*/
int identifier()
{
	int i = 0, count = 0, size;
	while(i < id_max_length && !isspace(current_char) && (isalpha(current_char) || isdigit(current_char) || current_char == '_'))
	{
		current_token.lexem.id[i] = current_char;
		i++;
		if(!read_char())
			break;
	}
	current_token.lexem.id[i] = '\0';
	if(!is_keyword(current_token.lexem.id, &current_token.lexem.symbol))
		current_token.lexem.symbol = sym_identifier;
	return 1;
}

// Esta função irá identificar se a sequencia de simbolos que está sendo lida é um
// número inteiro ou um número decimal.
// Se for um inteiro ou um decimal a função retornará "1" caso contrário ela irá retornar "0".
/*-----------------------------------------------------------------------------------------*/
int number()
{
	int i = 0;
	while(i < id_max_length && (isdigit(current_char)))
	{
		current_token.lexem.id[i] = current_char;
		if(!read_char())
			break;
		i++;
	}
	current_token.lexem.symbol = sym_integer;
	if(current_char == '.')
	{
		current_token.lexem.id[i] = current_char;
		read_char();
		if(!isdigit(current_char))
		{
			if(is_newline(current_char, last_char))
				fseek(input_file, -3, SEEK_CUR);
			else
				fseek(input_file, -2, SEEK_CUR);
			read_char();
			current_token.lexem.id[i] = '\0';
			return 1;
		}
		i++;
		while(i < id_max_length && isdigit(current_char))
		{
			current_token.lexem.id[i] = current_char;
			if(!read_char())
				break;
			i++;
		}
		current_token.lexem.symbol = sym_float;
	}
	short int invalid = 0;
	while(i < id_max_length && (isalpha(current_char) || current_char == '_'))
	{
		current_token.lexem.id[i] = current_char;
		invalid = 1;
		if(!read_char())
			break;
		i++;
	}
	current_token.lexem.id[i] = '\0';
	if(invalid)
	{
		current_token.lexem.symbol = sym_null;
		return 0;
	}
	return 1;
}

// Esta função verifica se a sequencia de caracteres que está sendo lida é uma string
// A função retorna "1" se for uma string, e "0" caso contrário.
/*-----------------------------------------------------------------------------------------*/
int string()
{
	int i = 0;
	while(i < id_max_length &&
		(isalpha(current_char) ||
		isdigit(current_char) ||
		current_char == ' ' ||
		current_char == '_' ||
		current_char == '-' ||
		current_char == '?' ||
		current_char == '!' ||
		current_char == '\"'
		))
	{
		current_token.lexem.id[i] = current_char;
		i++;
		read_char();
		if(current_char == '\"')
		{
			current_token.lexem.id[i] = current_char;
			i++;
			read_char();
			break;
		}
		else if(feof(input_file) || i == id_max_length)
		{
			current_token.lexem.symbol = sym_null;
			strcpy(current_token.lexem.id, "");
			return 0;
		}
	}
	current_token.lexem.id[i] ='\0';
	current_token.lexem.symbol = sym_string;
	return 1;
}

// Esta função irá capturar a próxima sequência de caracteres do arquivo passado como
// parâmetro na função inicializer_scanner().
/*-----------------------------------------------------------------------------------------*/
int read_token()
{
	while(isspace(current_char))
		read_char();

	if(feof(input_file))
	{
		strcpy(current_token.lexem.id, "EOF");
		current_token.lexem.symbol = sym_eof;
		return 1;
	}

	if(isalpha(current_char))
		return identifier();
	else if(isdigit(current_char))
		return number();

	if(current_char == '\"')
		return string();
	current_token.lexem.id[0] = current_char;
	switch(current_token.lexem.id[0])
	{
		case '+':
			current_token.lexem.symbol = sym_plus;
			break;
		case '-':
			current_token.lexem.symbol = sym_minus;
			break;
		case '%':
			current_token.lexem.symbol = sym_mod;
			break;
		case '*':
			current_token.lexem.symbol = sym_times;
			break;
		case '/':
			current_token.lexem.symbol = sym_division;
			break;
		case '=':
			current_token.lexem.symbol = sym_equal;
			break;
		case '<':
			current_token.lexem.symbol = sym_less;
			break;
		case '>':
			current_token.lexem.symbol = sym_greater;
			break;
		case ';':
			current_token.lexem.symbol = sym_semicolon;
			break;
		case ',':
			current_token.lexem.symbol = sym_comma;
			break;
		case ':':
			current_token.lexem.symbol = sym_two_points;
			break;
		case '(':
				current_token.lexem.symbol = sym_open_paren;
			break;
		case ')':
			current_token.lexem.symbol = sym_close_paren;
			break;
		case '[':
				current_token.lexem.symbol = sym_open_bracket;
			break;
		case ']':
			current_token.lexem.symbol = sym_close_bracket;
			break;
		case '{':
				current_token.lexem.symbol = sym_open_key;
			break;
		case '}':
			current_token.lexem.symbol = sym_close_key;
			break;
		case '!':
			current_token.lexem.symbol = sym_exclamation;
			break;
		default:
			current_token.lexem.symbol = sym_null;
			break;
	}
	current_token.lexem.id[1] = '\0';
	read_char();
	if(last_char == '<' && current_char == '-')
	{
		current_token.lexem.id[1] = '-';
		current_token.lexem.symbol = sym_allocation;
		current_token.lexem.id[2] = '\0';
		read_char();
	}
	else if(last_char == '<' && current_char == '=')
	{
		current_token.lexem.id[1] = '=';
		current_token.lexem.symbol = sym_less_equal;
		current_token.lexem.id[2] = '\0';
		read_char();
	}
	else if(last_char == '>' && current_char == '=')
	{
		current_token.lexem.id[1] = '=';
		current_token.lexem.symbol = sym_greater_equal;
		current_token.lexem.id[2] = '\0';
		read_char();
	}
	else if(last_char == '<' && current_char == '>' || last_char == '!' && current_char == '=')
	{
		current_token.lexem.id[1] = current_char;
		current_token.lexem.symbol = sym_not_equal;
		current_token.lexem.id[2] = '\0';
		read_char();
	}
}

// "initialize_scanner" possui a função de iniciar o analisador léxico, definindo os valores
// iniciais das variáveis globais.
/*-----------------------------------------------------------------------------------------*/
int initializer_scanner(FILE *file)
{
	input_file = file;
	strcpy(current_token.lexem.id, "");
	current_token.position.line = 0;
	current_token.position.column = 0;
	current_token.position.index = 0;
	current_position.line = 0;
	current_position.column = 0;
	current_position.index = 0;
	current_char = '\0';
	read_char();
}
