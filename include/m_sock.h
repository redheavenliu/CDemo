#ifndef M_SOCK_H
#define M_SOCK_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

#define MAXLINE 2048
#define	SA	struct sockaddr
#define	LISTENQ		1024	/* 2nd argument to listen() */

char *
sock_ntop(const struct sockaddr *sa, socklen_t salen);

ssize_t						/* Write "n" bytes to a descriptor. */
sock_writen(int fd, const void *vptr, size_t n);

ssize_t
sock_readline(int fd, void *vptr, size_t maxlen);

int
sock_open
(
    const char *host,
    int port
);

int
sock_ser_open
(
    int domain,
    int type,
    int port
);

int
sock_cli_open
(
    int domain,
    int type,
    const void *host,
    int port
);

int sockaddr_init(struct sockaddr_in *addr,void *host,int port,int family);

ssize_t
sock_recvfrom(int fd, void *buf, size_t len, int flags,
                struct sockaddr *addr, socklen_t *addr_len);

ssize_t
sock_sendto(int fd, void *buf, size_t len,
               int flags, struct sockaddr *addr,
               socklen_t addr_len);

/* int
 * inet_pton(af, src, dst)
 *	convert from presentation format (which usually means ASCII printable)
 *	to network format (which is usually some kind of binary format).
 * return:
 *	1 if the address was valid for the specified address family
 *	0 if the address wasn't valid (`dst' is untouched in this case)
 *	-1 if some other error occurred (`dst' is untouched in this case, too)
 * author:
 *	Paul Vixie, 1996.
 */
int
sock_inet_pton(
	int af,
	const char *src,
	void *dst);


#endif /* M_SOCK_H */
