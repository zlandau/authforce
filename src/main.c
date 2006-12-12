/* $Id: main.c,v 1.7 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"
#include <libintl.h>

static const char *program_name = PACKAGE;
static const char *author = "Zachary P. Landau";
static const char *version = VERSION;
static const char *email = "kapheine@hypa.net";
static const char *description = "an HTTP authentication brute forcer";

/* command line configurable variables */
char datafile_path[60] = "data";              /* location of data files */
char username_file[80] = "username.lst";      /* file containing usernames */
char passwords_file[80] = "password.lst";     /* file with password rules */
char common_pairs_file[80] = "userpass.lst";  /* file containing common pairs */
#ifdef USE_DUMMY
char submit_dummy_file[80] = "dummy.lst";     /* file containing dummy combos */
#endif /* USE_DUMMY */
char logfile[80] = "/dev/null";				  /* file to log to */
unsigned int max_users = 0;					  /* max number of users to try */
unsigned int max_connects = 0;				  /* max num of connects */
unsigned int per_user_delay = 0;			  /* delay between trying users */
unsigned int per_password_delay = 0;		  /* delay between attempt */
char user_agent[50];						  /* username to pass to browser */
char url[160];								  /* url to bruteforce */
char proxy[40] = "undef";					  /* proxy support */
int quiet = 0;								  /* no output to stdout? */
int debug_level = 0;						  /* what level of debug messages to show 0-5 */
int beep = 0;								  /* beep on found? */
char pathlist[120] = DEFAULT_PATH;
char configlist[120] = DEFAULT_CONFIG;

char *user_pass[2];							/* username, password */
FILE *log;									/* log file handle */
unsigned int found = 0;						/* valid auth found */
unsigned int num_users = 0;					/* cur number of users tried */
int session_usernumber = 0; 				/* current usernumber */
int session_function = passwords_ident;     /* current function */
int session_count = 0;						/* current loop */
int resume_session = 0;						/* resume old session? */
int save_session = 0;						/* save session? */
char session_file[80] = "session.save";		/* file containing session data */
/*unsigned short acs = 0;*/ 				/* average connects per second */
char **master_password_list = NULL;         /* master password list */

static const char *optstring = "bc:d:hl:p:P:qr::s::u:UvV";

struct option long_options[] = {
	{ "beep", 0, NULL, 'b' },
	{ "debug", 1, NULL, 'd' },
#ifdef USE_DUMMY
	{ "dummy-file", 1, NULL, 1 },
#endif /* USE_DUMMY */
	{ "help", 0, NULL, 'h' },
	{ "logfile", 1, NULL, 'l' },
	{ "max-connects", 1, NULL, 'c' },
	{ "max-users", 1, NULL, 'u' },
	{ "pairs-file", 1, NULL, 2 },
	{ "password-delay", 1, NULL, 3 },
	{ "password-file", 1, NULL, 4 },
	{ "path", 1, NULL, 'p' },
	{ "proxy", 1, NULL, 'P' },
	{ "quiet", 0, NULL, 'q' },
	{ "resume", 2, NULL, 'r' },
	{ "save", 2, NULL, 's' },
	{ "user-delay", 1, NULL, 5 },
	{ "user-agent", 1, NULL, 'a' },
	{ "username-file", 1, NULL, 6 },
	{ "verbose", 0, NULL, 'v' },
	{ "version", 0, NULL, 'V' },
	{ NULL }
};

