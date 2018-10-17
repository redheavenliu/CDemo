#include "mbtk_coap_generic.h"
#include "m_stdio.h"
#include "m_sock.h"

#ifndef FEATURE_MBTK_COAP
#define FEATURE_MBTK_COAP
#endif

#ifdef FEATURE_MBTK_COAP

#define HEXCHAR(x) ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))
#define COAP_SIG_WAITTING (1 << 30)

static mbtk_coap_generic_type coap_items[MBTK_COAP_TYPE_MAX] = {0};
static pthread_t coap_thread_id;
static mbtk_coap_read_cb_func coap_read_cb = NULL;

static bool coap_str_empty
(
    const void *str
)
{
    if(str == NULL || strlen((char*)str) == 0)
        return TRUE;

    return FALSE;
}

/* Open network connection. Return TRUE if success,FALSE or net.*/
static bool coap_net_open
(
    mbtk_coap_type_menu coap_type,
    rex_timer_cnt_type timeout
)
{
    logs0("coap_net_open(): SUCCESS");
    return TRUE;
}

static bool coap_net_close
(
    mbtk_coap_type_menu coap_type
)
{
    logs0("coap_net_close(): SUCCESS");
    return TRUE;
}


/* Create socket for COAP.(UDP) */
static sint15 coap_sock_create
(
    mbtk_coap_type_menu coap_type,
    void *host_ptr,
    int port,
    rex_timer_cnt_type timeout
)
{
    int sockfd;
    struct sockaddr_in recv_seraddr;
    socklen_t recv_len;
    int n;
    int serlen;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0) {
        logs1("socket() fail.[%d]",errno);
        return -1;
    }

    if(sockaddr_init(&coap_items[coap_type].seraddr,host_ptr,port,AF_INET) < 0) {
        logs1("sockaddr_init() fail.[%d]",errno);
        return -1;
    }

    serlen = sizeof(struct sockaddr_in);
    if(connect(sockfd, &coap_items[coap_type].seraddr, serlen) < 0) {
        logs1("connect() fail.[%d]",errno);
        return -1;
    }

    // 得到本地IP及端口
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);
    if(getsockname(sockfd,&cliaddr, &cliaddr_len) < 0) {
        logs1("getsockname() fail.[%d]",errno);
        return -1;
    }
    logs1("cliaddr = %s",sock_ntop(&cliaddr,cliaddr_len));

    coap_items[coap_type].handle = 1;
    coap_items[coap_type].sockfd = sockfd;
    coap_items[coap_type].port = port;

    return sockfd;
}

static bool
coap_sock_close
(
    mbtk_coap_type_menu coap_type,
    rex_timer_cnt_type timeout
)
{
    sint15 res;
    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return FALSE;
    }

    if(coap_items[coap_type].handle <= 0 || coap_items[coap_type].sockfd <= 0) {
        logs0("Socket not open.");
        return FALSE;
    }

    logs1("coap_sock_close(): sockfd = %d", coap_items[coap_type].sockfd);
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if (coap_items[coap_type].sockfd <= 0) {
        return TRUE;
    }
    //mbtk_dereg_ip_filter(sockfd, netPolicy);

    res = close(coap_items[coap_type].sockfd);

    if(res == 0) {
        coap_items[coap_type].sockfd = -1;
        return TRUE;
    } else {
        return FALSE;
    }
}

static bool coap_hex_right
(
    const char *str,
    int len
)
{
    if(coap_str_empty(str))
        return FALSE;

    if(len%2 != 0)
        return FALSE;

    int index = 0;
    while(index < len){
        if(str[index] == '\0'
            || !HEXCHAR(str[index])) {
            return FALSE;
        }
        index++;
    }

    return TRUE;
}

static char coap_str_to_hex
(
    char *pbDest,
    char *pbSrc,
    int nLen
)
{
    char h1,h2;
    char s1,s2;
    char i;

    for (i=0; i<nLen; i++) {
        h1 = pbSrc[2*i];
        h2 = pbSrc[2*i+1];
        s1 = toupper(h1) - 0x30;
        if (s1 > 9) {
            s1 -= 7;
        }
        s2 = toupper(h2) - 0x30;
        if (s2 > 9) {
            s2 -= 7;
        }
        pbDest[i] = s1*16 + s2;
    }
    return i;
}

void coap_log(bool is_ascii,const char* data,int len)
{
    if(!COAP_DEBUG_LB || len <= 0) {
        return;
    }

    if(is_ascii) { // Is ASCII
        int i = 0;
        int count = len / 100 + 1;

        // Has '\0'
        if(len != strlen(data)) {
            return;
        }

        while(i < count) {
            logs1("%s",data + i * 100);
            i++;
        }
    } else { // Is hex
        int index = 0;
        int i = 0;
        char buf_temp[100];
        while(index % 16 == 0 && index < len) {
            memset(buf_temp,0,100);
            for(i = 0;i < 16 && index + i < len;i++) {
                snprintf(buf_temp + i * 3,100 - i,"%.2x ",0xFF & ((char*)data)[index + i]);
            }
            index += 16;
            logs1("%s", buf_temp);
        }
    }
}

