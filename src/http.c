/* $Id: http.c,v 1.3 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef USE_CURL
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#endif /* USE_CURL */
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

#define BLANKSPACE "                                                           "

static int num_connects = 0;

#ifdef USE_CURL

CURL *curl;
FILE *devnull;

void initialize_submit(void) {

	devnull = fopen("/dev/null", "w");
	curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "initialize_submit: couldn't allocate curl\n");
		exit(EXIT_FAILURE);
	}
}

int submit(char *username, char *password) {

	CURLcode result;
	char error[CURL_ERROR_SIZE] = "";
	char authstring[92];
	time_t before, after;

	
	num_connects++;
	if (max_connects == num_connects && max_connects != 0) {
		if (!quiet)
			printf("max connects reached at %i\n", num_connects);
		fprintf(log, "max connects reached at %i\n", num_connects);
		exit(EXIT_SUCCESS);
	}

	snprintf(authstring, sizeof(authstring), "%s:%s", username, password);

	if (!quiet && (debug_level == 0)) {
		printf("trying %s->%s\r", username, password);
		fflush(stdout);
	} else
		printf("trying %s->%s\n", username, password);

	before = time(0);

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
		curl_easy_setopt(curl, CURLOPT_FILE, devnull);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
		curl_easy_setopt(curl, CURLOPT_USERPWD, authstring);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MUTE, 1);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
		if (strcmp(proxy, "undef"))
			curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
		result = curl_easy_perform(curl);
	}

	after = time(0);
	/* num_connects jumps the gun earlier on */
//	acs = stats(acs, after-before, num_connects-1);

	if (!quiet) {
		printf("%s\r", BLANKSPACE);
		fflush(stdout);
	}

	debug(3, "curl_result[%i]: %s\n", result, error);

	if (result == 6) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	} else if (result == 5) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	} else if (result == 0) {
		return(EXIT_SUCCESS);
	} else if (result == 22) {
		return(EXIT_FAILURE);
	}

	return(EXIT_FAILURE);
}

void shutdown_submit(void) {
	curl_easy_cleanup(curl);
}
#endif /* USE_CURL */

#ifdef USE_DUMMY
char **submit_dummy_list = NULL;

void initialize_submit(void) {}
void shutdown_submit(void) {}

int submit(char *username, char *password) {
	int i=0;
	char *path;
	char *valid_username;
	char *valid_password;

	valid_username = (char*)malloc_w(sizeof(char)*81);

	if (max_connects == num_connects && max_connects != 0) {
		if (!quiet)
			printf("max connects reached at %i\n", num_connects);
		fprintf(log, "max connects reached at %i\n", num_connects);
		exit(EXIT_SUCCESS);
	}
	num_connects++;
	
	if (!submit_dummy_list) {
		path = search_path(submit_dummy_file, pathlist);
		submit_dummy_list = textlist(path);
	}	

	if (!quiet && (debug_level == 0))	{
		printf("trying %s->%s\r", username, password);
		fflush(stdout);
	} else if (debug_level > 0)
			printf("trying %s->%s\n", username, password);

	while (submit_dummy_list[i] != NULL) {
		strncpy(valid_username, submit_dummy_list[i], sizeof(char)*81);
		valid_password = extract(valid_username, ':');
		if (!valid_password) {
			fprintf(stderr, "submit_dummy: list element invalid\n");
			exit(EXIT_FAILURE);
		}
		if (!strcmp(valid_username, username) && !strcmp(valid_password, password)) {
			free(valid_username);
			if (!quiet && (debug_level == 0)) {
				printf("%s\r", BLANKSPACE);
				fflush(stdout);
			}
			return(EXIT_SUCCESS);
		}
		i++;
	}

	if (!quiet && (debug_level == 0)) {
		printf("%s\r", BLANKSPACE);
		fflush(stdout);
	}
	free(valid_username);
	return(EXIT_FAILURE);
}
#endif /* USE_DUMMY */
