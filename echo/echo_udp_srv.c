#include "unp.h"

void dg_echo (int fd, struct sockaddr *cliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[256];
	char cli_ip[INET_ADDRSTRLEN];

	while (1)
	{
		len = clilen;
		n = Recvfrom (fd, mesg, 256, 0, cliaddr, &len);
		printf ("client ip = %s, port = %d\n", inet_ntop (AF_INET, &((struct sockaddr_in *)cliaddr)->sin_addr ,cli_ip, sizeof(cli_ip)), ntohs (((struct sockaddr_in *)cliaddr)->sin_port));
		Sendto (fd, mesg, n, 0, cliaddr, len);
	}
}


int main (int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = Socket (AF_INET, SOCK_DGRAM, 0);

	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (34567);
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	Bind (sockfd, (struct sockaddr *)&servaddr, sizeof (servaddr));
	dg_echo (sockfd, (struct sockaddr*)&cliaddr, sizeof (cliaddr));
}


