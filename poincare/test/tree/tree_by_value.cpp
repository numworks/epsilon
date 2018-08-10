//#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#include "blob_node.h"

using namespace Poincare;

static inline int pool_size() {
  return TreePool::sharedPool()->numberOfNodes();
}

static void assert_pool_size(int i) {
  assert(pool_size() == i);
}

void foobar() {
//QUIZ_CASE(tree_by_values_are_stored_in_pool) {
  assert_pool_size(0);
  {
    BlobByValue b(0);
    assert_pool_size(1);
  }
  assert_pool_size(0);
}

void foobaz() {
//QUIZ_CASE(tree_by_values_allocation_failures) {
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
