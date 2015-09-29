#include <stdlib.h>
#include <string.h>
#include <private/memconfig.h>

extern char * _liba_heap_start;
extern char * _liba_heap_end;

heap_config_t HeapConfig = {
  .nHeap = 0
};

// Memsys headers cannot be included easily so we rewrite them here
int memsys5Init(void *NotUsed);
void memsys5FreeUnsafe(void *pOld);
void * memsys5MallocUnsafe(int nByte);
void * memsys5Realloc(void *pPrior, int nBytes);
int memsys5Roundup(int n);

static void configure_heap() {
  HeapConfig.nHeap = (_liba_heap_end - _liba_heap_start);
  HeapConfig.pHeap = _liba_heap_start;
  HeapConfig.mnReq = 1;
  HeapConfig.bMemstat = 0;
  HeapConfig.xLog = 0;
  memsys5Init(0);
}

void free(void *ptr) {
  if (ptr != NULL) {
    memsys5FreeUnsafe(ptr);
  }
}

void * malloc(size_t size) {
  void * p = NULL;
  if (HeapConfig.nHeap == 0) {
    configure_heap();
  }
  if (size > 0) {
    p = memsys5MallocUnsafe(memsys5Roundup(size));
  }
  return p;
}

void * realloc(void *ptr, size_t size) {
  return memsys5Realloc(ptr, memsys5Roundup(size));
}
