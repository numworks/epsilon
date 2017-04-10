#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <private/memconfig.h>

#if LIBA_LOG_DYNAMIC_MEMORY
#include <ion/log.h>
#endif

extern char _heap_start;
extern char _heap_end;

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
  HeapConfig.nHeap = (&_heap_end - &_heap_start);
  HeapConfig.pHeap = &_heap_start;
  HeapConfig.mnReq = 1;
  HeapConfig.bMemstat = 0;
  HeapConfig.xLog = 0;
  memsys5Init(0);
}

void free(void *ptr) {
#if LIBA_LOG_DYNAMIC_MEMORY
  ion_log_print_string("FREE-");
  ion_log_print_integer((uint32_t)ptr);
  ion_log_print_string("\n");
#endif
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
#if LIBA_LOG_DYNAMIC_MEMORY
  ion_log_print_string("MALLOC-");
  ion_log_print_integer((uint32_t)p);
  ion_log_print_string("-");
  ion_log_print_integer((uint32_t)size);
  ion_log_print_string("\n");
#endif
  /* If the process could not find enough space in the memory dedicated to
   * dynamic allocation, p is NULL. The conventional malloc just return NULL
   * without crashing. However, for debuging purposes, we abort the process
   * in that case to easily spot memory leaking. */
  assert(p != NULL);
  return p;
}

void * realloc(void *ptr, size_t size) {
  return memsys5Realloc(ptr, memsys5Roundup(size));
}
