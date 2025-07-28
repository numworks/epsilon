#ifndef LIBA_BRIDGE_ALLOCA_H
#define LIBA_BRIDGE_ALLOCA_H

#if __MINGW32__
#include <malloc.h>
#else
#include_next <alloca.h>
#endif

#endif
