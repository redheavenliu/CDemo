//#include "m_stdio.h"
#include "m_util.h"
#include <string.h>

/**
 * Get OS is big or little endian.
 *
 * Return
 *  1  :  Big-endian
 *  0  :  Little-endian
 *  -1 :  Unknown
 */
int util_big_endian()
{
    union
    {
        short s;
        char c[sizeof(short)];
    } un;
    un.s = 0x0102;
    if (sizeof(short) == 2) {
        if (un.c[0] == 1 && un.c[1] == 2)
            return 1;
        else if (un.c[0] == 2 && un.c[1] == 1)
            return 0;
        else
            return -1;
    } else
        return -1;
}

