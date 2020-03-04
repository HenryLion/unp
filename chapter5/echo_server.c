#include "unp.h"

void sig_chld (int signo)
{
	pid_t pid;
	int stat;
	pid = wait (&stat);
	printf ("child %d terminated\n", pid);
	return;
}

int main (void)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket (AF_INET, SOCK_STREAM, 0);
	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (SERV_PORT);

	Bind (listenfd, (SA*)&servaddr, sizeof (servaddr));

	Listen (listenfd, LISTENQ);

	signal (SIGCHLD, sig_chld);

	for (;;)
	{
		clilen = sizeof (cliaddr);
		connfd = Accept (listenfd, (SA*) &cliaddr, &clilen);
		if ( (childpid = Fork ()) == 0)
		{
			Close (listenfd);
			str_echo (connfd);
			exit (0);
		}
		Close (connfd);
	}
}
