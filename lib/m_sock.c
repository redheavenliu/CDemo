#include <signal.h>
#include <sys/time.h>    /* timeval{} for select() */
#include <time.h>        /* timespec{} for pselect() */
//#include    <net/if_dl.h>

#include "m_stdio.h"
#include "m_sock.h"

#define    IN6ADDRSZ    16
#define    INADDRSZ     4
#define    INT16SZ         2

#ifndef    AF_INET6
#define    AF_INET6    AF_MAX+1    /* just to let this compile */
#endif

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

static int    inet_pton4(const char *src, u_char *dst);
static int    inet_pton6(const char *src, u_char *dst);


static void
sig_alrm(int signo)
{
    printf("Timeout.\n");
    exit(0);
}

/* include sock_ntop */
char *
sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char        portstr[8];
    static char str[128];        /* Unix domain is largest */

    switch (sa->sa_family) {
        case AF_INET: {
            struct sockaddr_in    *sin = (struct sockaddr_in *) sa;

            if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
                return(NULL);
            if (ntohs(sin->sin_port) != 0) {
                snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
                strcat(str, portstr);
            }
            return(str);
        }
        /* end sock_ntop */
        case AF_INET6: {
            struct sockaddr_in6    *sin6 = (struct sockaddr_in6 *) sa;

            str[0] = '[';
            if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
                return(NULL);
            if (ntohs(sin6->sin6_port) != 0) {
                snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
                strcat(str, portstr);
                return(str);
            }
            return (str + 1);
        }
        case AF_UNIX: {
                struct sockaddr_un    *unp = (struct sockaddr_un *) sa;

                    /* OK to have no pathname bound to the socket: happens on
                       every connect() unless client calls bind() first. */
                if (unp->sun_path[0] == 0)
                    strcpy(str, "(no pathname bound)");
                else
                    snprintf(str, sizeof(str), "%s", unp->sun_path);
                return(str);
        }
        //    case AF_LINK: {
        //        struct sockaddr_dl    *sdl = (struct sockaddr_dl *) sa;

        //        if (sdl->sdl_nlen > 0)
        //            snprintf(str, sizeof(str), "%*s (index %d)",
        //                     sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
        //        else
        //            snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
        //        return(str);
        //    }
        default:
            snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
                     sa->sa_family, salen);
        return(str);
    }
    return (NULL);
}

int
sock_open
(
    const char *host,
    int port
)
{
    int sock_fd;
    struct sockaddr_in servaddr;

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        log_errno(-1);
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_aton(host, &servaddr.sin_addr) < 0)
    {
        log_errno(-1);
        return -1;
    }

    struct hostent *he = gethostbyname(host);
    if (he == NULL){
        log_errno(-1);
        return -1;
    }
    memcpy(&servaddr.sin_addr, he->h_addr, sizeof(struct in_addr));

//    char ipbuf[128];
//    bzero(ipbuf,128);
//    strncpy(ipbuf, inet_ntoa(servaddr.sin_addr), 128);

//    if(inet_pton(AF_INET, ipbuf, &servaddr.sin_addr) < 0){
//        log_print("inet_pton fail.");
//        return -1;
//    }

    // 设置超时
    /* Set interval timer to go off before 3WHS completes */
    signal(SIGALRM, sig_alrm);
    struct itimerval    val;
    val.it_interval.tv_sec  = 0;  // 重复间隔
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec  = 15;  // 开始时间
    val.it_value.tv_usec = 0;    /* 0 ms */
    // 15s 超时
    if (setitimer(ITIMER_REAL, &val, NULL) == -1){
       log_print("setitimer fail.");
        return -1;
    }

    if(connect(sock_fd, (SA *) &servaddr, sizeof(servaddr)) < 0){
        log_errno(-1);
        return -1;
    }

    // 取消定时器
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_REAL, &value, NULL);

    struct sockaddr_in addr_cli,addr_ser;
    socklen_t serv_len = sizeof(addr_ser);
    socklen_t cli_len = sizeof(addr_cli);

    // 得到 fd 对端IP和Port
    if(getpeername(sock_fd, (SA *) &addr_ser, &serv_len) < 0){
        log_errno(-1);
        return -1;
    }
    printf("Connected to [%s]",sock_ntop((SA *) &addr_ser, sizeof(addr_ser)));

    // 得到 fd 本端IP和Port
    if(getsockname(sock_fd, (SA *) &addr_cli, &cli_len) < 0){
        log_errno(-1);
        return -1;
    }
    printf(" from [%s]\n",sock_ntop((SA *) &addr_cli, sizeof(addr_cli)));

    return sock_fd;
}

