#include "unp.h"

void dg_cli (FILE *fp, int fd, const struct sockaddr *servaddr, socklen_t addr_len)
{
	int n;
	char sendline[256], recvline[256+1];

	while ( fgets (sendline, 256, fp) != NULL)
	{
		Sendto (fd, sendline, strlen (sendline), 0, servaddr, addr_len);

		n = Recvfrom (fd, recvline, 256, 0, NULL, NULL);

		recvline[n] = 0;
		fputs (recvline, stdout);
	}
}

int main (int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit ("usage: udpcli <IPaddress>");

	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (34567);
	inet_pton (AF_INET, argv[1], &servaddr.sin_addr);

	sockfd = Socket (AF_INET, SOCK_DGRAM, 0);

	dg_cli (stdin, sockfd, (struct sockaddr*)&servaddr, sizeof (servaddr));

	exit (0);
}


