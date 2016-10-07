#ifndef LIBA_OPENBSD_SYS_TYPES_H
#define LIBA_OPENBSD_SYS_TYPES_H

#include <stdint.h>

#define LITTLE_ENDIAN  0x1234
#define BIG_ENDIAN     0x4321
#define BYTE_ORDER LITTLE_ENDIAN

typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

#endif
