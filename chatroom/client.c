#include "unp.h"
#include "common_define.h"

int execute_chat_msg (FILE *, int);

void print_prompt ()
{
	printf ("*****************************\n");
	printf ("[1] register\n");
	printf ("[2] log in\n");
	printf ("[3] file send\n");
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
	msg_header_t *p_head = (msg_header_t *)msg;
	p_head->m_type = m_type;
	p_head->m_len = htonl (sizeof (r_msg));
	
	ptr = msg + sizeof (msg_header_t);
	memcpy (ptr, &r_msg, sizeof (r_msg));

	Writen (sock_fd, msg, sizeof(msg_header_t) + sizeof(r_msg));

	read (sock_fd, r_buf, 1024);
	printf ("%s\n", r_buf);

	if (m_type == MSG_LOG_IN)
	{
		execute_chat_msg (stdin, sock_fd);
	}
	
}

/* 获取聊天的对象名,当用户输入开头是[xxx]表示用户切换聊天对象为xxx */
/* input: 表示用户的输入 [all_client] 表示发给所有人, [name] 表示只发给name */
/* name: 从用户的输入中获取的消息发送对象 */
/* real_msg_pos: 如果有[xxx]的话，real_msg_pos 返回消息开始的真正位置 */
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
			return 0; // 这是个普通消息，不用转变聊天对象
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

int execute_chat_msg (FILE *fp, int sock_fd)
{
	char w_buf[MAX_MSG_LEN] = {0};
	char r_buf[MAX_MSG_LEN] = {0};
	char chat_with [NAME_LEN] = {"alluser"};
	char new_chat_with[NAME_LEN] = {0};
	fd_set f_set;
	FD_ZERO (&f_set);
	int select_fd;
	char* p_msg_body = NULL;
	msg_header_t *p_head = NULL;
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
				real_msg_len = strlen(p_msg_body) - real_msg_pos;
				memcpy (p_msg_body, p_msg_body+real_msg_pos, real_msg_len);
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
			memset (r_buf, 0, sizeof(r_buf));
			read (sock_fd, r_buf, 1024);
			fputs (r_buf, stdout);
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
			default:
				break;
		}
	}

	return 0;
}
