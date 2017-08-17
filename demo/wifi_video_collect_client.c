#include "m_sock.h"
#include "m_stdio.h"

#include "m_log.h"
#include "m_type.h"


#define SERVER_IP "192.168.1.109"
#define SERVER_PORT 8888


int main(int arg, char * args [])
{
	log_init(LOG_D, FALSE, NULL);


	int					sockfd;
	struct sockaddr_in	servaddr;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);		/* daytime server */
	Inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    char buffer[MAXLINE];
    //Fputs("<<", stdout);

	log_d("Connected to server.");

	if(21 == write(sockfd,"CMD:VIDEO_COLLECT_REG",21)){
		int len = read(sockfd,buffer,MAXLINE);
		if(strncmp(buffer,"CMD:SUCCESS",11) == 0){
			while(1){ // Send data
				log_d("sockfd = %d",sockfd);
				len = write(sockfd,"data:xxx",8);
				log_d("write len = %d",len);
				if(len == 8){
					log_d("write data success.");
				}else{
					log_e("write data fail(%d).",errno);
					break;
				}

				sleep(2);
			}
		}else{
			log_e("Client too more.");
		}
	}else{
		log_e("Send cmd error.");
	}

	log_d("exit");

	return 0;
}

