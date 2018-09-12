#include "unp.h"
#include <stdarg.h>
#include <syslog.h>		/* for syslog() */


#define LOG 1
#define NOLOG 0

int		daemon_proc;		/* set nonzero by daemon_init() */

static void err_doit(int, int, const char *, va_list);


/* Nonfatal error related to system call
 * Print message and return */
void err_ret(const char *fmt,...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(LOG, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to system call
 * Print message and terminate */

void err_sys(const char *fmt,...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(LOG, LOG_INFO, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

/* Nonfatal error unrelated to system call
 * Print message and return */

void err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(LOG, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to system call
 * Print message and terminate */

void err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(NOLOG, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}


/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */

static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int 	errno_save, n;
	char 	buf[MAXLINE + 1];

	errno_save = errno;		/* value caller might want printed */
	
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);	/* safe */
#else
	vsprintf(buf, fmt, ap);		/* not safe */
#endif	
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_save));
	strcat(buf,"\n");
	
	if (daemon_proc)
		syslog(level,"err_doit: %s", buf);
	else {
		fflush(stdout);		/* incase stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}