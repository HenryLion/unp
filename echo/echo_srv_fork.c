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

// 关注一下waitpid的返回值,根据这三个返回值就可以解释为何下面的sig_chld函数可以等待所有的子进程结束.

// > 0: the process ID of the child whose state has changed
// = 0: if WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state
// < 0: error

// 假设一个父进程fork了5个子进程:
// 其中一个子进程结束,则此子进程给父进程发送SIGCHLD信号，父进程调用sig_chld处理函数,waitpid 返回>0,再次循环调用waitpid因为此时其他进程无状态变化所有返回0,sig_chld函数退出。
// 又有3个进程同时退出,假设由于linux的信号递送机制，只有1个信号递送给父进程,父进程调用sig_chld处理函数,waitpid 返回>0,再次循环调用waitpid处理完其他两个退出的进程后返回0,sig_chld退出


void sig_chld (int sig)
{
	pid_t pid;
	int stat;
	while ( (pid = waitpid (-1,&stat,WNOHANG)) > 0)
		printf ("child %d terminated\n", pid);
	return;
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

	// 添加SIGCHID信号的处理
	signal (SIGCHLD, sig_chld);

	while (1)
	{
		ad_len = sizeof (cli_addr);
		if ( ( conn_fd = accept (listen_fd, (struct sockaddr *)&cli_addr, &ad_len) ) < 0)
		{
			//为了防止父进程在accept时被信号打断(如SIGCHID信号),需要判断错误码，如果是被信号中断,则重新accept
			if (errno == EINTR)
				continue;
			else
				err_sys ("conect error");
		}
		

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

