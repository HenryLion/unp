#include "unp.h"
#include "common_define.h"

void print_prompt ()
{
	printf ("*****************************\n");
	printf ("[1] register\n");
	printf ("[2] log in\n");
	printf ("[3] chat\n");
	printf ("[4] file send\n");
	printf ("[5] log out\n");
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
}

int execute_chat_msg (FILE *fp, int sock_fd)
{
	char w_buf[MAX_MSG_LEN] = {0};
	char r_buf[MAX_MSG_LEN] = {0};
	fd_set f_set;
	FD_ZERO (&f_set);
	int select_fd;
	char* p_msg_body = NULL;
	msg_header_t *p_head = NULL;
	p_head = (msg_header_t*)w_buf;
	p_head->m_type = MSG_DATA;
	p_msg_body = w_buf + sizeof (msg_header_t);
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
			p_head->m_len = strlen (p_msg_body);
			printf ("write %d chs to server\n", (p_head->m_len));
			writen (sock_fd, w_buf, strlen (p_msg_body) + sizeof (msg_header_t));
			printf ("write to server finish, sock_fd = %d\n", sock_fd);
		}
		if (FD_ISSET (sock_fd, &f_set))
		{
			printf ("receive friend's prompt,sock_fd = %d\n", sock_fd);
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
				execute_reg_log_msg (sock_fd, MSG_REGISTER);
				break;
			case MSG_LOG_IN:
				execute_reg_log_msg (sock_fd, MSG_LOG_IN);
				break;
			case MSG_DATA:
				execute_chat_msg (stdin,sock_fd);
				break;
			default:
				break;
		}
	}

	return 0;
}
