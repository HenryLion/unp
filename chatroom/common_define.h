#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_

typedef enum msg_type
{
	MSG_REGISTER,
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
	char name[64];
	char password[64];
} register_msg_t;




#endif
