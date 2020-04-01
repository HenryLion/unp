#include "unp.h"
#include "common_define.h"


char g_client_name[NAME_LEN] = {0};
int execute_chat_msg (FILE *, int);

void print_prompt ()
{
	printf ("*****************************\n");
	printf ("[1] register\n");
	printf ("[2] log in\n");
	printf ("[3] [sendfile]\n");
	printf ("[4] log out\n");
	printf ("*****************************\n");
	printf ("please choose operation: ");
}

int execute_reg_log_msg (int sock_fd, e_msg_type m_type)
{
	char msg[MAX_MSG_LEN] = {0};
	char r_buf[MAX_MSG_LEN] = {0};
	char *ptr = NULL;
	register_msg_t r_msg;
	bzero (&r_msg, sizeof (r_msg));
	printf ("please input your name: ");
	scanf ("%s", r_msg.name);
	getchar ();
	printf ("please input your pw: ");
	scanf ("%s", r_msg.password);
	getchar ();

	memcpy (g_client_name, r_msg.name, strlen (r_msg.name));
	msg_header_t *p_head = (msg_header_t *)msg;
	p_head->m_type = m_type;
	p_head->m_len = htonl (sizeof (r_msg));
	
	ptr = msg + sizeof (msg_header_t);
	memcpy (ptr, &r_msg, sizeof (r_msg));

	Writen (sock_fd, msg, sizeof(msg_header_t) + sizeof(r_msg));

	readn (sock_fd, r_buf, 3);
	memset (r_buf, 0, sizeof(r_buf));
	read (sock_fd, r_buf, 1024);
	printf ("%s\n", r_buf);

	if (m_type == MSG_LOG_IN)
	{
		execute_chat_msg (stdin, sock_fd);
	}
	
}

/*  判断文件是否存在，如果存在则file_len返回文件的长度 */
int is_file_exist (char *file_name, off_t *file_len)
{
	if (NULL == file_name)
		return 0;
	if (access (file_name, F_OK) < 0)
		return 0;
	struct stat buf;
	if (stat (file_name, &buf) < 0)
		return 0;
	*file_len = buf.st_size;

	return 1;
}


int execute_file_send_msg (int sock_fd)
{
	char file_name[NAME_LEN] = {0};
	char chat_name[NAME_LEN] = {0};
	char r_buf[MAX_MSG_LEN] = {0};
	printf ("File you want to send: ");
	scanf ("%s", file_name);
	getchar ();
	printf ("Send to who: ");
	scanf ("%s", chat_name);
	getchar ();

	int fd;
	off_t f_len = 0;
	if (!is_file_exist (file_name, &f_len))
	{
		printf ("file_name %s not exist.\n", file_name);
		return -1;
	}
	char *w_buf = (char *)malloc (sizeof (msg_header_t) + f_len + 1);
	if (NULL == w_buf)
	{
		printf ("malloc return NULL!\n");
		return -1;
	}
	memset (w_buf, 0, sizeof (msg_header_t) + f_len + 1);
	msg_header_t *p_head = (msg_header_t*)w_buf;
	p_head->m_type = MSG_FILE_SEND;
	p_head->m_len = htonl (f_len);
	memcpy (p_head->chat_with, chat_name, strlen (chat_name));
	memcpy (p_head->file_name, file_name, strlen (file_name));

	if ( (fd = open (file_name, O_RDONLY, S_IRUSR|S_IRGRP)) < 0)
	{
		printf ("open send file error\n");
		return -1;
	}

	readn (fd, w_buf+sizeof(msg_header_t), f_len);

	writen (sock_fd, w_buf, sizeof(msg_header_t)+f_len);

	free (w_buf);
	close (fd);

	execute_chat_msg (stdin, sock_fd); // 发送完文件后还是回到聊天状态
	return 0;
}

/* 获取聊天的对象名,当用户输入开头是[xxx]表示用户切换聊天对象为xxx */
/* input: 表示用户的输入 [alluser] 表示发给所有人, [hanbin] 表示只发给hanbin */
/* name: 从用户的输入中获取的消息发送对象 */
/* real_msg_pos: 如果有[xxx]的话，real_msg_pos 返回消息开始的真正位置 */
/* 注: input 为[sendfile]时表示特殊含义，指用户想要发送文件,所以这就要求用户名不能为“sendfile” */
int get_chat_client (char *input, char *c_name, int *real_msg_pos)
{
	if (NULL == input || NULL == c_name || NULL == real_msg_pos)
	{
		printf ("get_chat_client: param error\n");
		return -1;
	}
	int cnt = 0;
	char *p = input;
	int record_name = 0;
	int get_finish = 0;
	while (*p != '\0')
	{
		*real_msg_pos += 1;
		if (record_name)
		{
			if (']' == *p)
			{
				get_finish = 1;
				break;
			}
			c_name[cnt++] = *p;
			if (cnt == NAME_LEN)
			{
				printf ("Too long chat name.\n");
				return -1;
			}
			p++;
			continue;
		}
		if ('\t' == *p || ' ' == *p)
		{
			p++;
			continue;
		}
		if ('[' != *p)
		{
			*real_msg_pos = 0;
			return 0; // 这是个普通消息，不用转变聊天对象
		}
		else
		{
			record_name = 1;
			p++;
			continue;
		}
	}
	
	c_name[cnt] = '\0';

	if (!get_finish) // 没有出现结束标志']' 则将c_name 清空
	{
		memset (c_name, 0, NAME_LEN);
		real_msg_pos = 0;
	}

	return 0;
}

