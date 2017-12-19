#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/epoll.h>


#include "stanet_type.h"
#include "stanet_qmi.h"

#define sta_log(fmt...) \
		pthread_mutex_lock(&mutex); \
        log_i(fmt); \
        pthread_mutex_unlock(&mutex);

#define	SA struct sockaddr

static sta_session_s *session_list = NULL;
static int session_count = 0;
static pthread_mutex_t mutex;

static void* thread_run(void *data);


static bool session_init(sta_session_s *session,int id) {
	session->session_id = id;
	session->sockfd = -1;
	session->err = STA_SESSION_ERR_SUCCESS;
	session->state = STA_SESSION_STATE_UNUSED;

	memset(session->host,0,SESSION_HOST_MAX_LEN);
	session->port = -1;

	session->pre = NULL;
	session->next = NULL;

	pthread_mutex_init(&(session->mutex), NULL);
    pthread_cond_init(&(session->cond), NULL);
	session->thread_running = TRUE;
	session->msg_q = NULL;
	if (0 != pthread_create(&(session->thread_id), NULL, thread_run, session))
    {
        sta_log("error when create pthread,%d\n", errno);
		return FALSE;
    }
	return TRUE;
}

static void session_reset(sta_session_s *session) {
	session->sockfd = -1;
	session->err = STA_SESSION_ERR_SUCCESS;
	session->state = STA_SESSION_STATE_UNUSED;

	memset(session->host,0,SESSION_HOST_MAX_LEN);
	session->port = -1;
	return TRUE;
}


static void list_deinit(sta_session_s *list) {

}

static sta_msg_q_s* msg_queue_delete(sta_session_s* session){
	sta_log("start");
	sta_msg_q_s *msg = session->msg_q;
	session->msg_q = session->msg_q->next;
	msg->next = NULL;
	sta_log("end");
	return msg;
}

static void msg_queue_insert(sta_session_s* session,sta_msg_q_s *msg){
	//sta_log("start");
    pthread_mutex_lock(&(session->mutex));
	msg->next = NULL;
	if(session->msg_q == NULL){
		session->msg_q = msg;
	}else{
		sta_msg_q_s *p = session->msg_q;
		while(p->next){
			p = p->next;
		}
		p->next = msg;
	}
	pthread_cond_signal(&(session->cond));
	pthread_mutex_unlock(&(session->mutex));
	//sta_log("end");
}

static bool session_add(sta_session_s *session) {
	if(session_count >= SESSION_MAX_COUNT) {
		return FALSE;
	}

	session->pre = NULL;
	session->next = session_list;

	if(session_list != NULL) {
		session_list->pre = session;
	}
	session_list = session;

	session_count++;
	return TRUE;
}

static bool session_del(sta_session_s *session) {
	if(session_list == NULL || session == NULL || session->session_id == -1)
		return FALSE;

	sta_session_s *session_p = session_list;
	while(session_p){
		if(session->session_id == session_p->session_id) {
			if(session_p->pre != NULL && session_p->next != NULL){
				session_p->pre->next = session_p->next;
				session_p->next->pre = session_p->pre;
			}else if(session_p->pre == NULL && session_p->next == NULL){ // Head and tail
				session_list = NULL;
			}else if(session_p->pre == NULL){// Head
				session_list = session_p->next;
				session_list->pre = NULL;
			}else{// Tail
				session_p->pre->next = NULL;
			}

			free(session_p);
			session_p = NULL;
			session_count--;
			return TRUE;
		}
		session_p = session_p->next;
	}

	return FALSE;
}

static sta_session_s* session_get(int session_id) {
	sta_session_s *session_p = session_list;
	while(session_p){
		if(session_id == session_p->session_id) {
			return session_p;
		}
		session_p = session_p->next;
	}

	return NULL;
}

