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
	MSG_DATA,
	MSG_FILE_SEND,
	MSG_LOG_OUT
} e_msg_type;

typedef struct msg_header
{
	e_msg_type m_type;
	int m_len;
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


#endif