int
sock_ser_open
(
    int domain,
    int type,
    int port
)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(domain, type, 0);
    if(sockfd < 0) {
        log_e("socket() fail[%d].",errno);
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);

    if (bind(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
        log_e("bind() fail[%d].",errno);
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int
sock_cli_open
(
    int domain,
    int type,
    const void *host,
    int port
)
{
    int sockfd;
    if((sockfd = socket(domain, type, 0)) < 0) {
        log_e("socket() fail[%d].",errno);
        return -1;
    }

    log_d("socket() ok.");

    if(sockaddr_init(NULL,host,port,domain) < 0) {
        log_e("sockaddr_init() fail[%d].",errno);
        return -1;
    }

    log_d("sockaddr_init() ok");

    return sockfd;
}

int sockaddr_init(struct sockaddr_in *addr,void *host,int port,int family)
{
    if(host == NULL || port <= 0) {
        return -1;
    }

    if(addr == NULL) { //
        struct sockaddr_in seraddr;
        memset(&seraddr, 0, sizeof(seraddr));
        seraddr.sin_family = family;
        seraddr.sin_port = htons(port);
        if(sock_inet_pton(family,(char*)host, &seraddr.sin_addr) < 0) {
            log_e("inet_aton() fail[%d].",errno);
            return -1;
        }

        log_d("sock_inet_pton() : addr = %x",seraddr.sin_addr.s_addr);

//        struct hostent *he = gethostbyname((char*)host);
//        if (he == NULL){
//            log_e("gethostbyname() fail[%d].",errno);
//            return -1;
//        } else {
//            memcpy(&seraddr.sin_addr, he->h_addr, sizeof(struct in_addr));
//            log_d("gethostbyname() : addr = %x",seraddr.sin_addr.s_addr);
//        }
    } else {
        memset(addr, 0, sizeof(struct sockaddr_in));
        addr->sin_family = family;
        addr->sin_port = htons(port);
        if(sock_inet_pton(family,(char*)host, &addr->sin_addr) < 0) {
            log_e("inet_aton() fail[%d].",errno);
            return -1;
        }

        log_d("sock_inet_pton() : addr = %x",addr->sin_addr.s_addr);

//        struct hostent *he = gethostbyname((char*)host);
//        if (he == NULL){
//            log_e("gethostbyname() fail[%d].",errno);
//            return -1;
//        } else {
//            memcpy(&addr->sin_addr, he->h_addr, sizeof(struct in_addr));
//            log_d("gethostbyname() : addr = %x",addr->sin_addr.s_addr);
//        }
    }

    return 0;
}


ssize_t
sock_recvfrom(int fd, void *buf, size_t len, int flags,
                struct sockaddr *addr, socklen_t *addr_len)
{
    return recvfrom(fd,buf,len,flags,addr,addr_len);
}

ssize_t sock_sendto(int fd, void *buf, size_t len,
               int flags, struct sockaddr *addr,
               socklen_t addr_len)
{
    return sendto(fd,buf,len,flags,addr,addr_len);
}

static int    read_cnt;
static char    *read_ptr;
static char    read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{

    if (read_cnt <= 0) {
again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR){
                goto again;
            }
            return(-1);
        } else if (read_cnt == 0)
            return(0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return(1);
}

ssize_t
sock_readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t    n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;    /* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return(n - 1);    /* EOF, n - 1 bytes were read */
        } else
            return(-1);        /* error, errno set by read() */
    }

    *ptr = 0;    /* null terminate like fgets() */
    return(n);
}

