#include "m_stdio.h"
#include "m_sock.h"
#include "m_wrap.h"

#define SERVER_PORT 8888

void
sig_alrm(int signo)
{
    printf("Timeout.\n");
    exit(0);
}

void
sig_pipe(int signo)
{
	printf("SIGPIPE received\n");
	return;
}

int main_1(int argc, char **argv)
{
    int sock_fd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: tcpcli <IPaddress>");

    sock_fd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    // 设置超时
    /* Set interval timer to go off before 3WHS completes */
    Signal(SIGALRM, sig_alrm);
    struct itimerval	val;
    val.it_interval.tv_sec  = 0;  // 重复间隔
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec  = 10;  // 开始时间
    val.it_value.tv_usec = 500000;	/* 500 ms */
    // 3.5s 超时
    if (setitimer(ITIMER_REAL, &val, NULL) == -1)
    	err_sys("setitimer error");

    Connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));

    // 取消定时器
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_REAL, &value, NULL);

    struct sockaddr_in addr_cli,addr_ser;
    socklen_t serv_len = sizeof(addr_ser);
    socklen_t cli_len = sizeof(addr_cli);

    // 得到 fd 对端IP和Port
    if(getpeername(sock_fd, (SA *) &addr_ser, &serv_len) < 0){
        err_quit("getsockname fail.");
    }
    printf("Connected to [%s]",Sock_ntop((SA *) &addr_ser, sizeof(addr_ser)));

    // 得到 fd 本端IP和Port
    if(getsockname(sock_fd, (SA *) &addr_cli, &cli_len) < 0){
        err_quit("getsockname fail.");
    }
    printf(" from [%s]\n",Sock_ntop((SA *) &addr_cli, sizeof(addr_cli)));

    char	sendline[MAXLINE], recvline[MAXLINE];
    Fputs("<<", stdout);
    while (Fgets(sendline, MAXLINE, stdin) != NULL) {
       char *ptr = sendline + strlen(sendline) - 1;
       while(*ptr == '\r' || *ptr == '\n'){
            *ptr-- = '\0';
       }

       //printf("line = \"%s\"\n",sendline);
       if(strcmp(sendline,"exit")==0){
            break;
       }
    	Writen(sock_fd, sendline, strlen(sendline));
    	if (Readline(sock_fd, recvline, MAXLINE) == 0)
    		err_quit("str_cli: server terminated prematurely");
       Fputs(">>", stdout);
    	Fputs(recvline, stdout);
       Fputs("<<", stdout);
    }

    struct linger ling;
    ling.l_onoff = 1;
    ling.l_linger = 10000;
    Setsockopt(sock_fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

    Close(sock_fd);

    exit(0);
}

int
main_2(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);		/* daytime server */
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Signal(SIGPIPE, sig_pipe);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	sleep(2);
	Write(sockfd, "hello", 5);  // server socket已关闭，会收到信号  SIGPIPE
       printf("Write : hello.\n");
	sleep(2);
	Write(sockfd, "world", 5); // 再写会报错 : Broken pipe
       printf("Write : world.\n");

	exit(0);
}

int
main_3(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);		/* daytime server */
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

        char	sendline[MAXLINE], recvline[MAXLINE];
        Fputs("<<", stdout);
        while (Fgets(sendline, MAXLINE, stdin) != NULL) {
           char *ptr = sendline + strlen(sendline) - 1;
           while(*ptr == '\r' || *ptr == '\n'){
                *ptr-- = '\0';
           }

           //printf("line = \"%s\"\n",sendline);
           if(strcmp(sendline,"exit")==0){
                break;
           }
        	Writen(sockfd, sendline, strlen(sendline));
        	if (Readline(sockfd, recvline, MAXLINE) == 0)
        		err_quit("str_cli: server terminated prematurely");
           Fputs(">>", stdout);
        	Fputs(recvline, stdout);
           Fputs("<<", stdout);
        }

	exit(0);
}

int
main_4(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);		/* daytime server */
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

        char	sendline[MAXLINE], recvline[MAXLINE];
        Fputs("<<", stdout);
        while (Fgets(sendline, MAXLINE, stdin) != NULL) {
           char *ptr = sendline + strlen(sendline) - 1;
           while(*ptr == '\r' || *ptr == '\n'){
                *ptr-- = '\0';
           }

           //printf("line = \"%s\"\n",sendline);
           if(strcmp(sendline,"exit")==0){
                break;
           }
        	Writen(sockfd, sendline, strlen(sendline));
        	if (Readline(sockfd, recvline, MAXLINE) == 0)
        		err_quit("str_cli: server terminated prematurely");
           Fputs(">>", stdout);
        	Fputs(recvline, stdout);
           Fputs("<<", stdout);
        }

	exit(0);
}

int main(int argc, char **argv)
{
    //return main_1(argc, argv);
    //return main_2(argc, argv);
    //return main_3(argc, argv);
    return main_4(argc, argv);
}