/* $Id: debug.c,v 1.1 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH*/
#include "extern.h"
#include <libintl.h>


/* debug_level: print debugging messages of this level and lower, based on
 * the following table
 *
 * 0 - no debugging
 * 1 - most important messages
 * 2 - important but ignorable in certain situations
 * 3 - usually for functions reporting what they are doing
 * 4 - functions reporting what they are doing, with greater detail
 * 5 - more than you usually want to know, can site specific values of variables
 */

/*
 * debug: called to print debug messages to stderr
 * parameters: debug level, printf-style arguments
 * returns: n/a
 */

void debug(int level, char *arguments, ...)
{
	char *str;
	va_list ap;

	str = (char*)malloc_w(384);
	
	if (level < 0)
		warning("debug level less than 0\n");

	if (level > 5)
		warning("debug level greater than 5\n");

	if (level <= debug_level) {
		va_start(ap, *arguments);
		vsnprintf(str, 384, arguments, ap);
		va_end(ap);
		printf("debug[%i]: %s", level, str);
	}

	free(str);
}

/*
 * warning: called to print warning messages to stdout
 * parameters: printf-style arguments
 * returns: n/a
 */

void warning(char *arguments, ...)
{
	char *str;
	va_list ap;

	str = (char*)malloc_w(384);

	va_start(ap, *arguments);
	vsnprintf(str, 384, arguments, ap);
	va_end(ap);

	printf("warning: %s", str);

	free(str);
}

