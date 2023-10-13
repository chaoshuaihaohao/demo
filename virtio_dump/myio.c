#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

       #include <sys/types.h>

       #include <sys/stat.h>

       #include <fcntl.h>

//io bdf bar_id len

int main(int argc, char **argv)
{
	int fd = openat(AT_FDCWD, "/sys/bus/pci/devices/0000:03:00.0/resource0", O_RDWR|O_SYNC);
	if (fd < 0) {
		printf("open failed\n");
		return -1;
	}

	u_int8_t *addr = mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, fd, 0);
	if (!addr) {
		printf("addr failed\n");
		return -1;
	}

	int i;
	u_int8_t buf;
	for (i = 0; i < *argv[3]; i++) {
		printf("%02x ", *(addr+i));
		if ((i+1) % 16 == 0)
			printf("\n");
	}
	close(fd);

	return 0;
}
