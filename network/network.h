#ifndef __NETWORK__
#define __NETWORK__

//recv
typedef struct recv_handler {
	const char *description;
	int (*callback)(int sockfd, void *buf, size_t len, struct msghdr * msg,
			int flags, struct sockaddr * src_addr,
			socklen_t * addrlen);
} recv_handler_t;

typedef enum readRequest {
	READ = 0,
	RECV = 1,
	RECVFROM = 2,
	RECVMSG = 3,
} readRequest;

static int net_read(int sockfd, void *buf, size_t len, struct msghdr *msg,
		    int flags, struct sockaddr *src_addr, socklen_t * addrlen)
{
	return read(sockfd, buf, len);
}

static int net_recv(int sockfd, void *buf, size_t len, struct msghdr *msg,
		    int flags, struct sockaddr *src_addr, socklen_t * addrlen)
{
	return recv(sockfd, buf, len, flags);
}

static int net_recvfrom(int sockfd, void *buf, size_t len, struct msghdr *msg,
			int flags, struct sockaddr *src_addr,
			socklen_t * addrlen)
{
	return recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

static int net_recvmsg(int sockfd, void *buf, size_t len, struct msghdr *msg,
		       int flags, struct sockaddr *src_addr,
		       socklen_t * addrlen)
{
	return recvmsg(sockfd, msg, flags);
}

//send
typedef struct send_handler {
	const char *description;
	int (*callback)(int sockfd, const void *buf, size_t len,
			const struct msghdr * msg, int flags,
			const struct sockaddr * dest_addr, socklen_t addrlen);
} send_handler_t;

typedef enum sendRequest {
	WRITE = 0,
	SEND = 1,
	SENDTO = 2,
	SENDMSG = 3,
} sendRequest;

static int net_write(int sockfd, const void *buf, size_t len,
		     const struct msghdr *msg, int flags,
		     const struct sockaddr *dest_addr, socklen_t addrlen)
{
	return write(sockfd, buf, len);
}

static int net_send(int sockfd, const void *buf, size_t len,
		    const struct msghdr *msg, int flags,
		    const struct sockaddr *dest_addr, socklen_t addrlen)
{
	return send(sockfd, buf, len, flags);
}

static int net_sendto(int sockfd, const void *buf, size_t len,
		      const struct msghdr *msg, int flags,
		      const struct sockaddr *dest_addr, socklen_t addrlen)

{
	return sendto(sockfd, buf, len, flags, dest_addr, addrlen);

}

static int net_sendmsg(int sockfd, const void *buf, size_t len,
		       const struct msghdr *msg, int flags,
		       const struct sockaddr *dest_addr, socklen_t addrlen)
{
	return sendmsg(sockfd, msg, flags);
}

#define RECV_HANDLERS \
HANDLER(READ, net_read) \
HANDLER(RECV, net_recv) \
HANDLER(RECVFROM, net_recvfrom) \
HANDLER(RECVMSG, net_recvmsg)

#define SEND_HANDLERS \
HANDLER(WRITE, net_write) \
HANDLER(SEND, net_send) \
HANDLER(SENDTO, net_sendto) \
HANDLER(SENDMSG, net_sendmsg)

#define HANDLER(id, handler) \
        [id] = { #id, handler },

static recv_handler_t recv_handlers[] = {
	RECV_HANDLERS
};

static send_handler_t send_handlers[] = {
	SEND_HANDLERS
};

#undef RECV_HANDLER

struct net_domain {
	int domain;
	char purpose[256];

};

struct net_type {
	int type;
};

struct net_protocol {
	int protocol;
};

struct network {
	struct net_domain net_domain;
	struct net_type net_type;
	struct net_protocol net_protocol;
	int recv_func;		//read function:recv/recvmsg/read/recvfrom?
	int send_func;
};

void network_check();
int parse_json(struct network *np);

#endif
