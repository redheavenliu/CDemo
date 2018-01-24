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

#define	SA struct sockaddr
#define sta_log(fmt...) \
		pthread_mutex_lock(&mutex); \
        log_i(fmt); \
        pthread_mutex_unlock(&mutex);


static sta_session_s *session_list = NULL;
//static int session_count = 0;
pthread_mutex_t mutex;
static struct epoll_event epoll_events[20];
static int epoll_fd = -1;

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



static void list_deinit(sta_session_s *list) {

}

static sta_msg_q_s* msg_queue_pop(sta_session_s* session){
	//sta_log("start");
	sta_msg_q_s *msg = session->msg_q;
	session->msg_q = session->msg_q->next;
	msg->next = NULL;
	//sta_log("end");
	return msg;
}

static void msg_queue_put(sta_session_s* session,sta_msg_q_s *msg){
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

static void msg_queue_clear(sta_session_s* session){
	//sta_log("start");
	sta_msg_q_s *msg = session->msg_q;
	while(msg){
		session->msg_q = session->msg_q->next;
		free(msg);
		msg = session->msg_q;
	}
	session->msg_q = NULL;
	//sta_log("end");
}


static void session_reset(sta_session_s *session) {
	pthread_mutex_lock(&mutex);
	session->err = STA_SESSION_ERR_SUCCESS;
	session->state = STA_SESSION_STATE_UNUSED;

	memset(session->host,0,SESSION_HOST_MAX_LEN);
	session->port = -1;

	struct epoll_event ev;
    ev.data.fd = session->sockfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,session->sockfd,&ev);

	close(session->sockfd);
	session->sockfd = -1;

	msg_queue_clear(session);
	pthread_mutex_unlock(&mutex);
}


static bool session_add(sta_session_s *session) {
//	if(session_count >= SESSION_MAX_COUNT) {
//		return FALSE;
//	}

	session->pre = NULL;
	session->next = session_list;

	if(session_list != NULL) {
		session_list->pre = session;
	}
	session_list = session;

	//session_count++;
	return TRUE;
}

static bool session_del(sta_session_s *session) {
	if(session_list == NULL || session == NULL || session->session_id == -1)
		return FALSE;

	pthread_mutex_lock(&mutex);
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
			//session_count--;
			pthread_mutex_unlock(&mutex);
			return TRUE;
		}
		session_p = session_p->next;
	}
	pthread_mutex_unlock(&mutex);

	return FALSE;
}

static sta_session_s* session_get(int session_id) {
	//sta_log("start:%d",session_id);
	sta_session_s *session_p = session_list;
	while(session_p){
		if(session_id == session_p->session_id) {
			//sta_log("end:return not NULL");
			return session_p;
		}
		session_p = session_p->next;
	}

	//sta_log("end:return NULL");
	return NULL;
}

static int session_get_usable(void) {
	pthread_mutex_lock(&mutex);
	sta_session_s *session_p = session_list;
	while(session_p){
		if(session_p->state == STA_SESSION_STATE_UNUSED) {
			pthread_mutex_unlock(&mutex);
			return session_p->session_id;
		}
		session_p = session_p->next;
	}
	pthread_mutex_unlock(&mutex);
	return -1;
}


// Create session.The session number is SESSION_MAX_COUNT,
// and session ID is [0 - (SESSION_MAX_COUNT-1)]
static void session_list_create(){
	int i;

	pthread_mutex_init(&mutex, NULL);
	epoll_fd = epoll_create(256);

	sta_session_s *session;
	for(i = 0;i<SESSION_MAX_COUNT;i++){
		sta_session_s *session = (sta_session_s*)malloc(sizeof(sta_session_s));
		if(session_init(session,SESSION_MAX_COUNT - i - 1)){
			session_add(session);
		}
	}
}

