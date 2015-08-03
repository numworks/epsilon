#include "init_lcd.h"
#include <private/memconfig.h>

extern char _ccm_heap_start;
extern char _ccm_heap_end;

heap_config_t HeapConfig;

int memsys5Init(void *NotUsed);

void init_heap() {
  HeapConfig.nHeap = (&_ccm_heap_end - &_ccm_heap_start);
  HeapConfig.pHeap = &_ccm_heap_start;
  HeapConfig.mnReq = 1;
  HeapConfig.bMemstat = 0;
  HeapConfig.xLog = 0;

  memsys5Init(0);
}
