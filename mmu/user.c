#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int virt;


	char *str = malloc(4096);
	if (!str) {
		printf("Error: Nomem\n");
		return -1;
	}

	memset(str, 0, sizeof(str));
	strcpy(str, "Hello World!");
	printf("the virt addr of str is %p\n", str);
	printf("the context of str is %s\n", str);

	printf("the pid is %d\n", getpid());
	pause();
}
