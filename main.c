#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "scanner.h"
#include "symbol_table.h"

int main()
{
	initializer_scanner(fopen("cod_portugol.txt", "r"));
	entry_t *t;
	t = initialize_table(0);
}
