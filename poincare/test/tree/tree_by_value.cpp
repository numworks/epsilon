#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#include "helpers.h"

using namespace Poincare;

QUIZ_CASE(tree_by_value_are_discared_after_block) {
  int initialPoolSize = pool_size();
  {
    BlobByValue b(0);
    assert_pool_size(initialPoolSize+1);
  }
  assert_pool_size(initialPoolSize);
}

static void make_temp_blob() {
  BlobByValue b(5);
}
QUIZ_CASE(tree_by_value_are_discared_after_function_call) {
  int initialPoolSize = pool_size();
  make_temp_blob();
  assert_pool_size(initialPoolSize);
}

QUIZ_CASE(tree_by_value_can_be_copied) {
  int initialPoolSize = pool_size();
  BlobByValue b(123);
  assert_pool_size(initialPoolSize+1);
  TreeByValue t = b;
  assert_pool_size(initialPoolSize + 2);
}

static TreeByValue blob_with_data_3() {
  return BlobByValue(3);
}
QUIZ_CASE(tree_by_value_can_be_returned) {
  int initialPoolSize = pool_size();
  TreeByValue b = blob_with_data_3();
  assert_pool_size(initialPoolSize+1);
}

QUIZ_CASE(tree_by_value_allocation_failures) {
  int initialPoolSize = pool_size();
  BlobByValue b(1);
  assert_pool_size(initialPoolSize+1);
  {
    BlobByValue array[500];
    assert(pool_size() > 1);
    assert(pool_size() < 500);
  }
  assert_pool_size(initialPoolSize+1);
}

QUIZ_CASE(tree_by_value_deep_copies) {
  int initialPoolSize = pool_size();
  BlobByValue b1(1);
  BlobByValue b2(2);
  assert_pool_size(initialPoolSize+2);
  PairByValue p(b1, b2);
  assert_pool_size(initialPoolSize+5);
  PairByValue p2 = p;
  assert_pool_size(initialPoolSize+8);
}
