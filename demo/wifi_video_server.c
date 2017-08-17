#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include "m_log.h"
#include "m_type.h"

#define VIDEO_COLLECT_FROM_NET

#define VIDEO_CLIENT_MAX 2
#define EPOLL_FDS_MAX (VIDEO_CLIENT_MAX + 1)   // Client fds and Server fd.

#define SERVER_PORT 8888
#define	LISTENQ		1024
#define BUF_SIZE 2048
#define	SA struct sockaddr

static struct epoll_event epoll_fds[EPOLL_FDS_MAX];
static int client_fds[VIDEO_CLIENT_MAX];
static pthread_t video_pid = -1;
static bool video_collect_running = FALSE;

#ifdef VIDEO_COLLECT_FROM_NET
static int video_collect_fd = -1;
static pthread_cond_t cond;
static pthread_mutex_t mutex;
#endif

static int
video_send_data
(
	char *data
)
{
	int i;
	for(i = 0;i < VIDEO_CLIENT_MAX;i++){
		if(client_fds[i] > 0){
			int result = write(client_fds[i], data, strlen(data));
			if(result == strlen(data)){
				log_d("(fd:%d)Write data success.",client_fds[i]);
			}else{
				if(EBADF == errno){
					log_w("%d disconnected.",client_fds[i]);
					//client_fds[i] = -1;
					int j = VIDEO_CLIENT_MAX - 1;
					for(;j > i;j--){
						if(client_fds[j] > 0){
							client_fds[i] = client_fds[j];
							client_fds[j] = -1;
							break;
						}
					}
					if(i == j){ // This is the last client index.
						client_fds[i] = -1;
					}
					i--; // This is replaced by other.
					continue;
				}else{
					log_e("errno:%d",errno);
					return -1;
				}
			}
		}else{
			break;
		}
	}
	return 0;
}

static void *
video_thread_run
(
	void * arg
)
{
	log_d("video_thread_run start.");
	int i = 0;
	char buf[BUF_SIZE];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	while(video_collect_running){
		if(video_collect_fd == -1){ // Wait for sig
			struct timeval now;
			struct timespec outtime;
			pthread_mutex_lock(&mutex);
			gettimeofday(&now, NULL);
    		outtime.tv_sec = now.tv_sec + 60; // 60s
    		outtime.tv_nsec = now.tv_usec * 1000;
			log_w("No video collect device.Will wait 60 s");
    		pthread_cond_timedwait(&cond, &mutex, &outtime);
			pthread_mutex_unlock(&mutex);
		}

		if(video_collect_fd == -1){
			log_w("No video collect device.");
			continue;
		}



		int size = read(video_collect_fd,buf,BUF_SIZE);
		buf[size] = '\0';
		if(size > 0){
			video_send_data(buf);
		}else if(size == 0){
			log_w("Video collect device disconnected.");
			video_collect_fd = -1;
		}else{
			log_e("read fail[%d],errno = %d",size,errno);
		}

		sleep(1);

	}
	log_d("video_thread_run end.");
}


