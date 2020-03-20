#include "unp.h"
#include "common_define.h"


void print_prompt ()
{
	printf ("*****************************\n");
	printf ("please choose operation:\n");
	printf ("[1] register\n");
	printf ("[2] log in\n");
	printf ("[3] file send\n");
	printf ("[4] log out\n");
	printf ("*****************************\n");
}

int execute_register_msg (int sock_fd)
{
	char msg[1024] = {0};
	char r_buf[1024] = {0};
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
	p_head->m_type = MSG_REGISTER;
	p_head->m_len = htonl (sizeof (r_msg));
	

	ptr = msg + sizeof (msg_header_t);
	memcpy (ptr, &r_msg, sizeof (r_msg));

	Writen (sock_fd, msg, sizeof(msg_header_t) + sizeof(r_msg));

	read (sock_fd, r_buf, 1024);
	printf ("%s\n", r_buf);
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

	connect (sock_fd, (struct sockaddr *)&servaddr, sizeof (servaddr));

	print_prompt ();

	scanf ("%d", &choice);
	getchar();
	switch (choice)
	{
		case 1:
			execute_register_msg (sock_fd);					
		default:
			break;
	}

	return 0;
}
