#ifndef M_UTIL_H
#define M_UTIL_H

#include "poll.h"

/**
 * Get OS is big or little endian.
 *
 * Return
 *  1  :  Big-endian
 *  0  :  Little-endian
 *  -1 :  Unknown
 */
int util_big_endian();


#endif /* M_UTIL_H */

