#include "unp.h"

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("param error\n");
		return -1;
	}

	int sock_fd;
	struct sockaddr_in serv_addr;
	char buf[256];
	char r_buf[256];
	int nread, nwrite;

	bzero (&serv_addr, sizeof (serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (35000);
	inet_pton (AF_INET, argv[1], &serv_addr.sin_addr);

	sock_fd = socket (AF_INET, SOCK_STREAM, 0);

	connect (sock_fd, (struct sockaddr *)&serv_addr, sizeof (serv_addr));

	while (1)
	{
		fgets (buf, sizeof (buf), stdin);
		if ( strncmp (buf, "quit", 4) == 0)
		{
			close (sock_fd);
			break;
		}
		nwrite = write (sock_fd, buf, strlen (buf));

		nread = read (sock_fd, r_buf, nwrite);
		r_buf[nread] = '\0';

		fputs (r_buf, stdout);
	}
	return 0;
}

