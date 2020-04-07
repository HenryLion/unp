#include "unp.h"

void dg_echo (int sock_fd, SA* cliaddr, socklen_t clilen)
{
	int n;
	socklen_t len;
	char msg[1024];
	while (1)
	{
		len = clilen;
		n = Recvfrom (sock_fd, msg, 1024, 0, cliaddr, &len); // 如果udp客户端写的数据大于recvfrom读去的数据量，则在服务端未读取的数据被丢弃了。
		msg[n] = 0;
		Sendto (sock_fd, msg, n, 0, cliaddr, len);
	}
	return;
}

int main (void)
{
	int sock_fd;
	struct sockaddr_in servaddr, cliaddr;
	bzero (&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (7777);
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	sock_fd = Socket (AF_INET, SOCK_DGRAM, 0);

	Bind (sock_fd, (SA*)&servaddr, sizeof (servaddr));

	dg_echo (sock_fd, (SA*) &cliaddr, sizeof (cliaddr));

	return 0;
}
