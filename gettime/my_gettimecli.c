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
	struct sockaddr_in cliaddr;
	char buf[128] = {0};
	
	socklen_t len = 0;

	bzero (&servaddr, sizeof(servaddr));
	bzero (&cliaddr, sizeof (cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (43333);
	inet_aton (argv[1], &servaddr.sin_addr);

	sock_fd = socket (AF_INET, SOCK_STREAM, 0);

	if ( connect (sock_fd, (struct sockaddr *)&servaddr, sizeof (servaddr)) < 0)
		err_sys ("connect error");
	
	len = sizeof (cliaddr);
	getsockname (sock_fd, (struct sockaddr*)&cliaddr, &len);

	printf ("local ip is: %s, port is %d.\n", inet_ntop (AF_INET, (struct sockaddr *)&cliaddr.sin_addr,buf,128), ntohs(cliaddr.sin_port)); 
	getpeername (sock_fd, (struct sockaddr*)&servaddr, &len);
	printf ("peer ip is : %s, port is %d.\n", inet_ntop (AF_INET, (struct sockaddr *)&servaddr.sin_addr,buf, 128), ntohs (servaddr.sin_port));

	char *p_buf = read_buf;
	while ( (nread = read (sock_fd, p_buf, sizeof(read_buf))) > 0)
		p_buf += nread;

	printf ("time is: %s", read_buf);
	return 0;
}
