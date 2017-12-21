#ifndef INCLUDE_STANET_TYPE_H
#define INCLUDE_STANET_TYPE_H

#include "config.h"
#include "m_log.h"


#ifdef M_BOOLEAN_DEFINED
typedef unsigned char boolean; /* Boolean value type. */
#endif

#ifdef M_BOOL_DEFINED
typedef unsigned char bool; /* Boolean value type. */
#endif

#ifdef M_UINT64_DEFINED
typedef unsigned long long uint64; /* Unsigned 64 bit value */
#endif

#ifdef M_UINT32_LB_DEFINED
#ifdef M_BIT_64
typedef unsigned int uint32; /* Unsigned 32 bit value */
#else
typedef unsigned long int uint32; /* Unsigned 32 bit value */
#endif
#endif

#ifdef M_UINT16_DEFINED
typedef unsigned short uint16; /* Unsigned 16 bit value */
#endif

#ifdef M_UINT8_DEFINED
typedef unsigned char uint8; /* Unsigned 8  bit value */
#endif

#ifdef M_INT64_DEFINED
typedef signed long long int64; /* Signed 64 bit value */
#endif

#ifdef M_INT32_DEFINED
#ifdef M_BIT_64
typedef signed int int32; /* Signed 32 bit value */
#else
typedef signed long int int32; /* Signed 32 bit value */
#endif
#endif

#ifdef M_INT16_DEFINED
typedef signed short int16; /* Signed 16 bit value */
#endif

#ifdef M_INT8_DEFINED
typedef signed char int8; /* Signed 8  bit value */
#endif

#ifdef  M_BYTE_DEFINED
typedef unsigned char byte; /* byte type */
#endif

#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef true
#define true   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef false
#define false  0   /* Boolean false value. */
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef null
#define null  0
#endif

#define SESSION_MAX_COUNT 5
#define SESSION_HOST_MAX_LEN 128
#define SESSION_MSG_MAX_LEN 2048

typedef enum {
	STA_SESSION_ERR_SUCCESS, /* Success */
	STA_SESSION_ERR_TO_MORE, /* No usable session */
	STA_SESSION_ERR_HOST,	 /* Host error */
	STA_SESSION_ERR_CONN,	 /* Connect error */


	STA_SESSION_ERR_UNKNOWN
} sta_session_err_enum;

typedef enum {
	STA_SESSION_STATE_UNUSED,
	STA_SESSION_STATE_CLOSED,
	STA_SESSION_STATE_OPENING,
	STA_SESSION_STATE_OPENED,
	STA_SESSION_STATE_WRITING,
	STA_SESSION_STATE_READING,
	STA_SESSION_STATE_CLOSING
} sta_session_state_enum;

typedef enum {
	STA_MSG_SOCKET_OPEN,
	STA_MSG_SOCKET_WRITE,
	STA_MSG_SOCKET_READ,
	STA_MSG_SOCKET_CLOSE
} sta_msg_id_enum;

typedef struct sta_msg_q{
	sta_msg_id_enum msg_id;
	int data_len;
	char data[SESSION_MSG_MAX_LEN + 1];

	struct sta_msg_q *next;
} sta_msg_q_s;

typedef struct sta_session{
	int session_id; // 1 - n
	int sockfd;
	sta_session_state_enum state;
	sta_session_err_enum err;

	char host[SESSION_HOST_MAX_LEN];
	int port;


	pthread_t thread_id;
	bool thread_running;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	sta_msg_q_s *msg_q;

	struct sta_session *pre;
	struct sta_session *next;
} sta_session_s;


int stanet_session_open(const char *host,int port);
int stanet_session_write(int session_id,void *buf,int len);
int stanet_session_read(int session_id,void *buf,int len);
int stanet_session_close(int session_id);

#endif /* INCLUDE_STANET_TYPE_H */
