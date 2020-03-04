#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "sock.h"


int main (int argc, char *argv[])
{
	int sockfd, connfd;
	struct sockaddr_in serv_addr;
	int n_read;
	char r_buf[1024];
	memset (&serv_addr, 0, sizeof (struct sockaddr_in));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (6666);
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("socket error\n");
		return -1;
	}
	
	if (bind (sockfd, (struct sockaddr*)&serv_addr, sizeof (struct sockaddr)) < 0)
	{
		printf ("bind error\n");
		return -1;
	}

	if (listen (sockfd, 5) < 0)
	{
		printf ("listen error\n");
		return -1;
	}

	if ((connfd = accept (sockfd, NULL, NULL)) < 0)
	{
		printf ("accept error\n");
		return -1;
	}

	while (1)
	{
		memset (r_buf, 0, sizeof (r_buf));
		if ( (n_read = read (connfd, r_buf, 1024)) > 0)
		{
			writeN (connfd, r_buf, n_read);
		}
	}

	return 0;
}

