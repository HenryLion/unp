#include "unp.h"

// 本程序是书上的echo程序，用来关注一下1、并发 2、服务端的读取和写回操作

ssize_t writen (int fd, const void *buf, size_t n)
{
	const void *ptr = buf;
	size_t nleft = n;
	ssize_t nwrite;

	while (nleft > 0)
	{
		if( (nwrite = write (fd, buf, n)) <= 0)
		{
			if (nwrite < 0 && errno == EINTR)
				nwrite = 0;
			else
				return -1;
		}
		nleft -= nwrite;
		buf += nwrite;
	}

	return n - nleft;
}


void str_echo (int fd)
{
	ssize_t n;
	char buf[256];

again:
	while ( (n = read (fd, buf, 256)) > 0)
		writen (fd, buf, n);

	if (n < 0)
	{
		if (errno == EINTR)
			goto again;
		else
			err_sys ("str_echo: read error");
	}
}


int main (void)
{
	int listen_fd, conn_fd;

	socklen_t ad_len;
	pid_t pid;
	struct sockaddr_in serv_addr, cli_addr;

	bzero (&serv_addr, sizeof (serv_addr));
	bzero (&cli_addr, sizeof (cli_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (34567);
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	listen_fd = Socket (AF_INET, SOCK_STREAM, 0);

	Bind (listen_fd, (struct sockaddr *)&serv_addr, sizeof (serv_addr));

	Listen (listen_fd, 6);

	while (1)
	{
		ad_len = sizeof (cli_addr);
		conn_fd = Accept (listen_fd, (struct sockaddr *)&cli_addr, &ad_len);

		if ( (pid = fork()) == 0) // 子进程处理
		{
			Close (listen_fd);
			str_echo (conn_fd);
			exit (0);
		}
		
		Close (conn_fd);
	}

	return 0;
}

