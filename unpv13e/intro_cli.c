#include "m_stdio.h"
#include "m_sock.h"
#include "m_wrap.h"

int
main_v4(int argc, char **argv)
{
	int					sockfd, n, counter = 0;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

       sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(4444);	/* daytime server */
	Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		counter++;
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	printf("counter = %d\n", counter);
	exit(0);
}

int
main_v6(int argc, char **argv)
{
	int					sockfd, n;
	struct sockaddr_in6	servaddr;
	char				recvline[MAXLINE + 1];

      sockfd = Socket(AF_INET6, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_port   = htons(6666);	/* daytime server */

	Inet_pton(AF_INET6, argv[2], &servaddr.sin6_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	exit(0);
}

int
main(int argc, char **argv)
{
    if (argc != 3)
        err_quit("usage: a.out <v4/v6> <IPaddress>");
    if(!strcmp(argv[1],"v4")){
        return main_v4(argc,argv);
    }else if(!strcmp(argv[1],"v6")){
        return main_v6(argc,argv);
    }else{
        err_quit("usage: a.out <v4/v6>");
    }
    return 0;
}