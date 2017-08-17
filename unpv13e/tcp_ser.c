#include "m_stdio.h"
#include "m_sock.h"
#include "m_wrap.h"

#include	<linux/limits.h>		/* for OPEN_MAX */
#include	<poll.h>		/* for convenience */
#include <sys/epoll.h>

#define SERVER_PORT 8888

void
sig_chld(int signo)
{
//	pid_t	pid;
//	int		stat;

//	pid = wait(&stat);
//	printf("Child[%d] exit.\n", pid);
//	return;

	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("Child[%d] exit.\n", pid);
	return;
}

int
main_1(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERVER_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

       // 子进程死后发送信号 SIGCHLD
       Signal(SIGCHLD, sig_chld);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

             char ip[MAXLINE];
             Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
             int port = ntohs(cliaddr.sin_port);

             printf("[%s:%d] connected.\n",ip,port);

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */

                    char *ip_cli = (char*)malloc(strlen(ip) + 1);
                    bzero(ip_cli,strlen(ip) + 1);
                    memcpy(ip_cli,ip,strlen(ip));
                    int port_cli = port;
                    int n = 0;
                    char buf[MAXLINE];

                	while ( (n = read(connfd, buf, MAXLINE)) != 0){
                            if(n > 0){
                               buf[n] = '\0';
                               //printf("line = \"%s\"\n",buf);
//                               if(strcmp(buf,"exit") == 0){
//                                    printf("Client exit!\n");
//                                    break;
//                               }
                               printf("[%s:%d]>>%s\n",ip_cli,port_cli,buf);
                               buf[n++] = '\n';
                               buf[n] = '\0';
                		    Writen(connfd, buf, n);
                            }else{ // n < 0
                                if(errno == EINTR){// (n<0 && errno == EINTR)  read again
                                    // usleep(100);
                                    continue;
                                }else{
                                    // exit
                                    printf("Read error(%d)\n",errno);
                                    break;
                                }
                            }
                	}
                    printf("[%s:%d] disconnected.\n",ip_cli,port_cli);
                    free(ip_cli);
                    ip_cli = NULL;
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
    exit(0);
}

int
main_2(int argc, char **argv)
{
	int					listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERVER_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
              // 有client连接后，马上关闭，client第二次写时会收到信号 : SIGPIPE
		Close(connfd);			/* parent closes connected socket */
	}
    exit(0);
}

int
main_3(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERVER_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

                     char ip[MAXLINE];
                     Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
                     int port = ntohs(cliaddr.sin_port);

                     printf("[%s:%d] connected.\n",ip,port);

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
                                  printf("No data.\n");
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
				    buf[n] = '\0';
                               if(strcmp(buf,"exit") == 0){
                                    printf("Client exit!\n");
                                    //break;
                               }
                               printf("%s\n",buf);
                               buf[n++] = '\n';
                               buf[n] = '\0';
				    Writen(sockfd, buf, n);
				}

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}

int
main_4(int argc, char **argv)
{
	int					i, maxi, listenfd, connfd, sockfd;
	int					nready;
	ssize_t				n;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct pollfd		client[_SC_OPEN_MAX];
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERVER_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < _SC_OPEN_MAX; i++)
		client[i].fd = -1;		/* -1 indicates available entry */
	maxi = 0;					/* max index into client[] array */
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		nready = Poll(client, maxi+1, INFTIM);

		if (client[0].revents & POLLRDNORM) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

            char ip[MAXLINE];
            Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
            int port = ntohs(cliaddr.sin_port);

            printf("[%s:%d] connected.\n",ip,port);

			for (i = 1; i < _SC_OPEN_MAX; i++)
				if (client[i].fd < 0) {
					client[i].fd = connfd;	/* save descriptor */
					break;
				}
			if (i == _SC_OPEN_MAX)
				err_quit("too many clients");

			client[i].events = POLLRDNORM;
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 1; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i].fd) < 0)
				continue;
			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ( (n = read(sockfd, buf, MAXLINE)) < 0) {
					if (errno == ECONNRESET) {
							/*4connection reset by client */
#ifdef	NOTDEF
						printf("client[%d] aborted connection\n", i);
#endif
						Close(sockfd);
						client[i].fd = -1;
					} else
						err_sys("read error");
				} else if (n == 0) {
						/*4connection closed by client */
#ifdef	NOTDEF
					printf("client[%d] closed connection\n", i);
#endif
					Close(sockfd);
					client[i].fd = -1;
				} else {
				    buf[n] = '\0';
                               if(strcmp(buf,"exit") == 0){
                                    printf("Client exit!\n");
                                    //break;
                               }
                               printf("%s\n",buf);
                               buf[n++] = '\n';
                               buf[n] = '\0';
				    Writen(sockfd, buf, n);
				}

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}

int
main_5(int argc, char **argv)
{
    int					i, maxi, listenfd, connfd, sockfd;
    int					nready;
    ssize_t				n;
    char				buf[MAXLINE];
    socklen_t			clilen;
    struct sockaddr_in	cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct epoll_event ev,events[20];
    //生成用于处理accept的epoll专用的文件描述符
    int epfd = epoll_create(256);
    //设置与要处理的事件相关的文件描述符
    ev.data.fd = listenfd;
    //设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;
    //注册epoll事件
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERVER_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

/* include fig02 */
    for ( ; ; ) {
        nready = epoll_wait(epfd,events,20,-1);

        for(i=0;i<nready;++i) {
            //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
            if(events[i].data.fd == listenfd) {
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

                char ip[MAXLINE];
                Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
                int port = ntohs(cliaddr.sin_port);

                printf("[%s:%d] connected.\n",ip,port);

                ev.data.fd = connfd;
                //设置用于注测的读操作事件
                ev.events = EPOLLIN|EPOLLET;
                //注册ev
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);

            } else if (events[i].events & EPOLLIN) {
            //如果是已经连接的用户，并且收到数据，那么进行读入。
                if ( (sockfd = events[i].data.fd) < 0)
                    continue;

                if ( (n = read(sockfd, buf, MAXLINE)) < 0) {
                	if (errno == ECONNRESET) {
                			/*4connection reset by client */
                        Close(sockfd);
                        events[i].data.fd = -1;
                	} else
                        err_sys("read error");
                } else if (n == 0) {
                		/*4connection closed by client */
                    Close(sockfd);
                    events[i].data.fd = -1;
                } else {
                    buf[n] = '\0';
                    printf("client:%s\n",buf);
                    if(strcmp(buf,"exit") == 0){
                        printf("Client exit!\n");
                        //break;
                    }
                    Writen(sockfd, "OK", 2);
                }

                //设置用于写操作的文件描述符
                //ev.data.fd=sockfd;
                //设置用于注测的写操作事件
                //ev.events=EPOLLOUT|EPOLLET;
                //修改sockfd上要处理的事件为EPOLLOUT
                //epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            } else if (events[i].events & EPOLLOUT) {// 如果有数据发送
                sockfd = events[i].data.fd;
                Writen(sockfd, buf, n);
                //设置用于读操作的文件描述符
                ev.data.fd=sockfd;
                //设置用于注测的读操作事件
                ev.events=EPOLLIN|EPOLLET;
                //修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
        }
    }
}

int
main(int argc, char **argv)
{
    //return main_1(argc, argv);
    //return main_2(argc, argv);
    //return main_3(argc, argv);
    //return main_4(argc, argv);
    return main_5(argc, argv);
}