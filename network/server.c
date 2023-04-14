#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> //AF_INET
#include <unistd.h>

#include "network.h"
#include "cJSON.h"

//network program
static struct network np;

void network_check()
{
	if (np.net_domain.domain == AF_UNIX && np.net_type.type == SOCK_DGRAM) {
		printf("Error\n");
		exit(-1);
	}
	printf("Check pass\n");
}

/* recv and send global varibles */
static int sock = 0;
static int sockfd = 0;

/* recv global varibles */

static recv_handler_t *recv_handler;
static char recv_buf[BUFSIZ] = { 0 };

static struct msghdr recv_msg;
static struct iovec recv_iov[1];

static int recv_flags = 0;
static struct sockaddr_un src_addr_un;
static struct sockaddr_in src_addr_in;
static socklen_t src_addrlen;
int do_recv(int sockfd)
{

	//----------------
	recv_iov[0].iov_base = recv_buf;
	recv_iov[0].iov_len = sizeof(recv_buf);
	recv_msg.msg_name = NULL;
	recv_msg.msg_namelen = 0;
	recv_msg.msg_iov = recv_iov;
	recv_msg.msg_iovlen = 1;
	//----------------

	recv_handler = &recv_handlers[np.recv_func];
	int recv_len =
	    recv_handler->callback(sockfd, recv_buf, sizeof(recv_buf),
				   &recv_msg, recv_flags,
				   (struct sockaddr *)&src_addr_un,
				   &src_addrlen);
	if (recv_len < 0) {
		perror("recv socket failed");
		return -1;
	} else if (recv_len == 0) {
		printf("client close connection\n");
		exit(0);
	}

	recv_buf[recv_len] = 0;

	/* Output recv message */
	printf("recv function %d: %s\n", np.recv_func, recv_buf);
}

static unsigned long tmp[BUFSIZ];

void do_service()
{
	/* print virt addr value */
	unsigned long *p;
	tmp[0] = strtoul(recv_buf, 0, 16);
	printf("tmp = 0x%lx\n", tmp[0]);

//      p = (unsigned long *)tmp[0];

//      printf("%ld\n", *p);
}

/* send global varibles */
static char send_buf[BUFSIZ] = { 0 };

static send_handler_t *send_handler;

static struct msghdr send_msg;
static struct iovec send_iov[1];

static int send_flags = 0;
const struct sockaddr_un dest_addr_un;
static socklen_t dest_addrlen;

const struct sockaddr_in dest_addr_in;

int do_send(int sockfd)
{
	switch (np.send_func) {
	case WRITE:
		printf("WRITE\n");
		break;
	case SEND:
		printf("SEND\n");
		break;
	case SENDTO:
		printf("SENDTO\n");
		break;
	case SENDMSG:
		printf("SENDMSG\n");
		break;
	default:
		printf("Not support send function\n");

	};

	/* Input send message */
//	strcpy(send_buf, "hello world");
	strcpy(send_buf, recv_buf);

	//----------------
	send_iov[0].iov_base = send_buf;
	send_iov[0].iov_len = sizeof(send_buf);
	send_msg.msg_name = NULL;
	send_msg.msg_namelen = 0;
	send_msg.msg_iov = send_iov;
	send_msg.msg_iovlen = 1;
	//----------------

	send_handler = &send_handlers[np.send_func];
	int send_len =
	    send_handler->callback(sockfd, send_buf, sizeof(send_buf),
				   &send_msg, send_flags,
				   (struct sockaddr *)&dest_addr_un,
				   dest_addrlen);
	printf("send function %d :%s\n", np.send_func, send_buf);
}

int main()
{
	parse_json(&np);
	/* np validation check */
//      network_check();

	printf("pid = %d\n", getpid());

	sock =
	    socket(np.net_domain.domain, np.net_type.type,
		   np.net_protocol.protocol);
	if (sock < 0) {
		perror("create socket failed");
		return -1;
	}
	
	//TODO:add socketopt

	switch (np.net_domain.domain) {
	case AF_UNIX || AF_LOCAL:
		const char *file_path = "/tmp/local_sock";
		/*  确保UNIX_DOMA un_FILE所指向的文件存在且可写，否则退出 */
		if (access(file_path, F_OK) == 0)
			unlink(file_path);

		struct sockaddr_un src_addr_un;
		src_addr_un.sun_family = AF_LOCAL;
		strcpy(src_addr_un.sun_path, file_path);

		// bind    
		if (bind
		    (sock, (struct sockaddr *)&src_addr_un,
		     sizeof(src_addr_un)) < 0) {
			perror("bind socket failed");
			return -1;
		}

		switch (np.net_type.type) {
		case SOCK_STREAM:
			// listen
			if (listen(sock, 0) < 0) {
				perror("listen socket failed");
				return -1;
			}

			while (1) {
				sockfd = accept(sock, (struct sockaddr *)
						&dest_addr_un, &dest_addrlen);
				if (sockfd < 0) {
					perror("accept socket failed");
					return -1;
				} else if (sockfd == 0) {
					perror("accept socket failed");
					return -1;
				}

				while (1) {
					/* Do recv */
					do_recv(sockfd);

					/* handle recv msg */
					do_service();

					/* Send handled msg reply */
					do_send(sockfd);
				}

				close(sockfd);
			}

			break;
		case SOCK_DGRAM:
			while (1) {
				/* Do recv */
				do_recv(sock);

				/* handle recv msg */
				do_service();

				/* Send handled msg reply */
				do_send(sock);
			}

			break;
		default:
			perror("Not support type.");
			exit(-1);
		};

		break;
	case AF_INET:
		struct sockaddr_in src_addr_in;

#if 0
		// Forcefully attaching socket to the port 8080
		if (setsockopt(server_fd, SOL_SOCKET,
			SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt))) {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
#endif
		src_addr_in.sin_family = AF_INET;
		src_addr_in.sin_addr.s_addr = INADDR_ANY; //all ip addr
		#define PORT 8080
		src_addr_in.sin_port = htons(PORT);

		// bind    
		if (bind
		    (sock, (struct sockaddr *)&src_addr_in,
		     sizeof(src_addr_in)) < 0) {
			perror("bind socket failed");
			return -1;
		}

		switch (np.net_type.type) {
		case SOCK_STREAM:
			// listen
			if (listen(sock, 0) < 0) {
				perror("listen socket failed");
				return -1;
			}

			while (1) {
				sockfd = accept(sock, (struct sockaddr *)
						&dest_addr_in, &dest_addrlen);
				if (sockfd < 0) {
					perror("accept socket failed");
					return -1;
				} else if (sockfd == 0) {
					perror("accept socket failed");
					return -1;
				}

				while (1) {
					/* Do recv */
					do_recv(sockfd);

					/* handle recv msg */
					do_service();

					/* Send handled msg reply */
					do_send(sockfd);
				}

				close(sockfd);
			}

			break;
		case SOCK_DGRAM:
			while (1) {
				/* Do recv */
				do_recv(sock);

				/* handle recv msg */
				do_service();

				/* Send handled msg reply */
				do_send(sock);
			}

			break;
		default:
			perror("Not support type.");
			exit(-1);
		};
		break;

	default:
		break;
	};

	close(sock);

	return 0;
}
