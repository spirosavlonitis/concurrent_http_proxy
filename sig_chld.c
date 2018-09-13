#include "unp.h"

void sig_chld(int signo)
{
	int stat;
	pid_t chldpid;

	while ( (chldpid = waitpid(-1, &stat, WNOHANG)) > 0) {
		if (stat)
			fprintf(stderr, "child %d treminated with status %d\n",chldpid, WEXITSTATUS(stat));
	}
	return;
}