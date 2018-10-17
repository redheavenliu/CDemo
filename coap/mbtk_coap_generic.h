#ifndef MBTK_COAP_GENERIC_H
#define MBTK_COAP_GENERIC_H
#include "m_stdio.h"
#include "m_type.h"
#include "m_sock.h"

#ifndef FEATURE_MBTK_COAP
#define FEATURE_MBTK_COAP
#endif


#define rex_timer_cnt_type long


#ifdef FEATURE_MBTK_COAP

#ifndef COAP_DEBUG_LB
#define COAP_DEBUG_LB 1
#endif
#define COAP_TAG "redheaven_coap:"

#ifdef logs0
#undef logs0
#endif
#ifdef logs1
#undef logs1
#endif
#ifdef logs2
#undef logs2
#endif
#ifdef logs3
#undef logs3
#endif
#ifdef logs4
#undef logs4
#endif

#define logs0(fmtstr) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n")
#define logs1(fmtstr, arg1) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1)
#define logs2(fmtstr, arg1, arg2) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2)
#define logs3(fmtstr, arg1, arg2, arg3) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2, arg3)
#define logs4(fmtstr, arg1, arg2, arg3, arg4) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2, arg3, arg4)

#define log0(fmtstr) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n")
#define log1(fmtstr, arg1) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1)
#define log2(fmtstr, arg1, arg2) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2)
#define log3(fmtstr, arg1, arg2, arg3) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2, arg3)
#define log4(fmtstr, arg1, arg2, arg3, arg4) \
                                        if(COAP_DEBUG_LB) \
                                            printf(COAP_TAG fmtstr "\n", arg1, arg2, arg3, arg4)

typedef void (*mbtk_coap_read_cb_func)(const void *data,uint16 data_len);

typedef enum {
    MBTK_COAP_TYPE_CMD,
    MBTK_COAP_TYPE_CTSN,
    MBTK_COAP_TYPE_ALIYUN,
    MBTK_COAP_TYPE_MAX
} mbtk_coap_type_menu;

typedef struct {
//    mbtk_coap_type_menu id;
    sint15 handle;
    sint15 net_handle;
    //mbtkssl_handle_type ssl_handle;
    //dss_net_policy_info_type net_policy;
   // mbtk_ipv4v6_addr ipv4v6;
    struct sockaddr_in seraddr;
    bool net_opened;

    sint15 sockfd;
    bool sock_reset;
    char host[128];
    int port;
} mbtk_coap_generic_type;

sint15 mbtk_coap_udp_sendto
(
    mbtk_coap_type_menu coap_type,
    const void *buffer,
    uint16 len,
    rex_timer_cnt_type timeout
);

sint15 mbtk_coap_udp_recvfrom
(
    mbtk_coap_type_menu coap_type,
    void *buffer,
    uint16 nbytes,
    rex_timer_cnt_type timeout
);

bool mbtk_coap_sock_init
(
    mbtk_coap_type_menu coap_type,
    void *host,
    int port,
    rex_timer_cnt_type timeout
);

bool mbtk_coap_sock_deinit
(
    mbtk_coap_type_menu coap_type,
    rex_timer_cnt_type timeout
);

void mbtk_coap_task_start
(
    mbtk_coap_type_menu coap_type,
    mbtk_coap_read_cb_func func
);

void coap_log(bool is_ascii,const char* data,int len);

#endif /* FEATURE_MBTK_COAP */
#endif /* MBTK_COAP_GENERIC_H */
