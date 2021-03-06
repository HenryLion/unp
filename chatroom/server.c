#include "unp.h"
#include "common_define.h"

#define CLIENT_MAX_NUM 1024

history_head_t cli_history[CLIENT_MAX_NUM]; //用户离线消息链表

/*********************************************************
* function: find client info in user_infos array
* user_infos: where user info stored
* cur_max_cli_num: the max num of client has beed stored
* c_name: client name
* return: pos of client or -1 when client not exist
*********************************************************/
int is_client_exist (user_info_t *user_infos, int cur_max_cli_num, char *c_name)
{
	if (NULL == user_infos || NULL == c_name)
	{
		printf ("param error NULL pointer\n");
		exit(-1);
	}
	int i;
	for (i = 0; i <= cur_max_cli_num; ++i)
	{
		if (!strcmp(user_infos[i].name, c_name))
			break;
	}
	if (i > cur_max_cli_num)
		return -1;
	else
		return i;
}

/****************************************************
* function: determine whether client has history msg when client log in
* client_name: client name
* author: herbert
* date: 2020-04-02
* return: -1 means no history msg, >=0 means history msg store array pos
****************************************************/
int is_client_has_history_msg (char *client_name)
{
	if (NULL == client_name)
		return -1;

	int i;
	for (i = 0; i < CLIENT_MAX_NUM; ++i)
	{
		if ( !strcmp (client_name, cli_history[i].name) )
			break;
	}

	if (i < CLIENT_MAX_NUM && cli_history[i].node != NULL)
	{
		return i;
	}
	else
		return -1;
}

/**********************************************
* function: delete client history message when send all the client's history msg
* head: client's history msg linked list
* author: herbert
* date : 2020-04-02
*********************************************/
int delete_client_history_msg (node_t *head)
{
	node_t *p = head;
	while (p)
	{
		head = p->next;
		free (p->msg);
		p->next = NULL;
		free (p);
		p = head;
	}
	return 0;
}


/************************************************
* function: send client's history message
* sock_fd: the client's socket id
* head: the client's history message linked list head
* Herbert 2020-04-02
***********************************************/
int send_client_history_msg (int sock_fd, node_t* head)
{
	node_t *p = head;
	while (p)
	{
		if (NULL != p->msg)
			write (sock_fd, p->msg, strlen (p->msg));
		p = p->next;
	}
	
	delete_client_history_msg (head); //发送完后就将用户的历史消息链表清空

	return 0;
}

/*******************************************
* function: add client's msg to his history msg list when client is offline
* client_name: client's name
* content: message content
* Herbert 2020-04-02
******************************************/
int add_client_history_msg (char *client_name, char *content)
{
	if (NULL == client_name)
		return -1;
	int i;
	node_t *p = NULL;
	node_t *new_node = NULL;
	int first_empty_pos = -1;

	new_node = (node_t *)malloc (sizeof (node_t));
	new_node->msg = (char *)malloc (strlen (content) + 1);
	memcpy (new_node->msg, content, strlen(content));
	new_node->next = NULL;
	
	for (i = 0; i < CLIENT_MAX_NUM; ++i)
	{
		if ( (strlen (cli_history[i].name) == 0) && (first_empty_pos == -1) )
			first_empty_pos = i; //找到第一个空位

		if (!strcmp (client_name, cli_history[i].name))
		{
			p = cli_history[i].node;
			while (p->next) // 让p指向链表尾端
			{
				p = p->next;
			}
			p->next = new_node;
			first_empty_pos = -1;
			break;
		}
	}
	
	if (-1 != first_empty_pos)
	{
		strcpy (cli_history[first_empty_pos].name, client_name);
		cli_history[first_empty_pos].node = new_node;
	}

	return 0;
}

