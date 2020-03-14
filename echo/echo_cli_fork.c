#include "unp.h"

// 本程序和echo_srv_fork.c配对，是其的客户端程序
// 客户端的动作是从标准输入读取一行文本，发送给服务端，然后再从服务端读回文本并显示在标准输出

void str_cli_0 (FILE *fp, int sock_fd) // 这个函数的问题是当服务器进程退出后，次函数不能及时的处理，因为其阻塞在fgets上
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
		nwrite = writen (sock_fd, buf, 1);
		sleep(1);
		if ( writen (sock_fd, buf+1, strlen(buf)-1) < 0)
		{
			err_sys ("write error");
		}
		if ( (nread = readn (sock_fd, r_buf, strlen(buf))) == 0)
		{
			err_quit ("str_cli: server terminated prematurely");
		}
		r_buf[nread] = '\0';
		fputs (r_buf, stdout);
	}
}

void str_cli (FILE *fp, int sock_fd)
{
	int maxfd;
	fd_set r_set;
	FD_ZERO (&r_set);
	char r_buf[256];
	char w_buf[256];
	int nwrite;
	while (1)
	{
		FD_SET (fileno(fp), &r_set);
		FD_SET (sock_fd, &r_set);
		maxfd = max (fileno(fp), sock_fd) + 1;
		select (maxfd, &r_set, NULL, NULL, NULL);
		if (FD_ISSET (fileno (fp), &r_set))
		{
			fgets (w_buf, 256, fp);
			if (strncmp (w_buf, "quit", 4) == 0)
			{
				close (sock_fd);
				exit (0);
			}
			writen (sock_fd, w_buf, strlen (w_buf));
		}

		if (FD_ISSET (sock_fd, &r_set))
		{
			if (Readline (sock_fd, r_buf, 256) == 0)	
				err_quit ("str_cli: server terminated prematurely");
			fputs (r_buf, stdout);
		}
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
