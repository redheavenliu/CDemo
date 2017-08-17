#include "m_http.h"

#define HTTP_REQ_HEAD_MAX_LEN 2048

static bool
http_url_parse
(
    const char *url,
    char *host,
    char *uri,
    int *port
)
{
    if(url == NULL || host == NULL || uri == NULL
        || strlen(url) > HTTP_URL_MAX_LEN) {
        return FALSE;
    }

    const char *ptr = NULL;
    if((ptr = strstr(url, "http://")) == url){
        *port = 80;
        ptr = url + strlen("http://");
    } else if((ptr = strstr(url, "https://")) == url){
        *port = 443;
        ptr = url + strlen("https://");
    } else {
        ptr = url;
    }

    // ptr point to host
    ssize_t pos = str_indexof(ptr, "/");
    if(pos < 0 || pos > HTTP_HOST_MAX_LEN){
        return FALSE;
    }

    memcpy(host,ptr,pos);
    char *host_ptr = host;
    while(*host_ptr){ // Delete port
        if(*host_ptr == ':'){
            *port = atoi(host_ptr+1);
            *host_ptr = '\0';
            break;
        }
        host_ptr++;
    }

    ssize_t uri_len = strlen(ptr) - pos;
    if(uri_len < 0 || uri_len > HTTP_URI_MAX_LEN){
        return FALSE;
    }

    memcpy(uri,ptr + pos,uri_len);

    return TRUE;
}

static bool
http_rsp_parse
(
    int fd
)
{
    char read_buf[2048];
    ssize_t len = -1;
    while((len = sock_readline(fd, read_buf, 2048 -1)) > 0){
        read_buf[len] = '\0';
        printf("%s",read_buf);
    }

    return TRUE;
}

/**
* HTTP Get request.Return socket fd for read.
*/
int
http_get
(
    const char *url
)
{
    char host[HTTP_HOST_MAX_LEN + 1];
    char uri[HTTP_URI_MAX_LEN + 1];
    char req_header_buffer[HTTP_REQ_HEAD_MAX_LEN + 1];
    bzero(host,HTTP_HOST_MAX_LEN + 1);
    bzero(uri,HTTP_URI_MAX_LEN + 1);
    bzero(req_header_buffer,HTTP_REQ_HEAD_MAX_LEN + 1);
    int port;

    if(!http_url_parse(url, host, uri, &port)) {
        log_print("http_url_parse fail.");
        return -1;
    }

    snprintf(req_header_buffer,HTTP_REQ_HEAD_MAX_LEN,
        "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",uri,host);

    printf("host:%s\nuri:%s\nport:%d\n",host,uri,port);

    int sock_fd = sock_open(host,port);
    if(sock_fd < 0){
        return -1;
    }

    if(sock_writen(sock_fd,req_header_buffer,strlen(req_header_buffer))
        != strlen(req_header_buffer)){
        log_print("writen fail.");
        return -1;
    }

    if(!http_rsp_parse(sock_fd)){
        log_print("http_rsp_parse fail.");
        return -1;
    }

    return 0;
}

/**
* Download pic by url and stored in the specified path.
*
* Parameters:
*   url     The URL of pic.
*   path    The path which pic should storage.
*   err_no  Error number.
* Returns:
*   TRUE if download success.Read err_no if return FALSE.
*/
bool
http_pic_download
(
    const char *url,
    const char *path,
    int *err_no
)
{

    return TRUE;
}
