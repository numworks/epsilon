#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#include "blob_node.h"
#include "pair_node.h"

#if POINCARE_TREE_LOG
#include <iostream>
#endif

using namespace Poincare;

static inline int pool_size() {
  return TreePool::sharedPool()->numberOfNodes();
}
#if POINCARE_TREE_LOG
static inline void log_pool() {
  TreePool::sharedPool()->flatLog(std::cout);
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

QUIZ_CASE(tree_by_values_are_stored_in_pool) {
  assert_pool_size(0);
  {
    BlobByValue b(0);
    assert_pool_size(1);
  }
  assert_pool_size(0);
}

TreeByValue blob_with_data_3() {
  return BlobByValue(3);
}
QUIZ_CASE(tree_by_values_can_be_returned) {
  assert_pool_size(0);
  TreeByValue b = blob_with_data_3();
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_values_allocation_failures) {
  assert_pool_size(0);
  BlobByValue b(1);
  assert_pool_size(1);
  {
    BlobByValue array[100];
    assert(pool_size() > 1);
    assert(pool_size() < 100);
  }
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_value_deep_copies) {
  assert_pool_size(0);
  BlobByValue b1(1);
  BlobByValue b2(2);
  PairByValue p(b1, b2);
  assert_pool_size(3);
  PairByValue p2 = p;
  assert_pool_size(6);
}
