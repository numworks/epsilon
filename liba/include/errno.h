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

/* Invalid argument */
#define EINVAL 22

/* Not enough space */
#define ENOMEM 12

/* Resource temporarily unavailable */
#define EAGAIN 35

extern int errno;

#endif
