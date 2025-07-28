#ifndef LIBA_ERRNO_H
#define LIBA_ERRNO_H

/* No such file or directory */
#define ENOENT 2

/*Interrupted function call*/
#define EINTR 4

/* EIO Input/output error */
#define EIO 5

/* No such device or address */
#define ENXIO 6

/* Not enough space */
#define ENOMEM 12

/* Invalid argument */
#define EINVAL 22

/* File too large */
#define EFBIG 27

/* Result too large */
#define ERANGE 34

/* Resource temporarily unavailable */
#define EAGAIN 35

extern int errno;

#endif
