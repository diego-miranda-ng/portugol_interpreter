#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"
#include "errors.h"
#include "symbol_table.h"
#include "parser.h"

int main()
{
	initialize_parser(fopen("codigo_portugol.txt", "r"));
	parse();
}
