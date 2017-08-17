#include "m_stdio.h"
#include "m_util.h"

//typedef unsigned char bool
//typedef unsigned char bool

void*
util_test
(
    void
)
{
    //char *tmp = (char*)malloc(100);
    static char tmp[100];
    memset(tmp,0,100);
    memcpy(tmp,"123",2);
    return tmp;
}


typedef struct
{
    char *key;
    char *value;
}mbtk_http_header_item_s;

typedef struct{
    mbtk_http_header_item_s *header_item[20];
    size_t item_count;
} mbtk_http_request_s,*mbtk_http_request_s_p;

mbtk_http_request_s http_session;

static int
mbtk_http_add_header_item
(
    char* key,
    char* value
)
{

    mbtk_http_header_item_s **tmp_item = http_session.header_item;
    while(*tmp_item){
        if(strcmp((*tmp_item)->key,key) == 0){
            if(strcmp((*tmp_item)->value,value) == 0){
                // Key and value all the same,do nothing.
                return TRUE;
            }else{
                // Should change value.
                break;
            }
        }
        tmp_item++;
    }

    // Should add or change item.
    mbtk_http_header_item_s *item = calloc(1,sizeof(mbtk_http_header_item_s));
    item->key = calloc(strlen(key) + 1,sizeof(char));
    item->value = calloc(strlen(value) + 1,sizeof(char));
    memcpy(item->key,key,strlen(key));
    memcpy(item->value,value,strlen(value));

    if(*tmp_item){// Change item,should free in the first.
        free((*tmp_item)->key);
        free((*tmp_item)->value);
        (*tmp_item)->key = NULL;
        (*tmp_item)->value = NULL;
        free(*tmp_item);
        *tmp_item = NULL;
    }else{
        http_session.item_count++;
    }

    *tmp_item = item;

    return TRUE;
}

static char*
mbtk_http_get_request_head_item_string
(
    void
)
{
    log_i("start.[%d]",10);
    uint16 lenght = 0;
    mbtk_http_header_item_s** tmp = http_session.header_item;
    while(*tmp){
        lenght += (strlen((*tmp)->key) + strlen((*tmp)->value) + 1 + 2);
        tmp++;
    }
    char* result = calloc(lenght + 1,1);
    char* p = result;
    uint16 position = 0;
    tmp = http_session.header_item;
    while(*tmp){
        //lenght += strlen(*http_request_ptr->http_request_head_ptr);
        position += sprintf(p + position,
                            "%s:%s\r\n",
                            (*tmp)->key,
                            (*tmp)->value);
        tmp++;
    }
    return result;
}

void my_printf(void *format,...)
{
    va_list ap;
    va_start(ap,format);

    char *para = va_arg(ap, char*);
    while (para)
    {
        printf("para:%s\n",para);
        para = va_arg(ap, char*);
    }

    va_end(ap);
}

void test()
{
    int result = util_big_endian();

    log(LOG_D,"big endian:%d",result);

    log(LOG_D,"data = %s",(char*)util_test());

    //log_clear();

    bzero(&http_session,sizeof(mbtk_http_request_s));

    mbtk_http_add_header_item("abc1","1111");
    mbtk_http_add_header_item("abc2","2222");
    mbtk_http_add_header_item("abc3","33333");
    mbtk_http_add_header_item("abc4","44444444");
    mbtk_http_add_header_item("abc5","5555555555");
    mbtk_http_add_header_item("abc6","666666666666666666");

    log(LOG_E,"%s",mbtk_http_get_request_head_item_string());

    log(LOG_W,"Changed.");

    log_init(LOG_D, FALSE, "log");

    mbtk_http_add_header_item("abc1","11112222222");
    mbtk_http_add_header_item("abc6","6666111111");

    log(LOG_I,"%s",mbtk_http_get_request_head_item_string());

    log_init(LOG_D, FALSE, "log1");

    mbtk_http_header_item_s **tmp = http_session.header_item;
    while(*tmp){
        free((*tmp)->key);
        free((*tmp)->value);
        (*tmp)->key = NULL;
        (*tmp)->value = NULL;
        free(*tmp);
        *tmp = NULL;
        tmp++;
    }
    bzero(&http_session,sizeof(mbtk_http_request_s));

    mbtk_http_add_header_item("qqqq","3333");
    mbtk_http_add_header_item("aaaaa","44444");

    log(LOG_D,"%s",mbtk_http_get_request_head_item_string());


    my_printf("OK:%s:%s\n","a","b",NULL);
}

int main(int argc,char **argv)
{
    log_init(LOG_D, FALSE, NULL);


    //test();

	log_d("pid = %d",getpid());

	int fd = open("/home/lb/test.h264",O_RDONLY);
	if(fd < 0){
		log_e("open fail[%d].",errno);
		return -1;
	}
	//char buf[2048];
	char *buf = NULL;
	int size = -1;
	int count = 0;

	while((size = read(fd,buf,2048)) > 0){
		if(size > 0){
			count += size;
		}
	}

	log_d("len = %d,%d",count,strlen(buf));






    return EXIT_SUCCESS;
}
