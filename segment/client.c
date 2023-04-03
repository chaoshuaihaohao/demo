#include <stdio.h>
#include <string.h>		//bzero
#include <stdlib.h>		//exit

#include <unistd.h>		//read,close

#include <sys/types.h>
#include <sys/socket.h>		//socket,bind

#include <linux/un.h>

int main(int argc, char *argv[])
{
	int c;
	char *mysocketpath = argv[1];
	struct msghdr msg;
	struct iovec io;

	int sockfd;
	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);	// 创建通信端点：套接字
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	struct sockaddr_un server_addr;
	bzero(&server_addr, sizeof(server_addr));	// 初始化服务器地址
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, mysocketpath,
		sizeof(server_addr.sun_path));
	printf("%s\n", server_addr.sun_path);

	char recv_buf[500];
	strncpy(recv_buf, argv[2], sizeof(recv_buf));
	while (1) {
		bzero(&msg, sizeof(struct msghdr));
		//msg.msg_name = NULL;
		msg.msg_name = &server_addr;
		msg.msg_namelen = sizeof(struct sockaddr_un);
		io.iov_base = recv_buf;
		io.iov_len = 500;
		msg.msg_iov = &io;
		msg.msg_iovlen = 1;
		printf("-----client sendmsg-----:%s\n", recv_buf);
		sendmsg(sockfd, &msg, 0);	// 向服务器发送信息
#if 1
		bzero(&msg, sizeof(struct msghdr));
		//msg.msg_name = NULL;
		msg.msg_name = &server_addr;
		msg.msg_namelen = sizeof(struct sockaddr_un);
		io.iov_base = recv_buf;
		io.iov_len = 500;
		msg.msg_iov = &io;
		msg.msg_iovlen = 1;
		printf("1\n");
		while (recvmsg(sockfd, &msg, 0) > 0) {
			printf("-----client recvmsg-----\n");
			printf("%s\n", (char*)msg.msg_iov->iov_base);
			break;
		}
		sleep(1);
		printf("2\n");
#endif
	}
	close(sockfd);

	return 0;
}
