#include "unp.h"

void proxy(int);

int main(int argc, char *argv[])
{
	char	buf[MAXLINE];
	int		listenfd, connfd;
	struct sockaddr_in	servaddr, cliaddr;
	socklen_t	clilen;
	pid_t	chldpid;
	void 	sig_chld(int);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	Bind(listenfd, (SA *) &servaddr, sizeof servaddr);

	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld);
	
	for (; ;) {
		clilen = sizeof(struct sockaddr_in);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		printf("New connection from %s:%d\n", 
			   Inet_ntop(AF_INET, (SA *) &cliaddr.sin_addr, buf, clilen),ntohs(cliaddr.sin_port));

		if ((chldpid = fork()) == 0) {
			Close(listenfd);
			proxy(connfd);
			exit(EXIT_SUCCESS);
		}
		Close(connfd);

	}

 
	exit(EXIT_SUCCESS);
}