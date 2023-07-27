#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{

	void *__addr = 0xd0000000;
	int length = 4096;
#if 1
	//char *addr = mmap(__addr, length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	char *addr =
	    mmap(NULL, length, PROT_READ | PROT_WRITE,
		 MAP_SHARED | MAP_ANONYMOUS, -1, 0);

#else
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
//      char *addr = mmap(0xe0000000, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xd0000000);
	char *addr =
	    mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		 0xd0000000);
//      char *addr = mmap(NULL, 0xff, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x20000);

#endif
	if (addr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}
	printf("pid = %d\naddr = %p, value = %d\n", getpid(), (int *)addr,
	       (int)*addr);
	*addr = 10;
	printf("pid = %d\naddr = %p, value = %d\n", getpid(), (int *)addr,
	       (int)*addr);
	pause();

	munmap(addr, length);
	return 0;

}
