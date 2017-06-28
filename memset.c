#include <stdio.h>
#include <string.h>

int main(void)
{
	char *ptr;

	ptr = (char *)malloc(10);

	ptr[0] = '\0';

	printf("%d\n",ptr[0]);

	return 0;
}
