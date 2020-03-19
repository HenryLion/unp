#include <stdio.h>

int main (void)
{
	int choice;

	char name[64] = {0};
	char pw[64] = {0};

	printf ("please input your choice: ");
	scanf ("%d", &choice);

	switch (choice)
	{
		case 1:
		{
			printf ("please input your name: ");
			scanf ("%s", name);
			printf ("please input your pw: ");
			scanf ("%s", pw);
			break;
		}
		default:
			break;
	}

	printf ("your name is: %s, pw is %s\n", name, pw);

	return 0;
}
