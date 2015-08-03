#include <liba.h>
#include <private/memconfig.h>

int memsys5Init(void *NotUsed);

heap_config_t HeapConfig;

extern char _liba_heap_start;
extern char _liba_heap_end;

static void liba_init_heap() {
  HeapConfig.nHeap = (&_liba_heap_end - &_liba_heap_start);
  HeapConfig.pHeap = &_liba_heap_start;
  HeapConfig.mnReq = 1;
  HeapConfig.bMemstat = 0;
  HeapConfig.xLog = 0;

  memsys5Init(0);
}

void liba_init() {
  liba_init_heap();
}
