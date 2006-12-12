/* $Id: extract.c,v 1.2 2001/02/13 22:54:53 kapheine Exp $ */

/* Copyright (C) 1987-1999 Free Software Foundation, Inc.

   This file is part of GNU Bash, the Bourne Again SHell.

   Bash is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Bash is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License along
   with Bash; see the file COPYING.  If not, write to the Free Software
   Foundation, 59 Temple Place, Suite 330, Boston, MA 02111 USA. */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

static char *sub_string(char *string, int start, int end);
/* note: this function was extracted, with the following changes, from 
 * general.c, a part of bash-2.04. the above license is the original license
 * of general.c and all of bash-2.04
 *
 * changes: xmalloc changed to malloc, changed function declaration
 */

/* Given a string containing units of information separated by colons,
   return the next one pointed to by (P_INDEX), or NULL if there are no more.
   Advance (P_INDEX) to the character after the colon. */
char *extract_colon_unit(char *string, int *p_index)
{
  int i, start, len;
  char *value;

  if (string == 0)
    return (string);

  len = strlen (string);
  if (*p_index >= len)
    return ((char *)NULL);

  i = *p_index;

  /* Each call to this routine leaves the index pointing at a colon if
     there is more to the path.  If I is > 0, then increment past the
     `:'.  If I is 0, then the path has a leading colon.  Trailing colons
     are handled OK by the `else' part of the if statement; an empty
     string is returned in that case. */
  if (i && string[i] == ':')
    i++;

  for (start = i; string[i] && string[i] != ':'; i++)
    ;

  *p_index = i;

  if (i == start)
    {
      if (string[i])
	(*p_index)++;
      /* Return "" in the case of a trailing `:'. */
      value = (char*)malloc_w(1);
      value[0] = '\0';
    }
  else
    value = sub_string(string, start, i);

  return (value);
}

/* note: this function was extracted, with the following changes, from
 * subst.c, a part of bash-2.04. the above license is the original license
 * of subst.c and all of bash-2.04 
 *
 * changes: xmalloc changed to malloc, changed function declaration
 */

/* Cons a new string from STRING starting at START and ending at END,
   not including END. */
static char *sub_string(char *string, int start, int end) {
	register int len;
	register char *result;

	len = end - start;
	result = malloc (len + 1);
	strncpy (result, string + start, len);
	result[len] = '\0';
	return (result);
}
