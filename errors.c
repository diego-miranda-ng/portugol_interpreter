#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "scanner.h"
#include "errors.h"

#define errors_tolerance 50

unsigned int errors_count = 0;

void mark_at(error_t error, position_t position, char message[],...)
{
	if(error > error_warning)
		errors_count++;
	if(errors_count > errors_tolerance)
	{
		printf("%d erros foram encontrados. O processo de análise foi cancelado.", errors_tolerance);
		exit(EXIT_FAILURE);
	}
	switch(error)
	{
		case error_log:
			printf("Log");
			break;
		case error_info:
			printf("Info");
			break;
		case error_tip:
			printf("Tip");
			break;
		case error_warning:
			printf("Atenção em ");
			break;
		case error_parser:
			printf("Erro em ");
			break;
		case error_fatal:
			printf("Erro fatal em ");
			break;
		default:
			printf("Erro desconhecido em ");
			break;
	}
	printf("(%d,%d): ", position.line, position.column);
	va_list args;
	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	printf("\n");
	if(error == error_fatal)
		exit(EXIT_FAILURE);
}

void mark(error_t error, char message[],...)
{
	if(error > error_warning)
		errors_count++;
	if(errors_count > errors_tolerance)
	{
		printf("%d erros foram encontrados. O processo de análise foi cancelado.", errors_tolerance);
		exit(EXIT_FAILURE);
	}
	switch(error)
	{
		case error_log:
			printf("Log");
			break;
		case error_info:
			printf("Info");
			break;
		case error_tip:
			printf("Tip");
			break;
		case error_warning:
			printf("Atenção em ");
			break;
		case error_parser:
			printf("Erro em ");
			break;
		case error_fatal:
			printf("Erro fatal em ");
			break;
		default:
			printf("Erro desconhecido em ");
			break;
	}
	printf("(%d,%d): ", current_token.position.line, current_token.position.column);
	va_list args;
	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	printf("\n");
	if(error == error_fatal)
		exit(EXIT_FAILURE);
}
