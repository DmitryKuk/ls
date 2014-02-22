#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>	// perror, fprintf
#include <errno.h>	// errno

void error_errno(const char *err_str);	// err_str is prefix
void error_str(const char *err_str);

#endif	// ERROR_H