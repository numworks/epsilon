#include "helpers.h"
#include <poincare/tree_pool.h>
#include <quiz.h>

int pool_size() {
  return Poincare::TreePool::sharedPool->numberOfNodes();
}

void assert_pool_size(int i) {
#if POINCARE_TREE_LOG
  int poolSize = pool_size();
  if (poolSize != i) {
    std::cout << "Expected pool of size " << i << " but got " << poolSize << std::endl;
    log_pool();
    quiz_assert(false);
  }
#else
  quiz_assert(pool_size() == i);
#endif
}

#if POINCARE_TREE_LOG
void log_pool() {
  Poincare::TreePool::sharedPool->treeLog(std::cout);
}
void log_pool_tree() {
  Poincare::TreePool::sharedPool->treeLog(std::cout);
}
#endif