/***********************************************************
* funciont: deal with client's log in message
* cli_fd: client's socket id;  user_idx:pos of client in usre_infos array; p_head: message header; 
* user_infos: store clients's name and pw array; cur_max_cli_num: current max clients number
* Herbert 2020-04-02
***********************************************************/
int handle_login_msg (int cli_fd, int user_idx, msg_header_t *p_head, user_info_t *user_infos, int cur_max_cli_num)
{
	char r_buf[MAX_MSG_LEN] = {0};
	char w_buf[MAX_MSG_LEN] = {0};
	register_msg_t *p_msg = NULL;
	int i;
	int history_idx = 0;
	if (NULL == p_head || NULL == user_infos)
		return ERROR;
	readn (cli_fd, r_buf, ntohl(p_head->m_len));
	p_msg = (register_msg_t *)r_buf;
	for (i = 0; i <= cur_max_cli_num; ++i)
	{
		if ( !strcmp (user_infos[i].name, p_msg->name) && !strcmp (user_infos[i].pw, p_msg->password) )
			break;
	}
	if (i > cur_max_cli_num)
	{
		snprintf (w_buf, MAX_MSG_LEN, "[L]No client named %s, please register first\n", p_msg->name);
	}
	else
	{
		user_infos[user_idx].conn_fd = -1;
		user_infos[i].conn_fd = cli_fd;
		snprintf (w_buf, MAX_MSG_LEN, "[L]Hi %s, you log in success, now you can chat with your friends\n", p_msg->name);

		// 登录成功后，查看有没有此用户的离线消息需要发送
		history_idx = is_client_has_history_msg (p_msg->name);
		if ( history_idx  >= 0)
		{
			printf ("will send offline msg to %s.\n", p_msg->name);
			send_client_history_msg (cli_fd, cli_history[history_idx].node);
			memset (cli_history[history_idx].name, 0, sizeof (cli_history[history_idx].name));
		}
	}
	writen (cli_fd, w_buf, strlen (w_buf));
	return 0;
}


/***********************************************************
* funciont: deal with client's chat data message
* cli_fd: client's socket id;  p_head: message header; 
* user_infos: store clients's name and pw array; cur_max_cli_num: current max clients number
* Herbert 2020-04-02
***********************************************************/
int handle_data_msg (int cli_fd, msg_header_t* p_head, user_info_t *user_infos, int cur_max_cli_num)
{
	char r_buf[MAX_MSG_LEN] = {0};
	char w_buf[MAX_MSG_LEN] = {0};
	char client_name[NAME_LEN] = {0};

	if (NULL == p_head || NULL == user_infos)
		return ERROR;

	int i;
	for (i = 0; i <= cur_max_cli_num; ++i)
	{
		if (user_infos[i].conn_fd == cli_fd)
		{
			memcpy (client_name, user_infos[i].name, strlen (user_infos[i].name));
			break;
		}
	}

	readn (cli_fd, r_buf, ntohl (p_head->m_len));
	snprintf (w_buf, MAX_MSG_LEN, "[D]%s say :\n\t%s", client_name, r_buf);
	if ( !strcmp (p_head->chat_with, "alluser") ) // send msg to all clients
	{
		for (i = 0; i <= cur_max_cli_num; ++i)
		{
			if ( (user_infos[i].conn_fd != -1) && (user_infos[i].conn_fd != cli_fd) )
			{
				writen (user_infos[i].conn_fd, w_buf, strlen(w_buf));
			}
		}
	}
	else  // send msg to specific clients
	{
		for ( i = 0; i <= cur_max_cli_num; ++i )
		{
			if (user_infos[i].conn_fd == cli_fd)
				continue;
			if ( !strcmp (p_head->chat_with, user_infos[i].name))
			{
				if (user_infos[i].conn_fd == -1) // 如果用户不在线，则将消息存放在离线消息链表
				{
					add_client_history_msg (p_head->chat_with, w_buf);
				}
				else // 用户在线则直接将消息发送给用户
				{
					writen (user_infos[i].conn_fd, w_buf, strlen(w_buf));
				}
			}
		}
	}
	return 0;
}


