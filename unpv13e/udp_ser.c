#include <stdio.h>

#include "m_stdio.h"
#include "m_type.h"
#include "m_sock.h"
#include "m_log.h"

#define SERVER_UDP_PORT 9877

static void main_1(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;
    int clilen;
    char mesg[MAXLINE];

    sockfd = sock_ser_open(AF_INET,SOCK_DGRAM,SERVER_UDP_PORT);
    if(sockfd < 0) {
        log_e("Open server socket fail.");
        return;
    }

    log_d("UDP server is running ...");

    clilen = sizeof(cliaddr);
    for ( ; ; ) {
        len = clilen;
        n = sock_recvfrom(sockfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
        if(n > 0) {
            mesg[n] = '\0';
            log_d("RECV:%s",mesg);
            sock_sendto(sockfd, mesg, n, 0, (SA *) &cliaddr, len);
        } else {
            log_d("recvfrom() fail[%d].",errno);
        }
    }
}

int main(int argc,char *argv[])
{
    log_init(LOG_D, FALSE, NULL);

    main_1(argc,argv);
    return 0;
}
