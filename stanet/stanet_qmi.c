#include <stdio.h>
#include <pthread.h>

#include "stanet_qmi.h"

static pthread_t thread_id;

#define sta_log(fmt...) \
		pthread_mutex_lock(&mutex); \
        log_i(fmt); \
        pthread_mutex_unlock(&mutex);

extern pthread_mutex_t mutex;




static char *http_header = "GET /httpdemo/http HTTP/1.1\r\nHost: 192.168.1.224\r\n\r\n";

static int session_id_tmp = -1;

static void* thread_run(void *data) {

	sleep(3);


	int result = stanet_session_open("192.168.1.224",8182);
	if(result < 0){
		sta_log("stanet_session_open fail.");
	}

	sleep(3);

	if(session_id_tmp >= 0){
		result = stanet_session_write(session_id_tmp,http_header,strlen(http_header));
		if(result < 0){
			sta_log("stanet_session_write fail.");
		}

		sleep(3);

		result = stanet_session_read(session_id_tmp,NULL,1024);
		if(result < 0){
			sta_log("stanet_session_write fail.");
		}
	}





//	send_msg(0, STA_MSG_SOCKET_OPEN, "22.22.22.22:80");
//	send_msg(1, STA_MSG_SOCKET_OPEN, "192.168.1.198:80");


}

/**
* Callback for session open/write/read/close
*/
void qmi_session_callback(
	int session_id,
	sta_msg_id_enum msg_id,
	sta_session_err_enum err,
	void *data,
	int len
)
{
	sta_log("session_id = %d,msg_id = %d,error = %d,len = %d\n",
		session_id,msg_id,err,len);

	if(data == NULL){
		sta_log("data = NULL\n");
	}else{
		sta_log("data = %s\n",data);
	}

	switch(msg_id){
		case STA_MSG_SOCKET_OPEN:{
			if(err == STA_SESSION_ERR_SUCCESS){ // Success
				session_id_tmp = session_id;
			}else{// Fail

			}
			break;
		}
		case STA_MSG_SOCKET_WRITE:{
			if(err == STA_SESSION_ERR_SUCCESS){ // Success

			}else{// Fail

			}
			break;
		}
		case STA_MSG_SOCKET_READ:{
			if(err == STA_SESSION_ERR_SUCCESS){ // Success

			}else{// Fail

			}

			// Should free buffer.
			free(data);
			data = NULL;
			break;
		}
		case STA_MSG_SOCKET_CLOSE:{
			if(err == STA_SESSION_ERR_SUCCESS){ // Success

			}else{// Fail

			}
			break;
		}
		default:{
			sta_log("Error MSG Callback.");
			break;
		}
	}

}


void stanet_qmi_init
(
	void
)
{
	if(pthread_create(&thread_id,NULL,thread_run,NULL) < 0){
		sta_log("pthread_create fail.");
		return;
	}
}
