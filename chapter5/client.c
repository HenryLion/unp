#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "sock.h"

int main (int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in addr;
	char read_buf[1024];
	char w_buf[1024];

	int n_write, n_read;
	if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0) ) < 0)
	{
		printf ("socket error\n");
		return -1;
	}

	memset (&addr, 0, sizeof (addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons (6666);
	
	if (inet_pton (AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
	{
		printf ("inet_pton error\n");
		return -1;
	}

	if (connect (sockfd, (struct sockaddr*)&addr, sizeof (struct sockaddr)) < 0)
	{
		printf ("connect error\n");
		return -1;
	}

	while (1)
	{
		memset (w_buf, 0, sizeof (w_buf));
		memset (read_buf, 0, sizeof (read_buf));

		if (fgets (w_buf, 1024, stdin) != NULL)
		{
			n_write = writeN (sockfd, w_buf, strlen(w_buf));
		}

		readN (sockfd, read_buf, n_write);
		read_buf[n_write] = '\0';

		fputs (read_buf, stdout);

	}
	return 0;
}



