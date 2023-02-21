#ifndef LIBA_BRIDGE_STRING_H
#define LIBA_BRIDGE_STRING_H

#include_next <string.h>

#include "../private/macros.h"

LIBA_BEGIN_DECLS

#if (__GLIBC__ || __MINGW32__)
size_t strlcat(char* dst, const char* src, size_t dstSize);
size_t strlcpy(char* dst, const char* src, size_t len);
#endif

LIBA_END_DECLS

#endif
