#include "unp.h"


void sig_pipe(int signo)
{
	fprintf(stderr, "SIGPIPE received \n");
	return;
}