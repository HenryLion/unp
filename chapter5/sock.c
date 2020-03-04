#include "sock.h"
#include <unistd.h>
#include <errno.h>

int readN (int fd, char *buf, int n)
{
	int n_left;
	n_left = n;
	int n_read;
	while (n_left > 0)
	{
		n_read = read (fd, buf, n_left);
	
		if (n_read > 0) // normal
		{
			buf += n_read;
			n_left -= n_read;
		}
		else if (n_read < 0)  // error
		{
			if (errno == EINTR)
				continue;
			else
				return -1;
		}
		else  // EOF
			break;
	}
	return (n - n_left);
}

int writeN (int fd, char *buf, int n)
{
	int n_left;
	n_left = n;
	int n_write;
	
	while (n_left > 0)
	{
		n_write = write (fd, buf, n_left);
	
		if (n_write > 0)
		{
			buf += n_write;
			n_left -= n_write;
		}
		else 
		{
			if (n_write < 0 && errno == EINTR)
				continue;
			else
				return -1;
		}
	}
	return (n);
}
