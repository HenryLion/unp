#include "unp.h"
#include <time.h>

int main (void)
{
	int sock_fd, conn_fd;
	struct sockaddr_in servaddr,cliaddr;
	time_t tm;
	char w_buf[128]={0};
	socklen_t sock_len;

	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (43333);
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	sock_fd = socket (AF_INET, SOCK_STREAM, 0);

	bind (sock_fd, (struct sockaddr *)&servaddr, sizeof (servaddr));

	listen (sock_fd, 5);

	while (1)
	{
		sock_len = sizeof (servaddr);
		conn_fd = accept (sock_fd, (struct sockaddr *)&cliaddr, &sock_len);
		printf ("accetp cli ip is: %s\n", inet_ntoa (cliaddr.sin_addr));
		tm = time (NULL);
		snprintf (w_buf, 128, "%s\r\n", ctime (&tm));
		write (conn_fd, w_buf, 128);
		close (conn_fd);
	}
	return 0;
}
		
