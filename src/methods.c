/* $Id: methods.c,v 1.2 2001/02/10 00:22:46 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

int process_passwords(char *username) {
	int i;
	char *password;
	char **password_list;
	int result;

	session_function = passwords_ident;
	i = session_count;

	/* because transform will make changes to list */
	password_list = copy_list(master_password_list); 

	while ((password = password_list[i]) != NULL) {
		password = transform(username, password);
		result = submit(username, password);
		sleep(per_password_delay);
		if (result == EXIT_SUCCESS) {
			strncpy(user_pass[0], username, sizeof(char)*41);
			strncpy(user_pass[1], password, sizeof(char)*41);
			free(password);
			free_list(password_list);
			session_count = 0;
			return(EXIT_SUCCESS);
		}	
		i++;
		session_count = i;
		free(password);
	}

	session_count = 0;
	free_list(password_list);
	return(EXIT_FAILURE);
}

/* transforms special variables in list
 * parameters:
 *    username currently being worked on
 *    password to be warped
 * new password is returned
 * current transformations:
 */

char *transform(char *username, char *password) {
	char *elements[] = {
		"{username}",
		"{emanresu}",
		NULL
	};

	char *trans_password;		/* transformed password */
	char *old_password;			/* before current transformation */
	char *cur_element;
	char *r_username;			/* reversed username */
	char insertion[41];			/* value to insert in {}s */
	char *left;  				/* left of element */
	char *right;				/* right of element */
	int i=0;

	trans_password = malloc_w(sizeof(char)*41);
	old_password = malloc_w(sizeof(char)*41);
	r_username = strdup_w(username);
	strrev(r_username);

	strncpy(old_password, password, sizeof(char)*41);

	while ((cur_element = elements[i]) != (char)NULL) {
		if (!strcmp(password, "{username}"))
			strncpy(insertion, username, sizeof(insertion));
		else if (!strcmp(password, "{emanresu}"))
			strncpy(insertion, r_username, sizeof(insertion));
		else strncpy(insertion, "", sizeof(insertion));
		while ((right = strstr(old_password, cur_element))) {
			left = old_password;
			*right = '\0';
			right += strlen(cur_element);
			snprintf(trans_password, sizeof(char)*41, "%s%s%s", left, insertion, right);
			strncpy(old_password, trans_password, sizeof(char)*41);
		}
		i++;
	}

	free(trans_password);
	free(r_username);
	return(old_password);
}

void common_pairs(void) {
	int i;
	char **common_pairs_list;
	int result;
	char *path = NULL;
	char *username;
	char *password;

	session_function = common_pairs_ident;
	i = session_count;

	username = (char*)malloc_w(sizeof(char)*81);

	path = search_path(common_pairs_file, pathlist);
	common_pairs_list = textlist(path);
	free(path);

	if (abs(session_count) > (num_lines+1)) {
		fprintf(stderr, "common_pairs: session_count greater than num_lines\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (common_pairs_list[i] == (char)NULL)
			break;
		strncpy(username, common_pairs_list[i], sizeof(char)*81);
		password = extract(username, ':');
		if (!password) {
			fprintf(stderr, "common_pairs: list element invalid\n");
			exit(EXIT_FAILURE);
		}
		result = submit(username, password);
		sleep(per_password_delay);
		if (result == EXIT_SUCCESS) {
			if (!quiet)
				printf("match [%s:%s]\n", username, password);
			if (beep)
				printf("\007");
			fprintf(log, "match [%s:%s]\n", username, password);
			found++;
		}
		i++;
		session_count = i;
		if (max_users == num_users && max_users != 0) {
			if (!quiet)
				printf("max users reached at %i\n", num_users);
			fprintf(log, "max users reached at %i\n", num_users);
			break;
		}
		num_users++;
	}

	session_count=0;
	free(username);
	free_list(common_pairs_list);
}
