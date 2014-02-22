#ifndef LS_H
#define LS_H

#include <stdio.h>			// printf
#include <sys/stat.h>		// stat
#include <dirent.h>			// dirent, DIR, opendir, closedir, readdir
#include <pwd.h>			// getpwuid
#include <grp.h>			// getgrgid
#include <time.h>			// localtime, strftime
#include <locale.h>			// Need for time localization
#include <langinfo.h>		// Need for time localization
#include <string.h>			// strlen, strcat
#include <stdlib.h>			// system, malloc, free

#include "status.h"			// STATUS_*
#include "arguments.h"		// arguments
#include "color.h"			// print_color, COLOR_*

//#define SHORT_FILE_NAMES 0

int ls(const arguments *args, const char *path);

#endif	// LS_H