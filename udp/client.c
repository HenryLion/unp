#include "unp.h"

void create_msg (char *msg, int msg_len)
{
	static char *chs = "abcedfghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int chs_len = strlen (chs);
	int i = 0;
	while (i < msg_len)
	{
		msg[i] = chs[rand()%chs_len];
		++i;
	}
	msg[i] = '\0';
	return;
}

void dg_client (FILE *fp, int sock_fd, const SA* servaddr, socklen_t servlen )
{
	int n;
	char sendline[70000], recvline[70000];
	int rand_msg_len = 0;
	while (1)
	{
		getchar ();
		rand_msg_len = 20000;
		printf ("will send %d chrs.\n",rand_msg_len);
		create_msg (sendline, rand_msg_len);
		Sendto (sock_fd, sendline, strlen(sendline), 0, servaddr, servlen);
		n = Recvfrom (sock_fd, recvline, 100, 0, NULL, NULL);

		recvline[n] = 0;
		Fputs (recvline, stdout);
	}
	return;
}

int main (int argc, char *argv[])
{

	srand (time(NULL));
	int sock_fd;
	struct sockaddr_in servaddr;

	if (argc != 2)
		err_quit ("usage: udpcli <IPaddress> msg_len");


	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (7777);
	Inet_pton (AF_INET, argv[1], &servaddr.sin_addr);

	sock_fd = Socket (AF_INET, SOCK_DGRAM, 0);

	dg_client (stdin, sock_fd, (SA*) &servaddr, sizeof(servaddr));

	exit (0);
}



