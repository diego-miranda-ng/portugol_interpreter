#ifndef portugol
#define portugol

#include <stdio.h>
#include <stdbool.h>

extern symbol_table_t *symbol_table;
// Esta função inicia o parser recebendo como parâmetro o arquivo que será analisado.
// Retorna true se a análise foi iniciada e false se o arquivo estiver em branco ou se a 
// análise nõ foi iniciada.
/*-----------------------------------------------------------------------------------------*/
bool initialize_parser(FILE *file);

// Esta função começa a executar a análise à partir da primeira regra sintática.
/*-----------------------------------------------------------------------------------------*/
void parse();

#endif
