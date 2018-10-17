#ifndef M_TYPE_H
#define M_TYPE_H

#include <stdbool.h>

#include "../config"

#define sint15 int

/**
 * Compiler-digit : 16
 * char : 1     (%c)
 * char* : 2
 * short int : 2
 * int : 2      (%d)
 * unsigned int : 2  (%u)
 * float : 4    (%f)
 * double : 8   (%f)
 * long : 4
 * unsigned long : 4
 * long long : 8
 * unsigned long long : 8
 *
 *
 * Compiler-digit : 32
 * char : 1
 * char* : 4
 * short int : 2
 * int : 4
 * unsigned int : 4
 * float : 4
 * double : 8
 * long : 4
 * unsigned long : 4
 * long long : 8
 * unsigned long long : 8
 *
 *
 * Compiler-digit : 64
 * char : 1
 * char* : 8
 * short int : 2
 * int : 4
 * unsigned int : 4
 * float : 4
 * double : 8
 * long : 8
 * unsigned long : 8
 * long long : 8
 * unsigned long long : 8
 */

#ifdef M_BOOLEAN_DEFINED
typedef unsigned char boolean; /* Boolean value type. */
#endif

#ifdef M_BOOL_DEFINED
typedef unsigned char bool; /* Boolean value type. */
#endif

#ifdef M_UINT64_DEFINED
typedef unsigned long long uint64; /* Unsigned 64 bit value */
//typedef unsigned long long uint64_t; /* Unsigned 64 bit value */
#endif

#ifdef M_UINT32_LB_DEFINED
#ifdef M_BIT_64
typedef unsigned int uint32; /* Unsigned 32 bit value */
typedef unsigned int uint32_t; /* Unsigned 32 bit value */
#else
typedef unsigned long int uint32; /* Unsigned 32 bit value */
typedef unsigned long int uint32_t;
#endif
#endif

#ifdef M_UINT16_DEFINED
typedef unsigned short uint16; /* Unsigned 16 bit value */
typedef unsigned short uint16_t;
#endif

#ifdef M_UINT8_DEFINED
typedef unsigned char uint8; /* Unsigned 8  bit value */
typedef unsigned char uint8_t;
#endif

#ifdef M_INT64_DEFINED
typedef signed long long int64; /* Signed 64 bit value */
#endif

#ifdef M_INT32_DEFINED
#ifdef M_BIT_64
typedef signed int int32; /* Signed 32 bit value */
#else
typedef signed long int int32; /* Signed 32 bit value */
#endif
#endif

#ifdef M_INT16_DEFINED
typedef signed short int16; /* Signed 16 bit value */
#endif

#ifdef M_INT8_DEFINED
typedef signed char int8; /* Signed 8  bit value */
#endif

#ifdef  M_BYTE_DEFINED
typedef unsigned char byte; /* byte type */
#endif

#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef true
#define true   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef false
#define false  0   /* Boolean false value. */
#endif


#ifndef NULL
#define NULL  0
#endif


#ifndef null
#define null  0
#endif

#ifdef M_OS_LINUX
#define FILE_SEPARATOR "/"
#else
#define FILE_SEPARATOR "\\"
#endif

#endif /* M_TYPE_H */
