#include "unp.h"

int print_val ();

int main (int argc, char **argv)
{
	union {
		short s;
		char c[sizeof(short)];
	} un;

	un.s = 0x0102;
	printf ("%s ", CPU_VENDOR_OS);

	if (sizeof(short) == 2)
	{
		if (un.c[0] == 1 && un.c[1] == 2)
			printf ("big-endian\n");
		else if (un.c[0] == 2 && un.c[1] == 1)
			printf ("little-endian\n");
		else
			printf ("unknown\n");
	}
	else
		printf ("sizeof(short) = %d\n", sizeof(short));

	print_val();

	exit(0);
}


int print_val ()
{
	short val1 = 3;
	printf ("val1 = %d, htons(val) = %d\n", val1, htons(val1));
	int val2 = 55;
	printf ("val2 = %d, htonl(val2) = %d\n", val2, htonl(val2));
	return 0;
}
