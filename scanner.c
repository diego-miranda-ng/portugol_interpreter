#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"
#include "parser.h"
#include "errors.h"

// Este struct está armazenando o cunjunto de palavras-chave do compilador.
/*-----------------------------------------------------------------------------------------*/
lexem_t keywords[] = {
	{"verdadeiro", sym_true},
	{"falso", sym_false},
	{"algoritmo", sym_algorith},
	{"fim_algoritmo", sym_end_algorith},
	{"retorne", sym_return},
	{"funcao", sym_function},
	{"fim_funcao", sym_end_function},
	{"var", sym_var},
	{"tipo", sym_type},
	{"registro", sym_registry},
	{"fim_registro", sym_end_registry},
	{"const", sym_const},
	{"ou", sym_or},
	{"xou", sym_or_exclusive},
	{"e", sym_and},
	{"nao", sym_not},
	{"se", sym_if},
	{"entao", sym_then},
	{"senao_se", sym_else_if},
	{"senao", sym_else},
	{"fim_se", sym_end_if},
	{"caso", sym_case},
	{"seja", sym_be},
	{"faca", sym_do},
	{"fim_caso", sym_end_case},
	{"enquanto", sym_while},
	{"sempre_que", sym_whenever},
	{"fim_enquanto", sym_end_while},
	{"repita", sym_repeat},
	{"ate", sym_until},
	{"ate_que", sym_until2},
	{"fim_ate", sym_end_until},
	{"para", sym_for},
	{"de", sym_of},
	{"passo", sym_step},
	{"fim_para", sym_end_for},
	{"leia ", sym_read},
	{"escreva", sym_write},
	{"limpa_tela", sym_clear},
	{"mod", sym_mod},
	{"exp", sym_exponentiation}
};
const unsigned int keywords_count = sizeof(keywords) / sizeof(lexem_t);

// Este struct está armazenando o conjunto de operadores do compilador.
/*-----------------------------------------------------------------------------------------*/
lexem_t operators[] = {
	{"^", sym_exponentiation},
	{"*", sym_times},
	{"/", sym_division},
	{"%", sym_mod},
	{"+", sym_plus},
	{"-", sym_minus},
	{"=", sym_allocation},
	{"==", sym_equal},
	{"<", sym_less},
	{">", sym_greater},
	{"<=", sym_less_equal},
	{">=", sym_greater_equal},
	{"!=", sym_not_equal},
	{"<>", sym_not_equal}
};
const unsigned int operators_count = sizeof(operators) / sizeof(lexem_t);

// Este struct está armazenando o conjunto de pontuação do compilador.
/*-----------------------------------------------------------------------------------------*/
lexem_t pontuation[] = {
	{"(", sym_open_paren},
	{")", sym_close_paren},
	{"[", sym_open_bracket},
	{"]", sym_close_bracket},
	{"{", sym_open_key},
	{"}", sym_close_key},
	{",", sym_comma},
	{".", sym_dot},
	{";", sym_semicolon},
	{":", sym_colon}
};
const unsigned int pontuation_count = sizeof(pontuation) / sizeof(lexem_t);

// last_token é um struct que armazena o núltimo token lido.
// current_token é um struct que armazena o token que está sendo analisado no momento.
// last_char armazena o último caractere lido.
// current_char armazena o caractere que está sendo analisado no momento.
/*-----------------------------------------------------------------------------------------*/
token_t last_token, current_token;
const position_t position_zero = {0,0,0};
char last_char, current_char;
position_t current_position;

FILE *input_file;

// Esta função verifica se o atual caracter que está sendo lido é uma quebra de linha.
// "char current_c" é o atual caractere que está sendo analisado.
// "last_c" é o último caractere que foi analisado.
// A Função retornará true se for uma quebra de linha e false caso contrário
/*-----------------------------------------------------------------------------------------*/
bool is_newline(char current_c, char last_c)
{
	if((current_c == '\n' && last_c != '\r') || current_c == '\r')
		return true;
	else
		return false;
}

// Esta função lê um único caractere do arquivo que foi passado como parâmetro na
// função inicializer_scanner().
// Ela retorna true se o caractere foi lido e false caso contrário.
/*-----------------------------------------------------------------------------------------*/
bool read_char()
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
		return true;
	}
	else
		return false;
}

// Esta função vereifica se o id(identificador) que foi lido pertence ao conjunto
// de palavras-chave do analisador.
// char id[] é o identificador que será comparado com as palavras chaves.
// symbol_t *symbol irá recebero endereço de memóoria da identificação do símbolo.
// Esta função irá retornar true se for uma palavra chave e false caso contrário.
/*-----------------------------------------------------------------------------------------*/
bool is_keyword(char id[], symbol_t *symbol)
{
	int i = 0;
	for(i = 0; i < keywords_count; i++)
	{
		if(strcmp(id, keywords[i].id) == 0)
		{
			*symbol = keywords[i].symbol;
			return true;
		}
	}
	return false;
}

