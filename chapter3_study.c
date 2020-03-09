#include "unp.h"

// test inet_ntoa and inet_aton functions
void test_aton_ntoa ()
{
	struct in_addr addr_v4;
	bzero (&addr_v4, sizeof (addr_v4));

	inet_aton ("192.172.12.11", &addr_v4);
	printf ("result is: %d\n", (int)addr_v4.s_addr);

	printf ("%s\n", inet_ntoa (addr_v4));
}
//test inet_pton and inet_ntop functions
void test_pton_ntop ()
{
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons (233);
	inet_pton (AF_INET, "192.172.12.88", &serv.sin_addr); // == serv.sin_addr.s_addr = inet_aton ("192.172.12.11"); 
	char ip[INET_ADDRSTRLEN];
	inet_ntop (AF_INET, &serv.sin_addr, ip, sizeof(ip));
	printf ("%s\n", ip);
}

// readn function
ssize_t readn (int fd, void *buf, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char ptr = buf;

	while (nleft > 0)
	{
		if ( (nread = read (fd, ptr, nleft) ) < 0) // return < 0 means error occur;
		{
			if (errno = EINTR)  // if errno is EINTR we should read again;
				nread = 0;
			else
				return -1;  // other errno, return -1
		}
		else if (nread == 0)  // return 0 means EOF occur
			break;  // we break here, after we computer the real read bytes.

		nleft -= nread;
		ptr += nread;
	}

	return n - nleft;
}

ssize_t writen (int fd, const void *buf, size_t n)
{
	size_t nleft = n;
	const char *ptr = buf;
	ssize_t nwrite;

	while (nleft > 0)
	{
		if ( (nwrite = write (fd, ptr, nleft)) <= 0)
		{
			if (nwrite < 0 && errno == EINTR)
				nwrite = 0;
			else
				return -1;
		}

		nleft -= nwrite;
		ptr += nwrite;
	}

	return n-nleft;
}

int main (void)
{
	test_aton_ntoa ();
	test_pton_ntop ();
	return 0;
}
