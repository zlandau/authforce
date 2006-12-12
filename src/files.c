/* $Id: files.c,v 1.4 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <readline/readline.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

//#define MAX_ENTRIES 10000
#define BUFFER_LEN 82

int num_lines=0;

char **textlist(char *textfile) {
	FILE *fp;
	char **wordlist;
	char buffer[BUFFER_LEN];
	int count;
	char *chop;
	struct stat status;

	num_lines = 0;

	stat(textfile, &status);
	
	if (S_ISDIR(status.st_mode)) {
		fprintf(stderr, "textfile: %s is a directory, quitting\n", textfile);
		exit(EXIT_FAILURE);
	}

	fp = fopen(textfile, "r");

	if (!fp) {
		fprintf(stderr, "textlist: error opening %s\n", textfile);
		exit(EXIT_FAILURE);
	}

	while (!feof(fp)) {
		fgets(buffer, sizeof(buffer), fp);
		if (buffer[0] != '#' && buffer[0] != ';' && buffer[0] != '\n')
			num_lines++;
	}

	if (num_lines != 0)
		num_lines--;

	debug(3, "textlist: reading %s [%i]\n", textfile, num_lines);

	rewind(fp);

	/* add +1 to num_lines to make room for NULL */

	wordlist = malloc_w((num_lines+1)*sizeof(char*));

	count=0;
	while (fgets(buffer, sizeof(buffer), fp)) {
		if (buffer[0] == '#' || buffer[0] == ';' || buffer[0] == '\n')
			continue;	
		wordlist[count] = (char*)strdup_w(buffer);
		chop = (char*)strstr(wordlist[count], "\n"); /* credits to cgichk */
		if (chop) *chop = 0;						 /* and toby deshane  */
		count++;
	}

	/* MEMWATCH: this isnt freed, why? */
	wordlist[count] = malloc_w(sizeof(NULL));
	wordlist[count] = (char)NULL;

	fclose(fp);

	return(wordlist);
}

/* make copy of list returned by textlist above */
char **copy_list(char **list) {
	int i=0;
	char **n_list; /* new list */

	/* find num of elements */
	while (list[i] != (char)NULL) 
		i++;
	/* plus one for NULL */
	i++;

	n_list = malloc_w(i*sizeof(char*));

	i=0;
	while (list[i] != (char)NULL) {
		n_list[i] = (char*)strdup_w(list[i]);
		i++;
	}

	n_list[i] = (char)NULL;

	return (n_list);
}

void free_list(char **list) {
	int i=0;

	while (list[i] != (char)NULL) {
		free(list[i]);
		i++;
	}
	free(list);
}

void read_session(char *sessionfile) {
	FILE *fp;
	char buffer[30];
	int result;

	fp = fopen(sessionfile, "r");

	if (!fp) {
		fprintf(stderr, "read_session: error opening %s\n", sessionfile);
		exit(EXIT_FAILURE);
	}

	fgets(buffer, sizeof(buffer), fp);
	result = sscanf(buffer, "%i, %i, %i", &session_usernumber, &session_function, &session_count);

	if (result != 3) {
		fprintf(stderr, "read_session: invalid format (#, #, #)\n");
		exit(EXIT_FAILURE);
	}

	debug(3, "read_session: reading %s [%i, %i, %i]\n", sessionfile, session_usernumber, session_function, session_count);
		
	fclose(fp);
}

void write_session(char *sessionfile) {
	FILE *fp;

	fp = fopen(sessionfile, "w");

	if (!fp) {
		fprintf(stderr, "write_session: error opening %s\n", sessionfile);
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%i, %i, %i\n", session_usernumber, session_function, session_count);

	debug(3, "write_session: writing %s [%i, %i, %i]\n", sessionfile, session_usernumber, session_function, session_count);

	fclose(fp);
}

/* search : delimited pathlist for filename, if found returns full path else
 * returns NULL
 */

char *search_path(char *filename, char *pathlist) {
	int i = 0;
	char *curpath;
	char *fullpath;

	/* check for absolute filenames */
	if (*filename == '~') {
		debug(3, "search_path: attempting to access %s\n", tilde_expand(filename));
		if (access(tilde_expand(filename), R_OK) == 0)
			return(tilde_expand(filename));
	}
	if (*filename == '.' && *(filename+1) == '.') {
		debug(3, "search_path: attempting to access %s\n", filename);
		if (access(filename, R_OK) == 0)
			return(filename);
	}
	if (*filename == '/') {
		debug(3, "search_path: attempting to access %s\n", filename);
		if (access(filename, R_OK) == 0)
			return(filename);
	}

	if (rindex(filename, '/')) {
		filename = rindex(filename, '/');
		filename++;
	}

	fullpath = (char*)malloc_w(sizeof(char)*80);
	/* then look for it in the path */
	while ((curpath = extract_colon_unit(pathlist, &i)) != NULL) {
		snprintf(fullpath, sizeof(char)*80, "%s/%s", curpath, filename);
		debug(3, "search_path: attempting to access %s\n", fullpath);
		if (access(fullpath, R_OK) == 0) {
			free(curpath);
			return(fullpath);
		}
		free(curpath);
	}

	free(fullpath);
	free(curpath);

	fprintf(stderr, "search_path: couldn't find or read %s\n", filename);
	exit(EXIT_FAILURE);
}
