#include <stdlib.h>
#include <string.h>

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
