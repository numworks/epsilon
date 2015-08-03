#include <stdlib.h>
#include <string.h>
#include <private/memconfig.h>

// Memsys headers cannot be included easily so we rewrite them here
void memsys5FreeUnsafe(void *pOld);
void * memsys5MallocUnsafe(int nByte);
void * memsys5Realloc(void *pPrior, int nBytes);
int memsys5Roundup(int n);

void free(void *ptr) {
  if (ptr != NULL) {
    memsys5FreeUnsafe(ptr);
  }
}

void * malloc(size_t size) {
  void * p = NULL;
  if (size > 0) {
    p = memsys5MallocUnsafe(memsys5Roundup(size));
  }
  return p;
}

void * realloc(void *ptr, size_t size) {
  return memsys5Realloc(ptr, memsys5Roundup(size));
}
