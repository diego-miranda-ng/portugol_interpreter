#include <stdlib.h>
#include <stdio.h>

#include "scanner_declarations.h"
#include "scanner.h"

int main()
{
	initializer_scanner(fopen("arquivo_cod_portugol.txt", "r"));
	return 0;
}
