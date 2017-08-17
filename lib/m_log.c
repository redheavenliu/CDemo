/*
 * m_log.c
 *
 *  Created on: Mar 22, 2017
 *      Author: lb
 */
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>      /* ANSI C header file */
#include <syslog.h>      /* for syslog() */
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <sys/time.h>

#include "m_log.h"

#define ERRNO_SIZE 133
#define LOG_BUF_SIZE 2048
#define TRUE 1
#define FALSE 0

static char* errno_str[] = {
        "0 : Success",
        "PERM 1 : Operation not permitted",
        "ENOENT 2 : No such file or directory",
        "ESRCH 3 : No such process",
        "EINTR 4 : Interrupted system call",
        "EIO 5 : I/O error",
        "ENXIO 6 : No such device or address",
        "E2BIG 7 : Argument list too long",
        "ENOEXEC 8 : Exec format error",
        "EBADF 9 : Bad file number",
        "ECHILD 10 : No child processes",
        "EAGAIN 11 : Try again",
        "ENOMEM 12 : Out of memory",
        "EACCES 13 : Permission denied",
        "EFAULT 14 : Bad address",
        "ENOTBLK 15 : Block device required",
        "EBUSY 16 : Device or resource busy",
        "EEXIST 17 : File exists",
        "EXDEV 18 : Cross-device link",
        "ENODEV 19 : No such device",
        "ENOTDIR 20 : Not a directory",
        "EISDIR 21 : Is a directory",
        "EINVAL 22 : Invalid argument",
        "ENFILE 23 : File table overflow",
        "EMFILE 24 : Too many open files",
        "ENOTTY 25 : Not a typewriter",
        "ETXTBSY 26 : Text file busy",
        "EFBIG 27 : File too large",
        "ENOSPC 28 : No space left on device",
        "ESPIPE 29 : Illegal seek",
        "EROFS 30 : Read-only file system",
        "EMLINK 31 : Too many links",
        "EPIPE 32 : Broken pipe",
        "EDOM 33 : Math argument out of domain of func",
        "ERANGE 34 : Math result not representable",
        "EDEADLK 35 : Resource deadlock would occur",
        "ENAMETOOLONG 36 : File name too long",
        "ENOLCK 37 : No record locks available",
        "ENOSYS 38 : Function not implemented",
        "ENOTEMPTY 39 : Directory not empty",
        "ELOOP 40 : Too many symbolic links encountered",
        "EWOULDBLOCK EAGAIN : Operation would block",
        "ENOMSG 42 : No message of desired type",
        "EIDRM 43 : Identifier removed",
        "ECHRNG 44 : Channel number out of range",
        "EL2NSYNC 45 : Level 2 not synchronized",
        "EL3HLT 46 : Level 3 halted",
        "EL3RST 47 : Level 3 reset",
        "ELNRNG 48 : Link number out of range",
        "EUNATCH 49 : Protocol driver not attached",
        "ENOCSI 50 : No CSI structure available",
        "EL2HLT 51 : Level 2 halted",
        "EBADE 52 : Invalid exchange",
        "EBADR 53 : Invalid request descriptor",
        "EXFULL 54 : Exchange full",
        "ENOANO 55 : No anode",
        "EBADRQC 56 : Invalid request code",
        "EBADSLT 57 : Invalid slot",
        "EDEADLOCK 58 : Resource deadlock would occur",
        "EBFONT 59 : Bad font file format",
        "ENOSTR 60 : Device not a stream",
        "ENODATA 61 : No data available",
        "ETIME 62 : Timer expired",
        "ENOSR 63 : Out of streams resources",
        "ENONET 64 : Machine is not on the network",
        "ENOPKG 65 : Package not installed",
        "EREMOTE 66 : Object is remote",
        "ENOLINK 67 : Link has been severed",
        "EADV 68 : Advertise error",
        "ESRMNT 69 : Srmount error",
        "ECOMM 70 : Communication error on send",
        "EPROTO 71 : Protocol error",
        "EMULTIHOP 72 : Multihop attempted",
        "EDOTDOT 73 : RFS specific error",
        "EBADMSG 74 : Not a data message",
        "EOVERFLOW 75 : Value too large for defined data type",
        "ENOTUNIQ 76 : Name not unique on network",
        "EBADFD 77 : File descriptor in bad state",
        "EREMCHG 78 : Remote address changed",
        "ELIBACC 79 : Can not access a needed shared library",
        "ELIBBAD 80 : Accessing a corrupted shared library",
        "ELIBSCN 81 : .lib section in a.out corrupted",
        "ELIBMAX 82 : Attempting to link in too many shared libraries",
        "ELIBEXEC 83 : Cannot exec a shared library directly",
        "EILSEQ 84 : Illegal byte sequence",
        "ERESTART 85 : Interrupted system call should be restarted",
        "ESTRPIPE 86 : Streams pipe error",
        "EUSERS 87 : Too many users",
        "ENOTSOCK 88 : Socket operation on non-socket",
        "EDESTADDRREQ 89 : Destination address required",
        "EMSGSIZE 90 : Message too long",
        "EPROTOTYPE 91 : Protocol wrong type for socket",
        "ENOPROTOOPT 92 : Protocol not available",
        "EPROTONOSUPPORT 93 : Protocol not supported",
        "ESOCKTNOSUPPORT 94 : Socket type not supported",
        "EOPNOTSUPP 95 : Operation not supported on transport endpoint",
        "EPFNOSUPPORT 96 : Protocol family not supported",
        "EAFNOSUPPORT 97 : Address family not supported by protocol",
        "EADDRINUSE 98 : Address already in use",
        "EADDRNOTAVAIL 99 : Cannot assign requested address",
        "ENETDOWN 100 : Network is down",
        "ENETUNREACH 101 : Network is unreachable",
        "ENETRESET 102 : Network dropped connection because of reset",
        "ECONNABORTED 103 : Software caused connection abort",
        "ECONNRESET 104 : Connection reset by peer",
        "ENOBUFS 105 : No buffer space available",
        "EISCONN 106 : Transport endpoint is already connected",
        "ENOTCONN 107 : Transport endpoint is not connected",
        "ESHUTDOWN 108 : Cannot send after transport endpoint shutdown",
        "ETOOMANYREFS 109 : Too many references: cannot splice",
        "ETIMEDOUT 110 : Connection timed out",
        "ECONNREFUSED 111 : Connection refused",
        "EHOSTDOWN 112 : Host is down",
        "EHOSTUNREACH 113 : No route to host",
        "EALREADY 114 : Operation already in progress",
        "EINPROGRESS 115 : Operation now in progress",
        "ESTALE 116 : Stale NFS file handle",
        "EUCLEAN 117 : Structure needs cleaning",
        "ENOTNAM 118 : Not a XENIX named type file",
        "ENAVAIL 119 : No XENIX semaphores available",
        "EISNAM 120 : Is a named type file",
        "121 : Unknown",
        "122 : Unknown",
        "123 : Unknown",
        "124 : Unknown",
        "125 : Unknown",
        "ENOKEY 126 : Required key not available",
        "EKEYEXPIRED 127 : Key has expired",
        "EKEYREVOKED 128 : Key has been revoked",
        "EKEYREJECTED 129 : Key was rejected by service",
        "EOWNERDEAD 130 : Owner died",
        "ENOTRECOVERABLE 131 : State not recoverable",
        "ERFKILL 132 : Operation not possible due to RF-kill",
        "EHWPOISON 133 : Memory page has hardware error"
};


