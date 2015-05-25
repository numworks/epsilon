#include <stddef.h>

void free(void *ptr) {
  if (ptr != NULL) {
    memsys5FreeUnsafe(ptr);
  }
}

void * malloc(int size) {
  void * p = NULL;
  if (size > 0) {
    p = memsys5MallocUnsafe(memsys5Roundup(size));
  }
  return p;
}

void * realloc(void *ptr, int size) {
  return memsys5Realloc(ptr, memsys5Roundup(size));
}
