/*************************************************************
Description:
    Base Coap support Header file.
Author:
    LiuBin
Date:
    2018/9/16 4:26:36
*************************************************************/
#ifndef _MBTK_COAP_BASE_H
#define _MBTK_COAP_BASE_H
#include "mbtk_coap_generic.h"
#include "mbtk_coap_api.h"


/*************************************************************
    Constants and Macros
*************************************************************/


/*************************************************************
    Definitions:enum,struct,union,class
*************************************************************/
typedef enum {
    MBTK_COAP_BLOCK_16,
    MBTK_COAP_BLOCK_32,
    MBTK_COAP_BLOCK_64,
    MBTK_COAP_BLOCK_128,
    MBTK_COAP_BLOCK_256,
    MBTK_COAP_BLOCK_512,
    MBTK_COAP_BLOCK_1024
} mbtk_coap_option_block_e;

typedef struct {
    mbtk_coap_option_block_e size;
    uint32 number;
    uint8 more_flag;
} mbtk_coap_option_block_s;

typedef struct mbtk_coap_option {
    mbtk_coap_option_type type;

    union
    {
        mbtk_coap_option_block_s opt_block;
        mbtk_content_format_type opt_content_format;
        uint8 opt_str[128];
        uint32 opt_int;
    } opt;

    struct mbtk_coap_option *next;
} mbtk_coap_option_s;

typedef struct mbtk_coap_package {
    uint8 send_count;

    uint8 version;
    mbtk_coap_type type;
    mbtk_coap_code_type code;

    uint8 token[8];
    uint8 token_len;

    uint16 message_id;

    mbtk_coap_option_s *options;

    uint8 *playload;
    uint16 playload_len;

    struct mbtk_coap_package *next;
} mbtk_coap_package_s;

/*************************************************************
    Extern variables
*************************************************************/


/*************************************************************
    Public Function Declaration
*************************************************************/


#endif /* _MBTK_COAP_BASE_H */
