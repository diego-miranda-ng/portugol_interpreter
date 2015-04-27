// "id_max_length" defini o limite de caracteres que poderão existir em um id.
/*-----------------------------------------------------------------------------------------*/
#define id_max_length 20

// Em "symbol_t" é declarada todas as constantes que serão utilizadas para definir um
// tipo de token.
/*-----------------------------------------------------------------------------------------*/
typedef enum _symbol
{
	sym_identifier,
	sym_integer, // 10
	sym_float, // 10.20
	sym_string, // "caractere"
	sym_type_integer, // inteiro
	sym_type_float, // real
	sym_type_string, // caractere
	sym_type_boolean, // logico
	sym_if,	// se
	sym_then, // entao
	sym_else, // senao
	sym_end_if, // fimse
	sym_case, // caso
	sym_end_case, // fimcase
	sym_equal_to, // igual
	sym_to,	// a
	sym_read, // leia
	sym_write, // escreva
	sym_clear, // limpatela
	sym_while, // enquanto
	sym_do,	// faça
	sym_end_while, // fimenquanto
	sym_for, // para
	sym_of,	// de
	sym_repeat,	// repita
	sym_until, // ate
	sym_end_for, // fimpara
	sym_exponentiation, // ^ ou exp
	sym_times, // *
	sym_division, // /
	sym_mod, // % ou mod
	sym_plus, // +
	sym_minus, // -
	sym_or, // ou
	sym_and, // e
	sym_not, // nao
	sym_or_exclusive, // xou
	sym_equal, // =
	sym_less, // <
	sym_less_equal, // <=
	sym_greater, // >
	sym_greater_equal, // >=
	sym_allocation, // <-
	sym_exclamation, // !
	sym_not_equal, // != ou <>
	sym_comma, // ,
	sym_semicolon, // ;
	sym_open_paren, // (
	sym_close_paren, // )
	sym_open_bracket, // [
	sym_close_bracket, // ]
	sym_open_key, // {
	sym_close_key, // }
	sym_two_points, // :
	sym_null, // null
	sym_eof // eof
} symbol_t;

// O struct "position_t" é uma estrutura em que se armazena a posição de um token.
/*-----------------------------------------------------------------------------------------*/
typedef struct _position
{
	unsigned int line;
	unsigned int column;
	unsigned int index;
} position_t;

// O struct "lexem_t" é uma estrutura em que se armazena o "id" (o token em si) e o tipo
// de token que ele está armazenando.
/*-----------------------------------------------------------------------------------------*/
typedef struct _lexem
{
	char id[id_max_length];
	symbol_t symbol;
} lexem_t;

// O struct "token_t" é uma estrutura em que se armazena um "lexem_t" (que contém as
// informações citaras acima) e a posição do token no arquivo que está sendo lido atravé
// do struct "position_t".
/*-----------------------------------------------------------------------------------------*/
typedef struct _token
{
	lexem_t lexem;
	position_t position;
} token_t;

// Este struct está armazenando o cunjunto de palavras chaves do compilador.
/*-----------------------------------------------------------------------------------------*/
lexem_t keywords[] = {
	{"inteiro", sym_type_integer},
	{"real", sym_type_float},
	{"caractere", sym_type_string},
	{"logico", sym_type_boolean},
	{"leia", sym_read},
	{"escreva", sym_write},
	{"limpatela", sym_clear},
	{"ou", sym_or},
	{"nao", sym_not},
	{"e", sym_and},
	{"xou", sym_or_exclusive},
	{"se", sym_if},
	{"entao", sym_then},
	{"senao", sym_else},
	{"fimse", sym_end_if},
	{"caso", sym_case},
	{"igual", sym_equal_to},
	{"a", sym_to},
	{"fimcaso", sym_end_case},
	{"enquanto", sym_while},
	{"faca", sym_do},
	{"fimenquanto", sym_end_while},
	{"para", sym_for},
	{"de", sym_of},
	{"repita", sym_repeat},
	{"ate", sym_until},
	{"fimpara", sym_end_for},
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
	{"<-", sym_allocation},
	{"=", sym_equal},
	{"<", sym_less},
	{">", sym_greater},
	{"<=", sym_less_equal},
	{">=", sym_greater_equal},
	{"!=", sym_not_equal},
	{"<>", sym_not_equal},
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
	{";", sym_semicolon},
	{":", sym_two_points},
	{"!", sym_exclamation}
};
const unsigned int pontuation_count = sizeof(pontuation) / sizeof(lexem_t);

// "last_token" é um struct que armazena o penúltimo token lido.
// "current_token" é um struct que armazena o último token lido.
// "last_char" armazena o penúltimo simbolo lido.
// "current_char" armazena o último simbolo lido.
// "current_position" é um struc que armazena a posição atual em que o
// arquivo está sendo lido.
/*-----------------------------------------------------------------------------------------*/
token_t last_token, current_token;
char last_char, current_char;
position_t current_position;

FILE *input_file;