static void send_msg(int session_id,sta_msg_id_enum msg_id,void *data,int len){
	//sta_log("start");
	sta_msg_q_s *msg = (sta_msg_q_s*)malloc(sizeof(sta_msg_q_s));
	memset(msg,0,sizeof(sta_msg_q_s));
	msg->msg_id = msg_id;
	msg->next = NULL;
	msg->data_len = len;
	if(data != NULL && len > 0)
		memcpy(msg->data,data,msg->data_len);
	msg_queue_put(session_get(session_id),msg);
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
		session->err = STA_SESSION_ERR_HOST;
		goto result_fail_with_close;
	}
	memcpy(&servaddr.sin_addr, he->h_addr, sizeof(struct in_addr));

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
			session->err = STA_SESSION_ERR_CONN;
			goto result_fail_with_close;
		}
	}

	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(session->sockfd, &rset);
	FD_SET(session->sockfd, &wset);
	struct timeval time_out;
	time_out.tv_sec = 10; // 10 s
	time_out.tv_usec = 0;
	int nready = select(session->sockfd + 1, &rset, &wset, NULL, &time_out);
	sta_log("nready = %d",nready);
	if(nready == 0){// Timeout
		sta_log("Timeout.");
		goto result_fail_with_close;
	}else{
		if (FD_ISSET(session->sockfd, &rset) && FD_ISSET(session->sockfd, &wset)) {
			int error = -1;
			int len = sizeof(int);
			sta_log("Can read and write.");
			if(getsockopt(session->sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
				sta_log("getsockopt fail.[%d]",errno);
				session->err = STA_SESSION_ERR_CONN;
				goto result_fail_with_close;
			}else{
				sta_log("error = %d",error);
				if(error == 0){ // Success
					goto result_success;
				}else{// Fail
					session->err = STA_SESSION_ERR_CONN;
					goto result_fail_with_close;
				}
			}
		}else if(FD_ISSET(session->sockfd, &wset)){
			sta_log("Can write.");
			goto result_success;
		}else{
			sta_log("Can read(Impossible).");
			session->err = STA_SESSION_ERR_CONN;
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

	struct epoll_event ev;
    ev.data.fd = session->sockfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,session->sockfd,&ev);

	sta_log("end:success");
	return;
result_fail_with_close:
result_fail:
	sta_log("end:fail");
	session->state = STA_SESSION_STATE_CLOSED;
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
		msg = msg_queue_pop(session);
		sta_log("Thread(%ld) run:msg:%d:%s",session->thread_id,msg->msg_id,msg->data);
		switch(msg->msg_id){
			case STA_MSG_SOCKET_OPEN:{
				int i = 0;
				while(msg->data[i] != ':')
					i++;
				//sta_log("index = %d",i);
				memcpy(session->host,msg->data,i);
				session->port = (int)atoi((msg->data) + i + 1);
				if(session->port > 0)
					socket_open(session);

				// Open session success
				if(session->state == STA_SESSION_STATE_OPENED){
					qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_OPEN,
						STA_SESSION_ERR_SUCCESS,
						NULL,
						0);
				}else{ // Open session fail
					qmi_session_callback(-1,
						STA_MSG_SOCKET_OPEN,
						session->err,
						NULL,
						0);
					session_reset(session);
				}
				break;
			}
			case STA_MSG_SOCKET_WRITE:{
				int len = 0;
				int count = 0;
				while(count < msg->data_len){
					len = write(session->sockfd,msg->data + count,msg->data_len - count);
					if(len < 0){
						if(errno == EWOULDBLOCK){
							usleep(20);
							continue;
						}else{
							sta_log("write error.[%d]",errno);
							session->err = STA_SESSION_ERR_UNKNOWN;
							break;
						}
					}else if(len == 0){
						sta_log("write error(len == 0).[%d]",errno);
					}else{
						count += len;
					}
				}

				if(count == msg->data_len){
					qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_WRITE,
						STA_SESSION_ERR_SUCCESS,
						NULL,
						count);
				}else{ // Open session fail
					qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_WRITE,
						session->err,
						NULL,
						count);
				}
				break;
			}
			case STA_MSG_SOCKET_READ:{
				int len = 0;
				int count = 0;
				char *buf = (char*)malloc(sizeof(char) * (msg->data_len + 1));
				while(count < msg->data_len){
					len = read(session->sockfd,buf + count,msg->data_len - count);
					if(len < 0){
						if(errno == EWOULDBLOCK){
							usleep(20);
							continue;
						}else{
							sta_log("read error.[%d]",errno);
							session->err = STA_SESSION_ERR_UNKNOWN;
							break;
						}
					}else if(len == 0){
						sta_log("read error(len == 0).[%d]",errno);
						break;
					}else{
						count += len;
					}
				}

				buf[count] = '\0';

				if(count == msg->data_len){
					qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_READ,
						STA_SESSION_ERR_SUCCESS,
						buf,
						count);
				}else{ // Open session fail
					qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_READ,
						session->err,
						buf,
						count);
				}

				break;
			}
			case STA_MSG_SOCKET_CLOSE:{
				qmi_session_callback(session->session_id,
						STA_MSG_SOCKET_CLOSE,
						STA_SESSION_ERR_SUCCESS,
						NULL,
						0);
				session_reset(session);
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

/**
* Init session by host and port,and return session ID.
*/
int stanet_session_open(const char *host,int port){
	//sta_log("start");
	if(host == NULL || port <= 0
		|| strlen(host) <= 0
		|| strlen(host) > SESSION_HOST_MAX_LEN) {
		return -1;
	}

	int session_id = session_get_usable();
	//sta_log("session_id = %d",session_id);
	if(session_id < 0){
		sta_log("No usable session...");
		return -1;
	}

	char str[SESSION_HOST_MAX_LEN + 5];
	int len = snprintf(str,SESSION_HOST_MAX_LEN + 5,
					"%s:%d",host,port);
	str[len] = '\0';

	send_msg(session_id, STA_MSG_SOCKET_OPEN, str,len);

	return 0;
}


int stanet_session_write(int session_id,void *buf,int len){
	//sta_log("session_id = %d",session_id);
	sta_session_s *session = session_get(session_id);
	if(session == NULL || (session->state != STA_SESSION_STATE_OPENED
		&& session->state != STA_SESSION_STATE_WRITING
		&& session->state != STA_SESSION_STATE_READING)) {
		sta_log("Session error,can not write.");
		return -1;
	}

	session->state = STA_SESSION_STATE_WRITING;

	send_msg(session_id, STA_MSG_SOCKET_WRITE, buf,len);

	return 0;
}

int stanet_session_read(int session_id,void *buf,int len){
	sta_session_s *session = session_get(session_id);
	if(session == NULL || (session->state != STA_SESSION_STATE_OPENED
		&& session->state != STA_SESSION_STATE_WRITING
		&& session->state != STA_SESSION_STATE_READING)) {
		sta_log("Session error,can not read.");
		return -1;
	}

	session->state = STA_SESSION_STATE_READING;

	send_msg(session_id, STA_MSG_SOCKET_READ, NULL,len);

	return 0;
}

int stanet_session_close(int session_id){
	sta_session_s *session = session_get(session_id);
	if(session == NULL || (session->state != STA_SESSION_STATE_OPENED
		&& session->state != STA_SESSION_STATE_WRITING
		&& session->state != STA_SESSION_STATE_READING)) {
		sta_log("Session error,can not read.");
		return -1;
	}

	session->state = STA_SESSION_STATE_CLOSING;

	send_msg(session_id, STA_MSG_SOCKET_READ, NULL,0);

	return 0;
}

int main (int argc, char **argv)
{
	log_i("start");

	init();

	int nready;
    while (TRUE) {
        nready = epoll_wait(epoll_fd,epoll_events,20,-1);
        for(int i=0;i<nready;++i) {
            if ((epoll_events[i].events & EPOLLIN) && (epoll_events[i].events & EPOLLOUT)) {
                sta_log("%d can read and write.",epoll_events[i].data.fd);
				int error = -1;
				int len = sizeof(int);
				if(getsockopt(epoll_events[i].data.fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0){
					sta_log("getsockopt fail.[%d]",errno);
				}else{
					sta_log("error = %d",error);
				}
                //ev.data.fd=sockfd;
                //ev.events=EPOLLOUT|EPOLLET;
                //epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            } else if (epoll_events[i].events & EPOLLOUT) {
                sta_log("%d can write.",epoll_events[i].data.fd);
//                ev.data.fd=sockfd;
//                ev.events=EPOLLIN|EPOLLET;
//                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            } else if (epoll_events[i].events & EPOLLIN) {
                sta_log("%d can read.",epoll_events[i].data.fd);
                //ev.data.fd=sockfd;
                //ev.events=EPOLLOUT|EPOLLET;
                //epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            } else {
				sta_log("%d can net read and write.",epoll_events[i].data.fd);
            }
        }
    }

    return 0;
}