static char log_buf[LOG_BUF_SIZE + 1];
static uint8 log_flag = 0;
static bool log_inited = FALSE;
static FILE *log_file = NULL;
static char log_file_path[100] = {0};
static log_level_enum log_level = LOG_NONE;

static bool
log_time_get
(
    char *buf,
    size_t len
)
{
    bzero(buf,len);
    struct timespec time;
    // Get time fail.
    if(clock_gettime(CLOCK_REALTIME,&time)){
        return FALSE;
    }

    time_t sec = (time_t)time.tv_sec;
    struct tm *tmp = gmtime(&sec);
    // 2017-06-06 05:10:57
    //size_t size = strftime(buf,len,"%F %T",tmp);
    size_t size = strftime(buf,len,"%m-%d %T",tmp);
    if(size > 0){
        snprintf(buf + size,len - size,
            ":%ld",
            time.tv_nsec / 1000000); // ms
        return TRUE;
    }else{// Fail
        bzero(buf,len);
        return FALSE;
    }
}

/**
 * Print error string.when error < 0,print current errno.
 */
void log_errno(const int error)
{
    static char mesg[100] = { '\0' };

    if (error >= 0 && error <= sys_nerr) {
        snprintf(mesg, sizeof(mesg), "%s : (%d)", sys_errlist[error], error);
    } else {
        if (errno) {
            if(errno > ERRNO_SIZE || errno < 0){
                snprintf(mesg, sizeof(mesg), "errno : Unknown (%d)", errno);
            }else {
                snprintf(mesg, sizeof(mesg), "errno : %s", errno_str[errno]);
            }
        } else {
            snprintf(mesg, sizeof(mesg), "Unknown error (%d)", error);
        }
    }
    printf("%s\n", mesg);
}

