#ifndef LIBA_MEMCONFIG_H
#define LIBA_MEMCONFIG_H

/* This structure contains all the configuration data for the mem5 allocator.
 * Before using mem5, the HeapConfig global variable should be created
 * and initialized. */

typedef struct {
  int nHeap;                               /* Size of pHeap[] */
  void *pHeap;                             /* Heap storage space */
  int mnReq;                               /* Min heap requests sizes */
  int bMemstat;                            /* True to enable memory status */
  void (*xLog)(void *, int, const char *); /* Function for logging */
} heap_config_t;

extern heap_config_t HeapConfig;

#endif
