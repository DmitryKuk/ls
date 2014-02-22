#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#include "arguments.h"
#include "error.h"
#include "usage.h"
#include "ls.h"
#include "color.h"

int main(int argc, char **argv)
{
	arguments args;
	int status = arguments_init(&args, &argc, &argv);
	switch (status) {
	case STATUS_OK:	// Всё задано корректно
		break;
	case STATUS_INCORRECT_ARGUMENT:
		usage();
		exit(STATUS_INCORRECT_ARGUMENT);	// Неизвестный аргумент
	default:	// Этого не случится
		error_str("Unknown error!");
		exit(STATUS_ERROR);
	}
	
	status = 0;
	if (argc == 0) {
		status = ls(&args, "./");
	} else {
		for ( ; argc > 0; --argc, ++argv) {
			int tmp = ls(&args, *argv);
			if (status == 0) status = tmp;
		}
	}
	
	exit(status);
}