sint15 mbtk_coap_udp_sendto
(
    mbtk_coap_type_menu coap_type,
    const void *buffer,
    uint16 len,
    rex_timer_cnt_type timeout
)
{
    sint15 dss_errno = 0;
    int total_size = 0;
    sint15 res;
    uint8_t retrycout = 4;

    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return -1;
    }

    if(coap_items[coap_type].handle <= 0 || coap_items[coap_type].sockfd <= 0) {
        logs0("Socket not open.");
        return -1;
    }

    logs0("COAP_SEND:");
    coap_log(FALSE,(char*)buffer,len);

retry_send:
//    res = sock_sendto(coap_items[coap_type].sockfd, buffer, len, 0,
//        &coap_items[coap_type].seraddr, sizeof(struct sockaddr_in));

    res = sock_sendto(coap_items[coap_type].sockfd, buffer, len, 0,NULL, 0);
    total_size += res;
    if(res > 0 && res < total_size)
    {
        while((res = sock_sendto(coap_items[coap_type].sockfd,
                (char*)buffer + total_size, len - total_size, 0,NULL, 0)) > 0) {
            total_size += res;
            if(total_size == len)
                break;
        }
    }
    retrycout--;
    if(total_size != len && retrycout > 0)
    {
        timeout = 5000;
        total_size = 0;
        res = 0;
        goto retry_send;
    }

    logs3("Send complete: %d/%d [dss_errno=%d]",
        total_size, len, dss_errno);

    return total_size;
}

sint15 mbtk_coap_udp_recvfrom
(
    mbtk_coap_type_menu coap_type,
    void *buffer,
    uint16 nbytes,
    rex_timer_cnt_type timeout
)
{
    sint15 dss_errno = 0;

    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return -1;
    }

    if(coap_items[coap_type].handle <= 0 || coap_items[coap_type].sockfd <= 0) {
        logs0("Socket not open.");
        return -1;
    }

    memset(buffer,0x0,nbytes);
    sint15 res = sock_recvfrom(coap_items[coap_type].sockfd,
                    buffer, nbytes, 0, NULL,0);

    logs3("Recv complete: %d/%d [dss_errno=%d]", res, nbytes, dss_errno);
    logs0("COAP_RECV:");
    coap_log(FALSE,(char*)buffer,res);
    return res;
}

bool mbtk_coap_sock_init
(
    mbtk_coap_type_menu coap_type,
    void *host,
    int port,
    rex_timer_cnt_type timeout
)
{
    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return FALSE;
    }

    if(coap_items[coap_type].handle > 0
        && !coap_net_open(coap_type,timeout)) {
        logs0("coap_net_open() fail.");
        return FALSE;
    }

    if(coap_items[coap_type].sock_reset && coap_items[coap_type].sockfd > 0) {
        sint15 sockfd = coap_items[coap_type].sockfd;
        sint15 dss_errno;
        coap_items[coap_type].sock_reset = FALSE;
        if(coap_sock_create(coap_type,
                    host,
                    port,
                    timeout) < 0) {
            logs0("coap_sock_create() fail.");
            return FALSE;
        }
        close(sockfd);

    } else {
        if(coap_items[coap_type].sockfd <= 0
            && coap_sock_create(coap_type,
                    host,
                    port,
                    timeout) < 0) {
            logs0("coap_sock_create() fail.");
            return FALSE;
        }
    }

    return TRUE;
}

bool mbtk_coap_sock_deinit
(
    mbtk_coap_type_menu coap_type,
    rex_timer_cnt_type timeout
)
{
    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return FALSE;
    }

    if(coap_items[coap_type].sockfd > 0
        && !coap_sock_close(coap_type,
                timeout)) {
        logs0("coap_sock_close() fail.");
        return FALSE;
    }

    return TRUE;
}

void coap_thread_func(void *args)
{
    logs0("coap_thread_func() start.");
    int maxfdp1,nready;
    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;
    int clilen;
    fd_set rset;
    char mesg[MAXLINE];
    FD_ZERO(&rset);

    mbtk_coap_type_menu coap_type = MBTK_COAP_TYPE_CMD;

    logs1("coap_type = %d",coap_type);

    clilen = sizeof(cliaddr);
    memcpy(&cliaddr,&coap_items[coap_type].seraddr,sizeof(struct sockaddr_in));
    maxfdp1 = coap_items[coap_type].sockfd + 1;
    for ( ; ; ) {
        FD_SET(coap_items[coap_type].sockfd, &rset);
        if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;   /* back to for() */
            else {
                logs0("select error");
                break;
            }
        }

        if (FD_ISSET(coap_items[coap_type].sockfd, &rset)) {
            len = clilen;

            //n = sock_recvfrom(coap_items[coap_type].sockfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
            n = mbtk_coap_udp_recvfrom(coap_type, mesg, MAXLINE, 3000);
            if(n > 0) {
                mesg[n] = '\0';
                logs1("RECV:[From : %s]",sock_ntop(&cliaddr,len));
                if(coap_read_cb != NULL) {
                    coap_read_cb(mesg,n);
                }
            } else {
                logs1("recvfrom() fail[%d].",errno);
            }
        }
    }
}


void mbtk_coap_task_start
(
    mbtk_coap_type_menu coap_type,
    mbtk_coap_read_cb_func func
)
{
    if(coap_type < 0 || coap_type >= MBTK_COAP_TYPE_MAX) {
        logs0("ARG error...");
        return ;
    }

    coap_read_cb = func;

    if (0 != pthread_create(&coap_thread_id, NULL, coap_thread_func, NULL))
    {
        logs1("error when create pthread,%d\n", errno);
        return ;
    }
}

#endif /* FEATURE_MBTK_COAP */
