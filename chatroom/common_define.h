#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_

#define NAME_LEN 64
#define PW_LEN NAME_LEN 
#define OK 0
#define ERROR -1

#define MAX_MSG_LEN 1024

typedef enum msg_type
{
	MSG_REGISTER = 1,
	MSG_LOG_IN,
	MSG_FILE_SEND,
	MSG_LOG_OUT,
	MSG_DATA
} e_msg_type;

typedef struct msg_header
{
	e_msg_type m_type;
	int m_len;
	char chat_with[NAME_LEN]; // 表示消息发给那个客户或者发给全体客户端
	char file_name[NAME_LEN]; // 表示要发送文件的文件名
} msg_header_t;

typedef struct register_msg
{
	char name[NAME_LEN];
	char password[PW_LEN];
} register_msg_t;

// 保存用户名和用户连接的sock_fd

typedef struct user_info
{
	char name[NAME_LEN];
	char pw[PW_LEN];
	int conn_fd;
} user_info_t;

typedef struct node 
{
	char *msg;
	struct node *next;
} node_t;

typedef struct history_head
{
	char name[NAME_LEN];
	node_t *node;
} history_head_t;

#endif