ssize_t
sock_readlinebuf(void **vptrptr)
{
    if (read_cnt)
        *vptrptr = read_ptr;
    return(read_cnt);
}
/* end readline */

ssize_t                        /* Write "n" bytes to a descriptor. */
sock_writen(int fd, const void *vptr, size_t n)
{
    size_t        nleft;
    ssize_t        nwritten;
    const char    *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;        /* and call write() again */
            else
                return(-1);            /* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

/* int
 * inet_pton(af, src, dst)
 *    convert from presentation format (which usually means ASCII printable)
 *    to network format (which is usually some kind of binary format).
 * return:
 *    1 if the address was valid for the specified address family
 *    0 if the address wasn't valid (`dst' is untouched in this case)
 *    -1 if some other error occurred (`dst' is untouched in this case, too)
 * author:
 *    Paul Vixie, 1996.
 */
int
sock_inet_pton(af, src, dst)
    int af;
    const char *src;
    void *dst;
{
    switch (af) {
    case AF_INET:
        return (inet_pton4(src, dst));
    case AF_INET6:
        return (inet_pton6(src, dst));
    default:
        errno = EAFNOSUPPORT;
        return (-1);
    }
    /* NOTREACHED */
}

/* int
 * inet_pton4(src, dst)
 *    like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *    1 if `src' is a valid dotted quad, else 0.
 * notice:
 *    does not touch `dst' unless it's returning 1.
 * author:
 *    Paul Vixie, 1996.
 */
static int
inet_pton4(src, dst)
    const char *src;
    u_char *dst;
{
    static const char digits[] = "0123456789";
    int saw_digit, octets, ch;
    u_char tmp[INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr(digits, ch)) != NULL) {
            u_int new = *tp * 10 + (pch - digits);

            if (new > 255)
                return (0);
            *tp = new;
            if (! saw_digit) {
                if (++octets > 4)
                    return (0);
                saw_digit = 1;
            }
        } else if (ch == '.' && saw_digit) {
            if (octets == 4)
                return (0);
            *++tp = 0;
            saw_digit = 0;
        } else
            return (0);
    }
    if (octets < 4)
        return (0);
    /* bcopy(tmp, dst, INADDRSZ); */
    memcpy(dst, tmp, INADDRSZ);
    return (1);
}

/* int
 * inet_pton6(src, dst)
 *    convert presentation level address to network order binary form.
 * return:
 *    1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *    (1) does not touch `dst' unless it's returning 1.
 *    (2) :: in a full address is silently ignored.
 * credit:
 *    inspired by Mark Andrews.
 * author:
 *    Paul Vixie, 1996.
 */
static int
inet_pton6(src, dst)
    const char *src;
    u_char *dst;
{
    static const char xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    u_char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int ch, saw_xdigit;
    u_int val;

    memset((tp = tmp), 0, IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':')
        if (*++src != ':')
            return (0);
    curtok = src;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (0);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!saw_xdigit) {
                if (colonp)
                    return (0);
                colonp = tp;
                continue;
            }
            if (tp + INT16SZ > endp)
                return (0);
            *tp++ = (u_char) (val >> 8) & 0xff;
            *tp++ = (u_char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
            inet_pton4(curtok, tp) > 0) {
            tp += INADDRSZ;
            saw_xdigit = 0;
            break;    /* '\0' was seen by inet_pton4(). */
        }
        return (0);
    }
    if (saw_xdigit) {
        if (tp + INT16SZ > endp)
            return (0);
        *tp++ = (u_char) (val >> 8) & 0xff;
        *tp++ = (u_char) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (0);
    /* bcopy(tmp, dst, IN6ADDRSZ); */
    memcpy(dst, tmp, IN6ADDRSZ);
    return (1);
}


