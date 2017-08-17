#include "m_stdio.h"
#include "m_http.h"

void x_pic_main
(
    void
)
{
    char *url = "http://www.qq.com/";
    if(http_get(url) == 0){
        printf("http_get:success.\n");
    }else{
        printf("http_get:fail.\n");
    }
}

