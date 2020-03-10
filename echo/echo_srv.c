#include "unp.h"

int is_lower_alpha (char ch)
{
	return (ch >= 'a' && ch <= 'z');
}

int main (void)
{
	int sock_fd, conn_fd;
	struct sockaddr_in serv_addr;
	char r_buf[256];
	int nread, nwrite,i;
	bzero (&serv_addr, sizeof (serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (35000);
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	sock_fd = socket (AF_INET, SOCK_STREAM, 0);

	bind (sock_fd, (struct sockaddr *)&serv_addr, sizeof (serv_addr));

	listen (sock_fd, 5);

	conn_fd = accept (sock_fd, (struct sockaddr *)NULL, NULL); // 只能accept一个客户端，且这个客户端close之后重连连不上，需要重启服务端

	while (1)
	{
		nread = read (conn_fd, r_buf, 256);
		for (i = 0; i < nread; ++i)
		{
			if (is_lower_alpha(r_buf[i]))
				r_buf[i] += ('A' - 'a');
		}

		nwrite = write (conn_fd, r_buf, nread);
	}

	return 0;
}
