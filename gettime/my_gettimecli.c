#include "unp.h"

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("param error!\n");
		return -1;
	}

	struct sockaddr_in servaddr;
	int sock_fd;
	int nread;
	char read_buf[128] = {0};

	bzero (&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (43333);
	inet_aton (argv[1], &servaddr.sin_addr);

	sock_fd = socket (AF_INET, SOCK_STREAM, 0);

	connect (sock_fd, (struct sockaddr *)&servaddr, sizeof (servaddr));

	char *p_buf = read_buf;
	while ( (nread = read (sock_fd, p_buf, sizeof(read_buf))) > 0)
		p_buf += nread;

	printf ("time is: %s", read_buf);
	return 0;
}
