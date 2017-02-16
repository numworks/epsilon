#include <stdlib.h>
#include <string.h>
#include <private/memconfig.h>

#if LIBA_LOG_DYNAMIC_MEMORY
#include <stdint.h>
#include <ion/c.h>

void write_uint32_in_buffer(uint32_t n, char * buffer) {
  for (int i=0; i<2*sizeof(uint32_t); i++) {
    unsigned char v = (n & 0xF);
    char c = (v>9) ? 'A'+v-10 : '0'+v;
    buffer[2*sizeof(uint32_t)-1-i] = c;
    n = n >> 4;
  }
}
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
  char message[5+2*sizeof(uint32_t)+1] = {'F', 'R', 'E', 'E', '-'};
  write_uint32_in_buffer((uint32_t)ptr, message+5);
  ion_log_print(message);
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
  char message[7+2*sizeof(uint32_t)+1+2*sizeof(uint32_t)+1] = {'M','A','L','L','O','C','-'};
  write_uint32_in_buffer((uint32_t)p, message+7);
  message[7+2*sizeof(uint32_t)] = '-';
  write_uint32_in_buffer(size, message+7+2*sizeof(uint32_t)+1);
  message[7+2*sizeof(uint32_t)+1+2*sizeof(uint32_t)] = 0;
  ion_log_print(message);
#endif
  return p;
}

void * realloc(void *ptr, size_t size) {
  return memsys5Realloc(ptr, memsys5Roundup(size));
}
