#include "unp.h"

#define HTTP_PORT	"80"

static void get_domain(char *, char *);

void proxy(int clifd)
{
	char		buf[MAXLINE], domain[MAXLINE];
	ssize_t 	n;
	int 		sockfd,	maxfd;
	struct addrinfo		hints, *servaddr;
	int 		clieof;
	fd_set		rset;
	struct timeval	timeout;
	void sig_pipe(int);

	if ( (n = Read(clifd, buf, MAXLINE)) == 0)
		return;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	get_domain(buf, domain);
	
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	getaddrinfo(domain, HTTP_PORT, &hints, &servaddr);

	Signal(SIGPIPE, sig_pipe);
	timeout.tv_sec = 10;
	maxfd = max(clifd, sockfd) + 1;
	clieof = 0;

	Connect(sockfd, (SA *) servaddr->ai_addr, servaddr->ai_addrlen);

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	Writen(sockfd, buf, n);
	for (; ;) {
		if (clieof == 0)
			FD_SET(clifd ,&rset);

		if (!Select(maxfd, &rset, NULL, NULL, &timeout))
			err_quit("proxy: timeout");

		if (FD_ISSET(sockfd, &rset)) {
			if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
				if (clieof == 0)
					err_quit("Remote server terminated primaturely");
				else {
					return;
				}
			}else if (n == -1)
				err_sys("read error");
			Writen(clifd, buf, n);
		}

		if (FD_ISSET(clifd, &rset)) {
			if ( (n = Read(clifd, buf, MAXLINE)) == 0) {
				clieof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(clifd, &rset);
				continue;
			}else if (n == -1)
				err_sys("read error");
			Writen(sockfd, buf, n);
		}
	}


	
}


static void get_domain(char *r, char *d)
{
	int i;
	char *ptrn;

	ptrn = "Host: ";
	for (; *r  ; r++){
		for (i = 0 ; r[i] == ptrn[i] ; ++i)
			i++;
		if (ptrn[i] == '\0') {
			r += i;
			break;
		}
	}
	
	for (; *r != '\r' ; r++, d++)
		*d = *r;
	*d = '\0';
}