/***********************************************************
* funciont: deal with client's file send  message
* cli_fd: client's socket id;  p_head: message header; 
* user_infos: store clients's name and pw array; cur_max_cli_num: current max clients number
* Herbert 2020-04-02
**********************************************************/
int handle_file_send_msg (int cli_fd, msg_header_t* p_head, user_info_t *user_infos, int cur_max_cli_num)
{
	char client_name[NAME_LEN] = {0};
	char w_buf [MAX_MSG_LEN] = {0};

	int fd = 0;
	if (NULL == p_head || NULL == user_infos)
		return ERROR;

	/*   | [X] | header | data | */   // 写给client端的结构 [X]表示消息类型
	char *r_buf = (char*)malloc (ntohl (p_head->m_len) + sizeof (msg_header_t) + 1 + 3); // 多3个字节用来给客户端反馈此消息类型
	if (NULL == r_buf)
		return -1;
	

	int i;
	for (i = 0; i <= cur_max_cli_num; ++i)
	{
		if (user_infos[i].conn_fd == cli_fd)
		{
			memcpy (client_name, user_infos[i].name, strlen (user_infos[i].name));
			break;
		}
	}

	memcpy (r_buf, "[F]", 3);
	memcpy (r_buf+3, p_head, sizeof (msg_header_t));
	readn (cli_fd, r_buf+sizeof(msg_header_t)+3, ntohl (p_head->m_len));

	snprintf (w_buf, MAX_MSG_LEN, " %s send you file: %s. \n", client_name, p_head->file_name);

	if ( !strcmp (p_head->chat_with, "alluser") ) // send file to all clients
	{
		for (i = 0; i <= cur_max_cli_num; ++i)
		{
			if ( (user_infos[i].conn_fd != -1) && (user_infos[i].conn_fd != cli_fd) )
			{
				writen (user_infos[i].conn_fd, r_buf, ntohl(p_head->m_len) + sizeof (msg_header_t)+3);
				writen (user_infos[i].conn_fd, w_buf, strlen(w_buf));
			}
		}
	}
	else  // send file to specific clients
	{
		for ( i = 0; i <= cur_max_cli_num; ++i )
		{
			if (user_infos[i].conn_fd == cli_fd)
				continue;
			if ( !strcmp (p_head->chat_with, user_infos[i].name))
			{
				writen (user_infos[i].conn_fd, r_buf, ntohl (p_head->m_len) + sizeof (msg_header_t) + 3 );
				writen (user_infos[i].conn_fd, w_buf, strlen(w_buf));
			}
		}
	}
	return 0;

}


/***********************************************************
* funciont: handle client message interface
* cli_fd: client's socket id; user_idx: pos of client in cli_infos array; rset: listened fd set; fp: the file to store user_infos
* user_infos: store clients's name and pw array; cur_max_cli_num: current max clients number
* Herbert 2020-04-02
*********************************************************/
int handle_client_msg (int cli_fd, int user_idx, user_info_t *user_infos, fd_set *rset, int cur_max_cli_num, FILE *fp)
{
	int nread;
	char *ptr;
	msg_header_t *p_head = NULL;
	char msg[MAX_MSG_LEN] = {0};
	char w_buf[MAX_MSG_LEN] = {0};
	int idx;
	if ( (nread = readn (cli_fd, msg, sizeof (msg_header_t)) == 0) )
	{
		// 处理客户端断连接消息,当一个客户端退出是其cli_fd变的可读,readn将返回0,此处需要将此客户端的信息从user_infos中删除
		close (cli_fd);
		FD_CLR (cli_fd, rset);
		user_infos[user_idx].conn_fd = -1;
		// memset (user_infos[user_idx].name, 0, NAME_LEN); // 用户名不进行清零以便在登录的时候判断用户是否已经注册
		return 0;
	}
	p_head = (msg_header_t*)msg;
	ptr = msg + sizeof (msg_header_t);
	switch (p_head->m_type)
	{
		case MSG_REGISTER:
		{
			readn (cli_fd, ptr, ntohl(p_head->m_len));
			if ( (idx = is_client_exist (user_infos, cur_max_cli_num, ((register_msg_t*)ptr)->name)) < 0 )
			{
				// 用户不存在,第一次注册
				memcpy (user_infos[user_idx].name, ((register_msg_t*)ptr)->name, NAME_LEN);
				memcpy (user_infos[user_idx].pw, ((register_msg_t*)ptr)->password, PW_LEN);
				snprintf (w_buf, MAX_MSG_LEN, "%s\t%s\n", user_infos[user_idx].name, user_infos[user_idx].pw);
				fputs (w_buf, fp);
				fflush (fp);
				memset (w_buf, 0, sizeof (w_buf));
				snprintf (w_buf, MAX_MSG_LEN, "[R]Hi %s, you register successful, user_idx = %d.", ((register_msg_t*)ptr)->name, user_idx);
			}
			else
			{
				// 用户已经存在
				// user_infos[user_idx].conn_fd = -1; // 不能将conn_fd赋值-1,否则连接的客户将不能再发送请求消息
				if (user_infos[idx].conn_fd == -1) // 用户已经存在但是未登录,如果用户已经存在且已经登录，则不能把cli_fd赋值给已存在用户
					user_infos[idx].conn_fd = cli_fd;

				snprintf (w_buf, MAX_MSG_LEN, "[R]Name %s has existed,please choose another one.", ((register_msg_t*)ptr)->name); 
			}
			writen (cli_fd, w_buf, strlen (w_buf));
			break;
		}
		case MSG_LOG_IN:
		{
			handle_login_msg (cli_fd, user_idx, p_head, user_infos, cur_max_cli_num);
			break;	
		}
		case MSG_DATA:
		{
			handle_data_msg (cli_fd, p_head, user_infos, cur_max_cli_num);	
			break;
		}
		case MSG_FILE_SEND:
		{
			handle_file_send_msg (cli_fd, p_head, user_infos, cur_max_cli_num);
			break;
		}
		default:
			printf ("default branch\n");
			break;
	}

	return 0;
}

