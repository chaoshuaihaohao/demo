#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <errno.h>


#include "network.h"

//network program
static struct network np;


static int sock;
struct sockaddr_un serv_un; //AF_UNIX\AF_LOCAL
struct sockaddr_in serv_in; //AF_INET

/* send global varibles */
static send_handler_t *send_handler;
static char send_buf[BUFSIZ];
static int send_flags = 0;
const struct sockaddr_un dest_addr_un;
const struct sockaddr_in dest_addr_in;
static socklen_t dest_addrlen;

static struct msghdr send_msg;
static struct iovec send_iov[1];

static int do_client_send(int sockfd)
{
	char *buff = NULL;
	size_t len = 0;

	printf("Please input the message you want to send:\n");
repeat:
	len = getline(&buff, &len, stdin);
	if (len <= 0) {
		goto repeat;
	}

	if (buff[0] == 'q' || buff[0] == 'Q') {
		// break;
		len = 0;
	}

	memcpy(send_buf, buff, sizeof(send_buf));
	//----------------
//        send_iov[0].iov_base = send_buf;
//        send_iov[0].iov_len = sizeof(send_buf);
	send_iov[0].iov_base = buff;
	send_iov[0].iov_len = len;
	send_msg.msg_name = NULL;
	send_msg.msg_namelen = 0;
	send_msg.msg_iov = send_iov;
	send_msg.msg_iovlen = 1;
	//----------------
//      int send_len = send(sock, buff, len, 0);
	send_handler = &send_handlers[np.send_func];
	int send_len =
	    send_handler->callback(sockfd, send_buf, sizeof(send_buf),
				   &send_msg, send_flags,
				   (struct sockaddr *)&dest_addr_un,
				   dest_addrlen);

	if (send_len < 0) {
		perror("send socket failed");
		return -1;
	} else if (send_len == 0) {
		printf("client close connection\n");
		close(sock);
		exit(0);
	}

	printf("send function %d :%s\n", np.send_func, buff);

	free(buff);
}

/* recv global varibles */

static recv_handler_t *recv_handler;
static char recv_buf[BUFSIZ] = { 0 };

static struct msghdr recv_msg;
static struct iovec recv_iov[1];

static int recv_flags = 0;
static struct sockaddr_un src_addr_un;
static socklen_t src_addrlen;

static void do_client_recv(int sockfd)
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
		return;
	} else if (recv_len == 0) {
		printf("client close connection\n");
		exit(0);
	}

	recv_buf[recv_len] = 0;

	/* Output recv message */
	printf("recv :%s\n", recv_buf);
	printf("recv function %d: %s\n", np.recv_func, recv_buf);

}

int main()
{

	/* Get network configuration from JSON */
	parse_json(&np);

	printf("np.net_domain.domain=%d\n", np.net_domain.domain);
	printf("np.net_type.type=%d\n", np.net_type.type);
	printf("np.net_protocol.protocol=%d\n", np.net_protocol.protocol);
	// socket
	/* np validation check */
// 	network_check();

	printf("pid = %d\n", getpid());

	sock =
	    socket(np.net_domain.domain, np.net_type.type,
		   np.net_protocol.protocol);
	if (sock < 0) {
		perror("create socket failed");
		return -1;
	}

	switch (np.net_domain.domain) {
	case AF_UNIX || AF_LOCAL:
		const char *filepath = "/tmp/local_sock";
		// if(access(filepath, F_OK) == 0){
		//     unlink(filepath);
		// }
		/*  确保UNIX_DOMA un_FILE所指向的文件存在且可写，否则退出 */
		if (access(filepath, F_OK | W_OK) < 0) {
			perror("has no access to socket file.");
			exit(-1);
		}

		//serv_un.sun_family = AF_LOCAL;
		//strcpy(serv_un.sun_path, filepath);

		serv_un.sun_family = AF_LOCAL;
		strcpy(serv_un.sun_path, filepath);
		switch (np.net_type.type) {
		case SOCK_STREAM:
			if (connect
			    (sock, (struct sockaddr *)&serv_un,
			     SUN_LEN(&serv_un)) < 0) {
				perror("connect socket failed");
				return -1;
			}

			while (1) {
				/* do_client_send */
				do_client_send(sock);

				do_client_recv(sock);

			}
			break;
		case SOCK_DGRAM:
#if 0
#define USE_TCP			// udp有问题
			serv_un.sun_family = AF_LOCAL;
			strcpy(serv_un.sun_path, filepath);
			if (bind(sock, (struct sockaddr *)&serv_un, sizeof(serv_un)) <
			    0) {
				perror("bind socket failed");
				return -1;
			}
#endif
			while (1) {
				/* do_client_send */
				do_client_send(sock);

				do_client_recv(sock);

			}

			break;
		default:
			break;
		};
		break;
	case AF_INET:
		//serv_un.sun_family = AF_LOCAL;
		//strcpy(serv_un.sun_path, filepath);
		serv_in.sin_family = AF_INET;
		#define PORT 8080
		serv_in.sin_port = htons(PORT);
		// Convert IPv4 and IPv6 addresses from text to binary
		// form
		if (inet_pton(AF_INET, "127.0.0.1", &serv_in.sin_addr)
		<= 0) {
			printf("\nInvalid address/ Address not supported \n");
			return -1;
		}

		switch (np.net_type.type) {
		case SOCK_STREAM:
			if (connect
			    (sock, (struct sockaddr *)&serv_in,
			     sizeof(serv_in)) < 0) {
				perror("connect socket failed");
				return -1;
			}

			while (1) {
				/* do_client_send */
				do_client_send(sock);

				do_client_recv(sock);

			}
			break;
		case SOCK_DGRAM:
#if 0
#define USE_TCP			// udp有问题
			serv_un.sun_family = AF_LOCAL;
			strcpy(serv_un.sun_path, filepath);
			if (bind(sock, (struct sockaddr *)&serv_un, sizeof(serv_un)) <
			    0) {
				perror("bind socket failed");
				return -1;
			}
#endif
			while (1) {
				/* do_client_send */
				do_client_send(sock);

				do_client_recv(sock);

			}

			break;
		default:
			break;
		};
		break;

	default:
		break;
	};

	close(sock);
	return 0;
}
