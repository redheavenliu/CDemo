#include <stdio.h>

#include "m_stdio.h"
#include "m_type.h"
#include "m_sock.h"
#include "m_log.h"

#define COAP_CT_SERVER_HOST   "117.60.157.137"      // Server address
#define COAP_CT_SERVER_PORT   5683                  // Server port

static bool udp_connected = FALSE;

#define PSK "07e73955bcbe4ff9b4889d8ee118148c"
#define HWV "HW.L610.01"
#define SWV "SW.L610.01"


static char *hex_str = "40450bb8c0ffaa5501010e4142434445464748494a4b4c4d4e0a48572e4c3631302e30310a53572e4c3631302e3031";

static uint8_t StrToHex(char *pbDest, char *pbSrc, int nLen)
{
	char h1,h2;
	byte s1,s2;
	uint8_t i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];
		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
		{
			s1 -= 7;
		}
		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
		{
			s2 -= 7;
		}
		pbDest[i] = s1*16 + s2;
		//DS_MBTK_MSG1_HIGH("StrToHex:%.2X",pbDest[i]);
	}
	return i;
}

static void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
	char   ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57)
		{
			ddh = ddh + 7;
		}
		if (ddl > 57)
		{
			ddl = ddl + 7;
		}
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}
	pbDest[nLen*2] = '\0';
}


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

    if(sockaddr_init(&seraddr,COAP_CT_SERVER_HOST,COAP_CT_SERVER_PORT,AF_INET) < 0) {
        log_e("sockaddr_init() fail.[%d]",errno);
        return;
    }

    // "40450bb8c0 ff aa550101 0e4142434445464748494a4b4c4d4e0a48572e4c3631302e30310a53572e4c3631302e3031";

    int index = 0;
    char sendline[200];
    memset(sendline,0x0,200);
//    sendline[index++] = 0x40;
//    sendline[index++] = 0x02;
//    sendline[index++] = 0x0b;
//    sendline[index++] = 0xb8;
//    sendline[index++] = 0xc0;

//    sendline[index++] = 0xff;

    index = snprintf(sendline,200,"%s","{\"hex\":\"",strlen("{\"hex\":\""));
    sendline[index++] = 0xaa;
    sendline[index++] = 0x55;
    sendline[index++] = 0x01;
    sendline[index++] = 0x01;

    sendline[index++] = strlen(PSK);
    memcpy(sendline + index,PSK,strlen(PSK));
    index += strlen(PSK);

    sendline[index++] = strlen(HWV);
    memcpy(sendline + index,HWV,strlen(HWV));
    index += strlen(HWV);


    sendline[index++] = strlen(SWV);
    memcpy(sendline + index,SWV,strlen(SWV));
    index += strlen(SWV);

    sendline[index++] = '\"';
    sendline[index++] = '}';

    serlen = sizeof(struct sockaddr_in);

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

    index = 0;
    int i = 0;
    char buf_temp[100];
    while(index % 16 == 0 && index < strlen(sendline)){
        memset(buf_temp,0,100);
        for(i = 0;i < 16 && index + i < strlen(sendline);i++) {
            snprintf(buf_temp + i * 3,100 - i,"%.2x ",((char*)sendline)[index + i]);
            //log_d("%.2x ",((char*)sendline)[index + i]);
        }
        index += 16;
        log_d("%s", buf_temp);
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
            log_d("Wait for data...");
            n = read(sockfd, recvline, MAXLINE);
            //n = sock_recvfrom(sockfd, recvline, MAXLINE, 0, NULL,0);
            log_d("Get data...");
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


