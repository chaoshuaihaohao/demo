#include <stdio.h>

#include <signal.h>		// 关于信号的头文件

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

unsigned long *p = 0x7f3be6836000;

static void do_sigint(int signal)
{
	printf("do sigint %d\n", signal);
}

int main(int argc, char **argv)
{
	signal(SIGINT, do_sigint);
	printf("pid = %d\n", getpid());

	/* Pause to see the /proc/<pid>/maps */
	pause();

	/* Block to recv the p address from peer */
	int fd, new_fd, numbytes, i;
	struct sockaddr_un server_addr;
	unsigned long send_buf[BUFSIZ];
	unsigned long recv_buf[BUFSIZ];
	char *mysocketpath = argv[1];
	struct msghdr msg;
	struct iovec io;

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, mysocketpath,
		sizeof(server_addr.sun_path));
	while ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) ;
	if (bind
	    (fd, (struct sockaddr *)(&server_addr),
	     sizeof(struct sockaddr_un)) < 0) {
		fprintf(stderr, "bind fail\n");
		exit(EXIT_FAILURE);
	}
	printf("Bind Success!\n");
	while (1) {
		bzero(&msg, sizeof(struct msghdr));
		msg.msg_name = &server_addr;
		msg.msg_namelen = sizeof(struct sockaddr_un);
		io.iov_base = send_buf;
		io.iov_len = BUFSIZ;
		msg.msg_iov = &io;
		msg.msg_iovlen = 1;

		numbytes = recvmsg(fd, &msg, 0);

		char *temp = msg.msg_iov[0].iov_base;	//获取得到的数据
		temp[numbytes] = '\0';	//为数据末尾添加结束符
		printf("get %d \n", numbytes);
		printf("server recv:%s\n", temp);

		char *buf = "server sendmsg";
		strncpy(send_buf, buf, sizeof(send_buf));
		bzero(&msg, sizeof(struct msghdr));
		msg.msg_name = &server_addr;
		msg.msg_namelen = sizeof(struct sockaddr_un);
		io.iov_base = send_buf;
		io.iov_len = sizeof(send_buf);
		msg.msg_iov = &io;
		msg.msg_iovlen = 1;

		sendmsg(fd, &msg, 0);
		printf("server send:%s\n", buf);
	}
	close(fd);
	return 0;

//	printf("%ld\n", *p);

}
