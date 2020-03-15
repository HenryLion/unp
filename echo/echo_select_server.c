#include "unp.h"

// 本版本的服务端程序利用select函数实现单进程服务多个客户端

int main (void)
{
	int sockfd, connfd;
	struct sockaddr_in serv_addr;

	int client_fd[1024]; // 用来存放和客户端建立的sock连接描述符
	int i, nready, nread;

	int maxfd;
	int maxi = -1;
	char r_buf[256], w_buf[256];
	bzero (&serv_addr, sizeof (serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (34567);
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);

	sockfd = Socket (AF_INET, SOCK_STREAM,0);
	
	Bind (sockfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr));

	Listen (sockfd, 5);

	// 初始化连接描述符数组全为无效值-1
	for (i = 0; i < 1024; ++i)
	{
		client_fd[i] = -1;
	}

	fd_set set, op_set;
	FD_ZERO (&op_set);

	FD_SET (sockfd, &op_set);
	maxfd = sockfd;

	while (1)
	{
		set = op_set; // 考虑到在select监听时，在FD_ISSET分支不应修改set的值，从而需要一个中间变量
		nready = Select (maxfd+1, &set, NULL, NULL, NULL);

		// 如果在监听描述符可读，说明有新的客户端连接过来
		if (FD_ISSET (sockfd, &set))
		{
			connfd = Accept (sockfd, NULL, NULL);
			if (connfd > maxfd)
				maxfd = connfd;
			FD_SET (connfd, &op_set); // 将新的连接加入监听
			for (i = 0; i < 1024; ++i)
			{
				if (client_fd[i] == -1)
				{
					client_fd[i] = connfd;
					if (i > maxi)
						maxi = i;
					break;
				}
			}
			if (i == 1024)
			{
				printf ("too many clients\n");
				close (connfd);
			}
			if (--nready <= 0) // 走到这里说明select只检测到了sockfd可读
				continue;
		}

		// select 返回后遍历所有的连接描述符，判断哪些是有数据发送过来的
		for (i = 0; i <= maxi; ++i)
		{
			if (client_fd[i] < 0)
				continue;
			if (FD_ISSET (client_fd[i], &set))
			{
				if ( (nread = read (client_fd[i], r_buf, 256)) == 0)
				{
					close (client_fd[i]);
					FD_CLR(client_fd[i], &op_set);
					client_fd[i] = -1;
				}
				else
				{
					writen (client_fd[i], r_buf, nread);
				}

				if (--nready <= 0)
					break;
			}
		}
		
	}
}



