#include "unp.h"
#include "common_define.h"

#define CLIENT_MAX_NUM 1024
#define MAX_MSG_LEN 1024

int handle_client_msg (int cli_fd, int user_idx, user_info_t *user_infos, fd_set *rset)
{
	int nread;
	char *ptr;
	msg_header_t *p_head = NULL;
	char msg[MAX_MSG_LEN] = {0};
	char w_buf[MAX_MSG_LEN] = {0};
	if ( (nread = readn (cli_fd, msg, sizeof (msg_header_t)) == 0) )
	{
		// 处理客户端断连接消息,当一个客户端退出是其cli_fd变的可读,readn将返回0,此处需要将此客户端的信息从user_infos中删除
		close (cli_fd);
		FD_CLR (cli_fd, rset);
		user_infos[user_idx].conn_fd = -1;
		memset (user_infos[user_idx].name, 0, NAME_LEN);
		return 0;
	}
	p_head = (msg_header_t*)msg;
	ptr = msg + sizeof (msg_header_t);
	switch (p_head->m_type)
	{
		case MSG_REGISTER:
		{
			readn (cli_fd, ptr, ntohl(p_head->m_len));
			snprintf (w_buf, 1024, "hi, %s I received your register msg, your pw is %s.", ((register_msg_t*)ptr)->name, ((register_msg_t*)ptr)->password);
			memcpy (user_infos[user_idx].name, ((register_msg_t*)ptr)->name, NAME_LEN);
			writen (cli_fd, w_buf, strlen (w_buf));
			break;
		}
		default:
			printf ("default branch\n");
			break;
	}

	return 0;
}


int main (void)
{
	int listen_fd, conn_fd, sock_fd;
	int maxfd;
	user_info_t cli_infos[CLIENT_MAX_NUM];
	int i;
	int max_i = -1;
	int nready;

	struct sockaddr_in sockaddr;
	bzero (&sockaddr, sizeof (sockaddr));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons (10009);
	sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	listen_fd = Socket (AF_INET, SOCK_STREAM, 0);

	Bind (listen_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

	Listen (listen_fd, 5);

	memset (cli_infos, 0, sizeof (cli_infos));

	for (i = 0; i < CLIENT_MAX_NUM; ++i)
	{
		cli_infos[i].conn_fd = -1;
	}

	fd_set rset,allset;
	FD_ZERO (&allset);
	maxfd = listen_fd;
	FD_SET (listen_fd, &allset);

	while (1)
	{
		rset = allset;
		nready = Select (maxfd+1, &rset, NULL, NULL, NULL);
		if (FD_ISSET (listen_fd, &rset))
		{
			conn_fd = Accept (listen_fd, NULL, NULL);
			// 有新的客户连接则在cli_infos数组中找位置存放客户信息
			for (i = 0; i < CLIENT_MAX_NUM; ++i)
			{
				if (cli_infos[i].conn_fd == -1)
				{
					cli_infos[i].conn_fd = conn_fd;
					break;
				}
			}
			if (i == CLIENT_MAX_NUM)
			{
				printf ("too many clients\n");
				close (conn_fd);
				continue;
			}
			FD_SET (conn_fd, &allset);

			if (conn_fd > maxfd)
				maxfd = conn_fd;
			if (i > max_i)
				max_i = i;

			handle_client_msg (conn_fd, i, cli_infos, &allset);
			if (--nready <= 0)
				continue;
		}

		for (i = 0; i <= max_i; ++i)
		{
			if ( (sock_fd = cli_infos[i].conn_fd) < 0)
				continue;
			if ( FD_ISSET (sock_fd, &rset))
			{
				handle_client_msg (sock_fd, i, cli_infos, &allset);
			}

			if (--nready <= 0)
				break;
		}

	}
	return 0;
}
