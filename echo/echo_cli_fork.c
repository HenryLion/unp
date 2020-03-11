#include "unp.h"

// 本程序和echo_srv_fork.c配对，是其的客户端程序
// 客户端的动作是从标准输入读取一行文本，发送给服务端，然后再从服务端读回文本并显示在标准输出

void str_cli (FILE *fp, int sock_fd)
{
	char buf[256];
	char r_buf[256];
	ssize_t nwrite, nread;
	while (1)
	{
		fgets (buf, 256, fp);
		if ( strncmp (buf, "quit", 4) == 0)
		{
			Close (sock_fd);
			return;
		}
		nwrite = writen (sock_fd, buf, strlen (buf));
		nread = read (sock_fd, r_buf, nwrite);
		r_buf[nread] = '\0';
		fputs (r_buf, stdout);
	}
}

int main (int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("usage: a.out [IP]\n");
		return -1;
	}

	int sock_fd;
	struct sockaddr_in serv_addr;

	ssize_t nwrite;

	bzero (&serv_addr, sizeof (serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (34567);
	inet_pton (AF_INET, argv[1], &serv_addr.sin_addr);

	sock_fd = Socket (AF_INET, SOCK_STREAM, 0);

	Connect (sock_fd, (struct sockaddr*)&serv_addr, sizeof (serv_addr));

	str_cli (stdin, sock_fd);

	return 0;
}