static void
video_server_init
(
	void
)
{
	log_d("video_server_init start.");

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd < 0){
		log_e("socket() fail[%d][%s].",errno,strerror(errno));
		return;
	}

	int epfd = epoll_create(256);
	struct epoll_event ev;
	ev.data.fd = server_fd;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&ev);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family 	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port		 = htons(SERVER_PORT);

	if(bind(server_fd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		log_d("errno = %d(%s)",errno,strerror(errno));
		return;
	}

	if (listen(server_fd, LISTENQ) < 0){
		log_d("errno = %d(%s)",errno,strerror(errno));
		return;
	}

	int nready;
	struct sockaddr_in cliaddr;
	char buf[BUF_SIZE];

	int i;
	for(i = 0;i < VIDEO_CLIENT_MAX;i++){
		client_fds[i] = -1;
	}

	log_d("Server init success.");

	video_collect_running = TRUE;
    if (0 != pthread_create(&video_pid, NULL, video_thread_run, NULL))
    {
        log_e("error when create pthread.\n");
        return -1;
    }

	socklen_t clilen;
	int sockfd;
	for ( ; ; ) {
		// Event number.
		nready = epoll_wait(epfd,epoll_fds,EPOLL_FDS_MAX,-1);
		for(i = 0;i < nready; ++i) {
			// New connection.
			if(epoll_fds[i].data.fd == server_fd) { // Server socket event.
				clilen = sizeof(cliaddr);
				int connfd = accept(server_fd, (SA *) &cliaddr, &clilen);

				char ip[20];
				Inet_ntop(AF_INET, &cliaddr.sin_addr, ip, sizeof(ip));
				int port = ntohs(cliaddr.sin_port);

				log_d("[%s:%d] connected.\n",ip,port);

				ev.data.fd = connfd;
				ev.events = EPOLLIN|EPOLLET;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
			}
			else if (epoll_fds[i].events & EPOLLIN) { // Has client socket writed data.
				log_d("Has data should read.");
				if ( (sockfd = epoll_fds[i].data.fd) < 0)
					continue;
				int n;
				if ( (n = read(sockfd, buf, BUF_SIZE)) < 0) {
					if (errno == ECONNRESET) {
						/*4connection reset by client */
						close(sockfd);
						epoll_fds[i].data.fd = -1;
					} else{
						log_e("read error");
						break;
					}
				} else if (n == 0) {
					/*4connection closed by client */
					close(sockfd);
					epoll_fds[i].data.fd = -1;
				} else {
					buf[n] = '\0';
					log_d("client:%s\n",buf);
					if(strncmp(buf,"CMD:VIDEO_GET",13) == 0){
						int j = 0;
						for(;j < VIDEO_CLIENT_MAX;j++){
							if(client_fds[j] == -1){
								client_fds[j] = sockfd;
								break;
							}
						}

						if(j == VIDEO_CLIENT_MAX){ // Client too more.
							if(8 == write(sockfd, "CMD:FAIL", 8)){
								log_d("Client(%d) connected fail beause of client too more.",sockfd);
							}else{
								log_d("Client(%d) connected fail.",sockfd);
							}
						}else{
							if(11 == write(sockfd, "CMD:SUCCESS", 11)){
								log_d("Client(%d) connected success.",sockfd);
							}else{
								log_d("Client(%d) connected fail.",sockfd);
							}
						}
					}
#ifdef VIDEO_COLLECT_FROM_NET
					else if(strncmp(buf,"CMD:VIDEO_COLLECT_REG",21) == 0)
					{
						if(video_collect_fd == -1){
							video_collect_fd = sockfd;
							// Remove from epoll.
							ev.data.fd = sockfd;
							ev.events = EPOLLIN|EPOLLET;
							epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&ev);

							if(11 == write(sockfd, "CMD:SUCCESS", 11)){
								log_d("Video collect register success.");
								pthread_mutex_lock(&mutex);
								pthread_cond_signal(&cond);
								pthread_mutex_unlock(&mutex);
							}else{
								log_d("Video collect register fail.");
							}
						}else{
							if(8 == write(sockfd, "CMD:FAIL", 8)){
								log_d("Video collect(%d) has exist.");
							}else{
								log_d("Video collect register fail.");
							}
						}
					}
#endif
					else
					{
						log_w("Other data.");
					}
				}

//				//设置用于写操作的文件描述符
//				//ev.data.fd=sockfd;
//				//设置用于注测的写操作事件
//				//ev.events=EPOLLOUT|EPOLLET;
//				//修改sockfd上要处理的事件为EPOLLOUT
//				//epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
			}
			else if (epoll_fds[i].events & EPOLLOUT) {// 如果有数据发送
//				sockfd = video_clients[i].data.fd;
//				writen(sockfd, buf, n);
//				//设置用于读操作的文件描述符
//				ev.data.fd=sockfd;
//				//设置用于注测的读操作事件
//				ev.events=EPOLLIN|EPOLLET;
//				//修改sockfd上要处理的事件为EPOLIN
//				epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
				log_d("Has data should send.");
			}
		}
	}
}

int main
(
	int argc,
	char *argv[]
)
{
	log_init(LOG_D, FALSE, NULL);

	video_server_init();

	return 0;
}

