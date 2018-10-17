#include "mbtk_coap_base.h"
#include "mbtk_coap_api.h"
#include "m_string.h"

// Point the first item.
static mbtk_coap_package_s *mbtk_coaps = NULL;
// Point the last item.
static mbtk_coap_package_s *mbtk_coaps_tail = NULL;
static mbtk_coap_type_menu coap_type = MBTK_COAP_TYPE_CMD;
static uint16 mbtk_coap_message_id = 1;

mbtk_coap_package_s* mbtk_coap_list_add(uint16 message_id);
bool mbtk_coap_list_del(uint16 message_id);
mbtk_coap_package_s *mbtk_coap_get_by_msg_id(uint16 message_id);
void mbtk_coap_coap_send(mbtk_coap_package_s* coap);


static void mbtk_coap_send_ack(mbtk_coap_code_type code,
                               uint16_t message_id,uint8 token_len,uint8 *token)
{
    coapReset();
    coapSetVersion(1);
    coapSetType(COAP_ACKNOWLEDGEMENT);
    coapSetMessageID(message_id);

    if(code == COAP_POST || code == COAP_PUT)
        coapSetCode(COAP_CHANGED);
    else
        coapSetCode(COAP_EMPTY);

    if(token_len > 0)
    {
        coapSetToken(token, token_len);
    }

//    if(coapHasOption(COAP_OPTION_OBSERVE)) {
//        uint16_t observe_len;
//        uint8_t observe[4];
//        coapGetOptionValueById(COAP_OPTION_OBSERVE,&observe_len,observe);
//        coapAddOption(COAP_OPTION_OBSERVE, observe_len, observe);
//        coapSetCode(COAP_CHANGED);
//    }

    uint8_t* pduPointer = coapGetPDUPointer();
    int pduLen = coapGetPDULength();
    int res = mbtk_coap_udp_sendto(coap_type, pduPointer, pduLen, 3000);
    if(res != pduLen)
    {
        logs2("Send ACK fail.[%d/%d]",res,pduLen);
    }
    else
    {
        logs0("Send ACK to server success.");
    }
}

static void mbtk_coap_recv_resolve(const void *data,uint16 data_len)
{
    if(coapCreateRecv((uint8_t*)data, data_len) <= 0)
    {
        logs0("coapCreateRecv fail.");
        coapDeleteRecv();
    }
    else
    {
        logs0("coapCreateRecv() success.");

        uint8_t version = coapGetRecvVersion();
        mbtk_coap_type type = coapGetRecvType();
        mbtk_coap_code_type code = coapGetRecvCode();
        uint16_t message_id = coapGetRecvMessageID();
        logs2("version:%d;type:%d",version,type);
        logs2("code:%d;message_id:%d",code,message_id);

        uint8 token_len = coapGetRecvTokenLength();
        char token[9] = {0};
        memcpy(token,coapGetRecvTokenPointer(),token_len);
        logs2("token[%d]:%s",token_len,token);

        uint16 playload_len = coapGetRecvPayloadLength();
        uint8 *playload = coapGetRecvPayloadPointer();
        logs0("----------------payload-----------------");
        coap_log(TRUE,(char*)playload,playload_len);
        logs0("--------------payload end----------------");

        uint8 uri_path[100];
        uint16_t len;
        memset(uri_path,0x0,100);
        if(coapGetRecvOptionUriPath(&len, uri_path) > 0)
        {
            logs1("Uri-Path:%s",uri_path);
        }

        if(type == COAP_CONFIRMABLE) // Should ACK
        {
            mbtk_coap_send_ack(code,message_id,token_len,token);
        }
        else if(type == COAP_NON_CONFIRMABLE)
        {
            // Do nothing.
        }
        else if(type == COAP_ACKNOWLEDGEMENT)
        {
            mbtk_coap_package_s * coap_send = mbtk_coap_get_by_msg_id(message_id);
            if(coap_send != NULL)
            {
                uint32_t number;
                uint8_t more_flag;
                uint8_t size;
                logs1("Get message(%d) ack.",message_id);
                if(coapGetRecvOptionBlock2(&number, &more_flag, &size))
                {
                    logs3("Block2 (size:%d; more_flag:%d; number:%d)",size,more_flag,number);
                    if(more_flag) // Has data no read.
                    {
                        coap_send->message_id = mbtk_coap_message_id++;
                        mbtk_coap_set_option(coap_send, COAP_OPTION_BLOCK2, NULL,
                                             size,number + 1,0);

                        mbtk_coap_coap_send(coap_send);
                    }
                    else
                    {
                        mbtk_coap_list_del(message_id);
                    }
                }
                else
                {
                    logs0("coapGetRecvOptionBlock2() fail.");
                    mbtk_coap_list_del(message_id);
                }
            }
            else
            {
                logs0("Coap not match.");
            }
        }
        else     // COAP_RESET
        {
            logs0("Coap type error.");
        }
    }

}