static void help(char *name) {

	printf("%s %s, %s\n\n", program_name, version, description);
	printf(_("usage: %s [OPTION]... URL\n\n"), name);
	printf(_("options:\n"));
	printf(_("\t-b,\t--beep\t\t\tbeep when a match is found\n"
		 	"\t-d,\t--debug=NUMBER\t\t\tlevel of debugging\n"
			"\t\t--dummy-file=FILE\tfile containing dummy matches\n"
			"\t\t\t\t\t[username:password form]\n"
			"\t-h,\t--help\t\t\tdisplay this help and exit\n"
			"\t-l,\t--logfile=FILE\t\tset logfile to FILE\n"
			"\t-r,\t--resume[=FILE]\t\tresume old session using FILE\n"
			"\t\t\t\t\t[default session.save]\n"
			"\t-s,\t--save[=FILE]\t\tsave session on SIGUSR1 to FILE\n"
			"\t\t\t\t\t[default session.save]\n"
			"\t-c,\t--max-connects=NUMBER\tdon't make more than NUMBER connections\n"
			"\t-u,\t--max-users=NUMBER\tdon't try more than NUMBER users\n"
			"\t-U,\t--user-agent=STRING\tset user agent to STRING\n"
			"\t\t--pairs-file=FILE\tfile containing username:password pairs\n"
			"\t\t--password-delay=NUMBER\tdelay for # seconds between attempts\n"
			"\t\t--password-file=FILE\tfile containing common passwords\n"
			"\t-p,\t--path=STRING\t\tlook for pathlist STRING\n"
			"\t-P,\t--proxy=STRING\t\tset proxy to STRING\n"
			"\t-q,\t--quiet\t\t\tdon't output to stdout\n"
			"\t\t--user-delay=NUMBER\tdelay for # seconds between usernames\n"
			"\t\t--username-file=FILE\tfile containing list of usernames\n"
			"\t\t--verbose\t\tbe verbose (default), opposite of --quiet\n"
			"\t-V,\t--version\t\tprint version information and exit\n")
	);
	printf(_("\nReport bugs to <%s>.\n"), email);
	exit(EXIT_SUCCESS);
}


static void initialize_main(int argc, char *argv[]) {
	time_t now;
	int optch;
	int index = 0;
	char *filename;

	if (argc < 1) {
		fprintf(stderr, _("%s: Missing arguments\n"), program_name);
		fprintf(stderr, _("Try %s --help for more information.\n"), program_name);
		exit(EXIT_FAILURE);
	}

	snprintf(user_agent, sizeof(user_agent), "%s/%s", program_name, version);

	while ((filename = extract_colon_unit(configlist, &index)) != NULL) {
		parse_config(filename);
		free(filename);
	}

	while ((optch = getopt_long(argc, argv, optstring, long_options, NULL)) != -1) {
		switch (optch) {
#ifdef USE_DUMMY
			case 1:
				strncpy(submit_dummy_file, optarg, sizeof(submit_dummy_file));
				break;
#endif /* USE_DUMMY */
			case 2:
				strncpy(common_pairs_file, optarg, sizeof(common_pairs_file)); 
				break;
			case 3:
				per_password_delay = atoi(optarg);
				break;
			case 4:
				strncpy(passwords_file, optarg, sizeof(passwords_file));
				break;
			case 5:
				per_user_delay = atoi(optarg);
				break;
			case 6:
				strncpy(username_file, optarg, sizeof(username_file));
				break;
			case 'a':
				strncpy(user_agent, optarg, sizeof(user_agent));
				break;
			case 'b':
				beep = 1;
				break;
			case 'c':
				max_connects = atoi(optarg);
				break;
			case 'd':
				debug_level = atoi(optarg);
				break;
			case 'h':
				help((char*)program_name);
				break;
			case 'l':
				strncpy(logfile, optarg, sizeof(logfile));
				break;
			case 'p':
				strncpy(pathlist, optarg, sizeof(pathlist));
				break;
			case 'P':
				strncpy(proxy, optarg, sizeof(proxy));
				break;
			case 'q':
				quiet = 1;
				break;
			case 'r':
				resume_session = 1;
				if (optarg)
					strncpy(session_file, optarg, sizeof(session_file));
				break;
			case 's':
				save_session = 1;
				if (optarg)
					strncpy(session_file, optarg, sizeof(session_file));
				break;
			case 'u':
				max_users = atoi(optarg);
				break;
			case 'v':
				quiet = 0;
				break;
			case 'V':
				printf( _("%s %s\n"
						"\n"
						"Copyright (C) 2000 %s\n"
						"There is NO warranty.  You may redistribute this software\n"
						"under the terms of the GNU General Public License.\n"
						"For more information about these matters, see the files named COPYING\n"
						"\n"
						"Written by %s <%s>.\n")
						, program_name, version, author, author, email);
				exit(EXIT_SUCCESS);
				break;
		}
	}

	optch = argc - optind;
	switch (optch) {
		case 0:
			fprintf(stderr, _("%s: missing URL\n"), program_name);
			fprintf(stderr, _("Usage: %s [OPTION]... [URL]\n\n"), program_name);
			fprintf(stderr, _("Try `%s --help` for more information.\n"), program_name);
			exit(EXIT_FAILURE);
		case 1:
			break;
		default:
			fprintf(stderr, _("%s: too many URLs\n"), program_name);
			fprintf(stderr, _("Usage: %s [OPTION]... [URL]\n\n"), program_name);
			fprintf(stderr, _("Try %s --help for more information.\n"), program_name);
			exit(EXIT_FAILURE);
	}

	strncpy(url, argv[optind], sizeof(url));

	if (resume_session)
		read_session(session_file);

	if (save_session)
		install_handler();

	log = fopen(logfile, "w+");
	if (!log) {
		fprintf(stderr, _("initialize_data: couldn't open %s\n"), logfile);
		exit(EXIT_FAILURE);
	}
	now = time(0);
	fprintf(log, _("scan started: %s"), ctime(&now));
	
}

