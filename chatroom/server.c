#include "unp.h"
#include "common_define.h"

int main (void)
{
	int listen_fd, conn_fd;
	char msg[1024] = {0};
	char w_buf[1024] = {0};
	msg_header_t *p_head = NULL;
	char *ptr = msg;

	register_msg_t *p_msg = NULL;

	struct sockaddr_in sockaddr;
	bzero (&sockaddr, sizeof (sockaddr));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons (10009);
	sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	listen_fd = Socket (AF_INET, SOCK_STREAM, 0);

	Bind (listen_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

	Listen (listen_fd, 5);

	conn_fd = Accept (listen_fd, NULL, NULL);

	readn (conn_fd, msg, sizeof (msg_header_t));
	printf ("read from client...\n");

	p_head = (msg_header_t*)msg;

	ptr = msg + sizeof (msg_header_t);

	switch (p_head->m_type)
	{
		case MSG_REGISTER:
		{
			readn (conn_fd, ptr, p_head->m_len);
			snprintf (w_buf, 1024, "hi, %s I received your register msg", ((register_msg_t*)ptr)->name);
			writen (conn_fd, w_buf, strlen (w_buf));
		}
		default:
			printf ("default branch\n");
			break;
	}

	return 0;
}
