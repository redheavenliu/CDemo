#ifndef M_STDIO_H
#define M_STDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <errno.h>

#include "../config"

#include "m_type.h"
#include "m_string.h"
#include "m_log.h"
#include "m_error.h"

#define BUFFER_SIZE 2048

#ifndef INFTIM
#define INFTIM          (-1)    /* infinite poll timeout */
#endif

#endif /* M_STDIO_H */
