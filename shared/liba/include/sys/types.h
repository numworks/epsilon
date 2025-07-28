#ifndef LIBA_SYS_TYPES_H
#define LIBA_SYS_TYPES_H

// python ulab requirements

// openbsd requirements

#include <stdint.h>

#define LITTLE_ENDIAN 0x1234
#define BIG_ENDIAN 0x4321
#define BYTE_ORDER LITTLE_ENDIAN

typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;

// libsodium requirements

typedef int dev_t;
typedef int ino_t;
typedef int mode_t;
typedef int nlink_t;
typedef int uid_t;
typedef int gid_t;
typedef int off_t;
typedef int blksize_t;
typedef int blkcnt_t;
typedef int time_t;

#endif