static void session_list_create(){
	int i;

	pthread_mutex_init(&mutex, NULL);

	sta_session_s *session;
	for(i = 0;i<SESSION_MAX_COUNT;i++){
		sta_session_s *session = (sta_session_s*)malloc(sizeof(sta_session_s));
		if(session_init(session,i)){
			session_add(session);
		}
	}
}

static void send_msg(int session_id,sta_msg_id_enum msg_id,void *data){
	//sta_log("start");
	sta_msg_q_s *msg = (sta_msg_q_s*)malloc(sizeof(sta_msg_q_s));
	memset(msg,0,sizeof(sta_msg_q_s));
	msg->msg_id = msg_id;
	msg->next = NULL;
	msg->data_len = strlen((char*)data);
	memcpy(msg->data,data,msg->data_len);

	msg_queue_insert(session_get(session_id),msg);
	//sta_log("end");
}


static void init() {
	session_list_create();

	// Communicate with the modem.
	stanet_qmi_init();

	// Communicate with the STA process.
	// sta_sock_init();
}

//static void
//sig_alrm(int signo)
//{
//    sta_log("Timeout.");
//}

static void socket_open(sta_session_s *session){
	//sta_log("start");
	session->state = STA_SESSION_STATE_OPENING;
	struct sockaddr_in servaddr;
	if((session->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		sta_log("socket() fail.[%d]",errno);
		session->sockfd = -1;
		goto result_fail;
	}

	int flags = fcntl(session->sockfd, F_GETFL, 0);
	if (flags < 0) {
		sta_log("Get flags error:%s\n", strerror(errno));
		goto result_fail_with_close;
	}
	flags |= O_NONBLOCK;
	if (fcntl(session->sockfd, F_SETFL, flags) < 0) {
		sta_log("Set flags error:%s\n", strerror(errno));
		goto result_fail_with_close;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(session->port);

	if (inet_aton(session->host, &servaddr.sin_addr) < 0)
	{
		sta_log("inet_aton() fail.[%d]",errno);
		goto result_fail_with_close;
	}

	struct hostent *he = gethostbyname(session->host);
	if (he == NULL){
		sta_log("gethostbyname() fail.[%d]",errno);
		goto result_fail_with_close;
	}
	memcpy(&servaddr.sin_addr, he->h_addr, sizeof(struct in_addr));

	sleep(5);

	/* Set interval timer to go off before 3WHS completes */
	//signal(SIGALRM, sig_alrm);

//    struct sigaction act,oldact;
//	act.sa_handler = sig_alrm;
//    sigemptyset(&act.sa_mask);
//    sigaddset(&act.sa_mask, SIGALRM);
//    act.sa_flags = SA_INTERRUPT;
//    sigaction(SIGALRM, &act, &oldact);

//	struct itimerval	val,old;
//	val.it_interval.tv_sec	= 0;
//	val.it_interval.tv_usec = 0;
//	val.it_value.tv_sec  = 5;	/* 5 s */
//	val.it_value.tv_usec = 0;	/* 0 us */
//	// 15s Timeout
//	if (setitimer(ITIMER_REAL, &val, &old) == -1){
//		sta_log("setitimer() fail.[%d]",errno);
//		goto result_fail_with_close;
//	}

//	sta_log("%ld,%ld,%ld,%ld",old.it_interval.tv_sec,old.it_interval.tv_usec,
//		old.it_value.tv_sec,old.it_value.tv_usec);

	if(connect(session->sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		if(EINPROGRESS != errno){
			sta_log("connect() fail.[%d]",errno);
			goto result_fail_with_close;
		}
	}

//	struct itimerval value;
//	value.it_value.tv_sec = 0;
//	value.it_value.tv_usec = 0;
//	value.it_interval = value.it_value;
//	setitimer(ITIMER_REAL, &value, NULL);

//	struct sockaddr_in addr_cli,addr_ser;
//	socklen_t serv_len = sizeof(addr_ser);
//	socklen_t cli_len = sizeof(addr_cli);

//	if(getpeername(session->sockfd, (SA *) &addr_ser, &serv_len) < 0){
//		sta_log("getpeername() fail.[%d]",errno);
//		goto result_fail_with_close;
//	}
//	sta_log("Connected to [%s]",sock_ntop((SA *) &addr_ser, sizeof(addr_ser)));

//	if(getsockname(session->sockfd, (SA *) &addr_cli, &cli_len) < 0){
//		sta_log("getsockname() fail.[%d]",errno);
//		goto result_fail_with_close;
//	}
//	sta_log("From [%s]",sock_ntop((SA *) &addr_cli, sizeof(addr_cli)));

result_success:
	session->state = STA_SESSION_STATE_OPENED;

	sta_log("end:success");
	return;
result_fail_with_close:
	close(session->sockfd);
result_fail:
	session_reset(session);
	sta_log("end:fail");
	return;
}

static void* thread_run(void *data){
	sta_session_s *session = (sta_session_s *)data;

	session->thread_id = pthread_self();

	pthread_mutex_lock(&(session->mutex));
	sta_msg_q_s *msg = NULL;
    while (session->thread_running)
    {
		sta_log("Session(%d) thread(%ld) wait ...",session->session_id,session->thread_id);
        pthread_cond_wait(&(session->cond), &(session->mutex));
		msg = msg_queue_delete(session);
		sta_log("Thread(%ld) run:msg:%d:%s",session->thread_id,msg->msg_id,msg->data);
		switch(msg->msg_id){
			case STA_MSG_SOCKET_OPEN:{
				int i = 0;
				while(msg->data[i] != ':')
					i++;
				sta_log("index = %d",i);
				if(i > 0 && i < SESSION_HOST_MAX_LEN){
					memcpy(session->host,msg->data,i);
					session->port = (int)atoi((msg->data) + i + 1);
					if(session->port > 0)
						socket_open(session);
				}
				break;
			}
			case STA_MSG_SOCKET_WRITE:{

				break;
			}
			case STA_MSG_SOCKET_READ:{

				break;
			}
			case STA_MSG_SOCKET_CLOSE:{

				break;
			}
			default:{

				break;
			}
		}
    }
    pthread_mutex_unlock(&(session->mutex));
	return ((void*)0);
}


int session_open(const char *host,int port){
	if(session_count >= SESSION_MAX_COUNT) {
		return -1;
	}

	if(host == NULL || port <= 0) {
		return -1;
	}





	return -1;
}

int main (int argc, char **argv)
{
	log_i("start");

	init();

	sleep(1);

//	send_msg(4, STA_MSG_SOCKET_OPEN, "www.baidu.com:80");

//	send_msg(2, STA_MSG_SOCKET_OPEN, "www.test.com:8081");

//	send_msg(1, STA_MSG_SOCKET_READ, "Read:HTTP/1.1 OK ...");

//	send_msg(1, STA_MSG_SOCKET_CLOSE, "Close:...");


	send_msg(2, STA_MSG_SOCKET_OPEN, "www.baidu.com:80");

	sleep(3);

	int fd = session_get(2)->sockfd;
	struct epoll_event ev,events[20];
    int epfd = epoll_create(256);
    ev.data.fd = fd;
    ev.events = EPOLLOUT | EPOLLIN | EPOLLET;
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);

	sta_log("fd = %d",fd);
	int nready;

    while (TRUE) {
        nready = epoll_wait(epfd,events,20,-1);
        for(int i=0;i<nready;++i) {
            if (events[i].events & EPOLLIN) {
                sta_log("%d can read.",events[i].data.fd);
                //ev.data.fd=sockfd;
                //ev.events=EPOLLOUT|EPOLLET;
                //epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            } else if (events[i].events & EPOLLOUT) {
                sta_log("%d can write.",events[i].data.fd);
				int error = -1;
				getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &error, sizeof(int));
				sta_log("error = %d",error);

//                ev.data.fd=sockfd;
//                ev.events=EPOLLIN|EPOLLET;
//                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
        }
    }

    return 0;
}
