#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string.h>	// memset
#include <stdlib.h>	// malloc, free
#include <unistd.h>	// isatty

#include "status.h"	// STATUS_*
#include "error.h"	// error_str


#define GETOPT_STRING "GR"	// Смотри arguments.c!!!
// -G  Цветная печать
// -R  Рекурсивно обрабатывать подкаталоги

struct _arguments {
	char colors;
	char recursive;
};

typedef struct _arguments arguments;


int arguments_init(arguments *args, int *argc_p, char ***argv_p);

#endif	// ARGUMENTS_H