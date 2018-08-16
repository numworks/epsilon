#include <quiz.h>
#include <poincare.h>
#include <assert.h>

#include "helpers.h"

using namespace Poincare;

QUIZ_CASE(tree_by_value_are_discared_after_block) {
  assert_pool_size(0);
  {
    BlobByValue b(0);
    assert_pool_size(1);
  }
  assert_pool_size(0);
}

static void make_temp_blob() {
  BlobByValue b(5);
}
QUIZ_CASE(tree_by_value_are_discared_after_function_call) {
  assert_pool_size(0);
  make_temp_blob();
  assert_pool_size(0);
}

QUIZ_CASE(tree_by_value_can_be_copied) {
  assert_pool_size(0);
  BlobByValue b(123);
  assert_pool_size(1);
  TreeByValue t = b;
  assert_pool_size(2);
}

static TreeByValue blob_with_data_3() {
  return BlobByValue(3);
}
QUIZ_CASE(tree_by_value_can_be_returned) {
  assert_pool_size(0);
  TreeByValue b = blob_with_data_3();
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_value_allocation_failures) {
  assert_pool_size(0);
  BlobByValue b(1);
  assert_pool_size(1);
  {
    BlobByValue array[500];
    assert(pool_size() > 1);
    assert(pool_size() < 500);
  }
  assert_pool_size(1);
}

QUIZ_CASE(tree_by_value_deep_copies) {
  assert_pool_size(0);
  BlobByValue b1(1);
  BlobByValue b2(2);
  assert_pool_size(2);
  PairByValue p(b1, b2);
  assert_pool_size(5);
  PairByValue p2 = p;
  assert_pool_size(8);
}
