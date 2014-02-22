#include "arguments.h"

int arguments_init(arguments *args, int *argc_p, char ***argv_p)
{
	// Обнуляем все поля *args
	memset(args, 0, sizeof(arguments));
	
	// Определяем, связан ли stdout с терминалом
	if (isatty(STDOUT_FILENO)) args->colors = 1;
	
	// Обрабатываем аргументы командной строки
	int ch;
	while ((ch = getopt(*argc_p, *argv_p, GETOPT_STRING)) != -1) {
		switch (ch) {
		case 'G':
			args->colors = 1;
			break;
		case 'R':
			args->recursive = 1;
			break;
		case 'l':
			args->long_format = 1;
			break;
		case 'a':
			args->all = 1;
			break;
		default:
			error_str("Incorrect argument!");
			return STATUS_INCORRECT_ARGUMENT;
		}
	}
	*argc_p -= optind;  // Количество обработанных данных
	*argv_p += optind;
	
	return STATUS_OK;
}