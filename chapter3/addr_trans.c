#include "unp.h"


/* this program use the API inet_aton and inet_ntoa */

// int inet_aton (const char *strptr, struct in_addr *addrptr);
// char *inet_ntoa (struct in_addr inaddr);


int main (void)
{
	struct in_addr inaddr;
	bzero (&inaddr, sizeof(struct in_addr));

	inet_aton ("192.172.11.42", &inaddr);
	printf ("%d\n", inaddr.s_addr);

	printf ("%s\n", inet_ntoa (inaddr));

	return 0;
}

