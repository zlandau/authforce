/* $Id: misc.c,v 1.3 2001/02/13 22:54:53 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

/* malloc wrapper with error checking */
void *malloc_wrapper(size_t size) {
	void *ptr;

	ptr = malloc(size);

	if (!ptr) {
		fprintf(stderr, "malloc_wrapper: couldn't allocate memory\n");
		exit(EXIT_FAILURE);
	}

	return (ptr);
}

/* strdup wrapper with error checking */
char *strdup_wrapper(const char *s) {
	char *ptr;

	ptr = strdup(s);

	if (!ptr) {
		fprintf(stderr, "strdup_wrapper: couldn't allocate memory\n");
		exit(EXIT_FAILURE);
	}

	return (ptr);
}

/*
 * seperates str using token, putting the first part in str and returning
 * the second part. (does a function like this already exist?)
 * why the hell wont it ever return null blah!
 */
char *extract(char *str, char token) {

	while (*str != token) {
		if (*str == '\0')
			return(NULL);
		++str;
	}
	*str = '\0';
	str++;
	return(str);
}

/* calculate new average, average being new entry, element being the new
 * number and num being the total elements in average
 */

float stats(float average, float element, int num) {
	printf("avg: %f, el: %f, num: %i\n", average, element, num);
	return (((average*num)+element)/num+1);
}

/* code released as public domain by Bob Stout */
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

/* +++Date last modified: 05-Jul-1997 */
/*
**  Originally published as part of the MicroFirm Function Library
**
**  Copyright 1986, S.E. Margison
**  Copyright 1989, Robert B.Stout
**
**  The user is granted a free limited license to use this source file
**  to create royalty-free programs, subject to the terms of the
**  license restrictions specified in the LICENSE.MFL file.
**
**  remove all whitespace from a string
*/
/* slightly modified */

void remove_crud(char *str)
{
      char *obuf, *nbuf;

      if (str)
      {
            for (obuf = str, nbuf = str; *obuf; ++obuf)
            {
                  if (*obuf != ' ' && *obuf != '\t')
                  	    *nbuf++ = *obuf;

            }
            *nbuf = 0;
      }
}
