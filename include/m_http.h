#ifndef M_HTTP_H
#define M_HTTP_H

#include "m_stdio.h"
#include "m_sock.h"

#define HTTP_HOST_MAX_LEN 100
#define HTTP_URI_MAX_LEN 1024
#define HTTP_URL_MAX_LEN (HTTP_HOST_MAX_LEN + HTTP_URI_MAX_LEN + 20)

/**
* HTTP Get request.Return socket fd for read.
*/
int
http_get
(
    const char *url
);

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
);

#endif /* M_HTTP_H */
