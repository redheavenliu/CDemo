#ifndef M_ERROR_H
#define M_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <stdarg.h>		/* ANSI C header file */
#include <syslog.h>		/* for syslog() */

#include <sys/stat.h>

void
err_ret(const char *fmt, ...);

/* Fatal error related to system call
 * Print message and terminate */

void
err_sys(const char *fmt, ...);

/* Fatal error related to system call
 * Print message, dump core, and terminate */

void
err_dump(const char *fmt, ...);

/* Nonfatal error unrelated to system call
 * Print message and return */

void
err_msg(const char *fmt, ...);

/* Fatal error unrelated to system call
 * Print message and terminate */

void
err_quit(const char *fmt, ...);

#endif /* M_ERROR_H */