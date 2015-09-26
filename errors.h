#include "scanner.h"

// Aqui são declaradas as constantes que representam cada tipo que pode ser encontrado
// durante a análise de um código-fonte.
/*-----------------------------------------------------------------------------------------*/
typedef enum _error {
	error_log,
	error_info,
	error_tip,
	error_warning,
	error_parser,
	error_fatal,
	error_unknown
} error_t;

// Esta função é usada para exibir mensagens de erro durante a análise de um código-fonte.
// error_t error recebe quall tipo de erro que foi econtrado.
// char message recebe a mensagem que será exiida.
/*-----------------------------------------------------------------------------------------*/
void mark(error_t error, char message[],...);

// Funciona igual a função mark, com exeção que é passada a posição do erro em:
// position_t positon.
/*-----------------------------------------------------------------------------------------*/
void mark_at(error_t error, position_t position, char message[],...);