void coap_read_cb(const void *data,uint16 data_len)
{
    logs1("read:%d",data_len);
    if(data_len > 0)
    {
        mbtk_coap_recv_resolve(data,data_len);
    }
}

static void mbtk_coap_list_item_init(mbtk_coap_package_s *coap,uint16 message_id)
{
    memset(coap,0x0,sizeof(mbtk_coap_package_s));
    coap->version = 1;
    coap->code = COAP_EMPTY;
    coap->message_id = message_id;

    coap->next = NULL;
}

static void mbtk_coap_free(mbtk_coap_package_s *coap)
{
    if(coap != NULL)
    {
        if(coap->playload != NULL)
            free(coap->playload);

        if(coap->options != NULL)
        {
            mbtk_coap_option_s *option_ptr = coap->options;
            while(option_ptr != NULL)
            {
                free(option_ptr);
                option_ptr = option_ptr->next;
            }
        }

        free(coap);
        coap = NULL;
    }
}

void mbtk_coap_set_payload(mbtk_coap_package_s *coap,const void *data,uint16 data_len)
{
    if(coap != NULL)
    {
        if(coap->playload != NULL)
        {
            free(coap->playload);
        }

        coap->playload = (uint8*)malloc(data_len);
        memset(coap->playload,0x0,data_len);
        memcpy(coap->playload,data,data_len);

        coap->playload_len = data_len;
    }
}

void mbtk_coap_set_token(mbtk_coap_package_s *coap,const void *token,uint8 token_len)
{
    if(coap != NULL)
    {
        coap->token_len = token_len;
        memset(coap->token,0x0,8);

        if(token_len > 0)
            memcpy(coap->token,token,token_len);
    }
}

void mbtk_coap_set_option(mbtk_coap_package_s *coap,mbtk_coap_option_type type,void *str,int int0,int int1,int int2)
{
    if(coap != NULL)
    {
        mbtk_coap_option_s *option = coap->options;
        while(option != NULL)
        {
            if(option->type == type)
            {
                break;
            }
            option = option->next;
        }

        if(option == NULL) // Now option
        {
            option = (mbtk_coap_option_s*)malloc(sizeof(mbtk_coap_option_s));
            memset(option,0x0,sizeof(mbtk_coap_option_s));
            option->next = coap->options;
            coap->options = option;
            option->type = type;
        }
        else // Change option
        {
            // Do noting.
        }

        switch(type)
        {
            case COAP_OPTION_IF_MATCH:
            case COAP_OPTION_URI_HOST:
            case COAP_OPTION_ETAG:
            case COAP_OPTION_IF_NONE_MATCH:
            case COAP_OPTION_LOCATION_PATH:
            case COAP_OPTION_URI_QUERY:
            case COAP_OPTION_LOCATION_QUERY:
            case COAP_OPTION_PROXY_URI:
            case COAP_OPTION_PROXY_SCHEME:
            case COAP_OPTION_URI_PATH:
            {
                memset(option->opt.opt_str,0x0,128);
                memcpy(option->opt.opt_str,str,strlen((char*)str));
                break;
            }
            case COAP_OPTION_OBSERVE:
            case COAP_OPTION_MAX_AGE:
            case COAP_OPTION_URI_PORT:
            case COAP_OPTION_SIZE2:
            case COAP_OPTION_SIZE1:
            {
                option->opt.opt_int = (uint32)int0;
                break;
            }
            case COAP_OPTION_BLOCK2:
            case COAP_OPTION_BLOCK1:
            {
                option->opt.opt_block.size = (mbtk_coap_option_block_e)int0;
                option->opt.opt_block.number = (uint32)int1;
                option->opt.opt_block.more_flag = (uint8)int2;
                break;
            }
            case COAP_OPTION_CONTENT_FORMAT:
            case COAP_OPTION_ACCEPT:
            {
                option->opt.opt_content_format = (mbtk_content_format_type)int0;
                break;
            }
            default:
                logs1("No such type:%s",type);
                break;
        }
    }
}

void mbtk_coap_set_type(mbtk_coap_package_s *coap,mbtk_coap_type type)
{
    if(coap != NULL)
    {
        coap->type = type;
    }
}

void mbtk_coap_set_code(mbtk_coap_package_s *coap,mbtk_coap_code_type code)
{
    if(coap != NULL)
    {
        coap->code = code;
    }
}

