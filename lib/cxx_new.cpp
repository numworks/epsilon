/*#ifndef LIB_MALLOC_H
#define LIB_MALLOC_H*/

extern "C" {
#include <malloc.h>
}

void * operator new (unsigned int size) {
  return malloc(size);
}

/*#endif*/
