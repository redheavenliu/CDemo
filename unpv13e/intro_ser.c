#include "m_stdio.h"
#include "m_sock.h"
#include "m_wrap.h"

#include <time.h>

int
main_v6(int argc, char **argv)
{
    int					listenfd, connfd;
    socklen_t			len;
    char				buff[MAXLINE];
    time_t				ticks;
    struct sockaddr_in6	servaddr, cliaddr;

    listenfd = Socket(AF_INET6, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr   = in6addr_any;
    servaddr.sin6_port   = htons(6666);	/* daytime server */

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
    	len = sizeof(cliaddr);
    	connfd = Accept(listenfd, (SA *) &cliaddr, &len);
    	printf("connection from %s\n",
    		   Sock_ntop((SA *) &cliaddr, len));

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

    	Close(connfd);
    }
    return 0;
}


int
main_v4(int argc, char **argv)
{
    int					listenfd, connfd;
    socklen_t			len;
    struct sockaddr_in	servaddr, cliaddr;
    char				buff[MAXLINE];
    time_t				ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(4444);	/* daytime server */

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for ( ; ; ) {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *) &cliaddr, &len);
            printf("connection from %s, port %d\n",
            Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
            ntohs(cliaddr.sin_port));

            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            Write(connfd, buff, strlen(buff));

            Close(connfd);
    }
    return 0;
}

int
main(int argc, char **argv)
{
    if(argc != 2)
        err_quit("usage: a.out <v4/v6>");
    if(!strcmp(argv[1],"v4")){
        return main_v4(argc,argv);
    }else if(!strcmp(argv[1],"v6")){
        return main_v6(argc,argv);
    }else{
        err_quit("usage: a.out <v4/v6>");
    }
    return 0;
}

