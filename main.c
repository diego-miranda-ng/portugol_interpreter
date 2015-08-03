#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"
#include "errors.h"
#include "symbol_table.h"
#include "parser.h"

int main()
{
	entry_t *t;
	t = initialize_table(0);
	initialize_parser(fopen("cod_portugol.txt", "r"), t);
	parse();
}
