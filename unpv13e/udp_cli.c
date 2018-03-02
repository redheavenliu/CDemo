#include <stdio.h>

#include "m_stdio.h"
#include "m_type.h"
#include "m_sock.h"
#include "m_log.h"

#define SERVER_UDP_PORT 9877

static void main_1(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in seraddr;
    socklen_t len;
    int n;
    int serlen;
    char recvline[MAXLINE + 1];
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0) {
        log_e("socket() fail.[%d]",errno);
        return;
    }

    if(sockaddr_init(&seraddr,argv[1],SERVER_UDP_PORT,AF_INET) < 0) {
        log_e("sockaddr_init() fail.[%d]",errno);
        return;
    }

    serlen = sizeof(struct sockaddr_in);
    char *sendline = "1234567tetegger";
    while(TRUE) {
        n = sock_sendto(sockfd, sendline, strlen(sendline), 0, &seraddr, serlen);
        if(n < 0) {
            log_e("sendto() fail[%d].",errno);
        }
        sleep(1);
        n = sock_recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = '\0';	/* null terminate */
        log_d("RECV:%s",recvline);
        sleep(1);
    }
}

int main(int argc,char *argv[])
{
    log_init(LOG_D, FALSE, NULL);

    main_1(argc,argv);
    return 0;
}

