#include <string.h>

#if (__GLIBC__ || __MINGW32__)
#include "strlcat.c"
#include "strlcpy.c"
#endif
