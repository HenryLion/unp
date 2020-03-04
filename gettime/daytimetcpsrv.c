#include "unp.h"

int main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	char writebuf[1];
	time_t ticks;

	listenfd = Socket (AF_INET, SOCK_STREAM, 0);
	bzero (&servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons (9999);

	Bind (listenfd, (SA *) &servaddr, sizeof (servaddr));
	Listen (listenfd, LISTENQ);

	for (; ;)
	{
		connfd = Accept (listenfd, (SA *) NULL, NULL);
		ticks = time (NULL);
		snprintf (buff, sizeof (buff), "hello,time is: %.24s\r\n", ctime (&ticks) );
		int i = 0;
		writebuf[0] = buff[0];
		while (writebuf[0] != '\0') 
		{
			Write (connfd, writebuf, sizeof (char));
			writebuf[0] = buff[++i];
		}
		writebuf[0] = '\0';
		Write (connfd, writebuf, sizeof (char));
		Close (connfd);
	}
}