int is_file_exist (const char* file_name)
{
	if (access (file_name, F_OK) < 0)
		return 0;
	return 1;
}

/***********************************************************
* funciont: get client's info from file (client.info)
* file_name: the file which store user's info 
* cli_infos: the array to store client's info
* Herbert 2020-04-02
***********************************************************/
int get_client_info (const char* file_name,user_info_t *cli_infos)
{
	char cli_info[256] = {0};
	int i = 0;
	struct stat buf;
	if (stat (file_name, &buf) < 0)
	{
		return 0;
	}
	if (buf.st_size == 0) /* 如果用户文件为空 */
		return 0;

	FILE *fp = fopen (file_name, "a+");

	while (fgets (cli_info, 256, fp) != NULL)
	{
		sscanf (cli_info, "%s%s", cli_infos[i].name,cli_infos[i].pw);
		++i;
	}

	return i;
	fclose (fp);
}


int main (void)
{
	int listen_fd, conn_fd, sock_fd;
	int maxfd;
	user_info_t cli_infos[CLIENT_MAX_NUM];
	int i;
	int max_i = -1;
	int nready;
	FILE *c_fp = NULL;
	
	struct sockaddr_in sockaddr;
	bzero (&sockaddr, sizeof (sockaddr));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons (10009);
	sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	listen_fd = Socket (AF_INET, SOCK_STREAM, 0);
	int on = 1;
	setsockopt (listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind (listen_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

	Listen (listen_fd, 5);

	memset (cli_infos, 0, sizeof (cli_infos));
	memset (cli_history, 0, sizeof (cli_history));

	if (is_file_exist ("./client.info"))
		max_i = get_client_info ("./client.info", cli_infos);

	for (i = 0; i < CLIENT_MAX_NUM; ++i)
	{
		cli_infos[i].conn_fd = -1;
	}

	c_fp = fopen ("./client.info", "a+"); // 此处打开文件是为了有新用户注册时写入用户信息
	if (NULL == c_fp)
	{
		printf ("fopen client.info error\n");
		exit (-1);
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
			// 有新的客户连接则在cli_infos数组中找位置存放客户信息,但是后续不一定就存储在此位置，因为用户可能之前已经存在
			for (i = 0; i < CLIENT_MAX_NUM; ++i)
			{
				if ( (cli_infos[i].conn_fd == -1) && (strlen (cli_infos[i].name) == 0) )
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
			if (i >= max_i)
				max_i = i;

			if (--nready <= 0)
				continue;
		}
		for (i = 0; i <= max_i; ++i)
		{
			if ( (sock_fd = cli_infos[i].conn_fd) < 0)
				continue;
			if ( FD_ISSET (sock_fd, &rset))
			{

				handle_client_msg (sock_fd, i, cli_infos, &allset, max_i, c_fp);
				if (--nready <= 0)
					break;
			}

		}

	}
	return 0;
}