int get_msg_and_create_file (int sock_fd, msg_header_t *msg_head)
{
	int fd;
	if (NULL == msg_head)
		return -1;
	int file_len = ntohl (msg_head->m_len);
	char file_name[NAME_LEN] = {0};
	memcpy (file_name, g_client_name, strlen (g_client_name));
	strncat (file_name, "_", 1);
	strncat (file_name, msg_head->file_name, strlen (msg_head->file_name));

	char *p_file = (char *)malloc (file_len + 1);
	if (NULL == p_file)
		return -1;
	
	fd = open (file_name, O_RDWR|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP);
	if (fd < 0)
		return -1;
	
	readn (sock_fd, p_file, file_len);

	writen (fd, p_file, file_len);

	free (p_file);
	close (fd);
}	

int execute_chat_msg (FILE *fp, int sock_fd)
{
	char w_buf[MAX_MSG_LEN] = {0};
	char r_buf[MAX_MSG_LEN] = {0};
	char msg_flag[64] = {0};
	char chat_with [NAME_LEN] = {"alluser"};
	char new_chat_with[NAME_LEN] = {0};
	fd_set f_set;
	FD_ZERO (&f_set);
	int select_fd;
	char* p_msg_body = NULL;
	msg_header_t *p_head = NULL;
	msg_header_t msg_head;
	bzero (&msg_head, sizeof(msg_head));
	p_head = (msg_header_t*)w_buf;
	p_head->m_type = MSG_DATA;
	p_msg_body = w_buf + sizeof (msg_header_t);
	int real_msg_pos = 0;
	int real_msg_len = 0;
	while (1)
	{
		FD_ZERO (&f_set);
		FD_SET (fileno (fp),&f_set);
		FD_SET (sock_fd, &f_set);
		select_fd = max (fileno(fp),sock_fd) + 1;

		Select (select_fd, &f_set, NULL, NULL, NULL);
		if (FD_ISSET (fileno(fp), &f_set))
		{
			if (fgets (p_msg_body, MAX_MSG_LEN-sizeof(msg_header_t), fp) == NULL)
				return -1;
			
			real_msg_pos = 0;
			get_chat_client (p_msg_body, new_chat_with, &real_msg_pos);
			if (strlen (new_chat_with) != 0)
			{
				if ( !strcmp (new_chat_with, "sendfile") ) // 用户想发送文件,则跳出循环让用户进入处理MSG_FILE_SEND模式
					break;
				real_msg_len = strlen(p_msg_body) - real_msg_pos;
				memmove (p_msg_body, p_msg_body+real_msg_pos, real_msg_len);
				p_msg_body[real_msg_len] = '\0';
				memcpy (chat_with, new_chat_with, strlen (new_chat_with));
				chat_with[strlen(new_chat_with)] = '\0';
			}
			
			memcpy (p_head->chat_with, chat_with, strlen(chat_with));
			p_head->chat_with[strlen(chat_with)] = '\0';

			p_head->m_len = htonl(strlen (p_msg_body));
			writen (sock_fd, w_buf, strlen (p_msg_body) + sizeof (msg_header_t));
		}
		if (FD_ISSET (sock_fd, &f_set))
		{
			memset (msg_flag, 0, sizeof (msg_flag));
			readn (sock_fd, msg_flag, 3); // 先读取3个字节的消息类型标记
			if ('L' != msg_flag[1] && 'R' != msg_flag[1] && 'D' != msg_flag[1] && 'F' != msg_flag[1])
			{
				printf ("%s\n", msg_flag);
				printf ("server send invalid msg content!\n");
			}

			if ('F' != msg_flag[1]) // 不是文件传输消息则直接输出
			{
				memset (r_buf, 0, sizeof(r_buf));
				read (sock_fd, r_buf, 1024);
				fputs (r_buf, stdout);
			}
			else // 文件传输消息
			{
				readn (sock_fd, &msg_head, sizeof (msg_head));
				get_msg_and_create_file (sock_fd, &msg_head);
				memset (r_buf, 0, sizeof(r_buf));
				read (sock_fd, r_buf, 1024);
				fputs (r_buf, stdout);
			}
		}
	}

	return 0;	
}

int main (int argc, char *argv[])
{
	if (2 != argc)
	{
		printf ("usage: a.out <IPaddress>\n");
		return -1;
	}
	int sock_fd;
	struct sockaddr_in servaddr;

	int choice;

	bzero (&servaddr, sizeof (servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (10009);
	inet_pton (AF_INET, argv[1], &(servaddr.sin_addr));

	sock_fd = Socket (AF_INET, SOCK_STREAM, 0);

	printf ("client sock_fd = %d\n", sock_fd);

	connect (sock_fd, (struct sockaddr *)&servaddr, sizeof (servaddr));

	while (1)
	{
		print_prompt ();

		scanf ("%d", &choice);
		getchar();
		switch ((e_msg_type)choice)
		{
			case MSG_REGISTER:
			case MSG_LOG_IN:
				execute_reg_log_msg (sock_fd, (e_msg_type)choice);
				break;
			case MSG_FILE_SEND:
				execute_file_send_msg (sock_fd);
				break;
			default:
				break;
		}
	}

	return 0;
}
