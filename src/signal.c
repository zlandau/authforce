/* $Id: signal.c,v 1.4 2001/04/28 20:32:47 kapheine Exp $ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifdef MEMWATCH
#include "memwatch.h"
#endif /* MEMWATCH */
#include "extern.h"

void save_session_handler() {

	if (!quiet)
		printf("Saving session to %s\n", session_file);
	write_session(session_file);

	exit(EXIT_SUCCESS);
}

void install_handler() {

	sigset_t sigint;

	debug(3, "install_handler: signal SIGINT installing\n");

	signal(SIGINT, save_session_handler);
	sigemptyset(&sigint);
	sigaddset(&sigint, SIGINT);
	sigprocmask(SIG_UNBLOCK, &sigint, NULL);
}
