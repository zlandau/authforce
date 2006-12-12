/* $Id: config.c,v 1.3 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <readline/tilde.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

#define BUFFER_LEN 82
#define IS_BIT(x) (!strcmp(x, "0") || !strcmp(x, "1"))

static void process_boolean(char *option, char *value, int *variable) {

	if (!IS_BIT(value)) {
		if (!quiet)
			printf("process_boolean: %s value %s is not valid\n", option, value);
	} else
		*variable = atoi(value);
}

void parse_config(char *config) {
	FILE *fp;
	char buffer[BUFFER_LEN];

	char *option;								/* config file option */
	char *value;								/* value for option */

	int line_number = 1;
	char *chop;

	/* MEMWATCH: reports this isnt freed, why? */
	option = malloc_w(((BUFFER_LEN/2)-1)*sizeof(char));

	config = tilde_expand(config);
	fp = fopen(config, "r");

	if (!fp) {
			debug(2, "parse_config: error opening %s, skipping\n", config);
		return;
	}

	while (fgets(buffer, sizeof(buffer), fp)) {
		if (buffer[0] == '#' || buffer[0] == ';' || buffer[0] == '\n')
			continue;
		remove_crud(buffer);
		chop = (char*)strstr(buffer, "\n");		/* credits to cgichk */
		if (chop) *chop = 0;					/* and toby deshane  */
		value = extract(buffer, '=');
		if (!value) {
			fprintf(stderr, "parse_config: invalid config file entry, file %s line %i\n", config, line_number);
			exit(EXIT_FAILURE);
		}
		strncpy(option, buffer, ((BUFFER_LEN/2)-1)*sizeof(char));
		debug(5, "parse_config: (%s) option: %s, value: %s\n", config, option, value);

		if (!strcmp("beep", option))
			process_boolean(option, value, &beep);
		else
		if (!strcmp("debug", option))
			debug_level = atoi(value);
#ifdef USE_DUMMY		
		else
		if (!strcmp("dummy_file", option))
			strncpy(submit_dummy_file, value, sizeof(submit_dummy_file));
#endif /* USE_DUMMY */
		else
		if (!strcmp("logfile", option))
			strncpy(logfile, value, sizeof(logfile));
		else
		if (!strcmp("resume", option))
			if (IS_BIT(value))
				process_boolean(option, value, &save_session);
			else {
				strncpy(session_file, value, sizeof(session_file));
				resume_session = 1;
			}
		else
		if (!strcmp("save", option))
			if (IS_BIT(value))
				process_boolean(option, value, &save_session);
			else {
				strncpy(session_file, value, sizeof(session_file));
				save_session = 1;
			}
		else
		if (!strcmp("max_connects", option))
			max_connects = atoi(value);
		else
		if (!strcmp("max_users", option))
			max_users = atoi(value);
		else
		if (!strcmp("user_agent", option))
			strncmp(user_agent, value, sizeof(user_agent));
		else
		if (!strcmp("pairs_file", option))
			strncmp(common_pairs_file, value, sizeof(common_pairs_file));
		else
		if (!strcmp("password_delay", option))
			per_password_delay = atoi(value);
		else
		if (!strcmp("password_file", option))
			strncmp(passwords_file, value, sizeof(passwords_file));
		else
		if (!strcmp("path", option))
			strncmp(pathlist, value, sizeof(pathlist));
		else
		if (!strcmp("proxy", option))
			strncmp(proxy, value, sizeof(proxy));
		else
		if (!strcmp("quiet", option))
			process_boolean(option, value, &quiet);
		else
		if (!strcmp("user_delay", option))
			per_user_delay = atoi(value);
		else
		if (!strcmp("username_file", option))
			strncmp(username_file, value, sizeof(username_file));
		else
		if (!quiet)
			printf("parse_config: option %s is not valid\n", option);

		line_number++;
	}

	debug(3, "parse_config: read %s [%i]\n", config, line_number-1);

	free(option);
	fclose(fp);
}
