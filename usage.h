#ifndef USAGE_H
#define USAGE_H

#include <stdio.h>	// puts

#include "arguments.h"	// GETOPT_STRING

#define USAGE "Usage: ls -"GETOPT_STRING" [PATH1] [PATH2] [...]"

void usage(void);

#endif	// USAGE_H