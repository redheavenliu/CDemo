#include <stdio.h>

#include "m_stdio.h"
#include "m_type.h"
#include "m_sock.h"
#include "m_log.h"

#define SERVER_UDP_PORT 9877

static bool udp_connected = TRUE;

static void main_1(int argc,char *argv[])
{
    int sockfd;
    struct sockaddr_in seraddr,recv_seraddr;
    socklen_t recv_len;
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

    if(udp_connected) {
        if(connect(sockfd, &seraddr, serlen) < 0) {
            log_e("connect() fail.[%d]",errno);
            return;
        }

        // 得到本地IP及端口
        struct sockaddr_in cliaddr;
        socklen_t cliaddr_len = sizeof(cliaddr);
        if(getsockname(sockfd,&cliaddr, &cliaddr_len) < 0) {
            log_e("getsockname() fail.[%d]",errno);
            return;
        }

        log_d("cliaddr = %s",sock_ntop(&cliaddr,cliaddr_len));
    }

    recv_len = serlen;
    while(TRUE) {
        if(udp_connected) {
            //n = write(sockfd, sendline, strlen(sendline));
            n = sock_sendto(sockfd, sendline, strlen(sendline), 0, NULL, 0);
        } else {
            n = sock_sendto(sockfd, sendline, strlen(sendline), 0, &seraddr, serlen);
        }
        if(n < 0) {
            log_e("sendto() fail[%d].",errno);
        } else {
            log_d("send success.[len = %d]",n);
        }
        sleep(1);

        if(udp_connected) {
            //n = read(sockfd, recvline, MAXLINE);
            n = sock_recvfrom(sockfd, recvline, MAXLINE, 0, NULL,0);
            if(n <= 0) {
                log_d("read fail.[%d]",errno);
                if(errno == ECONNREFUSED) {
                    log_e("Disonnected.");
                    break;
                } else {
                    continue;
                }
            }

        } else {
            n = sock_recvfrom(sockfd, recvline, MAXLINE, 0, &recv_seraddr, &recv_len);

            // 如果不是指定server返回的数据则忽略
            if(serlen != recv_len || memcmp(&seraddr,&recv_seraddr,recv_len) != 0) {
                log_d("Server error.[len = %d:%d;send:%x;recv:%x]",
                    serlen,(int)recv_len,
                    &seraddr,
                    &recv_seraddr);
                continue;
            }

            if(n <= 0) {
                log_d("sock_recvfrom fail.[%d]",errno);
                continue;
            }
        }

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