mbtk_coap_package_s *mbtk_coap_get_by_msg_id(uint16 message_id)
{
    mbtk_coap_package_s *coap_ptr = mbtk_coaps;
    while(coap_ptr != NULL)
    {
        if(coap_ptr->message_id == message_id)
        {
            return coap_ptr;;
        }
        coap_ptr = coap_ptr->next;
    }

    return NULL;
}

mbtk_coap_package_s* mbtk_coap_list_add(uint16 message_id)
{
    if(mbtk_coaps == NULL)
    {
        mbtk_coaps = (mbtk_coap_package_s*)malloc(sizeof(mbtk_coap_package_s));
        mbtk_coap_list_item_init(mbtk_coaps,message_id);
        mbtk_coaps_tail = mbtk_coaps;
        return mbtk_coaps;
    }
    else
    {
        mbtk_coap_package_s *coap_ptr = mbtk_coaps;
        while(coap_ptr != NULL)
        {
            if(coap_ptr->message_id == message_id)
            {
                logs1("Coap %d exist.",message_id);
                return NULL;
            }
            coap_ptr = coap_ptr->next;
        }
        mbtk_coaps_tail->next = (mbtk_coap_package_s*)malloc(sizeof(mbtk_coap_package_s));
        mbtk_coap_list_item_init(mbtk_coaps_tail->next,message_id);
        mbtk_coaps_tail = mbtk_coaps_tail->next;
        return mbtk_coaps_tail;
    }
}

bool mbtk_coap_list_del(uint16 message_id)
{
    if(mbtk_coaps == NULL)
        return FALSE;

    mbtk_coap_package_s *coap = mbtk_coaps;
    if(mbtk_coaps->message_id == message_id) // Is first item.
    {
        mbtk_coaps = mbtk_coaps->next;

        mbtk_coap_free(coap);
        return TRUE;
    }
    else     // Not first item.
    {
        while(coap->next != NULL)
        {
            if(message_id == coap->next->message_id)   // delete
            {
                mbtk_coap_package_s *coap_tmp = coap->next;
                if(mbtk_coaps_tail == coap_tmp)
                {
                    mbtk_coaps_tail = coap;
                }
                coap->next = coap_tmp->next;
                mbtk_coap_free(coap_tmp);
                return TRUE;;
            }
            coap = coap->next;
        }
        return FALSE;
    }
}

void mbtk_coap_option_add(mbtk_coap_option_s *option)
{
    if(option != NULL)
    {
        switch(option->type)
        {
            case COAP_OPTION_IF_MATCH:
            case COAP_OPTION_URI_HOST:
            case COAP_OPTION_ETAG:
            case COAP_OPTION_IF_NONE_MATCH:
            case COAP_OPTION_LOCATION_PATH:
            case COAP_OPTION_LOCATION_QUERY:
            case COAP_OPTION_PROXY_URI:
            case COAP_OPTION_PROXY_SCHEME:
            case COAP_OPTION_URI_PORT:
            case COAP_OPTION_ACCEPT:
            case COAP_OPTION_SIZE2:
            case COAP_OPTION_SIZE1:
            {
                break;
            }
            case COAP_OPTION_BLOCK2:
            case COAP_OPTION_BLOCK1:
            {
                logs1("option->opt.opt_block.number = %d",option->opt.opt_block.number);
                if(option->opt.opt_block.number <= 0x0F)
                {
                    uint8_t temp[1];
                    temp[0] = (uint8_t)(((option->opt.opt_block.number << 4) & 0xF0)
                                        | ((option->opt.opt_block.more_flag << 3) & 0x08)
                                        | (option->opt.opt_block.size & 0x07));
                    coapAddOption(option->type,1,temp);
                }
                else if(option->opt.opt_block.number <= 0x0FFF)
                {
                    uint8_t temp[2];
                    temp[0] = (uint8_t)(option->opt.opt_block.number >> 4);
                    temp[1] = (uint8_t)(((option->opt.opt_block.number << 4) & 0xF0)
                                        | ((option->opt.opt_block.more_flag << 3) & 0x08)
                                        | (option->opt.opt_block.size & 0x07));
                    coapAddOption(option->type,2,temp);

                }
                else
                {
                    uint8_t temp[3];
                    temp[0] = (uint8_t)(option->opt.opt_block.number >> 12);
                    temp[1] = (uint8_t)(option->opt.opt_block.number >> 4);
                    temp[2] = (uint8_t)(((option->opt.opt_block.number << 4) & 0xF0)
                                        | ((option->opt.opt_block.more_flag << 3) & 0x08)
                                        | (option->opt.opt_block.size & 0x07));
                    coapAddOption(option->type,3,temp);

                }
                break;
            }
            case COAP_OPTION_MAX_AGE:
            {
                uint8_t temp[1];
                temp[0] = (uint8_t)option->opt.opt_int;
                coapAddOption(option->type,1,temp);
                break;
            }
            case COAP_OPTION_OBSERVE:
            {
                uint8_t temp[1];
                temp[0] = (uint8_t)option->opt.opt_int;
                coapAddOption(option->type,1,temp);
                break;
            }
            case COAP_OPTION_URI_QUERY:
            {
                coapAddURIQuery((char*)option->opt.opt_str);
                break;
            }
            case COAP_OPTION_URI_PATH:
            {
                coapSetURI((char*)option->opt.opt_str);
                break;
            }
            case COAP_OPTION_CONTENT_FORMAT:
            {
                coapSetContentFormat(option->opt.opt_content_format);
                break;
            }
            default:
                logs1("No such type:%s",option->type);
                break;
        }
    }
}

