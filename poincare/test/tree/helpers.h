#include "blob_node.h"
#include "pair_node.h"

#if POINCARE_TREE_LOG
#include <iostream>
#endif

static inline int pool_size() {
  return Poincare::TreePool::sharedPool()->numberOfNodes();
}
#if POINCARE_TREE_LOG
static inline void log_pool() {
  Poincare::TreePool::sharedPool()->flatLog(std::cout);
}
#endif

static void assert_pool_size(int i) {
#if POINCARE_TREE_LOG
  int poolSize = pool_size();
  if (poolSize != i) {
    std::cout << "Expected pool of size " << i << " but got " << poolSize << std::endl;
    log_pool();
    assert(false);
  }
#else
  assert(pool_size() == i);
#endif
}
