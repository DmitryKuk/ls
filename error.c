#include "error.h"

void error_errno(const char *err_str)
{
	int err = errno;	// Защита от изменения errno ф-ей fprintf
	fprintf(stderr, "ERROR: ");	// Ошибки fprintf игнорируются
	errno = err;
	perror(err_str);
}

void error_str(const char *err_str)
{
	fprintf(stderr, "ERROR: %s\n", err_str);
}