void mbtk_coap_coap_send(mbtk_coap_package_s* coap)
{
    if(coap != NULL)
    {
        coapReset();
        coapSetVersion(coap->version);
        coapSetType(coap->type);
        coapSetCode(coap->code);
        coapSetMessageID(coap->message_id);

        if(coap->token_len > 0)
        {
            coapSetToken(coap->token, coap->token_len);
        }

        mbtk_coap_option_s *option = coap->options;
        while(option != NULL)
        {
            mbtk_coap_option_add(option);
            option = option->next;
        }

        if(coap->playload_len > 0)
        {
            coapSetPayload((uint8_t*)coap->playload, coap->playload_len);
        }

        uint8_t *pduPointer = coapGetPDUPointer();
        int pduLen = coapGetPDULength();
        sint15 send_len;
        if((send_len = mbtk_coap_udp_sendto(coap_type,
                                            pduPointer,
                                            pduLen, 3000)) != pduLen)
        {
            logs2("coap_udp_sendto complate.[%d/%d]",send_len,pduLen);

            coap->send_count++;
        }

        if(coap->type != COAP_CONFIRMABLE)
        {
            mbtk_coap_list_del(coap->message_id);
        }
    }
}

int main(int argc,char *argv[])
{
    log_init(LOG_D, FALSE, NULL);

    logs1("coap start.[%d]", 0);
    // 139.196.187.107
    // 104.196.15.150
    if(!mbtk_coap_sock_init(MBTK_COAP_TYPE_CMD, "104.196.15.150",5683, 3000))
    {
        logs0("mbtk_coap_sock_init() fail.");
        return 0;
    }

    mbtk_coap_task_start(MBTK_COAP_TYPE_CMD,coap_read_cb);

    char line[200];
    for (;;)
    {
        logs0("GET/POST/PUT/DELETE uri payload");
        memset(line,0x0,200);
        if(gets(line) == NULL)
        {
            continue;
        }
        char **argv = str_split_by_string(line, " ");
        if(argv == NULL)
        {
            logs0("NULL");
            continue;
        }
        int index = 0;
        while(*(argv + index++) != NULL)
            ;
        index--;
        logs1("index = %d",index);
        if(index == 2 || index == 3)
        {
            mbtk_coap_package_s* coap = mbtk_coap_list_add(mbtk_coap_message_id);
            if(coap != NULL)
            {
                mbtk_coap_message_id++;
                mbtk_coap_set_type(coap, COAP_CONFIRMABLE);
                if(!strncmp("GET",argv[0],3))
                    mbtk_coap_set_code(coap, COAP_GET);
                else if(!strncmp("POST",argv[0],3))
                {
                    mbtk_coap_set_code(coap, COAP_POST);
                }
                else if(!strncmp("PUT",argv[0],3))
                {
                    mbtk_coap_set_code(coap, COAP_PUT);
                }
                else if(!strncmp("DELETE",argv[0],3))
                    mbtk_coap_set_code(coap, COAP_DELETE);
                else
                    continue;

//                char token[2];
//                token[0] = 0xB6;
//                token[1] = 0xE5;
//                mbtk_coap_set_token(coap, token, 2);
                mbtk_coap_set_option(coap, COAP_OPTION_URI_PATH, argv[1], 0,0,0);
//                mbtk_coap_set_option(coap, COAP_OPTION_BLOCK2, NULL,
//                                     MBTK_COAP_BLOCK_64,0,0);
                mbtk_coap_set_option(coap, COAP_OPTION_MAX_AGE, NULL,1,0,0);
                mbtk_coap_set_option(coap, COAP_OPTION_CONTENT_FORMAT, NULL,
                                     COAP_CONTENT_FORMAT_TEXT_PLAIN,0,0);

                if(index == 3)
                    mbtk_coap_set_payload(coap, argv[2],strlen(argv[2]));

                mbtk_coap_coap_send(coap);
            }
        }
        else
        {
            continue;
        }

    }

    return 0;
}