void log_print(const char* str)
{
    char buf[100 + 1] = {0};
    char time_buf[90 + 1] = {0};
    log_time_get(time_buf, 90);
    if(!strlen(time_buf)){
        return;
    }

    if(log_file){// Write log to file
        snprintf(buf,100,"[%d][%s]-",log_level,time_buf);
        fwrite(buf,sizeof(char),strlen(buf),log_file);
        fwrite(str,sizeof(char),strlen(str),log_file);
    }else{// Write log to terminal
        snprintf(buf,100,"[%d][%s]-",log_level,time_buf);
        fflush(stdout);		/* in case stdout and stderr are the same */
        if(strlen(buf) > 0){
            fputs(buf,stderr);
        }
        fputs(str, stderr);
        fputs(NONE,stderr);
        fflush(stderr);
    }
}

/*
* Set the log level which should print.
*
* Parameters:
*   level               The log level.
*   only_this_level     TRUE if only want print this level,FALSE if want print
*                           all log which level <= [level]
*   log_path            The path of log file.NULL if disable log file.
*
*/
void log_init
(
    int level,
    bool only_this_level,
    const char *log_path
)
{
    if(level > LOG_D){
        level = LOG_D;
    }

    // Clear all level.
    if(level < LOG_F){
        log_flag = 0;
        return;
    }

    log_flag = 0;

    // Enable log file.
    if(log_path){
        // Change log file.
        if(strlen(log_file_path) > 0
            && log_file){
            fclose(log_file);
            log_file = NULL;
            bzero(log_file_path,100);
        }

        log_file = fopen(log_path,"a+");
        // Set I/O to no-buffer.
        if(log_file){
            memcpy(log_file_path,log_path,strlen(log_path));
            log_file_path[strlen(log_file_path)] = '\0';
            setvbuf(log_file,NULL,_IONBF,0);
        }
    }else{// Enable
        // Close log file.
        if(strlen(log_file_path) > 0
            && log_file){
            fclose(log_file);
            log_file = NULL;
            bzero(log_file_path,100);
        }
    }

    if(only_this_level){// Only one level
        SET_BIT8(log_flag, level);
    }else{// set many level
        int i;
        for(i = 1;i<=level;i++){
            SET_BIT8(log_flag,i);
        }
    }
    log_inited = TRUE;

    log_section();

}

/*
* Call log() to replace this.
*/
void
log_private
(
    int level,
    const char *file,
    int line,
    const char *func,
    char *fmt,...
)
{
    // Default level is <= [LOG_I]
    if(!log_inited){
        log_init(LOG_I,FALSE,NULL);
    }

    va_list		ap;
    int		n;
    bzero(log_buf,LOG_BUF_SIZE + 1);
    char *color = NONE;
    switch(level)
    {
        case LOG_F:
            color = RED;
            break;
        case LOG_A:
            color = RED;
            break;
        case LOG_E:
            color = RED;
            break;
        case LOG_W:
            color = BLUE;
            break;
        case LOG_I:
            color = GREEN;
            break;
        case LOG_D:
            color = NONE;
            break;
    }

    if(!BIT_ON8(log_flag, level)){
        return;
    }

    log_level = level;

    if(log_file){
        snprintf(log_buf, LOG_BUF_SIZE, "[%s,%d,%s()]:", file,line,func);
    }else{
        snprintf(log_buf, LOG_BUF_SIZE, "[%s,%d,%s()]:%s", file,line,func,color);
    }

    n = strlen(log_buf);

    va_start(ap, fmt);
#ifdef	HAVE_VSNPRINTF
    vsnprintf(log_buf + n, LOG_BUF_SIZE - n, fmt, ap);	/* safe */
#else
    vsprintf(log_buf + n, fmt, ap);					/* not safe */
#endif
    strcat(log_buf + strlen(log_buf), "\n");

    log_print(log_buf);

    va_end(ap);
}

void log_section(void)
{
    char buf[500 + 1] = {0};
    char time_buf[90 + 1] = {0};
    log_time_get(time_buf, 90);
    if(!strlen(time_buf)){
        return;
    }

    snprintf(buf,500,
        "\n====================== %s ======================\n\n",
        time_buf);

    if(log_file){// Write log to file
        fwrite(buf,sizeof(char),strlen(buf),log_file);
    }else{// Write log to terminal
        fflush(stdout);		/* in case stdout and stderr are the same */
        if(strlen(buf) > 0){
            fputs(YELLOW,stderr);
            fputs(buf,stderr);
            fputs(NONE,stderr);
        }
        fflush(stderr);
    }
}

void log_clear(void)
{
    if(log_file){// Write log to file
        //fclose(log_file);
        log_file = freopen(log_file_path,"w",log_file);
    }else{// Write log to terminal
        char *tmp = "\n\n\n\n\n\n\n\n\n\n";
        fflush(stdout);		/* in case stdout and stderr are the same */
        fputs(tmp,stderr);
        fflush(stderr);
    }
}

