#include "unp.h"
#include <stdio.h>

void test_str_copy (char *str)
{
	int len = strlen (str);
	memcpy (str, str+1, len-1); // memcpy 拷贝时内存地址不能overlap,否则需要使用memmove
	str[len-1] = '\0';
	printf ("%s\n", str);
	return;
}

void test_scanf ()
{
	int choice;

	char name[64] = {0};
	char pw[64] = {0};

	printf ("please input your choice: ");
	scanf ("%d", &choice);
	getchar ();
	switch (choice)
	{
		case 1:
		{
			printf ("please input your name: ");
			scanf ("%s", name);
			getchar ();
			printf ("please input your pw: ");
			scanf ("%s", pw);
			getchar ();
			break;
		}
		default:
			break;
	}

	printf ("your name is: %s, pw is %s\n", name, pw);

	return ;
}

void test_read_file (char *file_name)
{
	if (NULL == file_name)
		return;
	
	if (access (file_name, F_OK) < 0)
		return;
	int fd;
	struct stat buf;

	if (stat (file_name, &buf) < 0)
		return;
	char *p_file = (char *)malloc (buf.st_size + 1);
	
	 if ( (fd = open (file_name, O_RDONLY, S_IRUSR|S_IRGRP) ) < 0)
		 return ;

	readn (fd, p_file, buf.st_size);
	p_file[buf.st_size] = '\0';

	fputs (p_file, stdout);
	fflush (stdout);

	printf ("file size is: %ld\n", buf.st_size);

	free (p_file);
	return;
}
	

int main (void)
{
	//test_scanf ();
	//test_read_file ("./test.c");
	char str[64] = "hanbin";
	test_str_copy (str);
	return 0;
}