// Esta função procura pelos conjuntos de palavras-chave, operadores e pontuações um 
// determinado símbolo.
// symbol_t symbol recebe a identificação do simbolo.
// Esta função retorna NULL caso não encontre o símbolo o id do símbolo.
/*-----------------------------------------------------------------------------------------*/
char *find_symbol(symbol_t symbol)
{
	int i = 0;
	for(i = 0; i < keywords_count; i++)
	{
		if(symbol == keywords[i].symbol)
			return keywords[i].id;
	}
	for(i = 0; i < operators_count; i++)
	{
		if(symbol == operators[i].symbol)
			return operators[i].id;
	}
	for(i = 0; i < pontuation_count; i++)
	{
		if(symbol == pontuation[i].symbol)
			return pontuation[i].id;
	}
	return NULL;
}

// Esta função verifica se a sequencia de caracteres que está sendo lida pelo arquivo
// é um identificador retornando true se for e false caso contrário.
/*-----------------------------------------------------------------------------------------*/
bool identifier()
{
	int i = 0, count = 0, size;
	current_token.position = current_position;
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
	return true;
}

// Esta função irá identificar se a sequencia de simbolos que está sendo lida é um
// número inteiro ou um número decimal.
// Se for um inteiro ou um decimal a função retornará true caso contrário ela irá retornar false.
/*-----------------------------------------------------------------------------------------*/
bool number()
{
	int i = 0;
	current_token.position = current_position;
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
			return true;
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
		return false;
	}
	return true;
}

// Esta função verifica se a sequencia de caracteres que está sendo lida é um texto
// A função retorna true se for um texto, e false caso contrário.
/*-----------------------------------------------------------------------------------------*/
bool string()
{
	int i = 0;
	current_token.position = current_position;
	while(i < id_max_length)
	{
		current_token.lexem.id[i] = current_char;
		i++;
		read_char();
		if(current_char == '\"' || (i + 2) == id_max_length)
		{
			while(current_char != '\"' && !feof(input_file))
				read_char();
			
			current_token.lexem.id[i] = '\"';

			if(feof(input_file))
			{
				current_token.lexem.symbol = sym_eof;
				strcpy(current_token.lexem.id, "EOF");
				return false;
			}

			i++;
			read_char();
			break;
		}
	}
	current_token.lexem.id[i] ='\0';
	current_token.lexem.symbol = sym_string;
	return true;
}

// Esta função ignora um comentário
/*-----------------------------------------------------------------------------------------*/
bool comment()
{
	if(last_char == '/' && current_char == '*')
	{
		while(!feof(input_file))
		{
			read_char();
			if(last_char == '*' && current_char == '/')
				break;
		}

		read_char();
		return true;
	}
	else if(last_char == '/' && current_char == '/')
	{
		while(!is_newline(current_char, last_char) && !feof(input_file))
			read_char();
		read_char();
		return true;
	}
	return false;
}

// Esta função irá capturar a próxima sequência de caracteres do arquivo passado como
// parâmetro na função inicializer_scanner().
/*-----------------------------------------------------------------------------------------*/
bool read_token()
{
	while(isspace(current_char) && !is_newline(current_char, last_char))
		read_char();

	if(feof(input_file))
	{
		strcpy(current_token.lexem.id, "EOF");
		current_token.lexem.symbol = sym_eof;
		return true;
	}

	if(is_newline(current_char, last_char))
	{
		strcpy(current_token.lexem.id, "\\n");
		current_token.lexem.symbol = sym_new_line;
		read_char();
		return true;
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
			current_token.lexem.symbol = sym_allocation;
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
			current_token.lexem.symbol = sym_colon;
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
		case '.':
			current_token.lexem.symbol = sym_dot;
			break;
		default:
			current_token.lexem.symbol = sym_null;
			break;
	}
	current_token.lexem.id[1] = '\0';
	read_char();
	if(comment())
	{
		current_token.lexem.symbol = sym_null;
		current_token.lexem.id[0] = '\0';
		read_token();
		return true;
	}
	else if(last_char == '<' && current_char == '=')
	{
		current_token.lexem.id[1] = '=';
		current_token.lexem.symbol = sym_equal;
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
void initialize_scanner(FILE *file)
{
	input_file = file;
	strcpy(current_token.lexem.id, "");
	current_token.position.line = 0;
	current_token.position.column = 0;
	current_token.position.index = 0;
	current_position.line = 1;
	current_position.column = 0;
	current_position.index = 0;
	current_char = '\0';
	read_char();
}