static void shutdown_main(void) {
	time_t now;

	now = time(0);
/*	printf("sats: %f\n", acs);*/
	fprintf(log, _("scan ended: %s"), ctime(&now));
	fclose(log);
}

int main(int argc, char *argv[]) {
	int i;
	char **username_list;
	char *username;
	int result;
	char *path = NULL;


#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

	user_pass[0] = (char*)malloc_w(sizeof(char)*41);
	user_pass[1] = (char*)malloc_w(sizeof(char)*51);

	initialize_main(argc, argv);
	initialize_submit();

	/* cheating way to avoid negative numbers :P */
	if (abs(session_function) > 1) {
		fprintf(stderr, _("main: session_function greater than 1\n"));
		exit(EXIT_FAILURE);
	}

	if (session_function == passwords_ident) {

		path = search_path(username_file, pathlist);
		username_list = textlist(path);
		free(path);

		/* session_usernumber starts at 0, but num_lines starts at 1 */
		if (abs(session_usernumber) > (num_lines+1)) {
			fprintf(stderr, _("main: session_usernumber greater than num_lines\n"));
			exit(EXIT_FAILURE);
		}

		i = session_usernumber;

		path = search_path(passwords_file, pathlist);
		master_password_list = textlist(path);

		/* session_usernumber starts at 0, but num_lines starts at 1 */
		if (abs(session_count) > (num_lines-1)) {
			fprintf(stderr, _("main: session_count greater than num_lines\n"));
			exit(EXIT_FAILURE);
		}

		while ((username = username_list[i]) != NULL) {
			result = process_passwords(username);
			if (result == EXIT_SUCCESS) {
				if (!quiet) {
					printf(_("match [%s:%s]\n"), user_pass[0], user_pass[1]);
					if (beep)
						printf("\007");
				}
				fprintf(log, _("match [%s:%s]\n"), user_pass[0], user_pass[1]);
				found++;
			}
			sleep(per_user_delay);
			i++;
			session_usernumber = i;
			if (max_users == num_users && max_users != 0) {
				if (!quiet)
					printf(_("max users reached at %i\n"), num_users);
				fprintf(log, _("max users reached at %i\n"), num_users);
				break;
			}
			num_users++;
		}
		free_list(username_list);
		free_list(master_password_list);
	}


	/* seperate because common_pairs uses different scheme */
	common_pairs();

	if (found > 0) {
		if (!quiet)
			printf(_("Congratulations: %i password%s found.\n"), found, found == 1 ? "" : "s");
		fprintf(log, _("Congratulations: %i password%s found.\n"), found, found == 1 ? "" : "s");
	} else {
		if (!quiet)
			printf(_("No passwords found.\n"));
		fprintf(log, _("No passwords found.\n"));
	}
		
	free(user_pass[0]);
	free(user_pass[1]);
#ifdef USE_DUMMY
	free_list(submit_dummy_list);
#endif /* USE_DUMMY */

	shutdown_submit();
	shutdown_main();

	if (found > 0)
		exit(1); 					/* matches were found */
	else
		exit(0);					/* matches were not found */
}
