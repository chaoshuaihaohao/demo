#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
#include <sys/mman.h>


int main(int argc, char **argv)
{
	int length = 0x20000;
	
	int fd = open("/dev/demo_dev",O_RDWR|O_SYNC);
	if (fd < 0)
	{
		printf("Open failed\n");
		return -1;
	}
	char *addr = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
    	printf("Map failed\n");
	return -1;
    }

//    memset(addr, 0, length);
	
#if 1
    	printf("pid = %d\naddr = %x, value = %d\n", getpid(), (int *)addr, (int)*addr);
	*addr = 10;
    	printf("pid = %d\naddr = %x, value = %d\n", getpid(), (int *)addr, (int)*addr);
	pause();
#endif
	munmap(addr, length);
	return 0;

}
