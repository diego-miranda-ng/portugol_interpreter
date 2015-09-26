#ifndef portugol
#define portugol

#include <stdio.h>
#include <stdbool.h>

// id_max_length defini o limite de caracteres que poderão existir em um id.
/*-----------------------------------------------------------------------------------------*/
#define id_max_length 200

// tipo de dados para um token.
/*-----------------------------------------------------------------------------------------*/
typedef char identifier_t[id_max_length + 1];

// Em symbol_t são declaradas todas as constantes que serão utilizadas para definir um
// tipo de token.
/*-----------------------------------------------------------------------------------------*/
typedef enum _symbol
{
	sym_identifier, // identificador_1 _identificador1
	sym_integer, // 1 10 20 55 1000
	sym_float, // 10.10 11.11
	sym_string, // "texto" "texto texto"
	sym_true, // verdadeiro
	sym_false, // falso
	sym_algorith, // algoritmo
	sym_end_algorith, // fim_algoritmo
	sym_return, // retorne
	sym_function, // funcao
	sym_end_function, // fim_funcao
	sym_var, // variavel
	sym_type, // tipo
	sym_registry, // registro
	sym_end_registry, // fim_registro
	sym_const, // constante
	sym_or, // ou
	sym_or_exclusive, // xou
	sym_and, // e
	sym_not, // nao
	sym_if, // se
	sym_then, // entao
	sym_else_if, // senao_se
	sym_else, // senao
	sym_end_if, //fim_se
	sym_case, // caso
	sym_be, // seja
	sym_end_case, // fim_caso
	sym_do, // faca
	sym_while, // enquanto
	sym_whenever, // enquanto_que
	sym_end_while, // fim_enquanto
	sym_repeat, // repita
	sym_until, // ate
	sym_until2, // ate_que
	sym_end_until, // fim_enquanto
	sym_for, // para
	sym_of, // de
	sym_step, // passo
	sym_end_for, // fim_para
	sym_read, // leia
	sym_write, // escreva
	sym_clear, // escreva
	sym_exponentiation, // ^ ou exp
	sym_times, // *
	sym_division, // /
	sym_mod, // % ou mod
	sym_plus, // +
	sym_minus, // -
	sym_equal, // ==
	sym_less, // <
	sym_less_equal, // <=
	sym_greater, // >
	sym_greater_equal, // >=
	sym_allocation, // =
	sym_not_equal, // != ou <>
	sym_comma, // ,
	sym_dot, // .
	sym_semicolon, // ;
	sym_open_paren, // (
	sym_close_paren, // )
	sym_open_bracket, // [
	sym_close_bracket, // ]
	sym_open_key, // {
	sym_close_key, // }
	sym_colon, // :
	sym_new_line, // \n \n\r
	sym_null, // null
	sym_eof // eof
} symbol_t;

// O struct position_t é uma estrutura em que se armazena a posição de um token.
/*-----------------------------------------------------------------------------------------*/
typedef struct _position
{
	unsigned int line;
	unsigned int column;
	unsigned int index;
} position_t;

// O struct lexem_t é uma estrutura em que se armazena o id (o token em si) e o tipo
// de token que ele está armazenando.
/*-----------------------------------------------------------------------------------------*/
typedef struct _lexem
{
	identifier_t id;
	symbol_t symbol;
} lexem_t;

// O struct token_t é uma estrutura em que se armazena um lexem_t  e a posição do token
// no arquivo que está sendo lido através do struct position_t.
/*-----------------------------------------------------------------------------------------*/
typedef struct _token
{
	lexem_t lexem;
	position_t position;
} token_t;

// last_token é um struct que armazena o penúltimo token lido.
// current_token é um struct que armazena o último token lido.
// position_zero é uma constante que determina uma posição zero;
/*-----------------------------------------------------------------------------------------*/
extern token_t last_token, current_token;
extern const position_t position_zero;

void initialize_scanner(FILE *file);
bool read_token();

#endif
