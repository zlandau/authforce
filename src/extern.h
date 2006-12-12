/* $Id: extern.h,v 1.4 2001/04/28 20:32:47 kapheine Exp $ */

/* main.c */
#include <config.h>
#include <libintl.h>
#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

/* command line configurable variables */
extern char datafile_path[60];          /* location of data files */
extern char username_file[80];          /* file containing usernames */
extern char passwords_file[80];         /* file containing common passes */
extern char common_pairs_file[80];      /* file containing common pairs */
#ifdef USE_DUMMY
extern char submit_dummy_file[80];		/* file containing dummy combos */
#endif /* USE_DUMMY */
extern char logfile[80];   	            /* file to log to */
extern unsigned int max_users;          /* max number of users to try */
extern unsigned int max_connects;	    /* max num of connects */
extern unsigned int per_user_delay;     /* delay between trying users */
extern unsigned int per_password_delay; /* delay between attempt */
extern char user_agent[50];             /* username to pass to browser */
extern char url[160];                   /* url to bruteforce */
extern char proxy[40];                  /* proxy support */
extern int quiet;					    /* no output to stdout? */
extern int debug_level;                 /* what level of debug messages to show 0-5 */
extern int beep; 	                    /* beep on found? */

extern char *user_pass[2];			    /* username, password */
extern FILE *log;					    /* log file handle */
extern unsigned int found;				/* valid auth found */
extern unsigned int num_users;	        /* cur number of users tried */
extern int session_usernumber;          /* current usernumber */
extern int session_function; 	        /* current function */
extern int session_count; 	            /* current loop */
extern int resume_session;				/* resume old session? */
extern int save_session;				/* save session on USR1? */
extern char session_file[80];			/* file containing session data */
extern int skipped_password;            /* kludge, see explanation below */
//extern float acs;						/* average connects per second */
extern char pathlist[120];				/* path list for data files */
extern char **master_password_list;     /* master password list */

/* debug.c */
extern void debug(int level, char *arguments, ...);
extern void warning(char *arguments, ...);

/* methods.c */
extern int process_passwords(char *username);
extern char *transform(char *username, char *password);

extern void common_pairs(void);

typedef enum {
	passwords_ident,
	common_pairs_ident
} session_ident_types;

/* http.c */
extern void initialize_submit(void);
extern int submit(char *username, char *password);
extern void shutdown_submit(void);
#ifdef USE_DUMMY
char **submit_dummy_list;
#endif /* USE_DUMMY */

/* files.c */
extern char **textlist(char *textfile);
extern char **copy_list(char **list);
extern void free_list(char **list);
extern int num_lines;
extern void read_session(char *sessionfile);
extern void write_session(char *sessionfile);
extern char *search_path(char *filename, char *pathlist);

/* signal.c */
extern void save_session_handler();
extern void install_handler();

/* config.c */
extern void parse_config(char *config);

/* misc.c */
extern void *malloc_wrapper(size_t size);/* malloc wrapper w/ error checking */
extern char *strdup_wrapper(const char *s);/* strdup wrapper w/ error checking */
extern char *extract(char *str, char token);
extern float stats(float average, float element, int num);
extern char *strrev(char *str);
extern void remove_crud(char *str);

/* extract.c */
extern char *extract_colon_unit(char *string, int *p_index);

/* if debugging, use real versions so memwatch can detect linenumbers
 * otherwise, use wrappers 
 */

#ifdef MEMWATCH
	#define malloc_w malloc
	#define strdup_w strdup
#else
	#define malloc_w malloc_wrapper
	#define strdup_w strdup_wrapper
#endif /* MEMWATCH */
