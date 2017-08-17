#ifndef M_WRAP_H
#define M_WRAP_H

#include "m_stdio.h"
#include "m_sock.h"
#include "m_file.h"
#include "m_signal.h"

#include <poll.h>

int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void
Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void
Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);

void
Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr);

#ifdef	HAVE_INET6_RTH_INIT
int
Inet6_rth_space(int type, int segments);

void *
Inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments);

void
Inet6_rth_add(void *rthbuf, const struct in6_addr *addr);

void
Inet6_rth_reverse(const void *in, void *out);

int
Inet6_rth_segments(const void *rthbuf);

struct in6_addr *
Inet6_rth_getaddr(const void *rthbuf, int idx);
#endif

#ifdef HAVE_KQUEUE
int
Kqueue(void);

int
Kevent(int kq, const struct kevent *changelist, int nchanges,
       struct kevent *eventlist, int nevents, const struct timespec *timeout);
#endif


/* include Listen */
void
Listen(int fd, int backlog);
/* end Listen */

#ifdef	HAVE_POLL
int
Poll(struct pollfd *fdarray, unsigned long nfds, int timeout);
#endif

ssize_t
Recv(int fd, void *ptr, size_t nbytes, int flags);

ssize_t
Recvfrom(int fd, void *ptr, size_t nbytes, int flags,
		 struct sockaddr *sa, socklen_t *salenptr);

ssize_t
Recvmsg(int fd, struct msghdr *msg, int flags);

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout);

void
Send(int fd, const void *ptr, size_t nbytes, int flags);

void
Sendto(int fd, const void *ptr, size_t nbytes, int flags,
	   const struct sockaddr *sa, socklen_t salen);

void
Sendmsg(int fd, const struct msghdr *msg, int flags);

void
Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

void
Shutdown(int fd, int how);

int
Sockatmark(int fd);

/* include Socket */
int
Socket(int family, int type, int protocol);
/* end Socket */

void
Socketpair(int family, int type, int protocol, int *fd);

const char *
Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

void
Inet_pton(int family, const char *strptr, void *addrptr);

void *
Calloc(size_t n, size_t size);

void
Close(int fd);

void
Dup2(int fd1, int fd2);

int
Fcntl(int fd, int cmd, int arg);

void
Gettimeofday(struct timeval *tv, void *foo);

int
Ioctl(int fd, int request, void *arg);

pid_t
Fork(void);

void *
Malloc(size_t size);

int
Mkstemp(char *template);

#include	<sys/mman.h>

void *
Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

int
Open(const char *pathname, int oflag, mode_t mode);

void
Pipe(int *fds);

ssize_t
Read(int fd, void *ptr, size_t nbytes);

void
Sigaddset(sigset_t *set, int signo);

void
Sigdelset(sigset_t *set, int signo);

void
Sigemptyset(sigset_t *set);

void
Sigfillset(sigset_t *set);

int
Sigismember(const sigset_t *set, int signo);

void
Sigpending(sigset_t *set);

void
Sigprocmask(int how, const sigset_t *set, sigset_t *oset);

char *
Strdup(const char *str);

long
Sysconf(int name);

#ifdef	HAVE_SYS_SYSCTL_H
void
Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
	   void *newp, size_t newlen);
#endif

void
Unlink(const char *pathname);

pid_t
Wait(int *iptr);

pid_t
Waitpid(pid_t pid, int *iptr, int options);

void
Write(int fd, void *ptr, size_t nbytes);

char *
Sock_ntop(const struct sockaddr *sa, socklen_t salen);

Sigfunc *
Signal(int signo, Sigfunc *func);	/* for our signal() function */

void
Writen(int fd, void *ptr, size_t nbytes);

void
Fclose(FILE *fp);

FILE *
Fdopen(int fd, const char *type);

char *
Fgets(char *ptr, int n, FILE *stream);

FILE *
Fopen(const char *filename, const char *mode);

void
Fputs(const char *ptr, FILE *stream);

ssize_t
Readline(int fd, void *ptr, size_t maxlen);

#endif /* M_WRAP_H */
